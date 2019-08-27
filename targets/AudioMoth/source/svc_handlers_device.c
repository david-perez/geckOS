#include <stdbool.h>
#include "pinouts.h"
#include "em_gpio.h" // Contains GPIO port definitions.

void AudioMoth_setBothLED_handler(bool state) {
    GPIO_PinModeSet(LED_GPIOPORT, RED_PIN, gpioModePushPull, state);
    GPIO_PinModeSet(LED_GPIOPORT, GREEN_PIN, gpioModePushPull, state);
}
