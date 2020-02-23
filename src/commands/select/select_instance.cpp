#include "commands.hpp"

struct SelectInstanceArg {
    VkInstance instance;
};

struct SelectInstanceArg * load_select_instance(CacheScope * scope, PyObject * obj) {
    SelectInstanceArg * arg = scope->data.alloc<SelectInstanceArg>();
    use_variable(scope, arg->instance, obj, "instance");
    return arg;
}

void run_select_instance(RunScope * scope, struct SelectInstanceArg * arg) {
    scope->instance = arg->instance;
}
