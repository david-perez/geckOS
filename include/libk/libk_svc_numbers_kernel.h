#ifndef LIBK_SVC_NUMBERS_KERNEL_H
#define LIBK_SVC_NUMBERS_KERNEL_H

/*
 * This is an offset that specifies the first SVC number that can be used by
 * kernel services.
 * Right now it's set to 150. So that means SVC numbers 0-149
 * can be used by the device manufacturer, and SVC numbers 150-255 are reserved
 * for geckOS kernel services.
 *
 * The offset is pretty much arbitrary, but 150 SVC numbers reserved for the
 * kernel seems high enough to implement a lot of interesting stuff.
 */
#define SVC_NUMBER_KERNEL_OFFSET 150

// Kernel services. Board independent.
enum svc_no_kernel {
    TIMER_INIT = SVC_NUMBER_KERNEL_OFFSET,
    TIMER_START,
    SLEEP_MS,
    SLEEP_US,
};

#endif // LIBK_SVC_NUMBERS_KERNEL_H
