/**
 * @file jit-test.c
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


    printf("%sTest finished. TOTAL: %d, SUCCEED: %d, FAILED: %d%s\n",
            CYN, total_num, total_num - error_num, error_num, RESET);

    return 0;
}

