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
#include "BSH_stdinc.h"
#include <stdint.h>
#include "firmware_update/BootManager/BootManagerModule.h"
#include "firmware_update/fwu_version.h"
#include "firmware_update/mal/moduleAdministration.h"
#include "firmware_update/mal/module_administration_cfg.h"
#include "firmware_update/mal/mal_driver.h"
#include "firmware_update/hw_id_supported.h"


/******************************************************************************/
/* PUBLIC DATA DEFINTIONS                                                     */
/******************************************************************************/
#ifdef FWU_FLEX_PARTITION_USED
SDEF_SetSegmentConst(FWU_FLEX_PARTITION_HEADER)

SYMBOL_USED const struct MAL_ModuleHeader_s MAL_partitionFlexHeader =
{
    .crc_expected = 0x00, // will be replaced by patchbin!!
    .ISRtbl_ptr   = NULL,
    .size         = (uint32_t)((FWU_FLEX_PARTITION_END_ADDRESS-FWU_FLEX_PARTITION_START_ADDRESS)+1u),
    .executionAddress = FWU_FLEX_PARTITION_START_ADDRESS,
    .sw_version =
    {
     .version =
        { /** \todo please specify id-string for your flex partition */
            .ID       = {0,1,2,3,4,5,6,7},
            .major    = {0x12, 0x34},
            .minor    = {0x98, 0x76},
            .revision = {0xfe, 0xdc},
            .build    = {0xab, 0xcd, 0xef, 0x01}
        }
    },
    .hw_id_array = NULL
};

SDEF_SetSegmentConst(FWU_FLEX_PARTITION)
/** \brief flex partition table that can be updated */
SYMBOL_USED const struct MAL_partition_s MAL_partitionFlex[MAL_FLEX_PARTITION_ENTRIES] =
{
#ifndef FWU_APP1_START_ADDRESS2
    [0] = // Application
    {
        .mod_name            = "Application",
        .physical_start      = {.addr = FWU_APP1_START_ADDRESS},
        .physical_end        = {.addr = FWU_APP1_END_ADDRESS},
        .signature           = {.addr = MAL_CALC_SIG_ADDR(FWU_APP1_END_ADDRESS)},
        .mod_number          = MAL_PRODUCT_APP1,
        .driver_id           = MAL_IntFlashDriver,
        .mod_atributes       = MAL_CTX_MF_RWEX,
        .dep_module          = MAL_NO_MODULE
    },
    [1] =
    {
        .mod_name            = "",
        .physical_start      = {.addr = 0u},
        .physical_end        = {.addr = 0u},
        .signature           = {.addr = 0u},
        .mod_number          = MAL_NO_MODULE,
        .driver_id           = MAL_NoDriver,
        .mod_atributes       = 0u,
        .dep_module          = MAL_NO_MODULE
    },
#else
    [0] = // fragmented Application
    {
        .mod_name            = "Application",
        .physical_start      = {.addr = FWU_APP1_START_ADDRESS},
        .physical_end        = {.addr = FWU_APP1_END_ADDRESS},
        .signature           = {.addr = MAL_CALC_SIG_ADDR(FWU_APP1_END_ADDRESS)},
        .mod_number          = MAL_PRODUCT_APP1,
        .driver_id           = MAL_IntFlashDriver,
        .mod_atributes       = MAL_CTX_MF_RWEX,
        .dep_module          = MAL_PRODUCT_APP2
    },
    [1] =
    {    // second fragmented of application
        .mod_name            = "App_part2",
        .physical_start      = {.addr = FWU_APP1_START_ADDRESS2},
        .physical_end        = {.addr = FWU_APP1_END_ADDRESS2},
        .signature           = {.addr = 0u}, // signature in first part of application only
        .mod_number          = MAL_PRODUCT_APP2,
        .driver_id           = MAL_IntFlashDriver,
        .mod_atributes       = MAL_CTX_MF_RWEXX,  // it is just a part of application --> no execution
        .dep_module          = MAL_NO_MODULE
    },
#endif
    [2] =
    {
        .mod_name            = "",
        .physical_start      = {.addr = 0u},
        .physical_end        = {.addr = 0u},
        .signature           = {.addr = 0u},
        .mod_number          = MAL_NO_MODULE,
        .driver_id           = MAL_NoDriver,
        .mod_atributes       = 0u,
        .dep_module          = MAL_NO_MODULE
    },
    [3] =
    {
        .mod_name            = "",
        .physical_start      = {.addr = 0u},
        .physical_end        = {.addr = 0u},
        .signature           = {.addr = 0u},
        .mod_number          = MAL_NO_MODULE,
        .driver_id           = MAL_NoDriver,
        .mod_atributes       = 0u,
        .dep_module          = MAL_NO_MODULE
    }
};
SDEF_SetSegmentConst_Default()
#endif


