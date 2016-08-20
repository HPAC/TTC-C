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

