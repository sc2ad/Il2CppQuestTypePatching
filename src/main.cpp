#include "main.hpp"

#include "extern/modloader/modloader.hpp"

#include "extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "extern/beatsaber-hook/shared/utils/instruction-parsing.hpp"
#include "extern/beatsaber-hook/shared/utils/typedefs.h"
#include "extern/beatsaber-hook/shared/utils/logging.hpp"

#include "logging.hpp"

#include <string_view>
#include <vector>

inline static void (*Class_SetupGCDescriptor)(Il2CppClass* klass);
inline static bool (*Class_IsSubclassOf)(Il2CppClass *klass, Il2CppClass *klassc, bool check_interfaces);

ModInfo modInfo;
const Logger& modLogger()
{
    static const Logger& logger(modInfo);
    return logger;
}

extern "C" void setup(ModInfo& info)
{
    info.id      = "TypePatching";
    info.version = "0.1.0";
    modInfo      = info;
    modLogger().info("Leaving setup!");
}

static Il2CppAssembly* myAsm;
static Il2CppImage* myImage;
static std::vector<Il2CppClass*> customClasses{};

// Custom Update function
void UpdateFunction(Il2CppObject* self) {
    modLogger().info("Called Update! Instance: %p", self);
}

// Custom Start function
void StartFunction(Il2CppObject* self) {
    modLogger().info("Called Start! Instance: %p", self);
}

// Custom Awake function
void AwakeFunction(Il2CppObject* self) {
    modLogger().info("Called Awake! Instance: %p", self);
}

// Custom ctor function
void Ctor(Il2CppObject* self) {
    modLogger().info("Called ctor! Instance: %p", self);
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

custom_types::ClassWrapper& createMyClass(std::string_view nameSpace, std::string_view name, Il2CppClass* baseClass, const Il2CppImage* img) {
    auto myType = createMyType(-1 - customClasses.size());
    modLogger().info("Custom Type: %p", myType);
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
    // modLogger().debug("Calling Class::SetupGCDescriptor");
    // Class_SetupGCDescriptor(myClass);
    // modLogger().debug("Finished calling SetupGCDescriptor!");
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
    type_info* info = new type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, nameSpace, name, baseClass);
    // custom_types::ClassWrapper wrapper(info);
    auto& wrapper = custom_types::Register::classes.emplace_back(info);
    wrapper.setupTypeHierarchy(baseClass);
    return wrapper;
}

void createMethods(custom_types::ClassWrapper& wrapper, std::vector<method_info*> methods) {
    // Maybe we need to manually add a Finalize method?
    // Maybe also foricbly add a ctor? If it wasn't provided?
    // TODO: Check the above
    // wrapper.methods = methods;
    // wrapper.populateMethods();
    auto klass = wrapper.klass;
    klass->method_count = methods.size();
    klass->methods = reinterpret_cast<const MethodInfo**>(calloc(klass->method_count, sizeof(MethodInfo*)));
    // klass->methods[0] = createFinalizeMethod();
    for (auto i = 0; i < klass->method_count; i++) {
        auto method = methods[i];
        auto info = const_cast<MethodInfo*>(method->get());
        auto def = reinterpret_cast<Il2CppMethodDefinition*>(calloc(1, sizeof(Il2CppMethodDefinition)));
        info->methodDefinition = def;
        info->klass = klass;
        // TODO: Handle the method definition somehow
        // TODO: Populate other fields as necessary
        klass->methods[i] = info;
    }
}

void createUpdateMethod(std::vector<method_info*>& methods, void* invoker) {
    RET_V_UNLESS(il2cpp_functions::defaults->void_class);
    auto ret = const_cast<Il2CppType*>(il2cpp_functions::class_get_type(il2cpp_functions::defaults->void_class));
    std::vector<ParameterInfo> params;
    uint16_t flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
    method_info* methodInfo = new method_info("Update", (void*)UpdateFunction, (InvokerMethod)invoker, ret, params, flags);
    methods.push_back(methodInfo);
}

void createAwakeMethod(std::vector<method_info*>& methods, void* invoker) {
    RET_V_UNLESS(il2cpp_functions::defaults->void_class);
    auto ret = const_cast<Il2CppType*>(il2cpp_functions::class_get_type(il2cpp_functions::defaults->void_class));
    std::vector<ParameterInfo> params;
    uint16_t flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
    method_info* methodInfo = new method_info("Awake", (void*)AwakeFunction, (InvokerMethod)invoker, ret, params, flags);
    methods.push_back(methodInfo);
}

void createStartMethod(std::vector<method_info*>& methods, void* invoker) {
    RET_V_UNLESS(il2cpp_functions::defaults->void_class);
    auto ret = const_cast<Il2CppType*>(il2cpp_functions::class_get_type(il2cpp_functions::defaults->void_class));
    std::vector<ParameterInfo> params;
    uint16_t flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
    method_info* methodInfo = new method_info("Start", (void*)StartFunction, (InvokerMethod)invoker, ret, params, flags);
    methods.push_back(methodInfo);
}

