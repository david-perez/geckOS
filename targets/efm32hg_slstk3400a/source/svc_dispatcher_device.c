#include "svc_dispatcher_device.h"
#include "svc_handlers_device.h"

void svc_dispatcher_device(uint32_t *svc_args) {
    uint32_t svc_number = ((char *) svc_args[6])[-2];

    if (svc_number == LEDS_TOGGLE) {
        svc_leds_toggle_handler(svc_args[0]);
    } else if (svc_number == SET_BUTTON_HANDLER) {
        svc_set_button_handler_handler(svc_args[0], (void (*)()) svc_args[1]);
    }

    // We should never reach here.
    // TODO Panic!
}
