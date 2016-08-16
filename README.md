# TTC C API #

This is a C wrapper API for [TTC](https://github.com/HPAC/TTC).

# Dependencies
--------------

* `cmake 2.8+`
* [TTC](https://github.com/HPAC/TTC)
* python

To build the document, `doxygen` is also needed.

# Build
--------------

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

# Getting started
--------------

Here is a short (incomplete) example which illustrates the usage of this API on
the example of perm=2,1,0 for tensors of size 100x200x300:

    // include header
    #include <ttc_c.h>

    // Create handler
    ttc_handler_s *handler = ttc_init();

    // Create transpose parameter
    ttc_param_s param = { .alpha.s = 1.0, .beta.s = 1.0, .lda = NULL, .ldb = NULL, .perm = NULL, .size = NULL, .loop_perm = NULL, .dim = 4};
    uint32_t perm[] = { 2, 1, 0};
    uint32_t size[] = { 100, 200, 300};
    param.perm = perm;
    param.size = size;

    // Set TTC options (THIS IS OPTIONAL)
    int maxImplemenations = 100;
    ttc_set_opt( ttc_handle, TTC_OPT_MAX_IMPL, (void*)&maxImplemenations, 1 );
    int numThreads = 24;
    ttc_set_opt( ttc_handle, TTC_OPT_NUM_THREADS, (void*)&numThreads, 1 );
    char affinity[] = "compact,1";
    ttc_set_opt( ttc_handle, TTC_OPT_AFFINITY, (void*)affinity, strlen(affinity) );

    // Allocating memory for tensors
    void *A = (void *)malloc(sizeof(float) * 100*200*300);
    void *B = (void *)malloc(sizeof(float) * 100*200*300);

    // Execute transpose
    ttc_transpose(handler, &param, A, B);

For further examples please have a look at the provided [tests](https://github.com/HPAC/TTC-C/blob/master/test/jit-test.c).
