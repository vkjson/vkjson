#pragma once
#include "common.hpp"
#include "cache_scope.hpp"
#include "run_scope.hpp"

struct Command {
    int command_code;
    int location;
};

enum CommandCodes {
    COMMAND_IMPORT,
    COMMAND_EXPORT,
    COMMAND_CREATE_INSTANCE,
};

typedef void * (* LoadProc)(CacheScope * scope, PyObject * obj);
typedef void (* RunProc)(RunScope * scope, void * arg);

extern LoadProc load_proc[];
extern RunProc run_proc[];

extern PyObject * command_codes;

void build_command_codes();
