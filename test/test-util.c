/**
 * @file jit-test.c
 *
 * @brief Test example for TTC C API.
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <complex.h>

#include "test-util.h"
#include "ttc_c.h"


void set_scope(const char *name) {
    strcpy(common_prefix, name);
}


int32_t
reuse_test(
        ) {
    // Create handler
    ttc_handler_s *handler = ttc_init();
    if (NULL == handler) {
        TEST_ERR_OUTPUT("Cannot create handler.");
        exit(EXIT_FAILURE);
    }

    // Create transpose parameter
    ttc_param_s param = ttc_default_param();
    param.alpha.s = ALPHA;
    param.beta.s = BETA;
    param.dim = TENSOR_DIM;

    uint32_t perm[TENSOR_DIM] = { PERM_0, PERM_1, PERM_2 };
    uint32_t size[TENSOR_DIM]
        = { TENSOR_SIZE_0, TENSOR_SIZE_1, TENSOR_SIZE_2 };
    param.perm = perm;
    param.size = size;

    // Allocating memory for tensors
    void *input_tensor = (void *)malloc(sizeof(float) * TENSOR_SIZE);
    if (NULL == input_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for input_tensor.");
        return -1;
    }
    void *result_tensor = (void *)malloc(sizeof(float) * TENSOR_SIZE);
    if (NULL == result_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for result_tensor.");
        free(input_tensor);
        return -1;
    }

    // Execute transpose
    if (0 != ttc_transpose(handler, &param, input_tensor, result_tensor)) {
        TEST_ERR_OUTPUT("1st transpose failed.");
        free(input_tensor);
        free(result_tensor);
        return -1;
    }

    if (0 != ttc_transpose(handler, &param, input_tensor, result_tensor)) {
        TEST_ERR_OUTPUT("2nd transpose failed.");
        free(input_tensor);
        free(result_tensor);
        return -1;
    }

    free(input_tensor);
    free(result_tensor);

    return 0;
}


int32_t
datatype_test(
        ttc_datatype_e type
        ) {
    ttc_handler_s *handler = ttc_init();
    if (NULL == handler) {
        TEST_ERR_OUTPUT("Cannot create handler.");
        return -1;
    }

    ttc_param_s param = ttc_default_param();
    param.datatype = type;
    param.dim = TENSOR_DIM;

    uint32_t perm[TENSOR_DIM] = { PERM_0, PERM_1, PERM_2 };
    uint32_t size[TENSOR_DIM]
        = { TENSOR_SIZE_0, TENSOR_SIZE_1, TENSOR_SIZE_2 };
    param.perm = perm;
    param.size = size;

    uint32_t input_type_size, result_type_size;

    if (TTC_TYPE_S == type || TTC_TYPE_DEFAULT == type) {
        param.alpha.s = ALPHA;
        param.beta.s = BETA;
        input_type_size = result_type_size = sizeof(float);
    }
    else if (TTC_TYPE_D == type) {
        param.alpha.d = ALPHA;
        param.beta.d = BETA;
        input_type_size = result_type_size = sizeof(double);
    }
    else if (TTC_TYPE_C == type) {
        param.alpha.s = ALPHA;
        param.beta.s = BETA;
        input_type_size = result_type_size = sizeof(float complex);
    }
    else if (TTC_TYPE_Z == type) {
        param.alpha.d = ALPHA;
        param.beta.d = BETA;
        input_type_size = result_type_size = sizeof(double complex);
    }
    else if (TTC_TYPE_SD == type) {
        param.alpha.s = ALPHA;
        param.beta.d = BETA;
        input_type_size = sizeof(float);
        result_type_size = sizeof(double);
    }
    else if (TTC_TYPE_DS == type) {
        param.alpha.d = ALPHA;
        param.beta.s = BETA;
        input_type_size = sizeof(double);
        result_type_size = sizeof(float);
    }
    else if (TTC_TYPE_CZ == type) {
        param.alpha.s = ALPHA;
        param.beta.d = BETA;
        input_type_size = sizeof(float complex);
        result_type_size = sizeof(double complex);
    }
    else if (TTC_TYPE_ZC == type) {
        param.alpha.d = ALPHA;
        param.beta.s = BETA;
        input_type_size = sizeof(double complex);
        result_type_size = sizeof(float complex);
    }
    else {
        TEST_ERR_OUTPUT("Unknown data type.");
        return -1;
    }

    void *input_tensor = (void *)malloc(input_type_size * TENSOR_SIZE);
    if (NULL == input_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for input_tensor.");
        return -1;
    }
    void *result_tensor = (void *)malloc(result_type_size * TENSOR_SIZE);
    if (NULL == result_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for result_tensor.");
        free(input_tensor);
        return -1;
    }

    int32_t ret = ttc_transpose(handler, &param, input_tensor, result_tensor);
    if (0 != ret) {
        TEST_ERR_OUTPUT("Transpose failed.");
    }

    free(input_tensor);
    free(result_tensor);

    return ret;
}


int32_t
arch_compiler_test(
        ttc_arch_e      arch,
        ttc_compiler_e  compiler
        ) {
    ttc_handler_s *handler = ttc_init();
    if (NULL == handler) {
        TEST_ERR_OUTPUT("Cannot create handler.");
        return -1;
    }

    if (0 != ttc_set_opt(handler, TTC_OPT_ARCH, &arch, 0)) {
        TEST_ERR_OUTPUT("Cannot set architecture.");
        return -1;
    }

    if (0 != ttc_set_opt(handler, TTC_OPT_COMPILER, &compiler, 0)) {
        TEST_ERR_OUTPUT("Cannot set compiler.");
        return -1;
    }

    ttc_param_s param = ttc_default_param();
    param.datatype = TTC_TYPE_ZC;
    param.dim = TENSOR_DIM;

    uint32_t perm[TENSOR_DIM] = { PERM_0, PERM_1, PERM_2 };
    uint32_t size[TENSOR_DIM]
        = { TENSOR_SIZE_0, TENSOR_SIZE_1, TENSOR_SIZE_2 };
    param.perm = perm;
    param.size = size;

    param.alpha.d = ALPHA;
    param.beta.s = BETA;

    void *input_tensor = (void *)malloc(sizeof(double complex) * TENSOR_SIZE);
    if (NULL == input_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for input_tensor.");
        return -1;
    }
    void *result_tensor = (void *)malloc(sizeof(float complex) * TENSOR_SIZE);
    if (NULL == result_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for result_tensor.");
        free(input_tensor);
        return -1;
    }

    int32_t ret = ttc_transpose(handler, &param, input_tensor, result_tensor);
    if (0 != ret) {
        TEST_ERR_OUTPUT("Transpose failed.");
    }

    free(input_tensor);
    free(result_tensor);

    return ret;
}


int32_t
cuda_test(
        ttc_datatype_e      type,
        ttc_thread_blk_e    blk,
        int32_t             *lda,
        int32_t             *ldb
        ) {
    ttc_handler_s *handler = ttc_init();
    if (NULL == handler) {
        TEST_ERR_OUTPUT("Cannot create handler.");
        return -1;
    }

    ttc_arch_e arch = TTC_ARCH_CUDA;
    if (0 != ttc_set_opt(handler, TTC_OPT_ARCH, &arch, 0)) {
        TEST_ERR_OUTPUT("Cannot set architecture CUDA.");
        return -1;
    }

    if (0 != ttc_set_opt(handler, TTC_OPT_TB, &blk, 0)) {
        TEST_ERR_OUTPUT("Cannot set thread block.");
        return -1;
    }

    ttc_param_s param = ttc_default_param();
    param.datatype = type;
    param.dim = TENSOR_DIM;

    uint32_t perm[TENSOR_DIM] = { PERM_0, PERM_1, PERM_2 };
    uint32_t size[TENSOR_DIM]
        = { TENSOR_SIZE_0, TENSOR_SIZE_1, TENSOR_SIZE_2 };
    param.perm = perm;
    param.size = size;

    uint32_t input_type_size, result_type_size;

    if (TTC_TYPE_S == type || TTC_TYPE_DEFAULT == type) {
        param.alpha.s = ALPHA;
        param.beta.s = BETA;
        input_type_size = result_type_size = sizeof(float);
    }
    else if (TTC_TYPE_D == type) {
        param.alpha.d = ALPHA;
        param.beta.d = BETA;
        input_type_size = result_type_size = sizeof(double);
    }
    else if (TTC_TYPE_C == type) {
        param.alpha.s = ALPHA;
        param.beta.s = BETA;
        input_type_size = result_type_size = sizeof(float complex);
    }
    else if (TTC_TYPE_Z == type) {
        param.alpha.d = ALPHA;
        param.beta.d = BETA;
        input_type_size = result_type_size = sizeof(double complex);
    }
    else if (TTC_TYPE_SD == type) {
        param.alpha.s = ALPHA;
        param.beta.d = BETA;
        input_type_size = sizeof(float);
        result_type_size = sizeof(double);
    }
    else if (TTC_TYPE_DS == type) {
        param.alpha.d = ALPHA;
        param.beta.s = BETA;
        input_type_size = sizeof(double);
        result_type_size = sizeof(float);
    }
    else if (TTC_TYPE_CZ == type) {
        param.alpha.s = ALPHA;
        param.beta.d = BETA;
        input_type_size = sizeof(float complex);
        result_type_size = sizeof(double complex);
    }
    else if (TTC_TYPE_ZC == type) {
        param.alpha.d = ALPHA;
        param.beta.s = BETA;
        input_type_size = sizeof(double complex);
        result_type_size = sizeof(float complex);
    }
    else {
        TEST_ERR_OUTPUT("Unknown data type.");
        return -1;
    }

    void *input_tensor = (void *)malloc(input_type_size * TENSOR_SIZE);
    if (NULL == input_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for input_tensor.");
        return -1;
    }
    void *result_tensor = (void *)malloc(result_type_size * TENSOR_SIZE);
    if (NULL == result_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for result_tensor.");
        free(input_tensor);
        return -1;
    }

    int32_t ret = ttc_transpose(handler, &param, input_tensor, result_tensor);
    if (0 != ret) {
        TEST_ERR_OUTPUT("Transpose failed.");
    }

    free(input_tensor);
    free(result_tensor);

    return ret;
}


int32_t
no_beta_test(
        ttc_datatype_e  type,
        ttc_arch_e      arch,
        ttc_compiler_e  compiler
        ) {
    ttc_handler_s *handler = ttc_init();
    if (NULL == handler) {
        TEST_ERR_OUTPUT("Cannot create handler.");
        return -1;
    }

    if (0 != ttc_set_opt(handler, TTC_OPT_ARCH, &arch, 0)) {
        TEST_ERR_OUTPUT("Cannot set architecture.");
        return -1;
    }

    if (0 != ttc_set_opt(handler, TTC_OPT_COMPILER, &compiler, 0)) {
        TEST_ERR_OUTPUT("Cannot set compiler.");
        return -1;
    }

    ttc_param_s param = ttc_default_param();
    param.datatype = type;
    param.dim = TENSOR_DIM;

    uint32_t perm[TENSOR_DIM] = { PERM_0, PERM_1, PERM_2 };
    uint32_t size[TENSOR_DIM]
        = { TENSOR_SIZE_0, TENSOR_SIZE_1, TENSOR_SIZE_2 };
    param.perm = perm;
    param.size = size;

    param.alpha.d = ALPHA;
    param.beta.s = 0.0;

    int32_t lda[TENSOR_DIM] = { LDA_0, LDA_1, LDA_2 };
    int32_t ldb[TENSOR_DIM] = { LDB_0, LDB_1, LDB_2 };
    param.lda = lda;
    param.ldb = ldb;

    void *input_tensor = (void *)malloc(sizeof(double complex) * TENSOR_SIZE);
    if (NULL == input_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for input_tensor.");
        return -1;
    }
    void *result_tensor = (void *)malloc(sizeof(double complex) * TENSOR_SIZE);
    if (NULL == result_tensor) {
        TEST_ERR_OUTPUT("Cannot allocate memory for result_tensor.");
        free(input_tensor);
        return -1;
    }

    int32_t ret = ttc_transpose(handler, &param, input_tensor, result_tensor);
    if (0 != ret) {
        TEST_ERR_OUTPUT("Transpose failed.");
    }

    free(input_tensor);
    free(result_tensor);

    return ret;
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
    ttc_param_s param = ttc_default_param();
    param.alpha.s = ALPHA;
    param.beta.s = BETA;
    param.dim = TENSOR_DIM;

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

    return ret;
}
