# Half-checking propagators code

This is the experiment code for the Half-checking propagators paper.

## Building

The implementation is done in C++17, and uses CMake for building. To build locally, first install 
Gecode 6.2.0, and then crate a new directory `build`. From within `build`, use 
```
$ cmake ..
$ make
``` 
to build the application.

The build is only tested on Mac, with some support for building on Linux.

## Structure

There are three main folders of code, with the following intentions

* *src/propagators/*: Code for propagators is here. Posting functions
  are in the `propagators/propagators.h` and  `.cpp` files with the propagator
  and post function implementation in separate files.
* *src/models/*: This is the collection of models. 
* *src/programs/*: Files contanig main-functions. May use one
  or more models from *models/*.
* *src/extern/* are external dependecies
* *src/utilities/* are utility code, such as TSP readers, spatial indexes,
  disjoint sets, and so on.
* *test/* contains unit tests
* *data/* contains a copy of the relevant TSPLIB data
