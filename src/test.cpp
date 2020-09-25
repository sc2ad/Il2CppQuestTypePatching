// If LOCAL_TEST is defined, create a mod that uses custom types.
// Otherwise, it will be built as a library.
#ifdef LOCAL_TEST
#include "register.hpp"
#include "macros.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "NoteData.hpp"
#include <unordered_map>

ModInfo modInfo;

const Logger& modLogger() {
    static const Logger myLogger(modInfo);
    return myLogger;
}

DECLARE_CLASS(Il2CppNamespace, MyType, "UnityEngine", "MonoBehaviour", sizeof(Il2CppObject) + sizeof(void*),
    // TODO: Fields need to be copied from base type, size needs to be adjusted to match, offsets of all declared fields need to be correct
    // DECLARE_INSTANCE_FIELD(int, x);
    // DECLARE_INSTANCE_FIELD(Vector3, y);

    DECLARE_CTOR(ctor);

    DECLARE_METHOD(void, Start);

    DECLARE_METHOD(static void, Test, int x, float y);

    DECLARE_METHOD(int, asdf, int q);

    DECLARE_INTERFACE_METHOD(Il2CppString*, ToString, il2cpp_utils::FindMethod("UnityEngine", "Object", "ToString"));

    REGISTER_FUNCTION(MyType,
        modLogger().debug("Registering MyType!");
        // REGISTER_FIELD(x);
        // REGISTER_FIELD(y);
        REGISTER_METHOD(ctor);
        REGISTER_METHOD(Start);
        REGISTER_METHOD(Test);
        REGISTER_METHOD(asdf);
        REGISTER_METHOD(ToString);
        // REGISTER_METHOD(asdf);
    )
)

void Il2CppNamespace::MyType::ctor() {
    modLogger().debug("Called Il2CppNamespace::MyType::ctor");
    // y = {1, 2, 3};
    // x = 5;
}

void Il2CppNamespace::MyType::Start() {
    modLogger().debug("Called Il2CppNamespace::MyType::Start!");
    modLogger().debug("Return of asdf(1): %u", asdf(1));
    // Runtime invoke it.
    // We ARE NOT able to call GetClassFromName.
    // This is because our class name is NOT in the nameToClassHashTable
    // However, we ARE able to get our Il2CppClass* via the klass private static field of this type.
    auto* il2cppKlass = il2cpp_utils::GetClassFromName("Il2CppNamespace", "MyType");
    modLogger().debug("il2cpp obtained klass: %p", il2cppKlass);
    modLogger().debug("klass: %p", klass);
    auto* asdfMethod = il2cpp_utils::FindMethodUnsafe(const_cast<Il2CppClass*>(klass), "asdf", 1);
    modLogger().debug("asdf method info: %p", asdfMethod);
    // modLogger().debug("x: %u", x);
    // modLogger().debug("y: (%f, %f, %f)", y.x, y.y, y.z);
    modLogger().debug("runtime invoke of asdf(1): %u", CRASH_UNLESS(il2cpp_utils::RunMethod<int>(this, asdfMethod, 1)));
}

int Il2CppNamespace::MyType::asdf(int q) {
    return q + 1;
}

Il2CppString* Il2CppNamespace::MyType::ToString() {
    // We want to create a C# string that is different than from what might otherwise be expected!
    modLogger().info("Calling custom ToString!");
    return il2cpp_utils::createcsstr("My Custom ToString!");
}

void Il2CppNamespace::MyType::Test(int x, float y) {
    modLogger().debug("Called Il2CppNamespace::MyType::Test!");
}

DECLARE_CLASS_INTERFACES(Il2CppNamespace, MyCustomBeatmapLevelPackCollection, "System", "Object", sizeof(Il2CppObject),
    il2cpp_utils::GetClassFromName("", "IBeatmapLevelPackCollection"),
    DECLARE_INSTANCE_FIELD(Il2CppArray*, wrappedArr);
    
    DECLARE_INTERFACE_METHOD(Il2CppArray*, get_beatmapLevelPacks, il2cpp_utils::FindMethod("", "IBeatmapLevelPackCollection", "get_beatmapLevelPacks"));
    DECLARE_CTOR(ctor, Il2CppArray* originalArray);
    REGISTER_FUNCTION(MyCustomBeatmapLevelPackCollection,
        modLogger().debug("Registering MyCustomBeatmapLevelPackCollection!");
        REGISTER_METHOD(get_beatmapLevelPacks);
        REGISTER_METHOD(ctor);
        REGISTER_FIELD(wrappedArr);
    )
)

