__author__ = 'Hoang Phan'

import os

# import from local path

from underscore import _


def t():
    return map(lambda x: x * x, [1, 2, 3])


def s():
    data = [1, 2, 3, 4, 5, 6]
    try:
        __path = os.path.dirname(os.path.realpath(__file__))
        with open(__path + '\input.txt', 'r') as f:
            print f.readline()  # Hello from python
    except Exception, err:
        print err
    return _(data) \
        .chain() \
        .filter(lambda x, *a: x > 2) \
        .map(lambda x, *a: x * 2) \
        .sortBy() \
        .value()


def f():
    data = [1, 2, 3]
    return reduce(lambda x, y: x + y, data)


def fff(m, n):
    return m
    # return 1 if num == 0 or num == 1 else factor_fn(num - 1) * num
