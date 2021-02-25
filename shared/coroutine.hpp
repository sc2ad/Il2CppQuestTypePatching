#pragma once

#if __has_include(<coroutine>)
#include <coroutine>
namespace custom_types::Helpers {
    template<typename T>
    using coro_handle = std::coroutine_handle<T>;
    using suspend_always = std::suspend_always;
    using suspend_never = std::suspend_never;
}

#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace custom_types::Helpers {
    template<typename T>
    using coro_handle = std::experimental::coroutine_handle<T>;
    using suspend_always = std::experimental::suspend_always;
    using suspend_never = std::experimental::suspend_never;
}

#else
#error "You must have a definition for C++ 20 Coroutines in order to include coroutine.hpp! Is your NDK version >= 22? Are your compile flags set?"
#endif

#include <type_traits>
#include <utility>
// TODO: Conditionalize exception include
#include <exception>
#include <functional>
#include "macros.hpp"

#ifdef HAS_CODEGEN
#include "System/Collections/IEnumerator.hpp"
namespace custom_types::Helpers {
    using enumeratorT = System::Collections::IEnumerator;
}
#else
namespace custom_types::Helpers {
    using enumeratorT = Il2CppObject;
}
#endif

#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

namespace custom_types::Helpers {
    /// @brief Generator type used as a coroutine
    /// @tparam T The type of the value wrapped in the generator.
    template<typename T>
    class generator;
    using Coroutine = generator<enumeratorT*>;
    using CoroFuncType = std::function<Coroutine ()>;
}
// Coroutine* mapped to void*
template<>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<custom_types::Helpers::Coroutine*> {
    static inline Il2CppClass* get() {
        il2cpp_functions::Init();
        return il2cpp_functions::Class_GetPtrClass(il2cpp_functions::defaults->void_class);
    }
};
// CoroFuncType* mapped to void*
template<>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<custom_types::Helpers::CoroFuncType*> {
    static inline Il2CppClass* get() {
        il2cpp_functions::Init();
        return il2cpp_functions::Class_GetPtrClass(il2cpp_functions::defaults->void_class);
    }
};

namespace custom_types::Helpers {
    /// @brief A helper type for creating custom coroutines C# from C++.
    /// See ResetableCoroutine and StandardCoroutine for more info.
    struct CoroutineHelper;

    /// @brief Represents a coroutine wrapper that is resetable.
    class ResetableCoroutine;
    /// @brief Represents a coroutine wrapper that is not resetable, but is slightly faster and has less allocations.
    class StandardCoroutine;

    namespace internal {
        // Generator and promise types inspired greatly by https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/generator.hpp
        /// @brief A generic generator_promise used by a generator<T>
        /// @tparam T The value type/yields of the generator
        template<typename T>
        class generator_promise {
        public:
            using value_type = std::remove_reference_t<T>;
            using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
            using pointer_type = value_type*;

            generator_promise() = default;

            Helpers::generator<T> get_return_object() noexcept;

            constexpr Helpers::suspend_always initial_suspend() const noexcept { return {}; }
            constexpr Helpers::suspend_always final_suspend() const noexcept { return {}; }

            template<typename U = T>
            requires (!std::is_rvalue_reference<U>::value)
            Helpers::suspend_always yield_value(std::remove_reference_t<T>& value) noexcept {
                m_value = std::addressof(value);
                return {};
            }

            Helpers::suspend_always yield_value(std::remove_reference_t<T>&& value) noexcept {
                m_value = std::addressof(value);
                return {};
            }

            void unhandled_exception() {
                m_exception = std::current_exception();
            }

            void return_void() {
            }

            reference_type value() const noexcept {
                return static_cast<reference_type>(*m_value);
            }

            // Don't allow any use of 'co_await' inside the generator coroutine.
            template<typename U>
            Helpers::suspend_never await_transform(U&& value) = delete;

            void rethrow_if_exception() {
                if (m_exception) {
                    std::rethrow_exception(m_exception);
                }
            }
        private:
            pointer_type m_value;
            std::exception_ptr m_exception;
        };
    }
    /// @brief Generator type used as a coroutine
    /// @tparam T The type of the value wrapped in the generator.
    template<typename T>
    class [[nodiscard]] generator {
    public:
        using promise_type = internal::generator_promise<T>;

        generator() noexcept : m_coroutine(nullptr) {}

        generator(generator&& other) noexcept : m_coroutine(other.m_coroutine) {
            other.m_coroutine = nullptr;
        }

        generator(const generator& other) = delete;

        ~generator() {
            if (m_coroutine) {
                m_coroutine.destroy();
            }
        }

        generator& operator=(generator other) noexcept {
            swap(other);
            return *this;
        }

        void swap(generator& other) noexcept {
            std::swap(m_coroutine, other.m_coroutine);
        }

