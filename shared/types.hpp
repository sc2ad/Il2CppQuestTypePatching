#pragma once
#include <dlfcn.h>
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include <utility>
#include <string_view>
#include <memory>
#include <vector>
#include <initializer_list>
#include "logging.hpp"

struct Il2CppType;
struct ParameterInfo;
struct Il2CppClass;
struct FieldInfo;
struct MethodInfo;

namespace custom_types {
    class Register;
    class ClassWrapper;

    struct virtual_invoke_data {
        friend Register;
        friend ClassWrapper;
        private:
        VirtualInvokeData data;
        public:
        virtual_invoke_data(virtual_invoke_data&&) = default;
        ~virtual_invoke_data() = delete;
    };

    /// @struct A wrapper around information necessary to create an Il2CppClass*
    struct type_info {
        friend Register;
        friend ClassWrapper;
        private:
        Il2CppTypeEnum typeEnum;
        Il2CppClass* base;
        std::string namespaze;
        std::string name;
        std::vector<Il2CppClass*> interfaces;
        bool abst = false;
        public:
        constexpr const Il2CppTypeEnum getEnum() const {
            return typeEnum;
        }
        constexpr const Il2CppClass* getBase() const {
            return base;
        }
        std::string getNamespace() const {
            return namespaze;
        }
        std::string getName() const {
            return name;
        }
        constexpr bool isAbstract() const {
            return abst;
        }
        std::vector<Il2CppClass*> getInterfaces() const {
            return interfaces;
        }
        /// @brief Copy constructor.
        type_info(type_info&&) = default;
        /// @brief Construct a type_info from some information.
        /// @param typeE Il2CppTypeEnum to use for creating the type.
        /// @param ns Namespace of the type to create.
        /// @param n Name of the type to create.
        /// @param b Base Il2CppClass* to use as parent.
        type_info(Il2CppTypeEnum typeE, std::string_view ns, std::string_view n, Il2CppClass* b, std::initializer_list<Il2CppClass*> interfaces, bool abst = false);
        ~type_info() = delete;
    };

    /// @struct A wrapper around information necessary to create a MethodInfo*
    struct method_info {
        friend Register;
        friend ClassWrapper;
        private:
        std::vector<ParameterInfo> params;
        const MethodInfo* virtual_data;
        MethodInfo* info;
        constexpr void setClass(Il2CppClass* klass) const {
            info->klass = klass;
        }
        /// @brief Fixes any references to ourselves in either our return type or parameters.
        void fixSelf(Il2CppType* type);
        public:
        /// @brief Copy constructor.
        method_info(method_info&&) = default;
        /// @brief Construct a method_info from some information.
        /// @param name The name of the method to create.
        /// @param func The method pointer to use when creating the method.
        /// @param invoker The invoker method to use when creating the method.
        /// @param returnType The return type of the method to create.
        /// @param parameters The parameters of the method to create.
        /// @param flags The flags to use when creating the method.
        /// @param virtualInfo The virtual MethodInfo* to set in the vtable (nullptr if this is not a virtual method)
        method_info(std::string_view name, void* func, InvokerMethod invoker, const Il2CppType* returnType, std::vector<ParameterInfo>& parameters, uint16_t flags, const MethodInfo* virtualInfo);
        ~method_info() = default;
        /// @brief Get the MethodInfo* wrapped by this type.
        /// @return The MethodInfo*
        constexpr const MethodInfo* get() const {
            return info;
        }
    };

    /// @struct A wrapper around information necessary to create a FieldInfo
    struct field_info {
        friend Register;
        friend ClassWrapper;
        private:
        FieldInfo info;
        public:
        /// @brief Copy constructor
        field_info(field_info&&) = default;
        /// @brief Construct a field_info from some information.
        /// @param name Name of the field to create.
        /// @param type Il2CppType* of the field to create.
        /// @param fieldAttrs The attributes for the field to create.
        field_info(std::string_view name, const Il2CppType* type, uint16_t fieldAttrs);
        ~field_info() = default;
        /// @brief Get the FieldInfo wrapped by this type.
        /// @return The FieldInfo
        constexpr const FieldInfo get() const {
            return info;
        }
        /// @brief Sets the offset of the field created.
        /// @param offset The offset of the field to create (0, -1 for static, thread_static fields).
        constexpr void setOffset(int32_t offset) {
            info.offset = offset;
        }
    };

