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

        /// @brief Register a single type and return a ClassWrapper* to it.
        /// @tparam T The type to register. Must be a valid custom type from macros.hpp
        /// @return A ClassWrapper* for further modification/access.
        #ifndef CUSTOM_TYPES_USE_CONCEPTS
        template<typename T>
        #else
        template<::custom_types::has_func_register T>
        requires (::custom_types::has_get<::custom_types::name_registry<T>>)
        #endif
        static inline ClassWrapper* RegisterSingle() {
            il2cpp_functions::Init();
            EnsureHooks();
            if constexpr (!::custom_types::has_get<::custom_types::name_registry<T>>) {
                static_assert(false_t<T>, "Must have a DECLARE_ to start the type!");
            }
            else {
                // Create our type
                auto type = ::custom_types::name_registry<T>::get();
                #ifndef CUSTOM_TYPES_USE_CONCEPTS
                if constexpr (::custom_types::has_func_register<T, void(std::vector<::custom_types::field_info*>&, std::vector<::custom_types::field_info*>&, std::vector<::custom_types::method_info*>&)>::value) {
                #endif    
                    ClassWrapper* classWrapper = new ClassWrapper(type);
                    // We need to determine the vtable size.
                    classWrapper->createClass(sizeof(T));
                    T::__klass = classWrapper->get();
                    // Return for extra modification
                    static auto logger = _logger().WithContext("RegisterType");
                    logger.debug("Registered type: %s::%s at idx: %u", type->namespaze.c_str(), type->name.c_str(), classes.size());
                    // Set the klass static inline field on the type
                    classes.push_back(classWrapper);
                    return classWrapper;
                #ifndef CUSTOM_TYPES_USE_CONCEPTS
                } else {
                    static_assert(::custom_types::has_func_register<T, void(std::vector<::custom_types::field_info*>&, std::vector<::custom_types::field_info*>&, std::vector<::custom_types::method_info*>&)>::value, "Must have a REGISTER_FUNCTION within the type!");
                }
                #endif
            }
            return nullptr;
        }

        /// @brief Resolves the fields and methods of a registered type.
        /// Should only be done after all types provided are registered.
        /// @tparam T The type to resolve.
        #ifndef CUSTOM_TYPES_USE_CONCEPTS
        template<typename T>
        #else
        template<::custom_types::has_func_register T>
        requires (::custom_types::has_get<::custom_types::name_registry<T>>)
        #endif
        static inline void ResolveSingle(ClassWrapper* classWrapper) {
            if constexpr (!::custom_types::has_get<::custom_types::name_registry<T>>) {
                static_assert(false_t<T>, "Must have a DECLARE_ to start the type!");
            } else {
                logger.debug("Resolving type: %s::%s at idx: %u", classWrapper->info->namespaze.c_str(), classWrapper->info->name.c_str(), classWrapper->typeIdx);
                T::_register(classWrapper->fields, classWrapper->staticFields, classWrapper->methods);
                classWrapper->populateFields();
                classWrapper->populateMethods();
                logger.debug("Resolved type!");
            }
        }

        /// @brief Resolves many types given a vector of ClassWrapper* and an original size.
        /// @tparam oSize The original size of the vector.
        /// @tparam TSingle A single type to resolve.
        /// @tparam TArgs The remaining types to resolve.
        /// @param data The vector of ClassWrapper* instances to use for resolution.
        template<std::size_t oSize, typename TSingle, typename... TArgs>
        static void ResolveMany(std::vector<ClassWrapper*>& data) {
            if constexpr (sizeof...(TArgs) >= 1) {
                ResolveSingle<TSingle>(data[oSize - sizeof...(TArgs) - 1]);
                // If we have any types remaining, call ourselves
                ResolveMany<oSize, TArgs...>(data);
            } else {
                // We have no types remaining
                ResolveSingle<TSingle>(data[oSize - 1]);
            }
        }
        public:
        // All of the ClassWrapper*s that are accessible
        static std::vector<ClassWrapper*> classes;
        /// @brief Registers the provided types within the il2cpp domain, 
        /// resolving fields and methods after registering all provided types.
        /// @tparam TArgs Types to register within il2cpp.
        /// @return A vector for further type modification/access.
        #ifndef CUSTOM_TYPES_USE_CONCEPTS
        template<typename... TArgs>
        #else
        template<::custom_types::has_func_register... TArgs>
        requires (... && ::custom_types::has_get<::custom_types::name_registry<TArgs>>)
        #endif
        static const std::vector<ClassWrapper*> RegisterTypes() {
            if constexpr (sizeof...(TArgs) == 0) {
                // Allow for a registration of 0 types to happen
                return {};
            } else {
                // Resolve each type, place it into a collection.
                std::vector<ClassWrapper*> createdTypes({RegisterSingle<TArgs>()...});
                // Now that everything is registered, we want to resolve all of them
                ResolveMany<sizeof...(TArgs), TArgs...>(createdTypes);
                return createdTypes;
            }
        }

        /// @brief Registers the provided type within the il2cpp domain.
        /// Please use RegisterTypes if you register more than one type.
        /// @tparam T The type to register within il2cpp.
        /// @return A ClassWrapper* for future reference/modification.
        #ifndef CUSTOM_TYPES_USE_CONCEPTS
        template<typename T>
        #else
        template<::custom_types::has_func_register T>
        requires (::custom_types::has_get<::custom_types::name_registry<T>>)
        #endif
        static ClassWrapper* RegisterType() {
            auto* wrapper = RegisterSingle<T>();
            ResolveSingle<T>(wrapper);
            return wrapper;
        }

        /// @brief Unregisters all custom types, deleting all associated data.
        /// You should not (realistically) have a need for this unless you find custom types memory overhead to be significant.
        static void UnregisterAll() {
            ClassWrapper::typeIdx = kTypeDefinitionIndexInvalid;
            for (auto* wrapper : classes) {
                delete wrapper;
            }
            classes.clear();
            // We also need to correctly delete any created images or assemblies.
            // These need to deleted in a very delicate way so as not to have any dangling references.
        }
    };
}