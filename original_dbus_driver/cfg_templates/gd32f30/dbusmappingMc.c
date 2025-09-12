/*******************************************************************************
*   Copyright (c) 2023 BSH Hausgeraete GmbH,
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
/** \file      dbusmappingMc.c
 *
 *   \brief    GD32F33 specific mapping library for D-Bus-2 data link layer using
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

#ifdef DOXY_DEVELOPERS_DOC
/* Doxygen documentation of compiler switches */

/** Compiler switch. Definition for switching off functionality that generates
    break signals on line using TX pin as GPIO, e.g. for programmer variant.
*/ #define DBUS2_NO_BREAK_FUNCTIONALITY
#endif


#include "libtypes.h"
#include "dbusmapping.h"
#include "hsup.h"
#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
#include "mcal_channels.h"
#endif


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
void DBM_GPIO_SetBreakPinState(bool pinState)
{
    if(pinState == true)
    {
        MDIO_set(&DBM_BREAK_MDIO_CHANNEL);
    }
    else
    {
        MDIO_reset(&DBM_BREAK_MDIO_CHANNEL);
    }
}

void DBM_GPIO_SetBreakPinModeOutput(void)
{
    const struct MDIO_Port *port = MDIO_getPort(&DBM_BREAK_MDIO_CHANNEL);
    enum MDIO_Pin pin            = MDIO_getPin(&DBM_BREAK_MDIO_CHANNEL);
    MDIO_configureMode(port, pin, MDIO_MODE_DIGITAL_OUTPUT);

}

void DBM_GPIO_SetBreakPinModeAlternateFunction(void)
{
    const struct MDIO_Port *port = MDIO_getPort(&DBM_BREAK_MDIO_CHANNEL);
    enum MDIO_Pin pin            = MDIO_getPin(&DBM_BREAK_MDIO_CHANNEL);
    MDIO_configureMode(port, pin, MDIO_MODE_ALTERNATE_FUNCTION);
}

#endif // !DBUS2_NO_BREAK_FUNCTIONALITY
