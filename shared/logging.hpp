#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include <string_view>

struct Il2CppAssemblyName;
struct Il2CppAssembly;
struct Il2CppCodeGenModule;
struct Il2CppImage;
struct Il2CppType;
struct ParameterInfo;
struct MethodInfo;
struct Il2CppClass;
struct VirtualInvokeData;
struct Il2CppRuntimeInterfaceOffsetPair;

namespace custom_types {
    /// @brief Returns the logger used within custom types. Should not be called publicly.
    /// @return The logger used internally
    Logger& _logger();

    /// @brief Logs the provided Il2CppAssemblyName* with a provided label.
    /// @param name The Il2CppAssemblyName* to log all fields on.
    /// @param anameLabel The label for the logging.
    void logAname(const Il2CppAssemblyName* name, std::string_view anameLabel);

    /// @brief Logs the provided Il2CppAssembly*
    /// @param assem The Il2CppAssembly* to log all fields on.
    void logAsm(const Il2CppAssembly* assem);

    /// @brief Logs the provided Il2CppCodeGenModule* with a provided label.
    /// @param module The Il2CppCodeGenModule* to log all fields on.
    /// @param s The label for the logging.
    void logCodegen(const Il2CppCodeGenModule* m, std::string_view s);

    /// @brief Logs the provided Il2CppImage*
    /// @param img The Il2CppImage* to log all fields on.
    void logImage(const Il2CppImage* img);

    /// @brief Logs the provided Il2CppType* with a provided label.
    /// @param t The Il2CppType* to log all fields on.
    /// @param s The label for the logging.
    void logType(const Il2CppType* t, std::string_view s);

    /// @brief Logs the provided Il2CppClass*'s vtable
    void logVtable(const VirtualInvokeData* invokeData);

    /// @brief Logs various information about the fields of the provided Il2CppClass*
    void logFields(const Il2CppClass* klass);

    /// @brief Logs the provided Il2CppRuntimeInterfaceOffsetPair
    void logInterfaceOffset(const Il2CppRuntimeInterfaceOffsetPair* pair);

    /// @brief Logs the provided ParameterInfo*
    /// @param info The ParameterInfo* to log all fields on.
    void logParam(const ParameterInfo* info);

    /// @brief Logs the provided MethodInfo*
    /// @param info The MethodInfo* to log all fields on.
    void logMethod(const MethodInfo* info);

    /// @brief Logs the provided Il2CppClass*
    /// @param klass The Il2CppClass* to log all fields on.
    void logAll(const Il2CppClass* klass);
}