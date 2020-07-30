#include "include/main.hpp"

#include "extern/modloader/modloader.hpp"

#include "extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "extern/beatsaber-hook/shared/utils/instruction-parsing.hpp"
#include "extern/beatsaber-hook/shared/utils/typedefs.h"
#include "extern/beatsaber-hook/shared/utils/logging.hpp"

#include <string_view>
#include <vector>

inline static void (*Class_SetupGCDescriptor)(Il2CppClass* klass);
inline static bool (*Class_IsSubclassOf)(Il2CppClass *klass, Il2CppClass *klassc, bool check_interfaces);

ModInfo modInfo;
const Logger& logger()
{
    static const Logger& logger(modInfo);
    return logger;
}

extern "C" void setup(ModInfo& info)
{
    info.id      = "TypePatching";
    info.version = "0.1.0";
    modInfo      = info;
    logger().info("Leaving setup!");
}

static Il2CppAssembly* myAsm;
static Il2CppImage* myImage;
static std::vector<Il2CppClass*> customClasses{};

// Custom Update function
void UpdateFunction(Il2CppObject* self) {
    logger().info("Called Update! Instance: %p", self);
}

// Custom Start function
void StartFunction(Il2CppObject* self) {
    logger().info("Called Start! Instance: %p", self);
}

// Custom Awake function
void AwakeFunction(Il2CppObject* self) {
    logger().info("Called Awake! Instance: %p", self);
}

// Custom ctor function
void Ctor(Il2CppObject* self) {
    logger().info("Called ctor! Instance: %p", self);
    // TODO: Maybe need to do more in our constructor than simply this?
    // Perhaps il2cpp wants us to actually call our base ctor?
}

// After being created and attached to an image, the image must also be attached to the assembly.
Il2CppAssembly* createMyAssembly(Il2CppImage* image, std::string_view name) {
    myAsm = reinterpret_cast<Il2CppAssembly*>(calloc(1, sizeof(Il2CppAssembly)));
    myAsm->image = image;
    // Name doesn't need to be allocated on heap
    // Name MAY need to be copied, however.
    myAsm->aname.name = name.data();
    return myAsm;
}

