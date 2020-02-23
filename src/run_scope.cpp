#include "run_scope.hpp"

int load_run_scope(RunScope * scope, PyObject * args, PyObject * kwargs) {
    if (int args_len = (int)PyTuple_Size(args)) {
        if (kwargs || args_len != 1) {
            PyErr_Format(PyExc_TypeError, "invalid arguments");
            return 0;
        }
        kwargs = PyTuple_GetItem(args, 0);
        if (!PyDict_Check(kwargs)) {
            PyErr_Format(PyExc_TypeError, "invalid arguments");
            return 0;
        }
        args = empty_tuple;
    }
    static char * keywords[] = {
        "instance", "physical_device", "device", "queue", "command_buffer", "surface", "swapchain", NULL,
    };
    return PyArg_ParseTupleAndKeywords(
        args, kwargs, "|$KKKKKKK", keywords,
        &scope->instance,
        &scope->physical_device,
        &scope->device,
        &scope->queue,
        &scope->command_buffer,
        &scope->surface,
        &scope->swapchain
    );
}

PyObject * dump_run_scope(RunScope * scope) {
    return Py_BuildValue(
        "{sKsKsKsKsKsKsK}",
        "instance", scope->instance,
        "physical_device", scope->physical_device,
        "device", scope->device,
        "queue", scope->queue,
        "command_buffer", scope->command_buffer,
        "surface", scope->surface,
        "swapchain", scope->swapchain
    );
}
