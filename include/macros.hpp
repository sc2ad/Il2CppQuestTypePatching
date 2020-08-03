#pragma once
#include "types.hpp"

// Declare a field with type, name.
// Fields declared like this must also be registered via REGISTER_FIELD within the REGISTER_TYPE function.
#define DECLARE_INSTANCE_FIELD(type, name) \
public: \
type name; \
private: \
struct field_wrapper_##name { \
    static inline field_info* get() { \
        int32_t offset = _get_field_offset(); \
        _increment_field_offset(sizeof(type)); \
        ::il2cpp_functions::Init(); \
        auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
        uint16_t attrs = FIELD_ATTRIBUTE_PUBLIC; \
        return new field_info(#name, t, offset, attrs); \
    } \
    static inline constexpr bool isStatic() { \
        return false; \
    } \
}

#define DECLARE_STATIC_FIELD(type, name) \
public: \
static type name; \
private: \
struct field_wrapper_##name { \
    static inline field_info* get() { \
        ::il2cpp_functions::Init(); \
        auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
        uint16_t attrs = FIELD_ATTRIBUTE_PUBLIC | FIELD_ATTRIBUTE_STATIC; \
        return new field_info(#name, t, offset, attrs); \
    } \
    static inline constexpr bool isStatic() { \
        return true; \
    } \
}

// Declare an instance method with: returnType, declaringType, name, parameters, ...
// Methods declared like this must also be registered via REGISTER_METHOD within the REGISTER_TYPE function.
// Yes, this inner structure really does have to be a template, much to my dismay
#define DECLARE_METHOD(methodPrefix, name, ...) \
public: \
methodPrefix name(__VA_ARGS__); \
private: \
template<typename DeclType, class Discard = void> \
struct method_wrapper_##name { \
    static inline method_info* get() { \
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
        logger().debug("creating method_info for: %s", #name); \
        return new method_info(#name, ptr, invoker, ret, params, flags); \
    } \
}

#define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, impl) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline type_info* get() { \
        logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName)); \
    } \
}; \
namespace namespaze { \
    class name { \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        private: \
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

// Creates static inline _register function used to register type within il2cpp
#define REGISTER_FUNCTION(typeN, innards) \
static inline void _register(std::vector<::field_info*>& fields, std::vector<::field_info*>& staticFields, std::vector<::method_info*>& methods) { \
    using TargetType = typeN; \
    innards \
}

// Registers a field to be attached to this type.
// Must be called within the REGISTER_TYPE function.
#define REGISTER_FIELD(name) \
do { \
    if (field_wrapper_##name::isStatic()) { \
        staticFields.push_back(std::move(field_wrapper_##name::get())); \
    } \
    fields.push_back(std::move(field_wrapper_##name::get())); \
} while (0)

// Registers a method to be attached to this type.
// Must be called within the REGISTER_TYPE function.
#define REGISTER_METHOD(name) \
methods.push_back(std::move(method_wrapper_##name<TargetType>::get()))