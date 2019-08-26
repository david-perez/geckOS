#include "device_init.h"
#include "bsp.h"
#include "gpiointerrupt.h"

void device_init() {
    GPIOINT_Init();
    BSP_LedsInit();
}
