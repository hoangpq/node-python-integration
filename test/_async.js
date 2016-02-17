var async = require('async');
var clc = require('cli-color');

var fs = require('fs');
var path = require('path');

var _ = require('lodash');

var rootPath = process.env.PWD;



var data = [1, 2, 3]

// async with each method

async.each(data,
    function(item, callback) {
        if (item > 2) {
            callback(`Can\'t fetch number great than 2`);
        }
    }, function(err) {
        if (err) {
            console.log(clc.red(`Async each failed`));
        } else {
            console.log(clc.green(`Async each passed`));
        }
    });

// async forEachOf to read files

var obj = {dev: 'raw/dev.json', test: 'raw/test.json', prod: 'raw/prod.json'};

async.forEachOf(obj,
    function(val, key, callback) {
        fs.readFile(`${rootPath}/${val}`, 'utf8', function(err, data) {
            if (err) {
                callback(`${val}`);
            }
        });
    }, function(err) {
        if (err) {
            console.log(clc.red(`Async forEachOf failed: Error when try to read file ${err}`));
        } else {
            console.log(clc.green(`Async forEachOf passed`));
        }
    });

var files = ['raw/dev.json', 'raw/test.json'];

async.map(files,
    fs.stat,
    function(err, res) {
        if (err) {
            console.log(clc.red(`Async map method test failed`))
        } else {
            var sizes = _.reduce(res, function (s, f) {
                return s + f.size;
            }, 0);
            console.log(clc.green(`Passed, Total checked files size: ${sizes} byte(s)`));
        }
    });

//Load the request module
var request = require('request');

var items = [];
// async filter
var rq =
    _.chain(100)
        .range()
        .map(function () {
            return function (cb) {
                request({
                    url: 'http://localhost:3000/rand',
                    method: 'GET', //Specify the method
                    dataType: 'json'
                }, function (error, response, body) {
                    if (error) {
                        console.log(error);
                    } else {
                        var obj = JSON.parse(body);
                        cb(null, obj.num);
                    }
                });
            }
        }).value();

async.waterfall([
    function (cb) {
        async.map(rq,
            function (item, callback) {
                item(callback);
            }, function (err, res) {
                if (err) {
                    console.log(clc.red(`Error when transform data`));
                } else {
                    cb(null, res);
                }
            });
    }, function (res, cb) {
        var filtered = _.filter(res, function(num) {
            return num > 90;
        });
        cb(null, res, filtered);

    }
], function(err, origin, filtered) {
    if (err) {
        console.log(clc.red(`Error when process data`));
    } else {
        console.log(clc.green(`Origin: ${origin} \n\n\n\n\n Transformed & Filtered: ${filtered}`));
    }
});