    /// @class A wrapper around a macro-defined Il2CppClass*
    class ClassWrapper {
        friend Register;
        private:
        static TypeDefinitionIndex typeIdx;

        Il2CppClass* klass;
        type_info* info;
        std::vector<field_info*> fields;
        std::vector<field_info*> staticFields;
        std::vector<method_info*> methods;

        /// @brief Checks all valid virtual methods on the given info for a matching MethodInfo* with the given klass namespace, name, and methodName
        /// with an optional parameter count match.
        bool checkVirtualsForMatch(custom_types::method_info* info, std::string_view namespaze, std::string_view name, std::string_view methodName, int paramCount = -1);
        int getVtableSize();
        void getVtable(std::vector<VirtualInvokeData>& vtable, std::vector<Il2CppRuntimeInterfaceOffsetPair>& offsets);
        void setupTypeHierarchy(Il2CppClass* base);
        void populateMethods();
        void populateFields();
        Il2CppType* createType(Il2CppTypeEnum typeE);

        /// @brief Custom deletion function, to manage deletion of all owned resources.
        ~ClassWrapper();
        public:
        /// @brief Get fields
        /// @returns Fields
        const std::vector<field_info*> getFields() {
            return fields;
        }
        /// @brief Get static fields
        /// @returns Static fields
        const std::vector<field_info*> getStaticFields() {
            return staticFields;
        }
        /// @brief Get methods
        /// @returns Methods
        const std::vector<method_info*> getMethods() {
            return methods;
        }
        /// @brief Construct a ClassWrapper from a type_info*. The type_info* is not deleted during this call.
        /// @param type The type_info* to use for the construction of this ClassWrapper.
        ClassWrapper(type_info* type);
        /// @brief Copy constructor.
        ClassWrapper(ClassWrapper&& other) {
            // Ideally we copy everything from our original and null out the old things
            // This will be necessary when we switch back from ClassWrapper*s in our classes vector.
            klass = other.klass;
            info = other.info;
            fields = other.fields;
            staticFields = other.staticFields;
            methods = other.methods;
            other.klass = nullptr;
            other.info = nullptr;
            other.fields.clear();
            other.staticFields.clear();
            other.methods.clear();
        }
        /// @brief Creates the Il2CppClass* used within this wrapper.
        /// Should be done at a point in time after all fields and methods are set for this wrapper.
        void createClass(std::size_t size);
        /// @brief Get the Il2CppClass* wrapped by this ClassWrapper.
        constexpr const Il2CppClass* get() const {
            return klass;
        }
    };

    #if __has_include(<concepts>)
    #include <concepts>
    #include <type_traits>
    template<typename T>
    constexpr bool has_get = requires(const T& t) {
        t.get();
    };

    template<typename T>
    concept has_func_register = requires(std::vector<field_info*>& x, std::vector<method_info*>& y) {
        {T::_register(x, x, y)} -> std::same_as<void>;
    };

    #ifndef CUSTOM_TYPES_NO_CONCEPTS
    #define CUSTOM_TYPES_USE_CONCEPTS
    #endif

    #elif __has_include(<experimental/type_traits>)
    #include <experimental/type_traits>
    template<typename T>
    using get_type = decltype(T::get());

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
    template <class...> constexpr std::false_type false_t{};
    // Several of these concepts originally created by DaNike, modifications made by Sc2ad

    /// @struct A helper structure for getting parameters, return type, and function pointer from an instance method
    template<typename Decl, typename T>
    struct method_info_template_instance {};

    /// @struct A helper structure for getting parameters, return type, and function pointer from a static method
    template<typename Decl, typename T>
    struct method_info_template_static {};

    /// @struct A helper structure for getting the name of the type.
    template<typename T>
    struct name_registry {};

    /// @struct A helper structure for getting the invoker function of a method
    template<typename>
    struct invoker_creator {};

    /// @struct Type mapping struct
    template<typename T> struct type_tag {};

    /// @struct A helper structure for converting parameter types to il2cpp types.
    template<typename Decl, typename... Ps>
    struct parameter_converter;

