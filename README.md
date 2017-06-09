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

```bash
$ node-gyp configure
$ node-gyp build
$ node ./main.js
```

`Tested on my OSX and Ubuntu`

:sparkles::sparkles::sparkles::sparkles::sparkles::sparkles:
