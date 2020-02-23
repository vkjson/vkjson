import vkjson

ctx = vkjson.context()

ctx.run([
    {
        'type': 'create_instance',
        'id': 'instance',
        'application': {
            'application_name': 'Application',
            'application_version': '1.0.0',
            'engine_name': 'Engine',
            'engine_version': '1.0.0',
            'api_version': '1.0.0',
        },
        'layers': [
            'VK_LAYER_KHRONOS_validation'
        ],
        'extensions': [
            'VK_EXT_debug_utils',
        ]
    },
    {
        'type': 'select_instance',
        'instance': 'instance',
    }
])

ctx.select(physical_device=ctx.physical_devices()[0])
print(ctx.scope())
