#include "stdint.h"
#include "libk_svc_bodies.h"
#include "libk_svc_numbers_kernel.h"

#include "libk_timer.h"

SYSCALL2(void, svc_timer_init, TIMER_INIT, struct timer*, timer,
        timer_expiry_t, expiry_fn)
SYSCALL3(void, svc_timer_start, TIMER_START, struct timer*, timer, uint32_t,
        duration, uint32_t, period)
SYSCALL1(void, svc_sleep_ms, SLEEP_MS, uint32_t, milliseconds)
SYSCALL1(void, svc_sleep_us, SLEEP_US, uint32_t, microseconds)
