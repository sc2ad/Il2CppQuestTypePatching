#include "types.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

::custom_types::type_info::type_info(Il2CppTypeEnum typeE, std::string_view ns, std::string_view n, Il2CppClass* b, std::initializer_list<Il2CppClass*> interfaces, bool abst) {
    typeEnum = typeE;
    il2cpp_functions::Init();
    if (b) {
        il2cpp_functions::Class_Init(b);
    }
    base = b;
    namespaze = std::string(ns);
    name = std::string(n);
    this->interfaces = interfaces;
    this->abst = abst;
}

::custom_types::field_info::field_info(std::string_view name, const Il2CppType* type, uint16_t fieldAttrs) {
    // Create FieldInfo*
    info = FieldInfo{};
    info.name = name.data();
    // We want to make sure we set the correct attributes on this type
    // For that purpose, we actually want to COPY the type
    auto* tmp = new Il2CppType();
    tmp->attrs = type->attrs | fieldAttrs;
    tmp->byref = type->byref;
    tmp->data.dummy = type->data.dummy;
    tmp->num_mods = type->num_mods;
    tmp->pinned = type->pinned;
    tmp->type = type->type;
    info.type = tmp;
    info.token = -1;
}

::custom_types::method_info::method_info(std::string_view name, void* func, InvokerMethod invoker, const Il2CppType* returnType, std::vector<ParameterInfo>& parameters, uint16_t flags, const MethodInfo* virtualData) {
    // Create MethodInfo*
    info = new MethodInfo();
    virtual_data = virtualData;
    params = parameters;
    info->name = name.data();
    info->methodPointer = (Il2CppMethodPointer)func;
    // TODO: Need to figure out how to reliably set invoker functions
    // This is probably going to be really challenging...
    // For now, we will try to make our own and see if we crash
    info->invoker_method = invoker;
    info->return_type = returnType;
    info->parameters = params.data();
    info->parameters_count = params.size();
    info->flags = flags;
    // Slot is set when these methods are registered
    // info->slot = virtual_data ? virtual_data->slot : kInvalidIl2CppMethodSlot;
    // TODO: set more data on method, perhaps pass in less?
}
