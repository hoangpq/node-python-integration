// streaming

var clc = require('cli-color');
var fs = require('fs');
var zlib = require('zlib');

// use gzip to compress files

var data = '';

var reader = fs.createReadStream('E:\\big.txt');
var writer = fs.createWriteStream('E:\\done.txt');

writer.on('finish', function () {
    console.log('Write completed.');
});

// pipe stream
reader
    .pipe(zlib.createGzip())
    .pipe(fs.createWriteStream('big.tar.gz'));


reader.on('error', function (err) {
    console.log(`Something went wrong: ${err}`);
}).on('data', function (chunk) {
    // data += chunk;
    writer.write(chunk);
}).on('end', function () {
    console.log('Done!');
    writer.end();
});

