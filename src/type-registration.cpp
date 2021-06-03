#include "types.hpp"
#include <unordered_set>
#include <mutex>
#include "logging.hpp"
#include "register.hpp"

namespace custom_types {
    TypeDefinitionIndex TypeRegistration::typeIdx = kTypeDefinitionIndexInvalid;

    Il2CppType* TypeRegistration::createType() {
        // Create type using typeIdx
        auto* type = new Il2CppType();
        // TODO: For field flags/parameter flags?
        // type->attrs;
        // TODO: Unclear if this is always the case
        type->byref = 0;
        // TODO: Unclear if this is always the case
        type->num_mods = 0;
        // TODO: Unclear if this is always the case
        type->pinned = 0;
        // TODO: Change this for value types and other type enums
        type->type = typeEnum();
        // This should be a unique number, assigned when each new type is created.
        type->data.klassIndex = typeIdx;
        typeIdx--;
        return type;
    }

    std::mutex loadingLock;
    std::unordered_set<TypeRegistration*> loadingTypes;

    bool hasInterfaceIn(Il2CppClass* klass, Il2CppClass* inter) {
        for (uint16_t i = 0; i < klass->interfaces_count; i++) {
            if (klass->implementedInterfaces[i] == inter) {
                return true;
            }
        }
        if (klass->parent) {
            return hasInterfaceIn(klass->parent, inter);
        }
        return false;
    }

    void setupTypeHierarchy(Il2CppClass* klass, Il2CppClass* baseClass) {
        klass->parent = baseClass;
        if (klass->parent) {
            klass->typeHierarchyDepth = baseClass->typeHierarchyDepth + 1;
        } else {
            klass->typeHierarchyDepth = 1;
        }
        klass->typeHierarchy = reinterpret_cast<Il2CppClass**>(calloc(klass->typeHierarchyDepth, sizeof(Il2CppClass*)));
        if (klass->parent) {
            klass->typeHierarchy[klass->typeHierarchyDepth - 1] = klass;
            memcpy(klass->typeHierarchy, baseClass->typeHierarchy, baseClass->typeHierarchyDepth * sizeof(Il2CppClass*));
        } else {
            klass->typeHierarchy[0] = klass;
        }
    }

    uint16_t TypeRegistration::getVtableSize() {
        static auto logger = _logger().WithContext("getVtableSize");

        int vtableSize = 0;
        if (baseType()) {
            vtableSize = baseType()->vtable_count;
            // After everything is copied, we can easily iterate over our methods, find any that are virtual
            // Add each of our interface method counts to our vtable
            // ASSUMING THEY ARE UNIQUE INTERFACES!
            for (auto& inter : interfaces()) {
                if (!hasInterfaceIn(baseType(), inter)) {
                    // If we have an interface that isn't unique, as in, it is implemented by one of our base types
                    // Then we do NOT add to our vtable size.
                    vtableSize += inter->method_count;
                }
            }
        } else {
            // We have no base type.
            // For now, we will simply throw in such a case.
            logger.critical("Cannot create a vtable without a base type!");
            SAFE_ABORT();
        }
        logger.debug("vtable size: %u", vtableSize);
        return vtableSize;
    }

