#pragma once
#include "common.hpp"

struct Variable {
    int count;
    int location;

    template <typename T>
    void set(const T & value) {
        int * locations = (int *)((char *)this + location);
        for (int i = 0; i < count; ++i) {
            *(T *)((char *)this + locations[i]) = value;
        }
    }
};

struct LocationList {
    int location;
    LocationList * next;
};

struct VariableRef {
    int count;
    LocationList * lst;
    Variable * variable;
    VariableRef * next;
};
