# TTC C API

## Introduction

This is a C wrapper API for [TTC](https://github.com/HPAC/TTC).

## Dependencies

`cmake 2.8+`

[TTC](https://github.com/HPAC/TTC)

To build the document, `doxygen` is also needed.

## Build

Under the project directory, execute:

```shell
mkdir build; cd build
```

Currently, three customized options are used in CMake scripts. They are:

1. Building the test/demo program: `BUILD_TEST=[ON|OFF]`, default: `OFF`.

2. Building the documents: `BUILD_DOC=[ON|OFF]`, default: `OFF`.

3. Choosing building type: `BUILD_TYPE=[RELEASE|DEBUG]`, default `RELEASE`.

E.g.

```shell
cmake -D BUILD_TEST=ON -D BUILD_DOC=ON ..
```

Then, execute `make` to build the library, and `make doc` to build the document.
