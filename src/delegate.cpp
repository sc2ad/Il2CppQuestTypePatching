#include "delegate.hpp"

namespace custom_types {

int get_delegate_count() {
    static int counter = 0;
    return counter++;
}

void log_delegate(Il2CppDelegate* d) {
    auto& logger = custom_types::_logger();
    logger.debug("LOGGING DELEGATE: %p", d);
    if (d) {
        logger.debug("data: %p", d->data);
        logger.debug("delegate_trampoline: %p", d->delegate_trampoline);
        logger.debug("extraArg: %zu", d->extraArg);
        logger.debug("invoke_impl: %p", d->invoke_impl);
        logger.debug("method: %p", d->method);
        logger.debug("method_code: %p", d->method_code);
        logger.debug("method_info: %p", d->method_info);
        logger.debug("method_is_virtual: %s", d->method_is_virtual ? "true" : "false");
        logger.debug("method_ptr: %p", d->method_ptr);
        logger.debug("klass ptr: %p", d->object.klass);
        logger.debug("original_method_info: %p", d->original_method_info);
        logger.debug("target: %p", d->target);
    }
}

#ifdef TEST_DELEGATE

static int capture;

void main() {
    int cap = 3;
    std::function<void (Il2CppObject*, int, Il2CppObject*)> f([cap](Il2CppObject*, int, Il2CppObject*) {
        capture += cap;
    });
    custom_types::MakeDelegate<Il2CppObject*>(f);
    auto ptr = DelegateWrapperStatic<void, Il2CppObject*, int, Il2CppObject*>::___TypeRegistration::klass_ptr;
}

#endif

}