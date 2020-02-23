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
    {'type': 'export', 'ids': ['x']},
])

thread = vkjson.thread()

thread.run(cache)
thread.wait()

print(cache.scope())
print(cache.export())
