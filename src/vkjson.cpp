#include "common.hpp"
#include "allocator.hpp"
#include "cache_command.hpp"
#include "commands.hpp"
#include "run_scope.hpp"

struct Context {
    PyObject_HEAD
    RunScope scope;
    PyObject * allocated;
    Allocator data;
    Allocator temp;
};

struct Thread {
    PyObject_HEAD
};

struct Cache {
    PyObject_VAR_HEAD
    RunScope scope;
    PyObject * export_dict;
    Symbols imported;
    Symbols exported;
    int commands_len;
    union {
        CacheCommand commands[1];
        char base[1];
    };
};

PyTypeObject * Context_type;
PyTypeObject * Thread_type;
PyTypeObject * Cache_type;

void free_later(PyObject * capsule) {
    PyMem_Free(PyCapsule_GetPointer(capsule, NULL));
}

Context * vkjson_meth_context(PyObject * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"max_data_size", "max_temp_size", NULL};

    int max_data_size = 1024 * 1024;
    int max_temp_size = 1024 * 1024;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|ii", keywords, &max_data_size, &max_temp_size)) {
        return NULL;
    }

    Context * context = PyObject_New(Context, Context_type);
    char * ptr = (char *)PyMem_Malloc(max_data_size + max_temp_size);
    context->allocated = PyCapsule_New(ptr, NULL, free_later);
    context->data.base = ptr;
    context->data.size = max_data_size;
    context->temp.base = ptr + max_data_size;
    context->temp.size = max_temp_size;
    context->scope = {};
    return context;
}

Thread * vkjson_meth_thread(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "", keywords)) {
        return NULL;
    }

    Thread * res = PyObject_New(Thread, Thread_type);
    return res;
}

Cache * Context_meth_cache(Context * self, PyObject * commands) {
    if (!PyList_Check(commands)) {
        return NULL;
    }

    CacheScope cache_scope = {};
    CacheScope * scope = &cache_scope;
    scope->data = self->data;
    scope->temp = self->temp;
    scope->data.reset();
    scope->temp.reset();

    scope->store = PyDict_New();
    scope->imported = {};
    scope->exported = {};
    scope->refs = NULL;

    int commands_len = (int)PyList_Size(commands);

    Py_XINCREF(scope->imported.ids);
    Py_XINCREF(scope->exported.ids);
    Py_DECREF(scope->store);
    scope->store = NULL;

    Cache * res = PyObject_NewVar(Cache, Cache_type, scope->data.loc / 8);
    res->commands_len = commands_len;
    res->imported = scope->imported;
    res->exported = scope->exported;

    res->scope = self->scope;
    res->scope.base = res->base;
    res->export_dict = NULL;
    return res;
}

Context * Context_meth_select(Context * self, PyObject * args, PyObject * kwargs) {
    if (!load_scope(&self->scope, args, kwargs)) {
        return NULL;
    }
    Py_INCREF(self);
    return self;
}

PyObject * Context_meth_scope(Context * self) {
    return dump_scope(&self->scope);
}

Cache * Cache_meth_select(Cache * self, PyObject * args, PyObject * kwargs) {
    if (!load_scope(&self->scope, args, kwargs)) {
        return NULL;
    }
    Py_INCREF(self);
    return self;
}

PyObject * Cache_meth_scope(Cache * self) {
    return dump_scope(&self->scope);
}

Cache * Cache_meth_load(Cache * self, PyObject * args, PyObject * kwargs) {
    Py_INCREF(self);
    return self;
}

Cache * Cache_meth_run(Cache * self) {
    Py_INCREF(self);
    return self;
}

PyObject * Cache_meth_export(Cache * self) {
    if (!self->export_dict) {
        self->export_dict = PyDict_New();
    }
    Py_INCREF(self->export_dict);
    return self->export_dict;
}

PyObject * Cache_meth_get(Cache * self, PyObject * key) {
    PyObject * export_dict = Cache_meth_export(self);
    PyObject * res = PyDict_GetItem(export_dict, key);
    Py_DECREF(export_dict);
    return res;
}

