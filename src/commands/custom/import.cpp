#include "commands.hpp"

struct ImportArg {
    int count;
    void ** values;
    Variable * refs;
};

ImportArg * load_import(CacheScope * scope, PyObject * obj) {
    ImportArg * arg = scope->data.alloc<ImportArg>();
    if (PyObject * ids = PyDict_GetItemString(obj, "ids")) {
        if (!PyList_Check(ids)) {
            PyErr_Format(PyExc_TypeError, "invalid ids");
            return arg;
        }
        arg->count = (int)PyList_Size(ids);
        arg->values = scope->data.alloc<void *>(arg->count);
        scope->data.pointer(arg->values);
        arg->refs = scope->data.alloc<Variable>(arg->count);
        scope->data.pointer(arg->refs);
        for (int i = 0; i < arg->count; ++i) {
            new_variable(scope, arg->refs[i], PyList_GetItem(ids, i));
        }
        scope->imported.location = (int)((char *)arg->values - scope->data.base);
        scope->imported.count = arg->count;
        scope->imported.ids = ids;
    } else {
        PyErr_Format(PyExc_KeyError, "ids");
        return arg;
    }
    return arg;
}

void run_import(RunScope * scope, ImportArg * arg) {
    for (int i = 0; i < arg->count; ++i) {
        arg->refs[i].set(arg->values[i]);
    }
}
