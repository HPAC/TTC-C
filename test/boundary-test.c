/**
 * @file boundary-test.c
 *
 * @brief Test example for TTC C API.
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "test-util.h"
#include "ttc_c.h"

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



/**
 * @brief No transpose test
 *
 * @return The status, 0 if succeed, non-zero if failed.
 */
int32_t
no_transpose(
        );


int32_t main() {
    uint32_t total_num = 0, error_num = 0;

    set_scope("No transpose test");
    ++total_num;
    if (0 != no_transpose()) {
        TEST_ERR_OUTPUT("Test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Test succeed.");
    }

    printf("%sTest finished. TOTAL: %d, SUCCEED: %d, FAILED: %d%s\n",
            CYN, total_num, total_num - error_num, error_num, RESET);

    return 0;
}

int32_t
no_transpose(
        ) {
    TEST_INFO_OUTPUT("Create TTC handler.");
    ttc_handler_s *handler = ttc_init();
    if (NULL == handler) {
        TEST_ERR_OUTPUT("Cannot create handler.");
        return -1;
    }

    TEST_INFO_OUTPUT("Set maximum implementation.");
    int32_t max_impl = 100;
    if (0 != ttc_set_opt(handler, TTC_OPT_MAX_IMPL, &max_impl, 0)) {
        TEST_ERR_OUTPUT("Cannot set maximum implementation.");
        return -1;
    }

    TEST_INFO_OUTPUT("Set thread number.");
    int32_t num_threads = 24;
    if (0 != ttc_set_opt(handler, TTC_OPT_NUM_THREADS, &num_threads, 0)) {
        TEST_ERR_OUTPUT("Cannot set thread number.");
        return -1;
    }

    TEST_INFO_OUTPUT("Set param.");
    ttc_param_s param = {
        .loop_perm = NULL,
        .dim = 4
    };
    uint32_t perm[4] = { 0, 1, 2, 3 };
    uint32_t size[4] = { 128, 64, 32, 16 };
    param.perm = perm;
    param.size = size;

    param.alpha.s = 4;
    param.beta.s = 1.0;

    void *input_tensor = (void *)malloc(sizeof(float) * 128 * 64 * 32 * 16);
    if (NULL == input_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for input_tensor.");
        return -1;
    }
    void *result_tensor = (void *)malloc(sizeof(float) * 128 * 64 * 32 * 16);
    if (NULL == result_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for result_tensor.");
        free(input_tensor);
        return -1;
    }


    TEST_INFO_OUTPUT("Execute transpose.");
    int32_t ret = ttc_transpose(handler, &param,
            (void *)input_tensor, (void *)result_tensor);
    if (0 != ret) {
        TEST_ERR_OUTPUT("Transpose failed.");
    }

    TEST_INFO_OUTPUT("Release handler.");
    ttc_release(handler);

    return 0;
}
