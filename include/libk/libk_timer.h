#ifndef LIBK_TIMER_H
#define LIBK_TIMER_H

#include "libk_timeout.h"
#include <stdint.h>

struct timer {
    struct timeout timeout;
    void (*expiry_fn)(struct timer *timer); // Runs in ISR context.
    int32_t period;
    uint32_t status;
};

/**
 * @typedef timer_expiry_t
 * @brief Timer expiry function type.
 *
 * A timer's expiry function is executed by the system clock interrupt handler
 * each time the timer expires. The expiry function is optional, and is only
 * invoked if the timer has been initialized with one.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
typedef void (*timer_expiry_t)(struct timer *timer);

#endif // LIBK_TIMER_H
