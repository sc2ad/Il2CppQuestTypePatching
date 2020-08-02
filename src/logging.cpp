#include "logging.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

const Logger& logger() {
    static const Logger logger("CustomTypes|" VERSION);
    return logger;
}

void logAname(Il2CppAssemblyName* name, std::string_view anameLabel) {
    if (!name) {
        logger().debug("%s: NULL ASSEMBLY NAME!", anameLabel.data());
        return;
    }
    logger().debug("0 ======================ASSEMBLY NAME INFO FOR: %s======================", anameLabel.data());
    logger().debug("name: %s", name->name ? name->name : "NULL");
    logger().debug("culture: %s", name->culture ? name->culture : "NULL");
    logger().debug("hash_value: %s", name->hash_value ? name->hash_value : "NULL");
    logger().debug("public_key: %s", name->public_key ? name->public_key : "NULL");
    logger().debug("hash_alg: %u", name->hash_alg);
    logger().debug("hash_len: %u", name->hash_len);
    logger().debug("flags: 0x%x", name->flags);
    logger().debug("major: %u", name->major);
    logger().debug("minor: %u", name->minor);
    logger().debug("build: %u", name->build);
    logger().debug("revision: %u", name->revision);
    logger().debug("public_key_token: %p", name->public_key_token);
    logger().debug("0 ======================END ASSEMBLY NAME INFO======================");
}

void logAsm(Il2CppAssembly* assem) {
    if (!assem) {
        logger().debug("NULL ASSEMBLY!");
        return;
    }
    logger().debug("0 ======================ASSEMBLY INFO FOR: %p======================", assem);
    logger().debug("image: %p", assem->image);
    logger().debug("token: %u", assem->token);
    logger().debug("referencedAssemblyStart: %u", assem->referencedAssemblyStart);
    logger().debug("referencedAssemblyCount: %u", assem->referencedAssemblyCount);
    logAname(&assem->aname, "aname");
    logger().debug("0 ======================END ASSEMBLY INFO======================");
}

void logCodegen(const Il2CppCodeGenModule* module, std::string_view s) {
    if (!module) {
        logger().debug("%s: NULL MODULE!", s.data());
        return;
    }
    logger().debug("0 ======================CODEGEN INFO FOR: %p, %s======================", module, s.data());
    logger().debug("moduleName: %s", module->moduleName ? module->moduleName : "NULL");
    logger().debug("methodPointerCount: %u", module->methodPointerCount);
    logger().debug("methodPointers: %p", module->methodPointers);
    logger().debug("invokerIndices: %p", module->invokerIndices);
    logger().debug("reversePInvokeWrapperCount: %u", module->reversePInvokeWrapperCount);
    logger().debug("reversePInvokeWrapperIndices: %p", module->reversePInvokeWrapperIndices);
    logger().debug("rgctxRangesCount: %u", module->rgctxRangesCount);
    logger().debug("rgctxRanges: %p", module->rgctxRanges);
    logger().debug("rgctxsCount: %u", module->rgctxsCount);
    logger().debug("rgctxs: %p", module->rgctxs);
    logger().debug("debuggerMetadata: %p", module->debuggerMetadata);
    logger().debug("0 ======================END CODEGEN INFO======================");
}

void logImage(const Il2CppImage* img) {
    if (!img) {
        logger().debug("NULL IMAGE!");
        return;
    }
    logger().debug("0 ======================IMAGE INFO FOR: %p======================", img);
    logger().debug("name: %s", img->name ? img->name : "NULL");
    logger().debug("nameNoExt: %s", img->nameNoExt ? img->nameNoExt : "NULL");
    logAsm(img->assembly);
    logger().debug("typeStart: %u", img->typeStart);
    logger().debug("typeCount: %u", img->typeCount);
    logger().debug("exportedTypeStart: %u", img->exportedTypeStart);
    logger().debug("exportedTypeCount: %u", img->exportedTypeCount);
    logger().debug("customAttributeStart: %u", img->customAttributeStart);
    logger().debug("customAttributeCount: %u", img->customAttributeCount);
    logger().debug("entryPointIndex: %u", img->entryPointIndex);
    logger().debug("nameToClassHashTable: %p", img->nameToClassHashTable);
    logCodegen(img->codeGenModule, "codeGenModule");
    logger().debug("token: %u", img->token);
    logger().debug("dynamic: %u", img->dynamic);
    logger().debug("0 ======================END IMAGE INFO======================");
}

