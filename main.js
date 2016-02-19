var clc = require('cli-color');

// testing iterator
console.log(clc.green('Iterator testing'));
require('./test/_iter');

// testing async
console.log(clc.green('Async testing'));
require('./test/_async');

// testing stream
console.log(clc.green('Node stream testing'));
require('./test/_stream');

// addon test
console.log(clc.green('Testing addons'));
const addon = require('./build/Release/hello');
console.log(`Returns: ${addon.getUndefined({lat: 100, lng: 200, txt: "Hello, world"})}`);

// event testing
// console.log(clc.green(`Event emitter`));
// require('./test/_event');