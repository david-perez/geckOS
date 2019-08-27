#include "svc_device.h"
#include "libk_svc_bodies.h"

// We only need this include to have access to the SVC numbers.
#include "svc_dispatcher_device.h"

SYSCALL1(void, AudioMoth_setBothLED, SET_BOTH_LED, bool, state)
