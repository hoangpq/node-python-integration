const log = require('util').log;
const pyVM = require('./build/Release/pyvm').vm;

// init vm
const sys = pyVM().import('sys');
const __builtin__ = pyVM().import('__builtin__');

// call function
log(`Result of sys.getrecursionlimit() is ${sys.call('getrecursionlimit')}`);
log(`Result of sys.getrefcount(0) is ${sys.call('getrefcount', 0)}`);
log(`Result pow(2, 3) is ${__builtin__.call('pow', 2, 3)}`);
