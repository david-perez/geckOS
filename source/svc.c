#include <stdint.h>
#include "bsp.h"
#include "gpiointerrupt.h"
#include "em_gpio.h" // Contains GPIO port definitions.
#include "timer.h"

void svc_leds_toggle_handler(uint32_t button_no) {
    BSP_LedToggle(button_no);
}

void svc_set_button_handler(uint32_t button_no, void (*func)()) {
    uint8_t pin, port;
    if (button_no == 0) {
        port = BSP_GPIO_PB0_PORT;
        pin = BSP_GPIO_PB0_PIN;
    } else if (button_no == 1) {
        port = BSP_GPIO_PB1_PORT;
        pin = BSP_GPIO_PB1_PIN;
    } else return;

    // Register callback, interrupt is not yet enabled.
    GPIOINT_CallbackRegister(pin, (GPIOINT_IrqCallbackPtr_t) func);

    // Configure GPIO pin as input.
    GPIO_PinModeSet(port, pin, gpioModeInputPull, 1);

    // Enable interrupt. Interrupt number is the same as pin number.
    // Any pending interrupt for the selected interrupt is cleared by this function.
    GPIO_ExtIntConfig(port, pin, pin, false, true, true);
}

void svc_timer_init(struct timer *timer, timer_expiry_t expiry_fn) {
    timer_init(timer, expiry_fn);
}

void svc_timer_start(struct timer *timer, uint32_t duration, uint32_t period) {
    timer_start(timer, duration, period);
}

void svc_dispatcher(uint32_t *svc_args) {
    uint32_t svc_number;

    svc_number = ((char *) svc_args[6])[-2];
    switch(svc_number) {
        case 10:
            svc_leds_toggle_handler(svc_args[0]);
            break;
        case 11:
            svc_set_button_handler(svc_args[0], (void (*)()) svc_args[1]);
            break;
        case 12:
            svc_timer_init((struct timer*) svc_args[0], (timer_expiry_t) svc_args[1]);
            break;
        case 13:
            svc_timer_start((struct timer*) svc_args[0], svc_args[1], svc_args[2]);
            break;
        default:
            /* Unknown SVC */
            break;
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
