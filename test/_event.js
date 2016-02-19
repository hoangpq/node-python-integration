const EventEmitter = require('events');
const util = require('util');

function MyEmitter() {
    EventEmitter.call(this);
}
util.inherits(MyEmitter, EventEmitter);

const myEmitter = new MyEmitter();
myEmitter.on('event', (mes) => {
    setTimeout(function() {
        console.log(mes)
    }, Math.floor(Math.random()*10000));
});

for (var i = 0; i < 20; i++) {
    myEmitter.emit(`event`, `Execute ${i}`);
}
