#include "coroutine.hpp"
#include "register.hpp"

namespace custom_types::Helpers {
    DEFINE_TYPE(ResetableCoroutine);
    DEFINE_TYPE(StandardCoroutine);

    struct InternalHelper {
        static bool MoveNextHelper(Coroutine*& currentCoro, enumeratorT*& current) {
            if (!currentCoro) {
                // If we do not have a current coroutine, we exit immediately.
                custom_types::_logger().debug("redundant MoveNext call!");
                return false;
            }
            // Each call to MoveNext performs the following:
            // If we have an object that is current, wait on it until complete, fallthrough (this is performed by C#):
            // If we have no object, advance our own until we are complete, fallthough:
            // If we are complete, return false

            // if (current) {
            //     // current will never be GC'd, as it is a field on a custom type.
            //     // current will also always be an enumeratorT*. This will allow us to call RunMethod(current, "MoveNext") (possibly "System.Collections.IEnumerator.MoveNext")
            //     if (THROW_UNLESS(il2cpp_utils::RunMethod<bool>(current, "MoveNext"))) {
            //         // If we are waiting on our current object, wait until it is complete
            //         return true;
            //     }
            //     // Our current instance is complete, run ourselves now.
            //     current = nullptr;
            // }
            currentCoro->m_coroutine.resume();
            if (currentCoro->m_coroutine.done()) {
                current = nullptr;
                // If we have an exception, throw it
                currentCoro->m_coroutine.promise().rethrow_if_exception();
                // Set currentCoro to nullptr, so that we failsafe exit
                currentCoro = nullptr;
                return false;
            }
            // Otherwise, get the next current value
            current = currentCoro->m_coroutine.promise().value();
            return true;
        }
    };

    void ResetableCoroutine::ctor(CoroFuncType* creator) {
        // Create copy
        coroCreator = *creator;
        currentCoro = new Coroutine(std::move(coroCreator()));
        current = nullptr;
        // Valid is only set to false when this instance is destroyed to prevent further calls.
        valid = true;
    }

    bool ResetableCoroutine::MoveNext() {
        if (!valid) {
            throw CoroutineDisposed();
        }
        return InternalHelper::MoveNextHelper(currentCoro, current);
    }

    Il2CppObject* ResetableCoroutine::get_Current() {
        return reinterpret_cast<Il2CppObject*>(current);
    }

    void ResetableCoroutine::Reset() {
        if (!valid) {
            throw CoroutineDisposed();
        }
        // When we reset this coroutine, we need to destroy our current coroutine.
        // Then we need to get a new one
        // Then we need to set our current to nullptr
        if (currentCoro) {
            delete currentCoro;
        }
        currentCoro = new Coroutine(std::move(coroCreator()));
        current = nullptr;
    }

    void ResetableCoroutine::Finalize() {
        // On destruction, we must destroy everything we have.
        valid = false;
        coroCreator.~CoroFuncType();
        delete currentCoro;
        // It does not matter what current is, as GC will no longer see this reference and it will get cleaned up later.
    }

    void StandardCoroutine::ctor(Coroutine* coro) {
        current = nullptr;
        currentCoro = coro;
        valid = true;
    }

    bool StandardCoroutine::MoveNext() {
        if (!valid) {
            throw CoroutineDisposed();
        }
        return InternalHelper::MoveNextHelper(currentCoro, current);
    }

    Il2CppObject* StandardCoroutine::get_Current() {
        return reinterpret_cast<Il2CppObject*>(current);
    }

    void StandardCoroutine::Reset() {
        throw CoroutineNotResettable();
    }

    void StandardCoroutine::Finalize() {
        // On destruction, we must destroy everything we have.
        valid = false;
        delete currentCoro;
        // It does not matter what current is, as GC will no longer see this reference and it will get cleaned up later.
    }

    bool initialized = false;

    void CoroutineHelper::EnsureCoroutines() {
        if (!initialized) {
            custom_types::Register::RegisterTypes<
                ResetableCoroutine,
                StandardCoroutine
            >();
            initialized = true;
        }
    }
}