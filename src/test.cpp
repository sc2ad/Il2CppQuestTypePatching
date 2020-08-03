// If LOCAL_TEST is defined, create a mod that uses custom types.
// Otherwise, it will be built as a library.
#ifdef LOCAL_TEST
#include "register.hpp"
#include "macros.hpp"
#include "modloader/modloader.hpp"
#include "beatsaber-hook/utils/logging.hpp"
#include "beatsaber-hook/utils/utils.h"
#include "beatsaber-hook/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/utils/il2cpp-utils.hpp"

ModInfo modInfo;

const Logger& modLogger() {
    static const Logger myLogger(modInfo);
    return myLogger;
}


DECLARE_CLASS(Il2CppNamespace, MyType, "UnityEngine", "MonoBehaviour",
    // DECLARE_INSTANCE_FIELD(int, x);
    // DECLARE_INSTANCE_FIELD(Vector3, y);

    DECLARE_METHOD(void, Start);

    DECLARE_METHOD(static void, Test, int x, float y);

    DECLARE_METHOD(int, asdf, int q);

    // DECLARE_METHOD(Il2CppObject*, asdf, int z);

    REGISTER_FUNCTION(MyType,
        modLogger().debug("Registering MyType!");
        // REGISTER_FIELD(x);
        // REGISTER_FIELD(y);
        REGISTER_METHOD(Start);
        REGISTER_METHOD(Test);
        REGISTER_METHOD(asdf);
        // REGISTER_METHOD(asdf);
    )
)

void Il2CppNamespace::MyType::Start() {
    modLogger().debug("Called Il2CppNamespace::MyType::Start!");
    modLogger().debug("Return of asdf(1): %u", asdf(1));
    // Runtime invoke it.
    // We ARE NOT able to call GetClassFromName.
    // This is because our class name is NOT in the nameToClassHashTable
    // However, we ARE able to get our Il2CppClass* via the klass private static field of this type.
    modLogger().debug("klass: %p", klass);
}

int Il2CppNamespace::MyType::asdf(int q) {
    return q + 1;
}

// Il2CppObject* Il2CppNamespace::MyType::asdf(int z) {
//     modLogger().debug("Called Il2CppNamespace::MyType::asdf!");
//     // We want to return a correctly allocated il2cpp object.
//     // To do this, we would probably call New
//     // For now, lets just return nullptr since no one calls this method anyways
//     return nullptr;
// }

void Il2CppNamespace::MyType::Test(int x, float y) {
    modLogger().debug("Called Il2CppNamespace::MyType::Test!");
}

static Il2CppClass* klass;

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
    modLogger().debug("Completed setup!");
}

MAKE_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, void, Il2CppObject* self, bool firstActivation, int activationType) {
    MainMenuViewController_DidActivate(self, firstActivation, activationType);
    modLogger().debug("Getting GO...");
    auto* go = RET_V_UNLESS(il2cpp_utils::GetPropertyValue(self, "gameObject").value_or(nullptr));
    modLogger().debug("Got GO: %p", go);
    auto* customType = il2cpp_utils::GetSystemType(klass);
    modLogger().debug("Custom System.Type: %p", customType);
    auto* strType = RET_V_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(customType, "ToString"));
    modLogger().debug("ToString: %s", to_utf8(csstrtostr(strType)).data());
    auto* name = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<Il2CppString*>(customType, "Name"));
    modLogger().debug("Name: %s", to_utf8(csstrtostr(name)).c_str());
    modLogger().debug("Actual type: %p", &klass->byval_arg);
    modLogger().debug("Type: %p", customType->type);
    // crashNow = true;
    auto* comp = RET_V_UNLESS(il2cpp_utils::RunMethod(go, "AddComponent", customType));
    modLogger().debug("Custom Type added as a component: %p", comp);
}

extern "C" void load() {
    modLogger().debug("Registering type!");
    klass = custom_types::Register::RegisterType<::Il2CppNamespace::MyType>();
    INSTALL_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 2));
    // logAll(k);
    // logAll(k->parent);
    modLogger().debug("Custom types size: %u", custom_types::Register::classes.size());
}
#endif