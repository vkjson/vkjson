import vkjson

ctx = vkjson.context()

ctx.select({
    'instance': 1000,
    'physical_device': 2000,
    'device': 3000,
    'queue': 4000,
})

cache = ctx.cache([
    {'type': 'create_device_memory'},
    {'type': 'create_swapchain'},
    {'type': 'create_buffer'},
])

cache.run()

print(cache.scope())
print(cache.export())
