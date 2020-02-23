import vkjson

ctx = vkjson.context()

ctx.select({
    'instance': 1000,
    'physical_device': 2000,
    'device': 3000,
    'queue': 4000,
})

cache = ctx.cache([
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
        'type': 'export',
        'ids': ['instance'],
    }
])

cache.run()

print(cache.scope())
print(cache.export())
