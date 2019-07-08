#ifndef TIMER_H
#define TIMER_H

#include "libk_timer.h"

void timer_init(struct timer *timer, timer_expiry_t expiry_fn);
void timer_start(struct timer *timer, uint32_t duration, uint32_t period);
uint32_t timer_status_get(struct timer *timer);

#endif // TIMER_H