    private:
        friend struct InternalHelper;
        friend class internal::generator_promise<T>;
        explicit generator(coro_handle<promise_type> coroutine) noexcept : m_coroutine(coroutine) {}

        coro_handle<promise_type> m_coroutine;
    };

    namespace internal {
        template<typename T>
        Helpers::generator<T> generator_promise<T>::get_return_object() noexcept {
            using coroutine_handle = coro_handle<generator_promise<T>>;
            return Helpers::generator<T>{ coroutine_handle::from_promise(*this) };
        }
    }
}

DECLARE_CLASS_INTERFACES(custom_types::Helpers, ResetableCoroutine, "System", "Object", sizeof(Il2CppObject), 
        il2cpp_utils::GetClassFromName("System.Collections", "IEnumerator"),
    private:
    custom_types::Helpers::CoroFuncType coroCreator;
    // Explicitly delete copy constructor
    ResetableCoroutine(const ResetableCoroutine& other) = delete;
    // Explicitly delete move constructor
    ResetableCoroutine(ResetableCoroutine&& other) = delete;
    // These fields exist as C# fields for semantic purposes only
    DECLARE_INSTANCE_FIELD(custom_types::Helpers::Coroutine*, currentCoro);
    DECLARE_INSTANCE_FIELD(custom_types::Helpers::enumeratorT*, current);
    DECLARE_INSTANCE_FIELD(bool, valid);

    DECLARE_CTOR(ctor, custom_types::Helpers::CoroFuncType* creator);
    DECLARE_OVERRIDE_METHOD(bool, MoveNext, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "MoveNext"));
    DECLARE_OVERRIDE_METHOD(Il2CppObject*, get_Current, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "get_Current"));
    DECLARE_OVERRIDE_METHOD(void, Reset, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "Reset"));
    DECLARE_OVERRIDE_METHOD(void, Finalize, il2cpp_utils::FindMethod("System", "Object", "Finalize"));

    REGISTER_FUNCTION(ResetableCoroutine,
        REGISTER_FIELD(currentCoro);
        REGISTER_FIELD(current);
        REGISTER_FIELD(valid);
        REGISTER_METHOD(ctor);
        REGISTER_METHOD(MoveNext);
        REGISTER_METHOD(get_Current);
        REGISTER_METHOD(Reset);
        REGISTER_METHOD(Finalize);
    )
)
    
inline const Il2CppClass* custom_types::Helpers::ResetableCoroutine::klass = nullptr;

DECLARE_CLASS_INTERFACES(custom_types::Helpers, StandardCoroutine, "System", "Object", sizeof(Il2CppObject), 
        il2cpp_utils::GetClassFromName("System.Collections", "IEnumerator"),
    struct CoroutineNotResettable : std::runtime_error {
        CoroutineNotResettable() : std::runtime_error("StandardCoroutine is not resettable!") {}
    };
    // Explicitly delete copy constructor
    StandardCoroutine(const StandardCoroutine& other) = delete;
    // Explicitly delete move constructor
    StandardCoroutine(StandardCoroutine&& other) = delete;
    // These fields exist as C# fields for semantic purposes only
    DECLARE_INSTANCE_FIELD(custom_types::Helpers::Coroutine*, currentCoro);
    DECLARE_INSTANCE_FIELD(custom_types::Helpers::enumeratorT*, current);
    DECLARE_INSTANCE_FIELD(bool, valid);

    // Construct a StandardCoroutine around a provided Coroutine instance.
    DECLARE_CTOR(ctor, custom_types::Helpers::Coroutine* coro);
    DECLARE_OVERRIDE_METHOD(bool, MoveNext, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "MoveNext"));
    DECLARE_OVERRIDE_METHOD(Il2CppObject*, get_Current, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "get_Current"));
    DECLARE_OVERRIDE_METHOD(void, Reset, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "Reset"));
    DECLARE_OVERRIDE_METHOD(void, Finalize, il2cpp_utils::FindMethod("System", "Object", "Finalize"));

    REGISTER_FUNCTION(ResetableCoroutine,
        REGISTER_FIELD(currentCoro);
        REGISTER_FIELD(current);
        REGISTER_FIELD(valid);
        REGISTER_METHOD(ctor);
        REGISTER_METHOD(MoveNext);
        REGISTER_METHOD(get_Current);
        REGISTER_METHOD(Reset);
        REGISTER_METHOD(Finalize);
    )
)
    
inline const Il2CppClass* custom_types::Helpers::StandardCoroutine::klass = nullptr;

namespace custom_types::Helpers {
    /// @brief Represents an allocation failure when creating a new coroutine.
    struct CoroutineAllocationFailed : std::runtime_error {
        CoroutineAllocationFailed() : std::runtime_error("Failed to allocate a coroutine!") {}
    };
    /// @brief Represents an exception thrown by attempting to use an already destroyed coroutine instance.
    struct CoroutineDisposed : std::runtime_error {
        CoroutineDisposed() : std::runtime_error("Coroutine instance is already disposed!") {}
    };

