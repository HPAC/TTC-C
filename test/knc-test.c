/**
 * @file knc-test.c
 *
 * @brief KNC test example for TTC C API.
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
    TEST_INFO_OUTPUT("KNC + DEFAULT.");
    ++total_num;
    if (0 != arch_compiler_test(TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("KNC + DEFAULT failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("KNC + DEFAULT succeed.");
    }

    TEST_INFO_OUTPUT("KNC + ICPC.");
    ++total_num;
    if (0 != arch_compiler_test(TTC_ARCH_KNC, TTC_CMP_ICPC)) {
        TEST_ERR_OUTPUT("KNC + ICPC failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("KNC + ICPC succeed.");
    }

    TEST_INFO_OUTPUT("KNC + GXX.");
    ++total_num;
    if (0 != arch_compiler_test(TTC_ARCH_KNC, TTC_CMP_GXX)) {
        TEST_ERR_OUTPUT("KNC + GXX failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("KNC + GXX succeed.");
    }


    set_scope("No beta test");
    TEST_INFO_OUTPUT("TYPE_S + KNC + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_S, TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_S + KNC + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_S + KNC + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_D + KNC + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_D, TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_D + KNC + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_D + KNC + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_C + KNC + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_C, TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_C + KNC + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_C + KNC + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_Z + KNC + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_Z, TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_Z + KNC + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_Z + KNC + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_SD + KNC + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_SD, TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_SD + KNC + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_SD + KNC + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_DS + KNC + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_DS, TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_DS + KNC + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_DS + KNC + Default Compiler succeed.");
    }

    TEST_INFO_OUTPUT("TYPE_CZ + KNC + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_CZ, TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_CZ + KNC + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_CZ + KNC + Default Compiler succeed.");

    TEST_INFO_OUTPUT("TYPE_ZC + KNC + Default Compiler");
    ++total_num;
    if (0 != no_beta_test(TTC_TYPE_ZC, TTC_ARCH_KNC, TTC_CMP_DEFAULT)) {
        TEST_ERR_OUTPUT("TYPE_ZC + KNC + Default Compiler failed.");
        ++error_num;
    }
    else {
        TEST_SUCC_OUTPUT("TYPE_ZC + KNC + Default Compiler succeed.");
    }
    }

    printf("%sTest finished. TOTAL: %d, SUCCEED: %d, FAILED: %d%s\n",
            CYN, total_num, total_num - error_num, error_num, RESET);

    return 0;
}

