#include "logging.hpp"
#include "beatsaber-hook/utils/typedefs.h"

namespace custom_types {

    const Logger& _logger() {
        static const Logger logger("CustomTypes|" VERSION);
        return logger;
    }

    void logAname(const Il2CppAssemblyName* name, std::string_view anameLabel) {
        if (!name) {
            _logger().debug("%s: NULL ASSEMBLY NAME!", anameLabel.data());
            return;
        }
        _logger().debug("0 ======================ASSEMBLY NAME INFO FOR: %s======================", anameLabel.data());
        _logger().debug("name: %s", name->name ? name->name : "NULL");
        _logger().debug("culture: %s", name->culture ? name->culture : "NULL");
        _logger().debug("hash_value: %s", name->hash_value ? name->hash_value : "NULL");
        _logger().debug("public_key: %s", name->public_key ? name->public_key : "NULL");
        _logger().debug("hash_alg: %u", name->hash_alg);
        _logger().debug("hash_len: %u", name->hash_len);
        _logger().debug("flags: 0x%x", name->flags);
        _logger().debug("major: %u", name->major);
        _logger().debug("minor: %u", name->minor);
        _logger().debug("build: %u", name->build);
        _logger().debug("revision: %u", name->revision);
        _logger().debug("public_key_token: %p", name->public_key_token);
        _logger().debug("0 ======================END ASSEMBLY NAME INFO======================");
    }

    void logAsm(const Il2CppAssembly* assem) {
        if (!assem) {
            _logger().debug("NULL ASSEMBLY!");
            return;
        }
        _logger().debug("0 ======================ASSEMBLY INFO FOR: %p======================", assem);
        _logger().debug("image: %p", assem->image);
        _logger().debug("token: %u", assem->token);
        _logger().debug("referencedAssemblyStart: %u", assem->referencedAssemblyStart);
        _logger().debug("referencedAssemblyCount: %u", assem->referencedAssemblyCount);
        logAname(&assem->aname, "aname");
        _logger().debug("0 ======================END ASSEMBLY INFO======================");
    }

    void logCodegen(const Il2CppCodeGenModule* module, std::string_view s) {
        if (!module) {
            _logger().debug("%s: NULL MODULE!", s.data());
            return;
        }
        _logger().debug("0 ======================CODEGEN INFO FOR: %p, %s======================", module, s.data());
        _logger().debug("moduleName: %s", module->moduleName ? module->moduleName : "NULL");
        _logger().debug("methodPointerCount: %u", module->methodPointerCount);
        _logger().debug("methodPointers: %p", module->methodPointers);
        _logger().debug("invokerIndices: %p", module->invokerIndices);
        _logger().debug("reversePInvokeWrapperCount: %u", module->reversePInvokeWrapperCount);
        _logger().debug("reversePInvokeWrapperIndices: %p", module->reversePInvokeWrapperIndices);
        _logger().debug("rgctxRangesCount: %u", module->rgctxRangesCount);
        _logger().debug("rgctxRanges: %p", module->rgctxRanges);
        _logger().debug("rgctxsCount: %u", module->rgctxsCount);
        _logger().debug("rgctxs: %p", module->rgctxs);
        _logger().debug("debuggerMetadata: %p", module->debuggerMetadata);
        _logger().debug("0 ======================END CODEGEN INFO======================");
    }

    void logImage(const Il2CppImage* img) {
        if (!img) {
            _logger().debug("NULL IMAGE!");
            return;
        }
        _logger().debug("0 ======================IMAGE INFO FOR: %p======================", img);
        _logger().debug("name: %s", img->name ? img->name : "NULL");
        _logger().debug("nameNoExt: %s", img->nameNoExt ? img->nameNoExt : "NULL");
        logAsm(img->assembly);
        _logger().debug("typeStart: %u", img->typeStart);
        _logger().debug("typeCount: %u", img->typeCount);
        _logger().debug("exportedTypeStart: %u", img->exportedTypeStart);
        _logger().debug("exportedTypeCount: %u", img->exportedTypeCount);
        _logger().debug("customAttributeStart: %u", img->customAttributeStart);
        _logger().debug("customAttributeCount: %u", img->customAttributeCount);
        _logger().debug("entryPointIndex: %u", img->entryPointIndex);
        _logger().debug("nameToClassHashTable: %p", img->nameToClassHashTable);
        logCodegen(img->codeGenModule, "codeGenModule");
        _logger().debug("token: %u", img->token);
        _logger().debug("dynamic: %u", img->dynamic);
        _logger().debug("0 ======================END IMAGE INFO======================");
    }

