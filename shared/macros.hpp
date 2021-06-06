#pragma once
#include "types.hpp"
#include "register.hpp"
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

#ifdef DECLARE_CLASS_CUSTOM
#error "DECLARE_CLASS_CUSTOM is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef DECLARE_CLASS_CUSTOM_INTERFACES
#error "DECLARE_CLASS_CUSTOM_INTERFACES is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef DECLARE_VALUE
#error "DECLARE_VALUE is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef DECLARE_VALUE_CODEGEN
#error "DECLARE_VALUE_CODEGEN is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef ___DECLARE_TYPE_WRAPPER
#error "___DECLARE_TYPE_WRAPPER is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef ___DECLARE_TYPE_WRAPPER_INHERITANCE
#error "___DECLARE_TYPE_WRAPPER_INHERITANCE is already defined! Undefine it before including macros.hpp!"
#endif

// Helper macro for declaring classes with and without interfaces
#define ___DECLARE_TYPE_WRAPPER(namespaze_, name_, typeEnum_, baseNamespaze, baseName, baseSize, interfaces_, flags_, ...) \
namespace namespaze_ { \
    class name_; \
} \
namespace namespaze_ { \
    class name_ { \
        using ___TargetType = name_; \
        constexpr static auto ___Base__Size = baseSize; \
        friend ::custom_types::Register; \
        public: \
        struct ___TypeRegistration : ::custom_types::TypeRegistration { \
            ___TypeRegistration() { \
                ::custom_types::Register::AddType(this); \
                instance = this; \
            } \
            static std::vector<::custom_types::FieldRegistrator*> fields; \
            std::vector<::custom_types::FieldRegistrator*> const getFields() const override { \
                return fields; \
            } \
            static void addField(::custom_types::FieldRegistrator* inst) { \
                fields.push_back(inst); \
                ::custom_types::_logger().debug("Adding instance field: %s.%s", #name_, inst->name()); \
            } \
            static std::vector<::custom_types::StaticFieldRegistrator*> staticFields; \
            std::vector<::custom_types::StaticFieldRegistrator*> const getStaticFields() const override { \
                return staticFields; \
            } \
            static void addStaticFieldInstance(::custom_types::StaticFieldRegistrator* inst) { \
                staticFields.push_back(inst); \
                ::custom_types::_logger().debug("Adding static field: %s.%s", #name_, inst->name()); \
            } \
            static std::vector<::custom_types::MethodRegistrator*> methods; \
            std::vector<::custom_types::MethodRegistrator*> const getMethods() const override { \
                return methods; \
            } \
            static void addMethod(::custom_types::MethodRegistrator* inst) { \
                methods.push_back(inst); \
                ::custom_types::_logger().debug("Adding method: %s", inst->name()); \
            } \
            static size_t staticFieldOffset; \
            static size_t addStaticField(size_t sz) { \
                auto tmp = staticFieldOffset; \
                staticFieldOffset += sz; \
                return tmp; \
            } \
            static char* st_fields; \
            char*& static_fields() override { \
                return st_fields; \
            } \
            size_t static_fields_size() const override { \
                return staticFieldOffset; \
            } \
            constexpr const char* name() const override { \
                return #name_; \
            } \
            constexpr const char* namespaze() const override { \
                return #namespaze_; \
            } \
            Il2CppAssembly* assembly() const override {return nullptr;} \
            Il2CppClass* baseType() const override { \
                return ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName); \
            } \
            std::vector<Il2CppClass*> const interfaces() const override { \
                return interfaces_; \
            } \
            constexpr Il2CppTypeEnum typeEnum() const override { \
                return typeEnum_; \
            } \
            constexpr uint32_t typeFlags() const override { \
                return flags_; \
            } \
            static Il2CppClass* klass_ptr; \
            Il2CppClass*& klass() const override { \
                return klass_ptr; \
            } \
            size_t size() const override { \
                return sizeof(___TargetType); \
            } \
            TypeRegistration* customBase() const override { \
                return nullptr; \
            } \
            static TypeRegistration* instance; \
            static TypeRegistration* get() { \
                return instance; \
            } \
        }; \
        private: \
        uint8_t _baseFields[baseSize]; \
        public: \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze_::name_*> { \
    static inline Il2CppClass* get() { \
        return ::namespaze_::name_::___TypeRegistration::klass_ptr; \
    } \
};

// Helper for declaring classes with and without interfaces with explicit inheritance
#define ___DECLARE_TYPE_WRAPPER_INHERITANCE(namespaze_, name_, typeEnum_, baseT, interfaces_, flags_, baseCustom, ...) \
namespace namespaze_ { \
    class name_; \
} \
namespace namespaze_ { \
    class name_ : public baseT { \
        using ___TargetType = name_; \
        constexpr static auto ___Base__Size = sizeof(baseT); \
        friend ::custom_types::Register; \
        public: \
        struct ___TypeRegistration : ::custom_types::TypeRegistration { \
            ___TypeRegistration() { \
                ::custom_types::Register::AddType(this); \
                instance = this; \
            } \
            static std::vector<::custom_types::FieldRegistrator*> fields; \
            std::vector<::custom_types::FieldRegistrator*> const getFields() const override { \
                return fields; \
            } \
            static void addField(::custom_types::FieldRegistrator* inst) { \
                fields.push_back(inst); \
                ::custom_types::_logger().debug("Adding instance field: %s.%s", #name_, inst->name()); \
            } \
            static std::vector<::custom_types::StaticFieldRegistrator*> staticFields; \
            std::vector<::custom_types::StaticFieldRegistrator*> const getStaticFields() const override { \
                return staticFields; \
            } \
            static void addStaticFieldInstance(::custom_types::StaticFieldRegistrator* inst) { \
                staticFields.push_back(inst); \
                ::custom_types::_logger().debug("Adding static field: %s.%s", #name_, inst->name()); \
            } \
            static std::vector<::custom_types::MethodRegistrator*> methods; \
            std::vector<::custom_types::MethodRegistrator*> const getMethods() const override { \
                return methods; \
            } \
            static void addMethod(::custom_types::MethodRegistrator* inst) { \
                methods.push_back(inst); \
                ::custom_types::_logger().debug("Adding method: %s.%s", #name_, inst->name()); \
            } \
            static size_t staticFieldOffset; \
            static size_t addStaticField(size_t sz) { \
                auto tmp = staticFieldOffset; \
                staticFieldOffset += sz; \
                return tmp; \
            } \
            static char* st_fields; \
            char*& static_fields() override { \
                return st_fields; \
            } \
            size_t static_fields_size() const override { \
                return staticFieldOffset; \
            } \
            constexpr const char* name() const override { \
                return #name_; \
            } \
            constexpr const char* namespaze() const override { \
                return #namespaze_; \
            } \
            Il2CppAssembly* assembly() const override {return nullptr;} \
            Il2CppClass* baseType() const override { \
                return ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<baseT*>::get(); \
            } \
            std::vector<Il2CppClass*> const interfaces() const override { \
                return interfaces_; \
            } \
            constexpr Il2CppTypeEnum typeEnum() const override { \
                return typeEnum_; \
            } \
            constexpr uint32_t typeFlags() const override { \
                return flags_; \
            } \
            static Il2CppClass* klass_ptr; \
            Il2CppClass*& klass() const override { \
                return klass_ptr; \
            } \
            size_t size() const override { \
                return sizeof(___TargetType); \
            } \
            TypeRegistration* customBase() const override { \
                return baseCustom; \
            } \
            static TypeRegistration* instance; \
            static TypeRegistration* get() { \
                return instance; \
            } \
        }; \
        public: \
        __VA_ARGS__ \
    }; \
} \
template<> \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::namespaze_::name_*> { \
    static inline Il2CppClass* get() { \
        return ::namespaze_::name_::___TypeRegistration::klass_ptr; \
    } \
};

// Declares a class with the given namespace, name, base namespace, base name, and baseSize..
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, baseSize, ...) \
___DECLARE_TYPE_WRAPPER(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, baseNamespaze, baseName, baseSize, {}, 0, __VA_ARGS__)

// Declares a class with the given namespace, name, base namespace, base name, baseSize, and interface list.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this holds DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_INTERFACES(namespaze, name, baseNamespaze, baseName, baseSize, interfaces, ...) \
___DECLARE_TYPE_WRAPPER(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, baseNamespaze, baseName, baseSize, interfaces, 0, __VA_ARGS__)

// Declares a class with the given namespace, name, and base type.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_CODEGEN(namespaze, name, baseT, ...) \
___DECLARE_TYPE_WRAPPER_INHERITANCE(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, baseT, {}, 0, nullptr, __VA_ARGS__)

// Declares a class with the given namespace, name, base type, and interface types.
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
// TODO: Note that this type does NOT properly inherit its interfaces or provide conversion operators.
// Casts WILL be necessary.
#define DECLARE_CLASS_CODEGEN_INTERFACES(namespaze, name, baseT, interfaceTs, ...) \
___DECLARE_TYPE_WRAPPER_INHERITANCE(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, baseT, ::il2cpp_utils::ExtractTypes<interfaceTs>(), 0, nullptr, __VA_ARGS__)

// Declares a class with the given namespace, name, and base type (WHICH MUST BE A DECLARE TYPE OF ITS OWN!)
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_CUSTOM(namespaze, name, baseCustomT, ...) \
___DECLARE_TYPE_WRAPPER_INHERITANCE(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, baseCustomT, {}, 0, baseCustomT::___TypeRegistration::get(), __VA_ARGS__)

// Declares a class with the given namespace, name, and base type (WHICH MUST BE A DECLARE TYPE OF ITS OWN!)
// Assumes the class being declared is non-abstract.
// impl specifies the implementation of the class, the actual definition of the type.
// It is recommended this hold other DECLARE statements, as defined in macros.hpp
#define DECLARE_CLASS_CUSTOM_INTERFACES(namespaze, name, baseCustomT, interfaceTs, ...) \
___DECLARE_TYPE_WRAPPER_INHERITANCE(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, baseCustomT, ::il2cpp_utils::ExtractTypes<interfaceTs>(), 0, baseCustomT::___TypeRegistration::get(), __VA_ARGS__)

// Declares a value type with the given namespace, name, base namespace, base name, and baseSize.
// Assumes the struct being declared is non-generic/abstract.
// impl specifies the implementation of the class, which is the actual definition of the type.
// It is recommended that this holds DECLARE statements as defined in macros.hpp, but fields can ultimately be of any type.
#define DECLARE_VALUE(namespaze, name, baseNamespaze, baseName, baseSize, ...) \
___DECLARE_TYPE_WRAPPER(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE, baseNamespaze, baseName, baseSize, {}, 0, __VA_ARGS__)

// Declares a value type with the given namespace, name, and base type.
// Assumes the struct being declared is non-abstract.
// impl specifies the implementation of the struct, the actual definition of the type.
// It is recommended that this holds DECLARE statements as defined in macros.hpp, but fields can ultimately be of any type.
#define DECLARE_VALUE_CODEGEN(namespaze, name, baseT, ...) \
___DECLARE_TYPE_WRAPPER_INHERITANCE(namespaze, name, Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE, baseT, {}, 0, nullptr, __VA_ARGS__)

#ifdef DEFINE_TYPE
#error "DEFINE_TYPE is already defined! Undefine it before including macros.hpp!"
#endif

#define DEFINE_TYPE(namespaze, name) \
::custom_types::TypeRegistration* namespaze::name::___TypeRegistration::instance; \
static namespaze::name::___TypeRegistration __registration_instance_##name; \
char* namespaze::name::___TypeRegistration::st_fields; \
Il2CppClass* namespaze::name::___TypeRegistration::klass_ptr; \
std::vector<::custom_types::FieldRegistrator*> namespaze::name::___TypeRegistration::fields; \
std::vector<::custom_types::StaticFieldRegistrator*> namespaze::name::___TypeRegistration::staticFields; \
std::vector<::custom_types::MethodRegistrator*> namespaze::name::___TypeRegistration::methods; \
size_t namespaze::name::___TypeRegistration::staticFieldOffset = 0

// TODO: Add a way of declaring abstract/interface types.
// This requires messing with method slots even more than I do right now.

#ifdef DECLARE_INSTANCE_FIELD_DEFAULT
#error "DECLARE_INSTANCE_FIELD_DEFAULT is already defined! Undefine it before including macros.hpp!"
#endif
// Declares a field with type, name, value.
// Fields declared like this must also be registered via REGISTER_FIELD within the REGISTER_TYPE function.
// Fields like this are ONLY initialized when the C++ constructor is called. See the INVOKE_CTOR macro for more info.
#define DECLARE_INSTANCE_FIELD_DEFAULT(type_, name_, value) \
private: \
struct ___FieldRegistrator_##name_ : ::custom_types::FieldRegistrator { \
    ___FieldRegistrator_##name_() { \
        ___TargetType::___TypeRegistration::addField(this); \
    } \
    constexpr const char* name() const override { \
        return #name_; \
    } \
    const Il2CppType* type() const override { \
        ::il2cpp_functions::Init(); \
        return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type_>::get()); \
    } \
    constexpr uint16_t fieldAttributes() const override { \
        return FIELD_ATTRIBUTE_PUBLIC; \
    } \
    constexpr size_t size() const override { \
        return sizeof(type_); \
    } \
}; \
static inline ___FieldRegistrator_##name_ ___##name_##_FieldRegistrator; \
public: \
type_ name_ = value

#ifdef DECLARE_INSTANCE_FIELD
#error "DECLARE_INSTANCE_FIELD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare a field with type, name.
#define DECLARE_INSTANCE_FIELD(type_, name_) \
private: \
struct ___FieldRegistrator_##name_ : ::custom_types::FieldRegistrator { \
    ___FieldRegistrator_##name_() { \
        ___TargetType::___TypeRegistration::addField(this); \
    } \
    constexpr const char* name() const override { \
        return #name_; \
    } \
    const Il2CppType* type() const override { \
        ::il2cpp_functions::Init(); \
        return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type_>::get()); \
    } \
    constexpr uint16_t fieldAttributes() const override { \
        return FIELD_ATTRIBUTE_PUBLIC; \
    } \
    constexpr size_t size() const override { \
        return sizeof(type_); \
    } \
}; \
static inline ___FieldRegistrator_##name_ ___##name_##_FieldRegistrator; \
public: \
type_ name_

