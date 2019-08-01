#ifndef SVC_HANDLERS_H
#define SVC_HANDLERS_H

#include <stdint.h>
#include "timer.h"

void svc_timer_init_handler(struct timer *timer, timer_expiry_t expiry_fn);
void svc_timer_start_handler(struct timer *timer, uint32_t duration, uint32_t period);
void svc_sleep_ms_handler(uint32_t milliseconds);
void svc_sleep_us_handler(uint32_t microseconds);

#endif // SVC_HANDLERS_H
