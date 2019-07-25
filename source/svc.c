#include <stdint.h>
#include "bsp.h"
#include "gpiointerrupt.h"
#include "em_gpio.h" // Contains GPIO port definitions.
#include "timer.h"
#include "neopixels.h"
#include "temperature.h"
#include "pwmout_api.h"
#include "ustimer.h"
#include "libk_svc.h"

/**** SLSTK3400A-specific SVC handlers ****/

void svc_leds_toggle_handler(uint32_t button_no) {
    BSP_LedToggle(button_no);
}

void svc_set_button_handler_handler(uint32_t button_no, void (*func)()) {
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

/**** Kernel services SVC handlers ****/

void svc_timer_init_handler(struct timer *timer, timer_expiry_t expiry_fn) {
    timer_init(timer, expiry_fn);
}

void svc_timer_start_handler(struct timer *timer, uint32_t duration, uint32_t period) {
    timer_start(timer, duration, period);
}

volatile bool expired;

void break_loop(struct timer* busy_wait_timer) {
    expired = true;
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

/**** STACK (device-specific) SVC handlers ****/

// The next two are helper functions.
void sounder_on(uint16_t frequency) {
    pwmout_period(1.0f / frequency);
    pwmout_write(0.5f);
}

void sounder_off() {
    pwmout_write(0.0f);
}

void svc_sleep_handler(uint16_t milliseconds) {
    svc_sleep_ms_handler(milliseconds);
}

void svc_tone_handler(uint16_t frequency) {
    if (frequency == 0) sounder_off();
    else sounder_on(frequency);
}

void svc_beep_handler(uint16_t frequency, uint16_t milliseconds) {
    if (frequency != 0) {
        sounder_on(frequency);
        svc_sleep_ms_handler(milliseconds);
    }
    sounder_off();
}

void svc_rgb_handler(uint8_t red, uint8_t green, uint8_t blue) {
    // We don't check that the RGB values are in range.
    neopixels_rgb(red, green, blue);
}

void svc_pixel_handler(uint8_t rgb_lsb, uint8_t p) {
    // We don't check that 0 <= rgb_lsb <= 7, 1 <= p <= 9.
    neopixels_colour_pixel(rgb_lsb, p);
}

void svc_colour_handler(uint8_t rgb_lsb) {
    // We don't check that 0 <= rgb_lsb <= 7.
    neopixels_colour(rgb_lsb);
}

void svc_flash_handler(uint8_t rgb_lsb, uint16_t milliseconds) {
    // We don't check that 0 <= rgb_lsb <= 7.
    neopixels_colour(rgb_lsb);
    svc_sleep_ms_handler(milliseconds);
}

/* int32_t svc_temp_handler() { */
/*     return temperature_get(); */
/* } */

void svc_dispatcher(uint32_t *svc_args) {
    uint32_t svc_number;

    svc_number = ((char *) svc_args[6])[-2];
    switch(svc_number) {
        // Calls to SLSTK3400A-specific SVC handlers (but common to many boards).
        case LEDS_TOGGLE:
            svc_leds_toggle_handler(svc_args[0]);
            break;
        case SET_BUTTON_HANDLER:
            svc_set_button_handler_handler(svc_args[0], (void (*)()) svc_args[1]);
            break;
        // Calls to kernel services SVC handlers (board independent).
        case TIMER_INIT:
            svc_timer_init_handler((struct timer*) svc_args[0], (timer_expiry_t) svc_args[1]);
            break;
        case TIMER_START:
            svc_timer_start_handler((struct timer*) svc_args[0], svc_args[1], svc_args[2]);
            break;
        case SLEEP_MS:
            svc_sleep_ms_handler(svc_args[0]);
            break;
        case SLEEP_US:
            svc_sleep_us_handler(svc_args[0]);
            break;
        // Calls to STACK (device-specific) SVC handlers.
        case SLEEP:
            svc_sleep_handler(svc_args[0]);
            break;
        case TONE:
            svc_tone_handler((uint16_t) svc_args[0]);
            break;
        case BEEP:
            svc_beep_handler((uint16_t) svc_args[0], (uint16_t) svc_args[1]);
            break;
        case RGB:
            svc_rgb_handler((uint8_t) svc_args[0], (uint8_t) svc_args[1], (uint8_t) svc_args[2]);
            break;
        case PIXEL:
            svc_pixel_handler((uint8_t) svc_args[0], (uint8_t) svc_args[1]);
            break;
        case COLOUR:
            svc_colour_handler((uint8_t) svc_args[0]);
            break;
        case FLASH:
            svc_flash_handler((uint8_t) svc_args[0], (uint16_t) svc_args[1]);
            break;
        /* case TEMP: */
        /*     break; */
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
