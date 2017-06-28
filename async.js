// Javascript event handlers don't run until the thread is free

const EventEmitter = require('events');

class MyEmitter extends EventEmitter {}

const myEmitter = new MyEmitter();
myEmitter.on('event', (event) => {
  console.log(event);
});

setTimeout(function() {
  myEmitter.emit('event', 'setTimeout');
}, 10);

process.nextTick(function () {
  myEmitter.emit('event', 'process.nextTick')
});

console.time('event');
for (let i = 0; i < 1e9; i++) {
  // do something
}
console.timeEnd('event');

console.log('WTF');

let start = new Date();

// timer
setTimeout(function() {
  let end = new Date();
  console.log(`First time elapsed: ${end - start} ms`)
}, 100);

// timer
setTimeout(function() {
  let end = new Date();
  console.log(`Second time elapsed: ${end - start} ms`)
}, 500);

while (new Date() - start < 1000) {}

/*
In my multithreaded mind-set, I'd expected only 500ms to go by
before the timed function ran. But that would have required the loop.

So, if setTimeout isn't using another thread, then what is it doing?

Meet the Queue
When we call setTimeout, a timeout event is queued. Then execution
continues the line after the setTimeout all runs, and then the line
after that, and so on, until there are no lines left. Only then
does the Javascript VM ask, "What's on the queue?"

Input events work the same way, when a user click a DOM element with
a click handler attached, a click is queued. But the handler won't be
executed until all currently running code has finished.

Node.js wasn't created so that people could run Javascript on the server.
It was created because Ryan Dahl wanted an event-driven server fw build
on high-level language. Javascript just happened to be the right language
for the job.

runYourScript();
while (atLeastOneEventIsQueued) {
  fireNextQueuedEvent();
}

*/
