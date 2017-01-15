# A lightweight, cross-platform Modbus library
[![The GPL license](https://img.shields.io/badge/license-GPL-blue.svg?style=flat-square)](http://opensource.org/licenses/GPL-3.0)
[![Travis CI](https://img.shields.io/travis/Jacajack/liblightmodbus.svg?style=flat-square)](https://travis-ci.org/Jacajack/liblightmodbus)
[![Coveralls](https://img.shields.io/coveralls/Jacajack/liblightmodbus.svg?style=flat-square)](https://coveralls.io/github/Jacajack/liblightmodbus)

[More build results...](https://github.com/Jacajack/liblightmodbus/wiki/Build-results-history)
<br>[Library on launchpad...](https://launchpad.net/liblightmodbus)

`liblightmodbus` is a very lightweight Modbus RTU library that can run on every little-endian machine. It is developed with thought about embedded targets and personal computers as well.

## Features
- Minimal resources usage
- Lightweight and easy to use
- Supports all basic Modbus functions
- Library can be installed as a `*.deb` package on computer
- You can pick only modules, you want, when building library

*Currently supported functions include: 01, 02, 03, 04, 05, 06, 15, 16*
Check [wiki](https://github.com/Jacajack/liblightmodbus/wiki) and [docs](https://github.com/Jacajack/liblightmodbus/tree/master/doc) for more technical information.

If you need help - [email me](mailto:mrjjot@gmail.com). If you want to help - contribute here, on Github. **All contributions are welcome!**

## PPA
`liblightmodbus` can be obtained from [PPA](https://code.launchpad.net/~mrjjot/+archive/ubuntu/liblightmodbus) (Personal Package Archive).

This is how to install `liblightmodbus` (if you run Ubuntu):
 - Add PPA to your system -  `sudo add-apt-repository ppa:mrjjot/liblightmodbus`
 - Update software lists - `sudo apt-get update`
 - Install development package - `sudo apt-get install liblightmodbus-dev`