void logType(const Il2CppType* t, std::string_view s) {
    logger().debug("0 ======================TYPE INFO FOR: %s======================", s.data());
    logger().debug("klassIndex: %u", t->data.klassIndex);
    logger().debug("attrs: 0x%x", t->attrs);
    logger().debug("type: 0x%x", t->type);
    logger().debug("num_mods: %u", t->num_mods);
    logger().debug("byref: %u", t->byref);
    logger().debug("pinned: %u", t->pinned);
    logger().debug("0 ======================END TYPE INFO======================");
}

void logParam(const ParameterInfo* info) {
    if (!info) {
        logger().debug("NULL PARAMETER INFO!");
        return;
    }
    logger().debug("0 ======================PARAMETER INFO FOR: %p======================", info);
    logger().debug("name: %s", info->name ? info->name : "NULL");
    logger().debug("position: %u", info->position);
    logger().debug("token: %u", info->token);
    logger().debug("parameter_type: %p", info->parameter_type);
    logger().debug("0 ======================END PARAMETER INFO======================");
}

void logMethod(const MethodInfo* info) {
    if (!info) {
        logger().debug("NULL METHODINFO!");
        return;
    }
    logger().debug("0 ======================METHOD INFO FOR: %p======================", info);
    logger().debug("methodPointer: %p", info->methodPointer);
    logger().debug("invoker_method: %p", info->invoker_method);
    logger().debug("name: %s", info->name ? info->name : "NULL");
    logger().debug("klass: %p", info->klass);
    logger().debug("return_type: %p", info->return_type);
    if (info->parameters) {
        for (int i = 0; i < info->parameters_count; i++) {
            logParam(&info->parameters[i]);
        }
    } else {
        logger().debug("parameters: 0x0");
    }
    // logger().debug("parameters: %p", info->parameters);
    logger().debug("methodDefinition: %p", info->methodDefinition);
    logger().debug("genericContainer: %p", info->genericContainer);
    logger().debug("token: %u", info->token);
    logger().debug("flags: 0x%x", info->flags);
    logger().debug("iflags: 0x%x", info->iflags);
    logger().debug("slot: %u", info->slot);
    logger().debug("parameters_count: %u", info->parameters_count);
    logger().debug("is_generic: %u", info->is_generic);
    logger().debug("is_inflated: %u", info->is_inflated);
    logger().debug("wrapper_type: %u", info->wrapper_type);
    logger().debug("is_marshaled_from_native: %u", info->is_marshaled_from_native);
    logger().debug("0 ======================END METHOD INFO======================");
}

// Log every field on an Il2CppClass*, including the internal Il2CppType structs
void logAll(Il2CppClass* klass) {
    logger().debug("0 ======================CLASS INFO FOR CLASS: %s::%s======================", klass->namespaze, klass->name);
    logImage(klass->image);
    logger().debug("gc_desc: %p", klass->gc_desc);
    logger().debug("name: %s", klass->name);
    logger().debug("namespaze: %s", klass->namespaze);
    logType(&klass->byval_arg, "byval_arg");
    logType(&klass->this_arg, "this_arg");
    logger().debug("element_class: %p", klass->element_class);
    logger().debug("castClass: %p", klass->castClass);
    logger().debug("declaringType: %p", klass->declaringType);
    logger().debug("parent: %p", klass->parent);
    logger().debug("generic_class: %p", klass->generic_class);
    logger().debug("typeDefinition: %p", klass->typeDefinition);
    logger().debug("interopData: %p", klass->interopData);
    logger().debug("klass: %p", klass->klass);
    logger().debug("fields: %p", klass->fields);
    logger().debug("events: %p", klass->events);
    logger().debug("properties: %p", klass->properties);
    if (klass->methods) {
        for (int i = 0; i < klass->method_count; i++) {
            logMethod(klass->methods[i]);
            usleep(5000);  // 0.005s
        }
    } else {
        logger().debug("methods: 0x0");
    }
    logger().debug("nestedTypes: %p", klass->nestedTypes);
    logger().debug("implementedInterfaces: %p", klass->implementedInterfaces);
    logger().debug("interfaceOffsets: %p", klass->interfaceOffsets);
    logger().debug("static_fields: %p", klass->static_fields);
    logger().debug("rgctx_data: %p", klass->rgctx_data);
    logger().debug("typeHierarchy: %p", klass->typeHierarchy);
    logger().debug("unity_user_data: %p", klass->unity_user_data);
    logger().debug("initializationExceptionGCHandle: %u", klass->initializationExceptionGCHandle);
    logger().debug("cctor_started: %u", klass->cctor_started);
    logger().debug("cctor_finished: %u", klass->cctor_finished);
    logger().debug("cctor_thread: %u", klass->cctor_thread);
    logger().debug("genericContainerIndex: %u", klass->genericContainerIndex);
    logger().debug("instance_size: %u", klass->instance_size);
    logger().debug("actualSize: %u", klass->actualSize);
    logger().debug("element_size: %u", klass->element_size);
    logger().debug("native_size: %u", klass->native_size);
    logger().debug("static_fields_size: %u", klass->static_fields_size);
    logger().debug("thread_static_fields_size: %u", klass->thread_static_fields_size);
    logger().debug("thread_static_fields_offset: %u", klass->thread_static_fields_offset);
    logger().debug("flags: 0x%x", klass->flags);
    logger().debug("token: %u", klass->token);
    logger().debug("method_count: %u", klass->method_count);
    logger().debug("property_count: %u", klass->property_count);
    logger().debug("field_count: %u", klass->field_count);
    logger().debug("event_count: %u", klass->event_count);
    logger().debug("nested_type_count: %u", klass->nested_type_count);
    logger().debug("vtable_count: %u", klass->vtable_count);
    logger().debug("interfaces_count: %u", klass->interfaces_count);
    logger().debug("interface_offsets_count: %u", klass->interface_offsets_count);
    logger().debug("typeHierarchyDepth: %u", klass->typeHierarchyDepth);
    logger().debug("genericRecursionDepth: %u", klass->genericRecursionDepth);
    logger().debug("rank: %u", klass->rank);
    logger().debug("minimumAlignment: %u", klass->minimumAlignment);
    logger().debug("naturalAligment: %u", klass->naturalAligment);
    logger().debug("packingSize: %u", klass->packingSize);
    logger().debug("initialized_and_no_error: %u", klass->initialized_and_no_error);
    logger().debug("valuetype: %u", klass->valuetype);
    logger().debug("initialized: %u", klass->initialized);
    logger().debug("enumtype: %u", klass->enumtype);
    logger().debug("is_generic: %u", klass->is_generic);
    logger().debug("has_references: %u", klass->has_references);
    logger().debug("init_pending: %u", klass->init_pending);
    logger().debug("size_inited: %u", klass->size_inited);
    logger().debug("has_finalize: %u", klass->has_finalize);
    logger().debug("has_cctor: %u", klass->has_cctor);
    logger().debug("is_blittable: %u", klass->is_blittable);
    logger().debug("is_import_or_windows_runtime: %u", klass->is_import_or_windows_runtime);
    logger().debug("is_vtable_initialized: %u", klass->is_vtable_initialized);
    logger().debug("has_initialization_error: %u", klass->has_initialization_error);
    logger().debug("0 ======================END CLASS INFO======================");
}
