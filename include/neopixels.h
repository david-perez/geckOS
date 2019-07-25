#ifndef NEOPIXELS_H
#define NEOPIXELS_H

#include <stdint.h>

#define NUMBER_OF_NEOPIXELS     9

void neopixels_init();
void neopixels_rgb(uint8_t red, uint8_t green, uint8_t blue);
void neopixels_colour(uint8_t colour);
void neopixels_colour_pixel(uint8_t colour, uint8_t pixel);
void neopixels_off();
    
#endif // NEOPIXELS_H
