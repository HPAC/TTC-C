#include "ttc_c_util.h"

#include <stdlib.h>
#include <stdint.h>

#include <stdio.h>          // For sprintf
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/wait.h>

#include "tensor_util.h"
#include "ttc_c.h"



/* ======== Internal macro  ======== */

#define TTC_SET_ARG_CHECK_MALLOC(ptr)       \
    if (NULL == ptr) {                      \
        DEBUG_ERR_OUTPUT(strerror(errno));  \
        ttc_release_arg(argv);              \
        return NULL;                        \
    }


#define TTC_SET_ARG_STATUS(str, flag)                           \
    if (0 != (options->status & flag)) {                        \
        DEBUG_INFO_OUTPUT("Setting arg: " str);                 \
        int32_t str_len = sizeof(str);                          \
        argv[arg_idx] = (char *)malloc(sizeof(char) * str_len); \
        TTC_SET_ARG_CHECK_MALLOC(argv[arg_idx])                 \
        memcpy(argv[arg_idx], str, str_len);                    \
        ++arg_idx;                                              \
    }


#define TTC_SET_ARG_ARRAY(ptr, arr_size, str)                   \
    if (NULL != ptr) {                                          \
        DEBUG_INFO_OUTPUT("Setting arg: " str);                 \
        int32_t idx, val_len, str_len = sizeof(str) - 1;        \
        char *parse_ptr = arg_buf + str_len;                    \
        memcpy(arg_buf, str, str_len);                          \
        for (idx = 0; idx < arr_size; ++idx) {                  \
            val_len = sprintf(parse_ptr, "%d,", ptr[idx]);      \
            str_len += val_len; parse_ptr += val_len;           \
        }                                                       \
        argv[arg_idx] = (char *)malloc(sizeof(char) * str_len); \
        TTC_SET_ARG_CHECK_MALLOC(argv[arg_idx]);                \
        memcpy(argv[arg_idx], arg_buf, str_len);                \
        argv[arg_idx][str_len - 1] = '\0';                      \
        ++arg_idx;                                              \
    }


#define TTC_SET_ARG_SINGLE(target, str, pat)                        \
    if (0 != target) {                                              \
        DEBUG_INFO_OUTPUT("Setting arg: " str);                     \
        int32_t str_len = 1 + sprintf(arg_buf, pat, str, target);   \
        argv[arg_idx] = (char *)malloc(sizeof(char) * str_len);     \
        TTC_SET_ARG_CHECK_MALLOC(argv[arg_idx]);                    \
        memcpy(argv[arg_idx], arg_buf, str_len);                    \
        ++arg_idx;                                                  \
    }


#define TTC_SET_ARG_ENUM(str, val)                              \
    DEBUG_INFO_OUTPUT("Setting: " str val);                     \
    arg_len = sizeof(str) + sizeof(val) - 1;                    \
    argv[arg_idx] = (char *)malloc(sizeof(char) * arg_len);     \
    TTC_SET_ARG_CHECK_MALLOC(argv[arg_idx]);                    \
    memcpy(argv[arg_idx], str val, arg_len)


#define INCLUDE_STR "include"


/* ======== Internal function ======== */

int32_t
ttc_gen_code_avx(
        const ttc_opt_s     *options,
        const ttc_param_s   *param,
        const char          *target_prefix,
        const char          *target_suffix
        );


int32_t
ttc_gen_code_cuda(
        const ttc_opt_s     *options,
        const ttc_param_s   *param,
        const char          *target_prefix,
        const char          *target_suffix
        );


int32_t
ttc_gen_type_macro(
        const ttc_opt_s     *options,
        const ttc_param_s   *param,
        FILE                *target_file
        );



/* ======== Function definition ======== */

ttc_plan_s *
ttc_plan(
        ttc_handler_s       *handler,
        const ttc_param_s   *param
        ) {
    DEBUG_SET_NAMESPACE("ttc_plan");
    DEBUG_INFO_OUTPUT("Making a plan.");
    // Parameter check
    if (NULL == handler) {
        DEBUG_ERR_OUTPUT("handler is not initialized.");
        return NULL;
    }
    if (NULL == param || NULL == param->perm || NULL == param->size) {
        DEBUG_ERR_OUTPUT("param is not well initialized.");
        return NULL;
    }


    // Look up handler
    DEBUG_INFO_OUTPUT("Checking plan existence.");
    ttc_plan_s *tail = handler->plans;
    if (NULL != handler->plans) {
        ttc_plan_s *lookup = handler->plans;
        while (NULL != lookup) {
            ttc_param_s *cmp = &lookup->param;
            uint32_t dim = cmp->dim;

            // Compare signature members
            // Compare dim
            if (param->dim == dim
                // Compare perm
                && uint32cmp(param->perm, cmp->perm, dim)
                // Compare size
                && uint32cmp(param->size, cmp->size, dim)
                // Compare loop_perm
                && ((NULL == param->loop_perm && NULL == cmp->loop_perm)
                    || (NULL != param->loop_perm && NULL != cmp->loop_perm
                        && uint32cmp(param->loop_perm, cmp->loop_perm, dim)))
                ) {
                // If all match, return this plan
                DEBUG_INFO_OUTPUT("Matched a existed plan.");
                return lookup;
            }

            if (NULL == lookup->next)
                tail = lookup;
            lookup = lookup->next;
        }
    }


    // Create new plan and attach it to the handler
    DEBUG_INFO_OUTPUT("Creating a new plan.");
    ttc_plan_s *new_plan = ttc_create_plan(&handler->options, param);
    DEBUG_SET_NAMESPACE("ttc_plan");
    if (NULL == new_plan) {
        DEBUG_ERR_OUTPUT("Cannot create a new plan.");
        return NULL;
    }

    // Attach new plan to the tail of exist plan in handler
    DEBUG_INFO_OUTPUT("Attaching the new plan to the handler.");
    if (NULL == handler->plans)
        handler->plans = new_plan;
    else
        tail->next = new_plan;

    return new_plan;
}


#define TTC_PLAN_NULL_CHECK(ptr, str)       \
    if (NULL == ptr) {                      \
        DEBUG_ERR_OUTPUT(str);              \
        ttc_release_plan(new_plan);         \
        return NULL;                        \
    }

#define TTC_PLAN_SET_ARRAY(dest, src, type, str)                \
    DEBUG_INFO_OUTPUT("Setting arg: " str);    \
    dest = (type *)malloc(sizeof(type) * param->dim);           \
    TTC_PLAN_NULL_CHECK(dest, strerror(errno));                 \
    memcpy(dest, src, sizeof(type) * param->dim)


