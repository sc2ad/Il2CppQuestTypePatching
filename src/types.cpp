#include "types.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

field_info::field_info(std::string_view name, const Il2CppType* type, int32_t offset) {
    // Create FieldInfo*
    info = new FieldInfo();
    info->name = name.data();
    info->type = type;
    info->offset = offset;
    info->token = -1;
}

field_info::~field_info() {
    delete info;
}

constexpr void field_info::setClass(Il2CppClass* klass) const {
    info->parent = klass;
}

constexpr const FieldInfo* field_info::get() const {
    return info;
}

method_info::method_info(std::string_view name, void* func, const Il2CppType* returnType, std::vector<ParameterInfo>& parameters, uint16_t flags) {
    // Create MethodInfo*
    info = new MethodInfo();
    params = parameters;
    info->name = name.data();
    info->methodPointer = (Il2CppMethodPointer)func;
    info->return_type = returnType;
    info->parameters = params.data();
    info->parameters_count = params.size();
    info->flags = flags;
    // TODO: set more data on method, perhaps pass in less?
}

method_info::~method_info() {
    delete info;
}

constexpr const MethodInfo* method_info::get() const {
    return info;
}

constexpr void method_info::setClass(Il2CppClass* klass) const {
    info->klass = klass;
}

constexpr void method_info::setInvoker(InvokerMethod invoker) const {
    info->invoker_method = invoker;
}

int32_t _get_field_offset() {
    return 0;
}

void _increment_field_offset(std::size_t off) {
    // Increment field offset
}

class T {
    public:
    DECLARE_FIELD(int, x);
    DECLARE_FIELD(Vector3, y);

    DECLARE_METHOD(void, T, Start);

    static DECLARE_METHOD(void, T, Test, int x, float y);

    REGISTER_TYPE(
        REGISTER_FIELD(x);
        REGISTER_FIELD(y);
        REGISTER_METHOD(Start);
        REGISTER_METHOD(Test);
    )
};

void T::Start() {
    
}

void T::Test(int x, float y) {

}
