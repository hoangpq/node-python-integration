const log = require('util').log;
const pyvm = require('./build/Release/pyvm');

const __builtin__ = pyvm.import('__builtin__');
const sys = pyvm.import('sys');

// call function
log(`Result of pow(2, 3) is ${__builtin__.pow(2, 3)}`);
log(`Result of sys.getrecursionlimit() is ${sys.getrecursionlimit()}`);
log(`Result of sys.getrefcount(0) is ${sys.getrefcount(0)}`);