    void TypeRegistration::createClass() {
        // Check to see if we have already created our class. If we have, use that.
        auto* existing = klass();
        if (existing) {
            return;
        }
        // If we need to load, add ourselves to the list of types currently loading.
        // This locks for the entire duration of the class creation.
        std::lock_guard lock(loadingLock);
        loadingTypes.insert(this);
        auto* cb = customBase();
        if (cb) {
            // If the base is already being loaded, we have a circular dependency.
            if (loadingTypes.contains(cb)) {
                _logger().critical("Circular dependency! %s::%s (base type of: %s::%s) is already being loaded!", cb->namespaze(), cb->name(), namespaze(), name());
                SAFE_ABORT();
            }
            // Attempt to load the custom base if we haven't already.
            cb->createClass();
        }
        auto vtableSize = getVtableSize();
        // Create the Il2CppClass instance that will represent this custom type.
        auto* k = static_cast<Il2CppClass*>(calloc(1, sizeof(Il2CppClass) + vtableSize * sizeof(VirtualInvokeData)));
        auto* type = createType();
        setupTypeHierarchy(k, baseType());
        // Create image from namespace
        auto img = Register::createImage(namespaze());
        k->image = img;
        // Add ourselves to our image hash table (for class_from_name)
        img->nameToClassHashTable->insert(std::make_pair(std::make_pair(namespaze(), name()), type->data.klassIndex));
        // Set name
        k->name = name();
        k->namespaze = namespaze();
        k->this_arg = *type;
        // TODO: Modify this for future usage where byval argument may not match this argument
        k->byval_arg = *type;
        // TODO: Allow for changing flags in the future (avoid copying base flags)
        k->flags = typeFlags() | baseType()->flags;
        // This is for arrays, we can safely assume we are not an array so set this to ourselves.
        k->element_class = k;
        // TODO: Determine more information from this
        k->castClass = k;
        // Pointer to self
        k->klass = k;
        // Unless the class is used in native code, this isn't necessary
        k->native_size = -1;
        // Size is set from sizeof(T)
        k->actualSize = size();
        k->instance_size = size();
        k->size_inited = 1;
        // Methods are set after processing methods
        auto intfs = interfaces();
        k->interfaces_count = intfs.size();
        // k->implementedInterfaces needs to be allocated as well
        k->implementedInterfaces = reinterpret_cast<Il2CppClass**>(calloc(intfs.size(), sizeof(Il2CppClass*)));
        for (int i = 0; i < intfs.size(); i++) {
            k->implementedInterfaces[i] = intfs[i];
        }
        // TODO: Figure out generic class (will also need to inflate it)
        k->generic_class = nullptr;
        k->genericContainerIndex = kGenericContainerIndexInvalid;
        k->genericRecursionDepth = 1;
        // Pretend that the class has already been initialized
        k->initialized = 1;
        k->initialized_and_no_error = 1;
        k->init_pending = 0;
        k->has_initialization_error = 0;

        // TypeDefinition unused, can set to nullptr
        k->typeDefinition = nullptr;
        if (type->type == Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE) {
            k->valuetype = true;
        }
        // TODO: is this valid?
        k->token = -1;
        // TODO: See if this is always the case
        k->unity_user_data = nullptr;
        // Just assume we always have references. This is mostly unused anyways.
        k->has_references = 1;
        // Cleanup and finalization
        delete type;
        klass() = k;
        Register::classes.push_back(k);
        // At the end of our creation, remove ourselves from the load
        loadingTypes.erase(this);
    }

    void TypeRegistration::populateFields() {
        // TODO: Compute size from fields, may want to call Class::LayoutFieldsLocked
        // If we do that, we should not have size_inited set to true until after that call
        // If we do not choose to do that (and instead compute our own field offsets and whatnot) then size_inited should be true.
        auto* k = klass();
        k->size_inited = 1;
        // This is always:
        // if parent: parent's alignment if not value type, else 1
        // otherwise: sizeof(Il2CppObject*)
        // The actual size of the type seems to be a bit more annoying to compute...
        k->minimumAlignment = 8;
        k->naturalAligment = 8;
        // TODO: Eventually allow creation of thread static fields?
        k->thread_static_fields_offset = -1;
        k->thread_static_fields_size = 0;
        // The count of fields on this type is actually fields + static fields
        // fields contains ALL fields, including static ones.
        auto fields = getFields();
        auto staticFields = getStaticFields();
        k->field_count = fields.size() + staticFields.size();
        if (k->field_count > 0) {
            k->fields = reinterpret_cast<FieldInfo*>(calloc(k->field_count, sizeof(FieldInfo)));
            for (uint16_t i = 0; i < fields.size(); ++i) {
                auto* f = fields[i];
                k->fields[i] = FieldInfo{
                    .name = f->name(),
                    .type = f->type(),
                    .parent = k,
                    .token = static_cast<uint32_t>(f->type()->attrs | f->fieldAttributes())
                };
            }
            for (uint16_t i = 0; i < staticFields.size(); ++i) {
                auto* f = staticFields[i];
                k->fields[i] = FieldInfo{
                    .name = f->name(),
                    .type = f->type(),
                    .parent = k,
                    .offset = f->offset(),
                    .token = static_cast<uint32_t>(f->type()->attrs | f->fieldAttributes()),
                };
            }
        }

        // Static fields are more obnoxious, since klass->static_fields is a pointer to a region of memory
        // Where each static field is laid out with their offset mapping to the correct location.
        static_fields() = new char[static_fields_size()];
        k->static_fields = static_fields();
        k->static_fields_size = static_fields_size();
    }

