#pragma once

#include "macros.hpp"
#include <functional>
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace custom_types {

int get_delegate_count();

inline void setup_for_delegate(MethodInfo* info) {
    // The method in question actually isn't quite fit for being a proper delegate
    // So, here we will set it just to make sure it does what we want.
    info->is_marshaled_from_native = true;
    // TODO: Support virtual invokes some time in the distant, distant future.
    // m->slot = kInvalidIl2CppMethodSlot;
    // m->invoker_method = parent_invoke->invoker_method;
}

/// @brief The wrapper for an invokable delegate without an existing context.
/// DO NOT ATTEMPT TO CREATE THIS YOURSELF!
/// @tparam R The return type of the function being called.
/// @tparam TArgs The argument types of the function being called.
template<class RI, class... TArgsI>
struct DelegateWrapperStatic : Il2CppObject {
    // CT Boilerplate
    using ___TargetType = DelegateWrapperStatic<RI, TArgsI...>;
    constexpr static auto ___Base__Size = sizeof(Il2CppObject);
    friend ::custom_types::Register;
public:
    struct ___TypeRegistration : ::custom_types::TypeRegistration {
        ___TypeRegistration() {
            ::custom_types::Register::AddType(this);
            ::custom_types::_logger().debug("Adding delegate type: %s", name());
            instance = this;
        }
        std::vector<::custom_types::FieldRegistrator*> const getFields() const override {
            return {};
        }
        std::vector<::custom_types::StaticFieldRegistrator*> const getStaticFields() const override {
            return {};
        }
        std::vector<::custom_types::MethodRegistrator*> const getMethods() const override {
            return {&___Invoke_MethodRegistrator, &___dtor_MethodRegistrator};
        }
        static inline char* st_fields;
        char*& static_fields() override {
            return st_fields;
        }
        size_t static_fields_size() const override {
            return 0;
        }
        const char* name() const override {
            if constexpr (!std::is_same_v<void, RI>) {
                static std::string nm{string_format("DelegateWrapper(%zu ret, %zu args, %zu sz) (%d)", sizeof(RI), sizeof...(TArgsI), sizeof(___TargetType), custom_types::get_delegate_count())};
                return nm.c_str();
            } else {
                static std::string nm{string_format("DelegateWrapper(void ret, %zu args, %zu sz) (%d)", sizeof...(TArgsI), sizeof(___TargetType), custom_types::get_delegate_count())};
                return nm.c_str();
            }
        }
        constexpr const char* namespaze() const override {
            return "CustomTypes";
        }
        constexpr const char* dllName() const override {
            return "CustomTypes.dll";
        }
        Il2CppClass* baseType() const override {
            ::il2cpp_functions::Init();
            return ::il2cpp_functions::defaults->object_class;
        }
        std::vector<Il2CppClass*> const interfaces() const override {
            return {};
        }
        constexpr Il2CppTypeEnum typeEnum() const override {
            return IL2CPP_TYPE_CLASS;
        }
        constexpr uint32_t typeFlags() const override {
            return 0;
        }
        static inline Il2CppClass* klass_ptr;
        Il2CppClass*& klass() const override {
            return klass_ptr;
        }
        constexpr size_t size() const override {
            return sizeof(___TargetType);
        }
        TypeRegistration* customBase() const override {
            return nullptr;
        }
        bool initialized() const override {
            return init;
        }
        void setInitialized() const override {
            init = true;
        }
        static inline bool init = false;
        static inline TypeRegistration* instance;
        static TypeRegistration* get() {
            return instance;
        }
    };
public:
    static inline ___TypeRegistration __registration_instance_DelegateWrapperStatic;
    // Disallow creation
    DelegateWrapperStatic() = delete;
    // The (potentially capturing) function to invoke
    std::function<RI(TArgsI...)> wrappedFunc;

    // The invoke method that wraps the delegate call
    static RI Invoke(DelegateWrapperStatic<RI, TArgsI...>* instance, TArgsI... args);
private:
    struct ___MethodRegistrator_Invoke : ::custom_types::MethodRegistrator {
        constexpr const char* name() const override {
            return "Invoke";
        }
        constexpr const char* csharpName() const override {
            return "Invoke";
        }
        int flags() const override {
            return METHOD_ATTRIBUTE_STATIC;
        }
        const MethodInfo* virtualMethod() const override {
            return nullptr;
        }
        const Il2CppType* returnType() const override {
            return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<RI>::get());
        }
        std::vector<ParameterInfo> params() const override {
            int32_t counter = 1;
            il2cpp_functions::Init();
            return {ParameterInfo{"inst", 0, static_cast<uint32_t>(-1), ::il2cpp_functions::class_get_type(___TypeRegistration::klass_ptr)},
                    (ParameterInfo{"param", counter++, static_cast<uint32_t>(-1), ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TArgsI>::get())})...
            };
        }
        uint8_t params_size() const override {
            return sizeof...(TArgsI) + 1;
        }
        Il2CppMethodPointer methodPointer() const override {
            return reinterpret_cast<Il2CppMethodPointer>(&___TargetType::Invoke);
        }
        InvokerMethod invoker() const override {
            return &::custom_types::invoker_creator<decltype(&___TargetType::Invoke)>::invoke;
        }
    };
