from functools import partial


def f(n):
    return 1 if n in [0, 1] else n * f(n - 1)


def fact(f1, f2):
    return [f1, f2]


def f3():
    return partial(f, 5)()


def f4(d):
    return d.get('z', {'error': 'Can\'t get this object'})