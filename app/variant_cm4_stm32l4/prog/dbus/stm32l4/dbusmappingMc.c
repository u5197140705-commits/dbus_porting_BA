/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          IC Dbus2
*   COMP_ABBREV      DBM
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *   \brief    STM32L4 specific mapping library for D-Bus-2 data link layer using
 *             one specific UART channel.
 *
 *   \details  The mapping layer provides the implementation of the following mapping functions
 *     \li DBM_GPIO_SetBreakPinState(BOOL)
 *     \li DBM_GPIO_SetBreakPinModeOutput(void)
 *     \li DBM_GPIO_SetBreakPinModeAlternateFunction(void)
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"


/* compiler switches:*/
/*  DBUS2_NO_BREAK_FUNCTIONALITY*/
/*  DBUS_BREAK_PIN A9*/


#ifdef DOXY_DEVELOPERS_DOC
/* Doxygen documentation of compiler switches */

/** Compiler switch. Definition for switching off functionality that generates
    break signals on line using TX pin as GPIO, e.g. for programmer variant.
*/ #define DBUS2_NO_BREAK_FUNCTIONALITY

/** Compiler switch. Definition needed for mapping of wakeup pin (usually TX pin)
    for BAL break signal generation.
*/ #define DBUS_BREAK_PIN A9
#endif


#include "libtypes.h"
#include "dbusmapping.h"
#include "hsup.h"
#include "huart.h"
#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
#include "hdio.h"
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/*lint -e9023 -e9024 -e9026 #/## usage, function like macro */

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
/* Macro definition for calling HDIO functions to map wake up pin (DBUS_BREAK_PIN) */
#define DBM_GPIO_WRP(channel, function)             HDIO ##function ##channel
#define DBM_GPIO_FUNC_ARG(channel, function, arg)   DBM_GPIO_WRP(channel, function)(arg)
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
void DBM_GPIO_SetBreakPinState(BOOL pinState)
{
    DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetPort, pinState);
}

void DBM_GPIO_SetBreakPinModeOutput(void)
{
    DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetModePort, HDIO_OUTPUT);
}

void DBM_GPIO_SetBreakPinModeAlternateFunction(void)
{
    DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetModePort, HDIO_ALTERNATE_FUNCTION);
}
#endif

/**
 * Function called by data link layer for inserting a delay before the Acknowledge
 * in order to prevent generation of RxInterrupt immediately after the last sample
 * of the Stop bit has been performed (by single sampling this is around the middle of the bit).
 * Runtime of RxInterupt is not considered
 */
void DBM_vACK_Delay(void)
{
    HSUP_vNop();
}

