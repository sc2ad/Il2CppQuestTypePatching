#include "types.hpp"
#include "register.hpp"
#include "logging.hpp"

namespace custom_types {
    TypeDefinitionIndex ClassWrapper::typeIdx = kTypeDefinitionIndexInvalid;
    ClassWrapper::ClassWrapper(type_info* info) {
        this->info = info;
    }

    void ClassWrapper::getVtable(std::vector<VirtualInvokeData>& vtable, std::vector<Il2CppRuntimeInterfaceOffsetPair>& offsets) {
        // First, we want to iterate over our methods
        // With these methods, we want to determine if any of them have virtual data
        // If they do, we want to look in our existing vtable if they exist. If they do, remap the methodPointer
        // If they do not, create a new entry.
        // Total new vtable_count should always be >= info->base->vtable_count

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

        if (info->base) {
            // Logically speaking, the easiest approach we can follow is:
            // Copy over our base vtable
            // Change single values as we see them.
            _logger().debug("Copying base offsets...");
            for (uint16_t offIdx = 0; offIdx < info->base->interface_offsets_count; offIdx++) {
                offsets.push_back(info->base->interfaceOffsets[offIdx]);
            }
            _logger().debug("Copying base vtable...");
            for (uint16_t i = 0; i < info->base->vtable_count; i++) {
                vtable.push_back(info->base->vtable[i]);
            }
            // After everything is copied, we can easily iterate over our methods, find any that are virtual
            // If we find a virtual method, see if we are overriding a base interface, or making a new one
            for (auto m : methods) {
                if (m->virtual_data != nullptr) {
                    bool set = false;
                    if (m->virtual_data->slot < 0) {
                        _logger().critical("Virtual data: %p has slot: %u which is invalid!", m->virtual_data, m->virtual_data->slot);
                        CRASH_UNLESS(false);
                    }
                    for (auto& inter : offsets) {
                        if (inter.interfaceType == m->virtual_data->klass) {
                            // We found a virtual method which has an interface already listed in our interface offsets.
                            // The method's slot should be set to m->virtual_data->slot + inter.offset
                            // We also need to overwrite explicitly that index of the vtable
                            m->info->slot = m->virtual_data->slot + inter.offset;
                            if (m->info->slot >= vtable.size() || m->info->slot < 0) {
                                // Our corrected slot is invalid!
                                _logger().critical("Existing interface slot: %u invalid! vtable size: %u", m->info->slot, vtable.size());
                                CRASH_UNLESS(false);
                            }
                            _logger().debug("Using slot: %u", m->info->slot);
                            vtable[m->info->slot].method = m->get();
                            vtable[m->info->slot].methodPtr = m->get()->methodPointer;
                            set = true;
                            break;
                        }
                    }
                    if (!set) {
                        // If we haven't found a matching interface type for this method, we have a few options.
                        // First, we need to check to see if the virtual_data is in our type hierarchy.
                        // If it is, then we use that type's slot
                        auto* b = info->base;
                        while (b != nullptr) {
                            if (m->virtual_data->klass == b) {
                                _logger().debug("Matching base type: %p for method: %p", b, m->virtual_data);
                                // We are implementing an abstract method from our abstract base. Use the virtual_data's slot exactly.
                                _logger().debug("Using base slot: %u for method: %p", m->virtual_data->slot, m->get());
                                m->info->slot = m->virtual_data->slot;
                                if (m->info->slot >= vtable.size() || m->info->slot < 0) {
                                    // Our corrected slot is invalid!
                                    _logger().critical("virtual_data slot: %u invalid! vtable size: %u", m->info->slot, vtable.size());
                                    CRASH_UNLESS(false);
                                }
                                _logger().debug("Using slot: %u", m->info->slot);
                                vtable[m->info->slot].method = m->get();
                                vtable[m->info->slot].methodPtr = m->get()->methodPointer;
                                set = true;
                                break;
                            }
                            b = b->parent;
                        }
                        
                        if (!set) {
                            for (auto& inter : info->interfaces) {
                                if (m->virtual_data->klass == inter) {
                                    // Found a matching interface that we plan on implementing.
                                    // Add a new item to our offsets list
                                    if (inter->method_count < m->virtual_data->slot) {
                                        // For some reason, the interface has a lower method count than the slot of the method we are looking for...
                                        // This could theoretically be the case when we have an interface that implements multiple interfaces?
                                        _logger().critical("Interface: %p method count: %u < virtual data: %p slot: %u", inter, inter->method_count, m->virtual_data, m->virtual_data->slot);
                                        CRASH_UNLESS(false);
                                    }
                                    auto& off = offsets.emplace_back();
                                    off.interfaceType = inter;
                                    off.offset = vtable.size();
                                    _logger().debug("Made new interface offset for type: %p, offset: %u", off.interfaceType, off.offset);
                                    // Now we need to add to vtable to ensure we have all of our interface methods
                                    // for this interface properly added.
                                    // TODO: We should do an assertion here to make sure that we actually properly implement each method.
                                    // If we don't, we will segfault if this type is used like any other.
                                    for (int q = 0; q < inter->method_count; q++) {
                                        vtable.emplace_back();
                                    }
                                    // Once we have added all of the methods from our interface that we plan on implementing, set our slot
                                    m->info->slot = off.offset + m->virtual_data->slot;
                                    if (m->info->slot >= vtable.size() || m->info->slot < 0) {
                                        // Our corrected slot is invalid!
                                        _logger().critical("New interface slot: %u invalid! vtable size: %u", m->info->slot, vtable.size());
                                        CRASH_UNLESS(false);
                                    }
                                    _logger().debug("Using slot: %u", m->info->slot);
                                    vtable[m->info->slot].method = m->get();
                                    vtable[m->info->slot].methodPtr = m->get()->methodPointer;
                                    set = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (!set) {
                        // We should be implementing the interface.
                        // If we reach here, we don't implement or extend the virtual method we want to implement.
                        _logger().critical("Method: %p needs virtual_data: %p which requires type: %p which does not exist!", m->get(), m->virtual_data, m->virtual_data->klass);
                        _logger().critical("Ensure all of your virtual methods' types are defined in the interfaces in DECLARE_CLASS_INTERFACES!");
                        CRASH_UNLESS(false);
                    }
                }
            }
        } else {
            // We have no base type.
            // For now, we will simply throw in such a case.
            _logger().critical("Cannot create a vtable without a base type!");
            CRASH_UNLESS(false);
        }
    }

    void ClassWrapper::createClass(std::size_t size) {
        // After this, we have out vtable vector fully populated. The size of it will be correct.
        std::vector<VirtualInvokeData> vtable;
        std::vector<Il2CppRuntimeInterfaceOffsetPair> offsets;
        getVtable(vtable, offsets);

        // Create the class
        klass = static_cast<Il2CppClass*>(calloc(1, sizeof(Il2CppClass) + vtable.size() * sizeof(VirtualInvokeData)));
        auto* type = createType(info->typeEnum);
        setupTypeHierarchy(info->base);
        // Create image from namespace
        auto img = Register::createImage(info->namespaze);
        klass->image = img;
        // Add ourselves to our image hash table (for class_from_name)
        img->nameToClassHashTable->insert(std::make_pair(std::make_pair(info->namespaze.c_str(), info->name.c_str()), type->data.klassIndex));
        // Set name
        klass->name = info->name.c_str();
        klass->namespaze = info->namespaze.c_str();
        klass->this_arg = *type;
        // TODO: Modify this for future usage where byval argument may not match this argument
        klass->byval_arg = *type;
        // TODO: Modify flags in the future
        // For now, copy them from base class
        klass->flags = info->base->flags;
        // We should be explicit: Do we want to be abstract or be explicitly non-abstract?
        if (info->abst) {
            klass->flags |= TYPE_ATTRIBUTE_ABSTRACT;
        } else {
            klass->flags &= ~TYPE_ATTRIBUTE_ABSTRACT;
        }
        // This is for arrays, we can safely assume we are not an array so set this to ourselves.
        klass->element_class = klass;
        // TODO: Determine more information from this
        klass->castClass = klass;
        // Pointer to self
        klass->klass = klass;
        // Unless the class is used in native code, this isn't necessary
        klass->native_size = -1;
        // Size is set from sizeof(T)
        klass->actualSize = size;
        klass->instance_size = size;
        klass->size_inited = 1;
        // Methods are set after processing methods
        auto interfaceCnt = info->interfaces.size();
        klass->interfaces_count = interfaceCnt;
        // klass->implementedInterfaces needs to be allocated as well
        klass->implementedInterfaces = reinterpret_cast<Il2CppClass**>(calloc(interfaceCnt, sizeof(Il2CppClass*)));
        for (int i = 0; i < interfaceCnt; i++) {
            klass->implementedInterfaces[i] = info->interfaces[i];
        }
        // TODO: Figure out generic class (will also need to inflate it)
        klass->generic_class = nullptr;
        klass->genericContainerIndex = kGenericContainerIndexInvalid;
        klass->genericRecursionDepth = 1;
        // Pretend that the class has already been initialized
        klass->initialized = 1;
        klass->initialized_and_no_error = 1;
        klass->init_pending = 0;
        klass->has_initialization_error = 0;

        // TypeDefinition unused, can set to nullptr
        klass->typeDefinition = nullptr;
        if (type->type == Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE) {
            klass->valuetype = true;
        }
        // Method creation should happen around now
        klass->is_vtable_initialized = true;
        klass->vtable_count = vtable.size();
        // Use our vtable vector to create our vtable
        for (int i = 0; i < vtable.size(); i++) {
            // If we come across any vtable items that have null function pointers or other stuff, we become sad.
            // This means we haven't implemented everything, so we should make a point in ensuring this happens.
            if (vtable[i].method == nullptr || vtable[i].methodPtr == nullptr) {
                _logger().critical("Vtable index: %u has null method or method pointer! Ensure you implement the interface entirely (and do not use any nullptrs!)", i);
                CRASH_UNLESS(false);
            }
            klass->vtable[i] = vtable[i];
        }
        klass->interface_offsets_count = offsets.size();
        klass->interfaceOffsets = reinterpret_cast<Il2CppRuntimeInterfaceOffsetPair*>(calloc(offsets.size(), sizeof(Il2CppRuntimeInterfaceOffsetPair)));
        for (int i = 0; i < offsets.size(); i++) {
            klass->interfaceOffsets[i] = offsets[i];
        }
        // TODO: is this valid?
        klass->token = -1;
        // TODO: See if this is always the case
        klass->unity_user_data = nullptr;
        // Just assume we always have references. This is mostly unused anyways.
        klass->has_references = 1;
        // logger().debug("Deleting Il2CppType! Ptr: %p", type);
        delete type;
    }

    ClassWrapper::~ClassWrapper() {
        // static_assert(false, "asdf");
        // We need to delete info, all of the fields, methods, typeHierarchy, staticFields

        _logger().debug("Deleting typeHierarchy! Ptr: %p", klass->typeHierarchy);
        delete klass->typeHierarchy;
        _logger().debug("Deleting Il2CppClass*! Ptr: %p", klass);
        delete klass;
        _logger().debug("Deleting fields...");
        fields.clear();
        _logger().debug("Deleting static fields...");
        staticFields.clear();
        _logger().debug("Deleting methods...");
        methods.clear();
    }

    // Setup type hierarchy using base class
    void ClassWrapper::setupTypeHierarchy(Il2CppClass* baseClass) {
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

    void ClassWrapper::populateFields() {
        // TODO: Compute size from fields, may want to call Class::LayoutFieldsLocked
        // If we do that, we should not have size_inited set to true until after that call
        // If we do not choose to do that (and instead compute our own field offsets and whatnot) then size_inited should be true.
        klass->size_inited = 1;
        // This is always:
        // if parent: parent's alignment if not value type, else 1
        // otherwise: sizeof(Il2CppObject*)
        // The actual size of the type seems to be a bit more annoying to compute...
        klass->minimumAlignment = 8;
        klass->naturalAligment = 8;
        // TODO: Eventually allow creation of thread static fields?
        klass->thread_static_fields_offset = -1;
        klass->thread_static_fields_size = 0;
        // The count of fields on this type is actually fields + static fields
        // fields contains ALL fields, including static ones.
        klass->field_count = fields.size();
        if (fields.size() > 0) {
            klass->fields = reinterpret_cast<FieldInfo*>(calloc(klass->field_count, sizeof(FieldInfo)));
            for (uint16_t i = 0; i < fields.size(); i++) {
                klass->fields[i] = fields[i]->get();
                klass->fields[i].parent = klass;
            }
        }
        // TODO: Figure out static fields.
        // Static fields are more obnoxious, since klass->static_fields is a pointer to a region of memory
        // Where each static field is laid out with their offset mapping to the correct location.
        // This gets challenging, since for us we would like a pointer to our first static member and offset off that
        // Or we create a static member as a placeholder and offset off of that
        klass->static_fields = nullptr;
        klass->static_fields_size = 0;
    }

    bool ClassWrapper::checkVirtualsForMatch(custom_types::method_info* info, std::string_view namespaze, std::string_view name, std::string_view methodName, int paramCount) {
        // For now, we only match the highest level virtual data.
        // Any sub-level virtuals are missed/ignored.
        // This SHOULD be good enough for Finalize.
        if (info->virtual_data) {
            auto* k = info->virtual_data->klass;
            return namespaze == k->namespaze && name == k->name && methodName == info->virtual_data->name && (paramCount >= 0 ? info->virtual_data->parameters_count == paramCount : true);
        }
        return false;
    }

    void ClassWrapper::populateMethods() {
        // If any method exists that has a Finalize name, should use it
        klass->has_finalize = false;
        // TODO: Allow cctor to exist someday
        klass->has_cctor = false;

        klass->method_count = methods.size();
        klass->methods = reinterpret_cast<const MethodInfo**>(calloc(klass->method_count, sizeof(MethodInfo*)));
        for (auto i = 0; i < klass->method_count; i++) {
            methods[i]->setClass(klass);
            // As we iterate here, it's important for us to know if the method has references to ourselves in it.
            // If it does, we need to fix that with a PROPER reference to ourselves.
            methods[i]->fixSelf(&klass->byval_arg);
            auto* info = methods[i]->get();
            // TODO: Populate other fields as necessary
            klass->methods[i] = info;
            if (!klass->has_finalize && checkVirtualsForMatch(methods[i], "System", "Object", "Finalize", 0)) {
                klass->has_finalize = true;
            }
        }
    }

    Il2CppType* ClassWrapper::createType(Il2CppTypeEnum typeE) {
        // Create type using typeIdx
        auto type = new Il2CppType();
        // TODO: For field flags/parameter flags?
        // type->attrs;
        // TODO: Unclear if this is always the case
        type->byref = 0;
        // TODO: Unclear if this is always the case
        type->num_mods = 0;
        // TODO: Unclear if this is always the case
        type->pinned = 0;
        // TODO: Change this for value types and other type enums
        type->type = typeE;
        // This should be a unique number, assigned when each new type is created.
        type->data.klassIndex = typeIdx;
        typeIdx--;
        return type;
    }
}