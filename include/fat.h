#ifndef __FAT_H
#define __FAT_H

#include <stdint.h>

#define NUMBER_OF_ROOT_ENTRIES          16
#define SECTOR_SIZE          		512
#define DATA_START_SECTOR               3

#ifndef VOLUME_LABEL
#define VOLUME_LABEL                    "GECKOS"
#endif

typedef struct __attribute__((__packed__)) bpb_sector_t {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t number_of_fat;
    uint16_t number_of_root_directory_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t number_of_sectors_in_fat;
} bpb_sector_t;

typedef struct {
    char filename[8];
    char extension[3];
    uint8_t fileAttribute;
    uint8_t caseInformation;
    uint8_t creationTimeTenths;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t lastAccessDate;
    uint16_t empty;
    uint16_t lastWriteTime;
    uint16_t lastWriteDate;
    uint16_t firstLogicalCluster;
    uint32_t fileSize;
} directoryEntry;

uint16_t getFAT(uint16_t n, uint8_t *fatTable);
void setFAT(uint16_t n, uint16_t value, uint8_t *fatTable);
uint16_t physicalSector(uint16_t logicalCluster);
void write_bpb_sector(uint8_t* buffer, uint32_t number_of_sectors_in_filesystem);
void write_initial_fat_sector(uint8_t* buffer);
void write_initial_root_directory(uint8_t* buffer);

#endif
