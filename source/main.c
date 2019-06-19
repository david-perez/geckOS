#include "em_device.h"
#include "em_assert.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "em_msc.h"
#include "bsp.h"
#include "em_usb.h"
#include "msdd.h"
#include "msddmedia.h"
#include "descriptors.h"
#include "fat.h"

#define PROGRAM_MAXIMUM_LENGTH          0xb000 // ~45KiB; 0x5000 + 0xb000 = 0x10000 = 64KiB.
#define PROGRAM_ADDRESS_IN_FLASH        0x5000 // This should be kept to a value above the OS code.
#define MAGIC_ADDRESS			0xfe00000
#define MAGIC_VALUE			0xf00dbabe

extern uint32_t __Vectors;
extern uint32_t __StackTop;
static volatile uint32_t msTicks; // Counts 1ms time ticks.

void Default_Handler(void);
extern void jump_to_address(void * address);
static void stateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);

// Static buffer to hold sectors that we will transfer from/to the MSD device.
STATIC_UBUF(buffer, SECTOR_SIZE);

// Static buffer to write to flash.
STATIC_UBUF(flashPage, FLASH_PAGE_SIZE);

typedef union {
    void (*pFunc)(void);
    void *topOfStack;
} tVectorEntry;

// All 37 entries of a RAM-based interrupt vector table.
tVectorEntry ram_vector_table[] = { // Not const because we want it in RAM.
    /* Cortex-M Exception Handlers */
    { .topOfStack = &__StackTop },              /* Initial Stack Pointer */
    { Reset_Handler             },              /* Reset Handler */
    { NMI_Handler               },              /* NMI Handler */
    { HardFault_Handler         },              /* Hard Fault Handler */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { SVC_Handler               },              /* SVCall Handler */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { PendSV_Handler            },              /* PendSV Handler */
    { SysTick_Handler           },              /* SysTick Handler */

    /* External interrupts */

    { DMA_IRQHandler            },              /* 0 */
    { GPIO_EVEN_IRQHandler      },              /* 1 */
    { TIMER0_IRQHandler         },              /* 2 */
    { ACMP0_IRQHandler          },              /* 3 */
    { ADC0_IRQHandler           },              /* 4 */
    { I2C0_IRQHandler           },              /* 5 */
    { GPIO_ODD_IRQHandler       },              /* 6 */
    { TIMER1_IRQHandler         },              /* 7 */
    { USART1_RX_IRQHandler      },              /* 8 */
    { USART1_TX_IRQHandler      },              /* 9 */
    { LEUART0_IRQHandler        },              /* 10 */
    { PCNT0_IRQHandler          },              /* 11 */
    { RTC_IRQHandler            },              /* 12 */
    { CMU_IRQHandler            },              /* 13 */
    { VCMP_IRQHandler           },              /* 14 */
    { MSC_IRQHandler            },              /* 15 */
    { AES_IRQHandler            },              /* 16 */
    { USART0_RX_IRQHandler      },              /* 17 */
    { USART0_TX_IRQHandler      },              /* 18 */
    { USB_IRQHandler            },              /* 19 */
    { TIMER2_IRQHandler         },              /* 20 */
};

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
    if (newState == USBD_STATE_CONFIGURED) {
        BSP_LedSet(1);
    } else {
        BSP_LedClear(1);
    }

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
    msTicks++;

    if (msTicks % 500 == 0) { // Every half a second.
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

                // TODO: We should write the file size somewhere in flash also.
                //*(uint32_t*)(flashPage + FLASH_PAGE_SIZE - 4) = fileSize;

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

void new_sys_tick_handler() {
    msTicks++;

    if (msTicks % 250 == 0) BSP_LedToggle(0);
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

int main(void) {
    CHIP_Init();

    uint32_t value = *(uint32_t*) MAGIC_ADDRESS;
    if (value == MAGIC_VALUE) {
        /* TODO: BEGIN BOILERPLATE */

        /* Setup SysTick Timer for 1 msec interrupts  */
        if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
            EFM_ASSERT(false);
            while (true) {
            }
        }

        BSP_LedsInit();

        // Enable clock for GPIO module, initialize GPIOINT
        CMU_ClockEnable(cmuClock_GPIO, true);
        GPIOINT_Init();

        /* TODO: END BOILERPLATE */

        CORE_InitNvicVectorTable(
            &__Vectors,
            sizeof(ram_vector_table) / sizeof(ram_vector_table[0]),
            (uint32_t*) &ram_vector_table,
            sizeof(ram_vector_table) / sizeof(ram_vector_table[0]),
            &Default_Handler,
            true
        );

        NVIC_SetPriority(GPIO_ODD_IRQn, 1);
        NVIC_SetPriority(GPIO_EVEN_IRQn, 1);

        //CORE_SetNvicRamTableHandler(SysTick_IRQn, new_sys_tick_handler);

        jump_to_address((void*) (PROGRAM_ADDRESS_IN_FLASH | 0x1));

        // User application should never return, but we might as well loop
        // here in case it does.
        while (true) {
        }
    }

    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
    CMU_ClockEnable(cmuClock_GPIO, true);

    // Initialize LED driver.
    BSP_LedsInit();

    // Initialize the Mass Storage Media.
    if (!MSDDMEDIA_Init()) {
        EFM_ASSERT(false);
        while (true) {
        }
    }

    // Initialize the Mass Storage Device driver.
    MSDD_Init(gpioPortF, 4);
    
    init_fat_filesystem();

    /* Setup SysTick Timer for 1 msec interrupts  */
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
