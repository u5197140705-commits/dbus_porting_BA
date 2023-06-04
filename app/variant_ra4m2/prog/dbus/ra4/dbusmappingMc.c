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
/** \file
 *
 *   \brief     Renesas RA4 specific mapping library for D-Bus-2 data link 
 *              layer using one specific UART channel.
 *
 *   \details   The mapping layer provides the implementation of the following 
 *              mapping functions:
 *              TBD
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

struct MDIO_Config mextiGpioCfg = MDIO_CFG_ALTERNATE_FUNCTION_PULL_UP;
const struct MEXTI_Channel *MEXTI_IN1 = &DBM_IDLE_MEXTI_CHANNEL;

struct MEXTI_Config mextiCfg = {.trigger = MEXTI_TRIGGER_FALLING};

struct MEXTI_Handle MextiHandle1;
    
MCAL_Callback_t MextiCallback1;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/** Enable the used hardware interrupt */
void DBM_INT_EnableIrq(void)
{
    MEXTI_enableEvent(&MextiHandle1, &MextiCallback1);
}

/** Disable the used hardware interrupt */
void DBM_INT_DisableIrq(void)
{
    if(MextiHandle1.enabledEvent == true)   // if MEXTI already initialized
    {
        MEXTI_disableEvent(&MextiHandle1, &MextiCallback1);
    }
}

/** Clear interrupt pending flag. */
void DBM_INT_ClearIrq(void)
{
    MEXTI_clearEventReg(MextiHandle1.channel);
}

/** Enable the used hardware interrupt */
void DBM_INT_Init(uint8_t ucIndex)
{
    (void)ucIndex;
    (void)MDIO_configure(MEXTI_IN1->desc->mextiPin, &mextiGpioCfg);
    (void)MEXTI_init(&MextiHandle1, MEXTI_IN1, &mextiCfg);
    DBM_INT_DisableIrq();
}

/** Is interrupt pending? */
bool DBM_INT_IsIrqEvent(void)
{
    return MEXTI_isEvent(MEXTI_IN1);
}

/** Interrupt pending only on falling edge */
void DBM_INT_SetIrqModeFalling(void)
{
    // nothing to do here, edge trigger was already set in DBM_INT_Init
}

bool DBM_INT_bGetPin(void)
{
    return(MDIO_read(MEXTI_IN1->desc->mextiPin));
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
