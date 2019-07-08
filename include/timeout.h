#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "libk_timeout.h"

void abort_timeout(struct timeout *to);
void add_timeout(struct timeout *to, timeout_func_t fn, int32_t ticks);
void tick_announce(uint32_t ticks);

#endif // TIMEOUT_H
