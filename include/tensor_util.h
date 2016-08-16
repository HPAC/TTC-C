/**
 * @file tensor_util.h
 * @brief The utilities used for both TTC and TCC.
 *
 * @details The data structures, functions and macros defined here are only
 * designed for TTC and TCC and used internally. They are not robust enough to
 * all kinds of parameters like null pointers. Therefore, please *DO NOT* use
 * them in your own functions.
 *
 */

#pragma once



#ifdef __CPLUSPLUS
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>


#define RED                 "\x1B[31m"
#define GRN                 "\x1B[32m"
#define YEL                 "\x1B[33m"
#define BLU                 "\x1B[34m"
#define MAG                 "\x1B[35m"
#define CYN                 "\x1B[36m"
#define WHT                 "\x1B[37m"
#define RESET               "\x1B[0m"


#ifndef TENSOR_DEBUG

#define DEBUG_SET_NAMESPACE(str)
#define DEBUG_INFO_OUTPUT(str)
#define DEBUG_WARN_OUTPUT(str)
#define DEBUG_ERR_OUTPUT(str)

#else

#include <stdio.h>
#include <string.h>


extern char func_namespace[];

/**
 * @brief A macro that setting the namespace of the debug macro.
 *
 * @param str A string of the namespace.
 *
 */
#define DEBUG_SET_NAMESPACE(str) set_namespace(str)


/**
 * @brief A macro that outputs debug information when debug flag is turned on.
 *
 * @details The output of this macro is string parameter.
 *
 * @param str A string describing the debug information.
 *
 */
#define DEBUG_INFO_OUTPUT(str) fprintf(stdout, "[INFO] %s: %s\n", \
        func_namespace, str)


/**
 * @brief A macro that outputs warning information when debug flag is turned on.
 *
 * @details The output of this macro is the contents of the string argument.
 *
 * @param str A string describing the warning information.
 *
 */
#define DEBUG_WARN_OUTPUT(str) fprintf(stderr, "%s[WARN] %s: %s%s\n", \
        YEL, func_namespace, str, RESET)


/**
 * @brief A macro that outputs error information when debug flag is turned on.
 *
 * @details The output of this macro is the contents of the string argument.
 *
 * @param str A string describing the error information.
 *
 */
#define DEBUG_ERR_OUTPUT(str) fprintf(stderr, "%s[ERROR] %s: %s%s\n", \
        RED, func_namespace, str, RESET)



/**
 * @brief A function for setting debug information prefix.
 *
 * @parami[in] name The prefix.
 *
 */
void set_namespace(const char *name);


#endif



/* ======== Function declaration ======== */

/**
 * @brief A function for comparing two arrays of `uint32_t` type data.
 *
 * @param arr1 The first array to be compared, MUST NOT be null pointer.
 * @param arr2 The second array to be compared, MUST NOT be null pointer.
 * @param length The length for both arrays.
 *
 * @return If the content of the two arrays are the same, then return true,
 * else return false.
 *
 * @warning Not robust to null pointer input, which will lead to undefined
 * behaviors.
 *
 */
bool
uint32cmp(
        const uint32_t  *arr1,
        const uint32_t  *arr2,
        uint32_t        length
        );



#ifdef __CPLUSPLUS
}
#endif

