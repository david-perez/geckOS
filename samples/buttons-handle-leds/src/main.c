#include <stdbool.h>
#include <stdint.h>
#include "libk.c"

static void callback_button_0() {
    svc_leds_toggle(0);
}

static void callback_button_1() {
    svc_leds_toggle(1);
}

int main() {
    svc_set_button_handler(0, callback_button_0);
    svc_set_button_handler(1, callback_button_1);

    while (true) {
    }
}
