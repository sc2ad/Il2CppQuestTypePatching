#pragma once
#include <dlfcn.h>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include <utility>
#include <string_view>
#include <memory>
#include <vector>

struct Il2CppType;
struct ParameterInfo;
struct Il2CppClass;
struct FieldInfo;
struct MethodInfo;

namespace custom_types {
    class Register;
    class ClassWrapper;
}

struct type_info {
    friend ::custom_types::Register;
    friend ::custom_types::ClassWrapper;
    private:
    Il2CppTypeEnum typeEnum;
    Il2CppClass* base;
    std::string namespaze;
    std::string name;
    public:
    type_info(Il2CppTypeEnum typeE, std::string_view ns, std::string_view n, Il2CppClass* b);
};

struct method_info {
    friend ::custom_types::Register;
    friend ::custom_types::ClassWrapper;
    private:
    std::vector<ParameterInfo> params;
    MethodInfo* info;
    constexpr void setClass(Il2CppClass* klass) const {
        info->klass = klass;
    }
    public:
    // TODO: Figure out a way to dynamically get the invoker_function
    method_info(std::string_view name, void* func, InvokerMethod invoker, const Il2CppType* returnType, std::vector<ParameterInfo>& parameters, uint16_t flags);
    ~method_info();
    constexpr const MethodInfo* get() const {
        return info;
    }
};

struct field_info {
    friend ::custom_types::Register;
    friend ::custom_types::ClassWrapper;
    private:
    FieldInfo info;
    public:
    // Offset obtained via macro, as are fieldAttrs
    field_info(std::string_view name, const Il2CppType* type, int32_t offset, uint16_t fieldAttrs);
    ~field_info();
    constexpr const FieldInfo get() const {
        return info;
    }
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
    using get_type = decltype(T::get());

    template <class...> constexpr std::false_type false_t{};

    template<typename T>
    constexpr bool has_get = std::experimental::is_detected_v<get_type, T>;

    // Helper for checking if a type has a _register function
    // https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature/10707822
    template<typename, typename T>
    struct has_func_register;

    template<typename C, typename Ret, typename... Args>
    struct has_func_register<C, Ret(Args...)> {
    private:
        template<typename T>
        static constexpr auto check(T*)
        -> typename
            std::is_same<
                decltype(std::declval<T>()._register(std::declval<Args>()...)),
                Ret
            >::type;

        template<typename>
        static constexpr std::false_type check(...);

        typedef decltype(check<C>(0)) type;

    public:
        static constexpr bool value = type::value;
    };
    #else
    #error No libraries for the implementation of "has_" anything available!
    #endif

    template<typename T>
    struct method_info_template_instance {};

    template<typename T>
    struct method_info_template_static {};

    template<typename T>
    struct name_registry {};

    template<typename T> struct type_tag {};

    template<typename... Ps>
    struct parameter_converter;

