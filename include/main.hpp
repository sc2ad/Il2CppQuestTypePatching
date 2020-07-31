#pragma once
#include "types.hpp"
#include <unordered_map>

namespace custom_types {
    #if __has_include(<concepts>)
    #include <concepts>
    template<typename T>
    constexpr bool has_register = requires(const T& t) {
        t._register();
    };
    #elif __has_include(<experimental/type_traits>)
    #include <experimental/type_traits>
    template<typename T>
    using type = decltype(T::_register());

    template<typename T>
    constexpr bool has_register = std::experimental::is_detected_v<type, T>;
    #else
    #error No libraries for the implementation of "has_" anything available!
    #endif

    class Register;

    class ClassWrapper {
        friend Register;
        private:
        static TypeDefinitionIndex typeIdx;

        Il2CppClass* klass;
        std::vector<field_info> fields;
        std::vector<field_info> staticFields;
        std::vector<method_info> methods;

        ClassWrapper(type_info type);
        ~ClassWrapper();
        void setupTypeHierarchy(Il2CppClass* base);
        void populateMethods();
        void populateFields();
        Il2CppType* createType(Il2CppTypeEnum typeE);
        public:
        constexpr const Il2CppClass* get() const;
    };

    // Public API for registering types
    class Register {
        friend ClassWrapper;
        private:
        static std::unordered_map<std::string, Il2CppAssembly*> assembs;
        static std::unordered_map<std::string, Il2CppImage*> images;
        static bool installed;

        static Il2CppAssembly* createAssembly(std::string_view name, Il2CppImage* img);
        static Il2CppImage* createImage(std::string_view name);

        static void EnsureHooks();
        public:
        // All of the ClassWrappers that are accessible
        static std::vector<ClassWrapper> classes;
        // Registers the type within the il2cpp domain
        template<typename T>
        static Il2CppClass* RegisterType() {
            EnsureHooks();
            type_info type;
            if constexpr (::custom_types::has_get<::custom_types::name_registry<T>>) {
                // Create our type
                type = ::custom_types::name_registry<T>::get();
            }
            else {
                static_assert(false_t<T>, "Must have a DECLARE_ to start the type!");
            }
            if constexpr (::custom_types::has_register<T>) {
                ClassWrapper classWrapper(type);
                // Iterate over all methods, all fields
                T::_register(classWrapper.fields, classWrapper.staticFields, classWrapper.methods);
                classWrapper.populateFields();
                classWrapper.populateMethods();
                // Add to vector
                classes.push_back(classWrapper);
                // Return for extra modification
                logger().debug("Registered type: %s::%s", type.namespaze.c_str(), type.name.c_str());
                return classWrapper.klass;
            } else {
                static_assert(false_t<T>, "Must have a REGISTER_FUNCTION within the type!");
            }
            return nullptr;
        }
    };
}