#ifdef DECLARE_STATIC_FIELD
#error "DECLARE_STATIC_FIELD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare a static field with type, name.
// Static fields declared like this are GC aware, but are uninitialized on class creation.
// Consider creating a pseudo static initializer in which you initialize these fields to whatever you desire.
#define DECLARE_STATIC_FIELD(type_, name_) \
private: \
struct ___StaticFieldRegistrator_##name_ : ::custom_types::StaticFieldRegistrator { \
    size_t oft; \
    ___StaticFieldRegistrator_##name_() { \
        oft = ___TargetType::___TypeRegistration::addStaticField(size()); \
        ___TargetType::___TypeRegistration::addStaticFieldInstance(this); \
    } \
    constexpr const char* name() const override { \
        return #name_; \
    } \
    const Il2CppType* type() const override { \
        ::il2cpp_functions::Init(); \
        return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type_>::get()); \
    } \
    constexpr uint16_t fieldAttributes() const override { \
        return FIELD_ATTRIBUTE_PUBLIC | FIELD_ATTRIBUTE_STATIC; \
    } \
    constexpr size_t size() const override { \
        return sizeof(type_); \
    } \
    int32_t offset() const override { \
        return oft; \
    } \
}; \
static inline ___StaticFieldRegistrator_##name_ ___##name_##_StaticFieldRegistrator; \
public: \
static type_& name_() { \
    CRASH_UNLESS(___TargetType::___TypeRegistration::st_fields); \
    return *reinterpret_cast<type_*>(&___TargetType::___TypeRegistration::st_fields[___##name_##_StaticFieldRegistrator.offset()]); \
} \

#ifdef ___CREATE_INSTANCE_METHOD
#error "___CREATE_INSTANCE_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Helper macro for creating instance methods
#define ___CREATE_INSTANCE_METHOD(name_, csharpName_, flags_, virtualData) \
private: \
template<typename T> \
struct ___MethodRegistrator_##name_; \
template<typename R, typename T, typename... TArgs> \
struct ___MethodRegistrator_##name_<R (T::*)(TArgs...)> : ::custom_types::MethodRegistrator { \
    ___MethodRegistrator_##name_() { \
        ___TargetType::___TypeRegistration::addMethod(this); \
    } \
    constexpr const char* name() const override { \
        return #name_; \
    } \
    constexpr const char* csharpName() const override { \
        return csharpName_; \
    } \
    int flags() const override { \
        return flags_; \
    } \
    const MethodInfo* virtualMethod() const override { \
        return virtualData; \
    } \
    const Il2CppType* returnType() const override { \
        il2cpp_functions::Init(); \
        return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<R>::get()); \
    } \
    std::vector<ParameterInfo> params() const override { \
        int32_t counter = 0; \
        il2cpp_functions::Init(); \
        return {(ParameterInfo{"param", counter++, static_cast<uint32_t>(-1), ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TArgs>::get())})...}; \
    } \
    uint8_t params_size() const override { \
        return sizeof...(TArgs); \
    } \
    Il2CppMethodPointer methodPointer() const override { \
        return reinterpret_cast<Il2CppMethodPointer>(&::custom_types::invoker_creator<decltype(&___TargetType::name_)>::wrap<&___TargetType::name_>); \
    } \
    InvokerMethod invoker() const override { \
        return &::custom_types::invoker_creator<decltype(&___TargetType::name_)>::invoke; \
    } \
}; \
static inline ___MethodRegistrator_##name_<decltype(&___TargetType::name_)> ___##name_##_MethodRegistrator

