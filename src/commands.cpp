#include "commands.hpp"

void * load_noop(CacheScope * scope, PyObject * obj) {
    return NULL;
}

void run_noop(RunScope * scope, void * arg) {
}

LoadProc load_proc[] = {
    (LoadProc)load_noop,
    (LoadProc)load_noop,
    (LoadProc)load_noop,
};

RunProc run_proc[] = {
    (RunProc)run_noop,
    (RunProc)run_noop,
    (RunProc)run_noop,
};

PyObject * command_codes;

void build_command_codes() {
    command_codes = PyDict_New();
    PyDict_SetItemString(command_codes, "import", PyLong_FromLong(COMMAND_IMPORT));
    PyDict_SetItemString(command_codes, "export", PyLong_FromLong(COMMAND_EXPORT));
    PyDict_SetItemString(command_codes, "create_instance", PyLong_FromLong(COMMAND_CREATE_INSTANCE));
}
