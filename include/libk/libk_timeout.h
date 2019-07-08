#ifndef LIBK_TIMEOUT_H
#define LIBK_TIMEOUT_H

#include "libk_dlist.h"
#include <stdint.h>

struct timeout;
typedef void (*timeout_func_t)(struct timeout *t);

struct timeout {
    sys_dnode_t node;
    int32_t dticks;
    timeout_func_t fn;
};

#endif // LIBK_TIMEOUT_H
