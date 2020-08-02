#pragma once

#include <stdlib.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>

typedef struct ModInfo {
    std::string id;
    std::string version;
} ModInfo;

typedef struct ModloaderInfo {
    std::string name;
    std::string tag;
} ModloaderInfo;

class Mod;

// Different copy of Modloader for the modloader itself vs. as a header
#ifndef MODLOADER_DEFINED
class Modloader {
    public:
        // Returns the libil2cpp.so path, should only be called AFTER mods have been constructed
        // Check Mod::constructed for validity
        static const std::string getLibIl2CppPath();
        // Returns the application id, should only be called AFTER mods have been constructed
        // Check Mod::constructed for validity
        // Example return: com.beatgames.beatsaber
        static const std::string getApplicationId();
        // Returns whether all mods on this modloader have been loaded or not
        static const bool getAllConstructed();
        // Modloader info
        static const ModloaderInfo getInfo();
        // A map of id to mods managed by this modloader
        static const std::unordered_map<std::string, const Mod> getMods();
};
#endif

// Provides metadata for each mod
class Mod
{
    friend class Modloader;
    public:
        Mod(std::string_view name_, std::string_view path, ModInfo info_, void *handle_) : name(name_), pathName(path), info(info_), handle(handle_) {}
        const std::string name;
        const std::string pathName;
        const ModInfo info;
        const bool get_loaded() const;
    private:
        void init_mod();
        void load_mod();
        bool loaded;
        void *handle;
        bool init_loaded = false;
        void (*init_func)(void) = NULL;
        bool load_loaded = false;
        void (*load_func)(void) = NULL;
};