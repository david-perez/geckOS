// We only need this if the device SVC handlers are going to make use of kernel
// services.
#include "svc_handlers.h"

#include "svc_handlers_device.h"
#include "neopixels.h"
#include "temperature.h"
#include "pwmout_api.h"

// The next two are helper functions.
void sounder_on(uint16_t frequency) {
    pwmout_period(1.0f / frequency);
    pwmout_write(0.5f);
}

void sounder_off() {
    pwmout_write(0.0f);
}

/**** These are the implementations of the SVCs proper ****/

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
