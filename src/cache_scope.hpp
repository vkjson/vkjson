#pragma once
#include "common.hpp"
#include "allocator.hpp"
#include "symbols.hpp"
#include "variable.hpp"

struct CacheScope {
    PyObject * store;
    VariableRef * refs;
    Symbols imported;
    Symbols exported;
    Allocator data;
    Allocator temp;
};

inline void new_variable(CacheScope * scope, Variable & variable, PyObject * obj, const char * key = NULL) {
    if (key) {
        obj = PyDict_GetItemString(obj, key);
        if (!obj) {
            PyErr_Format(PyExc_KeyError, "%s", key);
            return;
        }
    }
    VariableRef * item = scope->temp.alloc<VariableRef>();
    item->count = 0;
    item->lst = NULL;
    item->variable = &variable;
    item->next = scope->refs;
    scope->refs = item;
    PyObject * ptr = PyLong_FromVoidPtr(item);
    if (PyDict_GetItem(scope->store, obj)) {
        PyErr_Format(PyExc_ValueError, "duplicate");
        return;
    }
    PyDict_SetItem(scope->store, obj, ptr);
    Py_DECREF(ptr);
}

template <typename T>
inline void use_variable(CacheScope * scope, T & variable, PyObject * obj, const char * key = NULL) {
    if (key) {
        obj = PyDict_GetItemString(obj, key);
        if (!obj) {
            PyErr_Format(PyExc_KeyError, "%s", key);
            return;
        }
    }
    if (PyObject * value = PyDict_GetItem(scope->store, obj)) {
        VariableRef * ref = (VariableRef *)PyLong_AsVoidPtr(value);
        LocationList * item = scope->temp.alloc<LocationList>();
        item->location = (int)((char *)&variable - scope->data.base);
        item->next = ref->lst;
        ref->lst = item;
        ref->count += 1;
    } else {
        PyErr_Format(PyExc_KeyError, "%s", key);
        return;
    }
}

inline void del_variable(CacheScope * scope, PyObject * id) {
    PyDict_DelItem(scope->store, id);
}
