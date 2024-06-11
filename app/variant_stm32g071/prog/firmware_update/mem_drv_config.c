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

#define MAL_NO_BP2_OFFSET 0u
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                   */
/******************************************************************************/
/*lint -esym(9003,MEMDRV_driverTable) needed external (outside MEMDRV_getDriverTableSize!)*/
/*lint -esym(552,MEMDRV_driverTable) is excessed (lintplus false positive)*/
const struct MEMDRV_driver_entry_s MEMDRV_driverTable[] =
{
/* --------------External memory drivers------------------- */
#if defined(MAL_ENABLE_SPI_FLASH_DRIVER)
    {&DRVSF_driver, MAL_SpiFlashDriver, MAL_NO_BP2_OFFSET},
#endif
#if defined(MAL_ENABLE_EEPROM_DRIVER)
    {&DRVEE_driver, MAL_EepromDriver, MAL_NO_BP2_OFFSET},
#endif
/* +++++++++ Add your external memory driver here +++++++++ */
 /* {DRVXX_driver, MAL_myExternalMemDriver, MAL_NO_BP2_OFFSET},  */

#ifndef VARIANT_BOOTMANAGER
/* --------------Internal memory drivers------------------- *
 * For internal memory boot manager does not need a driver, *
 * as just read-only access is needed                       *
 * -------------------------------------------------------- */
    #if defined(MAL_ENABLE_INT_FLASH_DRIVER)
        {&DRVIF_driver, MAL_IntFlashDriver, MAL_NO_BP2_OFFSET},
    #endif
    #if defined(MAL_ENABLE_OPT_FLASH_DRIVER)
        {&DRVOF_driver, MAL_OptFlashDriver, MAL_NO_BP2_OFFSET},
    #endif
    #if defined(MAL_ENABLE_DATA_FLASH_DRIVER)
        {&DRVDF_driver, MAL_DataFlashDriver, MAL_NO_BP2_OFFSET},
    #endif
#endif
    {NULL, MAL_NoDriver, MAL_NO_BP2_OFFSET} // do not remove end mark !!
};