void Il2CppNamespace::MyCustomBeatmapLevelPackCollection::ctor(Il2CppArray* originalArray) {
    // We want to basically wrap the original instance.
    // Also log.
    wrappedArr = originalArray;
    modLogger().debug("Added original array: %p", originalArray);
}

Il2CppArray* Il2CppNamespace::MyCustomBeatmapLevelPackCollection::get_beatmapLevelPacks() {
    modLogger().debug("My cool getter wrappedArr: %p!", wrappedArr);
    return wrappedArr;
}

// DECLARE_CLASS_INTERFACES(Il2CppNamespace, MyBeatmapObjectManager, "", "BeatmapObjectManager",
//     il2cpp_utils::GetClassFromName("", "IBeatmapObjectSpawner"),
//     DECLARE_CTOR(ctor);
//     DECLARE_INTERFACE_METHOD(void, SpawnBasicNote, il2cpp_utils::FindMethodUnsafe("", "IBeatmapObjectSpawner", "SpawnBasicNote", 11),
//     NoteData* noteData, Vector3 moveStartPos, Vector3 moveEndPos, Vector3 jumpEndPos, float moveDuration, float jumpDuration, float jumpGravity, float rotation, bool disappearingArrow, bool ghostNote, float cutDirectionAngleOffset);
//     REGISTER_FUNCTION(MyBeatmapObjectManager,
//         modLogger().debug("Registering MyBeatmapObjectManager!");
//         REGISTER_METHOD(ctor);
//         REGISTER_METHOD(SpawnBasicNote);
//     )
//     public:
//     static inline Il2CppObject* actualBeatmapObjectManager;
//     static inline void setActual(Il2CppObject* beatmapObjectManager) {
//         actualBeatmapObjectManager = actualBeatmapObjectManager;
//     }
// )

// void Il2CppNamespace::MyBeatmapObjectManager::ctor() {
//     modLogger().debug("Called Il2CppNamespace::MyBeatmapObjectManager::ctor");
// }

// void Il2CppNamespace::MyBeatmapObjectManager::SpawnBasicNote(NoteData* noteData, Vector3 moveStartPos, Vector3 moveEndPos, Vector3 jumpEndPos, float moveDuration, float jumpDuration, float jumpGravity, float rotation, bool disappearingArrow, bool ghostNote, float cutDirectionAngleOffset) {
//     modLogger().debug("Called Il2CppNamespace::MyBeatmapObjectManager::SpawnBasicNote, calling orig now!");
//     auto* method = il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectManager", "SpawnBasicNote", 11);
//     il2cpp_utils::RunMethod(actualBeatmapObjectManager, method, noteData, moveStartPos, moveEndPos, jumpEndPos, moveDuration, jumpDuration, jumpGravity, rotation);
// }

static custom_types::ClassWrapper* klassWrapper;

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
    auto* customType = il2cpp_utils::GetSystemType(klassWrapper->get());
    modLogger().debug("Custom System.Type: %p", customType);
    auto* strType = RET_V_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(customType, "ToString"));
    modLogger().debug("ToString: %s", to_utf8(csstrtostr(strType)).data());
    auto* name = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<Il2CppString*>(customType, "Name"));
    modLogger().debug("Name: %s", to_utf8(csstrtostr(name)).c_str());
    modLogger().debug("Actual type: %p", &klassWrapper->get()->byval_arg);
    modLogger().debug("Type: %p", customType->type);
    // crashNow = true;
    auto* comp = RET_V_UNLESS(il2cpp_utils::RunMethod(go, "AddComponent", customType));
    modLogger().debug("Custom Type added as a component: %p", comp);
}