#ifdef DECLARE_INSTANCE_METHOD_SPECIAL
#error "DECLARE_INSTANCE_METHOD_SPECIAL is already defined! Undefine it before including macros.hpp!"
#endif
// Declare an instance method with a special name.
// See DECLARE_INSTANCE_METHOD for more information.
#define DECLARE_INSTANCE_METHOD_SPECIAL(ret, name, specialName, flags, ...) \
public: \
ret name(__VA_ARGS__); \
___CREATE_INSTANCE_METHOD(name, specialName, flags | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, nullptr)

#ifdef DECLARE_CTOR
#error "DECLARE_CTOR is already defined! Undefine it before including macros.hpp!"
#endif
// Declare a method with name that will be called on construction.
#define DECLARE_CTOR(name, ...) \
public: \
void name(__VA_ARGS__); \
___CREATE_INSTANCE_METHOD(name, ".ctor", METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_SPECIAL_NAME | METHOD_ATTRIBUTE_RT_SPECIAL_NAME, nullptr)

#ifdef DECLARE_INSTANCE_METHOD
#error "DECLARE_INSTANCE_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare an instance method with: return type, name, parameters...
#define DECLARE_INSTANCE_METHOD(ret, name, ...) \
public: \
ret name(__VA_ARGS__); \
___CREATE_INSTANCE_METHOD(name, #name, METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, nullptr)

#ifdef DECLARE_OVERRIDE_METHOD
#error "DECLARE_OVERRIDE_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare an interface method with: return type, name, method it is implementing, parameters...
// This macro matches the DECLARE_INSTANCE_METHOD macro except it overrides the provided overriding MethodInfo*.
#define DECLARE_OVERRIDE_METHOD(ret, name, overridingMethodInfo, ...) \
public: \
ret name(__VA_ARGS__); \
___CREATE_INSTANCE_METHOD(name, #name, (overridingMethodInfo->flags & ~METHOD_ATTRIBUTE_ABSTRACT) | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, overridingMethodInfo)

#ifdef DECLARE_DTOR
#error "DECLARE_DTOR is already defined! Undefine it before including macros.hpp!"
#endif
// Declares a destructor with a given name for use when destructing non-trivial custom types.
// Should still be registered in the REGISTER_FUNCTION macro, just like any other method.
#define DECLARE_DTOR(name) \
public: \
void name(); \
___CREATE_INSTANCE_METHOD(name, #name, METHOD_ATTRIBUTE_VIRTUAL | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, ::il2cpp_utils::FindMethod("System", "Object", "Finalize"))

#ifdef DECLARE_SIMPLE_DTOR
#error "DECLARE_SIMPLE_DTOR is already defined! Undefine it before including macros.hpp!"
#endif
// Declares a simple destructor that simply forwards to the C++ destructor.
// This should be used if you do not wish to create your own destructor and explicitly call your own destructor.
// This method is __Finalize, and should not conflict with any existing declarations.
#define DECLARE_SIMPLE_DTOR() \
void __Finalize() { \
    this->~___TargetType(); \
} \
___CREATE_INSTANCE_METHOD(__Finalize, "__Finalize", (::il2cpp_utils::FindMethod("System", "Object", "Finalize")->flags & ~METHOD_ATTRIBUTE_ABSTRACT) | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG, ::il2cpp_utils::FindMethod("System", "Object", "Finalize"))

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
new (this) ___TargetType(__VA_ARGS__); \
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
#define INVOKE_VALUE_CTOR(...) new (this) ___TargetType(__VA_ARGS__)

#ifdef DECLARE_DEFAULT_CTOR
#error "DECLARE_DEFAULT_CTOR is already defined! Undefine it before including macros.hpp!"
#endif
// Declares a default C# constructor that simply forwards to the C++ constructor.
// Note that this does NOT call any base constructors (which may or may not be necessary).
// This is ONLY valid if there is a default C++ constructor with 0 args.
// Otherwise, consider using DECLARE_CTOR and INVOKE_CTOR.
#define DECLARE_DEFAULT_CTOR() \
void __ctor() { \
    INVOKE_CTOR(); \
} \
___CREATE_INSTANCE_METHOD(__ctor, ".ctor", METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_SPECIAL_NAME | METHOD_ATTRIBUTE_RT_SPECIAL_NAME, nullptr)

#ifdef ___CREATE_STATIC_METHOD
#error "___CREATE_STATIC_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Helper macro for creating instance methods
#define ___CREATE_STATIC_METHOD(name_, csharpName_, flags_) \
private: \
template<typename T> \
struct ___MethodRegistrator_##name_; \
template<typename R, typename... TArgs> \
struct ___MethodRegistrator_##name_<R (*)(TArgs...)> : ::custom_types::MethodRegistrator { \
    ___MethodRegistrator_##name_() { \
        ___TargetType::___TypeRegistration::addMethod(this); \
    } \
    constexpr const char* name() const override { \
        return #name_; \
    } \
    constexpr const char* csharpName() const override { \
        return csharpName_; \
    } \
    int flags() const override { \
        return flags_; \
    } \
    const MethodInfo* virtualMethod() const override { \
        return nullptr; \
    } \
    const Il2CppType* returnType() const override { \
        il2cpp_functions::Init(); \
        return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<R>::get()); \
    } \
    std::vector<ParameterInfo> params() const override { \
        int32_t counter = 0; \
        il2cpp_functions::Init(); \
        return {(ParameterInfo{"param", counter++, static_cast<uint32_t>(-1), ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TArgs>::get())})...}; \
    } \
    uint8_t params_size() const override { \
        return sizeof...(TArgs); \
    } \
    Il2CppMethodPointer methodPointer() const override { \
        return reinterpret_cast<Il2CppMethodPointer>(&___TargetType::name_); \
    } \
    InvokerMethod invoker() const override { \
        return &::custom_types::invoker_creator<decltype(&___TargetType::name_)>::invoke; \
    } \
}; \
static inline ___MethodRegistrator_##name_<decltype(&___TargetType::name_)> ___##name_##_MethodRegistrator

#ifdef DECLARE_STATIC_METHOD
#error "DECLARE_STATIC_METHOD is already defined! Undefine it before including macros.hpp!"
#endif
// Declare an instance method with: return type, name, parameters...
#define DECLARE_STATIC_METHOD(ret, name, ...) \
public: \
static ret name(__VA_ARGS__); \
___CREATE_STATIC_METHOD(name, #name, METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_STATIC)