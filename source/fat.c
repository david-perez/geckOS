#include <string.h>
#include "fat.h"

static uint8_t bpb_jmp_boot[3] = { 0xeb, 0x3c, 0x90 };

// A "boot signature" indicating that this is a valid boot sector.
static uint8_t bpb_boot_signature[2] = { 0x55, 0xaa };

uint16_t getFAT(uint16_t n, uint8_t *fatTable) {
    uint16_t offset = n + n / 2;
    if (n & 1) {
        return (fatTable[offset] >> 4) | ((uint16_t)fatTable[offset + 1] << 4);
    }

    return fatTable[offset] | ((uint16_t)(fatTable[offset + 1] & 0x0F) << 8);
}

void setFAT(uint16_t n, uint16_t value, uint8_t *fatTable) {
    uint16_t offset = n + n / 2;
    if (n & 1) {
    	fatTable[offset] = (fatTable[offset] & 0x0F) | ((value << 4) & 0xF0);
        fatTable[offset + 1] = value >> 4;
        return;
    }

    fatTable[offset] = value;
    fatTable[offset + 1] = (fatTable[offset + 1] & 0xF0) | ((value >> 8) & 0x0F);
}

uint16_t physicalSector(uint16_t logicalCluster) {
    return DATA_START_SECTOR + logicalCluster - 2;
}

void write_bpb_sector(uint8_t* buffer, uint32_t number_of_sectors_in_filesystem) {
    memcpy(buffer, bpb_jmp_boot, sizeof(bpb_jmp_boot));

    bpb_sector_t bpb_sector = {
        .bytes_per_sector = SECTOR_SIZE,
        .sectors_per_cluster = 1,
        .reserved_sectors = 1,
        .number_of_fat = 1,
        // TODO: On the FAT12/16 volumes, this field indicates number of 32-byte directory entries in the root directory.
        // The value should be set a value that the size of root directory is aligned to the 2-sector boundary, BPB_RootEntCnt * 32 becomes even multiple of BPB_BytsPerSec.
        // For maximum compatibility, this field should be set to 512 on the FAT16 volume. For FAT32 volumes, this field must be 0.
        // Why are we setting this to 16? 16 * 32 = 512, which is not an even multiple!
        .number_of_root_directory_entries = NUMBER_OF_ROOT_ENTRIES,
        .number_of_sectors = number_of_sectors_in_filesystem,
        .media_type = 0xF8, // 0xF8 is the standard value for non-removable disks.
        .number_of_sectors_in_fat = 1
    };

    memcpy(buffer + 11, &bpb_sector, sizeof(bpb_sector));
    memcpy(buffer + 510, bpb_boot_signature, sizeof(bpb_boot_signature));
}

void write_initial_fat_sector(uint8_t* buffer) {
    // The top two FAT entries, FAT[0] and FAT[1], are reserved and not associated with any cluster. These FAT entries are initialized on creating the volume as follows:
    // FAT12: FAT[0] = 0xF??; FAT[1] = 0xFFF;
    // ?? in the value of FAT[0] is the same value of BPB_Media.
    setFAT(0, 0xFF8, buffer);
    setFAT(1, 0xFFF, buffer);
}

void write_initial_root_directory(uint8_t* buffer) {
    char volume_label[8] = "        ";
    strncpy(volume_label, VOLUME_LABEL, 8);
    directoryEntry *entry = (directoryEntry*) buffer;
    memcpy(entry->filename, volume_label, 8);
    memcpy(entry->extension, "   ", 3);
    entry->fileAttribute = 0x08; // Volume label.
}
