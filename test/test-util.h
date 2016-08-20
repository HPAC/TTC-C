/**
 * @file test-util.h
 * @brief The utilities used for testing TTC C API code.
 *
 * @details The data structures, functions and macros defined here are only for
 * testing the C API of TTC.
 *
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ttc_c.h"
#include "tensor_util.h"

#define TEST_GEN_BUF_SIZE   1024

#define TENSOR_DIM      3
#define TENSOR_SIZE_0   256
#define TENSOR_SIZE_1   128
#define TENSOR_SIZE_2   64
#define TENSOR_SIZE     TENSOR_SIZE_0 * TENSOR_SIZE_1 * TENSOR_SIZE_2
#define LDA_0           256
#define LDA_1           128
#define LDA_2           64
#define LDB_0           256
#define LDB_1           64
#define LDB_2           128
#define PERM_0          0
#define PERM_1          2
#define PERM_2          1
#define ALPHA           1.0
#define BETA            1.0



char common_prefix[TEST_GEN_BUF_SIZE];


#ifdef __CPLUSPLUS
extern "C" {
#endif


void set_scope(const char *name);


/**
 * @brief Plan reuse test.
 *
 * @return The status, 0 if succeed, non-zero if failed.
 */
int32_t
reuse_test(
        );


/**
 * @brief Data type test.
 *
 * @param[in] type Tensor data type.
 *
 * @return The status, 0 if succeed, non-zero if failed.
 */
int32_t
datatype_test(
        ttc_datatype_e type
        );


/**
 * @brief Architecture + compiler test.
 *
 * @param[in] arch      Architecture to be tested.
 * @param[in] compiler  Compiler to be tested.
 *
 * @return The status, 0 if succeed, non-zero if failed.
 */
int32_t
arch_compiler_test(
        ttc_arch_e arch,
        ttc_compiler_e compiler
        );


/**
 * @brief CUDA interface test.
 *
 * @param[in] type  Tensor data type to be tested.
 * @param[in] blk   Thread block to be tested.
 * @param[in] lda   LDA
 * @param[in] ldb   LDB
 *
 * @return The status, 0 if succeed, non-zero if failed.
 */
int32_t
cuda_test(
        ttc_datatype_e type,
        ttc_thread_blk_e blk,
        int32_t *lda,
        int32_t *ldb
        );


/**
 * @brief No beta test.
 *
 * @param[in] type      Tensor data type to be tested.
 * @param[in] arch      Architecture to be tested.
 * @param[in] compiler  Compiler to be tested.
 *
 * @return The status, 0 if succeed, non-zero if failed.
 */
int32_t
no_beta_test(
        ttc_datatype_e type,
        ttc_arch_e arch,
        ttc_compiler_e compiler
        );


/**
 * @brief No transpose test
 *
 * @return The status, 0 if succeed, non-zero if failed.
 */
int32_t
no_transpose(
        );



#define TEST_INFO_OUTPUT(str) fprintf(stdout, "[TEST-INFO] %s: %s\n", \
        common_prefix, str)

#define TEST_SUCC_OUTPUT(str) fprintf(stdout, "%s[TEST-SUCC] %s: %s%s\n", \
        GRN, common_prefix, str, RESET)

#define TEST_WARN_OUTPUT(str) fprintf(stderr, "%s[TEST-WARN] %s: %s%s\n", \
        YEL, common_prefix, str, RESET)

#define TEST_ERR_OUTPUT(str) fprintf(stderr, "%s[TEST-ERROR] %s: %s%s\n", \
        RED, common_prefix, str, RESET)


#ifdef __CPLUSPLUS
}
#endif

