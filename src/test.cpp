#include "main.hpp"
#include "modloader/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"

ModInfo& modInfo;

const Logger& logger() {
    static const Logger logger(modInfo);
    return logger;
}


DECLARE_CLASS(Il2CppNamespace, MyType, "UnityEngine", "MonoBehaviour",
    DECLARE_INSTANCE_FIELD(int, x);
    DECLARE_INSTANCE_FIELD(Vector3, y);

    DECLARE_METHOD(void, Start);

    DECLARE_METHOD(static void, Test, int x, float y);

    DECLARE_METHOD(int, asdf, int q);

    REGISTER_FUNCTION(MyType,
        REGISTER_FIELD(x);
        REGISTER_FIELD(y);
        REGISTER_METHOD(Start);
        REGISTER_METHOD(Test);
        REGISTER_METHOD(asdf);
    )
)

void Il2CppNamespace::MyType::Start() {
    
}

int Il2CppNamespace::MyType::asdf(int q) {
    
}

void Il2CppNamespace::MyType::Test(int x, float y) {

}

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
    logger().debug("Completed setup!");
}

extern "C" void load() {
    logger().debug("Registering type!");
    custom_types::Register::RegisterType<::Il2CppNamespace::MyType>();
}