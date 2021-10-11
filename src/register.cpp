#define IL2CPP_FUNC_VISIBILITY public
#include "register.hpp"
#include "logging.hpp"
// Ensure we can see the il2cpp-functions "private" held values
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/capstone-utils.hpp"

template<class... TArgs>
struct Hook_FromIl2CppTypeMain {
    constexpr static const char* name() { return "FromIl2CppType"; }
    constexpr static void* addr() { return nullptr; }
    using funcType = Il2CppClass* (*)(TArgs...);
    static funcType* trampoline() { return &FromIl2CppType; }
    static inline Il2CppClass* (*FromIl2CppType)(TArgs...) = nullptr;
    static funcType hook() { return hook_FromIl2CppType; }
    static Il2CppClass* hook_FromIl2CppType(TArgs... args) {
        static auto logger = ::custom_types::_logger().WithContext("FromIl2CppType");
        auto typ = std::get<0>(std::make_tuple(args...));
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
                auto* k = ::custom_types::Register::classes[idx];
                if (k == nullptr) {
                    logger.warning("Class at idx: %u is null!", idx);
                }
                return k;
            }
        }
        // Otherwise, return orig
        auto klass = FromIl2CppType(args...);
        if (shouldBeOurs) {
            logger.debug("Called with klassIndex %i which is not our custom type?!", typ->data.klassIndex);
            il2cpp_utils::LogClass(logger, klass, false);
        }
        return klass;
    }
};

