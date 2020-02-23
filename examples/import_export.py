import vkjson

ctx = vkjson.context()

cache = ctx.cache([
    {
        'type': 'import',
        'ids': ['a', 'b', 'c', 'd'],
    },
    {
        'type': 'export',
        'ids': ['b', 'c'],
    },
])

cache.load({'a': 1, 'c': 2, 'd': 3}).run()
print(cache.export())

cache.load({'b': 10, 'c': 20}).run()
print(cache.export())
print(cache.export())
