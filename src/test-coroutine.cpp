#ifdef LOCAL_TEST
// #ifdef COROUTINE
#include "register.hpp"
#include "coroutine.hpp"

using namespace custom_types::Helpers;

Coroutine testCoro() {
    co_yield nullptr;
    co_yield nullptr;
}

Coroutine testNestedCoro(int n) {
    co_yield nullptr;
    co_yield reinterpret_cast<enumeratorT*>(CoroutineHelper::New(testCoro()));
    for (int i = 0; i < n; i++) {
        co_yield nullptr;
    }
}

Coroutine testRecursiveCall() {
    auto* instance = CoroutineHelper::New<il2cpp_utils::CreationType::Manual>(testNestedCoro, 10);
    co_yield reinterpret_cast<enumeratorT*>(instance);
    // Reset instance and try again
    instance->Reset();
    co_yield reinterpret_cast<enumeratorT*>(instance);
}

// #endif
#endif