PyObject * Context_meth_run(Context * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"commands", "objects", "select", NULL};

    PyObject * commands;
    PyObject * objects = Py_None;
    int select = true;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Op", keywords, &commands, &objects, &select)) {
        return NULL;
    }

    Cache * cache = Context_meth_cache(self, commands);
    if (!cache) {
        return NULL;
    }
    if (objects != Py_None) {
        Cache * load_res = Cache_meth_load(cache, NULL, objects);
        Py_XDECREF(load_res);
        if (!load_res) {
            Py_DECREF(cache);
            return NULL;
        }
    }
    Cache * exec_res = Cache_meth_run(cache);
    Py_XDECREF(exec_res);
    if (!exec_res) {
        Py_DECREF(cache);
        return NULL;
    }
    Py_DECREF(cache);
    PyObject * res = Cache_meth_export(cache);
    if (select) {
        Context * select_res = Context_meth_select(self, empty_tuple, res);
        Py_XDECREF(select_res);
        if (!select_res) {
            Py_DECREF(res);
            return NULL;
        }
    }
    return res;
}

Thread * Thread_meth_run(Thread * self, Cache * cache) {
    Py_INCREF(self);
    return self;
}

Thread * Thread_meth_wait(Thread * self) {
    Py_INCREF(self);
    return self;
}

PyMethodDef Context_methods[] = {
    {"cache", (PyCFunction)Context_meth_cache, METH_O, NULL},
    {"select", (PyCFunction)Context_meth_select, METH_VARARGS | METH_KEYWORDS, NULL},
    {"run", (PyCFunction)Context_meth_run, METH_VARARGS | METH_KEYWORDS, NULL},
    {"scope", (PyCFunction)Context_meth_scope, METH_NOARGS, NULL},
    {},
};

PyMethodDef Cache_methods[] = {
    {"select", (PyCFunction)Cache_meth_select, METH_VARARGS | METH_KEYWORDS, NULL},
    {"load", (PyCFunction)Cache_meth_load, METH_VARARGS | METH_KEYWORDS, NULL},
    {"run", (PyCFunction)Cache_meth_run, METH_NOARGS, NULL},
    {"scope", (PyCFunction)Cache_meth_scope, METH_NOARGS, NULL},
    {"export", (PyCFunction)Cache_meth_export, METH_NOARGS, NULL},
    {"get", (PyCFunction)Cache_meth_get, METH_O, NULL},
    {},
};

PyMethodDef Thread_methods[] = {
    {"run", (PyCFunction)Thread_meth_run, METH_O, NULL},
    {"wait", (PyCFunction)Thread_meth_wait, METH_NOARGS, NULL},
    {},
};

void default_dealloc(PyObject * self) {
    Py_TYPE(self)->tp_free(self);
}

PyType_Slot Context_slots[] = {
    {Py_tp_methods, Context_methods},
    {Py_tp_dealloc, default_dealloc},
    {},
};

PyType_Slot Cache_slots[] = {
    {Py_tp_methods, Cache_methods},
    {Py_tp_dealloc, default_dealloc},
    {},
};

PyType_Slot Thread_slots[] = {
    {Py_tp_methods, Thread_methods},
    {Py_tp_dealloc, default_dealloc},
    {},
};

PyType_Spec Context_spec = {"vkjson.Context", sizeof(Context), 0, Py_TPFLAGS_DEFAULT, Context_slots};
PyType_Spec Cache_spec = {"vkjson.Cache", offsetof(Cache, base), 8, Py_TPFLAGS_DEFAULT, Cache_slots};
PyType_Spec Thread_spec = {"vkjson.Thread", sizeof(Thread), 0, Py_TPFLAGS_DEFAULT, Thread_slots};

PyMethodDef vkjson_methods[] = {
    {"context", (PyCFunction)vkjson_meth_context, METH_VARARGS | METH_KEYWORDS, NULL},
    {"thread", (PyCFunction)vkjson_meth_thread, METH_VARARGS | METH_KEYWORDS, NULL},
    {},
};

PyModuleDef vkjson_def = {PyModuleDef_HEAD_INIT, "vkjson", NULL, -1, vkjson_methods};

PyObject * empty_tuple;
PyObject * helper;

extern "C" PyObject * PyInit_vkjson() {
    empty_tuple = PyTuple_New(0);

    helper = PyImport_ImportModule("_vkjson");
    if (!helper) {
        return NULL;
    }

    PyObject * module = PyModule_Create(&vkjson_def);

    build_command_codes();

    Context_type = (PyTypeObject *)PyType_FromSpec(&Context_spec);
    Cache_type = (PyTypeObject *)PyType_FromSpec(&Cache_spec);
    Thread_type = (PyTypeObject *)PyType_FromSpec(&Thread_spec);

    PyModule_AddObject(module, "Context", (PyObject *)Context_type);
    PyModule_AddObject(module, "Cache", (PyObject *)Cache_type);
    PyModule_AddObject(module, "Thread", (PyObject *)Cache_type);

    return module;
}
