#pragma once
#include "common.hpp"
#include "cache_scope.hpp"

uint32_t get_version(CacheScope * scope, PyObject * obj, const char * key);
char * get_string(CacheScope * scope, PyObject * obj, const char * key);
char ** get_string_array(CacheScope * scope, PyObject * obj, const char * key);
uint32_t get_array_size(CacheScope * scope, PyObject * obj, const char * key);
