from setuptools import Extension, setup

ext = Extension(
    name='vkjson',
    sources=[
        './src/commands.cpp',
        './src/run_scope.cpp',
        './src/vkjson.cpp',
    ],
    depends=[
        '.src/allocator.hpp',
        '.src/cache_command.hpp',
        '.src/cache_scope.hpp',
        '.src/commands.hpp',
        '.src/common.hpp',
        '.src/run_scope.hpp',
        '.src/symbols.hpp',
        '.src/variable.hpp',
        './include/vulkan/vk_icd.h',
        './include/vulkan/vk_layer.h',
        './include/vulkan/vk_platform.h',
        './include/vulkan/vk_sdk_platform.h',
        './include/vulkan/vulkan_android.h',
        './include/vulkan/vulkan_core.h',
        './include/vulkan/vulkan_fuchsia.h',
        './include/vulkan/vulkan_ggp.h',
        './include/vulkan/vulkan_ios.h',
        './include/vulkan/vulkan_macos.h',
        './include/vulkan/vulkan_metal.h',
        './include/vulkan/vulkan_vi.h',
        './include/vulkan/vulkan_wayland.h',
        './include/vulkan/vulkan_win32.h',
        './include/vulkan/vulkan_xcb.h',
        './include/vulkan/vulkan_xlib_xrandr.h',
        './include/vulkan/vulkan_xlib.h',
        './include/vulkan/vulkan.h',
    ],
    include_dirs=['include'],
    extra_compile_args=['-fpermissive'],
)

setup(
    name='vkjson',
    version='0.1.0',
    ext_modules=[ext],
    py_modules=['_vkjson'],
)
