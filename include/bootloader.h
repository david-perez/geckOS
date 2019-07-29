#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#define MAGIC_ADDRESS			0xfe00000
#define MAGIC_VALUE			0xf00dbabe

#define PROGRAM_MAXIMUM_LENGTH          0x6000 // ~24KiB; 0xa000 + 0x6000 = 0x10000 = 64KiB.
#define PROGRAM_ADDRESS_IN_FLASH        0xa000 // This should be kept to a value above the OS code.

void bootloader_init();

#endif // BOOTLOADER_H
