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
 *  \brief    Implementation of option bytes for firmware update.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "firmware_update/mal/fwu_opt_bytes.h"
#include "SegmentDef.h"
#include "std_lib/std_symbol.h"


/** \brief    Option bytes intended for users.
 *
 *  \details  integrityConfig: enable/disable suppress of CRC (FS ROM Check)
 *                MAL_INTEGRITY_ENABLE       - check is enabled
 *                MAL_INTEGRITY_IGNORE_ERROR - check error is ignored but calculation is executed
 *                MAL_INTEGRITY_DISABLE_CALC - check is complete disabled (errors + calculation)
 *
 *            compatibilityConfig: enable/suppress of hardware Id validation
 *                MAL_COMPATIBILITY_ENABLE       - check is enabled
 *                MAL_COMPATIBILITY_IGNORE_ERROR - check error is ignored
 *
 *  \note     When build_type = RELEASE then opt. bytes are set to default state:
 *                .integrityConfig     = MAL_INTEGRITY_ENABLE,
 *                .compatibilityConfig = MAL_COMPATIBILITY_ENABLE,
 *                .reserved            = MAL_RESERVED
 */
#ifdef REMOTE_FIRMWARE_UPDATE
    SDEF_SetSegmentConst(FWU_OPT_BYTES)
#endif
    SYMBOL_USED const struct MAL_FwuOptBytes_s MAL_FwuOptBytes =
    {
        .integrityConfig     = MAL_INTEGRITY_ENABLE,
        .compatibilityConfig = MAL_COMPATIBILITY_ENABLE,
        .reserved            = {MAL_RESERVED,MAL_RESERVED}
    };
#ifdef REMOTE_FIRMWARE_UPDATE
    SDEF_SetSegmentConst_Default()
#endif


