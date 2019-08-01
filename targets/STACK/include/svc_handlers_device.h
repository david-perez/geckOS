#ifndef SVC_HANDLERS_DEVICE_H
#define SVC_HANDLERS_DEVICE_H

#include <stdint.h>

void svc_sleep_handler(uint16_t milliseconds);
void svc_tone_handler(uint16_t frequency);
void svc_beep_handler(uint16_t frequency, uint16_t milliseconds);
void svc_rgb_handler(uint8_t red, uint8_t green, uint8_t blue);
void svc_pixel_handler(uint8_t rgb_lsb, uint8_t p);
void svc_colour_handler(uint8_t rgb_lsb);
void svc_flash_handler(uint8_t rgb_lsb, uint16_t milliseconds);
/* int32_t svc_temp_handler(); */

#endif // SVC_HANDLERS_DEVICE_H