    void logType(const Il2CppType* t, std::string_view s) {
        _logger().debug("0 ======================TYPE INFO FOR: %s======================", s.data());
        _logger().debug("klassIndex: %u", t->data.klassIndex);
        _logger().debug("attrs: 0x%x", t->attrs);
        _logger().debug("type: 0x%x", t->type);
        _logger().debug("num_mods: %u", t->num_mods);
        _logger().debug("byref: %u", t->byref);
        _logger().debug("pinned: %u", t->pinned);
        _logger().debug("0 ======================END TYPE INFO======================");
    }

    void logParam(const ParameterInfo* info) {
        if (!info) {
            _logger().debug("NULL PARAMETER INFO!");
            return;
        }
        _logger().debug("0 ======================PARAMETER INFO FOR: %p======================", info);
        _logger().debug("name: %s", info->name ? info->name : "NULL");
        _logger().debug("position: %u", info->position);
        _logger().debug("token: %u", info->token);
        _logger().debug("parameter_type: %p", info->parameter_type);
        _logger().debug("0 ======================END PARAMETER INFO======================");
    }

    void logMethod(const MethodInfo* info) {
        if (!info) {
            _logger().debug("NULL METHODINFO!");
            return;
        }
        _logger().debug("0 ======================METHOD INFO FOR: %p======================", info);
        _logger().debug("methodPointer: %p", info->methodPointer);
        _logger().debug("invoker_method: %p", info->invoker_method);
        _logger().debug("name: %s", info->name ? info->name : "NULL");
        _logger().debug("klass: %p", info->klass);
        _logger().debug("return_type: %p", info->return_type);
        if (info->parameters) {
            for (int i = 0; i < info->parameters_count; i++) {
                logParam(&info->parameters[i]);
            }
        } else {
            _logger().debug("parameters: 0x0");
        }
        // _logger().debug("parameters: %p", info->parameters);
        _logger().debug("methodDefinition: %p", info->methodDefinition);
        _logger().debug("genericContainer: %p", info->genericContainer);
        _logger().debug("token: %u", info->token);
        _logger().debug("flags: 0x%x", info->flags);
        _logger().debug("iflags: 0x%x", info->iflags);
        _logger().debug("slot: %u", info->slot);
        _logger().debug("parameters_count: %u", info->parameters_count);
        _logger().debug("is_generic: %u", info->is_generic);
        _logger().debug("is_inflated: %u", info->is_inflated);
        _logger().debug("wrapper_type: %u", info->wrapper_type);
        _logger().debug("is_marshaled_from_native: %u", info->is_marshaled_from_native);
        _logger().debug("0 ======================END METHOD INFO======================");
    }

