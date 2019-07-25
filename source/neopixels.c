#include "neopixels.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_core.h"
#include "ustimer.h"
#include "em_device.h"

#define NEOPIXEL_PORT       0 // Port A.
#define NEOPIXEL_PORT_PIN   2 // Pin 2.
#define NEOPIXEL_POLARITY   1 // Default LED polarity is active high.

static uint8_t pixels[NUMBER_OF_NEOPIXELS][3];

// Timings are stringent for the neopixel protocol.
// These nop delays have only been tested on EFM32HG with
// the GPIO CMU_HFPERCLKEN0 clock set to HXFO @ 24 MHz.
void __attribute__((optimize("-O0"))) neopixel_send_one() {
    GPIO->P[NEOPIXEL_PORT].DOUTSET = 1 << NEOPIXEL_PORT_PIN;
    __asm__(
            ".rept 12\n"
            "nop\n"
            ".endr\n"
           );
    GPIO->P[NEOPIXEL_PORT].DOUTCLR = 1 << NEOPIXEL_PORT_PIN;
}

void __attribute__((optimize("-O0"))) neopixel_send_zero() {
    GPIO->P[NEOPIXEL_PORT].DOUTSET = 1 << NEOPIXEL_PORT_PIN;
    __asm__(
            " nop\n" \
            " nop\n" \
           );
    GPIO->P[NEOPIXEL_PORT].DOUTCLR = 1 << NEOPIXEL_PORT_PIN;
    __asm__(
            ".rept 16\n"
            "nop\n"
            ".endr\n"
           );
}

void neopixels_init() {
    // Initialize stuff that we need to drive the neopixels.
    // I don't know whether it hurts to initialize these again, in case they've
    // already been initialized.
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(NEOPIXEL_PORT, NEOPIXEL_PORT_PIN, gpioModePushPull, NEOPIXEL_POLARITY);
    USTIMER_Init();

    neopixels_off();
}

void neopixels_update() {
    uint8_t colours[3];

    GPIO->P[NEOPIXEL_PORT].DOUTCLR = 1 << NEOPIXEL_PORT_PIN;

    // 50 us reset, loaded data is latched.
    // Note that we use USTIMER_DelayIntSafe instead of USTIMER_Delay
    // because this function might be being called from an SVC.
    USTIMER_DelayIntSafe(50);

    // Disable interrupts because the neopixels' timing protocol is very stringent
    // when sending a zero.
    // See https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
    CORE_CRITICAL_SECTION(
        for (int i = 0; i < NUMBER_OF_NEOPIXELS; i++) {
            // Neopixel wants colors in green then red then blue order.
            colours[0] = pixels[i][1];
            colours[1] = pixels[i][0];
            colours[2] = pixels[i][2];

            for (uint8_t j = 0; j < 3; j++) {
                for (uint8_t k = 128; k > 0; k >>= 1) {
                    if (colours[j] & k) neopixel_send_one();
                    else neopixel_send_zero();
                }
            }
        }
    )
}

void neopixels_off() {
    neopixels_rgb(0, 0, 0);
}

void neopixels_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < NUMBER_OF_NEOPIXELS; i++) {
       pixels[i][0] = red;
       pixels[i][1] = green;
       pixels[i][2] = blue;
    }

    neopixels_update();
}

// Pixels are 1-indexed.
void neopixels_rgb_pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t pixel) {
    pixels[pixel - 1][0] = red;
    pixels[pixel - 1][1] = green;
    pixels[pixel - 1][2] = blue;

    neopixels_update();
}

/* Colours are encoded as RGB using the three least significant bits, so 0 - off,
 * 1 - blue, 2 - green, 3 - cyan, 4 - red, 5 - magenta, 6 - yellow and 7 - white. */
void neopixels_colour(uint8_t colour) {
    uint8_t red = (colour & 0x04) ? 255 : 0;
    uint8_t green = (colour & 0x02) ? 255 : 0;
    uint8_t blue = (colour & 0x01) ? 255 : 0;

    neopixels_rgb(red, green, blue);
}


/* Colours are encoded as RGB using the three least significant bits, so 0 - off,
 * 1 - blue, 2 - green, 3 - cyan, 4 - red, 5 - magenta, 6 - yellow and 7 - white.
 * Pixels are 1-indexed. */
void neopixels_colour_pixel(uint8_t colour, uint8_t pixel) {
    pixels[pixel - 1][0] = (colour & 0x04) ? 255 : 0;
    pixels[pixel - 1][1] = (colour & 0x02) ? 255 : 0;
    pixels[pixel - 1][2] = (colour & 0x01) ? 255 : 0;

    neopixels_update();
}
