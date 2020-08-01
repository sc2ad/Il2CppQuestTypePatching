#include "main.hpp"
#include "modloader/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

ModInfo modInfo;

const Logger& modLogger() {
    static const Logger myLogger(modInfo);
    return myLogger;
}


DECLARE_CLASS(Il2CppNamespace, MyType, "UnityEngine", "MonoBehaviour",
    // DECLARE_INSTANCE_FIELD(int, x);
    // DECLARE_INSTANCE_FIELD(Vector3, y);

    // DECLARE_METHOD(void, Start);

    // DECLARE_METHOD(static void, Test, int x, float y);

    // At the moment, it will not function if primitive/non-pointer return types are returned
    // For now, please stick with `void` or `Il2CppObject*` and allocate them yourself before returning.
    // DECLARE_METHOD(int, asdf, int q);

    // DECLARE_METHOD(Il2CppObject*, asdf, int z);

    REGISTER_FUNCTION(MyType,
        modLogger().debug("Registering MyType!");
        // REGISTER_FIELD(x);
        // REGISTER_FIELD(y);
        // REGISTER_METHOD(Start);
        // REGISTER_METHOD(Test);
        // REGISTER_METHOD(asdf);
        // REGISTER_METHOD(asdf);
    )
)

// void Il2CppNamespace::MyType::Start() {
//     modLogger().debug("Called Il2CppNamespace::MyType::Start!");
// }

// int Il2CppNamespace::MyType::asdf(int q) {
    
// }

// Il2CppObject* Il2CppNamespace::MyType::asdf(int z) {
//     modLogger().debug("Called Il2CppNamespace::MyType::asdf!");
//     // We want to return a correctly allocated il2cpp object.
//     // To do this, we would probably call New
//     // For now, lets just return nullptr since no one calls this method anyways
//     return nullptr;
// }

// void Il2CppNamespace::MyType::Test(int x, float y) {
//     modLogger().debug("Called Il2CppNamespace::MyType::Test!");
// }

static Il2CppClass* klass;

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
    modLogger().debug("Completed setup!");
}

MAKE_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, void, Il2CppObject* self, bool firstActivation, int activationType) {
    MainMenuViewController_DidActivate(self, firstActivation, activationType);
    logger().debug("Getting GO...");
    auto* go = RET_V_UNLESS(il2cpp_utils::GetPropertyValue(self, "gameObject").value_or(nullptr));
    logger().debug("Got GO: %p", go);
    auto* customType = il2cpp_utils::GetSystemType(klass);
    logger().debug("Custom System.Type: %p", customType);
    auto* strType = RET_V_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(customType, "ToString"));
    logger().debug("ToString: %s", to_utf8(csstrtostr(strType)).data());
    auto* name = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<Il2CppString*>(customType, "Name"));
    logger().debug("Name: %s", to_utf8(csstrtostr(name)).c_str());
    logger().debug("Actual type: %p", &klass->byval_arg);
    logger().debug("Type: %p", customType->type);
    // crashNow = true;
    auto* comp = RET_V_UNLESS(il2cpp_utils::RunMethod(go, "AddComponent", customType));
    logger().debug("Custom Type added as a component: %p", comp);
}

extern "C" void load() {
    modLogger().debug("Registering type!");
    klass = custom_types::Register::RegisterType<::Il2CppNamespace::MyType>();
    INSTALL_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 2));
    // logAll(k);
    // logAll(k->parent);
    modLogger().debug("Custom types size: %u", custom_types::Register::classes.size());
}