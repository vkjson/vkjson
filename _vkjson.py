def parse_version(ver):
    major, minor, patch = map(int, (ver + '.0.0').split('.')[:3])
    return major << 22 | minor << 12 | patch

enums = {}
