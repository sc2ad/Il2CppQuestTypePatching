#ifdef LOCAL_TEST_COROUTINE
#include "register.hpp"
#include "coroutine.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

using namespace custom_types::Helpers;

ModInfo modInfo;

Logger& modLogger() {
	static auto myLogger = new Logger(modInfo);
	return *myLogger;
}

Coroutine testNestedCoro(int n) {
	// modLogger().debug("begin nested coro");
	co_yield nullptr;
	// modLogger().debug("one step nested coro");
	// This will not be deleted by GC, as it will be stored as a field in the coro when invoked.
	co_yield reinterpret_cast<enumeratorT*>(CRASH_UNLESS(il2cpp_utils::New("UnityEngine", "WaitForSecondsRealtime", 2.3f)));
	modLogger().debug("yielding until: %d", n);
	for (int i = 0; i < n; i++) {
		// modLogger().debug("nested coro yield: %d", i);
		co_yield nullptr;
	}
	co_return;
	// modLogger().debug("nested coroutine complete");
}

Coroutine testRecursiveCall() {
	// modLogger().debug("begin recursive coro");
	auto instance = CoroutineHelper::New<il2cpp_utils::CreationType::Manual>(testNestedCoro, 3);
	// modLogger().debug("created nested instance");
	co_yield instance;
	// modLogger().debug("complete with nested instance");
	// Reset instance and try again
	instance->Reset();
	// modLogger().debug("reset nested instance, running again");
	co_yield instance;
	modLogger().debug("all done");
	co_return;
}

Coroutine testWaitForSeconds() {
	modLogger().debug("About to wait!");
	co_yield reinterpret_cast<enumeratorT*>(CRASH_UNLESS(il2cpp_utils::New("UnityEngine", "WaitForSecondsRealtime", 2.3f)));
	modLogger().debug("I have performed a wait!");
	co_return;
}

extern "C" void setup(ModInfo & info) {
	info.id = ID;
	info.version = VERSION;
	modInfo = info;
	modLogger().debug("Completed setup!");
}

MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(MainMenuViewController_DidActivate, "", "MainMenuViewController", "DidActivate", void, Il2CppObject* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
	MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

	modLogger().debug("Starting coroutine");
	il2cpp_utils::RunMethod(self, "StartCoroutine", custom_types::Helpers::CoroutineHelper::New(testWaitForSeconds()));
}

extern "C" void load() {
	INSTALL_HOOK(modLogger(), MainMenuViewController_DidActivate);
}

#endif