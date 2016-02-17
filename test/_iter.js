// use generator
function* channel() {
    yield 10;
    yield 20;
    yield 30;
}

var iter = channel();

for (var i of iter) {
    console.log(i);
}

// fetch the second times
for (var i of iter) {
    console.log(i);
}