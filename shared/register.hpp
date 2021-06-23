#pragma once
#include "types.hpp"
#include <unordered_map>
#include <list>
#include <vector>
#include "logging.hpp"
#include <mutex>
#include <shared_mutex>
#include <string>

template<>
struct std::hash<std::pair<std::string, std::string>> {
    size_t operator()(const std::pair<std::string, std::string>& item) const noexcept {
        std::size_t h1 = std::hash<std::string>{}(item.first);
        std::size_t h2 = std::hash<std::string>{}(item.second);
        return h1 ^ (h2 << 1);
    }
};

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
        static std::mutex classMappingMtx;
        static bool installed;

        static std::vector<TypeRegistration*> toRegister;
        static std::vector<TypeRegistration*> registeredTypes;
        static TypeDefinitionIndex typeIdx;

        static Il2CppAssembly* createAssembly(std::string_view name, Il2CppImage* img);
        static const Il2CppImage* createImage(std::string_view name);

        static void EnsureHooks();

        static void addToMapping(const TypeRegistration* itr) {
            auto namePair = std::make_pair(std::string(itr->namespaze()), std::string(itr->name()));
            if (!classMapping.contains(namePair)) {
                custom_types::_logger().debug("Adding new type to class mapping: %p", itr->klass());
                // Only add to the class mapping if the exact type created does not already exist.
                classMapping.insert({namePair, itr->klass()});
            }
        }

        public:
        static std::unordered_map<std::pair<std::string, std::string>, Il2CppClass*> classMapping;
        static std::vector<Il2CppClass*> classes;
        static std::vector<TypeRegistration*> const& getTypes() {
            return registeredTypes;
        }
        /// @brief Automatically registers all pending types.
        /// To add a type to be registered, see: AddType
        /// To get the list of all registered types, see: getTypes()
        static void AutoRegister() {
            EnsureHooks();
            std::lock_guard<std::mutex> lck(registrationMtx);

            {
                std::lock_guard lock(classMappingMtx);
                for (auto itr : toRegister) {
                    itr->createClass();
                    _logger().debug("Created class! registration: %p, %s::%s klass: %p, %s::%s, image: %p", itr, itr->namespaze(), itr->name(), itr->klass(), itr->klass()->namespaze, itr->klass()->name, itr->klass()->image);
                    addToMapping(itr);
                }
            }
            for (auto actual : toRegister) {
                // Populate fields, methods, etc. here
                if (!actual->initialized()) {
                    actual->populateFields();
                    actual->populateMethods();
                    actual->setInitialized();
                }
                _logger().debug("Registered registration: %p, %s::%s klass: %p, %s::%s, image: %p", actual, actual->namespaze(), actual->name(), actual->klass(), actual->klass()->namespaze, actual->klass()->name, actual->klass()->image);
                registeredTypes.push_back(actual);
            }
            toRegister.clear();
        }

        static void ExplicitRegister(std::unordered_set<TypeRegistration*> toAdd) {
            EnsureHooks();
            std::lock_guard<std::mutex> lck(registrationMtx);

            // Remove everything we are explicitly registering from the collection of types that would be auto registered.
            // This allows us to avoid duplicate registrations.
            auto itr = toRegister.begin();
            while (itr != toRegister.end()) {
                if (toAdd.contains(*itr)) {
                    itr = toRegister.erase(itr);
                } else {
                    ++itr;
                }
            }
            {
                std::lock_guard lock(classMappingMtx);
                for (auto itr : toAdd) {
                    itr->createClass();
                    addToMapping(itr);
                }
            }
            for (auto actual : toAdd) {
                if (!actual->initialized()) {
                    actual->populateFields();
                    actual->populateMethods();
                    actual->setInitialized();
                }
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
            classMapping.clear();
            // We also need to correctly delete any created images or assemblies.
            // These need to deleted in a very delicate way so as not to have any dangling references.
        }
    };
}