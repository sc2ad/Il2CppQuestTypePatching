#include <dlfcn.h>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
typedef struct Il2CppType Il2CppType;
typedef struct ParameterInfo ParameterInfo;

typedef struct method_info {
    const char* name;
    void* function;
    void* invoker_function;
    Il2CppType* returnType;
    uint8_t parameterCount;
    // Should be allocated on heap, with length parameterCount
    // Must not be null
    ParameterInfo* parameters;
    uint16_t flags;
} method_info_t;

extern "C" void load();
