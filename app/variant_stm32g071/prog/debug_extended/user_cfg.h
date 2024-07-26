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
 *  COMP_ABBREV      DBG
 ******************************************************************************/

#ifndef DEBUG_EXTENDED_USER_CFG_H
#define DEBUG_EXTENDED_USER_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of configuration part of
 *            the debug_extended component: General user configuration
 *
 *  \details  See also FNG_DebugExtendedDataUsersManual.md
 */

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS, PART 1                                    */
/******************************************************************************/

/******************************
* Activation switch for text data functions DBGX_log... and DBGX_getString: */

#define DBGX_USR_TEXT_DAT_FUNCTIONS_ENABLED
            /* Enable the processing of debug_extended data by those functions */
            /* Defined:     Enabled */
            /* Not defined: Not enabled */
            /* Normally defined */

/******************************
* Activation switch for binary data functions DBGX_sendData and the function
* notified by DBGX_notifyReceiveData_CB: */

//#define DBGX_USR_BIN_DAT_FUNCTIONS_ENABLED
            /* Enable the writing of debug_extended data by those functions */
            /* Defined:     Enabled */
            /* Not defined: Not enabled */
            /* Normally not defined */

/******************************
* Activation switch for the DbgAttributes */

#define DBGX_USR_DBG_ATTRIBUTES_ENABLED
            /* Enable the debug attributes at the beginning of each api function */
            /* Defined:     Enabled */
            /* Not defined: Not enabled */
            /* Normally defined */

/******************************
* Activation switches for the error handling */

#define DBGX_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED
            /* Exceptional error functions enabling */
            /* Macros: DBGX_handleErr... and DBGX_handleErrIf... */
            /* Defined:     Functions are activated (API macros call the functions) */
            /* Not defined: Functions are not activated (API macros call nothing */
            /* Normally defined */

//#define DBGX_EXCEPTIONAL_ERROR_SOURCE_LOCATION_ACTIVATED
            /* Output of source locations enabling for exceptional error functions */
            /* Defined:     Source locations are activated */
            /* Not defined: Source locations are not activated */
            /* Normally not defined (saves much memory, comfort function, source */
            /* locations also identifiable by the unique exceptional error code) */

/******************************
* Activation switch for the interface input */

//#define DBGX_INPUT_FUNCTIONS_ENABLED
            /* Enable the interface input by those functions */
            /* Defined:     Enabled */
            /* Not defined: Not enabled */
            /* Normally not defined */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "debug_extended/api_cfg.h"
#include "debug_extended/protocol_cfg.h"
#include "debug_extended/interface_cfg.h"


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS, PART 2                                    */
/******************************************************************************/

#if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED)

    /* Choose the interface which should be used for the debug output data.
     * You can choose between following definitions:
     *      DBGP_PROTOCOL_TEXT,
     *      DBGP_PROTOCOL_TCH_DER,
     *      DBGP_PROTOCOL_NONE
     * Note: Some interfaces require a configuration in interface_cfg.h.
     */
    #define DBGX_USR_PROTOCOLS {   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_NONE            0 */   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_DBGX_UART       1 */   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_DBUS            2 */   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_RAM             3 */   \
        DBGP_PROTOCOL_TEXT    /* via DBGI_INTERFACE_SIMPLE_DEBUG    4 */   \
    }

#endif


#if defined(DBGX_INPUT_FUNCTIONS_ENABLED)

    /* Define the interface which should be used for the debug input data.
     * You can choose between following definitions:
     *      DBGI_INTERFACE_NONE
     *      DBGI_INTERFACE_DBGX_UART
     *      DBGI_INTERFACE_DBUS
     *      DBGI_INTERFACE_SIMPLE_DEBUG
     * Note: Some interfaces require a configuration in interface_cfg.h.
     */
    #define DBGP_PROTOCOL_INPUT_DATA  DBGI_INTERFACE_NONE

#endif


#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_USER_CFG_H */

