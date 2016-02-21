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
var sorted = addon.execute('embed.test', 's');
console.log(clc.green(`Python result:`), clc.red(`${sorted}`));
var e = new Date() - t;
console.log('Times', e, 'ms');

var number = addon.execute('embed.test', 'f');
console.log(clc.green(`Python Number value:`), clc.red(number));

var factor = addon.execute('embed.test', 'fff', 1, 2);
console.log(clc.green(`Factor number: `), clc.red(factor));