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
#ifdef SUPPORT_OPTION_BYTES
#include "firmware_update/mem_drv/optflash_drv.h"
#endif
#ifdef SUPPORT_DATA_FLASH
#include "mem_drv/dataflash_drv.h"
#endif

/******************************************************************************/
/* PUBLIC DATA DEFINTIONS                                                    */
/******************************************************************************/
const struct MEMDRV_driver_s* const MEMDRV_driverTable[] =
{
    &DRVIF_driver
  #ifdef SUPPORT_OPTION_BYTES
   ,&DRVOF_driver
  #endif
  #ifdef SUPPORT_DATA_FLASH
   ,&DRVDF_driver
  #endif
};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
uint8_t MEMDRV_getDriverTableSize(void)
{
    return (uint8_t)(sizeof(MEMDRV_driverTable)/sizeof(struct MEMDRV_driver_s*));
}