ttc_plan_s *
ttc_create_plan(
        const ttc_opt_s     *options,
        const ttc_param_s   *param
        ) {
    DEBUG_SET_NAMESPACE("ttc_create_plan");
    DEBUG_INFO_OUTPUT("Creating a new plan.");
    // Parameter check
    if (NULL == options) {
        DEBUG_ERR_OUTPUT("options is not well initialized.");
        return NULL;
    }
    if (NULL == param || NULL == param->perm || NULL == param->size) {
        DEBUG_ERR_OUTPUT("param is not well initialized.");
        return NULL;
    }


    // Create new plan
    DEBUG_INFO_OUTPUT("Allocating memory for the new plan.");
    ttc_plan_s *new_plan = (ttc_plan_s *)malloc(sizeof(ttc_plan_s));
    if (NULL == new_plan) {
        DEBUG_ERR_OUTPUT(strerror(errno));
        return NULL;
    }

    // Pre-initialize in case internal errors that could cause memory leak.
    DEBUG_INFO_OUTPUT("Initializing members in ttc_plan_s.");
    new_plan->param.lda         = NULL;
    new_plan->param.ldb         = NULL;
    new_plan->param.perm        = NULL;
    new_plan->param.size        = NULL;
    new_plan->param.loop_perm   = NULL;
    new_plan->dlhandler         = NULL;
    new_plan->fn                = NULL;
    new_plan->fn_cuda           = NULL;
    new_plan->next              = NULL;

    // Initialize member: param.alpha
    if (TTC_TYPE_DEFAULT == options->datatype
        || TTC_TYPE_S == options->datatype
        || TTC_TYPE_C == options->datatype
        || TTC_TYPE_SD == options->datatype
        || TTC_TYPE_CZ == options->datatype
       ) {
        DEBUG_INFO_OUTPUT("Setting ttc_plan_s::param::alpha with single "
                "precision.");
        new_plan->param.alpha.s = param->alpha.s <= 0 ? 1.0 : param->alpha.s;
    }
    else {
        DEBUG_INFO_OUTPUT("Setting ttc_plan_s::param::alpha with double "
                "precision.");
        new_plan->param.alpha.d = param->alpha.d <= 0 ? 1.0 : param->alpha.d;
    }

    // Initialize member: param.beta
    if (TTC_TYPE_DEFAULT == options->datatype
        || TTC_TYPE_S == options->datatype
        || TTC_TYPE_C == options->datatype
        || TTC_TYPE_DS == options->datatype
        || TTC_TYPE_ZC == options->datatype
       ) {
        DEBUG_INFO_OUTPUT("Setting ttc_plan_s::param::beta"
                "with single precision.");
        new_plan->param.beta.s = param->beta.s <= 0 ? 0.0 : param->beta.s;
    }
    else {
        DEBUG_INFO_OUTPUT("Setting ttc_plan_s::param::beta"
                "with double precision.");
        new_plan->param.beta.d = param->beta.d <= 0 ? 0.0 : param->beta.d;
    }

    // Initialize member: param.lda
    if (NULL != param->lda) {
        TTC_PLAN_SET_ARRAY(new_plan->param.lda, param->lda, int32_t,
                "ttc_plan_s::param::lda");
    }

    // Initialize member: param.ldb
    if (NULL != param->ldb) {
        TTC_PLAN_SET_ARRAY(new_plan->param.ldb, param->ldb, int32_t,
                "ttc_plan_s::param::ldb");
    }

    // Initialize members: param.perm, param.size
    TTC_PLAN_SET_ARRAY(new_plan->param.perm, param->perm, uint32_t,
            "ttc_plan_s::param::perm");
    TTC_PLAN_SET_ARRAY(new_plan->param.size, param->size, uint32_t,
            "ttc_plan_s::param::size");

    // Initialize member: param.loop_perm
    if (NULL != param->loop_perm) {
        TTC_PLAN_SET_ARRAY(new_plan->param.loop_perm, param->loop_perm,
                uint32_t, "ttc_plan_s::param::loop_perm");
    }

    // Initialize member: param.dim
    DEBUG_INFO_OUTPUT("Setting ttc_plan_s::param::dim.");
    new_plan->param.dim = param->dim;

    // Here: Database support will be involved in the future.
    //
    //
    // Initialize member: dlhandler
    // Create ttc process
    DEBUG_INFO_OUTPUT("Parent process: Creating pipe.");
    int32_t ttc_pipe[2];
    if (0 != pipe(ttc_pipe)) {
        DEBUG_ERR_OUTPUT("Parent process: "
                "Cannot create pipe.");
        ttc_release_plan(new_plan);
        return NULL;
    }

    DEBUG_INFO_OUTPUT("Parent process: Forking.");
    int32_t pid = fork();
    if (-1 == pid) {
        DEBUG_ERR_OUTPUT("Parent process: Fork failed.");
        ttc_release_plan(new_plan);
        return NULL;
    }
    else if (0 == pid) {
        DEBUG_INFO_OUTPUT("Child process: Child starts.");

        // Build arguments
        DEBUG_INFO_OUTPUT("Child process: Generating command line arguments.");
        char **argv = ttc_gen_arg(options, param);
        if (NULL == argv) {
            DEBUG_ERR_OUTPUT("Child process: Cannot parse arguments.");
            exit(-1);
        }

#ifdef TENSOR_DEBUG
        char **debug_ptr = argv;
        char cmd_buf[TTC_GEN_BUF_SIZE];
        cmd_buf[0] = '\0';
        for (; NULL != *debug_ptr; ++debug_ptr)
            sprintf(cmd_buf, "%s%s ", cmd_buf, *debug_ptr);
        DEBUG_INFO_OUTPUT(cmd_buf);
#endif


        // Redirect standard output
        close(ttc_pipe[TTC_PIPE_RD]);
        if (-1 == dup2(ttc_pipe[TTC_PIPE_WR], STDOUT_FILENO)) {
            DEBUG_ERR_OUTPUT("Child process: Cannot redirect IO.");
            exit(-1);
        }

        // Execute TTC command
        execvp(argv[0], argv);

        // If error happens
        DEBUG_ERR_OUTPUT("Child process: "
                "Child process failed to execute TTC command.");
        ttc_release_arg(argv);
        exit(-1);
    }
    else {
        DEBUG_INFO_OUTPUT("Parent process: "
                "Shutting down output pipe.");
        close(ttc_pipe[TTC_PIPE_WR]);
    }

    // Check results
    DEBUG_INFO_OUTPUT("Parent process: "
            "Waiting for child process.");
    int32_t ttc_result;
    waitpid(pid, &ttc_result, 0);
    DEBUG_INFO_OUTPUT("Parent process: "
            "Checking result of child process.");
    if (!WIFEXITED(ttc_result)) {
        DEBUG_ERR_OUTPUT("TTC exits abnormally.");
        ttc_release_plan(new_plan);
        return NULL;
    }
    else if (0 != WEXITSTATUS(ttc_result)) {
        DEBUG_ERR_OUTPUT("TTC exit code indicates error.");
        ttc_release_plan(new_plan);
        return NULL;
    }

    // Locate header file
    char seek_buf[TTC_GEN_BUF_SIZE];
    DEBUG_INFO_OUTPUT("Locating the header file name.");
    if (0 != ttc_locate_header(ttc_pipe[TTC_PIPE_RD], seek_buf)) {
        DEBUG_ERR_OUTPUT("Cannot locate header file name.");
        ttc_release_plan(new_plan);
        return NULL;
    }

    // Close input pipe
    DEBUG_INFO_OUTPUT("Shutting down input pipe.");
    close(ttc_pipe[TTC_PIPE_RD]);

    // Generating and loading shared library
    DEBUG_INFO_OUTPUT("Generating code.");
    DEBUG_INFO_OUTPUT("Change directory to " TTC_DIR_GEN_CODE);
    chdir(TTC_DIR_GEN_CODE);

    char target_prefix[TTC_GEN_BUF_SIZE];
    char target_suffix[TTC_GEN_BUF_SIZE];
    int ret
        = ttc_gen_code(options, param, seek_buf, target_prefix, target_suffix);
    DEBUG_SET_NAMESPACE("ttc_create_plan");
    if (0 != ret) {
        DEBUG_ERR_OUTPUT("Cannot generate code.");
        DEBUG_INFO_OUTPUT("Change directory to " TTC_DIR_BACK_GEN);
        chdir(TTC_DIR_BACK_GEN);
        ttc_release_plan(new_plan);
        return NULL;
    }

    DEBUG_INFO_OUTPUT("Compiling code.");
    new_plan->dlhandler = ttc_gen_lib(options, target_prefix, target_suffix);
    DEBUG_SET_NAMESPACE("ttc_create_plan");
    if(NULL == new_plan->dlhandler) {
        DEBUG_ERR_OUTPUT("Cannot generate shared library.");
        DEBUG_INFO_OUTPUT("Change directory to " TTC_DIR_BACK_GEN);
        chdir(TTC_DIR_BACK_GEN);
        ttc_release_plan(new_plan);
        return NULL;
    }
    DEBUG_INFO_OUTPUT("Change directory to " TTC_DIR_BACK_GEN);
    chdir(TTC_DIR_BACK_GEN);

    if (TTC_ARCH_CUDA == options->arch) {
        DEBUG_INFO_OUTPUT("Locating function symbol (CUDA): " TTC_FUNC_SYMBOL);
        new_plan->fn_cuda = dlsym(new_plan->dlhandler, TTC_FUNC_SYMBOL);
        TTC_PLAN_NULL_CHECK(new_plan->fn_cuda, "Cannot locate symbol: "
                TTC_FUNC_SYMBOL);
    }
    else {
        DEBUG_INFO_OUTPUT("Locating function symbol: " TTC_FUNC_SYMBOL);
        new_plan->fn = dlsym(new_plan->dlhandler, TTC_FUNC_SYMBOL);
        TTC_PLAN_NULL_CHECK(new_plan->fn, "Cannot locate symbol: "
                TTC_FUNC_SYMBOL);
    }

    new_plan->next = NULL;

    return new_plan;
}


