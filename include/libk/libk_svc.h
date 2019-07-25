#ifndef LIBK_SVC_H
#define LIBK_SVC_H

enum svc_no {
    // SLSTK3400A-specific (but common to many boards).
    LEDS_TOGGLE,
    SET_BUTTON_HANDLER,

    // Kernel services. Board independent.
    TIMER_INIT,
    TIMER_START,
    SLEEP_MS,
    SLEEP_US,

    // STACK (device-specific)
    SLEEP,
    TONE,
    BEEP,
    RGB,
    PIXEL,
    COLOUR,
    FLASH,
    TEMP
    // TODO ACCEL
};

#endif // LIBK_SVC_H
