#include "timer.h"
#include "timeout.h"
#include "util.h"
#include "libk_dlist.h"
#include "em_assert.h"
#include "em_core.h"

static inline void init_timeout(struct timeout *t)
{
	sys_dnode_init(&t->node);
}

/**
 * @brief Handle expiration of a kernel timer object.
 *
 * @param t  Timeout used by the timer.
 *
 * @return N/A
 */
static void timer_expiration_handler(struct timeout *t)
{
	struct timer *timer = CONTAINER_OF(t, struct timer, timeout);

        // If the timer is periodic, start it again.
	if (timer->period > 0) {
		add_timeout(&timer->timeout, timer_expiration_handler, timer->period);
	}

	// Update timer's status.
	timer->status += 1U;

	// Invoke timer's expiry function, if it has one.
	if (timer->expiry_fn != NULL) {
		timer->expiry_fn(timer);
	}
}

void timer_init(struct timer *timer, timer_expiry_t expiry_fn)
{
	timer->expiry_fn = expiry_fn;
	timer->status = 0U;

	init_timeout(&timer->timeout);
}

void timer_start(struct timer *timer, uint32_t duration, uint32_t period)
{
	EFM_ASSERT(duration != 0 || period != 0);

	abort_timeout(&timer->timeout);
	timer->period = period;
	timer->status = 0U;
	add_timeout(&timer->timeout, timer_expiration_handler, duration);
}

uint32_t timer_status_get(struct timer *timer)
{
        uint32_t result;
        CORE_CRITICAL_SECTION(
            result = timer->status;
            timer->status = 0U;
        )

	return result;
}
