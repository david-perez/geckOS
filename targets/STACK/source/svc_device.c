#include "svc_device.h"
#include "libk_svc_bodies.h"

// We only need this include to have access to the SVC numbers.
#include "svc_dispatcher_device.h"

SYSCALL1(void, svc_sleep, SLEEP, uint16_t, milliseconds)
SYSCALL1(void, svc_tone, TONE, uint16_t, frequency)
SYSCALL2(void, svc_beep, BEEP, uint16_t, frequency, uint16_t, milliseconds)
SYSCALL3(void, svc_rgb, RGB, uint8_t, red, uint8_t, green, uint8_t, blue)
SYSCALL2(void, svc_pixel, PIXEL, uint8_t, rgb_lsb, uint8_t, p)
SYSCALL1(void, svc_colour, COLOUR, uint8_t, rgb_lsb)
SYSCALL2(void, svc_flash, FLASH, uint8_t, rgb_lsb, uint16_t, milliseconds)
