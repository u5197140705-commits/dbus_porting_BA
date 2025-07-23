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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Project specific definitions for MAL
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "firmware_update/BootManager/BootManagerModule.h"
#include "firmware_update/version/fwu_version.h"
#include "firmware_update/mal/moduleAdministration.h"
#include "firmware_update/mal/module_administration_cfg.h"
#include "firmware_update/mal/mal_driver.h"


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
#ifdef VARIANT_BOOTMANAGER
/** \brief fixed partition table located in protected ares that cannot be updated */
SYMBOL_USED const struct MAL_partition_s MAL_partitionFix[MAL_FIX_PARTITION_ENTRIES] =
{
    #if !defined (FWU3_LITE)
    // Rom-Loder (LRO)
    {
        .mod_name            = "Rom-Loader_LRO",
        .physical_start      = {.addr = FWU_LRO_START_ADDRESS},
        .physical_end        = {.addr = FWU_LRO_END_ADDRESS},
        #if defined (BM_BACKWARD_COMPATIBILITY)
        .signature           = 0u,    // no update -> no signature
        #endif //defined (BM_BACKWARD_COMPATIBILITY)
        .mod_number          = MAL_BP2_LOADER_ID,
        .driver_id           = MAL_IntFlashDriver,
        .mod_attributes      = MAL_CTX_MF_EXECUTABLE,  // execute only = no update possible
        .dep_module          = MAL_NO_MODULE
    },
    #endif // !defined (FWU3_LITE)
    // Programmer
    {
        .mod_name            = "Programmer",
        .physical_start      = {.addr = FWU_PROGRAMMER_START_ADDRESS},
        .physical_end        = {.addr = FWU_PROGRAMMER_END_ADDRESS},
        #if defined (BM_BACKWARD_COMPATIBILITY)
        .signature           = 0u,    // no update -> no signature
        #endif //defined (BM_BACKWARD_COMPATIBILITY)
        .mod_number          = MAL_PROGRAMMER_ID,
        .driver_id           = MAL_IntFlashDriver,
        .mod_attributes      = MAL_CTX_MF_EXECUTABLE,  // execute only = no update possible
        .dep_module          = MAL_NO_MODULE
    },
    // Application
    {
        .mod_name            = "Application",
        .physical_start      = {.addr = FWU_APP1_START_ADDRESS},
        .physical_end        = {.addr = FWU_APP1_END_ADDRESS},
        #if defined (BM_BACKWARD_COMPATIBILITY)
        .signature           = 0u,    // no update -> no signature
        #endif //defined (BM_BACKWARD_COMPATIBILITY)
        .mod_number          = MAL_PRODUCT_APP1,
        .driver_id           = MAL_IntFlashDriver,
        .mod_attributes      = MAL_CTX_MF_RWEX,
        .dep_module          = MAL_NO_MODULE
    }
#ifdef FWU_APP1_START_ADDRESS2
    // second part of application (in case of fragmented applications)
   ,{
        .mod_name            = "App_part2",
        .physical_start      = {.addr = FWU_APP1_START_ADDRESS2},
        .physical_end        = {.addr = FWU_APP1_END_ADDRESS2},
        #if defined (BM_BACKWARD_COMPATIBILITY)
        .signature           = 0u,    // no update -> no signature
        #endif //defined (BM_BACKWARD_COMPATIBILITY)
        .mod_number          = MAL_PRODUCT_APP2,
        .driver_id           = MAL_IntFlashDriver,
        .mod_attributes      = MAL_CTX_MF_RWE,  // it is just a part of application --> no execution
        .dep_module          = MAL_NO_MODULE
    }
#endif
};
#endif
