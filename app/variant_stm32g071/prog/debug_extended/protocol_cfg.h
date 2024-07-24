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

#ifndef DEBUG_EXTENDED_PROTOCOL_CFG_H
#define DEBUG_EXTENDED_PROTOCOL_CFG_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of configuration part of
 *            the debug_extended component: Protocol layer
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "debug_extended/protocol.h"
#include "debug_extended/user_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - ACTIVATION SWITCHES                      */
/******************************************************************************/

/* Do not change the activation switches here. Use the global switches from user_cfg.h  */
#ifdef DBGX_USR_TEXT_DAT_FUNCTIONS_ENABLED
    #define DBGP_TEXT_DAT_FUNCTIONS_ENABLED
#endif

#ifdef DBGX_USR_BIN_DAT_FUNCTIONS_ENABLED
    #define DBGP_BIN_DAT_FUNCTIONS_ENABLED
#endif

#ifdef DBGX_USR_DBG_ATTRIBUTES_ENABLED
    #define DBGP_DBG_ATTRIBUTES_ENABLED
#endif


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - USER CONFIGURATION                       */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - INTERNAL CONFIGURATION                   */
/******************************************************************************/

#if defined(DBGP_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGP_BIN_DAT_FUNCTIONS_ENABLED)
    #define DBGP_PROTOCOLS   DBGX_USR_PROTOCOLS
#endif


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#if defined(DBGP_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGP_BIN_DAT_FUNCTIONS_ENABLED)
    /** \brief  Initializes the debug_extended protocols
     *  \note   Called by DBGX_init()
     */
    extern void DBGP_initProtocols_Cfg(void);
#endif


#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_PROTOCOL_CFG_H */

