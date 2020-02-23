import vkjson

ctx = vkjson.context()

ctx.select({
    'instance': 1000,
    'physical_device': 2000,
    'device': 3000,
    'queue': 4000,
})

ctx.run([
    {'type': 'import', 'ids': ['x']},
    {'type': 'create_instance'},
    {'type': 'export', 'ids': []},
])

print(ctx.scope())
