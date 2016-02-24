var clc = require('cli-color');

console.log(clc.green('Testing addons'));

// node_test.js
const addon = require('./build/Release/python');

var obj = new addon.MyObject(10);

console.log(obj.import('sys'));