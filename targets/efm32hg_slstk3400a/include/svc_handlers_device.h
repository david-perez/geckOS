#ifndef SVC_HANDLERS_DEVICE_H
#define SVC_HANDLERS_DEVICE_H

#include <stdint.h>

void svc_leds_toggle_handler(uint32_t button_no);
void svc_set_button_handler_handler(uint32_t button_no, void (*func)());

#endif // SVC_HANDLERS_DEVICE_H
