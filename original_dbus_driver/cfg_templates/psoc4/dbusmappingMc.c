/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          IC Dbus2
 *  PROCESSOR        PSoC4
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *   \brief    PSoC4 specific mapping library for D-Bus-2 data link layer.
 *
 *   \details  The mapping layer provides the implementation of the following mapping functions
 *     \li DBM_INT_DisableIrq(void);
 *     \li DBM_INT_ClearIrq(void)
 *     \li DBM_INT_Init(uint8_t ucIndex)
 *     \li DBM_INT_IsIrqEvent(void)
 *     \li DBM_INT_SetIrqModeFalling(void)
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
/*  DBUS_BREAK_PIN HUART_TX*/


#ifdef DOXY_DEVELOPERS_DOC
/* Doxygen documentation of compiler switches */

/** Compiler switch. Definition for switching off functionality that generates
    break signals on line using TX pin as GPIO, e.g. for programmer variant.
*/ #define DBUS2_NO_BREAK_FUNCTIONALITY

/** Compiler switch. Definition needed for mapping of wakeup pin (usually TX pin)
    for BAL break signal generation.
*/ #define DBUS_BREAK_PIN HUART_TX
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
#define DBUS_BREAK_PIN    HUART0_TX
/* Macro definition for calling HDIO functions to map wake up pin (DBUS_BREAK_PIN) */
#define DBM_GPIO_WRP(channel, function)             HDIO ##function ##channel
#define DBM_GPIO_FUNC_ARG(channel, function, arg)   DBM_GPIO_WRP(channel, function)(arg)
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


#ifdef __DBM_USE_IRQ_FOR_IDLE_DETECTION


/** Disable the used hardware interrupt */
void DBM_INT_DisableIrq(void)
{
    HUART0_RX_SetInterruptMode(HUART0_RX_0_INTR, HUART0_RX_INTR_NONE);
}

/** Clear interrupt pending flag. */
void DBM_INT_ClearIrq(void)
{
    (void)HUART0_RX_ClearInterrupt();
}

/** Enable the used hardware interrupt */
void DBM_INT_Init(uint8_t ucIndex)
{
    (void)ucIndex;
}

/** Is interrupt pending? */
bool DBM_INT_IsIrqEvent(void)
{
    return (HUART0_RX_ClearInterrupt() != 0U);
}

/** Interrupt pending only on falling edge      */
void DBM_INT_SetIrqModeFalling(void)
{
    HUART0_RX_SetInterruptMode(HUART0_RX_0_INTR, HUART0_RX_INTR_FALLING);
}

bool DBM_INT_bGetPin(void)
{
    return (HUART0_RX_Read() != 0U);
}

#endif //__DBM_USE_IRQ_FOR_IDLE_DETECTION

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
void DBM_GPIO_SetBreakPinState(bool pinState)
{
    uint8_t ucPinState = (pinState != false) ? 1U : 0U;
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
