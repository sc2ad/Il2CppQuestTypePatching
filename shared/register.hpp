#pragma once
#include "types.hpp"
#include <unordered_map>
#include "logging.hpp"
#include <mutex>
#include <shared_mutex>

namespace custom_types {
    /// @class Public API for registering types
    class Register {
        friend TypeRegistration;
        private:
        static std::unordered_map<std::string, Il2CppAssembly*> assembs;
        static std::unordered_map<std::string, Il2CppImage*> images;
        static std::shared_mutex assemblyMtx;
        static std::shared_mutex imageMtx;
        static std::mutex registrationMtx;
        static bool installed;

        static std::vector<TypeRegistration*> toRegister;
        static std::vector<TypeRegistration*> registeredTypes;
        static TypeDefinitionIndex typeIdx;

        static Il2CppAssembly* createAssembly(std::string_view name, Il2CppImage* img);
        static Il2CppImage* createImage(std::string_view name);

        static void EnsureHooks();

        public:
        static std::vector<Il2CppClass*> classes;
        static std::vector<TypeRegistration*> const& getTypes() {
            return registeredTypes;
        }
        /// @brief Automatically registers all pending types.
        /// To add a type to be registered, see: AddType
        /// To get the list of all registered types, see: getTypes()
        static void AutoRegister() {
            std::lock_guard<std::mutex> lck(registrationMtx);

            for (auto itr : toRegister) {
                itr->createClass();
            }
            for (auto actual : toRegister) {
                // Populate fields, methods, etc. here
                actual->populateFields();
                actual->populateMethods();
                registeredTypes.push_back(actual);
            }
            toRegister.clear();
        }

        static void ExplicitRegister(std::unordered_set<TypeRegistration*> toAdd) {
            std::lock_guard<std::mutex> lck(registrationMtx);

            // Remove all instances of the types we are registering from the list of types we would register via an auto pass.
            // This allows us to be explicit.
            auto itr = toRegister.begin();
            while (itr != toRegister.end()) {
                if (toAdd.contains(*itr)) {
                    itr = toRegister.erase(itr);
                } else {
                    ++itr;
                }
            }
            for (auto item : toAdd) {
                item->createClass();
            }
            for (auto actual : toAdd) {
                actual->populateFields();
                actual->populateMethods();
                registeredTypes.push_back(actual);
            }
        }

        static void AddType(TypeRegistration* type) {
            _logger().debug("Added instance to register: %p", type);
            toRegister.push_back(type);
        }

        /// @brief Unregisters all custom types, deleting all associated data.
        /// You should not (realistically) have a need for this unless you find custom types memory overhead to be significant.
        static void UnregisterAll() {
            std::lock_guard lck(registrationMtx);
            Register::typeIdx = kTypeDefinitionIndexInvalid;
            for (auto itr : registeredTypes) {
                itr->clear();
            }
            registeredTypes.clear();
            // This will obviously cause all sorts of issues if these types are currently being used within il2cpp.
            classes.clear();
            // We also need to correctly delete any created images or assemblies.
            // These need to deleted in a very delicate way so as not to have any dangling references.
        }
    };
}