// static bool first = false;
// MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_SpawnNote, void, Il2CppObject* self, Il2CppObject* noteData, float cutDirAngle) {
//     if (!first) {
//         first = true;
//         // We log, set the interface field, log, and then call orig.
//         modLogger().debug("Calling BeatmapObjectSpawnController.SpawnNote!");
//         auto* created = CRASH_UNLESS(il2cpp_utils::New<Il2CppNamespace::MyBeatmapObjectManager*>("Il2CppNamespace", "MyBeatmapObjectManager"));
//         modLogger().debug("Created MyBeatmapObjectManager: %p!", created);
//         auto* old = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_beatmapObjectSpawner"));
//         modLogger().debug("Got old: %p!", old);
//         Il2CppNamespace::MyBeatmapObjectManager::setActual(old);
//         modLogger().debug("Set actual to old!");
//         il2cpp_utils::SetFieldValue(self, "_beatmapObjectSpawner", created);
//         modLogger().debug("Set current to MyBeatmapObjectManager: %p!", created);
//         // Call orig
//         BeatmapObjectSpawnController_SpawnNote(self, noteData, cutDirAngle);
//         modLogger().debug("Called orig!");
//         il2cpp_utils::SetFieldValue(self, "_beatmapObjectSpawner", old);
//         // Then, we set the field back.
//         modLogger().debug("Set field back to old: %p", old);
//     } else {
//         BeatmapObjectSpawnController_SpawnNote(self, noteData, cutDirAngle);
//     }
// }

MAKE_HOOK_OFFSETLESS(BeatmapLevelModels_UpdateAllLoadedBeatmapLevelPacks, void, Il2CppObject* self) {
    BeatmapLevelModels_UpdateAllLoadedBeatmapLevelPacks(self);
    auto* existing = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_allLoadedBeatmapLevelPackCollection"));
    modLogger().debug("Existing: %p", existing);
    auto* arr = CRASH_UNLESS(il2cpp_utils::GetPropertyValue(existing, "beatmapLevelPacks"));
    modLogger().debug("Existing arr: %p", arr);
    modLogger().debug("Constructing custom type and setting it to field!");
    auto* myType = CRASH_UNLESS(il2cpp_utils::New<Il2CppNamespace::MyCustomBeatmapLevelPackCollection*>(arr));
    modLogger().debug("Created new type: %p", myType);
    auto* k = il2cpp_functions::object_get_class(existing);
    custom_types::logAll(k);
    k = il2cpp_functions::object_get_class((Il2CppObject*)myType);
    custom_types::logAll(k);
    CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_allLoadedBeatmapLevelPackCollection", myType));
}

extern "C" void load() {
    modLogger().debug("Registering types!");
    klassWrapper = CRASH_UNLESS(custom_types::Register::RegisterType<::Il2CppNamespace::MyType>());
    INSTALL_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 2));
    CRASH_UNLESS(custom_types::Register::RegisterType<::Il2CppNamespace::MyCustomBeatmapLevelPackCollection>());
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelModels_UpdateAllLoadedBeatmapLevelPacks, il2cpp_utils::FindMethod("", "BeatmapLevelsModel", "UpdateAllLoadedBeatmapLevelPacks"));
    // auto k = CRASH_UNLESS(custom_types::Register::RegisterType<Il2CppNamespace::MyBeatmapObjectManager>());
    // INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnController_SpawnNote, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController", "SpawnNote", 2));
    // il2cpp_utils::LogClass(il2cpp_utils::GetClassFromName("Il2CppNamespace", "MyBeatmapObjectManager"));
    // modLogger().debug("Vtables for MyBeatmapObjectManager: %u", k->get()->vtable_count);
    modLogger().debug("Custom types size: %u", custom_types::Register::classes.size());
    modLogger().debug("Logging vtables for custom type! There are: %u vtables", klassWrapper->get()->vtable_count);
    il2cpp_utils::LogClass(const_cast<Il2CppClass*>(klassWrapper->get()));
    // for (int i = 0; i < k->get()->vtable_count; i++) {
    //     custom_types::logVtable(&k->get()->vtable[i]);
    // }

    // modLogger().debug("Vtables for parent: %u", k->get()->parent->vtable_count);
    // for (int i = 0; i < k->get()->parent->vtable_count; i++) {
    //     custom_types::logVtable(&k->get()->parent->vtable[i]);
    // }
    // custom_types::logMethod(il2cpp_utils::FindMethodUnsafe("", "IBeatmapObjectSpawner", "SpawnBasicNote", 11));
}
#endif