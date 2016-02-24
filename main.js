var clc = require('cli-color');

// Iterator test
// console.log(clc.green('Iterator testing'));
// require('./test/_iter');

// Async test
// console.log(clc.green('Async testing'));
// require('./test/_async');

// Streaming test
// console.log(clc.green('Node stream testing'));
// require('./test/_stream');

// Addons test
console.log(clc.green('Testing addons'));
const addon = require('./build/Release/python');

var t = new Date();
var sorted = addon.execute('embed.node', 'f', 4);
console.log(clc.green(`Get factor from python recursive function:`), clc.red(`${sorted}`));
var e = new Date() - t;
console.log('Times', e, 'ms');

var factor = addon.execute('embed.node', 'fact', 10.2, 10.5);
console.log(clc.green(`List float test: `), clc.red(factor));

var buildin_test = addon.execute('embed.node', 'f3');
console.log(clc.green(`Build-in function test: `), clc.red(buildin_test));

var obj_py = addon.execute('embed.node', 'f4', {a: 1, z: {a: {b: {c: 2}}}});
console.log(clc.green(`Get Object`), clc.red(JSON.stringify(obj_py)));