int32_t
ttc_release_plan(
        ttc_plan_s  *plan
        ) {
    DEBUG_SET_NAMESPACE("ttc_release_plan");
    DEBUG_INFO_OUTPUT("Releasing ttc_plan_s object.");
    // Parameter check
    if (NULL == plan)
        return 0;

    // Release resource
    // Release member: param
    DEBUG_INFO_OUTPUT("Releasing ttc_plan_s::param.");
    free(plan->param.lda);
    free(plan->param.ldb);
    free(plan->param.perm);
    free(plan->param.size);
    free(plan->param.loop_perm);

    // Release member: dlhandler
    DEBUG_INFO_OUTPUT("Releasing ttc_plan_s::dlhandler.");
    if (NULL != plan->dlhandler && 0 != dlclose(plan->dlhandler)) {
        DEBUG_ERR_OUTPUT("Cannot close dlhandler.");
        return -1;
    }

    DEBUG_INFO_OUTPUT("Releasing plan object.");
    free(plan);

    return 0;
}


int32_t
ttc_exec_plan(
        const ttc_plan_s    *plan,
        const void          *input,
        void                *result
        ) {
    DEBUG_SET_NAMESPACE("ttc_exec_plan");
    DEBUG_INFO_OUTPUT("Executing plan.");
    // Parameter check
    if (NULL == plan || NULL == plan->fn) {
        DEBUG_ERR_OUTPUT("plan is not well initialized.");
        return -1;
    }
    if (NULL == input) {
        DEBUG_ERR_OUTPUT("input is not well initialized.");
        return -1;
    }
    if (NULL == result) {
        DEBUG_ERR_OUTPUT("result is not well initialized.");
        return -1;
    }

    // Execute plan
    DEBUG_INFO_OUTPUT("Calling ttc_plan_s::fn.");
    plan->fn(input, result, &plan->param.alpha, &plan->param.beta,
            plan->param.lda, plan->param.ldb);

    return 0;
}


int32_t
ttc_exec_plan_cuda(
        const ttc_plan_s    *plan,
        const void          *input,
        void                *result
        ) {
    DEBUG_SET_NAMESPACE("ttc_exec_plan_cuda");
    DEBUG_INFO_OUTPUT("Executing plan CUDA.");
    // Parameter check
    if (NULL == plan || NULL == plan->fn_cuda) {
        DEBUG_ERR_OUTPUT("plan is not well initialized.");
        return -1;
    }
    if (NULL == input) {
        DEBUG_ERR_OUTPUT("input is not well initialized.");
        return -1;
    }
    if (NULL == result) {
        DEBUG_ERR_OUTPUT("result is not well initialized.");
        return -1;
    }

    // Execute plan
    DEBUG_INFO_OUTPUT("Calling ttc_plan_s::fn_cuda.");
    int32_t total_size = 1;
    int32_t idx = plan->param.dim - 1;
    for (; idx >= 0; total_size *= plan->param.size[idx], --idx);
    plan->fn_cuda(input, result, &plan->param.alpha, &plan->param.beta,
            plan->param.lda, plan->param.ldb, plan->param.size, total_size);

    return 0;
}


