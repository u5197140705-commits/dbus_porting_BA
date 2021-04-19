/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          IC DBUS
*   COMP_ABBREV      DLL
*******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/

/** \file
*     Data link layer for D-Bus-2
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
#include "huart.h"
#include "libtypes.h"
#include "bustypes.h"
#include "dbusdll.h"
#include "timerlib.h"

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
#include "dbuspresentation_update.h"
#if defined(REMOTE_FIRMWARE_UPDATE)
#include "firmware_update/BootManager/BootManagerSharedData.h"
#endif
#endif


#define DLL_CONFIG_INDEX               0x00    //!<ConfigIndex reflects e.g. the baud-rate, which is to be used.

struct DllConfigurationItem DLL_ConfigurationItem; //!< Configurable settings for the data link layer.


void DLL_vInitDllConfig(void)
{
    DLL_ConfigurationItem.ucNodeAddress         = DLL_NODE_ADDRESS;
    DLL_ConfigurationItem.ucConfigIndex         = DLL_CONFIG_INDEX;
    DLL_ConfigurationItem.ucInputBufferLength   = DLL_MESSAGE_INPUT_BUFFER_SIZE;
    DLL_ConfigurationItem.ulWaitingTimeOffsetUs = 0U;
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
 \b type: BOOL\n
 \b range \li 0 = FALSE
          \li all other values = TRUE
*/
BOOL DLL_bIsCurrentNode(uint8_t ucAddress)
{
    return (((uint8_t)(ucAddress>>NIBBLE_SIZE) == DLL_ucGetMainNodeAddress()) ? TRUE : FALSE);
}


uint8_t DLL_ucGetStandardUartConfigIndex(void)
{
    return DLL_ConfigurationItem.ucConfigIndex;
}

/* Function prototype in dbusdll.c, this one is to be implemented, typically application specific, here in the excess template. */
/*When a transmission is acknowledged without time-out this function will be called from the receive interrupt routine. Therefore it is important to make sure that this function, which may be arbitrarily expanded by the user (application programmer), will not be too time consuming.*/
/*Make any necessary notification / save necessary variables, and leave this function as slim as possible.*/
void DLL_vMessageTransmitted_UserCallback(uint8_t ucNodeAddress, uint8_t ucAcknowledge, uint16_t uiUserCode)
{
    (void)ucNodeAddress;
    (void)ucAcknowledge;
    (void)uiUserCode;
    return;
}
