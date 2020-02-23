#include "commands.hpp"
#include "parse.hpp"

struct CreateInstanceArg {
    VkInstanceCreateInfo create_info;
    Variable ref;
};

CreateInstanceArg * load_create_instance(CacheScope * scope, PyObject * obj) {
    CreateInstanceArg * arg = scope->data.alloc<CreateInstanceArg>();
    VkInstanceCreateInfo & info = arg->create_info;

	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = NULL;
	info.flags = get_flags(scope, obj, "flags");

	if (PyObject * application = PyDict_GetItemString(obj, "application")) {
        if (!PyDict_Check(application)) {
        }

		VkApplicationInfo * app_info = scope->data.alloc<VkApplicationInfo>();
		info.pApplicationInfo = app_info;
        scope->data.pointer(info.pApplicationInfo);

        app_info->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info->pNext = NULL;
        app_info->pApplicationName = get_string(scope, application, "application_name");
        scope->data.pointer(app_info->pApplicationName);
        app_info->applicationVersion = get_version(scope, application, "application_version");
        app_info->pEngineName = get_string(scope, application, "engine_name");
        scope->data.pointer(app_info->pEngineName);
        app_info->engineVersion = get_version(scope, application, "engine_version");
        app_info->apiVersion = get_version(scope, application, "api_version");
	}

    info.enabledLayerCount = get_array_size(scope, obj, "layers");
    info.ppEnabledLayerNames = get_string_array(scope, obj, "layers");
    scope->data.pointer(info.ppEnabledLayerNames);
    info.enabledExtensionCount = get_array_size(scope, obj, "extensions");
    info.ppEnabledExtensionNames = get_string_array(scope, obj, "extensions");
    scope->data.pointer(info.ppEnabledExtensionNames);

    new_variable(scope, arg->ref, obj, "id");
    return arg;
}

void run_create_instance(RunScope * scope, CreateInstanceArg * arg) {
    VkInstance instance = NULL;
    if (vkCreateInstance(&arg->create_info, NULL, &instance)) {
        // TODO: error without python api
    }
    arg->ref.set(instance);
}