#ifdef VARIANT_BOOTMANAGER
/** \brief fixed partition table located in protected ares that cannot be updated */
SYMBOL_USED SDEF_FAR const struct MAL_partition_s MAL_partitionFix[MAL_FIX_PARTITION_ENTRIES] =
{
    // Rom-Loder (LRO)
    {
        .mod_name            = "Rom-Loader_LRO",
        .physical_start      = {.addr = FWU_LRO_START_ADDRESS},
        .physical_end        = {.addr = FWU_LRO_END_ADDRESS},
        .signature           = {.addr = 0u},    // no update -> no signature
        .mod_number          = MAL_BP2_LOADER_ID,
        .driver_id           = MAL_NoDriver,    // no update possible
        .mod_atributes       = MAL_CTX_MF_EXECUTABLE,  // execute only
        .dep_module          = MAL_NO_MODULE
    },
    // Programmer
    {
        .mod_name            = "Programmer",
        .physical_start      = {.addr = FWU_PROGRAMMER_START_ADDRESS},
        .physical_end        = {.addr = FWU_PROGRAMMER_END_ADDRESS},
        .signature           = {.addr = 0u},    // no update -> no signature
        .mod_number          = MAL_PROGRAMMER_ID,
        .driver_id           = MAL_NoDriver,    // no update possible
        .mod_atributes       = MAL_CTX_MF_EXECUTABLE,  // execute only
        .dep_module          = MAL_NO_MODULE
    },
    // Application
    {
        .mod_name            = "Application",
        .physical_start      = {.addr = FWU_APP1_START_ADDRESS},
        .physical_end        = {.addr = FWU_APP1_END_ADDRESS},
        .signature           = {.addr = MAL_CALC_SIG_ADDR(FWU_APP1_END_ADDRESS)},
        .mod_number          = MAL_PRODUCT_APP1,
        .driver_id           = MAL_IntFlashDriver,
        .mod_atributes       = MAL_CTX_MF_RWEX,
        .dep_module          = MAL_NO_MODULE
    }
#ifdef FWU_APP1_START_ADDRESS2
    // second part of application (in case of fragmented applications)
   ,{
        .mod_name            = "App_part2",
        .physical_start      = {.addr = FWU_APP1_START_ADDRESS2},
        .physical_end        = {.addr = FWU_APP1_END_ADDRESS2},
        .signature           = {.addr = 0u}, // signature in first part of application only
        .mod_number          = MAL_PRODUCT_APP2,
        .driver_id           = MAL_IntFlashDriver,
        .mod_atributes       = MAL_CTX_MF_RWE,  // it is just a part of application --> no execution
        .dep_module          = MAL_NO_MODULE
    }
#endif
#ifdef FWU_FLEX_PARTITION_USED
    // Flex Partition table
   ,{
        .mod_name            = "FlexPartition",
        .physical_start      = {.addr = FWU_FLEX_PARTITION_START_ADDRESS},
        .physical_end        = {.addr = FWU_FLEX_PARTITION_END_ADDRESS},
        .signature           = {.addr = MAL_CALC_SIG_ADDR(FWU_FLEX_PARTITION_END_ADDRESS)},
        .mod_number          = MAL_FLEX_PARTITION,
        .driver_id           = MAL_IntFlashDriver,
        .mod_atributes       = MAL_CTX_MF_RWE,
        .dep_module          = MAL_NO_MODULE

    }
#endif
};
#endif