char **
ttc_gen_arg(
        const ttc_opt_s     *options,
        const ttc_param_s   *param
        ) {
    DEBUG_SET_NAMESPACE("ttc_gen_arg");
    DEBUG_INFO_OUTPUT("Generating ttc command line arguments.");
    // Check parameter
    if (NULL == options) {
        DEBUG_ERR_OUTPUT("options is not initialized.");
        return NULL;
    }
    if (NULL == param) {
        DEBUG_ERR_OUTPUT("param is not initialized.");
        return NULL;
    }


    // Count argument
    DEBUG_INFO_OUTPUT("Counting arguments numbers.");
    int32_t arg_count = ttc_count_arg(options, param);
    DEBUG_SET_NAMESPACE("ttc_gen_arg");

    // Create arguments
    char arg_buf[TTC_GEN_BUF_SIZE];

    DEBUG_INFO_OUTPUT("Allocating memory for char *argv[].");
    char **argv = (char **)malloc(sizeof(char *) * arg_count);
    TTC_SET_ARG_CHECK_MALLOC(argv);
    int32_t init_idx;
    for (init_idx = 0; init_idx < arg_count; ++init_idx)
        argv[init_idx] = NULL;

    // Set executable
    DEBUG_INFO_OUTPUT("Setting ttc executable.");
    argv[0] = (char *)malloc(sizeof(char) * sizeof(TTC_EXECUTABLE));
    TTC_SET_ARG_CHECK_MALLOC(argv[0]);
    memcpy(argv[0], TTC_EXECUTABLE, sizeof(TTC_EXECUTABLE));

    uint32_t arg_idx = 1;
    // Set max_impl, num_threads and pref_dist
    TTC_SET_ARG_SINGLE(options->max_impl, TTC_ARG_MAX_IMPL, "%s%d");
    TTC_SET_ARG_SINGLE(options->num_threads, TTC_ARG_NUM_THREADS, "%s%d");

    TTC_SET_ARG_ARRAY(options->pref_dist, options->pref_dist_len,
            TTC_ARG_PREF_DIST);

    // Set blockings
    if (NULL != options->blockings) {
        DEBUG_INFO_OUTPUT("Setting blockings.");

        // Leave space for commas, x sign and null termination.
        int32_t idx, val_len, str_len = sizeof(TTC_ARG_BLOCKINGS) - 1;
        char *parse_ptr = arg_buf + str_len;
        memcpy(arg_buf, TTC_ARG_BLOCKINGS, str_len);

        for (idx = 0; idx < options->blockings_len; idx += 2) {
            val_len = sprintf(parse_ptr, "%dx%d,",
                    options->blockings[idx], options->blockings[idx + 1]);
            str_len += val_len; parse_ptr += val_len;
        }

        argv[arg_idx] = (char *)malloc(sizeof(char) * str_len);
        TTC_SET_ARG_CHECK_MALLOC(argv[arg_idx]);
        memcpy(argv[arg_idx], arg_buf, str_len);
        argv[arg_idx][str_len - 1] = '\0';

        ++arg_idx;
    }

    // Set affinity
    if (NULL != options->affinity) {
        DEBUG_INFO_OUTPUT("Setting affinity.");

        int32_t str_len = 1 + sprintf(arg_buf, "%s%s",
                TTC_ARG_AFFINITY, options->affinity);
        argv[arg_idx] = (char *)malloc(sizeof(char) * str_len);
        TTC_SET_ARG_CHECK_MALLOC(argv[arg_idx]);
        memcpy(argv[arg_idx], arg_buf, str_len);
        ++arg_idx;
    }

    // Enum members: compiler, datatype, arch and tb
    int32_t arg_len;
    switch (options->compiler) {
    case TTC_CMP_DEFAULT:
        DEBUG_INFO_OUTPUT("Using default compiler.");
        break;
    case TTC_CMP_GXX:
        TTC_SET_ARG_ENUM(TTC_ARG_COMPILER, TTC_ARG_CMP_GXX);
        ++arg_idx;
        break;
    case TTC_CMP_ICPC:
        TTC_SET_ARG_ENUM(TTC_ARG_COMPILER, TTC_ARG_CMP_ICPC);
        ++arg_idx;
        break;
    case TTC_CMP_IBM:
        DEBUG_ERR_OUTPUT("IBM compiler is not supported.");
        ttc_release_arg(argv);
        return NULL;
        TTC_SET_ARG_ENUM(TTC_ARG_COMPILER, TTC_ARG_CMP_IBM);
        ++arg_idx;
        break;
    case TTC_CMP_NVCC:
        TTC_SET_ARG_ENUM(TTC_ARG_COMPILER, TTC_ARG_CMP_NVCC);
        ++arg_idx;
        break;
    default:
        DEBUG_WARN_OUTPUT("Unknown compiler, using default.");
        break;
    }

    switch (options->datatype) {
    case TTC_TYPE_DEFAULT:
        DEBUG_INFO_OUTPUT("Using default datatype.");
        break;
    case TTC_TYPE_S:
        TTC_SET_ARG_ENUM(TTC_ARG_DATATYPE, TTC_ARG_TYPE_S);
        ++arg_idx;
        break;
    case TTC_TYPE_D:
        TTC_SET_ARG_ENUM(TTC_ARG_DATATYPE, TTC_ARG_TYPE_D);
        ++arg_idx;
        break;
    case TTC_TYPE_C:
        TTC_SET_ARG_ENUM(TTC_ARG_DATATYPE, TTC_ARG_TYPE_C);
        ++arg_idx;
        break;
    case TTC_TYPE_Z:
        TTC_SET_ARG_ENUM(TTC_ARG_DATATYPE, TTC_ARG_TYPE_Z);
        ++arg_idx;
        break;
    case TTC_TYPE_SD:
        TTC_SET_ARG_ENUM(TTC_ARG_DATATYPE, TTC_ARG_TYPE_SD);
        ++arg_idx;
        break;
    case TTC_TYPE_DS:
        TTC_SET_ARG_ENUM(TTC_ARG_DATATYPE, TTC_ARG_TYPE_DS);
        ++arg_idx;
        break;
    case TTC_TYPE_CZ:
        TTC_SET_ARG_ENUM(TTC_ARG_DATATYPE, TTC_ARG_TYPE_CZ);
        ++arg_idx;
        break;
    case TTC_TYPE_ZC:
        TTC_SET_ARG_ENUM(TTC_ARG_DATATYPE, TTC_ARG_TYPE_ZC);
        ++arg_idx;
        break;
    default:
        DEBUG_WARN_OUTPUT("Unknown datatype, using default.");
        break;
    }

    switch (options->arch) {
    case TTC_ARCH_DEFAULT:
        DEBUG_INFO_OUTPUT("Setting default architecture.");
        break;
    case TTC_ARCH_AVX:
        DEBUG_INFO_OUTPUT("Setting default architecture.");
        TTC_SET_ARG_ENUM(TTC_ARG_ARCH, TTC_ARG_ARCH_AVX);
        ++arg_idx;
        break;
    case TTC_ARCH_POWER:
        DEBUG_ERR_OUTPUT("POWER arch is not supported.");
        ttc_release_arg(argv);
        return NULL;
        TTC_SET_ARG_ENUM(TTC_ARG_ARCH, TTC_ARG_ARCH_POWER);
        ++arg_idx;
        break;
    case TTC_ARCH_AVX512:
        DEBUG_INFO_OUTPUT("Setting default architecture.");
        TTC_SET_ARG_ENUM(TTC_ARG_ARCH, TTC_ARG_ARCH_AVX512);
        ++arg_idx;
        break;
    case TTC_ARCH_KNC:
        DEBUG_INFO_OUTPUT("Setting default architecture.");
        TTC_SET_ARG_ENUM(TTC_ARG_ARCH, TTC_ARG_ARCH_KNC);
        ++arg_idx;
        break;
    case TTC_ARCH_CUDA:
        if (NULL == param->lda || NULL == param->ldb) {
            DEBUG_ERR_OUTPUT("When using CUDA, lda and ldb must be specified.");
            ttc_release_arg(argv);
            return NULL;
        }
        TTC_SET_ARG_ENUM(TTC_ARG_ARCH, TTC_ARG_ARCH_CUDA);
        ++arg_idx;
        break;
    default:
        DEBUG_WARN_OUTPUT("Unknown architecture, using default.");
        break;
    }

    switch (options->tb) {
    case TTC_TB_DEFAULT:
        DEBUG_INFO_OUTPUT("Using default thread block size.");
        break;
    case TTC_TB_128:
        TTC_SET_ARG_ENUM(TTC_ARG_TB, TTC_ARG_TB_128);
        ++arg_idx;
        break;
    case TTC_TB_256:
        TTC_SET_ARG_ENUM(TTC_ARG_TB, TTC_ARG_TB_256);
        ++arg_idx;
        break;
    case TTC_TB_512:
        TTC_SET_ARG_ENUM(TTC_ARG_TB, TTC_ARG_TB_512);
        ++arg_idx;
        break;
    default:
        DEBUG_WARN_OUTPUT("Unknown thread block, using default.");
        break;
    }

    // Set status
    TTC_SET_ARG_STATUS(TTC_ARG_KEEP, TTC_KEEP);
    TTC_SET_ARG_STATUS(TTC_ARG_IGN_DB, TTC_IGN_DB);
    TTC_SET_ARG_STATUS(TTC_ARG_NO_ALIGN, TTC_NO_ALIGN);
    TTC_SET_ARG_STATUS(TTC_ARG_NO_VEC, TTC_NO_VEC);
    TTC_SET_ARG_STATUS(TTC_ARG_NO_TEST, TTC_NO_TEST);
    TTC_SET_ARG_STATUS(TTC_ARG_USE_STM_STORE, TTC_USE_STM_STORE);
    TTC_SET_ARG_STATUS(TTC_ARG_HOT_A, TTC_HOT_A);
    TTC_SET_ARG_STATUS(TTC_ARG_HOT_B, TTC_HOT_B);

    // Param
    // Set size, perm, loop_perm, lda and ldb
    TTC_SET_ARG_ARRAY(param->size, param->dim, TTC_ARG_SIZE);
    TTC_SET_ARG_ARRAY(param->perm, param->dim, TTC_ARG_PERM);
    TTC_SET_ARG_ARRAY(param->loop_perm, param->dim, TTC_ARG_LOOP_PERM);
    TTC_SET_ARG_ARRAY(param->lda, param->dim, TTC_ARG_LDA);
    TTC_SET_ARG_ARRAY(param->ldb, param->dim, TTC_ARG_LDB);

    // Set beta
    if (TTC_TYPE_DEFAULT == options->datatype
        || TTC_TYPE_S == options->datatype
        || TTC_TYPE_C == options->datatype
        || TTC_TYPE_DS == options->datatype
        || TTC_TYPE_ZC == options->datatype
        ) {
        TTC_SET_ARG_SINGLE(param->beta.s, TTC_ARG_BETA, "%s%f");
    }
    else {
        TTC_SET_ARG_SINGLE(param->beta.d, TTC_ARG_BETA, "%s%f");
    }


    // The last argument will be set to NULL.
    argv[arg_idx] = NULL;

    return argv;
}