Il2CppImage* createMyImage(std::string_view name) {
    myImage = reinterpret_cast<Il2CppImage*>(calloc(1, sizeof(Il2CppImage)));
    myImage->name = name.data();
    myImage->nameNoExt = name.data();
    myImage->dynamic = 1;
    // TODO: Unclear if the rest of myImage is necessary or even useful!
    return myImage;
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

Il2CppType* createMyType(TypeDefinitionIndex klassIndex) {
    // Optimally, this would be replaced via a MetadataCalloc
    auto myType = reinterpret_cast<Il2CppType*>(calloc(1, sizeof(Il2CppType)));
    // TODO: For field flags/parameter flags?
    // myType->attrs;
    // TODO: Unclear if this is always the case
    myType->byref = 0;
    // TODO: Unclear if this is always the case
    myType->num_mods = 0;
    // TODO: Unclear if this is always the case
    myType->pinned = 0;
    // TODO: Change this for value types and other type enums
    myType->type = Il2CppTypeEnum::IL2CPP_TYPE_CLASS;
    // This should be a unique number, assigned when each new type is created.
    myType->data.klassIndex = klassIndex;
    return myType;
}

Il2CppClass* createMyClass(std::string_view nameSpace, std::string_view name, Il2CppClass* baseClass, const Il2CppImage* img) {
    auto myType = createMyType(-1 - customClasses.size());
    logger().info("Custom Type: %p", myType);
    if (baseClass) {
        // Init our base class so we can grab a type hierarchy from it
        il2cpp_functions::Class_Init(baseClass);
    }
    auto myClass = reinterpret_cast<Il2CppClass*>(calloc(1, sizeof(Il2CppClass)));
    // Fill in the values of the class
    // TODO: Create custom image
    // FOr now, just copy it
    myClass->image = img;
    myClass->parent = baseClass;
    // Setup type hierarchy
    if (myClass->parent) {
        myClass->typeHierarchyDepth = baseClass->typeHierarchyDepth + 1;
    } else {
        myClass->typeHierarchyDepth = 1;
    }
    myClass->typeHierarchy = reinterpret_cast<Il2CppClass**>(calloc(myClass->typeHierarchyDepth, sizeof(Il2CppClass*)));
    if (myClass->parent) {
        myClass->typeHierarchy[myClass->typeHierarchyDepth - 1] = myClass;
        memcpy(myClass->typeHierarchy, baseClass->typeHierarchy, baseClass->typeHierarchyDepth * sizeof(Il2CppClass*));
    } else {
        myClass->typeHierarchy[0] = myClass;
    }
    // TODO: Setup interfaces
    // This is for arrays, we can safely assume we are not an array so set this to ourselves.
    myClass->element_class = myClass;
    // TODO: Determine more information from this
    myClass->castClass = myClass;
    // Pointer to self
    myClass->klass = myClass;
    // Unless the class is used in native code, this isn't necessary
    myClass->native_size = -1;
    // Actual size of the class
    // TODO: This will change based off of fields, for now, we have no fields, so it should simply be baseClass size
    myClass->actualSize = baseClass->actualSize;  // "actualSize is instance_size for compiler generated values"
    myClass->instance_size = baseClass->instance_size;
    myClass->interfaces_count = 0;
    myClass->generic_class = nullptr;

    // Pretend that the class has already been initialized
    myClass->initialized = 1;
    myClass->initialized_and_no_error = 1;
    myClass->init_pending = 0;
    myClass->has_initialization_error = 0;
    myClass->size_inited = 1;
    // TODO: Type Definition
    myClass->typeDefinition = (Il2CppTypeDefinition*)0x12345678;
    // myClass->typeDefinition = reinterpret_cast<Il2CppTypeDefinition*>(calloc(1, sizeof(Il2CppTypeDefinition)));
    myClass->has_finalize = 0;
    myClass->is_vtable_initialized = 1;
    // TODO: For value types: myClass->valuetype
    // Set name
    myClass->name = name.data();
    myClass->namespaze = nameSpace.data();

    myClass->this_arg = *myType;
    // TODO: Modify this for future usage where byval argument may not match this argument
    myClass->byval_arg = *myType;
    // TODO: Modify flags in the future
    // For now, copy them from base class
    myClass->flags = baseClass->flags;
    // Method creation should happen around now
    // TODO: Modify vtables in future, instead of copying from base vtable
    myClass->vtable_count = baseClass->vtable_count;
    for (auto i = 0; i < baseClass->vtable_count; i++) {
        myClass->vtable[i] = baseClass->vtable[i];
    }
    // TODO: Support generic class creation
    myClass->genericContainerIndex = kGenericContainerIndexInvalid;
    // TODO: is this valid?
    myClass->token = baseClass->token;
    // logger().debug("Calling Class::SetupGCDescriptor");
    // Class_SetupGCDescriptor(myClass);
    // logger().debug("Finished calling SetupGCDescriptor!");
    // TODO: Is this always 8?
    myClass->minimumAlignment = 8;
    myClass->naturalAligment = 8;
    // TODO: Is this always 1?
    myClass->genericRecursionDepth = 1;
    myClass->thread_static_fields_offset = -1;
    // I hope I don't have to set this...
    myClass->unity_user_data = nullptr;
    // I have references to UnityEngine::MonoBehaviour (different namespace)
    myClass->has_references = 1;
    customClasses.push_back(myClass);

    return myClass;
}

MethodInfo* createFinalizeMethod() {
    auto info = reinterpret_cast<MethodInfo*>(calloc(1, sizeof(MethodInfo)));
    // info->methodPointer = (Il2CppMethodPointer)finalizeFunction;
    info->invoker_method = NULL;
    info->parameters_count = 1;
    info->slot = kInvalidIl2CppMethodSlot;
    info->is_marshaled_from_native = true;
    info->flags |= METHOD_ATTRIBUTE_STATIC | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
    info->name = "Finalize";
    return info;
}

void createMethods(Il2CppClass* klass, std::vector<method_info_t> methods) {
    // Maybe we need to manually add a Finalize method?
    // Maybe also foricbly add a ctor? If it wasn't provided?
    // TODO: Check the above
    klass->method_count = methods.size();
    klass->methods = reinterpret_cast<const MethodInfo**>(calloc(klass->method_count, sizeof(MethodInfo*)));
    // klass->methods[0] = createFinalizeMethod();
    for (auto i = 0; i < klass->method_count; i++) {
        auto method = methods[i];
        auto info = reinterpret_cast<MethodInfo*>(calloc(1, sizeof(MethodInfo)));
        info->return_type = method.returnType;
        info->methodPointer = (Il2CppMethodPointer)method.function;
        info->invoker_method = reinterpret_cast<InvokerMethod>(method.invoker_function);
        info->parameters_count = method.parameterCount;
        info->parameters = method.parameters;
        info->slot = kInvalidIl2CppMethodSlot;
        auto def = reinterpret_cast<Il2CppMethodDefinition*>(calloc(1, sizeof(Il2CppMethodDefinition)));
        // TODO: Handle the method definition somehow
        info->methodDefinition = def;
        info->flags = method.flags;
        info->name = method.name;
        info->klass = klass;
        // TODO: Populate other fields as necessary
        klass->methods[i] = info;
    }
}

void createUpdateMethod(std::vector<method_info_t>& methods, void* invoker) {
    method_info_t methodInfo;
    methodInfo.name = "Update";
    methodInfo.invoker_function = invoker;
    methodInfo.parameterCount = 0;
    RET_V_UNLESS(il2cpp_functions::defaults->void_class);
    methodInfo.returnType = const_cast<Il2CppType*>(il2cpp_functions::class_get_type(il2cpp_functions::defaults->void_class));
    methodInfo.parameters = reinterpret_cast<ParameterInfo*>(calloc(1, sizeof(ParameterInfo)));;
    methodInfo.flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
    methodInfo.function = (void*)UpdateFunction;
    methods.push_back(methodInfo);
}

void createAwakeMethod(std::vector<method_info_t>& methods, void* invoker) {
    method_info_t methodInfo;
    methodInfo.name = "Awake";
    methodInfo.invoker_function = invoker;
    methodInfo.parameterCount = 0;
    RET_V_UNLESS(il2cpp_functions::defaults->void_class);
    methodInfo.returnType = const_cast<Il2CppType*>(il2cpp_functions::class_get_type(il2cpp_functions::defaults->void_class));
    methodInfo.parameters = reinterpret_cast<ParameterInfo*>(calloc(1, sizeof(ParameterInfo)));
    methodInfo.flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
    methodInfo.function = (void*)AwakeFunction;
    methods.push_back(methodInfo);
}

void createStartMethod(std::vector<method_info_t>& methods, void* invoker) {
    method_info_t methodInfo;
    methodInfo.name = "Start";
    methodInfo.invoker_function = invoker;
    methodInfo.parameterCount = 0;
    RET_V_UNLESS(il2cpp_functions::defaults->void_class);
    methodInfo.returnType = const_cast<Il2CppType*>(il2cpp_functions::class_get_type(il2cpp_functions::defaults->void_class));
    methodInfo.parameters = reinterpret_cast<ParameterInfo*>(calloc(1, sizeof(ParameterInfo)));;
    methodInfo.flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
    methodInfo.function = (void*)StartFunction;
    methods.push_back(methodInfo);
}

void createCtorMethod(std::vector<method_info_t>& methods, void* invoker) {
    method_info_t methodInfo;
    methodInfo.name = ".ctor";
    methodInfo.invoker_function = invoker;
    methodInfo.parameterCount = 0;
    // Constructors have void return type
    methodInfo.returnType = const_cast<Il2CppType*>(il2cpp_functions::class_get_type(il2cpp_functions::defaults->void_class));
    methodInfo.parameters = reinterpret_cast<ParameterInfo*>(calloc(1, sizeof(ParameterInfo)));
    methodInfo.flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_SPECIAL_NAME;
    methodInfo.function = (void*)Ctor;
    methods.push_back(methodInfo);
}

static bool crashNow = false;
MAKE_HOOK(FromIl2CppType, NULL, Il2CppClass*, Il2CppType* typ) {
    // Runtime generated class
    // logger().debug("FromIl2CppType called, type: %p", typ);
    bool shouldBeOurs = false;
    // klassIndex is only meaningful for these types
    if ((typ->type == IL2CPP_TYPE_CLASS || typ->type == IL2CPP_TYPE_VALUETYPE) && typ->data.klassIndex < 0) {
        shouldBeOurs = true;
        // If the type matches our type
        // TODO: Add a map for created classes here
        auto idx = -1 - typ->data.klassIndex;
        if (idx < customClasses.size()) {
            logger().debug("Returning custom class with idx %i!", idx);
            CRASH_UNLESS(!crashNow);
            return customClasses[idx];
        }
    }
    // Otherwise, return orig
    auto klass = FromIl2CppType(typ);
    if (shouldBeOurs) {
        logger().debug("FromIl2CppType called with klassIndex %i which is not our custom type?!", typ->data.klassIndex);
        il2cpp_utils::LogClass(klass, false);
    }
    // logger().debug("Class: %p", klass);
    return klass;
}

// MAKE_HOOK(SetupGCDescriptor, NULL, void, Il2CppClass* klass) {
//     logger().debug("SetupGCDescriptor: %p", klass);
//     return SetupGCDescriptor(klass);
// }

MAKE_HOOK(IsSubclassOf, NULL, bool, Il2CppClass *klass, Il2CppClass *klassc, bool check_interfaces) {
    // logger().debug("%s is subclass of: %s", klass->name, klassc->name);
    if (klassc->byval_arg.data.klassIndex < 0 || klass == il2cpp_utils::GetClassFromName("UnityEngine", "Object")) {
        // logger().debug("IsSubclassOf: %p, %p, %i", klass, klassc, check_interfaces);
        // il2cpp_utils::LogClass(klass);
        // il2cpp_utils::LogClass(klassc);
    }
    return IsSubclassOf(klass, klassc, check_interfaces);
}

MAKE_HOOK(MetadataCache_GetTypeDefinitionFromIndex, NULL, const Il2CppTypeDefinition*, TypeDefinitionIndex index) {
    auto ret = MetadataCache_GetTypeDefinitionFromIndex(index);
    logger().debug("MetadataCache_GetTypeDefinitionFromIndex: %p from %d", ret, index);
    return ret;
}

MAKE_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, void, Il2CppObject* self, bool firstActivation, int activationType) {
    MainMenuViewController_DidActivate(self, firstActivation, activationType);
    logger().debug("Getting GO...");
    auto* go = RET_V_UNLESS(il2cpp_utils::GetPropertyValue(self, "gameObject").value_or(nullptr));
    logger().debug("Got GO: %p", go);
    auto myClass = customClasses[0];
    auto* customType = il2cpp_utils::GetSystemType(myClass);
    logger().debug("Custom System.Type: %p", customType);
    auto* strType = RET_V_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(customType, "ToString"));
    logger().debug("ToString: %s", to_utf8(csstrtostr(strType)).data());
    auto* name = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<Il2CppString*>(customType, "Name"));
    logger().debug("Name: %s", to_utf8(csstrtostr(name)).c_str());
    logger().debug("Actual type: %p", &myClass->byval_arg);
    logger().debug("Type: %p", customType->type);
    // Force call Class::Init
    il2cpp_functions::Class_Init(il2cpp_utils::GetClassFromName("", "AudioClipQueue"));
    logAll(il2cpp_utils::GetClassFromName("", "AudioClipQueue"));
    logAll(il2cpp_utils::GetClassFromName("UnityEngine", "MonoBehaviour"));
    // crashNow = true;
    // il2cpp_functions::CheckS_GlobalMetadata();
    // customType = il2cpp_utils::GetSystemType(il2cpp_functions::defaults->string_class);
    auto* comp = RET_V_UNLESS(il2cpp_utils::RunMethod(go, "AddComponent", customType));
    logger().debug("Custom Type added as a component: %p", comp);
}

