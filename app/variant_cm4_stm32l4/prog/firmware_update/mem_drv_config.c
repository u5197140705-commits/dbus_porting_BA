/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    configuration file for general memory driver (Boot-Loader Memory Abstraction).
 *
 *  \details  definition of all access functions for all drivers.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "firmware_update/mem_drv/mem_drv.h"
#include "firmware_update/mem_drv/intflash_drv.h"
#ifdef MAL_ENABLE_OPT_FLASH_DRIVER
#include "firmware_update/mem_drv/optflash_drv.h"
#endif
#ifdef MAL_ENABLE_DATA_FLASH_DRIVER
#include "mem_drv/dataflash_drv.h"
#endif

/******************************************************************************/
/* PUBLIC DATA DEFINTIONS                                                    */
/******************************************************************************/

const struct MEMDRV_driver_s* const MEMDRV_driverTable[] =
{
    &DRVIF_driver
  #if defined(MAL_ENABLE_OPT_FLASH_DRIVER)
   ,&DRVOF_driver
  #endif
  #if defined(MAL_ENABLE_DATA_FLASH_DRIVER)
   ,&DRVDF_driver
  #endif
  #if defined(MAL_ENABLE_SPI_FLASH_DRIVER)
   ,&DRVSF_driver
  #endif
  #if defined(MAL_ENABLE_EEPROM_DRIVER)
   ,&DRVEE_driver
  #endif
};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
uint8_t MEMDRV_getDriverTableSize(void)
{
    return (uint8_t)(sizeof(MEMDRV_driverTable)/sizeof(struct MEMDRV_driver_s*));
}
