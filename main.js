const log = require('util').log;
const pyVM = require('./build/Release/main').vm();

// import python sys module
const sys = pyVM.import('sys');
// call function
log('Default python recursion limit', sys.call('getrecursionlimit'));
