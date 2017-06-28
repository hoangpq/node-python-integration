const fs = require('fs');

/*
 [Timer]
 [I/O callbacks]
 [idle, prepare]
 [poll] <- [incoming: connections, data, etc.]
 [check]
 [close callbacks]
 */

/*
 The main advantage to using setImmediate() over setTimeout()
 is setImmediate() will always be executed before any timers if
 scheduled within an I/O cycle, independently of how many timers
 are present.
 */

function readFileWithDelay(time) {
  setTimeout(() => {
    fs.readFile(__filename, (err, data) => {
      // console.log(data.toString());

      setImmediate(() => {
        console.log(`Immediate within I/O`);
      });

      setTimeout(() => {
        console.log(`Timeout within I/O`);
      }, 0);

    });
  }, time);
}

setImmediate(function () {
  console.log(`Immediate`);
}); // fire immediate

setTimeout(function () {
  console.log(`Timeout`);
}, 10); // fire after 2s

readFileWithDelay(1000);

// delay 2s
let start = new Date();
while (new Date() - start < 2000) {
}
console.log('Release main thread after 2s');

/*
The user defines `someAsyncApiCall()` to have an asynchronous
signature, but it actually operates synchronously. When it is called,
the callback provided to `someAsyncApiCall()` is called in the same
phase of the event loop because `someAsyncApiCll()` doesn't
actually do anything asynchronously.
 */

function someAsyncApiCall(callback) {
  process.nextTick(callback);
}

someAsyncApiCall(() => {
  console.log('bar', bar);
});

var bar = 1;

/*
The script still has the ability to run to completion,
allowing all the variables, functions, etc.., to be initialized
prior to the callback being called. It also has the advantage of
not allowing the event loop to continue. It may be useful for the
user to be alerted to an error before the event loop is allowed to
continue.
 */