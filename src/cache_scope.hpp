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
