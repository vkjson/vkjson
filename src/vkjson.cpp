#include "common.hpp"
#include "allocator.hpp"
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
    bool export_invalid;
    Symbols imported;
    Symbols exported;
    int commands_len;
    char base[1];
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
        PyErr_Format(PyExc_TypeError, "not a list");
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
    Command * commands_array = scope->data.alloc<Command>(commands_len);

    for (int i = 0; i < commands_len; ++i) {
        PyObject * obj = PyList_GetItem(commands, i);
        if (!PyDict_Check(obj)) {
            PyErr_Format(PyExc_TypeError, "not a dict");
            return NULL;
        }

        PyObject * type_obj = PyDict_GetItemString(obj, "type");
        if (!type_obj) {
            PyErr_Format(PyExc_KeyError, "type");
            return NULL;
        }

        PyObject * command_code_obj = PyDict_GetItem(command_codes, type_obj);
        if (!command_code_obj) {
            PyErr_Format(PyExc_ValueError, "invalid type");
            return NULL;
        }

        int command_code = PyLong_AsLong(command_code_obj);
        commands_array[i].location = (int)((char *)load_proc[command_code](scope, obj) - scope->data.base);
        commands_array[i].command_code = command_code;
    }

    VariableRef * refs = scope->refs;
    while (refs) {
        Variable * variable = refs->variable;
        int * locations = scope->data.alloc<int>(refs->count);
        variable->location = (int)((char *)locations - (char *)variable);
        variable->count = refs->count;
        LocationList * lst = refs->lst;
        for (int i = 0; i < variable->count; ++i) {
            locations[i] = (int)(scope->data.base + lst->location - (char *)variable);
            lst = lst->next;
        }
        refs = refs->next;
    }

    Py_XINCREF(scope->imported.ids);
    Py_XINCREF(scope->exported.ids);
    Py_DECREF(scope->store);
    scope->store = NULL;

    Cache * res = PyObject_NewVar(Cache, Cache_type, scope->data.loc / 8);
    memcpy(res->base, scope->data.base, scope->data.loc);
    scope->data.relocate(res->base);

    res->commands_len = commands_len;
    res->imported = scope->imported;
    res->exported = scope->exported;

    res->scope = self->scope;
    res->scope.base = res->base;
    res->export_dict = NULL;
    res->export_invalid = true;
    return res;
}

PyObject * Context_meth_physical_devices(Context * self) {
    self->temp.reset();
    uint32_t num_physical_devices = 0;
    vkEnumeratePhysicalDevices(self->scope.instance, &num_physical_devices, NULL);
    VkPhysicalDevice * physical_devices = self->temp.alloc<VkPhysicalDevice>(num_physical_devices);
    vkEnumeratePhysicalDevices(self->scope.instance, &num_physical_devices, physical_devices);
    PyObject * res = PyList_New(num_physical_devices);
    for (uint32_t i = 0; i < num_physical_devices; ++i) {;
        PyList_SetItem(res, i, PyLong_FromVoidPtr(physical_devices[i]));
    }
    return res;
}

Context * Context_meth_select(Context * self, PyObject * args, PyObject * kwargs) {
    if (!load_run_scope(&self->scope, args, kwargs)) {
        return NULL;
    }
    Py_INCREF(self);
    return self;
}

PyObject * Context_meth_scope(Context * self) {
    return dump_run_scope(&self->scope);
}

Cache * Cache_meth_select(Cache * self, PyObject * args, PyObject * kwargs) {
    if (!load_run_scope(&self->scope, args, kwargs)) {
        return NULL;
    }
    Py_INCREF(self);
    return self;
}

PyObject * Cache_meth_scope(Cache * self) {
    return dump_run_scope(&self->scope);
}

Cache * Cache_meth_load(Cache * self, PyObject * args, PyObject * kwargs) {
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
    void ** values = (void **)(self->base + self->imported.location);
    for (int i = 0; i < self->imported.count; ++i) {
        if (PyObject * obj = PyDict_GetItem(kwargs, PyList_GetItem(self->imported.ids, i))) {
            values[i] = PyLong_AsVoidPtr(obj);
        }
    }
    Py_INCREF(self);
    return self;
}

Cache * Cache_meth_run(Cache * self) {
    Command * commands = (Command *)self->base;
    for (int i = 0; i < self->commands_len; ++i) {
        run_proc[commands[i].command_code](&self->scope, self->base + commands[i].location);
    }
    self->export_invalid = true;
    Py_INCREF(self);
    return self;
}

PyObject * Cache_meth_export(Cache * self) {
    if (!self->export_dict) {
        self->export_dict = PyDict_New();
    }
    if (self->export_invalid) {
        self->export_invalid = false;
        void ** values = (void **)(self->base + self->exported.location);
        for (int i = 0; i < self->exported.count; ++i) {
            PyObject * value = PyLong_FromVoidPtr(values[i]);
            PyDict_SetItem(self->export_dict, PyList_GetItem(self->exported.ids, i), value);
            Py_DECREF(value);
        }
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
    static char * keywords[] = {"commands", "objects", NULL};

    PyObject * commands;
    PyObject * objects = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &commands, &objects)) {
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
    Cache * run_res = Cache_meth_run(cache);
    Py_XDECREF(run_res);
    if (!run_res) {
        Py_DECREF(cache);
        return NULL;
    }
    Py_DECREF(cache);
    self->scope = cache->scope;
    PyObject * res = Cache_meth_export(cache);
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
    {"physical_devices", (PyCFunction)Context_meth_physical_devices, METH_NOARGS, NULL},
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
PyObject * enums;

extern "C" PyObject * PyInit_vkjson() {
    empty_tuple = PyTuple_New(0);

    helper = PyImport_ImportModule("_vkjson");
    if (!helper) {
        return NULL;
    }

    enums = PyObject_GetAttrString(helper, "enums");

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
