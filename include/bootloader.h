#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#define MAGIC_ADDRESS			0xfe00000
#define MAGIC_VALUE			0xf00dbabe


// This should be kept to a value above the OS code.
#ifndef APPLICATION_ORIGIN
    #define APPLICATION_ORIGIN 0x8800
#endif

#ifndef APPLICATION_MAX_SIZE
    #define APPLICATION_MAX_SIZE 0x7800
#endif

void bootloader_init();

#endif // BOOTLOADER_H
