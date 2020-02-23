import vkjson

ctx = vkjson.context()

ctx.select({
    'instance': 1000,
    'physical_device': 2000,
    'device': 3000,
    'queue': 4000,
})

ctx.run([
    {'type': 'create_device_memory'},
    {'type': 'create_swapchain'},
    {'type': 'create_buffer'},
])

print(ctx.scope())
