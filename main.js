const log = require('util').log;
const pyvm = require('./build/Release/pyvm');

const __builtin__ = pyvm.import('__builtin__');
const sys = pyvm.import('sys');

// call function
log(`Result pow(2, 3) is ${__builtin__.call('pow', 2, 3)}`);
log(`Result of sys.getrecursionlimit() is ${sys.call('getrecursionlimit')}`);
log(`Result of sys.getrefcount(0) is ${sys.call('getrefcount', 0)}`);

console.log(__builtin__.list());