int32_t
ttc_count_arg(
        const ttc_opt_s     *options,
        const ttc_param_s   *param
        ) {
    DEBUG_SET_NAMESPACE("ttc_count_arg");
    DEBUG_INFO_OUTPUT("Counting arguments to be generated.");
    // Check parameter
    if (NULL == options) {
        DEBUG_ERR_OUTPUT("options is not initialized.");
        return -1;
    }
    if (NULL == param) {
        DEBUG_ERR_OUTPUT("param is not initialized.");
        return -1;
    }

    // Count options
    // Use 4 for TTC executable, perm, size and termination null.
    int32_t arg_count = 4;

    DEBUG_INFO_OUTPUT("Counting ttc_handler_s::options.");
    if (0 != options->max_impl)
        ++arg_count;
    if (0 != options->num_threads)
        ++arg_count;
    if (NULL != options->pref_dist)
        ++arg_count;
    if (NULL != options->blockings)
        ++arg_count;
    if (NULL != options->affinity)
        ++arg_count;
    if (TTC_CMP_DEFAULT != options->compiler)
        ++arg_count;
    if (TTC_TYPE_DEFAULT != options->datatype)
        ++arg_count;
    if (TTC_ARCH_DEFAULT != options->arch)
        ++arg_count;
    if (TTC_TB_DEFAULT != options->tb)
        ++arg_count;

    // Count bit 1 in status
    int32_t status_count = 0;
    uint32_t target = options->status;
    while (0 != target) {
        ++status_count;
        target &= target - 1;
    }
    arg_count += status_count;

    // Param
    // alpha will be set to 1.0, so it will not be accounted.
    DEBUG_INFO_OUTPUT("Counting ttc_plan_s::param.");
    if (TTC_TYPE_DEFAULT == options->datatype
        || TTC_TYPE_S == options->datatype
        || TTC_TYPE_C == options->datatype
        || TTC_TYPE_DS == options->datatype
        || TTC_TYPE_ZC == options->datatype
        ) {
        if (0.0 != param->beta.s)
            ++arg_count;
    }
    else {
        if (0.0 != param->beta.d)
            ++arg_count;
    }
    if (NULL != param->lda)
        ++arg_count;
    if (NULL != param->ldb)
        ++arg_count;
    if (NULL != param->loop_perm)
        ++arg_count;

    return arg_count;
}