    /// @brief A helper type for creating custom coroutines C# from C++.
    /// See ResetableCoroutine and StandardCoroutine for more info.
    struct CoroutineHelper {
        /// @brief Creates a new StandardCoroutine from the provided Coroutine instance, which is immediately rendered invalid.
        /// This function will throw a ::custom_types::Helpers::CoroutineAllocationFailed exception on failure.
        /// @tparam cType The creation type of the created coroutine.
        /// @param c The Coroutine instance to construct the instance with.
        /// @return The created coroutine instance.
        template<il2cpp_utils::CreationType cType = il2cpp_utils::CreationType::Temporary>
        static inline StandardCoroutine* New(Coroutine&& c) {
            auto res = il2cpp_utils::New<StandardCoroutine*, cType>(new Coroutine(std::move(c)));
            if (!res) {
                throw CoroutineAllocationFailed();
            }
            return res.value();
        }

        /// @brief Creates a new ResetableCoroutine from the provided Coroutine creator function, which is copied.
        /// This function will throw a ::custom_types::Helpers::CoroutineAllocationFailed exception on failure.
        /// @tparam cType The creation type of the created coroutine.
        /// @param func The CoroFuncType to construct the instance with.
        /// @return The created coroutine instance.
        template<il2cpp_utils::CreationType cType = il2cpp_utils::CreationType::Temporary>
        static inline ResetableCoroutine* New(CoroFuncType func) {
            auto res = il2cpp_utils::New<ResetableCoroutine*, cType>(&func);
            if (!res) {
                throw CoroutineAllocationFailed();
            }
            return res.value();
        }

        /// @brief Creates a new ResetableCoroutine from the provided Coroutine creator function and arguments used to invoke it.
        /// The arguments used to invoke this function are copied by value (be careful of references!)
        /// This function will throw a ::custom_types::Helpers::CoroutineAllocationFailed exception on failure.
        /// @tparam cType The creation type of the created coroutine.
        /// @tparam TArgs The arguments to provide to the creator function.
        /// @param func The creator function to wrap.
        /// @param args The arguments to provide to the function.
        /// @return The created coroutine instance.
        template<il2cpp_utils::CreationType cType = il2cpp_utils::CreationType::Temporary, class... TArgs>
        static inline ResetableCoroutine* New(std::function<Coroutine (TArgs...)> func, TArgs... args) {
            return New(std::function<Coroutine ()>([func, args...] {
                return func(args...);
            }));
        }

        /// @brief Creates a new ResetableCoroutine from the provided Coroutine creator function and arguments used to invoke it.
        /// The arguments used to invoke this function are copied by value (be careful of references!)
        /// This function will throw a ::custom_types::Helpers::CoroutineAllocationFailed exception on failure.
        /// @tparam cType The creation type of the created coroutine.
        /// @tparam TArgs The arguments to provide to the creator function.
        /// @param func The creator function to wrap.
        /// @param args The arguments to provide to the function.
        /// @return The created coroutine instance.
        template<il2cpp_utils::CreationType cType = il2cpp_utils::CreationType::Temporary, class... TArgs>
        static inline ResetableCoroutine* New(function_ptr_t<Coroutine, TArgs...> func, TArgs... args) {
            return New(std::function<Coroutine ()>([func, args...] {
                return func(args...);
            }));
        }

        /// @brief Creates a new ResetableCoroutine from the provided Coroutine creator instance function and arguments used to invoke it.
        /// The arguments used to invoke this function are copied by value (be careful of references!)
        /// This function will throw a ::custom_types::Helpers::CoroutineAllocationFailed exception on failure.
        /// @tparam cType The creation type of the created coroutine.
        /// @tparam I The instance type of the instance function
        /// @tparam TArgs The arguments to provide to the creator function.
        /// @param func The creator function to wrap.
        /// @param instance The instance pointer to provide to the coroutine constructor. This instance must always exist when the returned ResetableCoroutine's Reset method is called.
        /// @param args The arguments to provide to the function.
        /// @return The created coroutine instance.
        template<il2cpp_utils::CreationType cType = il2cpp_utils::CreationType::Temporary, class I, class... TArgs>
        static inline ResetableCoroutine* New(Coroutine (I::*func)(TArgs...), I* instance, TArgs... args) {
            return New(std::function<Coroutine ()>([func, instance, args...] {
                return std::function<Coroutine (I*, TArgs...)>(func)(instance, args...);
            }));
        }
    };
}

namespace std {
    template<typename T>
    void swap(custom_types::Helpers::generator<T>& a, custom_types::Helpers::generator<T>& b) {
        a.swap(b);
    }
}
