#pragma once

#include "macros.hpp"
#include <functional>
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace custom_types {

int get_delegate_count();

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
            instance = this;
        }
        std::vector<::custom_types::FieldRegistrator*> const getFields() const override {
            return {};
        }
        std::vector<::custom_types::StaticFieldRegistrator*> const getStaticFields() const override {
            return {};
        }
        static inline std::vector<::custom_types::MethodRegistrator*> methods;
        std::vector<::custom_types::MethodRegistrator*> const getMethods() const override {
            return methods;
        }
        static void addMethod(::custom_types::MethodRegistrator* inst) {
            methods.push_back(inst);
        }
        static char* st_fields;
        char*& static_fields() override {
            return st_fields;
        }
        size_t static_fields_size() const override {
            return 0;
        }
        const char* name() const override {
            static std::string nm{string_format("DelegateWrapper(%zu ret, %zu args, %zu sz) (%d)", sizeof(RI), sizeof...(TArgsI), sizeof(___TargetType), custom_types::get_delegate_count())};
            return nm.c_str();
        }
        const char* namespaze() const override {
            return "CustomTypes";
        }
        const char* dllName() const override {
            return "CustomTypes.dll";
        }
        Il2CppClass* baseType() const override {
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
        static Il2CppClass* klass_ptr;
        Il2CppClass*& klass() const override {
            return klass_ptr;
        }
        size_t size() const override {
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
        static bool init;
        static TypeRegistration* instance;
        static TypeRegistration* get() {
            return instance;
        }
    };
public:
    // Disallow creation
    DelegateWrapperStatic() = delete;
    // The invoke method that wraps the delegate call
    DECLARE_STATIC_METHOD(RI, Invoke, DelegateWrapperStatic<RI, TArgsI...>* instance, TArgsI... args);
public:
    // The (potentially capturing) function to invoke
    std::function<RI(TArgsI...)> wrappedFunc;

    void dtor();
    private:
    template<typename T>
    struct ___MethodRegistrator_dtor;
    template<typename R, typename T, typename... TArgs>
    struct ___MethodRegistrator_dtor<R (T::*)(TArgs...)> : ::custom_types::MethodRegistrator {
        ___MethodRegistrator_dtor() {
            ___TargetType::___TypeRegistration::addMethod(this);
        }
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

// DEFINE TYPE
template<class RI, class... TArgsI>
::custom_types::TypeRegistration* DelegateWrapperStatic<RI, TArgsI...>::___TypeRegistration::instance;
template<class RI, class... TArgsI>
static typename DelegateWrapperStatic<RI, TArgsI...>::___TypeRegistration __registration_instance_DelegateWrapperStatic;
template<class RI, class... TArgsI>
char* DelegateWrapperStatic<RI, TArgsI...>::___TypeRegistration::st_fields;
// TODO: This might cause a whole lot of problems because it is in a header?
template<class RI, class... TArgsI>
Il2CppClass* DelegateWrapperStatic<RI, TArgsI...>::___TypeRegistration::klass_ptr;
template<class RI, class... TArgsI>
bool DelegateWrapperStatic<RI, TArgsI...>::___TypeRegistration::init = false;


/// @brief The invoker function for a delegate with a wrapped type.
/// @tparam R The return type of the function.
/// @tparam TArgs The argument types of the function.
/// @param instance The wrapped instance of this context function.
/// @param args The arguments to pass to this function.
/// @return The return from the wrapped function.
template<class RI, class... TArgsI>
RI __attribute__((noinline)) DelegateWrapperStatic<RI, TArgsI...>::Invoke(DelegateWrapperStatic<RI, TArgsI...>* instance, TArgsI... args) {
    IL2CPP_CATCH_HANDLER(
        if constexpr (std::is_same_v<RI, void>) {
            instance->wrappedFunc(args...);
        } else {
            return instance->wrappedFunc(args...);
        }
    )
}
template<class RI, class... TArgsI>
void DelegateWrapperStatic<RI, TArgsI...>::dtor() {
    this->~DelegateWrapperStatic();
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
    auto* method = DelegateWrapperStatic<R, TArgs...>::___TypeRegistration::methods[0]->get();
    auto* delegate = CRASH_UNLESS(il2cpp_utils::NewUnsafe<T>(delegateClass, inst, &method));
    auto* asDelegate = reinterpret_cast<Il2CppDelegate*>(delegate);
    custom_types::_logger().debug("Created delegate: %p (%p), for instance: %p with MethodInfo*: %p", delegate, delegateClass, inst, method);
    log_delegate(asDelegate);
    // TODO: Find another assertion that works as well
    // if ((void*)asDelegate->method_ptr != (void*)&DelegateWrapperStatic<R, TArgs...>::Invoke) {
    //     custom_types::_logger().error("Created Delegate's method_ptr (%p) is incorrect (should be %p)!", (void*)asDelegate->method_ptr, &DelegateWrapperStatic<R, TArgs...>::Invoke);
    //     throw il2cpp_utils::exceptions::StackTraceException("Failed to MakeDelegate! Mismatched method pointers!");
    // }
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
}
