#include "coroutine.hpp"
#include "register.hpp"

namespace custom_types::Helpers {
    DEFINE_TYPE(custom_types::Helpers, ResetableCoroutine);
    DEFINE_TYPE(custom_types::Helpers, StandardCoroutine);

    struct InternalHelper {
        static bool MoveNextHelper(Coroutine*& currentCoro, Wrapper& current) {
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
            //     // current will also always be an enumeratorT. This will allow us to call RunMethod(current, "MoveNext") (possibly "System.Collections.IEnumerator.MoveNext")
            //     if (THROW_UNLESS(il2cpp_utils::RunMethod<bool>(current, "MoveNext"))) {
            //         // If we are waiting on our current object, wait until it is complete
            //         return true;
            //     }
            //     // Our current instance is complete, run ourselves now.
            //     current = nullptr;
            // }
            currentCoro->m_coroutine.resume();
            // TODO: See if there is ever a case where this caught exception would result in the coroutine not properly being reset
            // ex: current not being set to nullptr, currentCoro not being set to nullptr
            IL2CPP_CATCH_HANDLER(
                currentCoro->m_coroutine.promise().rethrow_if_exception();
            )
            if (currentCoro->m_coroutine.done()) {
                current = nullptr;
                // If we have an exception, throw it to the il2cpp domain if we can.
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
        __set_coroCreator(*creator);
        __set_currentCoro(new Coroutine(__get_coroCreator()()));
        __set_current(nullptr);
        // Valid is only set to false when this instance is destroyed to prevent further calls.
        __set_valid(true);
    }

    bool ResetableCoroutine::MoveNext() {
        if (!__get_valid()) {
            throw CoroutineDisposed();
        }
        return InternalHelper::MoveNextHelper(__get_currentCoro(), __get_current());
    }

    Il2CppObject* ResetableCoroutine::get_Current() {
        return reinterpret_cast<Il2CppObject*>(__get_current().instance);
    }

    void ResetableCoroutine::Reset() {
        if (!__get_valid()) {
            throw CoroutineDisposed();
        }
        // When we reset this coroutine, we need to destroy our current coroutine.
        // Then we need to get a new one
        // Then we need to set our current to nullptr
        if (__get_currentCoro()) {
            delete __get_currentCoro();
        }
        __set_currentCoro(new Coroutine(__get_coroCreator()()));
        __set_current(nullptr);
    }

    void ResetableCoroutine::Finalize() {
        // On destruction, we must destroy everything we have.
        __set_valid(false);
        __get_coroCreator().~CoroFuncType();
        delete __get_currentCoro();
        // It does not matter what current is, as GC will no longer see this reference and it will get cleaned up later.
    }

    void StandardCoroutine::ctor(Coroutine* coro) {
        __set_current(nullptr);
        __set_currentCoro(coro);
        __set_valid(true);
    }

    bool StandardCoroutine::MoveNext() {
        if (!__get_valid()) {
            throw CoroutineDisposed();
        }
        return InternalHelper::MoveNextHelper(__get_currentCoro(), __get_current());
    }

    Il2CppObject* StandardCoroutine::get_Current() {
        return reinterpret_cast<Il2CppObject*>(__get_current().instance);
    }

    void StandardCoroutine::Reset() {
        throw CoroutineNotResettable();
    }

    void StandardCoroutine::Finalize() {
        // On destruction, we must destroy everything we have.
        __set_valid(false);
        delete __get_currentCoro();
        // It does not matter what current is, as GC will no longer see this reference and it will get cleaned up later.
    }

    bool initialized = false;

    void CoroutineHelper::EnsureCoroutines() {
        if (!initialized) {
            custom_types::Register::ExplicitRegister({&__registration_instance_ResetableCoroutine, &__registration_instance_StandardCoroutine});
            initialized = true;
        }
    }
}
