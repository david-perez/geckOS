#include "bsp.h"
#include "gpiointerrupt.h"
#include "em_gpio.h" // Contains GPIO port definitions.

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
