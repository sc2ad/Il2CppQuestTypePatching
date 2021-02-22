#pragma once
#include "types.hpp"
#include <stdint.h>
#include <stddef.h>
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#ifdef DECLARE_CLASS
#error "DECLARE_CLASS is already defined! Undefine it before including macros.hpp!"
#endif
#ifndef CUSTOM_TYPES_USE_CONCEPTS
// Declares a class with the given namespace, name, base namespace, base name, and baseSize..
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, baseSize, impl) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName), {}, false); \
    } \
}; \
namespace namespaze { \
    class name { \
        using ___Target__Type = name; \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static const Il2CppClass* klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        impl \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::klass); \
    } \
};

#else
// Declares a class with the given namespace, name, base namespace, base name, and baseSize..
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, baseSize, impl) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName), {}, false); \
    } \
}; \
namespace namespaze { \
    class name { \
        using ___Target__Type = name; \
        friend ::custom_types::Register; \
        public: \
        static const Il2CppClass* klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        impl \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::klass); \
    } \
};
#endif

#ifdef DECLARE_CLASS_INTERFACES
#error "DECLARE_CLASS_INTERFACES is already defined! Undefine it before including macros.hpp!"
#endif

#ifndef CUSTOM_TYPES_USE_CONCEPTS
// Declares a class with the given namespace, name, base namespace, base name, baseSize, and interface list.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_INTERFACES(namespaze, name, baseNamespaze, baseName, baseSize, interfaces, impl) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName), {interfaces}, false); \
    } \
}; \
namespace namespaze { \
    class name { \
        using ___Target__Type = name; \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static const Il2CppClass* klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        impl \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::klass); \
    } \
};
#else
// Declares a class with the given namespace, name, base namespace, base name, baseSize, and interface list.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_INTERFACES(namespaze, name, baseNamespaze, baseName, baseSize, interfaces, impl) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName), {interfaces}, false); \
    } \
}; \
namespace namespaze { \
    class name { \
        using ___Target__Type = name; \
        friend ::custom_types::Register; \
        public: \
        static const Il2CppClass* klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        impl \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::klass); \
    } \
};
#endif

#ifdef DECLARE_CLASS_CODEGEN
#error "DECLARE_CLASS_CODEGEN is already defined! Undefine it before including macros.hpp!"
#endif
#ifndef CUSTOM_TYPES_USE_CONCEPTS
// Declares a class with the given namespace, name, and base type.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_CODEGEN(namespaze, name, baseT, impl) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<baseT*>::get(), {}, false); \
    } \
}; \
namespace namespaze { \
    class name : public baseT { \
        using ___Target__Type = name; \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static const Il2CppClass* klass; \
        impl \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::klass); \
    } \
};
#else

// Declares a class with the given namespace, name, and base type.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_CODEGEN(namespaze, name, baseT, impl) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<baseT*>::get(), {}, false); \
    } \
}; \
namespace namespaze { \
    class name : public baseT { \
        using ___Target__Type = name; \
        friend ::custom_types::Register; \
        public: \
        static const Il2CppClass* klass; \
        impl \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::klass); \
    } \
};
#endif

// TODO: This is not yet implemented, due to extracting Il2CppClass*s from interface types via __VA_ARGS__
// #ifdef DECLARE_CLASS_CODEGEN_INTERFACES
// #error "DECLARE_CLASS_CODEGEN_INTERFACES is already defined! Undefine it before including macros.hpp!"
// #endif
// // Declares a class with the given namespace, name, base type, and interface types.
// // Assumes the class being declared is non-abstract.
// // impl specifies the implementation of the class, the actual definition of the type.
// // It is recommended this hold other DECLARE statements, as defined in macros.hpp
// #define DECLARE_CLASS_CODEGEN_INTERFACES(namespaze, name, baseT, interfaceTs, impl) \
// namespace namespaze { \
//     class name; \
// } \
// template<> \
// struct ::custom_types::name_registry<namespaze::name> { \
//     static inline ::custom_types::type_info* get() { \
//         _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
//         auto interfaces = ::il2cpp_utils::ExtractTypes<interfaceTs>(); \
//         return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, classof(baseT), interfaces, false); \
//     } \
// }; \
// namespace namespaze { \
//     class name : public baseT, interfaceTs { \
//         friend ::custom_types::Register; \
//         friend ::custom_types::has_func_register<name, void*>; \
//         public: \
//         static inline const Il2CppClass* klass = nullptr; \
//         impl \
//     }; \
// } \
// template<> \
// struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
//     static inline Il2CppClass* get() { \
//         return const_cast<Il2CppClass*>(::namespaze::name::klass); \
//     } \
// };


#ifdef DEFINE_CLASS
#error "DECLARE_CLASS_CODEGEN is already defined! Undefine it before including macros.hpp!"
#endif

#define DEFINE_CLASS(type) \
const Il2CppClass* type::klass = nullptr

// TODO: Add a way of declaring abstract/interface types.
// This requires messing with method slots even more than I do right now.

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
        ::il2cpp_functions::Init(); \
        auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
        uint16_t attrs = FIELD_ATTRIBUTE_PUBLIC; \
        return new ::custom_types::field_info(#name, t, attrs); \
    } \
    static inline constexpr bool isStatic() { \
        return false; \
    } \
    static inline auto offset() { \
        return offsetof(___Target__Type, name); \
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
        return new ::custom_types::field_info(#name, t, attrs); \
    } \
    static inline constexpr bool isStatic() { \
        return true; \
    } \
    static inline auto offset() { \
        return 0; \
    } \
}