void
ttc_release_arg(
        char **argv
        ) {
    DEBUG_SET_NAMESPACE("ttc_release_arg");
    DEBUG_INFO_OUTPUT("Releasing char *argv[].");
    // Parameter check
    if (NULL == argv)
        return;

    char **release_ptr = argv;
    for (; NULL != *release_ptr; ++release_ptr)
        free(*release_ptr);

    free(argv);
}


int32_t
ttc_locate_header(
        int32_t pipe_fileno,
        char    seek_buf[]
        ) {
    DEBUG_SET_NAMESPACE("ttc_locate_header");
    DEBUG_INFO_OUTPUT("Locating the header file name.");
    // Parameter check
    if (pipe_fileno < 0) {
        DEBUG_ERR_OUTPUT("Parameter pipe_fileno is not initialized.");
        return -1;
    }
    if (NULL == seek_buf) {
        DEBUG_ERR_OUTPUT("Parameter seek_buf is not initialized.");
        return -1;
    }

    // Pointer for seeking the target.
    char *store_ptr = seek_buf;
    // Target indicator's length.
    int32_t include_size = sizeof(INCLUDE_STR) - 1;

    // Locating
    while (read(pipe_fileno, seek_buf, 1) > 0) {
        if ('#' == *seek_buf) {
            DEBUG_INFO_OUTPUT("# sign found.");
            // If find the # sign which is the head of target.
            // Read target indicator's length and check if it's indicator.
            read(pipe_fileno, seek_buf, include_size);
            if (0 == strncmp(seek_buf, INCLUDE_STR, include_size)) {
                DEBUG_INFO_OUTPUT("include found.");
                // If the indicator is found, begin to record target.
                bool record_flag = false;
                while (read(pipe_fileno, store_ptr, 1) > 0) {
                    // This piece of code is used for cut the string and leave
                    // only the header file name (without quotation marks).
                    if (record_flag)
                        if ('"' != *store_ptr)
                            ++store_ptr;
                        else
                            break;
                    else
                        if ('"' == *store_ptr)
                            record_flag = true;
                }
                // Set null at the end of the record.
                *store_ptr = '\0';
                break;
            }
        }
    }

    return seek_buf == store_ptr ? -1 : 0;
}


int32_t
ttc_gen_code(
        const ttc_opt_s     *options,
        const ttc_param_s   *param,
        const char          *header_file_name,
        char                *target_prefix,
        char                *target_suffix
        ) {
    DEBUG_SET_NAMESPACE("ttc_gen_code");
    DEBUG_INFO_OUTPUT("Generating shared library.");
    // Parameter check
    if (NULL == options) {
        DEBUG_ERR_OUTPUT("Parameter options is not initialized.");
        return -1;
    }
    if (NULL == param) {
        DEBUG_ERR_OUTPUT("Parameter param is not initialized.");
        return -1;
    }
    if (NULL == header_file_name) {
        DEBUG_ERR_OUTPUT("Parameter header_file_name is not initialized.");
        return -1;
    }
    if (NULL == target_prefix) {
        DEBUG_ERR_OUTPUT("Parameter target_prefix is not initialized.");
        return -1;
    }
    if (NULL == target_suffix) {
        DEBUG_ERR_OUTPUT("Parameter target_suffix is not initialized.");
        return -1;
    }

    // From "file_name.h" to "file_name"
    int32_t header_file_name_len = strlen(header_file_name);

    strcpy(target_prefix, header_file_name);
    target_prefix[header_file_name_len - 2] = '\0';
    if (TTC_ARCH_DEFAULT == options->arch || TTC_ARCH_AVX == options->arch) {
        DEBUG_INFO_OUTPUT("Generating C++ code.");
        strcpy(target_suffix, "cpp");
        if (0 != ttc_gen_code_avx(options, param, target_prefix, target_suffix))
        {
            DEBUG_SET_NAMESPACE("ttc_gen_code");
            DEBUG_ERR_OUTPUT("Generating C++ code failed.");
            return -1;
        }
    }
    else if (TTC_ARCH_CUDA == options->arch) {
        DEBUG_INFO_OUTPUT("Generating CUDA code.");
        strcpy(target_suffix, "cu");
        if (0
            != ttc_gen_code_cuda(options, param, target_prefix, target_suffix))
        {
            DEBUG_SET_NAMESPACE("ttc_gen_code");
            DEBUG_ERR_OUTPUT("Generating C++ code failed.");
            return -1;
        }
    }
    else {
        DEBUG_ERR_OUTPUT("The architecture is currently not supported.");
        return -1;
    }

    return 0;
}


int32_t
ttc_gen_code_avx(
        const ttc_opt_s     *options,
        const ttc_param_s   *param,
        const char          *target_prefix,
        const char          *target_suffix
        ) {
    DEBUG_SET_NAMESPACE("ttc_gen_code_avx");
    DEBUG_INFO_OUTPUT("Generating C++ code (.cpp file).");
    // Parameter check
    if (NULL == options) {
        DEBUG_ERR_OUTPUT("Parameter options is not initialized.");
        return -1;
    }
    if (NULL == param) {
        DEBUG_ERR_OUTPUT("Parameter param is not initialized.");
        return -1;
    }
    if (NULL == target_prefix) {
        DEBUG_ERR_OUTPUT("Parameter target_prefix is not initialized.");
        return -1;
    }
    if (NULL == target_suffix) {
        DEBUG_ERR_OUTPUT("Parameter target_suffix is not initialized.");
        return -1;
    }

    char gen_buf[TTC_GEN_BUF_SIZE];
    sprintf(gen_buf, "%s.%s", target_prefix, target_suffix);
    FILE *target_file = fopen(gen_buf, "wc");
    if (NULL == target_file) {
        DEBUG_ERR_OUTPUT("Cannot create .cpp file.");
        return -1;
    }

    DEBUG_INFO_OUTPUT("Generating code.");
    // include MACRO
    fprintf(target_file, "#include \"%s.h\"\n", target_prefix);
    if (TTC_TYPE_C == options->datatype || TTC_TYPE_Z == options->datatype
        || TTC_TYPE_CZ == options->datatype || TTC_TYPE_ZC == options->datatype)
        fprintf(target_file, "#include <complex.h>\n");

    // Function declaration
    fprintf(target_file, "\nextern \"C\" {\n"
            "    void transpose(const void *, void *,\n"
            "        const void *, const void *,\n"
            "        const int *, const int *);\n}\n\n");

    // MACRO definition
    if (ttc_gen_type_macro(options, param, target_file)) {
        DEBUG_ERR_OUTPUT("Unknown data type.");
        fclose(target_file);
        return -1;
    }

    // Function definition
    fprintf(target_file, "void transpose(const void *input, void *result,\n"
            "const void *alpha, const void *beta,\n"
            "const int *lda, const int *ldb) {\n"
            "    %s<", target_prefix);

    int32_t dim_idx;
    for (dim_idx = 0; dim_idx < param->dim; ++dim_idx) {
        fprintf(target_file, "%d", param->size[dim_idx]);
        if (param->dim - 1 != dim_idx)
            fprintf(target_file, ", ");
    }
    fprintf(target_file, ">((const TENSOR_IN_T *)input,\n"
            "        (TENSOR_OUT_T *)result,\n"
            "        *(ALPHA_T *)alpha, BETA_PARAM\n"
            "        lda, ldb);\n}");

    DEBUG_INFO_OUTPUT(".cpp file generation finished.");
    fclose(target_file);

    return 0;
}


