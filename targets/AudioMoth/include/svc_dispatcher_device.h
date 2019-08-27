#ifndef SVC_DISPATCHER_DEVICE_H
#define SVC_DISPATCHER_DEVICE_H

#include <stdint.h>

enum svc_no_device {
    // AudioMoth specific
    SET_BOTH_LED,
};

void svc_dispatcher_device(uint32_t *svc_args);

#endif // SVC_DISPATCHER_DEVICE_H
