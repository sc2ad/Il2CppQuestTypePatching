#pragma once
#include "types.hpp"
#include <unordered_map>
#include "logging.hpp"

namespace custom_types {
    /// @class Public API for registering types
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
        /// @brief Registers the provided type within the il2cpp domain.
        /// @tparam T Type to register within il2cpp
        /// @return An Il2CppClass* for further modification
        template<typename T>
        static Il2CppClass* RegisterType() {
            il2cpp_functions::Init();
            EnsureHooks();
            if constexpr (!::custom_types::has_get<::custom_types::name_registry<T>>) {
                static_assert(false_t<T>, "Must have a DECLARE_ to start the type!");
            }
            else {
                // Create our type
                auto type = ::custom_types::name_registry<T>::get();
                if constexpr (::custom_types::has_func_register<T, void(std::vector<::custom_types::field_info*>&, std::vector<::custom_types::field_info*>&, std::vector<::custom_types::method_info*>&)>::value) {
                    ClassWrapper& classWrapper = classes.emplace_back(type);
                    // Iterate over all methods, all fields
                    T::_register(classWrapper.fields, classWrapper.staticFields, classWrapper.methods);
                    classWrapper.populateFields();
                    classWrapper.populateMethods();
                    // Return for extra modification
                    _logger().debug("Registered type: %s::%s", type->namespaze.c_str(), type->name.c_str());
                    // Set the klass static inline field on the type
                    T::klass = classWrapper.klass;
                    return classWrapper.klass;
                } else {
                    static_assert(::custom_types::has_func_register<T, void(std::vector<::custom_types::field_info*>&, std::vector<::custom_types::field_info*>&, std::vector<::custom_types::method_info*>&)>::value, "Must have a REGISTER_FUNCTION within the type!");
                }
            }
            return nullptr;
        }
    };
}