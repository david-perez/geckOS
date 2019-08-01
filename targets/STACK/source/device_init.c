#include "pwmout_api.h"
#include "neopixels.h"

void device_init() {
    neopixels_init();
    pwmout_init();
}