public:
    static inline ___MethodRegistrator_Invoke ___Invoke_MethodRegistrator;

public:
    void dtor();
private:
    template<typename T>
    struct ___MethodRegistrator_dtor;
    template<typename R, typename T, typename... TArgs>
    struct ___MethodRegistrator_dtor<R (T::*)(TArgs...)> : ::custom_types::MethodRegistrator {
        constexpr const char* name() const override {
            return "dtor";
        }
        constexpr const char* csharpName() const override {
            return "dtor";
        }
        int flags() const override {
            return METHOD_ATTRIBUTE_VIRTUAL | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
        }
        const MethodInfo* virtualMethod() const override {
            return ::il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0);
        }
        const Il2CppType* returnType() const override {
            ::il2cpp_functions::Init();
            return &il2cpp_functions::defaults->void_class->byval_arg;
        }
        std::vector<ParameterInfo> params() const override {
            return {};
        }
        uint8_t params_size() const override {
            return 0;
        }
        Il2CppMethodPointer methodPointer() const override {
            static auto mptr = &DelegateWrapperStatic<RI, TArgsI...>::dtor;
            return reinterpret_cast<Il2CppMethodPointer>(+[](DelegateWrapperStatic<RI, TArgsI...>* inst) {
                (inst->*mptr)();
            });
        }
        InvokerMethod invoker() const override {
            return &::custom_types::invoker_creator<decltype(&___TargetType::dtor)>::invoke;
        }
    };
public:
    static inline ___MethodRegistrator_dtor<decltype(&___TargetType::dtor)> ___dtor_MethodRegistrator;
};

} // Close the custom_types namespace for implementing specializations

template<class R, class... TArgs>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<custom_types::DelegateWrapperStatic<R, TArgs...>*> {
    static inline Il2CppClass* get() {
        return custom_types::DelegateWrapperStatic<R, TArgs...>::___TypeRegistration::klass_ptr;
    }
};
template<class R, class... TArgs>
struct ::il2cpp_utils::il2cpp_type_check::need_box<custom_types::DelegateWrapperStatic<R, TArgs...>> {
    constexpr static bool value = false;
};

