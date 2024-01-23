#pragma once
#include <type_traits>
#include "beatsaber-hook/shared/utils/type-concepts.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

namespace custom_types {
    template<class F>
    struct Decomposer;

    template<class R, class T, class... TArgs>
    struct Decomposer<R (T::*)(TArgs...)> {
        template<class... TArgs2>
        static constexpr bool equal() {
            if constexpr (sizeof...(TArgs) != sizeof...(TArgs2)) {
                return false;
            } else {
                return (std::is_same_v<TArgs, TArgs2> && ...);
            }
        }
        template<class... TArgs2>
        static constexpr bool convertible() {
            if constexpr (sizeof...(TArgs) != sizeof...(TArgs2)) {
                return false;
            } else {
                // Convertible from the args specified TO the function pointer provided
                return (std::is_convertible_v<TArgs2, TArgs> && ...);
            }
        }
    };

    template<class... TArgs>
    std::vector<Il2CppClass*> ExtractClasses() {
        return {classof(TArgs)...};
    }
    template<typename T>
    struct field_accessor {
        inline constexpr T const* field_addr(const void* instance, std::size_t offset) const noexcept {
            return static_cast<T const*>(static_cast<void const*>(static_cast<uint8_t const*>(instance) + offset));
        }

        inline constexpr T* field_addr(void* instance, std::size_t offset) const noexcept {
            return static_cast<T*>(static_cast<void*>(static_cast<uint8_t*>(instance) + offset));
        }

        inline constexpr T& read(void* instance, std::size_t offset) const noexcept {
            return *field_addr(instance, offset);
        }

        inline constexpr T const& read(void const* instance, std::size_t offset) const noexcept {
            return *field_addr(instance, offset);
        }

        inline constexpr void write(void* instance, std::size_t offset, T&& v) const noexcept {
            *field_addr(instance, offset) = v;
        }
    };

    template<il2cpp_utils::il2cpp_reference_type_pointer T>
    struct field_accessor<T> {
        inline constexpr void* field_addr(void* instance, std::size_t offset) const noexcept {
            return static_cast<void*>(static_cast<uint8_t*>(instance) + offset);
        }

        inline constexpr void const* field_addr(void const* instance, std::size_t offset) const noexcept {
            return static_cast<void const*>(static_cast<uint8_t const*>(instance) + offset);
        }

        inline constexpr T& read(void* instance, std::size_t offset) const noexcept {
            return *static_cast<T*>(field_addr(instance, offset));
        }

        inline constexpr T const& read(void const* instance, std::size_t offset) const noexcept {
            return *static_cast<T const*>(field_addr(instance, offset));
        }

        inline void write(void* instance, std::size_t offset, T&& v) const noexcept {
            il2cpp_functions::gc_wbarrier_set_field(
                static_cast<Il2CppObject*>(instance),
                static_cast<void**>(field_addr(instance, offset)),
                il2cpp_utils::il2cpp_reference_type_value<T>(std::forward<T>(v))
            );
        }
    };
}
