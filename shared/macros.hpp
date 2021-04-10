#pragma once
#include "types.hpp"
#include <stdint.h>
#include <stddef.h>
#include <new>
#include <utility>
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

// Explicit macros defined here are laid out all here, ctrl-f for a specific name to find its impls.

#ifdef DECLARE_CLASS
#error "DECLARE_CLASS is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef DECLARE_CLASS_INTERFACES
#error "DECLARE_CLASS_INTERFACES is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef DECLARE_CLASS_CODEGEN
#error "DECLARE_CLASS_CODEGEN is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef DECLARE_CLASS_CODEGEN_INTERFACES
#error "DECLARE_CLASS_CODEGEN_INTERFACES is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef DECLARE_VALUE
#error "DECLARE_VALUE is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef DECLARE_VALUE_CODEGEN
#error "DECLARE_VALUE_CODEGEN is already defined! Undefine it before including macros.hpp!"
#endif

#ifndef CUSTOM_TYPES_USE_CONCEPTS
// Declares a class with the given namespace, name, base namespace, base name, and baseSize..
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, baseSize, ...) \
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
        constexpr static auto ___Base__Size = baseSize; \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static const Il2CppClass* __klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        public: \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a class with the given namespace, name, base namespace, base name, baseSize, and interface list.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_INTERFACES(namespaze, name, baseNamespaze, baseName, baseSize, interfaces, ...) \
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
        constexpr static auto ___Base__Size = baseSize; \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static const Il2CppClass* __klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        public: \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a class with the given namespace, name, and base type.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_CODEGEN(namespaze, name, baseT, ...) \
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
        constexpr static auto ___Base__Size = sizeof(baseT); \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static const Il2CppClass* __klass; \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a class with the given namespace, name, base type, and interface types.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
// TODO: Note that this type does NOT properly inherit its interfaces or provide conversion operators.
// Casts WILL be necessary.
#define DECLARE_CLASS_CODEGEN_INTERFACES(namespaze, name, baseT, interfaceTs, ...) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        auto interfaces = ::il2cpp_utils::ExtractTypes<interfaceTs>(); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<baseT*>::get(), interfaces, false); \
    } \
}; \
namespace namespaze { \
    class name : public baseT { \
        using ___Target__Type = name; \
        constexpr static auto ___Base__Size = sizeof(baseT); \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static inline const Il2CppClass* __klass = nullptr; \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a value type with the given namespace, name, base namespace, base name, and baseSize.
// Assumes the struct being declared is non-generic/abstract.
// impl specifies the implementation of the class, which is the actual definition of the type.
// It is recommended that this holds DECLARE statements as defined in macros.hpp, but fields can ultimately be of any type.
#define DECLARE_VALUE(namespaze, name, baseNamespaze, baseName, baseSize, ...) \
namespace namespaze { \
    struct name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName), {}, false); \
    } \
}; \
namespace namespaze { \
    struct name { \
        using ___Target__Type = name; \
        constexpr static auto ___Base__Size = baseSize; \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static const Il2CppClass* __klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        public: \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a value type with the given namespace, name, and base type.
// Assumes the struct being declared is non-abstract.
// impl specifies the implementation of the struct, the actual definition of the type.
// It is recommended that this holds DECLARE statements as defined in macros.hpp, but fields can ultimately be of any type.
#define DECLARE_VALUE_CODEGEN(namespaze, name, baseT, ...) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE, #namespaze, #name, ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<baseT>::get(), {}, false); \
    } \
}; \
namespace namespaze { \
    class name : public baseT { \
        using ___Target__Type = name; \
        constexpr static auto ___Base__Size = sizeof(baseT); \
        friend ::custom_types::Register; \
        friend ::custom_types::has_func_register<name, void*>; \
        public: \
        static const Il2CppClass* __klass; \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};
#else
// Declares a class with the given namespace, name, base namespace, base name, and baseSize..
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, baseSize, ...) \
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
        constexpr static auto ___Base__Size = baseSize; \
        friend ::custom_types::Register; \
        public: \
        static const Il2CppClass* __klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        public: \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a class with the given namespace, name, base namespace, base name, baseSize, and interface list.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_INTERFACES(namespaze, name, baseNamespaze, baseName, baseSize, interfaces, ...) \
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
        constexpr static auto ___Base__Size = baseSize; \
        friend ::custom_types::Register; \
        public: \
        static const Il2CppClass* __klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        public: \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a class with the given namespace, name, and base type.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_CODEGEN(namespaze, name, baseT, ...) \
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
        constexpr static auto ___Base__Size = sizeof(baseT); \
        friend ::custom_types::Register; \
        public: \
        static const Il2CppClass* __klass; \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a class with the given namespace, name, base type, and interface types.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