int32_t
ttc_gen_code_cuda(
        const ttc_opt_s     *options,
        const ttc_param_s   *param,
        const char          *target_prefix,
        const char          *target_suffix
        ) {
    DEBUG_SET_NAMESPACE("ttc_gen_code_cuda");
    DEBUG_INFO_OUTPUT("Generating CUDA code (.cu file).");
    // Parameter check
    if (NULL == options) {
        DEBUG_ERR_OUTPUT("Parameter options is not initialized.");
        return -1;
    }
    if (NULL == target_prefix) {
        DEBUG_ERR_OUTPUT("Parameter target_prefix is not initialized.");
        return -1;
    }
    if (NULL == target_suffix) {
        DEBUG_ERR_OUTPUT("Parameter target_suffix is not initialized.");
        return -1;
    }

    // For storing the target file's name, it is also used for generating the
    // transpose function's name in --dataType=zc case.
    char gen_buf[TTC_GEN_BUF_SIZE];
    sprintf(gen_buf, "lib%s.%s", target_prefix, target_suffix);
    FILE *target_file = fopen(gen_buf, "wc");
    if (NULL == target_file) {
        DEBUG_ERR_OUTPUT("Cannot create .cu file.");
        return -1;
    }

    DEBUG_INFO_OUTPUT("Generating code.");
    // include MACRO
    // fprintf(target_file, "#include \"%s.h\"\n\n", target_prefix);
    fprintf(target_file, "#include <cuda_runtime.h>\n");
    if (TTC_TYPE_C == options->datatype || TTC_TYPE_Z == options->datatype
        || TTC_TYPE_CZ == options->datatype || TTC_TYPE_ZC == options->datatype)
        fprintf(target_file, "#include <cuComplex.h>\n");

    // MACRO definition
    fprintf(target_file, "#define CHECK_CUDA \\\n"
            "    if ((err = cudaGetLastError()) != cudaSuccess) { \\\n"
            "        cudaFree(dev_input); \\\n"
            "        cudaFree(dev_result); \\\n"
            "        return err; \\\n"
            "    }\n\n");

    if (0 != ttc_gen_type_macro(options, param, target_file)) {
        DEBUG_ERR_OUTPUT("Unknown data type.");
        fclose(target_file);
        return -1;
    }

    // The transpose function's name is inconsistent with the file name.
    strcpy(gen_buf, target_prefix);
    if (TTC_TYPE_ZC == options->datatype)
        gen_buf[1] = 'c';

    // Function declaration
    fprintf(target_file, "\nextern \"C\" {\n"
            "    int transpose(const void *, void *,\n"
            "            const void *, const void *,\n"
            "            const int *, const int *,\n"
            "            const int *, int);\n\n");
    // TTC generated .h file doesn't include cuComplex.h, which leads to type
    // undefined. Here using function delcaration instead of include .h file.
    fprintf(target_file, "    void %s(const TENSOR_IN_T *,\n"
            "        TENSOR_OUT_T *, const ALPHA_T, BETA_DECL\n"
            "        const int *, const int *, const int *);\n}\n\n", gen_buf);

    // Function definition
    fprintf(target_file, "int transpose(const void *input, void *result,\n"
            "        const void *alpha, const void *beta,\n"
            "        const int *lda, const int *ldb,\n"
            "        const int *size, int total_size) {\n"
            "    size_t input_size = total_size * sizeof(TENSOR_IN_T);\n"
            "    size_t result_size = total_size * sizeof(TENSOR_OUT_T);\n"
            "    TENSOR_IN_T *dev_input = NULL;\n"
            "    TENSOR_OUT_T *dev_result = NULL;\n"
            "    cudaError_t err;\n\n"
            "    cudaMalloc(&dev_input, input_size);\n"
            "    CHECK_CUDA;\n"
            "    cudaMalloc(&dev_result, result_size);\n"
            "    CHECK_CUDA;\n"
            "    cudaMemcpy(dev_input, input, "
            "input_size, cudaMemcpyHostToDevice);\n"
            "    CHECK_CUDA;\n\n"
            "    %s(dev_input, dev_result,\n"
            "        *(ALPHA_T *)alpha, BETA_PARAM size, lda, ldb);\n"
            "    cudaMemcpy(result, dev_result, result_size,\n"
            "        cudaMemcpyDeviceToHost);\n"
            "    CHECK_CUDA;\n\n"
            "    cudaFree(dev_input);\n"
            "    cudaFree(dev_result);\n\n"
            "    return 0;\n}", gen_buf);

    DEBUG_INFO_OUTPUT(".cu file generation finished.");
    fclose(target_file);

    return 0;
}


