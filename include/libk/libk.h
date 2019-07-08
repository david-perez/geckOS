#ifndef LIBK_H
#define LIBK_H

#include "libk_timer.h"

#define SVC(code) __asm__ volatile ("svc %0" : : "I" (code))

// Tell gcc to not optimize out the apparently unused parameters.
void __attribute__((optimize("-O0"))) svc_leds_toggle(uint32_t button_no) {
    SVC(10);
}

void __attribute__((optimize("-O0"))) svc_set_button_handler(uint32_t button_no, void (*func)) {
    SVC(11);
}

void __attribute__((optimize("-O0"))) svc_timer_init(struct timer *timer, timer_expiry_t expiry_fn) {
    SVC(12);
}

void __attribute__((optimize("-O0"))) svc_timer_start(struct timer *timer, uint32_t duration, uint32_t period) {
    SVC(13);
}

#endif // LIBK_H
