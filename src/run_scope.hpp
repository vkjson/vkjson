#pragma once
#include "common.hpp"

struct RunScope {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue queue;
    VkCommandBuffer command_buffer;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    char * base;
};

int load_scope(RunScope * scope, PyObject * args, PyObject * kwargs);
PyObject * dump_scope(RunScope * scope);
