# Running Python VM in NodeJS

## Installation

Install `node-gyp` with `npm`

```bash
$ npm install -g node-gyp
```

You will also need to install:

  * On Unix(Ubuntu):
    * `sudo apt-get install python-dev` :zap:
  * On Mac OS X:
    * `brew install python` :zap:

## Examples

How to run

```bash
$ node-gyp configure
$ node-gyp build
$ node ./main.js
```
or
```bash
npm run build && npm test
```

```
9 Jun 16:06:02 - Result of sys.getrecursionlimit() is 1000
9 Jun 16:06:02 - Result of sys.getrefcount(0) is 1
9 Jun 16:06:02 - Result pow(2, 3) is 8
```


`Tested on my OSX and Ubuntu`

:sparkles::sparkles::sparkles::sparkles::sparkles::sparkles:
