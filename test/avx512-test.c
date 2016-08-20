/**
 * @file avx512-test.c
 *
 * @brief AVX 512 test example for TTC C API.
 *
 */

#include <stdlib.h>
#include <stdint.h>

#include "test-util.h"
#include "ttc_c.h"


int32_t
main() {
    uint32_t total_num = 0, error_num = 0;


    set_scope("Arch+Compiler test");
    TEST_INFO_OUTPUT("AVX512 + DEFAULT.");
    ++total_num;
    if (0 != arch_compiler_test(TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("AVX512 + DEFAULT failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("AVX512 + DEFAULT succeed.");
    }

    TEST_INFO_OUTPUT("AVX512 + ICPC.");
    ++total_num;
    if (0 != arch_compiler_test(TTC_ARCH_AVX512, TTC_CMP_ICPC)) {
        TEST_ERR_OUTPUT("AVX512 + ICPC failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("AVX512 + ICPC succeed.");
    }

    TEST_INFO_OUTPUT("AVX512 + GXX.");
    ++total_num;
    if (0 != arch_compiler_test(TTC_ARCH_AVX512, TTC_CMP_GXX)) {
        TEST_ERR_OUTPUT("AVX512 + GXX failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("AVX512 + GXX succeed.");
    }


    set_scope("No beta test");
    TEST_INFO_OUTPUT("TYPE_S + AVX512 + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_S, TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_S + AVX512 + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_S + AVX512 + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_D + AVX512 + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_D, TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_D + AVX512 + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_D + AVX512 + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_C + AVX512 + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_C, TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_C + AVX512 + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_C + AVX512 + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_Z + AVX512 + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_Z, TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_Z + AVX512 + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_Z + AVX512 + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_SD + AVX512 + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_SD, TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_SD + AVX512 + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_SD + AVX512 + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_DS + AVX512 + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_DS, TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_DS + AVX512 + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_DS + AVX512 + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_CZ + AVX512 + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_CZ, TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_CZ + AVX512 + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_CZ + AVX512 + Default Compiler succeed.");

    TEST_INFO_OUTPUT("TYPE_ZC + AVX512 + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_ZC, TTC_ARCH_AVX512, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_ZC + AVX512 + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_ZC + AVX512 + Default Compiler succeed.");
    }
    }

    printf("%sTest finished. TOTAL: %d, SUCCEED: %d, FAILED: %d%s\n",
            CYN, total_num, total_num - error_num, error_num, RESET);

    return 0;
}