    // Create a vector of ParameterInfo objects (good ol tail recursion)
    // 1 or more parameters
    template<typename P, typename... Ps>
    struct parameter_converter<P, Ps...> {
        static inline std::vector<ParameterInfo> get() {
            std::vector<ParameterInfo> params;
            auto& info = params.emplace_back();
            il2cpp_functions::Init();
            auto type = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<P>::get());
            // Ignore name, it will be set when we iterate over all of them (param_1, param_2, etc.)
            // Ignore position, it will also be set on iteration.
            // TODO: Maybe some day we can actually use the parameters names themselves!
            info.parameter_type = type;
            info.token = -1;
            for (const auto& q : parameter_converter<Ps...>::get()) {
                params.push_back(q);
            }
            return std::move(params);
        }
    };

    // 0 parameters
    template<>
    struct parameter_converter<> {
        static inline std::vector<ParameterInfo> get() {
            return std::vector<ParameterInfo>();
        }
    };

    void* allocate(std::size_t size);

    // These concepts originally created by DaNike, modifications made by Sc2ad
    template<typename>
    struct invoker_creator {};

    struct arg_helper {
        template<typename Q>
        static inline Q unpack_arg(void* arg, type_tag<Q>) {
            if constexpr (std::is_pointer_v<Q>) {
                return reinterpret_cast<Q>(arg);
            } else {
                return *reinterpret_cast<Q*>(arg);
            }
        }
        template<typename Q>
        static inline void* pack_result(Q&& thing) {
            if constexpr (std::is_pointer_v<Q>) {
                return reinterpret_cast<void*>(std::forward<Q>(thing));
            } else {
                // For now, we will fail a static_assert here to force people to use non-primitive returns
                // We can't actually have a static assert, sooooo do the following as usual
                // allocate simply returns nullptr.
                auto mem = allocate(sizeof(Q));
                new(mem) Q(std::forward<Q>(thing));
                return mem;
            }
        }
    };

    template<typename TRet, typename T, typename ...TArgs>
    struct invoker_creator<TRet(T::*)(TArgs...)> {
        template<std::size_t ...Ns>
        static void* instance_invoke(TRet(*func)(T*, TArgs...), T* self, void** args, std::index_sequence<Ns...>) {
            if constexpr (std::is_same_v<TRet, void>) {
                func(self,
                    std::forward<TArgs>(arg_helper::unpack_arg(args[Ns], type_tag<TArgs>{}))...
                );
                return nullptr;
            } else {
                return arg_helper::pack_result(
                    func(self,
                        std::forward<TArgs>(arg_helper::unpack_arg(args[Ns], type_tag<TArgs>{}))...
                    )
                );
            }
        }
        [[gnu::noinline]]
        static void* invoke(Il2CppMethodPointer ptr, [[maybe_unused]] const MethodInfo* m, void* obj, void** args) {
            // We also don't need to use anything from m so it is ignored.
            // Perhaps far in the future we will check attributes on it
            auto func = reinterpret_cast<TRet(*)(T*, TArgs...)>(ptr);
            T* self = static_cast<T*>(obj);

            auto seq = std::make_index_sequence<sizeof...(TArgs)>();
            
            return instance_invoke(func, self, args, seq);
        }
    };

    template<typename TRet, typename ...TArgs>
    struct invoker_creator<TRet(*)(TArgs...)> {
        template<std::size_t ...Ns>
        static void* static_invoke(TRet(*func)(TArgs...), void** args, std::index_sequence<Ns...>) {
            if constexpr (std::is_same_v<TRet, void>) {
                func(
                    std::forward<TArgs>(arg_helper::unpack_arg(args[Ns], type_tag<TArgs>{}))...
                );
                return nullptr;
            } else {
                return arg_helper::pack_result(
                    func(
                        std::forward<TArgs>(arg_helper::unpack_arg(args[Ns], type_tag<TArgs>{}))...
                    )
                );
            }
        }
        [[gnu::noinline]]
        static void* invoke(Il2CppMethodPointer ptr, [[maybe_unused]] const MethodInfo* m, [[maybe_unused]] void* obj, void** args) {
            // We also don't need to use anything from m so it is ignored.
            // Perhaps far in the future we will check attributes on it
            auto func = reinterpret_cast<TRet(*)(TArgs...)>(ptr);

            auto seq = std::make_index_sequence<sizeof...(TArgs)>();

            return static_invoke(func, args, seq);
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
        [[gnu::noinline]]
        static inline TRet wrap(T* self, TArgs&& ...args) {
            return (self->*member)(std::forward<TArgs>(args)...);
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

    // Declare a field with type, name.
    // Fields declared like this must also be registered via REGISTER_FIELD within the REGISTER_TYPE function.
    #define DECLARE_INSTANCE_FIELD(type, name) \
    public: \
    type name; \
    private: \
    struct field_wrapper_##name { \
        static inline field_info get() { \
            int32_t offset = _get_field_offset(); \
            _increment_field_offset(sizeof(type)); \
            ::il2cpp_functions::Init(); \
            auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
            uint16_t attrs = FIELD_ATTRIBUTE_PUBLIC; \
            return field_info{#name, t, offset, attrs}; \
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
        static inline field_info get() { \
            ::il2cpp_functions::Init(); \
            auto* t = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type>::get()); \
            uint16_t attrs = FIELD_ATTRIBUTE_PUBLIC | FIELD_ATTRIBUTE_STATIC; \
            return field_info{#name, t, offset, attrs}; \
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
        static inline method_info get() { \
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
            return method_info{#name, ptr, invoker, ret, params, flags}; \
        } \
    }

    #define DECLARE_CLASS(namespaze, name, baseNamespaze, baseName, impl) \
    namespace namespaze { \
        class name; \
    } \
    template<> \
    struct ::custom_types::name_registry<namespaze::name> { \
        static inline type_info get() { \
            return type_info{Il2CppTypeEnum::IL2CPP_TYPE_CLASS, #namespaze, #name, ::il2cpp_utils::GetClassFromName(baseNamespaze, baseName)}; \
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
    static inline void _register(std::vector<::field_info>& fields, std::vector<::field_info>& staticFields, std::vector<::method_info>& methods) { \
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
}
