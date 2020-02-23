import vkjson

ctx = vkjson.context()

ctx.select({
    'instance': 1000,
    'physical_device': 2000,
    'device': 3000,
    'queue': 4000,
})

cache = ctx.cache([
    {'type': 'import'},
    {'type': 'create_instance'},
    {'type': 'export'},
])

cache.run()

print(cache.scope())
print(cache.export())
