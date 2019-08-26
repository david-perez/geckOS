#include "libk_svc_numbers_kernel.h"
#include "svc_handlers.h"
#include "svc_dispatcher_device.h"
#include <stdint.h>

void svc_dispatcher(uint32_t *svc_args) {
    uint32_t svc_number = ((char *) svc_args[6])[-2];

    // First, let's see if this is an SVC to a kernel service.
    if (svc_number == TIMER_INIT) {
        svc_timer_init_handler((struct timer*) svc_args[0], (timer_expiry_t) svc_args[1]);
    } else if (svc_number == TIMER_START) {
        svc_timer_start_handler((struct timer*) svc_args[0], svc_args[1], svc_args[2]);
    } else if (svc_number == SLEEP_MS) {
        svc_sleep_ms_handler(svc_args[0]);
    } else if (svc_number == SLEEP_US) {
        svc_sleep_us_handler(svc_args[0]);
    } else {
        // Let's see if this is an SVC defined by the device manufacturer.
        svc_dispatcher_device(svc_args);
    }
}

void __attribute__((naked)) SVC_Handler() {
    // We assume thread mode uses MSP, contrary to how the majority of OSes work.
    // If in the future we introduce support for scheduling, this has to be changed.
    __asm__ volatile(
        "mov r0, sp\t\n"
        "b svc_dispatcher\t\n"
    );
}
