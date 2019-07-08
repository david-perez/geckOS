#include "timeout.h"
#include "em_core.h"
#include "util.h"
#include <stdint.h>

static sys_dlist_t timeout_list = SYS_DLIST_STATIC_INIT(&timeout_list);

// Ticks left to process in the currently-executing tick_announce().
static int announce_remaining;

static uint64_t curr_tick;

static struct timeout *first(void)
{
	sys_dnode_t *t = sys_dlist_peek_head(&timeout_list);

	return t == NULL ? NULL : CONTAINER_OF(t, struct timeout, node);
}

static struct timeout *next(struct timeout *t)
{
	sys_dnode_t *n = sys_dlist_peek_next(&timeout_list, &t->node);

	return n == NULL ? NULL : CONTAINER_OF(n, struct timeout, node);
}

static void remove_timeout(struct timeout *t)
{
	if (next(t) != NULL) {
		next(t)->dticks += t->dticks;
	}

	sys_dlist_remove(&t->node);
}

void abort_timeout(struct timeout *to)
{
        CORE_CRITICAL_SECTION(
            if (sys_dnode_is_linked(&to->node)) {
                remove_timeout(to);
            }
        )
}

void add_timeout(struct timeout *to, timeout_func_t fn, int32_t ticks)
{
	EFM_ASSERT(!sys_dnode_is_linked(&to->node));
	to->fn = fn;
	ticks = MAX(1, ticks);

        CORE_CRITICAL_SECTION(
                struct timeout *t;

                to->dticks = ticks;
                for (t = first(); t != NULL; t = next(t)) {
                    EFM_ASSERT(t->dticks >= 0);

                    if (t->dticks > to->dticks) {
                        t->dticks -= to->dticks;
                        sys_dlist_insert(&t->node, &to->node);
                        break;
                    }
                    to->dticks -= t->dticks;
                }

                if (t == NULL) {
                    sys_dlist_append(&timeout_list, &to->node);
                }
        )
}

// The handler for SysTick will call this function every tick. Since 1 tick = 1 ms (for now),
// and SysTick is configured for 1 ms interrupts, this means this function is called every 1 ms.
void tick_announce(uint32_t ticks)
{
        __disable_irq();

	announce_remaining = ticks;

	while (first() != NULL && first()->dticks <= announce_remaining) {
		struct timeout *t = first();
		int dt = t->dticks;

		curr_tick += dt;
		announce_remaining -= dt;
		t->dticks = 0;
		remove_timeout(t);

                __enable_irq();
		t->fn(t);
                __disable_irq();
	}

	if (first() != NULL) {
		first()->dticks -= announce_remaining;
	}

	curr_tick += announce_remaining;
	announce_remaining = 0;

        __enable_irq();
}
