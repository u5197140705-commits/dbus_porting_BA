/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of Module Header
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "std_lib/std_symbol.h"
#include "firmware_update/mal/moduleAdministration.h"
#include "SegmentDef.h"
#include "IntTbl.h"
#include "build_version.h"



/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/*lint -esym(528,MDHD_*) "Symbol '...' not referenced"
 *     referenced by boot manager!
 */


SDEF_SetSegmentConst(FWU_SW_MODULE_HEADER) 
static SYMBOL_USED const struct MAL_ModuleHeader_s MDHD_ModuleHeader =
{
    .crc_expected = 0u, // will be replaced by patchBin!!
  #if !defined(__ICCRx__)
    .ISRtbl_ptr   = ITBL_coreVectors,
  #else
    .ISRtbl_ptr   = (const MAL_isrPtr_t *)(sizeof(struct MAL_ModuleHeader_s)+FWU_APP1_START_ADDRESS), //lint !e923 cast tested to work correctly on RLX
  #endif
    .size         = (uint32_t)((FWU_APP1_END_ADDRESS-FWU_APP1_START_ADDRESS)+1u),
    .executionAddress = FWU_APP1_START_ADDRESS,
    .sw_version =
    {
     .version =
        {
            .ID       = STDV_ParseIdentifier(UID),
            .major    = STDV_ParseVersion(VERSION_MAJOR),
            .minor    = STDV_ParseVersion(VERSION_MINOR),
            .revision = STDV_ParseVersion(VERSION_REVISION),
            .build    = STDV_ParseVersionBuild(VERSION_BUILD)
        }
    },
    .reserved = 0u,
    .hw_id_array = NULL
};
SDEF_SetSegmentConst_Default()


#ifdef FWU_APP1_START_ADDRESS2
SDEF_SetSegmentConst(FWU_SW_MODULE_HEADER2) 
static SYMBOL_USED const struct MAL_ModuleHeader_s MDHD_ModuleHeader2 =
{
    .crc_expected = 0u, // will be replaced by patchBin!!
    .ISRtbl_ptr   = NULL,   // unused
    .size         = (uint32_t)((FWU_APP1_END_ADDRESS2-FWU_APP1_START_ADDRESS2)+1u),
    .executionAddress = 0u, // unused
    .sw_version =
    {
     .version =
        {
            .ID       = STDV_ParseIdentifier(UID),
            .major    = STDV_ParseVersion(VERSION_MAJOR),
            .minor    = STDV_ParseVersion(VERSION_MINOR),
            .revision = STDV_ParseVersion(VERSION_REVISION),
            .build    = STDV_ParseVersionBuild(VERSION_BUILD)
        }
    },
    .reserved = 0u,
    .hw_id_array = NULL
};
SDEF_SetSegmentConst_Default()
#endif

