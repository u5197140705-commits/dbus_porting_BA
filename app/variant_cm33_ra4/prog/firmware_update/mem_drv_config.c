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
#if defined(MAL_ENABLE_INT_FLASH_DRIVER)
    #include "firmware_update/mem_drv/intflash_drv.h"
#endif
#ifdef MAL_ENABLE_OPT_FLASH_DRIVER
    #include "firmware_update/mem_drv/optflash_drv.h"
#endif
#ifdef MAL_ENABLE_DATA_FLASH_DRIVER
    #include "firmware_update/mem_drv/dataflash_drv.h"
#endif
#ifdef MAL_ENABLE_EEPROM_DRIVER
    #include "firmware_update/mem_drv/inteeprom_drv.h"
#endif
#ifdef MAL_ENABLE_SPI_FLASH_DRIVER
    #include "firmware_update/mem_drv/extflash_drv.h"
#endif


/******************************************************************************/
/* PUBLIC DATA DEFINTIONS                                                    */
/******************************************************************************/
/*lint -esym(9003,MEMDRV_driverTable) needed external (outside MEMDRV_getDriverTableSize!)*/
/*lint -esym(552,MEMDRV_driverTable) is excessed (lintplus false positive)*/
const struct MEMDRV_driver_s* const MEMDRV_driverTable[] =
{
#ifndef VARIANT_BOOTMANAGER
  #if defined(MAL_ENABLE_INT_FLASH_DRIVER)
    &DRVIF_driver,
  #endif
  #if defined(MAL_ENABLE_OPT_FLASH_DRIVER)
    &DRVOF_driver,
  #endif
  #if defined(MAL_ENABLE_DATA_FLASH_DRIVER)
    &DRVDF_driver,
  #endif
  #if defined(MAL_ENABLE_SPI_FLASH_DRIVER)
    &DRVSF_driver,
  #endif
  #if defined(MAL_ENABLE_EEPROM_DRIVER)
    &DRVEE_driver
  #endif
#else
  /* Hint this part is relevant if you are using external memory which contains
     an updatable partition e.g. in external flash. 
   */
    NULL
  #if defined(MAL_ENABLE_OPT_FLASH_DRIVER)
   ,NULL
  #endif
  #if defined(MAL_ENABLE_DATA_FLASH_DRIVER)
   ,NULL
  #endif
  #if defined(MAL_ENABLE_SPI_FLASH_DRIVER)
   ,&DRVSF_driver
  #endif
  #if defined(MAL_ENABLE_EEPROM_DRIVER)
   ,&DRVEE_driver
  #endif
#endif
};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
uint8_t MEMDRV_getDriverTableSize(void)
{
    return (uint8_t)(sizeof(MEMDRV_driverTable)/sizeof(struct MEMDRV_driver_s*));
}
