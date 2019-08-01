#ifndef SVC_DISPATCHER_DEVICE_H
#define SVC_DISPATCHER_DEVICE_H

#include <stdint.h>

enum svc_no_device {
    // SLSTK3400A-specific (but common to many boards).
    LEDS_TOGGLE,
    SET_BUTTON_HANDLER,
};

void svc_dispatcher_device(uint32_t *svc_args);

#endif // SVC_DISPATCHER_DEVICE_H
