#ifndef SVC_DEVICE_H
#define SVC_DEVICE_H

#include <stdint.h>

void svc_sleep(uint16_t milliseconds);
void svc_tone(uint16_t frequency);
void svc_beep(uint16_t frequency, uint16_t milliseconds);
void svc_rgb(uint8_t red, uint8_t green, uint8_t blue);
void svc_pixel(uint8_t rgb_lsb, uint8_t p);
void svc_colour(uint8_t rgb_lsb);
void svc_flash(uint8_t rgb_lsb, uint16_t milliseconds);

#endif // SVC_DEVICE_H
