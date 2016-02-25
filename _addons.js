const addon = require('./build/Release/python');

var obj = new addon.MyObject(10);
// import python sys module
var sys = obj.import('sys');
// call function
console.log(sys.call('getrecursionlimit'));
console.log(sys.call('version'));