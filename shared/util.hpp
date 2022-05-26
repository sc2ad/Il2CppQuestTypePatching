#pragma once
#include <type_traits>

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
}