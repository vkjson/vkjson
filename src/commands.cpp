#include "commands.hpp"

void * load_noop(CacheScope * scope, PyObject * obj) {
    return NULL;
}

void run_noop(RunScope * scope, void * arg) {
}

struct ImportArg * load_import(CacheScope * scope, PyObject * obj);
void run_import(RunScope * scope, struct ImportArg * arg);

struct ExportArg * load_export(CacheScope * scope, PyObject * obj);
#define run_export run_noop

struct SelectInstanceArg * load_select_instance(CacheScope * scope, PyObject * obj);
void run_select_instance(RunScope * scope, struct SelectInstanceArg * arg);

struct CreateInstanceArg * load_create_instance(CacheScope * scope, PyObject * obj);
void run_create_instance(RunScope * scope, struct CreateInstanceArg * arg);

LoadProc load_proc[] = {
    (LoadProc)load_import,
    (LoadProc)load_export,
    (LoadProc)load_select_instance,
    (LoadProc)load_create_instance,
};

RunProc run_proc[] = {
    (RunProc)run_import,
    (RunProc)run_export,
    (RunProc)run_select_instance,
    (RunProc)run_create_instance,
};

PyObject * command_codes;

void build_command_codes() {
    command_codes = PyDict_New();
    PyDict_SetItemString(command_codes, "import", PyLong_FromLong(COMMAND_IMPORT));
    PyDict_SetItemString(command_codes, "export", PyLong_FromLong(COMMAND_EXPORT));
    PyDict_SetItemString(command_codes, "select_instance", PyLong_FromLong(COMMAND_SELECT_INSTANCE));
    PyDict_SetItemString(command_codes, "create_instance", PyLong_FromLong(COMMAND_CREATE_INSTANCE));
}