namespace custom_types {
// Reopen it here

/// @brief The invoker function for a delegate with a wrapped type.
/// @tparam R The return type of the function.
/// @tparam TArgs The argument types of the function.
/// @param instance The wrapped instance of this context function.
/// @param args The arguments to pass to this function.
/// @return The return from the wrapped function.
template<class RI, class... TArgsI>
RI __attribute__((noinline)) DelegateWrapperStatic<RI, TArgsI...>::Invoke(DelegateWrapperStatic<RI, TArgsI...>* instance, TArgsI... args) {
    IL2CPP_CATCH_HANDLER(
        if (!instance || !instance->wrappedFunc) {
            custom_types::_logger().critical("Attempting to invoke delegate on a null or destroyed instance: %p, class: %p (%s)", instance, ___TypeRegistration::klass_ptr, ___TypeRegistration::get()->name());
        }
        if constexpr (std::is_same_v<RI, void>) {
            instance->wrappedFunc(args...);
        } else {
            return instance->wrappedFunc(args...);
        }
    )
}
template<class RI, class... TArgsI>
void DelegateWrapperStatic<RI, TArgsI...>::dtor() {
    custom_types::_logger().debug("Destroying delegate: %p, class: %p (%s)", this, ___TypeRegistration::klass_ptr, ___TypeRegistration::get()->name());
    this->~DelegateWrapperStatic();
}

/// @brief The wrapper for an invokable delegate without an existing context.
/// DO NOT ATTEMPT TO CREATE THIS YOURSELF!
/// @tparam R The return type of the function being called.
/// @tparam TArgs The argument types of the function being called.
template<class RI, class TI, class... TArgsI>
struct DelegateWrapperInstance : Il2CppObject {
    // CT Boilerplate
    using ___TargetType = DelegateWrapperInstance<RI, TI, TArgsI...>;
    constexpr static auto ___Base__Size = sizeof(Il2CppObject);
    friend ::custom_types::Register;
public:
    struct ___TypeRegistration : ::custom_types::TypeRegistration {
        ___TypeRegistration() {
            ::custom_types::Register::AddType(this);
            ::custom_types::_logger().debug("Adding instance delegate type: %s", name());
            instance = this;
        }
        std::vector<::custom_types::FieldRegistrator*> const getFields() const override {
            return {&___obj_FieldRegistrator};
        }
        std::vector<::custom_types::StaticFieldRegistrator*> const getStaticFields() const override {
            return {};
        }
        std::vector<::custom_types::MethodRegistrator*> const getMethods() const override {
            return {&___Invoke_MethodRegistrator, &___dtor_MethodRegistrator};
        }
        static inline char* st_fields;
        char*& static_fields() override {
            return st_fields;
        }
        size_t static_fields_size() const override {
            return 0;
        }
        const char* name() const override {
            if constexpr (!std::is_same_v<void, RI>) {
                static std::string nm{string_format("DelegateWrapper(%zu ret, %zu args, %zu sz) (%d)", sizeof(RI), sizeof...(TArgsI), sizeof(___TargetType), custom_types::get_delegate_count())};
                return nm.c_str();
            } else {
                static std::string nm{string_format("DelegateWrapper(void ret, %zu args, %zu sz) (%d)", sizeof...(TArgsI), sizeof(___TargetType), custom_types::get_delegate_count())};
                return nm.c_str();
            }
        }
        constexpr const char* namespaze() const override {
            return "CustomTypes";
        }
        constexpr const char* dllName() const override {
            return "CustomTypes.dll";
        }
        Il2CppClass* baseType() const override {
            ::il2cpp_functions::Init();
            return ::il2cpp_functions::defaults->object_class;
        }
        std::vector<Il2CppClass*> const interfaces() const override {
            return {};
        }
        constexpr Il2CppTypeEnum typeEnum() const override {
            return IL2CPP_TYPE_CLASS;
        }
        constexpr uint32_t typeFlags() const override {
            return 0;
        }
        static inline Il2CppClass* klass_ptr;
        Il2CppClass*& klass() const override {
            return klass_ptr;
        }
        constexpr std::size_t size() const override {
            return sizeof(___TargetType);
        }
        constexpr TypeRegistration* customBase() const override {
            return nullptr;
        }
        bool initialized() const override {
            return init;
        }
        void setInitialized() const override {
            init = true;
        }
        static inline bool init = false;
        static inline TypeRegistration* instance;
        static TypeRegistration* get() {
            return instance;
        }
    };
public:
    static inline ___TypeRegistration __registration_instance_DelegateWrapperInstance;
    // Disallow creation
    DelegateWrapperInstance() = delete;

