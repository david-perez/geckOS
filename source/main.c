#include <stdbool.h>
#include "em_chip.h"
#include "bootloader.h"
#include "kernel.h"

extern void jump_to_address(void * address);

int main(void) {
    // Chip errata.
    CHIP_Init();

    uint32_t value = *(uint32_t*) MAGIC_ADDRESS;
    if (value == MAGIC_VALUE) {
        prepare_kernel();

        jump_to_address((void*) (PROGRAM_ADDRESS_IN_FLASH | 0x1));

        // User application should never return, but we might as well loop
        // here in case it does.
        while (true) {
        }
    }

    // No magic value detected. Load bootloader.
    bootloader_init();
}
