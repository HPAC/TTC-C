/**
 * @file tensor_tc.h
 * @brief The C APIs for TTC
 *
 */
#pragma once


#include <stdint.h>


#ifdef __CPLUSPLUS
extern "C" {
#endif



/* ======== Typedef ======== */

// typedef enum
/// @brief typedef for replacing enum ttc_compiler
typedef enum ttc_compiler ttc_compiler_e;

/// @brief typedef for replacing enum ttc_datatype
typedef enum ttc_datatype ttc_datatype_e;

/// @brief typedef for replacing enum ttc_arch
typedef enum ttc_arch ttc_arch_e;

/// @brief typedef for replacing enum ttc_thread_blk
typedef enum ttc_thread_blk ttc_thread_blk_e;

/// @brief typedef for replacing enum ttc_opt_status
typedef enum ttc_opt_status ttc_opt_status_e;

/// @brief typedef for replacing enum ttc_opt_type
typedef enum ttc_opt_type ttc_opt_type_e;


// typedef union
/// @brief typedef for replacing union ttc_float
typedef union ttc_float ttc_float_u;


// typedef struct
/// @brief typedef for replacing struct ttc_param
typedef struct ttc_param ttc_param_s;

/// @brief typedef for replacing struct ttc_plan
typedef struct ttc_plan ttc_plan_s;

/// @brief typedef for replacing struct ttc_opt
typedef struct ttc_opt ttc_opt_s;

/// @brief typedef for replacing struct ttc_handler
typedef struct ttc_handler ttc_handler_s;


/* ======== Enumeration definition ======== */

/**
 * @brief Enumeration for definition different compilers.
 * @details Used for specifying TTC `--compiler=` parameter.
 *
 * @sa struct ttc_opt, typedef struct ttc_opt ttc_opt_s
 *
 */
enum ttc_compiler {
    TTC_CMP_DEFAULT = 0,    ///< Use default compiler.
    TTC_CMP_GXX     = 1,    ///< G++.
    TTC_CMP_ICPC    = 2,    ///< Intel ICPC.
    TTC_CMP_IBM     = 3,    ///< IBM for AIX.
    TTC_CMP_NVCC    = 4     ///< Nvidia NVCC.
};


/**
 * @brief Enumeration for definition different float types.
 * @details Used for specifying TTC `--datatype=` parameter.
 *
 * @sa struct ttc_opt, typedef struct ttc_opt ttc_opt_s
 *
 */
enum ttc_datatype {
    TTC_TYPE_DEFAULT    = 0,
    ///< Use default datatype.

    TTC_TYPE_S          = 1,
    ///< Both input and output are single precision.

    TTC_TYPE_D          = 2,
    ///< Both input and output are double precision.

    TTC_TYPE_C          = 3,
    ///< Both input and output are single precision complex.

    TTC_TYPE_Z          = 4,
    ///< Both input and output are double precision complex.

    TTC_TYPE_SD         = 5,
    ///< Input is single precision, output is double precision.

    TTC_TYPE_DS         = 6,
    ///< Input is double precision, output is single precision.

    TTC_TYPE_CZ         = 7,
    ///< Input is single precision complex, output is double precision complex.

    TTC_TYPE_ZC         = 8
    ///< Input is double precision complex, output is single precision complex.
};


/**
 * @brief Enumeration for definition different architectures.
 * @details Used for specifying TTC `--architecture=` parameter.
 *
 * @sa struct ttc_opt, typedef struct ttc_opt ttc_opt_s
 *
 */
enum ttc_arch {
    TTC_ARCH_DEFAULT    = 0,    ///< Use default architecture.
    TTC_ARCH_AVX        = 1,    ///< AVX architecture.
    TTC_ARCH_POWER      = 2,    ///< POWER PC architecture (experiment).
    TTC_ARCH_AVX512     = 3,    ///< AVX512 architecture.
    TTC_ARCH_KNC        = 4,    ///< KNC architecture.
    TTC_ARCH_CUDA       = 5     ///< CUDA architecture.
};


/**
 * @brief Enumeration for definition different thread block size when
 * using CUDA architecture.
 * @details Used for specifying TTC `--threadsPerBlock=` parameter.
 *
 * @sa struct ttc_opt, typedef struct ttc_opt ttc_opt_s
 *
 */
enum ttc_thread_blk {
    TTC_TB_DEFAULT  = 0,    ///< Use default thread block size
    TTC_TB_128      = 1,    ///< 128 threads per block
    TTC_TB_256      = 2,    ///< 256 threads per block
    TTC_TB_512      = 3     ///< 512 threads per block
};


/**
 * @brief Enumeration for other options provided by TTC.
 * @details Used in setting `ttc_opt.status` with bitwise OR.
 * For example:
 *
 * @code{.c}
 * obj.status = TTC_KEEP | TTC_IGN_DB;
 * @endcode
 *
 * @sa struct ttc_opt, typedef struct ttc_opt ttc_opt_s
 *
 */
enum ttc_opt_status {
    TTC_KEEP            = 0x1,
    ///< `--keep`: Keep the generated C++files in `${TTC_ROOT}/tmp`.

    TTC_IGN_DB          = 0x2,
    ///< `--ignoreDB`: Don't use SQL database (i.e. no lookup).

    TTC_NO_ALIGN        = 0x4,
    /**<
     * `--no-align`: Prevent using of aligned load/store instructions
     * (use this if your arrays are not aligned).
     */

    TTC_NO_VEC          = 0x8,
    ///< `--no-vec`: Disable vectorization explicitly.

    TTC_NO_TEST         = 0x10,
    ///< `--noTest`: No validation will be done.

    TTC_USE_STM_STORE   = 0x20,
    ///< `--use-streamingStores`: Enable streaming sotres.

    TTC_HOT_A           = 0x40,
    ///< `--hotA`: Keep the input tensor A in cache while measruing.

    TTC_HOT_B           = 0x80,
    ///< `--hotB`: Keep the result tensor B in cache while measruing.
};


/**
 * @brief Enumeration for specifying the options to be set.
 * @details This enumeration is used in struct ttc_set_opt for specifying the
 * options to be set. For example, to set the
 * maximum implementation and some status:
 *
 * @code
 * uint32_t status = TTC_IGN_DB | TTC_KEEP;
 * uint32_t max_impl = 100;
 *
 * int32_t ret = ttc_set_opt(handler, TTC_OPT_STATUS, &status, 1);
 * ret = ttc_set_opt(handler, TTC_OPT_MAX_IMPL, &max_impl, 1);
 * @endcode
 *
 * @sa ttc_set_opt
 *
 */
enum ttc_opt_type {
    TTC_OPT_MAX_IMPL,
    /**<
     * `--maxImplementations=<value>`: Limit the number of implementations.
     * The related `value` must be an `uint32_t` type object, `length` will
     * be omitted.
     */

    TTC_OPT_NUM_THREADS,
    /**<
     * `--numThreads=<value>`: Number of threads to launch. The related
     * `value` must be an `uint32_t` type object, `length` will be omitted.
     */

    TTC_OPT_PREF_DIST,
    /**<
     * `--prefetchDistances=<value>[,<value>, ...]`: Number of blocks
     * ahead of the current block. Default: 5. The related `value` must be a
     * pointer pointing to the first element of an `uint32_t` array, the
     * `length` is the length of this array.
     */

    TTC_OPT_BLOCKINGS,
    /**<
     * `--blockings=<value>x<value>[,<value>x<value>, ...]`:
     * Available blockings. Default: all. The related `value` must be a
     * pointer pointing to the first element of an `uint32_t` array, then
     * length of this array is `length` * 2.
     */

    TTC_OPT_AFFINITY,
    /**<
     * `--affinity=<text>`: Thread affinity. Default:
     * `granularity=fine,compact,1,0`. The `value` must be a pointer pointing
     * to the first character in a string, the `length` is the length of this
     * string. (i.e. the return value of `strlen((char *)value);`)
     */

    TTC_OPT_COMPILER,
    /**<
     * `--compiler=[g++,icpc,ibm,nvcc]`: Choose compiler. Default: icpc.
     * The `value` must be a pointer pointing to a ttc_compiler_e object, the
     * `length` will be omitted.
     * @sa enum ttc_compiler, typedef enum ttc_compiler ttc_compiler_e
     */

    TTC_OPT_DATATYPE,
    /**<
     * `--dataType=[s,d,c,z,sd,ds,cz,zc]`: Select the datatype. Default:
     * Single-precision float. The `value` must be a pointer pointing to a
     * ttc_datatype_e object, the `length` will be omitted.
     * @sa enum ttc_datatype, typedef enum ttc_datatype ttc_datatype_e
     */

    TTC_OPT_ARCH,
    /**<
     * `--architecture=[avx,power,avx512,knc,cuda]`: Choose architecture.
     * Default: avx. The `value` must be a pointer pointing to a
     * ttc_arch_e object, the `length` will be omitted.
     * @sa enum ttc_arch, typedef enum ttc_arch ttc_arch_e
     */

    TTC_OPT_TB,
    /**<
     * `--threadsPerBlock=[128,256,512]`: Set the number of threads per
     * threadblock (CUDA only). Default: 256. The `value` must be a pointer
     * pointing to a ttc_thread_blk_e object, the `length` will be omitted.
     * @sa enum ttc_thread_blk, typedef enum ttc_thread_blk ttc_thread_blk_e
     */

    TTC_OPT_STATUS
    /**<
     * Other options without value. The `value` is set by bitwise OR on
     * the values in ttc_opt_status_e.
     * @sa enum ttc_opt_status, typedef enum ttc_opt_status ttc_opt_status_e
     */
};



/* ======== Union definition ======== */

/**
 * @brief Union for defining float type for `alpha` and `beta` parameters in
 * struct ttc_param.
 * @details This union is used for specifying the type of `alpha` and `beta`.
 *
 * @sa struct ttc_param, typedef struct ttc_param ttc_param_s
 *
 */
union ttc_float {
    float   s;
    /**<
     * For single precision the related `alpha` or `beta`, also for single
     * complex precision.
     */

    double  d;
    /**<
     * For double precision the related `alpha` or `beta`, also for double
     * complex precision.
     */
};



/* ======== Struct definition ======== */

/**
 * @brief Struct for specifying the parameter of every
 * ttc_plan.
 *
 * @details TTC supports arbitrarily dimensional, out-of-place tensor
 * transpositions of the general form:
 *
 * <img src="img/equation.png" width=500>
 *
 * where A and B respectively denote the input and output tensor; PI represents
 * the user-specified transposition, and alpha and beta being scalars (i.e.
 * setting beta != 0 enables the user to update the output tensor B).
 *
 * The parameters here are divided into two kinds, the signature parameters and
 * the non-signature ones. The signature parameters are used to identify a plan
 * in a handler. When comparing two groups of parameters, if the
 * signature parameters are the same, then the two parameter groups will be
 * treated as equal. It is useful to reuse previous plans. `perm`, `size`,
 * `loop_perm` and `dim` are the signature parameters, the others are the
 * non-signature ones.
 *
 * @sa struct ttc_plan, typedef struct ttc_plan ttc_plan_s
 *
 */
struct ttc_param {
    // Non-signature members
    ttc_float_u alpha;
    /**<
     * The alpha in the general form formula, the format should be set
     * according to the data type setting.
     *
     * @sa union ttc_float
     */

    ttc_float_u beta;
    /**<
     * The beta in the general form formula, the format should be set
     * according to the data type setting. Please set it to 0 if you want
     * to use the default value (0.0).
     * @sa union ttc_float
     */

    int32_t    *lda;
    /**<
     * Leading dimension of each dimension of the input tensor. It must be
     * either an empty pointer, or `uint32_t` type array whose length is
     * `dim`.
     */

    int32_t    *ldb;
    /**<
     * Leading dimension of each dimension of the output tensor. It must be
     * either an empty pointer, or `uint32_t` type array whose length is
     * `dim`.
     */

    // Signature members
    uint32_t    *perm;
    /**<
     * The permutations, e.g. for 3-dim tensor, the target transpose could be
     * [1, 0, 2]. It must be a `uint32_t` type array whose length is `dim`.
     */

    uint32_t    *size;
    /**<
     * The size in each dimension of the origin tensor, e.g. for 3-dim tensor,
     * the origin size before transposition could be [100, 200, 300]. It must
     * be a `uint32_t` type array whose length is `dim`.
     */

    uint32_t    *loop_perm;
    /**<
     * Only generating the specified loop order, it could be either an empty
     * pointer, or a `uint32_t` array whose length is `dim`.
     */

    uint32_t    dim;
    /**<
     * The dimension of the tensor to be transposed.
     */
};


/**
 * @brief Struct for a transpose plan.
 *
 * @details A struct describing a transpose plan. It contains some parameters
 * for transposition, two function pointers pointing to the loaded transposition
 * algorithms. The struct is a node of linked list, the head pointer will be
 * saved in the struct ttc_handler.
 *
 * @sa struct ttc_param, typdef struct ttc_param ttc_param_s,
 * struct ttc_handler, typedef struct ttc_handler ttc_handler_s
 *
 */
struct ttc_plan {
    ttc_param_s param;
    ///< The parameter of a plan.

    void        *dlhandler;

    int32_t
    (*fn)(
        const void      *input,
        void            *result,
        const void      *alpha,
        const void      *beta,
        const int32_t   *lda,
        const int32_t   *ldb
        );
    ///< A function pointer pointing to the transposition algorithms.

    int32_t
    (*fn_cuda)(
        const void      *input,
        void            *result,
        const void      *alpha,
        const void      *beta,
        const int32_t   *lda,
        const int32_t   *ldb,
        const int32_t   *size,
        int32_t         total_size
        );
    ///< A function pointer pointing to the CUDA transposition algorithms.

    ttc_plan_s  *next;
    ///< Next pointer for linked list.
};


/**
 * @brief Struct for storing parameters in struct ttc_handler.
 *
 * @details All the fields here should be set with the function ttc_set_opt
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s,
 * enum ttc_opt_type, typedef enum ttc_opt_type ttc_opt_type_e,
 * enum ttc_opt_status, typedef enum ttc_opt_status ttc_opt_status_e,
 * enum ttc_compiler, typedef enum ttc_compiler ttc_compiler_e,
 * enum ttc_datatype, typedef enum ttc_datatype ttc_datatype_e,
 * enum ttc_arch, typedef enum ttc_arch ttc_arch_e,
 * enum ttc_thread_blk, typedef enum ttc_thread_blk ttc_thread_blk_e
 *
 */
struct ttc_opt {
    uint32_t            max_impl;
    /**< `--maxImplementations=<value>`: Limit the number of implementations.
     * The related `value` must be an `uint32_t` type object, `length` will
     * be omitted.
     */

    uint32_t            num_threads;
    /**< `--numThreads=<value>`: Number of threads to launch. The related
     * `value` must be an `uint32_t` type object, `length` will be omitted.
     */

    uint32_t            *pref_dist;
    /**< `--prefetchDistances=<value>[,<value>, ...]`: Number of blocks
     * ahead of the current block. Default: 5. It is a pointer pointing to
     * the first element of an `uint32_t` array.
     */

    uint32_t            pref_dist_len;
    /**< Length of the pref_dist array. */

    uint32_t            *blockings;
    /**< `--blockings=<value>x<value>[,<value>x<value>, ...]`:
     * Available blockings. Default: all. It is a pointer pointing to the
     * first element of an `uint32_t` array.
     */

    uint32_t            blockings_len;
    /**< Length of the blockings array. */

    char                *affinity;
    /**< `--affinity=<text>`: Thread affinity. Default:
     * `granularity=fine,compact,1,0`. It is a pointer pointing to the first
     * character in a string.
     */

    ttc_compiler_e      compiler;
    /**< `--compiler=[g++,icpc,ibm,nvcc]`: Choose compiler. Default: icpc.
     * It is a ttc_compiler_e object.
     * @sa enum ttc_compiler, typedef enum ttc_compiler ttc_compiler_e
     */

    ttc_datatype_e      datatype;
    /**< `--dataType=[s,d,c,z,sd,ds,cz,zc]`: Select the datatype. Default:
     * Single-precision float. It is a ttc_datatype_e object.
     * @sa enum ttc_datatype, typedef enum ttc_datatype ttc_datatype_e
     */

    ttc_arch_e          arch;
    /**< `--architecture=[avx,power,avx512,knc,cuda]`: Choose architecture.
     * Default: avx. It is a ttc_arch_e object.
     * @sa enum ttc_arch, typedef enum ttc_arch ttc_arch_e
     */

    ttc_thread_blk_e    tb;
    /**< `--threadsPerBlock=[128,256,512]`: Set the number of threads per
     * threadblock (CUDA only). Default: 256. It is a ttc_thread_blk_e
     * object.
     * @sa enum ttc_thread_blk, typedef enum ttc_thread_blk ttc_thread_blk_e
     */

    uint32_t            status;
    /**< Other options without value. It is set by bitwise OR.
     * @sa enum ttc_opt_status, typedef enum ttc_opt_status ttc_opt_status_e
     */
};


/**
 * @brief Struct for TTC handler.
 *
 * @details This handelr will be generated by function ttc_init and released
 * by ttc_release . It is used for specifiying the environment parameters.
 *
 * @sa struct ttc_opt, typedef struct ttc_opt ttc_opt_s,
 * struct ttc_plan, typedef struct ttc_plan ttc_plan_s,
 * ttc_init, ttc_release, ttc_set_opt, ttc_plan, ttc_transpose
 *
 */
struct ttc_handler {
    ttc_opt_s   options;
    ///< Options for transposition, such as compiler choice, thread number.

    ttc_plan_s  *plans;
    ///< A list of plans that used for performing transpositions.
};



/* ======== Function declaration ======== */

/**
 * @brief A function for initialization and generating a TTC handler.
 *
 * @return A pointer point to the generated handler, it should be released
 * in function ttc_release . If error happens, it will return a null pointer.
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s
 *
 */
ttc_handler_s *
ttc_init(
        );


/**
 * @brief A function for releasing the handler.
 *
 * @param[in,out] handler A pointer pointing to a TTC handler to be released.
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s
 *
 */
void
ttc_release(
        ttc_handler_s   *handler
        );


/**
 * @brief A function for setting options in a TTC handler.
 *
 * @param[in,out]   handler A pointer pointing to a TTC handler.
 * @param[in]       type    The target option to be set.
 * @param[in]       value   A pointer pointing to the option's value.
 * @param[in]       length  If the value is an array, this parameter will be
 * used to describe the length of it. If the the value is not an array, it could
 * be any values.
 *
 * @return The status, if the function parameter are not correct (e.g. `value`
 * is null), then it will return -1. If some internal error happens (e.g. cannot
 * allocate memory), the return value will be the `errno`. If everything goes
 * well, the return value will be 0.
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s,
 * enum ttc_opt_type, typedef enum ttc_opt_type ttc_opt_type_e
 *
 */
int32_t
ttc_set_opt(
        ttc_handler_s   *handler,
        ttc_opt_type_e  type,
        const void      *value,
        uint32_t        length
        );


/**
 * @brief A function for executing a plan.
 *
 * @details It will perform the transposition described by the `handler` and
 * `param`. It will firstly check if the plan described by the `param` exists.
 * If so, then executes the plan directly. If not, it will create a new plan and
 * save it in the handler.
 *
 * @param[in,out]   handler A pointer pointing to a TTC handler.
 * @param[in]       param   A parameter describing the transposition. Stored in
 * host memory when using CUDA.
 *
 * @param[in]       input   A pointer pointing to the input tensor. Stored in
 * host memory when using CUDA.
 *
 * @param[out]      result  A pointer pointing to a piece of memory for storing
 * result. Stored in the host memory when using CUDA.
 *
 * @return The status, if the function parameter are not correct (e.g. `value`
 * is null), then it will return -1. If some internal error happens (e.g. cannot
 * allocate memory), the return value will be the `errno`. If everything goes
 * well, the return value will be 0.
 *
 * @sa struct ttc_handler, typedef struct ttc_handler ttc_handler_s,
 * struct ttc_param, typedef struct ttc_param_s
 *
 */
int32_t
ttc_transpose(
        ttc_handler_s       *handler,
        const ttc_param_s   *param,
        const void          *input,
        void                *result
        );



#ifdef __CPLUSPLUS
}
#endif

