/***************************************************************************//**
 * @file
 * @brief USB protocol stack library, application supplied configuration options.
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

#ifndef USBCONFIG_H
#define USBCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//#define BUSPOWERED      /* Uncomment to build buspowered device */

#define USB_DEVICE        /* Compile stack for device mode. */

#if defined(BUSPOWERED)
  #define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND)
#else
  #define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND | USB_PWRSAVE_MODE_ONVBUSOFF)
#endif

/****************************************************************************
**                                                                         **
** Specify number of endpoints used (in addition to EP0).                  **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 2

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 1

/****************************************************************************
**                                                                         **
** USB Mass storage class device driver definitions.                       **
**                                                                         **
*****************************************************************************/
#define MSD_INTERFACE_NO  (0)
#define MSD_BULK_OUT      (0x01)   /* Endpoint for MSD data reception.      */
#define MSD_BULK_IN       (0x81)   /* Endpoint for MSD data transmission.   */

#ifdef __cplusplus
}
#endif

#endif /* USBCONFIG_H */
