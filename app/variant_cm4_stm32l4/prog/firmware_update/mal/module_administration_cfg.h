/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update Memory Abstraction Layer
 ******************************************************************************/


#ifndef MODULEADMINISTRATION_CONFIG_H
#define MODULEADMINISTRATION_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Public definitions and declarations for unit mal_partition.
 *
 *  \details  provides interfaces and definition for partition table
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>


#if !defined(FWU_APP1_START_ADDRESS2) && !defined(FWU_FLEX_PARTITION_USED)
    #define MAL_FIX_PARTITION_ENTRIES   3u ///< number of entries in fix partition table
#elif defined(FWU_APP1_START_ADDRESS2) && defined(FWU_FLEX_PARTITION_USED)
    #define MAL_FIX_PARTITION_ENTRIES   5u ///< number of entries in fix partition table
#else
    #define MAL_FIX_PARTITION_ENTRIES   4u ///< number of entries in fix partition table
#endif

#define MAL_FLEX_PARTITION_ENTRIES      4u ///< number of entries in flex partition table
#define MAL_HW_VERSION_ENTRIES          1u ///< number of entries in HW version table


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief    Software module enumeration
 *
 *  \details  This enumeration provides an enumeration for all provided modules
 *  \note It is not recommended to modify the fixed enumerators
 *  
 */
enum MAL_ModuleEnum
//lint --e{575} "Warning 575: enumeration constant exceeds range for integers", no problem even with RL78
{
    MAL_PARTITION_TABLE = 0x5500u ///< Fixed: Module id for partition table
   ,MAL_PROGRAMMER_ID   = 0x5510u ///< Fixed: Module id for programmer
   ,MAL_BP2_LOADER_ID   = 0x5520u ///< Fixed: Module id for bp2 loader
    
   ,MAL_PRODUCT_APP1    = 0x2200u ///< Module id of product application1
   ,MAL_PRODUCT_APP2    = 0x2210u ///< Module id of product application2
   ,MAL_FLEX_PARTITION  = 0x22F0u ///< Module id of flex partition table
   ,MAL_NO_MODULE       = 0xFFFFu ///< Fixed: no valid module, used as last entry in flex table
};


/** \brief    Mal driver enumeration
 *
 */
enum MAL_DriverID_e
{
    MAL_IntFlashDriver,
  #if defined(MAL_ENABLE_OPT_FLASH_DRIVER)
    MAL_OptFlashDriver,
  #endif
  #if defined(MAL_ENABLE_DATA_FLASH_DRIVER)
    MAL_DataFlashDriver,
  #endif
  #if defined(MAL_ENABLE_SPI_FLASH_DRIVER)
    MAL_SpiFlashDriver,
  #endif
  #if defined(MAL_ENABLE_EEPROM_DRIVER)
    MAL_EepromDriver,
  #endif
    /* define your project specific driver here */
    MAL_DRIVER_COUNTER,
    MAL_NoDriver = 0xFF
};




#ifdef __cplusplus
}
#endif

#endif // MODULEADMINISTRATION_CONFIG_H
