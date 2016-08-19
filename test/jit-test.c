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



int32_t
main() {
    uint32_t total_num = 0, error_num = 0;


    set_scope("Reuse test");
    TEST_INFO_OUTPUT("Transpose reuse test.");
    ++total_num;
    if (0 != reuse_test()) {
        TEST_ERR_OUTPUT("Reuse test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Reuse test succeed.");
    }


    set_scope("Datatype test");
    TEST_INFO_OUTPUT("Data type test: single precision.");
    ++total_num;
    if (0 != datatype_test(TTC_TYPE_S)) {
        TEST_ERR_OUTPUT("Single precision test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Single precision test succeed.");
    }

    TEST_INFO_OUTPUT("Data type test: double precision.");
    ++total_num;
    if (0 != datatype_test(TTC_TYPE_D)) {
        TEST_ERR_OUTPUT("Double precision test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Double precision test succeed.");
    }

    TEST_INFO_OUTPUT("Data type test: single complex precision.");
    ++total_num;
    if (0 != datatype_test(TTC_TYPE_C)) {
        TEST_ERR_OUTPUT("Single complex precision test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Single complex precision test succeed.");
    }

    TEST_INFO_OUTPUT("Data type test: double complex precision.");
    ++total_num;
    if (0 != datatype_test(TTC_TYPE_Z)) {
        TEST_ERR_OUTPUT("Double complex precision test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Double complex precision test succeed.");
    }

    TEST_INFO_OUTPUT("Data type test: single input double output precision.");
    ++total_num;
    if (0 != datatype_test(TTC_TYPE_SD)) {
        TEST_ERR_OUTPUT("Single input double output precision test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Single input double output precision test succeed.");
    }

    TEST_INFO_OUTPUT("Data type test: double input single output precision.");
    ++total_num;
    if (0 != datatype_test(TTC_TYPE_DS)) {
        TEST_ERR_OUTPUT("Double input single output precision test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Double input single output precision test succeed.");
    }

    TEST_INFO_OUTPUT("Data type test: single complex input "
            "double complex output precision.");
    ++total_num;
    if (0 != datatype_test(TTC_TYPE_CZ)) {
        TEST_ERR_OUTPUT("Single complex input double complex output "
                "precision test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Single complex input double complex output "
                "precision test succeed.");
    }

    TEST_INFO_OUTPUT("Data type test: double complex input "
            "single complex output precision.");
    ++total_num;
    if (0 != datatype_test(TTC_TYPE_ZC)) {
        TEST_ERR_OUTPUT("Double complex input single complex output "
                "precision test failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("Double complex input single complex output "
                "precision test succeed.");
    }


    set_scope("Arch+Compiler test");
    TEST_INFO_OUTPUT("DEFAULT + DEFAULT.");
    ++total_num;
    if (0 != arch_compiler_test(TTC_ARCH_DEFAULT, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("DEFAULT + DEFAULT failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("DEFAULT + DEFAULT succeed.");
    }

    TEST_INFO_OUTPUT("AVX + GXX.");
    ++total_num;
    if (0 != arch_compiler_test(TTC_ARCH_AVX, TTC_CMP_GXX)) {
        TEST_ERR_OUTPUT("AVX + GXX failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("AVX + GXX succeed.");
    }

    TEST_INFO_OUTPUT("AVX + IBM.");
    ++total_num;
    if (0 == arch_compiler_test(TTC_ARCH_AVX, TTC_CMP_IBM)) {
        TEST_ERR_OUTPUT("AVX + IBM failed. "
                "Library should complain IBM not supported.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("AVX + IBM succeed. Library complained.");
    }

    TEST_INFO_OUTPUT("KNC + GXX.");
    ++total_num;
    if (0 == arch_compiler_test(TTC_ARCH_KNC, TTC_CMP_GXX)) {
        TEST_ERR_OUTPUT("KNC + GXX failed. "
                "Library should complain KNC not supported.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("KNC + GXX succeed. Library complained.");
    }


    set_scope("CUDA test");
    int32_t lda[] = { LDA_0, LDA_1, LDA_2 };
    int32_t ldb[] = { LDB_0, LDB_1, LDB_2 };
    TEST_INFO_OUTPUT("float + TB_128.");
    ++total_num;
    if (0 != cuda_test(TTC_TYPE_S, TTC_TB_128, lda, ldb)) {
        TEST_ERR_OUTPUT("float + TB_128 failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("float + TB_128 succeed.");
    }

    TEST_INFO_OUTPUT("float + TB_128. (NULL lda ldb test)");
    ++total_num;
    if (0 == cuda_test(TTC_TYPE_S, TTC_TB_128, NULL, NULL)) {
        TEST_ERR_OUTPUT("float + TB_128 failed. "
                "Library should complain null lda and ldb.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("float + TB_128 succeed. "
                "Library complained null lda and ldb.");
    }

    TEST_INFO_OUTPUT("double + TB_128.");
    ++total_num;
    if (0 != cuda_test(TTC_TYPE_D, TTC_TB_128, lda, ldb)) {
        TEST_ERR_OUTPUT("double + TB_128 failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("double + TB_128 succeed.");
    }

    TEST_INFO_OUTPUT("double float + TB_256.");
    ++total_num;
    if (0 != cuda_test(TTC_TYPE_DS, TTC_TB_256, lda, ldb)) {
        TEST_ERR_OUTPUT("double float + TB_256 failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("double float + TB_256 succeed.");
    }

    TEST_INFO_OUTPUT("double cplx float cplx + TB_512.");
    ++total_num;
    if (0 != cuda_test(TTC_TYPE_ZC, TTC_TB_512, lda, ldb)) {
        TEST_ERR_OUTPUT("double cplx float cplx + TB_512 failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("double cplx float cplx + TB_512 succeed.");
    }


    set_scope("No beta test");
    TEST_INFO_OUTPUT("TYPE_S + DEF + DEF.");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_S, TTC_ARCH_DEFAULT, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_S + DEF + DEF failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_S + DEF + DEF succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_CZ + DEF + GXX.");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_CZ, TTC_ARCH_DEFAULT, TTC_CMP_GXX)) {
        TEST_ERR_OUTPUT("TYPE_CZ + DEF + GXX failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_CZ + DEF + GXX succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_DS + CUDA + DEF.");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_DS, TTC_ARCH_CUDA, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_DS + CUDA + DEF failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_DS + CUDA + DEF succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_Z + CUDA + DEF.");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_Z, TTC_ARCH_CUDA, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_Z + CUDA + DEF failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_Z + CUDA + DEF succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_ZC + DEF + DEF.");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_ZC, TTC_ARCH_CUDA, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_ZC + CUDA + DEF failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_ZC + CUDA + DEF succeed.");
    }


    printf("%sTest finished. TOTAL: %d, SUCCEED: %d, FAILED: %d%s\n",
            CYN, total_num, total_num - error_num, error_num, RESET);

    return 0;
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
    ttc_param_s param = {
        .alpha.s = ALPHA,
        .beta.s = BETA,
        .lda = NULL,
        .ldb = NULL,
        .loop_perm = NULL,
        .dim = TENSOR_DIM
    };
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

    if (0 != ttc_set_opt(handler, TTC_OPT_DATATYPE, &type, 0)) {
        TEST_ERR_OUTPUT("Cannot set datatype.");
        return -1;
    }

    ttc_param_s param = {
        .lda = NULL,
        .ldb = NULL,
        .loop_perm = NULL,
        .dim = TENSOR_DIM
    };
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
        ttc_arch_e arch,
        ttc_compiler_e compiler
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

    ttc_datatype_e type = TTC_TYPE_ZC;
    if (0 != ttc_set_opt(handler, TTC_OPT_DATATYPE, &type, 0)) {
        TEST_ERR_OUTPUT("Cannot set type.");
        return -1;
    }

    ttc_param_s param = {
        .lda = NULL,
        .ldb = NULL,
        .loop_perm = NULL,
        .dim = TENSOR_DIM
    };
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
        ttc_datatype_e type,
        ttc_thread_blk_e blk,
        int32_t *lda,
        int32_t *ldb
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

    if (0 != ttc_set_opt(handler, TTC_OPT_DATATYPE, &type, 0)) {
        TEST_ERR_OUTPUT("Cannot set type.");
        return -1;
    }

    if (0 != ttc_set_opt(handler, TTC_OPT_TB, &blk, 0)) {
        TEST_ERR_OUTPUT("Cannot set thread block.");
        return -1;
    }

    ttc_param_s param = {
        .lda = lda,
        .ldb = ldb,
        .loop_perm = NULL,
        .dim = TENSOR_DIM
    };
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
        ttc_datatype_e type,
        ttc_arch_e arch,
        ttc_compiler_e compiler
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

    if (0 != ttc_set_opt(handler, TTC_OPT_DATATYPE, &type, 0)) {
        TEST_ERR_OUTPUT("Cannot set type.");
        return -1;
    }

    ttc_param_s param = {
        .loop_perm = NULL,
        .dim = TENSOR_DIM
    };
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

