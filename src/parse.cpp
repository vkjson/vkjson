#include "parse.hpp"

uint32_t get_version(CacheScope * scope, PyObject * obj, const char * key) {
    PyObject * value;
    if (obj && (value = PyDict_GetItemString(obj, key))) {
        PyObject * version = PyObject_CallMethod(helper, "parse_version", "O", value);
        return PyLong_AsUnsignedLong(version);
    }
    return 0;
}

char * get_string(CacheScope * scope, PyObject * obj, const char * key) {
    PyObject * value;
    if (obj && (value = PyDict_GetItemString(obj, key))) {
        int len = (int)PyUnicode_GetLength(value);
        char * res = scope->data.alloc<char>(len + 1);
        memcpy(res, PyUnicode_AsUTF8(value), len + 1);
        return res;
    }
    return NULL;
}

char ** get_string_array(CacheScope * scope, PyObject * obj, const char * key) {
    PyObject * array;
    if (obj && (array = PyDict_GetItemString(obj, key))) {
        int size = (int)PyList_Size(array);
        char ** res = scope->data.alloc<char *>(size);
        for (int i = 0; i < size; ++i) {
            PyObject * tmp = PyList_GetItem(array, i);
            int len = (int)PyUnicode_GetLength(tmp);
            res[i] = scope->data.alloc<char>(len + 1);
            scope->data.pointer(res[i]);
            memcpy(res[i], PyUnicode_AsUTF8(tmp), len + 1);
        }
        return res;
    }
    return NULL;
}

uint32_t get_array_size(CacheScope * scope, PyObject * obj, const char * key) {
    PyObject * array;
    if (obj && (array = PyDict_GetItemString(obj, key))) {
        return (uint32_t)PyList_Size(array);
    }
    return 0;
}
