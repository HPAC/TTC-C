/**
 * @file cuda-test.c
 *
 * @brief Test example for TTC C API.
 *
 */

#include <stdlib.h>
#include <stdint.h>

#include "test-util.h"
#include "ttc_c.h"


int32_t
main() {
    uint32_t total_num = 0, error_num = 0;

    set_scope("CUDA test");
    int32_t lda[] = { LDA_0, LDA_1, LDA_2 };
    int32_t ldb[] = { LDB_0, LDB_1, LDB_2 };
    TEST_INFO_OUTPUT("float + TB_128.");
    ++total_num;
    if (0 != cuda_test(TTC_TYPE_S, TTC_TB_128, lda, ldb)) {
        TEST_ERR_OUTPUT("TYPE_S + TB_128 failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_S + TB_128 succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_S + TB_128. (NULL lda ldb test)");
    ++total_num;
    if (0 == cuda_test(TTC_TYPE_S, TTC_TB_128, NULL, NULL)) {
        TEST_ERR_OUTPUT("TYPE_S + TB_128 failed. "
                "Library should complain null lda and ldb.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_S + TB_128 succeed. "
                "Library complained null lda and ldb.");
    }

    TEST_INFO_OUTPUT("TYPE_D + TB_128.");
    ++total_num;
    if (0 != cuda_test(TTC_TYPE_D, TTC_TB_128, lda, ldb)) {
        TEST_ERR_OUTPUT("TYPE_D + TB_128 failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_D + TB_128 succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_DS + TB_256.");
    ++total_num;
    if (0 != cuda_test(TTC_TYPE_DS, TTC_TB_256, lda, ldb)) {
        TEST_ERR_OUTPUT("TYPE_DS + TB_256 failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_DS + TB_256 succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_ZC + TB_512.");
    ++total_num;
    if (0 != cuda_test(TTC_TYPE_ZC, TTC_TB_512, lda, ldb)) {
        TEST_ERR_OUTPUT("TYPE_ZC + TB_512 failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_ZC + TB_512 succeed.");
    }


    set_scope("No beta test");
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
        TEST_SUCC_OUTPUT("TYPE_CZ + CUDA + DEF succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_CZ + CUDA + DEF.");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_CZ, TTC_ARCH_CUDA, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_CZ + CUDA + DEF failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_CZ + CUDA + DEF succeed.");
    }


    printf("%sTest finished. TOTAL: %d, SUCCEED: %d, FAILED: %d%s\n",
            CYN, total_num, total_num - error_num, error_num, RESET);

    return 0;
}

