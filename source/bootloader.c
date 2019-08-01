#include "bootloader.h"
#include "stdint.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_emu.h"
#include "em_msc.h"
#include "em_usb.h"
#include "msdd.h"
#include "msddmedia.h"
#include "descriptors.h"
#include "fat.h"

/* #include "bsp.h" */

static volatile uint32_t ms_ticks; // Counts 1ms time ticks.

static void stateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);

// Static buffer to hold sectors that we will transfer from/to the MSD device.
STATIC_UBUF(buffer, SECTOR_SIZE);

// Static buffer to write to flash.
STATIC_UBUF(flashPage, FLASH_PAGE_SIZE);

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = stateChangeEvent,
  .setupCmd        = MSDD_SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

// Called by USB device stack when USB has changed.
static void stateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState) {
    // Light LED 1 according to USB state.
    /* if (newState == USBD_STATE_CONFIGURED) { */
    /*     BSP_LedSet(1); */
    /* } else { */
    /*     BSP_LedClear(1); */
    /* } */

    // Call MSD drivers state change event handler.
    MSDD_StateChangeEvent(oldState, newState);
}

static void __attribute__((section(".ram"))) writeToFlash(uint32_t *page, uint8_t *data, uint16_t size) {
    MSC_Init();
    uint32_t irq_state = __get_PRIMASK();
    __disable_irq();
    MSC->LOCK = MSC_UNLOCK_CODE;
    MSC_ErasePage(page);
    MSC_WriteWord(page, data, size);
    MSC->LOCK = 0;
    __set_PRIMASK(irq_state);
    MSC_Deinit();
}

// This is called every millisecond, if the board is not in EM2.
void SysTick_Handler(void) {
    ms_ticks++;

    if (ms_ticks % 500 == 0) { // Every half a second.
        MSDD_CmdStatus_TypeDef pCmd;
        pCmd.direction = 1;
        pCmd.pData = buffer;

        // Get sector 2 of the MSD, which contains the root directory area.
        memset(buffer, 0, SECTOR_SIZE);
        MSDDMEDIA_CheckAccess(&pCmd, 2, 1);
        MSDDMEDIA_Read(&pCmd, buffer, 1);

        directoryEntry *entry = (directoryEntry*) buffer;
        for (int i = 0; i < NUMBER_OF_ROOT_ENTRIES; i++) {
            uint32_t fileSize = entry->fileSize;
            if (strncmp(entry->extension, "BIN", 3) == 0 && fileSize > 0 && fileSize <= PROGRAM_MAXIMUM_LENGTH) {
                // Calculate how many clusters the file spans.
                int how_many_clusters = fileSize / SECTOR_SIZE;
                if (fileSize % SECTOR_SIZE != 0) how_many_clusters++;

                uint16_t cluster_number = entry->firstLogicalCluster;
                for (int j = 0; j < how_many_clusters; j += 2) {
                    memset(flashPage, 0, FLASH_PAGE_SIZE);
                    MSDDMEDIA_CheckAccess(&pCmd, physicalSector(cluster_number), 1);
                    MSDDMEDIA_Read(&pCmd, flashPage, 1);

                    // If this is not the last cluster, put the next one also into the flash page buffer.
                    if (j != how_many_clusters - 1) {
                        // Get sector 1 of the MSD, which contains the FAT.
                        MSDDMEDIA_CheckAccess(&pCmd, 1, 1);
                        MSDDMEDIA_Read(&pCmd, buffer, 1);

                        // Get the next cluster after cluster_number, and update cluster_number.
                        cluster_number = getFAT(cluster_number, buffer);
                        MSDDMEDIA_CheckAccess(&pCmd, physicalSector(cluster_number), 1);
                        MSDDMEDIA_Read(&pCmd, flashPage + SECTOR_SIZE, 1);
                    }

                    writeToFlash((uint32_t*) (PROGRAM_ADDRESS_IN_FLASH + j * SECTOR_SIZE), flashPage, FLASH_PAGE_SIZE);

                    // Get the next cluster after cluster_number, if there is one, and update cluster_number.
                    if (j + 2 < how_many_clusters) {
                        cluster_number = getFAT(cluster_number, buffer);
                    }
                }

                // Write magic value to magic address.
                memset(flashPage, 0, FLASH_PAGE_SIZE);
                *(uint32_t*) (flashPage) = MAGIC_VALUE;
                writeToFlash((uint32_t*) (MAGIC_ADDRESS), flashPage, FLASH_PAGE_SIZE);

                NVIC_SystemReset();
            }

            entry++;
        } // End of for loop.
    }
}

void init_fat_filesystem() {
    // Make structure for initial setup.
    MSDD_CmdStatus_TypeDef pCmd;
    pCmd.direction = 1;
    pCmd.pData = buffer;

    // Write BPB sector to sector 0 of the MSD.
    memset(buffer, 0, SECTOR_SIZE);
    write_bpb_sector(buffer, MSDDMEDIA_GetSectorCount());
    MSDDMEDIA_CheckAccess(&pCmd, 0, 1);
    MSDDMEDIA_Write(&pCmd, buffer, 1);

    // Write initial fat sector.
    memset(buffer, 0, SECTOR_SIZE);
    write_initial_fat_sector(buffer);

    // Write the contents of buffer (which now contains 3 12-bit entries) to sector 1 of the MSD (we only have one FAT, in sector 1).
    MSDDMEDIA_CheckAccess(&pCmd, 1, 1);
    MSDDMEDIA_Write(&pCmd, buffer, 1);

    // Write root directory to sector 2 of the MSD.
    memset(buffer, 0, SECTOR_SIZE);
    write_initial_root_directory(buffer);
    MSDDMEDIA_CheckAccess(&pCmd, 2, 1);
    MSDDMEDIA_Write(&pCmd, buffer, 1);
}

void bootloader_init() {
    // Initialize clocks. Do we really need this if we're not going to use the LEDs?
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
    CMU_ClockEnable(cmuClock_GPIO, true);

    // Initialize LED driver.
    /* BSP_LedsInit(); */

    // Initialize the Mass Storage Media.
    if (!MSDDMEDIA_Init()) {
        EFM_ASSERT(false);
        while (true) {
        }
    }

    // Initialize the Mass Storage Device driver.
    MSDD_Init(gpioPortF, 4);

    init_fat_filesystem();

    // Setup SysTick for 1 msec interrupts.
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        EFM_ASSERT(false);
        while (true) {
        }
    }

    // Initialize and start USB device stack.
    USBD_Init(&usbInitStruct);

    while (true) {
        if (MSDD_Handler()) {
            // There is no pending activity in the MSDD handler.
            // Enter sleep mode to conserve energy.

            if (USBD_SafeToEnterEM2()) {
                // Disable IRQ's and perform test again to avoid race conditions!
                CORE_ATOMIC_SECTION(
                    if (USBD_SafeToEnterEM2()) {
                        EMU_EnterEM2(true);
                    }
                )
            } else {
                EMU_EnterEM1();
            }
        }
    }
}