    // The invoke method that wraps the delegate call
    static RI Invoke(DelegateWrapperInstance<RI, TI, TArgsI...>* instance, TArgsI... args);
private:
    struct ___MethodRegistrator_Invoke : ::custom_types::MethodRegistrator {
        constexpr const char* name() const override {
            return "Invoke";
        }
        constexpr const char* csharpName() const override {
            return "Invoke";
        }
        int flags() const override {
            return METHOD_ATTRIBUTE_STATIC;
        }
        const MethodInfo* virtualMethod() const override {
            return nullptr;
        }
        const Il2CppType* returnType() const override {
            ::il2cpp_functions::Init();
            return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<RI>::get());
        }
        std::vector<ParameterInfo> params() const override {
            int32_t counter = 1;
            il2cpp_functions::Init();
            return {ParameterInfo{"inst", 0, static_cast<uint32_t>(-1), ::il2cpp_functions::class_get_type(___TypeRegistration::klass_ptr)},
                    (ParameterInfo{"param", counter++, static_cast<uint32_t>(-1), ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TArgsI>::get())})...
            };
        }
        uint8_t params_size() const override {
            return sizeof...(TArgsI) + 1;
        }
        Il2CppMethodPointer methodPointer() const override {
            return reinterpret_cast<Il2CppMethodPointer>(&___TargetType::Invoke);
        }
        InvokerMethod invoker() const override {
            return &::custom_types::invoker_creator<decltype(&___TargetType::Invoke)>::invoke;
        }
    };
public:
    static inline ___MethodRegistrator_Invoke ___Invoke_MethodRegistrator;

private:
    struct ___FieldRegistrator_obj : ::custom_types::FieldRegistrator {
        constexpr const char* name() const override {
            return "obj";
        }
        const Il2CppType* type() const override {
            ::il2cpp_functions::Init();
            return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<TI>::get());
        }
        constexpr uint16_t fieldAttributes() const override {
            return FIELD_ATTRIBUTE_PUBLIC;
        }
        constexpr size_t size() const override {
            return sizeof(TI);
        }
        int32_t offset() const override {
            return offsetof(___TargetType, obj);
        }
    };
    static inline ___FieldRegistrator_obj ___obj_FieldRegistrator;
public:
    // The instance object
    TI obj;
    // The (potentially capturing) function to invoke
    std::function<RI(TI, TArgsI...)> wrappedFunc;

    void dtor();
private:
    template<typename T>
    struct ___MethodRegistrator_dtor;
    template<typename R, typename T, typename... TArgs>
    struct ___MethodRegistrator_dtor<R (T::*)(TArgs...)> : ::custom_types::MethodRegistrator {
        constexpr const char* name() const override {
            return "dtor";
        }
        constexpr const char* csharpName() const override {
            return "dtor";
        }
        int flags() const override {
            return METHOD_ATTRIBUTE_VIRTUAL | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG;
        }
        const MethodInfo* virtualMethod() const override {
            return ::il2cpp_utils::FindMethodUnsafe("System", "Object", "Finalize", 0);
        }
        const Il2CppType* returnType() const override {
            ::il2cpp_functions::Init();
            return &il2cpp_functions::defaults->void_class->byval_arg;
        }
        std::vector<ParameterInfo> params() const override {
            return {};
        }
        uint8_t params_size() const override {
            return 0;
        }
        Il2CppMethodPointer methodPointer() const override {
            static auto mptr = &DelegateWrapperInstance<RI, TArgsI...>::dtor;
            return reinterpret_cast<Il2CppMethodPointer>(+[](DelegateWrapperInstance<RI, TArgsI...>* inst) {
                (inst->*mptr)();
            });
        }
        InvokerMethod invoker() const override {
            return &::custom_types::invoker_creator<decltype(&___TargetType::dtor)>::invoke;
        }
    };
