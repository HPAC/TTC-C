/**
 * @file test-util.h
 * @brief The utilities used for testing TTC C API code.
 *
 * @details The data structures, functions and macros defined here are only for
 * testing the C API of TTC.
 *
 */

#pragma once


#ifdef __CPLUSPLUS
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "tensor_util.h"


#define TEST_GEN_BUF_SIZE   1024


void set_scope(const char *name);


char common_prefix[TEST_GEN_BUF_SIZE];


#define TEST_INFO_OUTPUT(str) fprintf(stdout, "[TEST-INFO] %s: %s\n", \
        common_prefix, str)

#define TEST_SUCC_OUTPUT(str) fprintf(stdout, "%s[TEST-SUCC] %s: %s%s\n", \
        GRN, common_prefix, str, RESET)

#define TEST_WARN_OUTPUT(str) fprintf(stderr, "%s[TEST-WARN] %s: %s%s\n", \
        YEL, common_prefix, str, RESET)

#define TEST_ERR_OUTPUT(str) fprintf(stderr, "%s[TEST-ERROR] %s: %s%s\n", \
        RED, common_prefix, str, RESET)


void set_scope(const char *name) {
    strcpy(common_prefix, name);
}

#ifdef __CPLUSPLUS
}
#endif

