#pragma once
#include <dlfcn.h>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include <string_view>
#include <memory>
#include <vector>

struct Il2CppType;
struct ParameterInfo;
struct Il2CppClass;
struct FieldInfo;
struct MethodInfo;
class Register;

struct type_info {
    friend Register;
    private:
    Il2CppClass* base;
    public:
    type_info(std::string_view namespaze, std::string_view name, Il2CppClass* base);
};

struct method_info {
    friend Register;
    private:
    std::vector<ParameterInfo> params;
    MethodInfo* info;
    constexpr void setClass(Il2CppClass* klass) const;
    constexpr void setInvoker(InvokerMethod invoker) const;
    public:
    // TODO: Figure out a way to dynamically get the invoker_function
    method_info(std::string_view name, void* func, const Il2CppType* returnType, std::vector<ParameterInfo>& parameters, uint16_t flags);
    ~method_info();
    constexpr const MethodInfo* get() const;
};

struct field_info {
    friend Register;
    private:
    FieldInfo* info;
    constexpr void setClass(Il2CppClass* klass) const;
    public:
    // Offset obtained via macro
    field_info(std::string_view name, const Il2CppType* type, int32_t offset);
    ~field_info();
    constexpr const FieldInfo* get() const;
};

namespace custom_types {
    #if __has_include(<concepts>)
    #include <concepts>
    template<typename T>
    constexpr bool has_field_wrapper = requires(const T& t) {
        t.get();
    };
    #elif __has_include(<experimental/type_traits>)
    #include <experimental/type_traits>
    template<typename T>
    using type = decltype(T::get());

    template <class...> constexpr std::false_type false_t{};

    template<typename T>
    constexpr bool has_get = std::experimental::is_detected_v<type, T>;
    #else
    #error No libraries for the implementation of "has_" anything available!
    #endif

    template<typename T>
    struct method_info_template_instance {};

    template<typename T>
    struct method_info_template_static {};

    template<typename T>
    struct name_registry {};

    // Create a single ParameterInfo* from P
    template<typename P>
    struct single_parameter_converter {
        static inline const ParameterInfo get() {
            ParameterInfo info;
            il2cpp_functions::Init();
            auto type = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<P>::get());
            // Ignore name, it will be set when we iterate over all of them (param_1, param_2, etc.)
            // Ignore position, it will also be set on iteration.
            // TODO: Maybe some day we can actually use the parameters names themselves!
            info.parameter_type = type;
            info.token = -1;
            return info;
        }
    };

    // Create a vector of ParameterInfo objects (good ol tail recursion)
    template<typename P, typename... Ps>
    struct parameter_converter {
        static inline std::vector<ParameterInfo> get() {
            std::vector<ParameterInfo> params;
            params.push_back(single_parameter_converter<P>::get());
            for (auto p : parameter_converter<Ps...>::get()) {
                params.push_back(p);
            }
            return params;
        }
    };

    template<typename TRet, typename T, typename... TArgs>
    struct method_info_template_instance<TRet(T::*)(TArgs...)> {
        static inline const Il2CppType* get() {
            il2cpp_functions::Init();
            return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TRet>::get());
        }
        static inline const std::vector<ParameterInfo> get_params() {
            std::vector<ParameterInfo> vec = parameter_converter<TArgs...>::get();
            for (std::size_t i = 0; i < vec.size(); i++) {
                vec[i].name = string_format("param_%u", i).c_str();
                vec[i].position = i;
            }
            return vec;
        }
        template<TRet(T::* member)(TArgs...)>
        static inline TRet wrap(T* self, TArgs&& ...args) {
            return self->*member(std::forward<TArgs>(args)...);
        }
    };

    template<typename TRet, typename... TArgs>
    struct method_info_template_static<TRet(*)(TArgs...)> {
        static inline const Il2CppType* get() {
            il2cpp_functions::Init();
            return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TRet>::get());
        }
        static inline const std::vector<ParameterInfo> get_params() {
            std::vector<ParameterInfo> vec = parameter_converter<TArgs...>::get();
            for (std::size_t i = 0; i < vec.size(); i++) {
                vec[i].name = string_format("param_%u", i).c_str();
                vec[i].position = i;
            }
            return vec;
        }
    };

    // Creates static inline _register function used to register type within il2cpp
    #define REGISTER_FUNCTION(typeN, innards) \
    static inline void _register(std::vector<::field_info>& fields, std::vector<::method_info>& methods) { \
        using TargetType = typeN; \
        innards \
    }

    // Declare a field with type, name.
    // Fields declared like this must also be registered via REGISTER_FIELD within the REGISTER_TYPE function.
    #define DECLARE_FIELD(type, name) \
    type name; \
    struct field_wrapper_##name { \
        static inline field_info get() { \
            int32_t offset = _get_field_offset(); \
            _increment_field_offset(sizeof(type)); \
            ::il2cpp_functions::Init(); \
            auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
            return field_info{#name, t, offset}; \
        } \
    }

    // Declare an instance method with: returnType, declaringType, name, parameters, ...
    // Methods declared like this must also be registered via REGISTER_METHOD within the REGISTER_TYPE function.
    // Yes, this inner structure really does have to be a template, much to my dismay
    #define DECLARE_METHOD(retType, name, ...) \
    retType name(__VA_ARGS__); \
    template<typename DeclType, class Discard = void> \
    struct method_wrapper_##name { \
        static inline method_info get() { \
            const Il2CppType* ret; \
            std::vector<ParameterInfo> params; \
            uint16_t flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG; \
            void* ptr; \
            if (std::string(#name).starts_with(".")) { \
                flags |= METHOD_ATTRIBUTE_SPECIAL_NAME; \
            } \
            using memberPtr = decltype(&DeclType::name); \
            using instanceClass = ::custom_types::method_info_template_instance<memberPtr>; \
            using staticClass = ::custom_types::method_info_template_static<memberPtr>; \
            if constexpr (::custom_types::has_get<instanceClass>) { \
                ret = instanceClass::get(); \
                params = instanceClass::get_params(); \
                ptr = (void*)&instanceClass::wrap<memberPtr>(); \
            } else if constexpr (::custom_types::has_get<staticClass>) {\
                ret = staticClass::get(); \
                params = staticClass::get_params(); \
                flags |= METHOD_ATTRIBUTE_STATIC; \
                ptr = (void*)&staticClass::wrap<memberPtr>(); \
            } else { \
                static_assert(false_t<memberPtr>, "Must define either an instance or a static method! Could not match either!"); \
            } \
            return method_info{#name, ptr, ret, params, flags}; \
        } \
    }

    #define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, impl) \
    namespace namespaze { \
        class name; \
    } \
    template<> \
    struct ::custom_types::name_registry<namespaze::name> { \
        static inline type_info get() { \
            return type_info{#namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName)}; \
        } \
    }; \
    namespace namespaze { \
        class name { \
            static inline int32_t _field_offset = 0; \
            static inline int32_t _get_field_offset() { \
                return _field_offset; \
            } \
            static inline void _increment_field_offset(std::size_t off) { \
                _field_offset += off; \
            } \
            impl \
        }; \
    }


    // Registers a field to be attached to this type.
    // Must be called within the REGISTER_TYPE function.
    #define REGISTER_FIELD(name) \
    fields.push_back(field_wrapper_##name::get())

    // Registers a method to be attached to this type.
    // Must be called within the REGISTER_TYPE function.
    #define REGISTER_METHOD(name) \
    methods.push_back(method_wrapper_##name<TargetType>::get())
}
