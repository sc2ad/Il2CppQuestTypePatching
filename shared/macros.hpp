#pragma once
#include "types.hpp"

#ifdef DECLARE_CLASS
#error "DECLARE_CLASS is already defined! Undefine it before including macros.hpp!"
#endif
// Declares a class with the given namespace, name, base namespace and base name.
// impl specifies the implementation of the class, the actual definition.
// It is recommended this holds DECLARE statements.
#define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, impl) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName)); \
    } \
}; \
namespace namespaze { \
    class name { \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        private: \
        static inline const Il2CppClass* klass = nullptr; \
        static inline int32_t _field_offset = 0; \
        static inline int32_t _get_field_offset() { \
            return _field_offset + ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName)->actualSize; \
        } \
        static inline void _increment_field_offset(std::size_t off) { \
            _field_offset += off; \
        } \
        impl \
    }; \
}

#ifdef DECLARE_INSTANCE_FIELD
#error "DECLARE_INSTANCE_FIELD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare a field with type, name.
// Fields declared like this must also be registered via REGISTER_FIELD within the REGISTER_TYPE function.
#define DECLARE_INSTANCE_FIELD(type, name) \
public: \
type name; \
private: \
struct field_wrapper_##name { \
    static inline ::custom_types::field_info* get() { \
        int32_t offset = _get_field_offset(); \
        _increment_field_offset(sizeof(type)); \
        ::il2cpp_functions::Init(); \
        auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
        uint16_t attrs = FIELD_ATTRIBUTE_PUBLIC; \
        return new ::custom_types::field_info(#name, t, offset, attrs); \
    } \
    static inline constexpr bool isStatic() { \
        return false; \
    } \
}

#ifdef DECLARE_STATIC_FIELD
#error "DECLARE_STATIC_FIELD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare a static field with type, name.
// Fields declared like this must also be registered via REGISTER_FIELD within the REGISTER_FUNCTION macro.
#define DECLARE_STATIC_FIELD(type, name) \
public: \
static type name; \
private: \
struct field_wrapper_##name { \
    static inline ::custom_types::field_info* get() { \
        ::il2cpp_functions::Init(); \
        auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
        uint16_t attrs = FIELD_ATTRIBUTE_PUBLIC | FIELD_ATTRIBUTE_STATIC; \
        return new ::custom_types::field_info(#name, t, offset, attrs); \
    } \
    static inline constexpr bool isStatic() { \
        return true; \
    } \
}

#ifdef DECLARE_METHOD
#error "DECLARE_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare an instance method with: returnType, declaringType, name, parameters, ...
// Methods declared like this must also be registered via REGISTER_METHOD within the REGISTER_FUNCTION macro.
// Yes, this inner structure really does have to be a template, much to my dismay
#define DECLARE_METHOD(methodPrefix, name, ...) \
public: \
methodPrefix name(__VA_ARGS__); \
private: \
template<typename DeclType, class Discard = void> \
struct method_wrapper_##name { \
    static inline ::custom_types::method_info* get() { \
        using memberPtr = decltype(&DeclType::name); \
        using instanceClass = ::custom_types::method_info_template_instance<memberPtr>; \
        using staticClass = ::custom_types::method_info_template_static<memberPtr>; \
        const Il2CppType* ret; \
        std::vector<ParameterInfo> params; \
        uint16_t flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG; \
        void* ptr; \
        if (std::string(#name).starts_with(".")) { \
            flags |= METHOD_ATTRIBUTE_SPECIAL_NAME; \
        } \
        InvokerMethod invoker = (InvokerMethod)&::custom_types::invoker_creator<memberPtr>::invoke; \
        if constexpr (::custom_types::has_get<instanceClass>) { \
            ret = instanceClass::get(); \
            params = instanceClass::get_params(); \
            ptr = (void*)&instanceClass::template wrap<&DeclType::name>; \
        } else if constexpr (::custom_types::has_get<staticClass>) {\
            ret = staticClass::get(); \
            params = staticClass::get_params(); \
            flags |= METHOD_ATTRIBUTE_STATIC; \
            ptr = (void*)&DeclType::name; \
        } else { \
            static_assert(false_t<memberPtr>, "Must define either an instance or a static method! Could not match either!"); \
        } \
        return new ::custom_types::method_info(#name, ptr, invoker, ret, params, flags); \
    } \
}

#ifdef REGISTER_FUNCTION
#error "REGISTER_FUNCTION is already defined! Undefine it before including macros.hpp!"
#endif
// Creates static inline _register function used to register type within il2cpp
#define REGISTER_FUNCTION(typeN, innards) \
static inline void _register(std::vector<::custom_types::field_info*>& fields, std::vector<::custom_types::field_info*>& staticFields, std::vector<::custom_types::method_info*>& methods) { \
    using TargetType = typeN; \
    innards \
}

#ifdef REGISTER_FIELD
#error "REGISTER_FIELD is already defined! Undefine it before including macros.hpp!"
#endif
// Registers a field to be attached to this type.
// Must be called within the REGISTER_FUNCTION macro.
#define REGISTER_FIELD(name) \
do { \
    if (field_wrapper_##name::isStatic()) { \
        staticFields.push_back(std::move(field_wrapper_##name::get())); \
    } \
    fields.push_back(std::move(field_wrapper_##name::get())); \
} while (0)

#ifdef REGISTER_METHOD
#error "REGISTER_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Registers a method to be attached to this type.
// Must be called within the REGISTER_FUNCTION macro.
#define REGISTER_METHOD(name) \
methods.push_back(std::move(method_wrapper_##name<TargetType>::get()))