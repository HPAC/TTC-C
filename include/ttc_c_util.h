/**
 * @file tensor_tc_util.h
 * @brief The utilities for TTC C APIs' internal usage.
 *
 */
#pragma once



#ifdef __CPLUSPLUS
extern "C" {
#endif


#include <stdint.h>

#include "ttc_c.h"



/* ======== Macro ======== */

#define TTC_GEN_BUF_SIZE        1024

#define TTC_EXECUTABLE          "ttc"

#define TTC_ARG_ALPHA           "--alpha="
#define TTC_ARG_BETA            "--beta="
#define TTC_ARG_LDA             "--lda="
#define TTC_ARG_LDB             "--ldb="
#define TTC_ARG_SIZE            "--size="
#define TTC_ARG_PERM            "--perm="
#define TTC_ARG_LOOP_PERM       "--loopPerm="
#define TTC_ARG_MAX_IMPL        "--maxImplementations="
#define TTC_ARG_NUM_THREADS     "--numThreads="
#define TTC_ARG_PREF_DIST       "--prefetchDistances="
#define TTC_ARG_BLOCKINGS       "--blockings="
#define TTC_ARG_AFFINITY        "--affinity="

#define TTC_ARG_COMPILER        "--compiler="
#define TTC_ARG_CMP_GXX         "g++"
#define TTC_ARG_CMP_ICPC        "icpc"
#define TTC_ARG_CMP_IBM         "ibm"
#define TTC_ARG_CMP_NVCC        "nvcc"

#define TTC_ARG_DATATYPE        "--dataType="
#define TTC_ARG_TYPE_S          "s"
#define TTC_ARG_TYPE_D          "d"
#define TTC_ARG_TYPE_C          "c"
#define TTC_ARG_TYPE_Z          "z"
#define TTC_ARG_TYPE_SD         "sd"
#define TTC_ARG_TYPE_DS         "ds"
#define TTC_ARG_TYPE_CZ         "cz"
#define TTC_ARG_TYPE_ZC         "zc"

#define TTC_ARG_ARCH            "--architecture="
#define TTC_ARG_ARCH_AVX        "avx"
#define TTC_ARG_ARCH_POWER      "power"
#define TTC_ARG_ARCH_AVX512     "avx512"
#define TTC_ARG_ARCH_KNC        "knc"
#define TTC_ARG_ARCH_CUDA       "cuda"

#define TTC_ARG_TB              "--threadsPerBlock="
#define TTC_ARG_TB_128          "128"
#define TTC_ARG_TB_256          "256"
#define TTC_ARG_TB_512          "512"

#define TTC_ARG_KEEP            "--keep"
#define TTC_ARG_IGN_DB          "--ignoreDatabase"
#define TTC_ARG_NO_ALIGN        "--no-align"
#define TTC_ARG_NO_VEC          "--no-vec"
#define TTC_ARG_NO_TEST         "--noTest"
#define TTC_ARG_USE_STM_STORE   "--use-streamingStores"
#define TTC_ARG_HOT_A           "--hotA"
#define TTC_ARG_HOT_B           "--hotB"

#define TTC_PIPE_RD             0
#define TTC_PIPE_WR             1

#define TTC_DIR_GEN_CODE        "ttc_transpositions/"
#define TTC_DIR_BACK_GEN        "../"
#define TTC_DIR_TTC_ROOT        "$TTC_ROOT"

#define TTC_FUNC_SYMBOL         "transpose"


#define TTC_GXX_CMPL            "g++ -c -O2 -w -fPIC "
#define TTC_GXX_LINK            "g++ -shared "

#define TTC_ICPC_CMPL           "icpc -c -O2 -w -fPIC "
#define TTC_ICPC_LINK           "icpc -shared "

#define TTC_NVCC_CMPL           "nvcc -c -O2 -rdc=true "
#define TTC_NVCC_LINK           "nvcc -rdc=true -shared "

#define TTC_ARCH_AVX_GXX_CMPL   TTC_GXX_CMPL "-fopenmp -march=native "
#define TTC_ARCH_AVX_GXX_LINK   TTC_GXX_LINK "-fopenmp "

#define TTC_ARCH_AVX_ICPC_CMPL  TTC_ICPC_CMPL "-xhost -openmp "
#define TTC_ARCH_AVX_ICPC_LINK  TTC_ICPC_LINK "-openmp "

#define TTC_ARCH_CUDA_CMPL      TTC_NVCC_CMPL "-Xcompiler '-fPIC' -lgomp "
#define TTC_ARCH_CUDA_LINK      TTC_NVCC_LINK "-lgomp "

#define TTC_ARCH_KNC_CMPL       TTC_ICPC_CMPL "-mmic -openmp "
#define TTC_ARCH_KNC_LINK       TTC_ICPC_LINK "-openmp "

#define TTC_ARCH_AVX512_CMPL    TTC_ICPC_CMPL "-xMIC-AVX512 -openmp "
#define TTC_ARCH_AVX512_LINK    TTC_ICPC_LINK "-openmp "

#define TTC_ARCH_DEF_CMPL       TTC_ARCH_AVX_ICPC_CMPL
#define TTC_ARCH_DEF_LINK       TTC_ARCH_AVX_ICPC_LINK



/* ======== Function declaration ======== */

/**
 * @brief A function for creating a new transposition plan.
 *
 * @details If a same plan exists (according to the parameter signature, see
 * also struct ttc_param), it will return a pointer pointing to
 * this plan, which is stored in a handler.
 *
 * @param[in,out]   handler A pointer pointing to a TTC handler.
 * @param[in]       param   A paramter object describing the plan.
 *
 * @return The pointer pointing to the plan. If some errors happen, it will
 * return a null pointer.
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s,
 * struct ttc_param, typedef struct ttc_param ttc_param_s
 *
 */
ttc_plan_s *
ttc_plan(
        ttc_handler_s       *handler,
        const ttc_param_s   *param
        );


/**
 * @brief A function that actually creats a new transposition plan.
 *
 * @details This function is called by ttc_plan, and create a new one according
 * to the provided options and parameters.
 *
 * @param[in] options A pointer pointing to a ttc_opt_s object.
 * @param[in] param A paramter object describing the plan.
 *
 * @return The pointer pointing to the plan. If some errors happen, it will
 * return a null pointer.
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s,
 * struct ttc_param, typedef struct ttc_param ttc_param_s
 *
 */
ttc_plan_s *
ttc_create_plan(
        const ttc_opt_s     *options,
        const ttc_param_s   *param
        );

/**
 * @brief A function for releasing a plan.
 *
 * @details It is actually used internally.
 *
 * @warning It should not be used directly unless you are sure that this plan
 * does not locate in a linked list. Otherwise it will cause memory leak.
 *
 * @param[in,out] plan A pointer pointing to a plan to be released.
 *
 * @return The status, if everything goes well or the input pointer is null,
 * it will return 0. Otherwise it will return non-zero value that defined in
 * the dlfcn.h.
 *
 * @sa struct ttc_plan, typedef struct ttc_plan ttc_plan_s,
 *
 */
int32_t
ttc_release_plan(
        ttc_plan_s  *plan
        );


/**
 * @brief A function for executing a plan.
 *
 * @details It is used internally.
 *
 * @param[in]   plan    A pointer pointing to a plan to be executed.
 * @param[in]   input   A pointer pointing to the input tensor.
 * @param[out]  result  A pointer pointing to a piece of memory for storing
 * result.
 *
 * @return The status, if the function parameter are not correct (e.g. `value`
 * is null), then it will return -1. If some internal error happens (e.g. cannot
 * allocate memory), the return value will be the `errno`. If everything goes
 * well, the return value will be 0.
 *
 * @sa struct ttc_plan, typedef struct ttc_plan ttc_plan_s,
 *
 */
int32_t
ttc_exec_plan(
        const ttc_plan_s    *plan,
        const void          *input,
        void                *result
        );


/**
 * @brief A function for executing a plan with CUDA.
 *
 * @details It is used internally.
 *
 * @param[in]   plan    A pointer pointing to a plan to be executed.
 * @param[in]   input   A pointer pointing to the input tensor.
 * @param[out]  result  A pointer pointing to a piece of memory for storing
 * result.
 *
 * @return The status, if the function parameter are not correct (e.g. `value`
 * is null), then it will return -1. If some internal error happens (e.g. cannot
 * allocate memory), the return value will be the `errno`. If everything goes
 * well, the return value will be 0.
 *
 * @sa struct ttc_plan, typedef struct ttc_plan ttc_plan_s,
 *
 */
int32_t
ttc_exec_plan_cuda(
        const ttc_plan_s    *plan,
        const void          *input,
        void                *result
        );


/**
 * @brief Function for generating the TTC command line arguments.
 *
 * @details It accepts two parameters which describe the arguments. One is the
 * ttc_opt_s in ttc_handler_s, it indicates the environments for executing the
 * TTC, e.g. the compiler choice. Another parameter is a ttc_param_s object in
 * the ttc_plan_s. It describes the transposition itself. It will finally
 * generate the argument vector which can be used in execvp function.
 *
 * @param[in] options   A pointer pointing to the ttc_opt_s object in the
 * ttc_handler_s object.
 *
 * @param[in] param     A pointer pointing to the ttc_param_s object in the
 * ttc_plan_s object.
 *
 * @return The generated argument vector describing the arguments. The last
 * element of the vector is a null pointer. If error happens, the return value
 * will be a null pointer.
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s,
 * struct ttc_opt, typedef struct ttc_opt, ttc_opt_s,
 * struct ttc_plan, typedef struct ttc_plan ttc_plan_s,
 * struct ttc_param, typedef struct ttc_param, ttc_param_s
 *
 */
char **
ttc_gen_arg(
        const ttc_opt_s     *options,
        const ttc_param_s   *param
        );


/**
 * @brief Function for calculating the length of the argument vector.
 *
 * @details It is used internally in the function ttc_gen_arg.
 *
 * @param[in] options   A pointer pointing to the ttc_opt_s object in the
 * ttc_handler_s object.
 *
 * @param[in] param     A pointer pointing to the ttc_param_s object in the
 * ttc_plan_s object.
 *
 * @return The length of argument vector. If either the parameter is a null
 * pointer, the function returns -1.
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s,
 * struct ttc_opt, typedef struct ttc_opt, ttc_opt_s,
 * struct ttc_plan, typedef struct ttc_plan ttc_plan_s,
 * struct ttc_param, typedef struct ttc_param, ttc_param_s
 * ttc_gen_arg
 *
 */
int32_t
ttc_count_arg(
        const ttc_opt_s     *options,
        const ttc_param_s   *param
        );


/**
 * @brief Function for releasing the generated argument vector.
 *
 * @param[in,out] arg A pointer pointing to the argument vector to be released.
 *
 * @sa ttc_gen_arg
 *
 */
void
ttc_release_arg(
        char **arg
        );


/**
 * @brief Function for locating the header file name in the TTC output.
 *
 * @details It reads the standard output from the pipe created for communication
 * between parent process and the Python TTC program. It is called by function
 * ttc_create_plan.
 *
 * @param[in]   pipe_fileno File descriptor of the pipe output.
 * @param[out]  seek_buf    A buffer for storing the header file names.
 *
 * @return The status, if succeed, return 0. Non-zero means error happens, e.g.
 * not well initialized parameter. If succeed, the result will be stored in the
 * seek_buf.
 *
 * @warning The functions should not be used directly.
 *
 * @sa ttc_create_plan
 *
 */
int32_t
ttc_locate_header(
        int32_t pipe_fileno,
        char    seek_buf[]
        );


/**
 * @brief Function for generating C++/CUDA code.
 *
 * @details It is called by function ttc_create_plan.
 *
 * @param[in]   options             A pointer pointing to the ttc_opt_s object
 * in the ttc_handler_s object.
 *
 * @param[in]   param               A pointer pointing to the ttc_param_s object
 * in the ttc_plan_s object.
 *
 * @param[in]   header_file_name    A string of the header file name.
 * @param[out]  target_prefix       Buffer for storing generated file prefix, if
 * the function returns successfully, it will be set with the name of target
 * file without suffix, e.g. target_file.cpp -> target_file.
 *
 * @param[out]  target_suffix       Buffer for storing generated file suffix, if
 * the function returns successfully, it will be set with the suffix of target
 * file without name, e.g. target_file.cpp -> cpp.
 *
 * @return The status, return 0 if succeed, otherwise non-zero value.
 *
 * @warning The functions should not be used directly.
 *
 * @sa ttc_create_plan
 *
 */
int32_t
ttc_gen_code(
        const ttc_opt_s     *options,
        const ttc_param_s   *param,
        const char          *header_file_name,
        char                *target_prefix,
        char                *target_suffix
        );


/**
 * @brief Function for creating shared library.
 *
 * @details It is called by function ttc_create_plan.
 *
 * @param[in]   options             A pointer pointing to the ttc_opt_s object
 * in the ttc_handler_s object.
 *
 * @param[in]   header_file_name    A string of the header file name.
 * @param[in]  target_prefix        Buffer for storing generated file prefix.
 * @param[in]  target_suffix        Buffer for storing generated file suffix.
 *
 * @return A pointer pointing to a dlhandler when succeed, or NULL when errors
 * happen.
 *
 * @warning The functions should not be used directly.
 *
 * @sa ttc_create_plan
 *
 */
void *
ttc_gen_lib(
        const ttc_opt_s *options,
        const char      *target_prefix,
        const char      *target_suffix
        );



#ifdef __CPLUSPLUS
}
#endif

