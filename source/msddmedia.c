/***************************************************************************//**
 * @file
 * @brief Media interface for Mass Storage class Device (MSD).
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_usb.h"

#include "msdd.h"
#include "msddmedia.h"

#if (MSD_MEDIA == MSD_PSRAM_MEDIA)
#include "em_ebi.h"
#endif

#if (MSD_MEDIA == MSD_SDCARD_MEDIA)
#include "microsd.h"
#include "diskio.h"
#include "bsp.h"
#include "bsp_trace.h"
#endif

#if (MSD_MEDIA == MSD_FLASH_MEDIA)
#include "em_msc.h"
#include "em_core.h"
#include "em_ramfunc.h"
#endif

#if (MSD_MEDIA == MSD_NORFLASH_MEDIA)
#include "em_ebi.h"
#include "norflash.h"
#endif

#if (MSD_MEDIA == MSD_SRAM_MEDIA)

/* Figure out if the SRAM is large enough */
  #if (SRAM_SIZE < (128 * 1024) )
  #error "SRAM based media can only be used on devices with 128K SRAM size."
  #endif

  #define MEDIA_SIZE (96 * 1024)
SL_ALIGN(4)
static uint8_t storage[MEDIA_SIZE];

#elif (MSD_MEDIA == MSD_PSRAM_MEDIA)

  #define MEDIA_SIZE (4 * 1024 * 1024)
static uint8_t *storage;

#elif (MSD_MEDIA == MSD_SDCARD_MEDIA)

#elif (MSD_MEDIA == MSD_FLASH_MEDIA)

/* The first 32K of FLASH is reserved for application code. */
  #define MEDIA_SIZE (FLASH_SIZE - (32 * 1024) )

  #define FLASH_PAGESIZE FLASH_PAGE_SIZE

struct {
  uint8_t *pPageBase;
  bool    pendingWrite;
} flashStatus;

static uint8_t  *storage = (uint8_t*)(32 * 1024);
static uint32_t flashPageSize = FLASH_PAGESIZE;
STATIC_UBUF(flashPageBuf, FLASH_PAGESIZE);

#elif (MSD_MEDIA == MSD_NORFLASH_MEDIA)

struct {
  uint8_t *pPageBase;
  bool    pendingWrite;
} flashStatus;

static uint8_t  *storage;
static uint8_t  *flashPageBuf;
static uint32_t flashPageSize;
#else

  #error "Illegal media definition."

#endif

#define FLUSH_TIMER           0       /* Timer id. */
#define FLUSH_TIMER_TIMEOUT   250     /* Unit is milliseconds. */

static uint32_t numSectors;

#if (MSD_MEDIA == MSD_FLASH_MEDIA) || (MSD_MEDIA == MSD_NORFLASH_MEDIA)
/***************************************************************************//**
 * @brief
 *   Erase and rewrite a flash page.
 ******************************************************************************/
#if (MSD_MEDIA == MSD_FLASH_MEDIA)

#if defined(__ICCARM__) /* IAR compiler */
/* Suppress warnings originating from use of CORE_ENTER/EXIT_ATOMIC()   */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif

SL_RAMFUNC_DEFINITION_BEGIN
void FlushFlash(void)
{
  CORE_DECLARE_IRQ_STATE;

  /* We can't serve interrupts while erasing or writing to flash. */
  CORE_ENTER_ATOMIC();

  MSC->LOCK = MSC_UNLOCK_CODE;

  /* Erase flash page */
  MSC_ErasePage( (uint32_t*)flashStatus.pPageBase);

  /* Program flash page */
  MSC_WriteWord( (uint32_t*)flashStatus.pPageBase, flashPageBuf, flashPageSize);

  MSC->LOCK = 0;

  CORE_EXIT_ATOMIC();
}
SL_RAMFUNC_DEFINITION_END

#if defined(__ICCARM__)
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif

#else
static void FlushFlash(void)
{
  /* Erase flash sector */
  NORFLASH_EraseSector( (uint32_t)flashStatus.pPageBase);

  /* Program flash sector */
  NORFLASH_Program( (uint32_t)flashStatus.pPageBase, flashPageBuf, flashPageSize);
}
#endif
#endif

#if (MSD_MEDIA == MSD_FLASH_MEDIA) || (MSD_MEDIA == MSD_NORFLASH_MEDIA)
/***************************************************************************//**
 * @brief
 *   Flush pending media write operations.
 *   This will only affect flash based media.
 ******************************************************************************/
