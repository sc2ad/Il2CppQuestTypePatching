#pragma once
#include <type_traits>
#include "beatsaber-hook/shared/utils/size-concepts.hpp"
#include "beatsaber-hook/shared/utils/type-concepts.hpp"

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
    struct passes_il2cpp_ref_req {
        static constexpr bool value = false;
    };
    template<il2cpp_utils::il2cpp_reference_type_requirements T>
    struct passes_il2cpp_ref_req<T> {
        static constexpr bool value = true;
    };

    template<typename T>
    static constexpr bool passes_il2cpp_ref_req_v = passes_il2cpp_ref_req<T>::value;

    template<typename T>
    T* field_addr(void* instance, size_t offset) {
        return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(instance) + offset);
    }

    template<typename T>
    requires(!passes_il2cpp_ref_req_v<T>)
    T& read_field(void* instance, size_t offset) {
        return *field_addr<T>(instance, offset);
    }

    template<typename T>
    requires(passes_il2cpp_ref_req_v<T>)
    T read_field(void* instance, size_t offset) {
        return T{*field_addr<void*>(instance, offset)};
    }

    template<typename T>
    void write_field(void* instance, size_t offset, T&& value) {
        if constexpr(std::is_pointer_v<T>) {
            il2cpp_functions::gc_wbarrier_set_field(reinterpret_cast<Il2CppObject*>(instance), field_addr<void*>(instance, offset), value);
        } else if constexpr (passes_il2cpp_ref_req_v<T>) {
            il2cpp_functions::gc_wbarrier_set_field(reinterpret_cast<Il2CppObject*>(instance), field_addr<void*>(instance, offset), value.convert());
        } else {
            *field_addr<T>(instance, offset) = value;
        }
    }

    template<typename T>
    struct ExtractedFieldType;

    /// @brief if il2cpp size is safe, just make the type the same
    template<typename T>
    requires(il2cpp_safe(T))
    struct ExtractedFieldType<T> {
        using Type = T;
    };

    /// @brief safeguard for fields that don't match their size
    template<typename T>
    requires(passes_il2cpp_ref_req_v<T> && sizeof(T) != sizeof(void*))
    struct ExtractedFieldType<T> {
        using Type = Il2CppObject*;
    };

    template<typename T>
    using extracted_field_t = typename ExtractedFieldType<T>::Type;
}
