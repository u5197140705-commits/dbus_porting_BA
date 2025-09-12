/*******************************************************************************
*   Copyright (c) 2022 BSH Hausgeraete GmbH,
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
/** \file       dbusmappingMc.c
 *
 *   \brief     Renesas RA2 specific mapping library for D-Bus-2 data link 
 *              layer using one specific UART channel.
 *
 *   \details   The mapping layer provides the implementation of the following mapping functions:
 *     \li DBM_GPIO_SetBreakPinState(bool)
 *     \li DBM_GPIO_SetBreakPinModeOutput(void)
 *     \li DBM_GPIO_SetBreakPinModeAlternateFunction(void)
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "libtypes.h"
#include "dbusmapping.h"
#include "hsup.h"

#if defined(MCAL_MEXTI_INCLUDED)
#include "mcal/mexti.h"
#include "mcal_channels.h"
#endif

/* compiler switches:*/
/* TDB */

#ifdef DOXY_DEVELOPERS_DOC
/* TBD */

#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#if defined(MCAL_MEXTI_INCLUDED)
static const struct MEXTI_Channel *DBM_MEXTI_IN1 = &DBM_IDLE_MEXTI_CHANNEL;
static struct MEXTI_Handle DBM_MextiHandle1;
static MCAL_Callback_t DBM_MextiCallback1;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/** Enable the used hardware interrupt */
void DBM_INT_EnableIrq(void)
{
    MEXTI_enableEvent(&DBM_MextiHandle1, &DBM_MextiCallback1);
}

/** Disable the used hardware interrupt */
void DBM_INT_DisableIrq(void)
{
    if(DBM_MextiHandle1.enabledEvent == true)   // if MEXTI already initialized
    {
        MEXTI_disableEvent(&DBM_MextiHandle1, &DBM_MextiCallback1);
    }
}

/** Clear interrupt pending flag. */
void DBM_INT_ClearIrq(void)
{
    MEXTI_clearEventReg(DBM_MextiHandle1.channel);
}

/** Enable the used hardware interrupt */
void DBM_INT_Init(uint8_t ucIndex)
{
    static const struct MDIO_Config mextiGpioCfg = MDIO_CFG_ALTERNATE_FUNCTION;
    static const struct MEXTI_Config mextiCfg = {.trigger = MEXTI_TRIGGER_FALLING};
    (void)ucIndex;
    (void)MDIO_configure(DBM_MEXTI_IN1->desc->mextiPin, &mextiGpioCfg);
    (void)MEXTI_init(&DBM_MextiHandle1, DBM_MEXTI_IN1, &mextiCfg);
    DBM_INT_DisableIrq();
}

/** Is interrupt pending? */
bool DBM_INT_IsIrqEvent(void)
{
    return MEXTI_isEvent(DBM_MEXTI_IN1);
}

/** Interrupt pending only on falling edge */
void DBM_INT_SetIrqModeFalling(void)
{
    // nothing to do here, edge trigger was already set in DBM_INT_Init
}

bool DBM_INT_bGetPin(void)
{
    return(MDIO_read(DBM_MEXTI_IN1->desc->mextiPin));
}

#else
    #warning "DBus IDLE detection unavailable due to missing MCAL MEXTI library"

// dummy functions, to be implemented later

/** Enable the used hardware interrupt */
void DBM_INT_EnableIrq(void)
{
    //to be implemented
}

/** Disable the used hardware interrupt */
void DBM_INT_DisableIrq(void)
{
    //to be implemented
}

/** Clear interrupt pending flag. */
void DBM_INT_ClearIrq(void)
{
    //to be implemented
}

/** Enable the used hardware interrupt */
void DBM_INT_Init(uint8_t ucIndex)
{
    //to be implemented
}

/** Is interrupt pending? */
bool DBM_INT_IsIrqEvent(void)
{
    //to be implemented
    return false;
}

/** Interrupt pending only on falling edge */
void DBM_INT_SetIrqModeFalling(void)
{
    //to be implemented
}

bool DBM_INT_bGetPin(void)
{
    //to be implemented
    return true;
}
#endif

#ifndef DBUS2_NO_BREAK_FUNCTIONALITY
void DBM_GPIO_SetBreakPinState(bool pinState)
{
    //DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetPort, ucPinState);
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
    //DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetModePort, HDIO_OUTPUT);
    const struct MDIO_Port *port = MDIO_getPort(&DBM_BREAK_MDIO_CHANNEL);
    enum MDIO_Pin pin            = MDIO_getPin(&DBM_BREAK_MDIO_CHANNEL);
    MDIO_configureMode(port, pin, MDIO_MODE_DIGITAL_OUTPUT, MDIO_ALT_FUNC_DEFAULT);

}

void DBM_GPIO_SetBreakPinModeAlternateFunction(void)
{
    //DBM_GPIO_FUNC_ARG(DBUS_BREAK_PIN, _vSetModePort, HDIO_ALTERNATE_FUNCTION);
    const struct MDIO_Port *port = MDIO_getPort(&DBM_BREAK_MDIO_CHANNEL);
    enum MDIO_Pin pin            = MDIO_getPin(&DBM_BREAK_MDIO_CHANNEL);
    MDIO_configureMode(port, pin, MDIO_MODE_ALTERNATE_FUNCTION, DBUS_MUART_PORT.txPin->scfg.altFunc);
}

#endif