static void FlushTimerTimeout(void)
{
  MSDDMEDIA_Flush();
}
#endif

/***************************************************************************//**
 * @brief
 *   Check if a media access is legal, prepare for later data transmissions.
 *
 * @param[in] pCmd
 *   Points to a MSDD_CmdStatus_TypeDef structure which holds info about the
 *   current transfer.
 *
 * @param[in] lba
 *   Media "Logical Block Address".
 *
 * @param[in] sectors
 *   Number of 512 byte sectors to transfer.
 *
 * @return
 *   True if legal access, false otherwise.
 ******************************************************************************/
bool MSDDMEDIA_CheckAccess(MSDD_CmdStatus_TypeDef *pCmd,
                           uint32_t lba, uint32_t sectors)
{
  if ( (lba + sectors) > numSectors ) {
    return false;
  }

  #if (MSD_MEDIA == MSD_SRAM_MEDIA) || (MSD_MEDIA == MSD_PSRAM_MEDIA)
  pCmd->pData    = &storage[lba * 512];
  pCmd->xferType = XFER_MEMORYMAPPED;
  #endif

  #if (MSD_MEDIA == MSD_SDCARD_MEDIA)
  pCmd->lba      = lba;
  pCmd->xferType = XFER_INDIRECT;
  pCmd->maxBurst = MEDIA_BUFSIZ;
  #endif

  #if (MSD_MEDIA == MSD_FLASH_MEDIA) || (MSD_MEDIA == MSD_NORFLASH_MEDIA)
  pCmd->lba   = lba;
  pCmd->pData = &storage[lba * 512];
  if ( pCmd->direction && !flashStatus.pendingWrite ) {
    pCmd->xferType = XFER_MEMORYMAPPED;
  } else {
    pCmd->xferType = XFER_INDIRECT;
    pCmd->maxBurst = MEDIA_BUFSIZ;
  }
  #endif

  pCmd->xferLen = sectors * 512;

  return true;
}

/***************************************************************************//**
 * @brief
 *   Flush pending media writes.
 ******************************************************************************/
void MSDDMEDIA_Flush(void)
{
  #if (MSD_MEDIA == MSD_FLASH_MEDIA) || (MSD_MEDIA == MSD_NORFLASH_MEDIA)
  if ( flashStatus.pendingWrite ) {
    flashStatus.pendingWrite = false;
    USBTIMER_Stop(FLUSH_TIMER);
    FlushFlash();
  }
  #endif
}

/***************************************************************************//**
 * @brief
 *   Get number of 512 byte sectors on the media.
 *
 * @return
 *   Number of sectors on media.
 ******************************************************************************/
uint32_t MSDDMEDIA_GetSectorCount(void)
{
  return numSectors;
}

/***************************************************************************//**
 * @brief
 *   Initialize the storage media interface.
 ******************************************************************************/
bool MSDDMEDIA_Init(void)
{
  #if (MSD_MEDIA != MSD_SDCARD_MEDIA) && (MSD_MEDIA != MSD_NORFLASH_MEDIA)
  numSectors = MEDIA_SIZE / 512;
  #endif

  #if (MSD_MEDIA == MSD_PSRAM_MEDIA)
  storage = (uint8_t*)EBI_BankAddress(EBI_BANK2);
  storage[0] = 0;   /* To force new "format disk" when host detects disk. */
  #endif

  #if (MSD_MEDIA == MSD_SDCARD_MEDIA)
  /* Enable SPI access to MicroSD card */
  BSP_PeripheralAccess(BSP_MICROSD, true);
  MICROSD_Init();

  if ( disk_initialize(0) != 0 ) {
    return false;
  }

  /* Get numSectors from media. */
  if ( disk_ioctl(0, GET_SECTOR_COUNT, &numSectors) != RES_OK ) {
    return false;
  }
  #endif

  #if (MSD_MEDIA == MSD_FLASH_MEDIA)
  flashStatus.pendingWrite = false;
  MSC_Init();                         /* Unlock and calibrate flash timing  */
  MSC_Deinit();                       /* Lock flash                         */
  #endif

  #if (MSD_MEDIA == MSD_NORFLASH_MEDIA)
  flashStatus.pendingWrite = false;
  NORFLASH_Init();                    /* Initialize NORFLASH interface      */

  storage       = (uint8_t*)NORFLASH_DeviceInfo()->baseAddress;
  flashPageSize = NORFLASH_DeviceInfo()->sectorSize;
  /* Use external PSRAM as page (flash sector) buffer */
  flashPageBuf  = (uint8_t*)EBI_BankAddress(EBI_BANK2);
  numSectors    = NORFLASH_DeviceInfo()->deviceSize / 512;
  #endif

  return true;
}

