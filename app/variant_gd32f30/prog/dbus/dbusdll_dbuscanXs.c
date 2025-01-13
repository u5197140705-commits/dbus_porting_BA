/*******************************************************************************
*   Copyright (c) 2022 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          DBUS
*   COMP_ABBREV      DLL
*******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/

/** \file
*     Application layer configuration of data link layer for D-Bus-2
*
*     This layer handles the message reception and transmission close to the bus-hardware,
*     handling the following:
*             - wrapping the message data into a frame as described below, which includes message legth,
*               TargetAddress, MessageIdentifier, the data of the message and the attached CRC (16 bit),
*               all followed by an acknowledge from the receiving node (or sending node by broadcasts)
*
*     Frame: \image html dbusdll.gif
*/
/*
*     ___________________________________________________________________________
*     |Msg.Len|   TA  |     MsgID     |    DATA ...             |      CRC       |
*     ___________________________________________________________________________
*NoOfBits  8       8         16           (Msg.Len-2)*8                16
*
*     |------------------CRC-Calculation------------------------|
*****************************************************************************************************************/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "bustypes.h"
#include "dbusdll.h"
#include "dbus_rtos_interface.h"
#include "dbuscan_drv.h"
#ifdef DBAL_INCLUDED
#include "bal.h"
#include "DBal/BshDBus2AppLayer_internal.h"
/*lint -e755 excessive global definitions not dangerous*/
#include "DBal_cfg.h"
/*lint -save -restore*/
#endif

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
#include "dbuspresentation_update.h"
#if defined(REMOTE_FIRMWARE_UPDATE)
#include "firmware_update/BootManager/BootManagerSharedData.h"
#endif
#endif


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
#ifdef APP_VARIANT
static void DLL_dbuscanIrqCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*lint -esym(522,DLL_vSetupSavedBaudRate) highest operation, function XY lacks side effects [MISRA 2012 Rule 2.2, advisory] */
/*lint -esym(523,DLL_vSetupSavedBaudRate) expression statement involving function XY lacks side effects */

bool DLL_init(void)
{
#ifdef APP_VARIANT
    if (DBC_OK != DBCDRV_init(DLL_dbuscanIrqCallback))
#else
    if (DBC_OK != DBCDRV_init(NULL))
#endif
    {
        return false;
    }
    DLL_vSetupSavedBaudRate();
    return true;
}


void DLL_vSetupSavedBaudRate(void)
{
#if (defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI))&&defined(REMOTE_FIRMWARE_UPDATE)
    DBPL_vConfigureBaudrate(BMDAT_getBaudRate());
#endif
}


/**
By default a physical node (communication partner) reacts only to one node address. It is, however, possible to define alias addresses, e.g. a node address Ah (decimal 10), which is a virtual address for the communication partner addressed by a system interface.
The default implementation (containing only one node address) can be replaced by a function, which accepts several node addresses.

\note Scenario: Electronic node answers to node address: 1 and Ah (decimal: 10).
When addressing node 1 an acknowledgment from node 1 is sent
When addressing node Ah an acknowledgment from node Ah is sent.

Furthermore, it is possible to implement a subsystem differentiation by adapting this function here. Example: Communication partner 1 will additionally react to alias address A, but only when subsystem 0, 1, 8 or 9 is used. This can be, for example, implemented by checking the following condition:\n
if ((ucAddress & 0xF6)== 0xA0).

\return
 \b type: bool\n
 \b range \li 0 = false
          \li all other values = true
*/
bool DLL_bIsCurrentNode(uint8_t ucAddress)
{
    return ((ucAddress>>NIBBLE_SIZE) == DLL_ucGetMainNodeAddress()) ? true : false;
}


/* Function prototype in dbusdll.c, this one is to be implemented, typically application specific, here in the excess template. */
/*When a transmission is acknowledged without time-out this function will be called from the receive interrupt routine. Therefore it is important to make sure that this function, which may be arbitrarily expanded by the user (application programmer), will not be too time consuming.*/
/*Make any necessary notification / save necessary variables, and leave this function as slim as possible.*/
void DLL_vMessageTransmitted_UserCallback(uint8_t ucNodeAddress, uint8_t ucAcknowledge, uint16_t uiUserCode)
{
#ifndef DBAL_INCLUDED
    (void)ucNodeAddress;
    (void)ucAcknowledge;
    (void)uiUserCode;
#else
    (void)ucNodeAddress;
    uint8_t SubsystemIndex = (uint8_t)(uiUserCode >> 8);
    if(SubsystemIndex == BAL_ucGetSubsystemIndex(DBAL_DBUS_HANDLER_SUBSYSTEM))
    {
        DBAL_setLastSendingStatus(ucNodeAddress, DLL_GET_ACKNOWLEDGE(ucAcknowledge), (uint8_t)uiUserCode);
    }
#endif //DBAL_INCLUDED
    return;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

#ifdef APP_VARIANT
/** Callback function to handle external interrupt events from the DBusCAN chip */
/*lint -esym(818,obj) Pointer parameter 'obj' could be declared as pointing to const [MISRA 2012 Rule 8.13, advisory] */
static void DLL_dbuscanIrqCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void)*(uint8_t*)obj; (void)flags; (void)*eventResponse;

    #ifndef RTOS_DBUS_EVENTDRIVEN
    DLL_setDbuscanIrqPending();
    #endif
    DBR_RunEventdrivenDbusTask(DBR_ED_TASK_DLL_ISR);
}
#endif //APP_VARIANT
