#include "main.hpp"
#include "macros.hpp"

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

// Custom invoker function
void* invoke(Il2CppMethodPointer ptr, [[maybe_unused]] const MethodInfo* m, void* obj, void** args) {
    auto method = reinterpret_cast<void (*)(void*)>(ptr);
    method(obj);
    return nullptr;
}

DECLARE_CLASS(Il2CppNamespace, MyType, "UnityEngine", "MonoBehaviour",
    DECLARE_METHOD(void, Start);
    REGISTER_FUNCTION(MyType,
        REGISTER_METHOD(Start);
    )
)

void Il2CppNamespace::MyType::Start() {
    modLogger().info("Cool start called! Instance: %p", this);
}

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

custom_types::ClassWrapper& createMyClass(std::string_view nameSpace, std::string_view name, Il2CppClass* baseClass, const Il2CppImage* img) {
    type_info* info = new type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, nameSpace, name, baseClass);
    auto& wrapper = custom_types::Register::classes.emplace_back(info);
    wrapper.setupTypeHierarchy(baseClass);
    return wrapper;
}

void createMethods(custom_types::ClassWrapper& wrapper, std::vector<method_info*> methods) {
    // Maybe we need to manually add a Finalize method?
    // Maybe also foricbly add a ctor? If it wasn't provided?
    // TODO: Check the above
    wrapper.methods = methods;
    wrapper.populateMethods();
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
    // INSTALL_HOOK_DIRECT(SetupGCDescriptor, (void*)Class_SetupGCDescriptor);
    // INSTALL_HOOK_DIRECT(IsSubclassOf, (void*)Class_IsSubclassOf);
    // Create custom monobehaviour
    modLogger().info("Attempting to create custom monobehaviour!");
    // TODO: Any more image setup?
    // TODO: Use type in creation of class
    // static auto monoBehaviourClass = il2cpp_utils::GetClassFromName("UnityEngine", "MonoBehaviour");
    // auto& myClass = createMyClass("CustomIl2CppNamespace", "CustomType", monoBehaviourClass, img);
    // modLogger().info("Custom Class: %p", myClass.get());
    // modLogger().info("Custom Class name: %s", myClass->name);
    // Create methods
    // std::vector<method_info*> methods;
    // TODO: Create methods
    // createUpdateMethod(methods);
    // createStartMethod(methods);
    // createCtorMethod(methods);
    // TODO: See if we need an invoker function.
    // Invoker function seems to be the same for all functions with same size parameters
    // We could grab it from our base type like so:
    // auto zeroArgInvoker = (void*)il2cpp_utils::FindMethod("", "AudioClipQueue", "Awake")->invoker_method;
    // Create an invoker method
    // modLogger().info("Invoker method: %p", invoke);
    // createAwakeMethod(methods, (void*)invoke);
    // createCtorMethod(methods, (void*)invoke);
    // createStartMethod(methods, (void*)invoke);
    // createUpdateMethod(methods, (void*)invoke);
    // createMethods(myClass, methods);
    // Type should now be completely valid, and created!
    modLogger().info("Created custom monobehaviour!");
    // Create second one!
    auto myClass = custom_types::Register::RegisterType<Il2CppNamespace::MyType>();
    // Now, we need to actually USE the new type somewhere. Preferrably, we make a new one via a call to AddComponent
    // TODO: Check to ensure that this doesn't invalidate our class/break il2cpp (it does)
    logAll(myClass);
    // logAll(il2cpp_utils::GetClassFromName("", "AudioClipQueue"));
    // logAll(monoBehaviourClass);
    INSTALL_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 2));
}
