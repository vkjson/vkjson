import vkjson

ctx = vkjson.context()

ctx.select({
    'instance': 1000,
    'physical_device': 2000,
    'device': 3000,
    'queue': 4000,
})

cache = ctx.cache([
    {'type': 'import', 'ids': ['x']},
    {'type': 'create_instance'},
    {'type': 'export', 'ids': ['x']},
])

cache.run()

print(cache.scope())
print(cache.export())
