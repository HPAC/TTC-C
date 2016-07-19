#include "tensor_tc.h"

#include <stdlib.h>
#include <stdint.h>

#include <stdio.h>          // For sprintf
#include <string.h>

#include <errno.h>
#include <unistd.h>

#include "tensor_util.h"
#include "tensor_tc_util.h"



/* ======== Function definition ======== */

ttc_handler_s *
ttc_init(
        ) {
    DEBUG_SET_NAMESPACE("ttc_init");
    DEBUG_INFO_OUTPUT("Initializing ttc_handler_s object.");
    DEBUG_INFO_OUTPUT("Allocating memory for ttc_handler_s.");
    ttc_handler_s *handler = (ttc_handler_s *)malloc(sizeof(ttc_handler_s));
    if (NULL == handler) {
        DEBUG_ERR_OUTPUT(strerror(errno));
        return NULL;
    }

    // Initialize value of options and plans
    DEBUG_INFO_OUTPUT("Initializing members in ttc_handler_s.");
    handler->options.max_impl       = 0;
    handler->options.num_threads    = 0;
    handler->options.pref_dist      = NULL;
    handler->options.pref_dist_len  = 0;
    handler->options.blockings      = NULL;
    handler->options.blockings_len  = 0;
    handler->options.affinity       = NULL;
    handler->options.compiler       = TTC_CMP_DEFAULT;
    handler->options.datatype       = TTC_TYPE_DEFAULT;
    handler->options.arch           = TTC_ARCH_DEFAULT;
    handler->options.tb             = TTC_TB_DEFAULT;
    handler->options.status         = 0;
    handler->plans                  = NULL;

    return handler;
}


void
ttc_release(
        ttc_handler_s   *handler
        ) {
    DEBUG_SET_NAMESPACE("ttc_release");
    DEBUG_INFO_OUTPUT("Releasing ttc_handler_s object.");
    // Parameter check
    if (NULL == handler)
        return;

    // Release options
    DEBUG_INFO_OUTPUT("Releasing ttc_handler_s::options.");
    free(handler->options.pref_dist);
    free(handler->options.blockings);
    free(handler->options.affinity);

    // Release plans
    DEBUG_INFO_OUTPUT("Releasing ttc_handler_s::plans.");
    ttc_plan_s *plan_ptr = handler->plans;
    while (NULL != plan_ptr) {
        handler->plans = plan_ptr->next;
        // Release allocated memory in plan
        ttc_release_plan(plan_ptr);
        plan_ptr = handler->plans;
    }

    // Release handler
    DEBUG_INFO_OUTPUT("Releasing handler object.");
    free(handler);
}


int32_t
ttc_set_opt(
        ttc_handler_s   *handler,
        ttc_opt_type_e  type,
        const void      *value,
        uint32_t        length
        ) {
    DEBUG_SET_NAMESPACE("ttc_set_opt");
    DEBUG_INFO_OUTPUT("Setting option in ttc_handler_s object.");
    // Parameter check
    if (NULL == handler) {
        DEBUG_ERR_OUTPUT("handler is not initialized.");
        return -1;
    }
    if (NULL == value) {
        DEBUG_ERR_OUTPUT("opt_val is not initialized.");
        return -1;
    }


    // Set options
    switch (type) {
    case TTC_OPT_MAX_IMPL:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::max_impl.");

        handler->options.max_impl = *(uint32_t *)value;
        break;

    case TTC_OPT_NUM_THREADS:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::num_threads.");

        handler->options.num_threads = *(uint32_t *)value;
        break;

    case TTC_OPT_PREF_DIST:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::pref_dist.");

        handler->options.pref_dist
            = (uint32_t *)malloc(sizeof(uint32_t) * length);
        if (NULL == handler->options.pref_dist) {
            DEBUG_ERR_OUTPUT(strerror(errno));
            return errno;
        }
        handler->options.pref_dist_len = length;
        memcpy(handler->options.pref_dist, value, sizeof(uint32_t) * length);
        break;

    case TTC_OPT_BLOCKINGS:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::blockings.");

        handler->options.blockings
            = (uint32_t *)malloc(sizeof(uint32_t) * length * 2);
        if (NULL == handler->options.blockings) {
            DEBUG_ERR_OUTPUT(strerror(errno));
            return errno;
        }
        handler->options.blockings_len = length * 2;
        memcpy(handler->options.blockings, value,
                sizeof(uint32_t) * length * 2);
        break;

    case TTC_OPT_AFFINITY:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::affinity.");

        handler->options.affinity
            = (char *)malloc(sizeof(char) * length + 1);
        if (NULL == handler->options.affinity) {
            DEBUG_ERR_OUTPUT(strerror(errno));
            return errno;
        }
        memcpy(handler->options.affinity, value, sizeof(char) * length);
        handler->options.affinity[sizeof(char) * length] = '\0';
        break;

    case TTC_OPT_COMPILER:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::compiler.");

        handler->options.compiler = *(ttc_compiler_e *)value;
        break;

    case TTC_OPT_DATATYPE:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::datatype.");

        handler->options.datatype = *(ttc_datatype_e *)value;
        break;

    case TTC_OPT_ARCH:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::arch.");

        handler->options.arch = *(ttc_arch_e *)value;
        break;

    case TTC_OPT_TB:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::tb.");

        handler->options.tb = *(ttc_thread_blk_e *)value;
        break;

    case TTC_OPT_STATUS:
        DEBUG_INFO_OUTPUT("Setting option: "
                "ttc_handler_s::options::status.");

        handler->options.status = *(uint32_t *)value;
        break;

    default:
        DEBUG_WARN_OUTPUT("Unknown option. Won't change handler.");
        break;
    }

    return 0;
}


int32_t ttc_transpose(
        ttc_handler_s       *handler,
        const ttc_param_s   *param,
        const void          *input,
        void                *result
        ) {
    DEBUG_SET_NAMESPACE("ttc_transpose");
    DEBUG_INFO_OUTPUT("Executing transposition.");
    // Parameter check
    if (NULL == handler) {
        DEBUG_ERR_OUTPUT("handler is not initialized.");
        return -1;
    }
    if (NULL == param) {
        DEBUG_ERR_OUTPUT("param is not initialized.");
        return -1;
    }
    if (NULL == input) {
        DEBUG_ERR_OUTPUT("input is not initialized.");
        return -1;
    }
    if (NULL == result) {
        DEBUG_ERR_OUTPUT("result is not initialized.");
        return -1;
    }


    // Create plan
    DEBUG_INFO_OUTPUT("Creating a plan.");
    ttc_plan_s *plan = ttc_plan(handler, param);
    DEBUG_SET_NAMESPACE("ttc_transpose");
    if (NULL == plan) {
        DEBUG_ERR_OUTPUT("Cannot create plan.");
        return -1;
    }

    // Execute transpose
    DEBUG_INFO_OUTPUT("Executing transposition.");
    if (TTC_ARCH_CUDA == handler->options.arch)
        return ttc_exec_plan_cuda(plan, input, result);
    else
        return ttc_exec_plan(plan, input, result);
}

