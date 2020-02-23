#pragma once
#include "common.hpp"

inline int align_size(const int & size) {
    return (size + 0x7) & ~0x7;
}

struct Allocator {
    char * base;
    int size;
    int loc;
    int idx;

    void reset() {
        idx = size;
        loc = 0;
    }
    template <typename T>
    T * alloc(int count = 1) {
        T * res = (T *)(base + loc);
        const int res_size = align_size(count * sizeof(T));
        memset(res, 0, res_size);
        loc += res_size;
        return res;
    }
    template <typename T>
    void pointer(T *& ptr) {
        idx -= sizeof(int);
        *(int *)(base + idx) = (int)((char *)&ptr - base);
    }
    void relocate(char * new_base) const {
        const ssize_t offset = new_base - base;
        int index = idx;
        while (index < size) {
            *(char **)(new_base + *(int *)(base + index)) += offset;
            index += sizeof(int);
        }
    }
};
