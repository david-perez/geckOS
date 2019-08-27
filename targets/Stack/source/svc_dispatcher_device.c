#include "svc_dispatcher_device.h"
#include "svc_handlers_device.h"

void svc_dispatcher_device(uint32_t *svc_args) {
    uint32_t svc_number = ((char *) svc_args[6])[-2];

    if (svc_number == SLEEP) {
        svc_sleep_handler(svc_args[0]);
    } else if (svc_number == TONE) {
        svc_tone_handler((uint16_t) svc_args[0]);
    } else if (svc_number == BEEP) {
        svc_beep_handler((uint16_t) svc_args[0], (uint16_t) svc_args[1]);
    } else if (svc_number == RGB) {
        svc_rgb_handler((uint8_t) svc_args[0], (uint8_t) svc_args[1], (uint8_t) svc_args[2]);
    } else if (svc_number == PIXEL) {
        svc_pixel_handler((uint8_t) svc_args[0], (uint8_t) svc_args[1]);
    } else if (svc_number == COLOUR) {
        svc_colour_handler((uint8_t) svc_args[0]);
    } else if (svc_number == FLASH) {
        svc_flash_handler((uint8_t) svc_args[0], (uint16_t) svc_args[1]);
    } else if (svc_number == TEMP) {
        // TODO
    }

    // We should never reach here.
    // TODO Panic!
}
