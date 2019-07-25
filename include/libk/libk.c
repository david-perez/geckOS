#ifndef LIBK_H
#define LIBK_H

#include "libk_svc.h"
#include "libk_timer.h"

#define SYSCALL1(ret_type, svc_name, svc_nr, t1, p1)     void svc_name(t1 p1) { \
    register t1 r0 asm("r0") = p1; \
    \
    __asm__ volatile("svc %[nr]\n" \
            : "=r" (r0) \
            : [nr] "i" (svc_nr), "r" (r0) \
            : "memory", "r1", "r2", "r3", "r12", "lr"); \
}

#define SYSCALL2(ret_type, svc_name, svc_nr, t1, p1, t2, p2)     void svc_name(t1 p1, t2 p2) { \
    register t1 r0 asm("r0") = p1; \
    register t2 r1 asm("r1") = p2; \
    \
    __asm__ volatile("svc %[nr]\n" \
            : "=r" (r0) \
            : [nr] "i" (svc_nr), "r" (r0), "r" (r1) \
            : "memory", "r2", "r3", "r12", "lr"); \
}

#define SYSCALL3(ret_type, svc_name, svc_nr, t1, p1, t2, p2, t3, p3)     void svc_name(t1 p1, t2 p2, t3 p3) { \
    register t1 r0 asm("r0") = p1; \
    register t2 r1 asm("r1") = p2; \
    register t3 r2 asm("r2") = p3; \
    \
    __asm__ volatile("svc %[nr]\n" \
            : "=r" (r0) \
            : [nr] "i" (svc_nr), "r" (r0), "r" (r1), "r" (r2) \
            : "memory", "r3", "r12", "lr"); \
}

typedef void (*button_handler_fn_t)();

SYSCALL1(void, svc_leds_toggle, LEDS_TOGGLE, uint32_t, button_no)
SYSCALL2(void, svc_set_button_handler, SET_BUTTON_HANDLER, uint32_t, button_no,
        button_handler_fn_t, handler_fn)
SYSCALL2(void, svc_timer_init, TIMER_INIT, struct timer*, timer,
        timer_expiry_t, expiry_fn)
SYSCALL3(void, svc_timer_start, TIMER_START, struct timer*, timer, uint32_t,
        duration, uint32_t, period)

/**** Device-specific SVCs below. ****/

SYSCALL1(void, svc_sleep, SLEEP, uint16_t, milliseconds)
SYSCALL1(void, svc_tone, TONE, uint16_t, frequency)
SYSCALL2(void, svc_beep, BEEP, uint16_t, frequency, uint16_t, milliseconds)
SYSCALL3(void, svc_rgb, RGB, uint8_t, red, uint8_t, green, uint8_t, blue)
SYSCALL2(void, svc_pixel, PIXEL, uint8_t, rgb_lsb, uint8_t, p)
SYSCALL1(void, svc_colour, COLOUR, uint8_t, rgb_lsb)
SYSCALL2(void, svc_flash, FLASH, uint8_t, rgb_lsb, uint16_t, milliseconds)

/* int32_t __attribute__((optimize("-O0"))) svc_temp() { */
/*     SVC(TEMP); */
/* } */

#endif // LIBK_H
