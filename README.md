# Half-checking propagators

This repository contains the code for the paper "Half-checking
propagators" by Mikael Zayenz Lagerkvist and Magnus Rattfeldt.  The
paper is [available on aXiv](https://arxiv.org/abs/2007.05423) and there is a 
[blog post summarizing and with some context](https://zayenz.se/blog/post/half-checking-propagators-2020-arxiv-paper/).

The code is located in the directory `code/` and is a CMake C++ 17
project. The paper as submitted to arXiv is located in `paper/`.


## Licenses

* The code is licensed under the MIT license.
* The paper is licensed under CC-BY-SA-4.0.

Relevant license texts are available in the subdirectories.


## Building and running the experiments

[Gecode 6.2.0](https://www.gecode.org/ ) is required to be
installed on the system before building. The code has only been tested
on a Mac, with some support for building on Linux.

First, create a new build directory `code/build`. From within `build`, use 
```
$ cmake ..
$ make
``` 
to build the application.
