#include "svc_device.h"
#include "libk_svc_bodies.h"

// We only need this include to have access to the SVC numbers.
#include "svc_dispatcher_device.h"

SYSCALL1(void, svc_leds_toggle, LEDS_TOGGLE, uint32_t, button_no)
SYSCALL2(void, svc_set_button_handler, SET_BUTTON_HANDLER, uint32_t, button_no,
        button_handler_fn_t, handler_fn)
