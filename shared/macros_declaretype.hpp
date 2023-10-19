#pragma once

#ifdef ___DECLARE_TYPE_WRAPPER
#error "___DECLARE_TYPE_WRAPPER is already defined! Undefine it before including macros.hpp!"
#endif

#ifdef ___DECLARE_TYPE_WRAPPER_INHERITANCE
#error "___DECLARE_TYPE_WRAPPER_INHERITANCE is already defined! Undefine it before including macros.hpp!"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Helper macro for declaring classes with and without interfaces
#define ___DECLARE_TYPE_WRAPPER(_mNamespace, _mName, _mIl2cppTypeEnum, _mBaseNamespace, _mBaseName, _mBaseSize, _mDllName, _mInterfaces, _mIl2cppTypeFlags, ...)                                                        \
___DECLARE_TYPE_FWD_DECL(_mNamespace, _mName)                                                                                                                                                                           \
namespace _mNamespace {                                                                                                                                                                                                 \
    class _mName {                                                                                                                                                                                                      \
        ___DECLARE_TYPE_TYPE_REGISTRATION(_mDllName, #_mNamespace, _mName, _mIl2cppTypeEnum, _mIl2cppTypeFlags, _mBaseSize, ::il2cpp_utils::GetClassFromName(_mBaseNamespace, _mBaseName), nullptr, _mInterfaces)       \
        uint8_t _baseFields[_mBaseSize];                                                                                                                                                                                \
        public:                                                                                                                                                                                                         \
        __VA_ARGS__                                                                                                                                                                                                     \
    };                                                                                                                                                                                                                  \
}                                                                                                                                                                                                                       \
___DECLARE_TYPE_TYPECHECK_DECL(_mNamespace::_mName)

// Helper for declaring classes with and without interfaces with explicit inheritance
#define ___DECLARE_TYPE_WRAPPER_INHERITANCE(_mNamespace, _mName, _mIl2cppTypeEnum, _mBaseType, _mDllName, _mInterfaces, _mIl2cppTypeFlags, _mCustomBase, ...)                                                                               \
___DECLARE_TYPE_FWD_DECL(_mNamespace, _mName)                                                                                                                                                                                               \
namespace _mNamespace {                                                                                                                                                                                                                     \
    class _mName : public _mBaseType {                                                                                                                                                                                                      \
        ___DECLARE_TYPE_TYPE_REGISTRATION(_mDllName, #_mNamespace, _mName, _mIl2cppTypeEnum, _mIl2cppTypeFlags, sizeof(_mBaseType), ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<_mBaseType*>::get(), _mCustomBase, _mInterfaces) \
        public:                                                                                                                                                                                                                             \
        __VA_ARGS__                                                                                                                                                                                                                         \
    };                                                                                                                                                                                                                                      \
}                                                                                                                                                                                                                                           \
___DECLARE_TYPE_TYPECHECK_DECL(_mNamespace::_mName)

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define ___DECLARE_TYPE_FWD_DECL(_mNamespace, _mName)   \
namespace _mNamespace {                                 \
    class _mName;                                       \
}

#define ___DECLARE_TYPE_TYPE_REGISTRATION(_mDllName, _mNamespaceStr, _mName, _mIl2cppTypeEnum, _mIl2cppTypeFlags, _mBaseSize, _mBaseTypeGetter, _mCustomBase, _mInterfaces) \
        using ___TargetType = _mName;                                                                                                                                       \
        constexpr static auto ___Base__Size = _mBaseSize;                                                                                                                   \
        friend ::custom_types::Register;                                                                                                                                    \
        public:                                                                                                                                                             \
        struct ___TypeRegistration : ::custom_types::TypeRegistration {                                                                                                     \
            ___TypeRegistration() {                                                                                                                                         \
                ::custom_types::Register::AddType(this);                                                                                                                    \
                instance = this;                                                                                                                                            \
            }                                                                                                                                                               \
            static inline std::vector<::custom_types::FieldRegistrator*> fields;                                                                                            \
            std::vector<::custom_types::FieldRegistrator*> const getFields() const override {                                                                               \
                return fields;                                                                                                                                              \
            }                                                                                                                                                               \
            static void addField(::custom_types::FieldRegistrator* inst) {                                                                                                  \
                fields.push_back(inst);                                                                                                                                     \
                ::custom_types::_logger().debug("Adding instance field: %s.%s new size: %lu", #_mName, inst->name(), fields.size());                                        \
            }                                                                                                                                                               \
            static inline std::vector<::custom_types::StaticFieldRegistrator*> staticFields;                                                                                \
            std::vector<::custom_types::StaticFieldRegistrator*> const getStaticFields() const override {                                                                   \
                return staticFields;                                                                                                                                        \
            }                                                                                                                                                               \
            static void addStaticFieldInstance(::custom_types::StaticFieldRegistrator* inst) {                                                                              \
                staticFields.push_back(inst);                                                                                                                               \
                ::custom_types::_logger().debug("Adding static field: %s.%s new size: %lu", #_mName, inst->name(), staticFields.size());                                    \
            }                                                                                                                                                               \
            static inline std::vector<::custom_types::MethodRegistrator*> methods;                                                                                          \
            std::vector<::custom_types::MethodRegistrator*> const getMethods() const override {                                                                             \
                return methods;                                                                                                                                             \
            }                                                                                                                                                               \
            static void addMethod(::custom_types::MethodRegistrator* inst) {                                                                                                \
                methods.push_back(inst);                                                                                                                                    \
                ::custom_types::_logger().debug("Adding method: %s.%s new size: %lu", #_mName, inst->name(), methods.size());                                               \
            }                                                                                                                                                               \
            static inline size_t staticFieldOffset;                                                                                                                         \
            static size_t addStaticField(size_t sz) {                                                                                                                       \
                auto tmp = staticFieldOffset;                                                                                                                               \
                staticFieldOffset += sz;                                                                                                                                    \
                return tmp;                                                                                                                                                 \
            }                                                                                                                                                               \
            static char* st_fields;                                                                                                                                         \
            char*& static_fields() override {                                                                                                                               \
                return st_fields;                                                                                                                                           \
            }                                                                                                                                                               \
            size_t static_fields_size() const override {                                                                                                                    \
                return staticFieldOffset;                                                                                                                                   \
            }                                                                                                                                                               \
            constexpr const char* name() const override {                                                                                                                   \
                return #_mName;                                                                                                                                             \
            }                                                                                                                                                               \
            constexpr const char* namespaze() const override {                                                                                                              \
                return _mNamespaceStr;                                                                                                                                      \
            }                                                                                                                                                               \
            constexpr const char* dllName() const override {                                                                                                                \
                return _mDllName;                                                                                                                                           \
            }                                                                                                                                                               \
            Il2CppClass* baseType() const override {                                                                                                                        \
                return _mBaseTypeGetter;                                                                                                                                    \
            }                                                                                                                                                               \
            std::vector<Il2CppClass*> const interfaces() const override {                                                                                                   \
                return _mInterfaces;                                                                                                                                        \
            }                                                                                                                                                               \
            constexpr Il2CppTypeEnum typeEnum() const override {                                                                                                            \
                return _mIl2cppTypeEnum;                                                                                                                                    \
            }                                                                                                                                                               \
            constexpr uint32_t typeFlags() const override {                                                                                                                 \
                return _mIl2cppTypeFlags;                                                                                                                                   \
            }                                                                                                                                                               \
            static Il2CppClass* klass_ptr;                                                                                                                                  \
            Il2CppClass*& klass() const override {                                                                                                                          \
                return klass_ptr;                                                                                                                                           \
            }                                                                                                                                                               \
            size_t size() const override {                                                                                                                                  \
                return sizeof(___TargetType);                                                                                                                               \
            }                                                                                                                                                               \
            TypeRegistration* customBase() const override {                                                                                                                 \
                return _mCustomBase;                                                                                                                                        \
            }                                                                                                                                                               \
            bool initialized() const override {                                                                                                                             \
                return init;                                                                                                                                                \
            }                                                                                                                                                               \
            void setInitialized() const override {                                                                                                                          \
                init = true;                                                                                                                                                \
            }                                                                                                                                                               \
            static bool init;                                                                                                                                               \
            static TypeRegistration* instance;                                                                                                                              \
            static TypeRegistration* get() {                                                                                                                                \
                return instance;                                                                                                                                            \
            }                                                                                                                                                               \
        };

#define ___DECLARE_TYPE_TYPECHECK_DECL(_mFullName)                                          \
template<>                                                                                  \
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<::_mFullName*> {              \
    static inline Il2CppClass* get() {                                                      \
        return ::_mFullName::___TypeRegistration::klass_ptr;                                \
    }                                                                                       \
};                                                                                          \
template<>                                                                                  \
struct ::il2cpp_utils::il2cpp_type_check::need_box<::_mFullName> {                          \
    constexpr static bool value = false;                                                    \
};

#define __DECLARE_TYPE_DEFINE(_mNamespace, _mName)                                      \
::custom_types::TypeRegistration* _mNamespace::_mName::___TypeRegistration::instance;   \
static _mNamespace::_mName::___TypeRegistration __registration_instance_##_mName;       \
char* _mNamespace::_mName::___TypeRegistration::st_fields;                              \
Il2CppClass* _mNamespace::_mName::___TypeRegistration::klass_ptr;                       \
bool _mNamespace::_mName::___TypeRegistration::init = false;