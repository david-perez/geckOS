#ifndef SVC_DEVICE_H
#define SVC_DEVICE_H

#include <stdint.h>

typedef void (*button_handler_fn_t)();

void svc_leds_toggle(uint32_t button_no);
void svc_set_button_handler(uint32_t button_no, button_handler_fn_t handler_fn);

#endif // SVC_DEVICE_H
