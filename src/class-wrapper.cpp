#include "types.hpp"
#include "register.hpp"
#include "logging.hpp"

namespace custom_types {
    TypeDefinitionIndex ClassWrapper::typeIdx = kTypeDefinitionIndexInvalid;
    ClassWrapper::ClassWrapper(type_info* info) {
        // Create the class
        klass = new Il2CppClass();
        this->info = info;
        auto* type = createType(info->typeEnum);
        // Create image from namespace
        // klass->image = Register::createImage(info->namespaze);
        setupTypeHierarchy(info->base);
        // Grab image from our parent for now
        klass->image = Register::createImage(info->namespaze);
        // Set name
        klass->name = info->name.c_str();
        klass->namespaze = info->namespaze.c_str();
        klass->this_arg = *type;
        // TODO: Modify this for future usage where byval argument may not match this argument
        klass->byval_arg = *type;
        // TODO: Modify flags in the future
        // For now, copy them from base class
        klass->flags = info->base->flags;
        // This is for arrays, we can safely assume we are not an array so set this to ourselves.
        klass->element_class = klass;
        // TODO: Determine more information from this
        klass->castClass = klass;
        // Pointer to self
        klass->klass = klass;
        // Unless the class is used in native code, this isn't necessary
        klass->native_size = -1;
        // Size is set after processing fields
        // Methods are set after processing methods
        // TODO: Figure out interfaces
        klass->interfaces_count = 0;
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
        // TODO: Modify vtables in future, instead of copying from base vtable
        klass->is_vtable_initialized = true;
        klass->vtable_count = info->base->vtable_count;
        for (auto i = 0; i < info->base->vtable_count; i++) {
            klass->vtable[i] = info->base->vtable[i];
        }
        // TODO: is this valid?
        klass->token = -1;
        // TODO: See if this is always the case
        klass->unity_user_data = nullptr;
        // Just assume we always have references. This is mostly unused anyways.
        klass->has_references = 1;
        // logger().debug("Deleting Il2CppType! Ptr: %p", type);
        // delete type;
    }

    ClassWrapper::~ClassWrapper() {
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

    void ClassWrapper::populateMethods() {
        // TODO: Should ensure this value from cross checking it against methods.
        // If any method exists that has a Finalize name, should use it
        klass->has_finalize = false;
        // TODO: Allow cctor to exist someday
        klass->has_cctor = false;

        klass->method_count = methods.size();
        klass->methods = reinterpret_cast<const MethodInfo**>(calloc(klass->method_count, sizeof(MethodInfo*)));
        for (auto i = 0; i < klass->method_count; i++) {
            methods[i]->setClass(klass);
            auto* info = methods[i]->get();
            // TODO: Populate other fields as necessary
            klass->methods[i] = info;
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