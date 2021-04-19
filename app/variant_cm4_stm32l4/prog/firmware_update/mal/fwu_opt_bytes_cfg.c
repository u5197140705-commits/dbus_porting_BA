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
#include "BSH_stdinc.h"
#include <stdint.h>
#include "firmware_update/mal/fwu_opt_bytes.h"
#include "SegmentDef.h"
#include "std_lib/std_symbol.h"


/** \brief    Option bytes intended for users.
 *
 *  \details  securityConfig: enable/disable suppress of signature(Auth. boot)
 *                MAL_SECURITY_RELEASE_KEY  - check is enabled, RELEASE key is used
 *                MAL_SECURITY_DEVELOP_KEY  - check is enabled, DEVELOP key is used
 *                MAL_SECURITY_IGNORE_ERROR - check error is ignored but calculation is executed
 *                MAL_SECURITY_DISABLE_CALC - check is complete disabled (errors + calculation)
 *
 *            integrityConfig: enable/disable suppress of CRC (FS ROM Check)
 *                MAL_INTEGRITY_ENABLE       - check is enabled
 *                MAL_INTEGRITY_IGNORE_ERROR - check error is ignored but calculation is executed
 *                MAL_INTEGRITY_DISABLE_CALC - check is complete disabled (errors + calculation)
 *
 *            compatibilityConfig: enable/suppress of hardware Id validation
 *                MAL_COMPATIBILITY_ENABLE       - check is enabled
 *                MAL_COMPATIBILITY_IGNORE_ERROR - check error is ignored
 *
 *  \note     When build_type = RELEASE then opt. bytes are set to default state:
 *                .securityConfig      = MAL_SECURITY_RELEASE_KEY,
 *                .integrityConfig     = MAL_INTEGRITY_ENABLE,
 *                .compatibilityConfig = MAL_COMPATIBILITY_ENABLE,
 *                .reserved            = MAL_RESERVED
 */
#ifdef REMOTE_FIRMWARE_UPDATE
    SDEF_SetSegmentConst(FWU_OPT_BYTES)
#endif
    SYMBOL_USED SDEF_FAR const struct MAL_FwuOptBytes_s MAL_FwuOptBytes =
    {
        .securityConfig      = MAL_SECURITY_DEVELOP_KEY,
        .integrityConfig     = MAL_INTEGRITY_ENABLE,
        .compatibilityConfig = MAL_COMPATIBILITY_ENABLE,
        .reserved            = MAL_RESERVED
    };
#ifdef REMOTE_FIRMWARE_UPDATE
    SDEF_SetSegmentConst_Default()
#endif


