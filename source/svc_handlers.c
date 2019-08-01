#include "svc_handlers.h"
#include "timer.h"
#include "ustimer.h"

void svc_timer_init_handler(struct timer *timer, timer_expiry_t expiry_fn) {
    timer_init(timer, expiry_fn);
}

void svc_timer_start_handler(struct timer *timer, uint32_t duration, uint32_t period) {
    timer_start(timer, duration, period);
}

void svc_sleep_ms_handler(uint32_t milliseconds) {
    uint32_t max_sleep_time = UINT32_MAX / 1000;
    while (milliseconds > max_sleep_time) {
        USTIMER_DelayIntSafe(UINT32_MAX);
        milliseconds -= max_sleep_time;
    }
    USTIMER_DelayIntSafe(milliseconds * 1000);
}

void svc_sleep_us_handler(uint32_t microseconds) {
    USTIMER_DelayIntSafe(microseconds);
}