    // Create a vector of ParameterInfo objects (good ol tail recursion)
    // 1 or more parameters
    template<typename Decl, typename P, typename... Ps>
    struct parameter_converter<Decl, P, Ps...> {
        static inline std::vector<ParameterInfo> get() {
            std::vector<ParameterInfo> params;
            auto& info = params.emplace_back();
            il2cpp_functions::Init();
            _logger().debug("Getting class type of parameter!");
            const Il2CppType* type = ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<P>::get());
            // Ignore name, it will be set when we iterate over all of them (param_1, param_2, etc.)
            // Ignore position, it will also be set on iteration.
            // TODO: Maybe some day we can actually use the parameters names themselves!
            info.parameter_type = type;
            info.token = -1;
            for (const auto& q : parameter_converter<Decl, Ps...>::get()) {
                params.push_back(q);
            }
            return std::move(params);
        }
    };

    // 0 parameters
    template<typename Decl>
    struct parameter_converter<Decl> {
        static inline std::vector<ParameterInfo> get() {
            return std::vector<ParameterInfo>();
        }
    };

    /// @struct Helper structure for unpacking and packing arguments/return types for invoker function creation
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
                // We SHOULD simply be able to grab the class and box our result
                // Once boxed, we should just be able to return without any issue
                // I DO wonder if our invoke functions miss registration with GC...
                auto* klass = il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<Q>::get();
                if (!klass) {
                    _logger().critical("Failed to get non-null Il2CppClass* during invoke of custom function!");
                    return nullptr;
                }
                il2cpp_functions::Init();
                return static_cast<void*>(il2cpp_functions::value_box(klass, static_cast<void*>(&thing)));
            }
        }
    };

    template<typename TRet, typename T, typename ...TArgs>
    struct invoker_creator<TRet(T::*)(TArgs...)> {
        template<std::size_t ...Ns>
        static void* instance_invoke(TRet(*func)(T*, TArgs...), T* self, void** args, std::index_sequence<Ns...>) {
            if constexpr (std::is_same_v<TRet, void>) {
                func(self,
                    arg_helper::unpack_arg(args[Ns], type_tag<TArgs>{})...
                );
                return nullptr;
            } else {
                return arg_helper::pack_result(
                    func(self,
                        arg_helper::unpack_arg(args[Ns], type_tag<TArgs>{})...
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
                    arg_helper::unpack_arg(args[Ns], type_tag<TArgs>{})...
                );
                return nullptr;
            } else {
                return arg_helper::pack_result(
                    func(
                        arg_helper::unpack_arg(args[Ns], type_tag<TArgs>{})...
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

    template<typename Decl, typename TRet, typename T, typename... TArgs>
    struct method_info_template_instance<Decl, TRet(T::*)(TArgs...)> {
        static inline const Il2CppType* get() {
            il2cpp_functions::Init();
            return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TRet>::get());
        }
        static inline const std::vector<ParameterInfo> get_params() {
            std::vector<ParameterInfo> vec = parameter_converter<Decl, TArgs...>::get();
            for (std::size_t i = 0; i < vec.size(); i++) {
                auto str = string_format("param_%u", i);
                char* buff = static_cast<char*>(calloc(str.size() + 1, sizeof(char)));
                str.copy(buff, str.size());
                buff[str.size()] = '\0';
                vec[i].name = buff;
                vec[i].position = i;
            }
            return vec;
        }
        template<TRet(T::* member)(TArgs...)>
        [[gnu::noinline]]
        static inline TRet wrap(T* self, TArgs ...args) {
            return (self->*member)(args...);
        }
    };

    template<typename Decl, typename TRet, typename... TArgs>
    struct method_info_template_static<Decl, TRet(*)(TArgs...)> {
        static inline const Il2CppType* get() {
            il2cpp_functions::Init();
            return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TRet>::get());
        }
        static inline const std::vector<ParameterInfo> get_params() {
            std::vector<ParameterInfo> vec = parameter_converter<Decl, TArgs...>::get();
            for (std::size_t i = 0; i < vec.size(); i++) {
                auto str = string_format("param_%u", i);
                char* buff = static_cast<char*>(calloc(str.size() + 1, sizeof(char)));
                str.copy(buff, str.size());
                buff[str.size()] = '\0';
                vec[i].name = buff;
                vec[i].position = i;
            }
            return vec;
        }
    };
}