// This function is called once il2cpp_init has been called, so il2cpp_utils and il2cpp_functions can be used safely here.
// This is where OFFSETLESS hooks must be installed.
extern "C" void load() {
    il2cpp_functions::Init();

    Instruction classInitInst(CRASH_UNLESS((int32_t*)il2cpp_functions::Class_Init));
    auto j2Cl_IL = CRASH_UNLESS(classInitInst.findNthCall(2));
    Instruction Class_InitLocked(CRASH_UNLESS(j2Cl_IL->label));
    logger().debug("Class::InitLocked offset: %lX", ((intptr_t)Class_InitLocked.addr) - getRealOffset(0));
    auto gcptr = CRASH_UNLESS(Class_InitLocked.findNthCall(15, -1)); // 15th bl in InitLocked
    Class_SetupGCDescriptor = (decltype(Class_SetupGCDescriptor))CRASH_UNLESS(gcptr->label);
    logger().debug("Class::SetupGCDescriptor found? offset: %lX", ((intptr_t)Class_SetupGCDescriptor) - getRealOffset(0));

    Instruction classIsSubclassOfApiFunc(CRASH_UNLESS((int32_t*)il2cpp_functions::class_is_subclass_of));
    auto j2Cl_ISO = CRASH_UNLESS(classInitInst.findNthDirectBranchWithoutLink(1));
    Class_IsSubclassOf = (decltype(Class_IsSubclassOf))CRASH_UNLESS(j2Cl_ISO->label);

    // INSTALL_HOOK_DIRECT(SetupGCDescriptor, (void*)Class_SetupGCDescriptor);
    // INSTALL_HOOK_DIRECT(IsSubclassOf, (void*)Class_IsSubclassOf);
    INSTALL_HOOK_DIRECT(FromIl2CppType, (void*)il2cpp_functions::Class_FromIl2CppType);
    INSTALL_HOOK_DIRECT(MetadataCache_GetTypeDefinitionFromIndex, (void*)il2cpp_functions::MetadataCache_GetTypeDefinitionFromIndex);
    // Create custom monobehaviour
    logger().info("Attempting to create custom monobehaviour!");
    auto img = createMyImage("CustomIl2CppImage");  // il2cpp_utils::GetClassFromName("", "AudioClipQueue")->image
    auto a = createMyAssembly(img, "CustomIl2CppAssembly");
    img->assembly = a;
    // TODO: Any more image setup?
    // TODO: Use type in creation of class
    static auto monoBehaviourClass = il2cpp_utils::GetClassFromName("UnityEngine", "MonoBehaviour");
    auto myClass = createMyClass("CustomIl2CppNamespace", "CustomType", monoBehaviourClass, img);
    logger().info("Custom Class: %p", myClass);
    logger().info("Custom Class name: %s", myClass->name);
    // Create methods
    std::vector<method_info_t> methods;
    // TODO: Create methods
    // createUpdateMethod(methods);
    // createStartMethod(methods);
    // createCtorMethod(methods);
    // TODO: See if we need an invoker function.
    // Invoker function seems to be the same for all functions with same size parameters
    // We could grab it from our base type like so:
    auto zeroArgInvoker = (void*)il2cpp_utils::FindMethod("", "AudioClipQueue", "Awake")->invoker_method;
    logger().info("Invoker method: %p", zeroArgInvoker);
    createAwakeMethod(methods, zeroArgInvoker);
    createCtorMethod(methods, zeroArgInvoker);
    createStartMethod(methods, zeroArgInvoker);
    createUpdateMethod(methods, zeroArgInvoker);
    createMethods(myClass, methods);
    // Type should now be completely valid, and created!
    logger().info("Created custom monobehaviour!");
    // Now, we need to actually USE the new type somewhere. Preferrably, we make a new one via a call to AddComponent
    // TODO: Check to ensure that this doesn't invalidate our class/break il2cpp (it does)
    logAll(myClass);
    logAll(il2cpp_utils::GetClassFromName("", "AudioClipQueue"));
    logAll(monoBehaviourClass);
    INSTALL_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 2));
}
