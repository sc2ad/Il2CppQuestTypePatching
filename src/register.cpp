#include "register.hpp"
#include "logging.hpp"

MAKE_HOOK(FromIl2CppType, NULL, Il2CppClass*, Il2CppType* typ) {
    // _logger().debug("FromIl2CppType: %p", typ);
    bool shouldBeOurs = false;
    // klassIndex is only meaningful for these types
    if ((typ->type == IL2CPP_TYPE_CLASS || typ->type == IL2CPP_TYPE_VALUETYPE) && typ->data.klassIndex < 0) {
        shouldBeOurs = true;
        // If the type matches our type
        auto idx = kTypeDefinitionIndexInvalid - typ->data.klassIndex;
        ::custom_types::_logger().debug("(FromIl2CppType) custom idx: %u for type: %p", idx, typ);
        if (idx < ::custom_types::Register::classes.size() && idx >= 0) {
            ::custom_types::_logger().debug("(FromIl2CppType) Returning custom class with idx %i!", idx);
            auto* wrapper = ::custom_types::Register::classes[idx];
            return const_cast<Il2CppClass*>(wrapper->get());
        }
    }
    // Otherwise, return orig
    auto klass = FromIl2CppType(typ);
    if (shouldBeOurs) {
        ::custom_types::_logger().debug("FromIl2CppType called with klassIndex %i which is not our custom type?!", typ->data.klassIndex);
        il2cpp_utils::LogClass(klass, false);
    }
    return klass;
}

MAKE_HOOK(MetadataCache_GetTypeInfoFromTypeDefinitionIndex, NULL, Il2CppClass*, TypeDefinitionIndex index) {
    if (index < 0) {
        // index is either invalid or one of ours
        auto idx = kTypeDefinitionIndexInvalid - index;
        ::custom_types::_logger().debug("(MetadataCache) custom idx: %u", idx);
        if (idx < ::custom_types::Register::classes.size() && idx >= 0) {
            ::custom_types::_logger().debug("(MetadataCache) Returning custom class with idx %i!", idx);
            auto* wrapper = ::custom_types::Register::classes[idx];
            return const_cast<Il2CppClass*>(wrapper->get());
        }
    }
    // Otherwise, return orig
    return MetadataCache_GetTypeInfoFromTypeDefinitionIndex(index);
}

namespace custom_types {
    std::unordered_map<std::string, Il2CppAssembly*> Register::assembs;
    std::unordered_map<std::string, Il2CppImage*> Register::images;
    std::vector<ClassWrapper*> Register::classes;
    bool Register::installed = false;

    Il2CppAssembly* Register::createAssembly(std::string_view name, Il2CppImage* img) {
        // Name is NOT copied, so should be a constant string
        // Check to see if an assembly with the given name already exists.
        // If it does, use that instead.
        std::string strName(name);
        auto itr = assembs.find(strName);
        if (itr != assembs.end()) {
            return itr->second;
        }
        auto assemb = new Il2CppAssembly();
        assemb->image = img;
        img->assembly = assemb;
        assemb->aname.name = name.data();
        il2cpp_functions::Assembly_GetAllAssemblies()->push_back(assemb);
        assembs.insert({strName, assemb});
        _logger().debug("Created new assembly: %s, %p", name.data(), assemb);
        return assemb;
    }

    Il2CppImage* Register::createImage(std::string_view name) {
        // Name is NOT copied, so should be a constant string
        // Check to see if an image with the given name already exists.
        // If it does, use that instead.
        // TODO: WE NEED TO CREATE A non-null culture!
        // That way il2cpp doesn't hard exit with our image
        std::string strName(name);
        auto itr = images.find(strName);
        if (itr != images.end()) {
            return itr->second;
        }
        auto img = new Il2CppImage();
        img->name = name.data();
        img->nameNoExt = name.data();
        img->dynamic = true;
        img->assembly = createAssembly(name, img);
        img->nameToClassHashTable = new Il2CppNameToTypeDefinitionIndexHashTable();
        // Types are pushed here on class creation
        // TODO: Unclear if more is required
        images.insert({strName, img});
        _logger().debug("Created new image: %s, %p", name.data(), img);
        return img;
    }

    void Register::EnsureHooks() {
        if (!installed) {
            il2cpp_functions::Init();
            _logger().debug("Installing FromIl2CppType hook...");
            INSTALL_HOOK_DIRECT(FromIl2CppType, (void*)il2cpp_functions::Class_FromIl2CppType);
            INSTALL_HOOK_DIRECT(MetadataCache_GetTypeInfoFromTypeDefinitionIndex, (void*)il2cpp_functions::MetadataCache_GetTypeInfoFromTypeDefinitionIndex);
            installed = true;
        }
    }
}