public:
    static inline ___MethodRegistrator_dtor<decltype(&___TargetType::dtor)> ___dtor_MethodRegistrator;
};

} // Close the custom_types namespace for implementing specializations

template<class R, class T, class... TArgs>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<custom_types::DelegateWrapperInstance<R, T, TArgs...>*> {
    static inline Il2CppClass* get() {
        return custom_types::DelegateWrapperInstance<R, T, TArgs...>::___TypeRegistration::klass_ptr;
    }
};
template<class R, class T, class... TArgs>
struct ::il2cpp_utils::il2cpp_type_check::need_box<custom_types::DelegateWrapperInstance<R, T, TArgs...>> {
    constexpr static bool value = false;
};

namespace custom_types {
// Reopen it here

/// @brief The invoker function for a delegate with a wrapped type.
/// @tparam R The return type of the function.
/// @tparam TArgs The argument types of the function.
/// @param instance The wrapped instance of this context function.
/// @param args The arguments to pass to this function.
/// @return The return from the wrapped function.
template<class RI, class TI, class... TArgsI>
RI __attribute__((noinline)) DelegateWrapperInstance<RI, TI, TArgsI...>::Invoke(DelegateWrapperInstance<RI, TI, TArgsI...>* instance, TArgsI... args) {
    IL2CPP_CATCH_HANDLER(
        if (!instance || !instance->wrappedFunc) {
            custom_types::_logger().critical("Attempting to invoke instance delegate that is null or has been destroyed! %p, class: %p (%s)", instance, ___TypeRegistration::klass_ptr, ___TypeRegistration::get()->name());
        }
        if constexpr (std::is_same_v<RI, void>) {
            instance->wrappedFunc(instance->obj, args...);
        } else {
            return instance->wrappedFunc(instance->obj, args...);
        }
    )
}
template<class RI, class TI, class... TArgsI>
void DelegateWrapperInstance<RI, TI, TArgsI...>::dtor() {
    custom_types::_logger().debug("Destroying delegate: %p, class: %p (%s)", this, ___TypeRegistration::klass_ptr, ___TypeRegistration::get()->name());
    this->~DelegateWrapperInstance();
}

void log_delegate(Il2CppDelegate* d);

template<class T = MulticastDelegate*, class R, class... TArgs>
T MakeDelegate(const Il2CppClass* delegateClass, DelegateWrapperStatic<R, TArgs...>* inst) {
    // TODO: We could still make this a native method info, but for now it's much much easier to just wrap it
    // This will make delegates a little bit slower than just a pure native call, since it'll have to box the args
    // and then unbox them (as necessary).
    // But it should also make them much safer, hopefully solving all sorts of problems.
    // This gets the first method (which should be our Invoke static method)'s MethodInfo*.
    // It should already have been allocated, so get will cache it for us.
    // We need to ensure static initialization of both the dtor method registrator
    // and the invoke method registrator:
    custom_types::_logger().debug("Delegate dtor registrator: %p", DelegateWrapperStatic<R, TArgs...>::___dtor_MethodRegistrator.get());
    auto* invokeMethod = CRASH_UNLESS(il2cpp_functions::class_get_method_from_name(delegateClass, "Invoke", -1));
    auto* method = DelegateWrapperStatic<R, TArgs...>::___Invoke_MethodRegistrator.get();
    setup_for_delegate(method);
    auto *delegate = CRASH_UNLESS(il2cpp_utils::NewUnsafe<T>(delegateClass, inst, &method));
    custom_types::_logger().debug("Created delegate: %p (%p), for instance: %p with MethodInfo*: %p", delegate, delegateClass, inst, method);
    log_delegate(reinterpret_cast<Il2CppDelegate*>(delegate));
    return delegate;
}

template<class T = MulticastDelegate*, class R, class I, class... TArgs>
T MakeDelegate(const Il2CppClass* delegateClass, DelegateWrapperInstance<R, I, TArgs...>* inst) {
    custom_types::_logger().debug("Delegate instance dtor registrator: %p", DelegateWrapperInstance<R, I, TArgs...>::___dtor_MethodRegistrator.get());
    auto* invokeMethod = CRASH_UNLESS(il2cpp_functions::class_get_method_from_name(delegateClass, "Invoke", -1));
    auto* method = DelegateWrapperInstance<R, I, TArgs...>::___Invoke_MethodRegistrator.get();
    setup_for_delegate(method);
    auto* delegate = CRASH_UNLESS(il2cpp_utils::NewUnsafe<T>(delegateClass, inst, &method));
    custom_types::_logger().debug("Created instance delegate: %p (%p), for instance: %p with MethodInfo*: %p", delegate, delegateClass, inst, method);
    log_delegate(reinterpret_cast<Il2CppDelegate*>(delegate));
    return delegate;
}

/// @brief Makes a delegate wrapping a context function (such as a context lambda).
/// @tparam T The type to return.
/// @tparam I The instance object to provide to this delegate.
/// @tparam R The return type of the delegate.
/// @tparam TArgs The arguments of the delegate.
/// @param delegateClass The Il2CppClass* of the delegate to create.
/// @param instance The (move constructible) instance reference to provide to the delegate. This instance is moved and will no longer be valid.
/// @param f The function to invoke with the delegate.
/// @return The created delegate.
template<typename T = MulticastDelegate*, class R, class... TArgs>
T MakeDelegate(const Il2CppClass* delegateClass, std::function<R(TArgs...)> const& f) {
    il2cpp_functions::Init();
    // NOTE: This static field MUST be used in order for it to be instantiated within the generic, thus, it is important to NOT remove this log
    custom_types::_logger().debug("Type registration for delegate being created (forcibly instantiated): %s", DelegateWrapperStatic<R, TArgs...>::__registration_instance_DelegateWrapperStatic.name());
    auto* wrapperInstance = reinterpret_cast<DelegateWrapperStatic<R, TArgs...>*>(il2cpp_functions::object_new(DelegateWrapperStatic<R, TArgs...>::___TypeRegistration::klass_ptr));
    wrapperInstance->wrappedFunc = f;
    return custom_types::MakeDelegate<T>(delegateClass, wrapperInstance);
}

/// @brief Makes a delegate wrapping a context function (such as a context lambda).
/// @tparam T The type to return.
/// @tparam I The instance object to provide to this delegate.
/// @tparam R The return type of the delegate.
/// @tparam TArgs The arguments of the delegate.
/// @param instance The (move constructible) instance reference to provide to the delegate. This instance is moved and will no longer be valid.
/// @param f The function to invoke with the delegate.
/// @return The created delegate.
template<typename T = MulticastDelegate*, class R, class... TArgs>
T MakeDelegate(std::function<R(TArgs...)> const& f) {
    return MakeDelegate<T>(classof(T), f);
}

template<typename T = MulticastDelegate*, class R, class I, class... TArgs>
// TODO: Requires that I has a classof
T MakeDelegate(const Il2CppClass* delegateClass, I obj, std::function<R (I, TArgs...)> const& f) {
    il2cpp_functions::Init();
    custom_types::_logger().debug("Type registration for delegate being created (forcibly instantiated): %s", DelegateWrapperInstance<R, I, TArgs...>::__registration_instance_DelegateWrapperInstance.name());
    auto* wrapperInstance = reinterpret_cast<DelegateWrapperInstance<R, I, TArgs...>*>(il2cpp_functions::object_new(DelegateWrapperInstance<R, I, TArgs...>::___TypeRegistration::klass_ptr));
    wrapperInstance->wrappedFunc = f;
    wrapperInstance->obj = obj;
    return custom_types::MakeDelegate<T>(delegateClass, wrapperInstance);
}

template<typename T = MulticastDelegate*, class R, class I, class... TArgs>
T MakeDelegate(I obj, std::function<R (I, TArgs...)> const& f) {
    return MakeDelegate<T>(classof(T), obj, f);
}
}