void *
ttc_gen_lib(
        const ttc_opt_s *options,
        const char      *target_prefix,
        const char      *target_suffix
        ) {
    DEBUG_SET_NAMESPACE("ttc_gen_lib");
    DEBUG_INFO_OUTPUT("Generating shared library.");
    // Parameter check
    if (NULL == options) {
        DEBUG_ERR_OUTPUT("Parameter options is not initialized.");
        return NULL;
    }
    if (NULL == target_prefix) {
        DEBUG_ERR_OUTPUT("Parameter target_prefix is not initialized.");
        return NULL;
    }
    if (NULL == target_suffix) {
        DEBUG_ERR_OUTPUT("Parameter target_suffix is not initialized.");
        return NULL;
    }

    char cmd_buf[TTC_GEN_BUF_SIZE];
    cmd_buf[0] = '\0';

    if (TTC_ARCH_DEFAULT == options->arch) {
        DEBUG_INFO_OUTPUT("Default architecture.");

        // Compiling
        sprintf(cmd_buf, "%s -o %s.o %s.%s", TTC_ARCH_DEF_CMPL,
                target_prefix, target_prefix, target_suffix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);

        // Linking
        sprintf(cmd_buf, "%s -o lib%s.so %s.o", TTC_ARCH_DEF_LINK,
                target_prefix, target_prefix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);
    }
    else if (TTC_ARCH_AVX == options->arch) {
        DEBUG_INFO_OUTPUT("AVX architecture.");
        const char *cmpl = TTC_CMP_GXX == options->compiler ?
            TTC_ARCH_AVX_GXX_CMPL : TTC_ARCH_AVX_ICPC_CMPL;
        const char *link = TTC_CMP_GXX == options->compiler ?
            TTC_ARCH_AVX_GXX_LINK : TTC_ARCH_AVX_ICPC_LINK;

        // Compiling
        sprintf(cmd_buf, "%s -o %s.o %s.%s", cmpl,
                target_prefix, target_prefix, target_suffix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);

        // Linking
        sprintf(cmd_buf, "%s -o lib%s.so %s.o", link,
                target_prefix, target_prefix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);
    }
    else if (TTC_ARCH_AVX512 == options->arch) {
        DEBUG_INFO_OUTPUT("AVX512 architecture.");

        // Compiling
        sprintf(cmd_buf, "%s -o %s.o %s.%s", TTC_ARCH_AVX512_CMPL,
                target_prefix, target_prefix, target_suffix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);

        // Linking
        sprintf(cmd_buf, "%s -o lib%s.so %s.o", TTC_ARCH_AVX512_LINK,
                target_prefix, target_prefix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);
    }
    else if (TTC_ARCH_KNC == options->arch) {
        DEBUG_INFO_OUTPUT("KNC architecture.");

        // Compiling
        sprintf(cmd_buf, "%s -o %s.o %s.%s", TTC_ARCH_KNC_CMPL,
                target_prefix, target_prefix, target_suffix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);

        // Linking
        sprintf(cmd_buf, "%s -o lib%s.so %s.o", TTC_ARCH_KNC_LINK,
                target_prefix, target_prefix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);
    }
    else if (TTC_ARCH_CUDA == options->arch) {
        DEBUG_INFO_OUTPUT("CUDA architecture.");

        // Compiling
        sprintf(cmd_buf, "%s -o %s.o %s.%s", TTC_ARCH_CUDA_CMPL,
                target_prefix, target_prefix, target_suffix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);
        sprintf(cmd_buf, "%s -o lib%s.o lib%s.%s", TTC_ARCH_CUDA_CMPL,
                target_prefix, target_prefix, target_suffix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);

        // Linking
        sprintf(cmd_buf, "%s -o lib%s.so %s.o lib%s.o", TTC_ARCH_CUDA_LINK,
                target_prefix, target_prefix, target_prefix);
        DEBUG_INFO_OUTPUT(cmd_buf);
        system(cmd_buf);
    }
    else {
        DEBUG_ERR_OUTPUT("The architecture is currently not supported.");
        return NULL;
    }

    // Loading shared library
    DEBUG_INFO_OUTPUT("Loading shared library.");
    sprintf(cmd_buf, "./lib%s.so", target_prefix);
    void *dlhandler = dlopen(cmd_buf, RTLD_NOW);
    if (NULL == dlhandler) {
        DEBUG_ERR_OUTPUT(dlerror());
        return NULL;
    }

    return dlhandler;
}


int32_t
ttc_gen_type_macro(
        const ttc_opt_s     *options,
        const ttc_param_s   *param,
        FILE                *target_file
        ) {
    const char *fcplx_type = TTC_ARCH_CUDA == options->arch
        ? "cuFloatComplex" : "float complex";
    const char *dcplx_type = TTC_ARCH_CUDA == options->arch
        ? "cuDoubleComplex" : "double complex";
    if (TTC_TYPE_DEFAULT == options->datatype
        || TTC_TYPE_S == options->datatype) {
        fprintf(target_file, "#define TENSOR_IN_T float\n"
                "#define TENSOR_OUT_T float\n"
                "#define ALPHA_T float\n");
    }
    else if (TTC_TYPE_D == options->datatype) {
        fprintf(target_file, "#define TENSOR_IN_T double\n"
                "#define TENSOR_OUT_T double\n"
                "#define ALPHA_T double\n");
    }
    else if (TTC_TYPE_C == options->datatype) {
        fprintf(target_file, "#define TENSOR_IN_T %s\n"
                "#define TENSOR_OUT_T %s\n"
                "#define ALPHA_T float\n",
                fcplx_type, fcplx_type);
    }
    else if (TTC_TYPE_Z == options->datatype) {
        fprintf(target_file, "#define TENSOR_IN_T %s\n"
                "#define TENSOR_OUT_T %s\n"
                "#define ALPHA_T double\n",
                dcplx_type, dcplx_type);
    }
    else if (TTC_TYPE_SD == options->datatype) {
        fprintf(target_file, "#define TENSOR_IN_T float\n"
                "#define TENSOR_OUT_T double\n"
                "#define ALPHA_T float\n");
    }
    else if (TTC_TYPE_DS == options->datatype) {
        fprintf(target_file, "#define TENSOR_IN_T double\n"
                "#define TENSOR_OUT_T float\n"
                "#define ALPHA_T double\n");
    }
    else if (TTC_TYPE_CZ == options->datatype) {
        fprintf(target_file, "#define TENSOR_IN_T %s\n"
                "#define TENSOR_OUT_T %s\n"
                "#define ALPHA_T float\n",
                fcplx_type, dcplx_type);
    }
    else if (TTC_TYPE_ZC == options->datatype)
        fprintf(target_file, "#define TENSOR_IN_T %s\n"
                "#define TENSOR_OUT_T %s\n"
                "#define ALPHA_T double\n",
                dcplx_type, fcplx_type);
    else
        return -1;

    if (TTC_TYPE_DEFAULT == options->datatype
        || TTC_TYPE_S == options->datatype || TTC_TYPE_C == options->datatype
        || TTC_TYPE_DS == options->datatype || TTC_TYPE_ZC == options->datatype
       ) {
        fprintf(target_file, "#define BETA_T float\n");
        if (0.0 != param->beta.s)
            fprintf(target_file, "#define BETA_PARAM *(BETA_T *)beta,\n"
                    "#define BETA_DECL const BETA_T,\n\n");
        else
            fprintf(target_file, "#define BETA_PARAM\n"
                    "#define BETA_DECL\n\n");
    }
    else {
        fprintf(target_file, "#define BETA_T double\n");
        if (0.0 != param->beta.d)
            fprintf(target_file, "#define BETA_PARAM *(BETA_T *)beta,\n"
                    "#define BETA_DECL const BETA_T,\n\n");
        else
            fprintf(target_file, "#define BETA_PARAM\n"
                    "#define BETA_DECL\n\n");
    }

    return 0;
}

