#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"

const Logger& logger();

void logAname(Il2CppAssemblyName* name, std::string_view anameLabel);
void logAsm(Il2CppAssembly* assem);
void logCodegen(const Il2CppCodeGenModule* module, std::string_view s);
void logImage(const Il2CppImage* img);
void logType(const Il2CppType* t, std::string_view s);
void logParam(const ParameterInfo* info);
void logMethod(const MethodInfo* info);
void logAll(Il2CppClass* klass);