/***************************************************************************//**
 * @brief
 *   Read from indirectly accessed media.
 *
 * @param[in] pCmd
 *   Points to a MSDD_CmdStatus_TypeDef structure which holds info about the
 *   current transfer.
 *
 * @param[in] data
 *   Pointer to data buffer.
 *
 * @param[in] sectors
 *   Number of 512 byte sectors to read from media.
 ******************************************************************************/
void MSDDMEDIA_Read(MSDD_CmdStatus_TypeDef *pCmd, uint8_t *data, uint32_t sectors)
{
  #if (MSD_MEDIA == MSD_SRAM_MEDIA) || (MSD_MEDIA == MSD_PSRAM_MEDIA)
  (void)pCmd;
  (void)data;
  (void)sectors;
  #endif

  #if (MSD_MEDIA == MSD_SDCARD_MEDIA)
  disk_read(0, data, pCmd->lba, sectors);
  #endif

  #if (MSD_MEDIA == MSD_FLASH_MEDIA) || (MSD_MEDIA == MSD_NORFLASH_MEDIA)
  /* Write pending data to flash before starting the read operation. */
  MSDDMEDIA_Flush();
  memcpy(data, pCmd->pData, sectors * 512);
  pCmd->pData += sectors * 512;
  #endif
}

/***************************************************************************//**
 * @brief
 *   Write to indirectly accessed media.
 *
 * @param[in] pCmd
 *   Points to a MSDD_CmdStatus_TypeDef structure which holds info about the
 *   current transfer.
 *
 * @param[in] data
 *   Pointer to data buffer.
 *
 * @param[in] sectors
 *   Number of 512 byte sectors to write to media.
 ******************************************************************************/
void MSDDMEDIA_Write(MSDD_CmdStatus_TypeDef *pCmd, uint8_t *data, uint32_t sectors)
{
  #if (MSD_MEDIA == MSD_SRAM_MEDIA) || (MSD_MEDIA == MSD_PSRAM_MEDIA)
  (void)pCmd;
  (void)data;
  (void)sectors;
  #endif

  #if (MSD_MEDIA == MSD_SDCARD_MEDIA)
  disk_write(0, data, pCmd->lba, sectors);
  #endif

  #if (MSD_MEDIA == MSD_FLASH_MEDIA) || (MSD_MEDIA == MSD_NORFLASH_MEDIA)
  unsigned int i;
  uint32_t offset;

  i = 0;
  while ( i < sectors ) {
    if ( !flashStatus.pendingWrite ) {
      /* Copy an entire flash page to the page buffer */
      flashStatus.pendingWrite = true;
      flashStatus.pPageBase    = (uint8_t*)((uint32_t)pCmd->pData & ~(flashPageSize - 1));
      offset                    = pCmd->pData - flashStatus.pPageBase;
      memcpy(flashPageBuf, flashStatus.pPageBase, flashPageSize);

      /* Write the received data in the page buffer */
      memcpy(flashPageBuf + offset, data, 512);
      data        += 512;
      pCmd->pData += 512;

      USBTIMER_Start(FLUSH_TIMER, FLUSH_TIMER_TIMEOUT, FlushTimerTimeout);
    } else {
      /* Check if current sector is located in the page buffer. */
      offset = pCmd->pData - flashStatus.pPageBase;
      if ( offset >= flashPageSize ) {
        /*
         * Current sector not located in page buffer, flush pending data
         * before continuing.
         */
        MSDDMEDIA_Flush();
        i--;
      } else {
        /* Write the received data in the page buffer */
        memcpy(flashPageBuf + offset, data, 512);
        data        += 512;
        pCmd->pData += 512;

        USBTIMER_Start(FLUSH_TIMER, FLUSH_TIMER_TIMEOUT, FlushTimerTimeout);
      }
    }
    i++;
  }
  #endif
}
