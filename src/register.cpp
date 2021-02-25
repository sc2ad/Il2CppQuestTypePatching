#include "register.hpp"
#include "logging.hpp"
#if __has_include(<coroutine>)
#include "coroutine.hpp"

void inline RegisterHelpers() {
    custom_types::Register::RegisterTypes<
        custom_types::Helpers::ResetableCoroutine,
        custom_types::Helpers::StandardCoroutine
    >();
}
#else
void inline RegisterHelpers() {

}
#endif

MAKE_HOOK(FromIl2CppType, NULL, Il2CppClass*, Il2CppType* typ) {
    static auto logger = ::custom_types::_logger().WithContext("FromIl2CppType");
    // _logger().debug("FromIl2CppType: %p", typ);
    if (typ == nullptr) {
        // Extra error checking to avoid unknown null derefs.
        logger.warning("FromIl2CppType was given a null Il2CppType*! Returning a null!");
        return nullptr;
    }
    bool shouldBeOurs = false;
    // klassIndex is only meaningful for these types
    if ((typ->type == IL2CPP_TYPE_CLASS || typ->type == IL2CPP_TYPE_VALUETYPE) && typ->data.klassIndex < 0) {
        shouldBeOurs = true;
        // If the type matches our type
        auto idx = kTypeDefinitionIndexInvalid - typ->data.klassIndex;
        #ifndef NO_VERBOSE_LOGS
        logger.debug("Custom idx: %u for type: %p", idx, typ);
        #endif
        if (idx < ::custom_types::Register::classes.size() && idx >= 0) {
            #ifndef NO_VERBOSE_LOGS
            logger.debug("Returning custom class with idx %i!", idx);
            #endif
            auto* wrapper = ::custom_types::Register::classes[idx];
            if (wrapper == nullptr) {
                logger.warning("Wrapper at idx: %u is null!", idx);
            }
            if (wrapper->get() == nullptr) {
                logger.warning("Wrapper value is null at idx: %u!", idx);
            }
            return const_cast<Il2CppClass*>(wrapper->get());
        }
    }
    // Otherwise, return orig
    auto klass = FromIl2CppType(typ);
    if (shouldBeOurs) {
        logger.debug("Called with klassIndex %i which is not our custom type?!", typ->data.klassIndex);
        il2cpp_utils::LogClass(logger, klass, false);
    }
    return klass;
}

MAKE_HOOK(Class_Init, NULL, bool, Il2CppClass* klass) {
    static auto logger = ::custom_types::_logger().WithContext("Class::Init");
    // If we are attempting to call Class::Init() on an Il2CppClass* that is a custom Il2CppClass*, we need to ignore.
    if (!klass) {
        // We will provide some useful debug info here
        logger.warning("Called with a null Il2CppClass*! (Specifically: %p)", klass);
        CRASH_UNLESS(false);
    }
    auto typ = klass->this_arg;
    if ((typ.type == IL2CPP_TYPE_CLASS || typ.type == IL2CPP_TYPE_VALUETYPE) && typ.data.klassIndex < 0) {
        // This is a custom class. Skip it.
        auto idx = kTypeDefinitionIndexInvalid - typ.data.klassIndex;
        #ifndef NO_VERBOSE_LOGS
        logger.debug("custom idx: %u", idx);
        #endif
        return true;
    } else {
        return Class_Init(klass);
    }
}

MAKE_HOOK(MetadataCache_GetTypeInfoFromTypeDefinitionIndex, NULL, Il2CppClass*, TypeDefinitionIndex index) {
    if (index < 0) {
        static auto logger = ::custom_types::_logger().WithContext("MetadataCache::GetTypeInfoFromTypeDefinitionIndex");
        // index is either invalid or one of ours
        auto idx = kTypeDefinitionIndexInvalid - index;
        logger.debug("custom idx: %u", idx);
        if (idx < ::custom_types::Register::classes.size() && idx >= 0) {
            logger.debug("Returning custom class with idx %i!", idx);
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
            static auto logger = _logger().WithContext("EnsureHooks");
            logger.debug("Installing FromIl2CppType hook...");
            INSTALL_HOOK_DIRECT(logger, FromIl2CppType, (void*)il2cpp_functions::Class_FromIl2CppType);
            INSTALL_HOOK_DIRECT(logger, MetadataCache_GetTypeInfoFromTypeDefinitionIndex, (void*)il2cpp_functions::MetadataCache_GetTypeInfoFromTypeDefinitionIndex);
            INSTALL_HOOK_DIRECT(logger, Class_Init, (void*)il2cpp_functions::Class_Init);
            installed = true;

            // Also register types that would otherwise be never registered (see: all Helper types)
            RegisterHelpers();
        }
    }
}