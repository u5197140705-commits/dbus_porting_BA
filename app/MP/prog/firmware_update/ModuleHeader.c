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
#include "firmware_update/hw_id_supported.h"
#include "SegmentDef.h"
#include "intTbl.h"



/******************************************************************************/
/* PRIVATE DATA DEFINTIONS                                                    */
/******************************************************************************/
/*lint -esym(528,MDHD_*) "Symbol '...' not referenced"
 *     referenced by boot manager!
 */

#ifdef SCTY_AUTO_APPROVED_FLAG
SDEF_SetSegmentConst(FWU_SIGNATURE)
/*lint -e{785} "partially initialized struct MisraC2012 9.3 required"
 *      partially initialized by intention
 */
static SYMBOL_USED SDEF_FAR const struct SCTY_signature_s MDHD_signature = 
{
    .EdDSA = {0},
    .sigApproved =  SCTY_APPROVED
};
SDEF_SetSegmentConst_Default()
#endif



static const struct hw_id_array_s MAL_HwIdSupport[MAL_SUPPORTED_HW_ID_COUNT] = 
{
    {FWU_HW_ID_SUPPORT1},
    {FWU_HW_ID_SUPPORT2},
    {FWU_HW_ID_SUPPORT3},
    {FWU_HW_ID_SUPPORT4},
    {FWU_HW_ID_SUPPORT5},
    {FWU_HW_ID_SUPPORT6},
    {FWU_HW_ID_SUPPORT7},
    {FWU_HW_ID_SUPPORT8},
    {FWU_HW_ID_SUPPORT9},
    {FWU_HW_ID_SUPPORT10},
    {FWU_HW_ID_SUPPORT11},
    {FWU_HW_ID_SUPPORT12},
    {FWU_HW_ID_SUPPORT13},
    {FWU_HW_ID_SUPPORT14},
    {FWU_HW_ID_SUPPORT15},
    {FWU_HW_ID_SUPPORT16}
};


SDEF_SetSegmentConst(FWU_SW_MODULE_HEADER) 
static SYMBOL_USED const struct MAL_ModuleHeader_s MDHD_ModuleHeader =
{
    .crc_expected = 0x00, // will be replaced by patchbin!!
  #ifndef __ICCRL78__
    .ISRtbl_ptr   = ITBL_coreVectors,
  #else
    .ISRtbl_ptr   = (void SDEF_NEAR *)(sizeof(struct MAL_ModuleHeader_s)+FWU_APP1_START_ADDRESS), //lint !e923 cast tested to work correctly on RL78
  #endif
    .size         = (uint32_t)((FWU_APP1_END_ADDRESS-FWU_APP1_START_ADDRESS)+1u),
    .executionAddress = FWU_APP1_START_ADDRESS,
    .sw_version =
    {
     .version =
        { /** \todo please specify id-string for your application */
            .ID       = {0,1,2,3,4,5,6,7},
            .major    = {0x12, 0x34},
            .minor    = {0x98, 0x76},
            .revision = {0xfe, 0xdc},
            .build    = {0xab, 0xcd, 0xef, 0x01}
        }
    },
    .hw_id_array = MAL_HwIdSupport
};
SDEF_SetSegmentConst_Default()


#ifdef FWU_APP1_START_ADDRESS2
SDEF_SetSegmentConst(FWU_SW_MODULE_HEADER2) 
static SYMBOL_USED const struct MAL_ModuleHeader_s MDHD_ModuleHeader2 =
{
    .crc_expected = 0x00,   // unused
    .ISRtbl_ptr   = NULL,   // unused
    .size         = (uint32_t)((FWU_APP1_END_ADDRESS2-FWU_APP1_START_ADDRESS2)+1u),
    .executionAddress = 0u, // unused
    .sw_version   = {{{0}}},// unused
    .hw_id_array  = NULL    // unused
};
SDEF_SetSegmentConst_Default()
#endif

