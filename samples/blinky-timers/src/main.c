#include <stdbool.h>
#include <stdint.h>
#include "libk.c"

void my_expiry_function(struct timer *timer) {
    svc_leds_toggle(1);
}

void my_expiry_function2(struct timer *timer) {
    svc_leds_toggle(0);
}

int main() {
    /* struct timer my_timer; */
    /* svc_timer_init(&my_timer, my_expiry_function); */
    /* svc_timer_start(&my_timer, 1000, 1000); */

    /* struct timer my_timer2; */
    /* svc_timer_init(&my_timer2, my_expiry_function2); */
    /* svc_timer_start(&my_timer2, 2000, 2000); */

    while (true) {
    }
}
