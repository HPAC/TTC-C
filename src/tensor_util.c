#include "tensor_util.h"



bool
uint32cmp(
        const uint32_t  *arr1,
        const uint32_t  *arr2,
        uint32_t        length
        ) {
    uint32_t idx;
    for (idx = 0; idx < length && arr1[idx] == arr2[idx]; ++idx);
    return (idx == length);
}


#ifdef TENSOR_DEBUG

char func_namespace[128];
void set_namespace(const char *name) {
    strcpy(func_namespace, name);
}

#endif

