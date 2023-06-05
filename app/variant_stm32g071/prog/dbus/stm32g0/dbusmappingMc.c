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
 *   \brief    STM32G0 specific mapping library for D-Bus-2 data link layer using
 *             one specific UART channel.
 *
 *   \details  The mapping layer provides the implementation of the following mapping functions
 *     \li DBM_GPIO_SetBreakPinState(bool)
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

#ifdef DBM_DBUSCAN
    #define DBM_MCAL /* use MCAL with DBusCAN here */
#endif

#include "LibTypes.h"
#include "dbusmapping.h"
#include "hsup.h"
#include "hdio.h"
#ifdef DBM_MCAL
#include "mcal/mcal_includes.h"
#include "mcal_channels.h"
#endif


/******************************************************************************/
/* PRIVATE DATA AND PRIVATE MACRO FUNCTION DEFINITIONS                        */
/******************************************************************************/

/*lint -e9023 -e9024 -e9026 #/## usage, function like macro */

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY

#ifndef DBM_MCAL
    /* Macro definition for calling HDIO functions to map wake up pin (DBUS_BREAK_PIN) */
    #define DBM_GPIO_WRP(channel, function)             HDIO ##function ##channel
    #define DBM_GPIO_FUNC_ARG(channel, function, arg)   DBM_GPIO_WRP(channel, function)(arg)
#else
#ifndef DBM_BREAK_MDIO_CHANNEL
    #define DBM_BREAK_MDIO_CHANNEL    MDIOA2  // default definition
#endif
    static const struct MDIO_Channel *DBM_MDIO_breakChannel = &DBM_BREAK_MDIO_CHANNEL;
#endif // DBM_MCAL

#endif // DBUS2_NO_BREAK_FUNCTIONALITY


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
void DBM_GPIO_SetBreakPinState(bool pinState)
{
#ifdef DBM_MCAL
    ((pinState != false) ? MDIO_set(DBM_MDIO_breakChannel) : MDIO_reset(DBM_MDIO_breakChannel));
#else
    uint8_t ucPinState = (pinState != false) ? HDIO_ON : HDIO_OFF;
    DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetPort, ucPinState);
#endif
}

void DBM_GPIO_SetBreakPinModeOutput(void)
{
#ifdef DBM_MCAL
    (void)MDIO_configure(DBM_MDIO_breakChannel, &MDIO_OUTPUT);
#else
    DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetModePort, HDIO_OUTPUT);
#endif
}

void DBM_GPIO_SetBreakPinModeAlternateFunction(void)
{
#ifdef DBM_MCAL
    (void)MDIO_configure(DBM_MDIO_breakChannel, &MDIO_ALTERNATE_FUNCTION);
#else
    DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetModePort, HDIO_ALTERNATE_FUNCTION);
#endif
}
#endif  // DBUS2_NO_BREAK_FUNCTIONALITY
