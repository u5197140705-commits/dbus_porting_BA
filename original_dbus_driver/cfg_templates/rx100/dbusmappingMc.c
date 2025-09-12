/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          DBus2
*   COMP_ABBREV      DBM
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *   \brief    RX100 specific mapping library for D-Bus-2 data link layer using
 *             one specific UART channel.
 *
 *   \details  The mapping layer provides the implementation of the following mapping functions
 *     \li DBM_INT_ClearIrq(void)
 *     \li DBM_INT_Init(uint8_t ucIndex)
 *     \li DBM_INT_IsIrqEvent(void)
 *     \li DBM_INT_SetIrqModeFalling(void)
 *     \li DBM_INT_bGetPin (void)
 *     \li DBM_GPIO_SetBreakPinState(bool)
 *     \li DBM_GPIO_SetBreakPinModeOutput(void)
 *     \li DBM_GPIO_SetBreakPinModeAlternateFunction(void)
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"


/* compiler switches:*/
/*  __DBM_USE_IRQ_FOR_IDLE_DETECTION*/
/*  DBUS_UART_CHANNEL 0*/
/*  DBUS2_NO_BREAK_FUNCTIONALITY*/
/*  DBUS_BREAK_PIN A9*/


#ifdef DOXY_DEVELOPERS_DOC
/* Doxygen documentation of compiler switches */

/** Compiler switch. When defined an external interrupt is used for
    determining IDLE state. This is useful for controller, which don't have
    ReceiveOngoing or IDLE flags available.
*/ #define __DBM_USE_IRQ_FOR_IDLE_DETECTION

/** Compiler switch. Definition needed for mapping D-Bus-2 mapping layer to a specific DBUS_UART_CHANNEL. This defined D-Bus-2 for usage of the first UART channel.
*/ #define DBUS_UART_CHANNEL 0

/** Compiler switch. Definition for switching off functionality that generates
    break signals on line using TX pin as GPIO, e.g. for programmer variant.
*/ #define DBUS2_NO_BREAK_FUNCTIONALITY

/** Compiler switch. Definition needed for mapping of pin that is used for
    generating of break signal on DBus line (usually TX pin).
*/ #define DBUS_BREAK_PIN A9
#endif


#include "LibTypes.h"
#include "dbusmapping.h"
#include "hsup.h"
#include "huart.h"
#ifdef __DBM_USE_IRQ_FOR_IDLE_DETECTION
#include "hint.h"
#endif
#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
#include "hdio.h"
#endif


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/*lint -e9023 -e9024 -e9026 #/## usage, function like macro */

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
#define DBUS_BREAK_PIN    C6
/* Macro definition for calling HDIO functions to map wake up pin (DBUS_BREAK_PIN) */
#define DBM_GPIO_WRP(channel, function)             HDIO ##function ##channel
#define DBM_GPIO_FUNC_ARG(channel, function, arg)   DBM_GPIO_WRP(channel, function)(arg)
#endif

#ifdef __DBM_USE_IRQ_FOR_IDLE_DETECTION

/*Macro definitions for building names of functions to map according to used channel (DBM_UART_CHANNEL)*/
#define _DBM_INT_FUNC(channel, function)  HINT ## channel ##function
#define DBM_INT_FUNC(channel, function) _DBM_INT_FUNC(channel, function())
#define DBM_INT_FUNC_ARG(channel, function, arg) _DBM_INT_FUNC(channel, function(arg))

/* to be done */

#error "No idle detection implemnted for selected UART channel"

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

/** Clear interrupt pending flag. */
void DBM_INT_ClearIrq(void)
{
    DBM_INT_FUNC(DBUS_HINT_CHANNEL,_vClearEvent);
}

/** Enable the used hardware interrupt */
void DBM_INT_Init(uint8_t ucIndex)
{
    (void)ucIndex;
    DBM_INT_FUNC_ARG(DBUS_HINT_CHANNEL,_vInit,0);
}

/** Is interrupt pending? */
bool DBM_INT_IsIrqEvent(void)
{
    return DBM_INT_FUNC(DBUS_HINT_CHANNEL,_bIsEvent);
}

/** Interrupt pending only on falling edge      */
void DBM_INT_SetIrqModeFalling(void)
{
    DBM_INT_FUNC_ARG(DBUS_HINT_CHANNEL,_vSetMode,(uint8_t)HINT_FALLING_EDGE);
}


bool DBM_INT_bGetPin(void)
{
    return DBM_INT_FUNC(DBUS_HINT_CHANNEL,_bGetPin);
}

#endif

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
void DBM_GPIO_SetBreakPinState(bool pinState)
{
    uint8_t ucPinState = (pinState != false) ? HDIO_ON : HDIO_OFF;
    DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetPort, ucPinState);
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
