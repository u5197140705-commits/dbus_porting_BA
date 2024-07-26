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
 *  COMP_ABBREV      DBGP
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Definitions and declarations of configuration part of the
 *            debug_extended component: Protocol layer
 *
 *  \details  See also FNG_DebugExtendedDataUsersManual.md
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdint.h>
#include "bsh_stdinc.h"
#include "debug_extended/user_cfg.h"
#include "sts_sim_test_uty.h"


/******************************************************************************/
/* PRIVATE PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE DATA DECLARATIONS                                                  */
/******************************************************************************/

#if defined(DBGP_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGP_BIN_DAT_FUNCTIONS_ENABLED)
    //lint -esym(9003,DBGP_Protocols_Cfg) variable is not yet used outside of the block scope @@
    static uint8_t DBGP_Protocols_Cfg[] = DBGP_PROTOCOLS;
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#if defined(DBGP_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGP_BIN_DAT_FUNCTIONS_ENABLED)
void DBGP_initProtocols_Cfg(void)
{
    bool DbgAttributesActive;

#if defined DBGP_DBG_ATTRIBUTES_ENABLED
    DbgAttributesActive = true;
#else
    DbgAttributesActive = false;
#endif

    DBGP_setProtocolConfiguration(DBGP_Protocols_Cfg, (uint8_t)(sizeof(DBGP_Protocols_Cfg) / sizeof(DBGP_Protocols_Cfg[0])), DbgAttributesActive);
}
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