#ifdef DECLARE_INSTANCE_FIELD_DEFAULT
#error "DECLARE_INSTANCE_FIELD_DEFAULT is already defined! Undefine it before including macros.hpp!"
#endif
// Declare a field with type, name and set it to value.
// Fields declared like this must also be registered via REGISTER_FIELD within the REGISTER_TYPE function.
#define DECLARE_INSTANCE_FIELD_DEFAULT(type, name, value) \
public: \
type name = value; \
private: \
struct field_wrapper_##name { \
    static inline ::custom_types::field_info* get() { \
        ::il2cpp_functions::Init(); \
        auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
        uint16_t attrs = FIELD_ATTRIBUTE_PUBLIC; \
        return new ::custom_types::field_info(#name, t, attrs); \
    } \
    static inline constexpr bool isStatic() { \
        return false; \
    } \
    static inline auto offset() { \
        return offsetof(___Target__Type, name); \
    } \
}

#ifdef __CREATE_METHOD_WRAPPER
#error "__CREATE_METHOD_WRAPPER is already defined! Undefine it before including macros.hpp!"
#endif

#define __CREATE_METHOD_WRAPPER(name, csharpName, flags, virtualData) \
private: \
template<typename __DeclType, class __Discard = void> \
struct method_wrapper_##name { \
    static inline ::custom_types::method_info* get() { \
        using memberPtr = decltype(&__DeclType::name); \
        using instanceClass = ::custom_types::method_info_template_instance<__DeclType, memberPtr>; \
        using staticClass = ::custom_types::method_info_template_static<__DeclType, memberPtr>; \
        const Il2CppType* ret; \
        std::vector<ParameterInfo> params; \
        uint16_t _flags = flags; \
        void* ptr; \
        InvokerMethod invoker = (InvokerMethod)&::custom_types::invoker_creator<memberPtr>::invoke; \
        if constexpr (::custom_types::has_get<instanceClass>) { \
            ret = instanceClass::get(); \
            params = instanceClass::get_params(); \
            ptr = (void*)&instanceClass::template wrap<&__DeclType::name>; \
        } else if constexpr (::custom_types::has_get<staticClass>) {\
            ret = staticClass::get(); \
            params = staticClass::get_params(); \
            _flags |= METHOD_ATTRIBUTE_STATIC; \
            ptr = (void*)&__DeclType::name; \
        } else { \
            static_assert(::custom_types::false_t<memberPtr>, "Must define either an instance or a static method! Could not match either!"); \
        } \
        return new ::custom_types::method_info(csharpName, ptr, invoker, ret, params, _flags, virtualData); \
    } \
}

#ifdef DECLARE_METHOD_SPECIAL
#error "DECLARE_METHOD_SPECIAL is already defined! Undefine it before including macros.hpp!"
#endif

#define DECLARE_METHOD_SPECIAL(methodPrefix, name, specialName, flags, ...) \
public: \
methodPrefix name(__VA_ARGS__); \
__CREATE_METHOD_WRAPPER(name, specialName, METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | flags, nullptr)

#define DECLARE_CTOR(name, ...) \
public: \
void name(__VA_ARGS__); \
__CREATE_METHOD_WRAPPER(name, ".ctor", METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_SPECIAL_NAME | METHOD_ATTRIBUTE_RT_SPECIAL_NAME, nullptr)

#ifdef DECLARE_METHOD
#error "DECLARE_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare a method with: methodPrefix, name, parameters...
// Methods declared like this must also be registered via REGISTER_METHOD within the REGISTER_FUNCTION macro.
// Yes, this inner structure really does have to be a template, much to my dismay
#define DECLARE_METHOD(methodPrefix, name, ...) \
public: \
methodPrefix name(__VA_ARGS__); \
__CREATE_METHOD_WRAPPER(name, #name, METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, nullptr)

#ifdef DECLARE_OVERRIDE_METHOD
#error "DECLARE_OVERRIDE_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare an interface method with: methodPrefix, name, method it is implementing, parameters...
// Methods declared like this must also be registered via REGISTER_METHOD within the REGISTER_FUNCTION macro.
// This macro matches the DECLARE_METHOD macro except it overrides the provided overriding MethodInfo*.
#define DECLARE_OVERRIDE_METHOD(methodPrefix, name, overridingMethodInfo, ...) \
public: \
methodPrefix name(__VA_ARGS__); \
__CREATE_METHOD_WRAPPER(name, #name, (overridingMethodInfo->flags & ~METHOD_ATTRIBUTE_ABSTRACT) | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, overridingMethodInfo)

#ifdef REGISTER_FUNCTION
#error "REGISTER_FUNCTION is already defined! Undefine it before including macros.hpp!"
#endif
// Creates static inline _register function used to register type within il2cpp
// TODO: Remove typeN from this call
#define REGISTER_FUNCTION(typeN, innards) \
static inline void _register(std::vector<::custom_types::field_info*>& fields, std::vector<::custom_types::field_info*>& staticFields, std::vector<::custom_types::method_info*>& methods) { \
    innards \
}

#ifdef REGISTER_FIELD
#error "REGISTER_FIELD is already defined! Undefine it before including macros.hpp!"
#endif
// Registers a field to be attached to this type.
// Must be called within the REGISTER_FUNCTION macro.
#define REGISTER_FIELD(name) \
do { \
    auto val = field_wrapper_##name::get(); \
    if constexpr (field_wrapper_##name::isStatic()) { \
        staticFields.push_back(std::move(val)); \
    } \
    val->setOffset(field_wrapper_##name::offset()); \
    fields.push_back(std::move(val)); \
} while (0)

#ifdef REGISTER_METHOD
#error "REGISTER_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Registers a method to be attached to this type.
// Must be called within the REGISTER_FUNCTION macro.
#define REGISTER_METHOD(name) \
methods.push_back(std::move(method_wrapper_##name<___Target__Type>::get()))