MAKE_HOOK(Class_Init, nullptr, bool, Il2CppClass* klass) {
    static auto logger = ::custom_types::_logger().WithContext("Class::Init");
    // If we are attempting to call Class::Init() on an Il2CppClass* that is a custom Il2CppClass*, we need to ignore.
    if (!klass) {
        // We will provide some useful debug info here
        logger.warning("Called with a null Il2CppClass*! (Specifically: %p)", klass);
        SAFE_ABORT();
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

MAKE_HOOK(MetadataCache_GetTypeInfoFromTypeDefinitionIndex, nullptr, Il2CppClass*, TypeDefinitionIndex index) {
    if (index < 0) {
        static auto logger = ::custom_types::_logger().WithContext("MetadataCache::GetTypeInfoFromTypeDefinitionIndex");
        // index is either invalid or one of ours
        auto idx = kTypeDefinitionIndexInvalid - index;
        logger.debug("custom idx: %u", idx);
        if (idx < ::custom_types::Register::classes.size() && idx >= 0) {
            logger.debug("Returning custom class with idx %i!", idx);
            auto* k = ::custom_types::Register::classes[idx];
            return k;
        }
    }
    // Otherwise, return orig
    return MetadataCache_GetTypeInfoFromTypeDefinitionIndex(index);
}

// NOTE THAT THIS HOOK DOES NOT PERMIT TYPES OF IDENTICAL NAMESPACE AND NAME BUT IN DIFFERENT IMAGES!
// This could be worked around if we also have image be a part of the key, but as it stands, that is not necessary.
MAKE_HOOK(Class_FromName, nullptr, Il2CppClass*, Il2CppImage* image, const char* namespaze, const char* name) {
    #ifndef NO_VERBOSE_LOGS
    static auto logger = ::custom_types::_logger().WithContext("Class::FromName");
    #endif
    auto pair = std::make_pair(std::string(namespaze), std::string(name));
    auto itr = custom_types::Register::classMapping.find(pair);
    if (itr != custom_types::Register::classMapping.end()) {
        #ifndef NO_VERBOSE_LOGS
        logger.debug("Returning custom class from: %s::%s lookup: %p", namespaze, name, itr->second);
        #endif
        return itr->second;
    }
    return Class_FromName(image, namespaze, name);
}

namespace custom_types {
    std::unordered_map<std::string, Il2CppAssembly*> Register::assembs;
    std::unordered_map<std::string, Il2CppImage*> Register::images;
    std::unordered_map<std::pair<std::string, std::string>, Il2CppClass*> Register::classMapping;
    std::shared_mutex Register::assemblyMtx;
    std::shared_mutex Register::imageMtx;
    std::mutex Register::classMappingMtx;
    std::mutex Register::registrationMtx;
    std::mutex installationMtx;
    bool Register::installed = false;
    std::vector<Il2CppClass*> Register::classes;
    std::vector<TypeRegistration*> Register::toRegister;
    std::vector<TypeRegistration*> Register::registeredTypes;

    Il2CppAssembly* Register::createAssembly(std::string_view name, Il2CppImage* img) {
        // Name is NOT copied, so should be a constant string
        // Check to see if an assembly with the given name already exists.
        // If it does, use that instead.
        std::string strName(name);
        {
            std::shared_lock lock(assemblyMtx);
            auto itr = assembs.find(strName);
            if (itr != assembs.end()) {
                return itr->second;
            }
        }
        auto assemb = new Il2CppAssembly();
        assemb->image = img;
        img->assembly = assemb;
        assemb->aname.name = name.data();
        {
            std::unique_lock lock(assemblyMtx);
            // Add our new assembly to the collection of all known assemblies
            il2cpp_functions::Assembly_GetAllAssemblies()->push_back(assemb);
            _logger().debug("Added new assembly image: %p", assemb->image);
            assembs.insert({strName, assemb});
        }
        _logger().debug("Created new assembly: %s, %p", name.data(), assemb);
        return assemb;
    }

    const Il2CppImage* Register::createImage(std::string_view name) {
        // Name is NOT copied, so should be a constant string
        // Check to see if an image with the given name already exists.
        // If it does, use that instead.
        std::string strName(name);
        {
            std::shared_lock lock(imageMtx);
            auto itr = images.find(strName);
            if (itr != images.end()) {
                return itr->second;
            }
        }
        auto img = new Il2CppImage();
        std::unique_lock lock(imageMtx);
        auto res = images.insert({strName, img});
        lock.unlock();
        img->name = res.first->first.c_str();
        auto strToCopy = strName.substr(0, strName.find_last_of('.'));
        auto* allocNameNoExt = new char[strToCopy.size() + 1];
        memcpy(allocNameNoExt, strToCopy.c_str(), strToCopy.size());
        allocNameNoExt[strToCopy.size()] = '\0';
        img->nameNoExt = allocNameNoExt;
        img->dynamic = true;
        img->assembly = createAssembly(allocNameNoExt, img);
        img->nameToClassHashTable = new Il2CppNameToTypeDefinitionIndexHashTable();
        // Types are pushed here on class creation
        // TODO: Avoid copying eventually
        img->exportedTypeStart = 0;
        img->exportedTypeCount = 0;
        // Custom attribute start and count is used somewhere within unity
        // (which makes a call to: il2cpp_custom_attrs_from_class/il2cpp_custom_attrs_from_method)
        // These are required to not be undefined (though perhaps a -1 and a 0 would work just as well here?)
        // RGCTXes are also from codeGenModule, so that must also be defined.
        img->customAttributeStart = 0;
        img->customAttributeCount = 0;
        img->entryPointIndex = 0;
        // TODO: Populate this in a more reasonable way
        // auto* codegen = new Il2CppCodeGenModule{Il2CppCodeGenModule{
        //     .moduleName = name.data(),
        //     .methodPointerCount = 0,
        //     .reversePInvokeWrapperCount = 0,
        //     .rgctxRangesCount = 0,
        //     .rgctxsCount = 0
        // }};
        // img->codeGenModule = codegen;
        // TOOD: We shall leave the others undefined for now.
        _logger().debug("Created new image: %s, %p", name.data(), img);
        return img;
    }

    void Register::EnsureHooks() {
        std::lock_guard lock(installationMtx);
        if (!installed) {
            il2cpp_functions::Init();
            static auto logger = _logger().WithContext("EnsureHooks");
            logger.debug("Installing FromIl2CppType hook...");
            if constexpr (sizeof(Il2CppCodeGenModule) < 104) {
                Hooking::InstallHookDirect<Hook_FromIl2CppTypeMain<Il2CppType*>>(logger, (void*)il2cpp_functions::il2cpp_Class_FromIl2CppType);
            } else {
                Hooking::InstallHookDirect<Hook_FromIl2CppTypeMain<Il2CppType*, bool>>(logger, (void*)il2cpp_functions::il2cpp_Class_FromIl2CppType);
            }
            INSTALL_HOOK_DIRECT(logger, MetadataCache_GetTypeInfoFromTypeDefinitionIndex, (void*)il2cpp_functions::il2cpp_MetadataCache_GetTypeInfoFromTypeDefinitionIndex);
            INSTALL_HOOK_DIRECT(logger, Class_Init, (void*)il2cpp_functions::il2cpp_Class_Init);
            {
                // We need to do a tiny bit of xref tracing to find the bottom level Class::FromName call
                // Trace is: il2cpp_class_from_name --> b --> b --> result
                INSTALL_HOOK_DIRECT(logger, Class_FromName, (void*)cs::findNthB<1>(reinterpret_cast<const uint32_t*>(il2cpp_functions::il2cpp_class_from_name)));
            }
            installed = true;
        }
    }
}