    bool TypeRegistration::checkVirtualsForMatch(MethodRegistrator* info, std::string_view namespaze, std::string_view name, std::string_view methodName, int paramCount) {
        // For now, we only match the highest level virtual data.
        // Any sub-level virtuals are missed/ignored.
        // This SHOULD be good enough for Finalize.
        if (info->virtualMethod()) {
            auto* k = info->virtualMethod()->klass;
            return namespaze == k->namespaze && name == k->name && methodName == info->csharpName() && (paramCount >= 0 ? info->params_size() == paramCount : true);
        }
        return false;
    }

    void TypeRegistration::getVtable(std::vector<VirtualInvokeData>& vtable, std::vector<Il2CppRuntimeInterfaceOffsetPair>& offsets) {
        static auto logger = _logger().WithContext("getVtable");
        // First, we want to iterate over our methods
        // With these methods, we want to determine if any of them have virtual data
        // If they do, we want to look in our existing vtable if they exist. If they do, remap the methodPointer
        // If they do not, create a new entry.
        // Total new vtable_count should always be >= baseT->vtable_count

        // VirtualInvokeData holds a MethodInfo* and Il2CppMethodPointer to the method DEFINITION
        // The indices of the vtable is dictated by our interface offsets
        // Interfaces are like normal types in that its MethodInfo*s have slots.
        // In order to tell il2cpp where to find our definition for a given method, we setup our interface offsets
        // We find our correct VirtualInvokeData, and thus correct MethodInfo*/Il2CppMethodPointer
        // from matching interface offset + slot of interface method.
        // Therefore, it is essential for our vtable to hold enough slots for each interface's methods.
        // And we need to properly set our offsets such that they are all contigous
        // and interface 1.offset starts slot = 0 for interface 1's slot 0 method.

        // Thus, vtable becomes a bit more annoying to compute:
        // We need to iterate over all of our interfaces, in the order we define them
        // For each interface, we need to allocate enough space for each interface method
        // We also need to store the offsets of each interface into interface_offsets
        // We also need to go through our parent's interface_offsets + vtable and copy it over
        // If we have any matching interface methods (ex: virtual_data->klass == interface, virtual_data->slot == i - interface_offset)
        // Then we overwrite it, yay!
        // Otherwise, we copy it over
        auto* baseT = baseType();
        if (baseT) {
            // Logically speaking, the easiest approach we can follow is:
            // Copy over our base vtable
            // Change single values as we see them.
            logger.debug("Copying base offsets...");
            for (uint16_t offIdx = 0; offIdx < baseT->interface_offsets_count; ++offIdx) {
                offsets.push_back(baseT->interfaceOffsets[offIdx]);
            }
            logger.debug("Copying base vtable...");
            for (uint16_t i = 0; i < baseT->vtable_count; ++i) {
                vtable.push_back(baseT->vtable[i]);
            }
            // After everything is copied, we can easily iterate over our methods, find any that are virtual
            // If we find a virtual method, see if we are overriding a base interface, or making a new one
            for (auto m : getMethods()) {
                auto* vMethod = m->virtualMethod();
                if (vMethod != nullptr) {
                    bool set = false;
                    if (vMethod->slot < 0) {
                        logger.critical("Virtual data: %p has slot: %u which is invalid!", vMethod, vMethod->slot);
                        SAFE_ABORT();
                    }
                    for (auto& inter : offsets) {
                        if (inter.interfaceType == vMethod->klass) {
                            // We found a virtual method which has an interface already listed in our interface offsets.
                            // The method's slot should be set to vMethod->slot + inter.offset
                            // We also need to overwrite explicitly that index of the vtable
                            m->get()->slot = vMethod->slot + inter.offset;
                            if (m->get()->slot >= vtable.size() || m->get()->slot < 0) {
                                // Our corrected slot is invalid!
                                logger.critical("Existing interface slot: %u invalid! vtable size: %u", m->get()->slot, vtable.size());
                                SAFE_ABORT();
                            }
                            logger.debug("Using slot: %u", m->get()->slot);
                            vtable[m->get()->slot].method = m->get();
                            vtable[m->get()->slot].methodPtr = m->methodPointer();
                            set = true;
                            break;
                        }
                    }
                    if (!set) {
                        // If we haven't found a matching interface type for this method, we have a few options.
                        // First, we need to check to see if the virtual_data is in our type hierarchy.
                        // If it is, then we use that type's slot
                        auto* b = baseT;
                        while (b != nullptr) {
                            if (vMethod->klass == b) {
                                logger.debug("Matching base type: %p for method: %p", b, vMethod);
                                // We are implementing an abstract method from our abstract base. Use the virtual_data's slot exactly.
                                logger.debug("Using base slot: %u for method: %p", vMethod->slot, m->get());
                                m->get()->slot = vMethod->slot;
                                if (m->get()->slot >= vtable.size() || m->get()->slot < 0) {
                                    // Our corrected slot is invalid!
                                    logger.critical("virtual_data slot: %u invalid! vtable size: %u", m->get()->slot, vtable.size());
                                    SAFE_ABORT();
                                }
                                logger.debug("Using slot: %u", m->get()->slot);
                                vtable[m->get()->slot].method = m->get();
                                vtable[m->get()->slot].methodPtr = m->get()->methodPointer;
                                set = true;
                                break;
                            }
                            b = b->parent;
                        }
                        
                        if (!set) {
                            auto infs = interfaces();
                            for (auto inter : infs) {
                                if (vMethod->klass == inter) {
                                    // Found a matching interface that we plan on implementing.
                                    // Add a new item to our offsets list
                                    if (inter->method_count < vMethod->slot) {
                                        // For some reason, the interface has a lower method count than the slot of the method we are looking for...
                                        // This could theoretically be the case when we have an interface that implements multiple interfaces?
                                        logger.critical("Interface: %p method count: %u < virtual data: %p slot: %u", inter, inter->method_count, vMethod, vMethod->slot);
                                        SAFE_ABORT();
                                    }
                                    auto& off = offsets.emplace_back();
                                    off.interfaceType = inter;
                                    off.offset = vtable.size();
                                    logger.debug("Made new interface offset for type: %p, offset: %u", off.interfaceType, off.offset);
                                    // Now we need to add to vtable to ensure we have all of our interface methods
                                    // for this interface properly added.
                                    // TODO: We should do an assertion here to make sure that we actually properly implement each method.
                                    // If we don't, we will segfault if this type is used like any other.
                                    for (int q = 0; q < inter->method_count; q++) {
                                        vtable.emplace_back();
                                    }
                                    // Once we have added all of the methods from our interface that we plan on implementing, set our slot
                                    m->get()->slot = off.offset + vMethod->slot;
                                    if (m->get()->slot >= vtable.size() || m->get()->slot < 0) {
                                        // Our corrected slot is invalid!
                                        logger.critical("New interface slot: %u invalid! vtable size: %u", m->get()->slot, vtable.size());
                                        SAFE_ABORT();
                                    }
                                    logger.debug("Using slot: %u", m->get()->slot);
                                    vtable[m->get()->slot].method = m->get();
                                    vtable[m->get()->slot].methodPtr = m->get()->methodPointer;
                                    set = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (!set) {
                        // We should be implementing the interface.
                        // If we reach here, we don't implement or extend the virtual method we want to implement.
                        logger.critical("Method: %p needs virtual_data: %p which requires type: %p which does not exist!", m, vMethod, vMethod->klass);
                        logger.critical("Ensure all of your virtual methods' types are defined in the interfaces in DECLARE_CLASS_INTERFACES!");
                        SAFE_ABORT();
                    }
                }
            }
        } else {
            // We have no base type.
            // For now, we will simply throw in such a case.
            logger.critical("Cannot create a vtable without a base type!");
            SAFE_ABORT();
        }
    }

    void TypeRegistration::populateMethods() {
        // After this, we have out vtable vector fully populated. The size of it will be correct.
        std::vector<VirtualInvokeData> vtable;
        std::vector<Il2CppRuntimeInterfaceOffsetPair> offsets;
        getVtable(vtable, offsets);

        auto* k = klass();
        // Method creation should happen around now
        k->is_vtable_initialized = true;
        k->vtable_count = vtable.size();
        // Use our vtable vector to create our vtable
        for (int i = 0; i < vtable.size(); i++) {
            // If we come across any vtable items that have null function pointers or other stuff, we become sad.
            // This means we haven't implemented everything, so we should make a point in ensuring this happens.
            if (vtable[i].method == nullptr || vtable[i].methodPtr == nullptr) {
                _logger().critical("Vtable index: %u has null method or method pointer! Ensure you implement the interface entirely (and do not use any nullptrs!)", i);
                SAFE_ABORT();
            }
            k->vtable[i] = vtable[i];
        }
        k->interface_offsets_count = offsets.size();
        k->interfaceOffsets = reinterpret_cast<Il2CppRuntimeInterfaceOffsetPair*>(calloc(offsets.size(), sizeof(Il2CppRuntimeInterfaceOffsetPair)));
        for (int i = 0; i < offsets.size(); i++) {
            k->interfaceOffsets[i] = offsets[i];
        }
        // If any method exists that has a Finalize name, should use it
        k->has_finalize = false;
        // TODO: Allow cctor to exist someday
        k->has_cctor = false;

        auto methods = getMethods();
        k->method_count = methods.size();
        k->methods = reinterpret_cast<const MethodInfo**>(calloc(k->method_count, sizeof(MethodInfo*)));
        for (uint16_t i = 0; i < k->method_count; i++) {
            auto* m = methods[i];
            m->get()->klass = k;
            // TODO: Populate other fields as necessary
            k->methods[i] = m->get();
            if (!k->has_finalize && checkVirtualsForMatch(methods[i], "System", "Object", "Finalize", 0)) {
                k->has_finalize = true;
            }
        }
    }

    void TypeRegistration::clear() {
        auto* k = klass();
        if (k) {
            if (k->typeHierarchy) {
                free(k->typeHierarchy);
            }
            if (k->fields) {
                free(k->fields);
            }
            if (k->interfaceOffsets) {
                free(k->interfaceOffsets);
            }
            if (k->methods) {
                for (uint16_t i = 0; i < k->method_count; ++i) {
                    free(const_cast<ParameterInfo*>(k->methods[i]->parameters));
                    delete k->methods[i];
                }
                free(k->methods);
            }
            free(k);
        }
        auto* f = static_fields();
        if (f) {
            il2cpp_functions::GC_free(f);
        }
    }
}