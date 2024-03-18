#include <chrono>
#ifdef LOCAL_TEST_COROUTINE
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "coroutine.hpp"
#include "register.hpp"

using namespace custom_types::Helpers;

modloader::ModInfo modInfo{ MOD_ID"-test", VERSION, VERSION_LONG };

Logger& modLogger() {
    static auto myLogger = new Logger(modInfo, LoggerOptions(false, true));
    return *myLogger;
}

Coroutine testNestedCoro(int n) {
    modLogger().debug("begin nested coro");
    co_yield nullptr;
    // modLogger().debug("one step nested coro");
    // This will not be deleted by GC, as it will be stored as a field in the coro when invoked.
    co_yield reinterpret_cast<enumeratorT>(CRASH_UNLESS(il2cpp_utils::New("UnityEngine", "WaitForSecondsRealtime", 2.3f)));
    modLogger().debug("yielding until: {}", n);
    for (int i = 0; i < n; i++) {
        modLogger().debug("nested coro yield: {}", i);
        co_yield nullptr;
    }
    co_return;
    modLogger().debug("nested coroutine complete");
}

Coroutine testRecursiveCall() {
    modLogger().debug("Recursive call start!");
    auto instance = CoroutineHelper::New<il2cpp_utils::CreationType::Manual>(testNestedCoro, 3);
    modLogger().debug("Recursive call yield 1 {}!", fmt::ptr(instance.convert()));
    co_yield instance;
    // Reset instance and try again
    modLogger().debug("Recursive yield reset!");
    instance->Reset();

    modLogger().debug("Recursive call yield 2! {}", fmt::ptr(instance.convert()));
    co_yield instance;

    modLogger().debug("all done");
    co_return;
}

Coroutine testWaitForSeconds() {
    modLogger().debug("About to wait!");
    auto start = std::chrono::system_clock::now();
    co_yield reinterpret_cast<enumeratorT>(CRASH_UNLESS(il2cpp_utils::New("UnityEngine", "WaitForSecondsRealtime", 2.3f)));
    auto end = std::chrono::system_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    modLogger().debug("I have performed a wait! {}ms", duration);
    co_return;
}

CUSTOM_TYPES_FUNC void setup(CModInfo* info) {
    *info = modInfo.to_c();
    modLogger().debug("Completed setup!");
}

MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(MainMenuViewController_DidActivate, "", "MainMenuViewController", "DidActivate", void, Il2CppObject* self, bool firstActivation, bool addedToHierarchy,
                                     bool screenSystemEnabling) {
    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    modLogger().debug("Starting coroutine");

    il2cpp_utils::RunMethod(self, "StartCoroutine", custom_types::Helpers::CoroutineHelper::New(testWaitForSeconds()));
    il2cpp_utils::RunMethod(self, "StartCoroutine", custom_types::Helpers::CoroutineHelper::New(testRecursiveCall()));
}

MAKE_HOOK(abort_hook, nullptr, void) {
    modLogger().info("abort was called!");
    modLogger().Backtrace(40);
    abort_hook();
}

CUSTOM_TYPES_FUNC void load() {
    INSTALL_HOOK(modLogger(), MainMenuViewController_DidActivate);

    auto libc = dlopen("libc.so", RTLD_NOW);
    auto abort_addr = dlsym(libc, "abort");
    INSTALL_HOOK_DIRECT(modLogger(), abort_hook, abort_addr);
}

#endif