// TODO: Note that this type does NOT properly inherit its interfaces or provide conversion operators.
// Casts WILL be necessary.
#define DECLARE_CLASS_CODEGEN_INTERFACES(namespaze, name, baseT, interfaceTs, ...) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        auto interfaces = ::il2cpp_utils::ExtractTypes<interfaceTs>(); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<baseT*>::get(), interfaces, false); \
    } \
}; \
namespace namespaze { \
    class name : public baseT { \
        using ___Target__Type = name; \
        constexpr static auto ___Base__Size = sizeof(baseT); \
        friend ::custom_types::Register; \
        public: \
        static inline const Il2CppClass* __klass = nullptr; \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name*> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a value type with the given namespace, name, base namespace, base name, and baseSize.
// Assumes the struct being declared is non-generic/abstract.
// impl specifies the implementation of the struct, which is the actual definition of the type.
// It is recommended that this holds DECLARE statements as defined in macros.hpp, but fields can ultimately be of any type.
#define DECLARE_VALUE(namespaze, name, baseNamespaze, baseName, baseSize, ...) \
namespace namespaze { \
    struct name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName), {}, false); \
    } \
}; \
namespace namespaze { \
    struct name { \
        using ___Target__Type = name; \
        constexpr static auto ___Base__Size = baseSize; \
        friend ::custom_types::Register; \
        public: \
        static const Il2CppClass* __klass; \
        private: \
        uint8_t _baseFields[baseSize]; \
        public: \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};

// Declares a value type with the given namespace, name, and base type.
// Assumes the struct being declared is non-abstract.
// impl specifies the implementation of the struct, the actual definition of the type.
// It is recommended that this holds DECLARE statements as defined in macros.hpp, but fields can ultimately be of any type.
#define DECLARE_VALUE_CODEGEN(namespaze, name, baseT, ...) \
namespace namespaze { \
    class name; \
} \
template<> \
struct ::custom_types::name_registry<namespaze::name> { \
    static inline ::custom_types::type_info* get() { \
        _logger().debug("returning type_info for: %s::%s", #namespaze, #name); \
        return new ::custom_types::type_info(Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE, #namespaze, #name, ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<baseT>::get(), {}, false); \
    } \
}; \
namespace namespaze { \
    class name : public baseT { \
        using ___Target__Type = name; \
        constexpr static auto ___Base__Size = sizeof(baseT); \
        friend ::custom_types::Register; \
        public: \
        static const Il2CppClass* __klass; \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze::name> { \
    static inline Il2CppClass* get() { \
        return const_cast<Il2CppClass*>(::namespaze::name::__klass); \
    } \
};
#endif

#ifdef DEFINE_TYPE
#error "DEFINE_TYPE is already defined! Undefine it before including macros.hpp!"
#endif

#define DEFINE_TYPE(type) \
const Il2CppClass* type::__klass = nullptr

// TODO: Add a way of declaring abstract/interface types.
// This requires messing with method slots even more than I do right now.

#ifdef DECLARE_INSTANCE_FIELD_DEFAULT
#error "DECLARE_INSTANCE_FIELD_DEFAULT is already defined! Undefine it before including macros.hpp!"
#endif
// Declares a field with type, name, value.
// Fields declared like this must also be registered via REGISTER_FIELD within the REGISTER_TYPE function.
// Fields like this are ONLY initialized when the C++ constructor is called. See the INVOKE_CTOR macro for more info.
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
// TODO: Make static fields live in GC-aware memory, make references from static fields avoid GC.
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
#define REGISTER_FUNCTION(...) \
static inline void _register(std::vector<::custom_types::field_info*>& fields, std::vector<::custom_types::field_info*>& staticFields, std::vector<::custom_types::method_info*>& methods) { \
    __VA_ARGS__ \
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

#ifdef DECLARE_DTOR
#error "DECLARE_DTOR is already defined! Undefine it before including macros.hpp!"
#endif

// Declares a destructor with a given name for use when destructing non-trivial custom types.
// Should still be registered in the REGISTER_FUNCTION macro, just like any other method.
#define DECLARE_DTOR(name) \
void name(); \
__CREATE_METHOD_WRAPPER(name, #name, (::il2cpp_utils::FindMethod("System", "Object", "Finalize")->flags & ~METHOD_ATTRIBUTE_ABSTRACT) | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, ::il2cpp_utils::FindMethod("System", "Object", "Finalize"))

#ifdef DECLARE_SIMPLE_DTOR
#error "DECLARE_SIMPLE_DTOR is already defined! Undefine it before including macros.hpp!"
#endif

// Declares a simple destructor that simply forwards to the C++ destructor.
// This should be used if you do not wish to create your own destructor and explicitly call your own destructor.
// This method is __Finalize, and should not conflict with any existing declarations.
#define DECLARE_SIMPLE_DTOR() \
void __Finalize() { \
    this->~___Target__Type(); \
} \
__CREATE_METHOD_WRAPPER(__Finalize, "__Finalize", (::il2cpp_utils::FindMethod("System", "Object", "Finalize")->flags & ~METHOD_ATTRIBUTE_ABSTRACT) | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, ::il2cpp_utils::FindMethod("System", "Object", "Finalize"))

#ifdef REGISTER_SIMPLE_DTOR
#error "REGISTER_SIMPLE_DTOR is already defined! Undefine it before including macros.hpp!"
#endif

// Registers the simple dtor.
// Should only be used if DECLARE_SIMPLE_DTOR is also present.
#define REGISTER_SIMPLE_DTOR() REGISTER_METHOD(__Finalize)

#ifdef INVOKE_CTOR
#error "INVOKE_CTOR is already defined! Undefine it before including macros.hpp!"
#endif

// Invokes the C++ constructor of the provided typeName and arguments.
// Small performance overhead due to placement new.
// This can be done in order to explicitly initialize non-trivially constructible types, such as vectors
// within a C# ctor (declared with DECLARE_CTOR).
// This should ONLY be called on types that inherit Il2CppObject.
// For value types, try placement new instead, or INVOKE_VALUE_CTOR.
#define INVOKE_CTOR(...) \
do { \
char ___buff[___Base__Size]; \
memcpy(___buff, this, ___Base__Size); \
new (this) ___Target__Type(__VA_ARGS__); \
memcpy(this, ___buff, ___Base__Size); \
} while (0)

#ifdef INVOKE_VALUE_CTOR
#error "INVOKE_VALUE_CTOR is already defined! Undefine it before including macros.hpp!"
#endif

// Invokes the C++ constructor of the provided typeName and arguments.
// Small performance overhead due to placement new.
// This can be done in order to explicitly initialize non-trivially constructible types, such as vectors
// within a C# ctor (declared with DECLARE_CTOR).
// Note that value type constructors are much less likely to be called and no existing code will provide them.
// This function simply calls the placement new operator, so should NOT be used for anything that inherits Il2CppObject.
#define INVOKE_VALUE_CTOR(...) new (this) ___Target__Type(__VA_ARGS__)

#ifdef DECLARE_DEFAULT_CTOR()
#error "DECLARE_DEFAULT_CTOR is already defined! Undefine it before including macros.hpp!"
#endif

// Declares a default C# constructor that simply forwards to the C++ constructor.
// Note that this does NOT call any base constructors (which may or may not be necessary).
// This is ONLY valid if there is a default C++ constructor with 0 args.
// Otherwise, consider using DECLARE_CTOR and INVOKE_CTOR.
#define DECLARE_DEFAULT_CTOR() \
void __ctor() \
    INVOKE_CTOR(); \
} \
DECLARE_CTOR(__ctor)

#ifdef REGISTER_DEFAULT_CTOR
#error "REGISTER_DEFAULT_CTOR is already defined! Undefine it before including macros.hpp!"
#endif

// Registers a default C# constructor that simply forwards to the C++ constructor.
// Should only be used if DECLARE_DEFAULT_CTOR is also present.
#define REGISTER_DEFAULT_CTOR() REGISTER_METHOD(__ctor)