    // Log every field on an Il2CppClass*, including the internal Il2CppType structs
    void logAll(const Il2CppClass* klass) {
        _logger().debug("0 ======================CLASS INFO FOR CLASS: %s::%s======================", klass->namespaze, klass->name);
        logImage(klass->image);
        _logger().debug("gc_desc: %p", klass->gc_desc);
        _logger().debug("name: %s", klass->name);
        _logger().debug("namespaze: %s", klass->namespaze);
        logType(&klass->byval_arg, "byval_arg");
        logType(&klass->this_arg, "this_arg");
        _logger().debug("element_class: %p", klass->element_class);
        _logger().debug("castClass: %p", klass->castClass);
        _logger().debug("declaringType: %p", klass->declaringType);
        _logger().debug("parent: %p", klass->parent);
        _logger().debug("generic_class: %p", klass->generic_class);
        _logger().debug("typeDefinition: %p", klass->typeDefinition);
        _logger().debug("interopData: %p", klass->interopData);
        _logger().debug("klass: %p", klass->klass);
        _logger().debug("fields: %p", klass->fields);
        _logger().debug("events: %p", klass->events);
        _logger().debug("properties: %p", klass->properties);
        if (klass->methods) {
            for (int i = 0; i < klass->method_count; i++) {
                logMethod(klass->methods[i]);
                usleep(5000);  // 0.005s
            }
        } else {
            _logger().debug("methods: 0x0");
        }
        _logger().debug("nestedTypes: %p", klass->nestedTypes);
        _logger().debug("implementedInterfaces: %p", klass->implementedInterfaces);
        _logger().debug("interfaceOffsets: %p", klass->interfaceOffsets);
        _logger().debug("static_fields: %p", klass->static_fields);
        _logger().debug("rgctx_data: %p", klass->rgctx_data);
        _logger().debug("typeHierarchy: %p", klass->typeHierarchy);
        _logger().debug("unity_user_data: %p", klass->unity_user_data);
        _logger().debug("initializationExceptionGCHandle: %u", klass->initializationExceptionGCHandle);
        _logger().debug("cctor_started: %u", klass->cctor_started);
        _logger().debug("cctor_finished: %u", klass->cctor_finished);
        _logger().debug("cctor_thread: %u", klass->cctor_thread);
        _logger().debug("genericContainerIndex: %u", klass->genericContainerIndex);
        _logger().debug("instance_size: %u", klass->instance_size);
        _logger().debug("actualSize: %u", klass->actualSize);
        _logger().debug("element_size: %u", klass->element_size);
        _logger().debug("native_size: %u", klass->native_size);
        _logger().debug("static_fields_size: %u", klass->static_fields_size);
        _logger().debug("thread_static_fields_size: %u", klass->thread_static_fields_size);
        _logger().debug("thread_static_fields_offset: %u", klass->thread_static_fields_offset);
        _logger().debug("flags: 0x%x", klass->flags);
        _logger().debug("token: %u", klass->token);
        _logger().debug("method_count: %u", klass->method_count);
        _logger().debug("property_count: %u", klass->property_count);
        _logger().debug("field_count: %u", klass->field_count);
        _logger().debug("event_count: %u", klass->event_count);
        _logger().debug("nested_type_count: %u", klass->nested_type_count);
        _logger().debug("vtable_count: %u", klass->vtable_count);
        _logger().debug("interfaces_count: %u", klass->interfaces_count);
        _logger().debug("interface_offsets_count: %u", klass->interface_offsets_count);
        _logger().debug("typeHierarchyDepth: %u", klass->typeHierarchyDepth);
        _logger().debug("genericRecursionDepth: %u", klass->genericRecursionDepth);
        _logger().debug("rank: %u", klass->rank);
        _logger().debug("minimumAlignment: %u", klass->minimumAlignment);
        _logger().debug("naturalAligment: %u", klass->naturalAligment);
        _logger().debug("packingSize: %u", klass->packingSize);
        _logger().debug("initialized_and_no_error: %u", klass->initialized_and_no_error);
        _logger().debug("valuetype: %u", klass->valuetype);
        _logger().debug("initialized: %u", klass->initialized);
        _logger().debug("enumtype: %u", klass->enumtype);
        _logger().debug("is_generic: %u", klass->is_generic);
        _logger().debug("has_references: %u", klass->has_references);
        _logger().debug("init_pending: %u", klass->init_pending);
        _logger().debug("size_inited: %u", klass->size_inited);
        _logger().debug("has_finalize: %u", klass->has_finalize);
        _logger().debug("has_cctor: %u", klass->has_cctor);
        _logger().debug("is_blittable: %u", klass->is_blittable);
        _logger().debug("is_import_or_windows_runtime: %u", klass->is_import_or_windows_runtime);
        _logger().debug("is_vtable_initialized: %u", klass->is_vtable_initialized);
        _logger().debug("has_initialization_error: %u", klass->has_initialization_error);
        _logger().debug("0 ======================END CLASS INFO======================");
    }
}