void createCtorMethod(std::vector<method_info*>& methods, void* invoker) {
    RET_V_UNLESS(il2cpp_functions::defaults->void_class);
    auto ret = const_cast<Il2CppType*>(il2cpp_functions::class_get_type(il2cpp_functions::defaults->void_class));
    std::vector<ParameterInfo> params;
    uint16_t flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_SPECIAL_NAME;
    method_info* methodInfo = new method_info(".ctor", (void*)Ctor, (InvokerMethod)invoker, ret, params, flags);
    methods.push_back(methodInfo);
}

MAKE_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, void, Il2CppObject* self, bool firstActivation, int activationType) {
    MainMenuViewController_DidActivate(self, firstActivation, activationType);
    modLogger().debug("Getting GO...");
    auto* go = RET_V_UNLESS(il2cpp_utils::GetPropertyValue(self, "gameObject").value_or(nullptr));
    modLogger().debug("Got GO: %p", go);
    auto myClass = custom_types::Register::classes[0].get();
    auto* customType = il2cpp_utils::GetSystemType(myClass);
    modLogger().debug("Custom System.Type: %p", customType);
    auto* strType = RET_V_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(customType, "ToString"));
    modLogger().debug("ToString: %s", to_utf8(csstrtostr(strType)).data());
    auto* name = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<Il2CppString*>(customType, "Name"));
    modLogger().debug("Name: %s", to_utf8(csstrtostr(name)).c_str());
    modLogger().debug("Actual type: %p", &myClass->byval_arg);
    modLogger().debug("Type: %p", customType->type);
    // Force call Class::Init
    il2cpp_functions::Class_Init(il2cpp_utils::GetClassFromName("", "AudioClipQueue"));
    logAll(il2cpp_utils::GetClassFromName("", "AudioClipQueue"));
    logAll(il2cpp_utils::GetClassFromName("UnityEngine", "MonoBehaviour"));
    // crashNow = true;
    // il2cpp_functions::CheckS_GlobalMetadata();
    // customType = il2cpp_utils::GetSystemType(il2cpp_functions::defaults->string_class);
    auto* comp = RET_V_UNLESS(il2cpp_utils::RunMethod(go, "AddComponent", customType));
    modLogger().debug("Custom Type added as a component: %p", comp);
}

// This function is called once il2cpp_init has been called, so il2cpp_utils and il2cpp_functions can be used safely here.
// This is where OFFSETLESS hooks must be installed.
extern "C" void load() {
    il2cpp_functions::Init();
    custom_types::Register::EnsureHooks();
    // INSTALL_HOOK_DIRECT(SetupGCDescriptor, (void*)Class_SetupGCDescriptor);
    // INSTALL_HOOK_DIRECT(IsSubclassOf, (void*)Class_IsSubclassOf);
    // Create custom monobehaviour
    modLogger().info("Attempting to create custom monobehaviour!");
    auto img = custom_types::Register::createImage("CustomIl2CppImage");  // il2cpp_utils::GetClassFromName("", "AudioClipQueue")->image
    auto a = custom_types::Register::createAssembly("CustomIl2CppAssembly", img);
    // TODO: Any more image setup?
    // TODO: Use type in creation of class
    static auto monoBehaviourClass = il2cpp_utils::GetClassFromName("UnityEngine", "MonoBehaviour");
    auto& myClass = createMyClass("CustomIl2CppNamespace", "CustomType", monoBehaviourClass, img);
    modLogger().info("Custom Class: %p", myClass.get());
    // modLogger().info("Custom Class name: %s", myClass->name);
    // Create methods
    std::vector<method_info*> methods;
    // TODO: Create methods
    // createUpdateMethod(methods);
    // createStartMethod(methods);
    // createCtorMethod(methods);
    // TODO: See if we need an invoker function.
    // Invoker function seems to be the same for all functions with same size parameters
    // We could grab it from our base type like so:
    auto zeroArgInvoker = (void*)il2cpp_utils::FindMethod("", "AudioClipQueue", "Awake")->invoker_method;
    modLogger().info("Invoker method: %p", zeroArgInvoker);
    createAwakeMethod(methods, zeroArgInvoker);
    createCtorMethod(methods, zeroArgInvoker);
    createStartMethod(methods, zeroArgInvoker);
    createUpdateMethod(methods, zeroArgInvoker);
    createMethods(myClass, methods);
    // Type should now be completely valid, and created!
    modLogger().info("Created custom monobehaviour!");
    // Now, we need to actually USE the new type somewhere. Preferrably, we make a new one via a call to AddComponent
    // TODO: Check to ensure that this doesn't invalidate our class/break il2cpp (it does)
    logAll((Il2CppClass*)myClass.get());
    logAll(il2cpp_utils::GetClassFromName("", "AudioClipQueue"));
    logAll(monoBehaviourClass);
    INSTALL_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 2));
}
