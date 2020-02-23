#include "commands.hpp"

struct ExportArg {
    int count;
    void ** values;
};

ExportArg * load_export(CacheScope * scope, PyObject * obj) {
    ExportArg * arg = scope->data.alloc<ExportArg>();
    if (PyObject * ids = PyDict_GetItemString(obj, "ids")) {
        if (!PyList_Check(ids)) {
            PyErr_Format(PyExc_TypeError, "invalid ids");
            return arg;
        }
        arg->count = (int)PyList_Size(ids);
        arg->values = scope->data.alloc<void *>(arg->count);
        scope->data.pointer(arg->values);
        for (int i = 0; i < arg->count; ++i) {
            use_variable(scope, arg->values[i], PyList_GetItem(ids, i));
        }
        scope->exported.location = (int)((char *)arg->values - scope->data.base);
        scope->exported.count = arg->count;
        scope->exported.ids = ids;
    } else {
        PyErr_Format(PyExc_KeyError, "ids");
        return arg;
    }
    return arg;
}
