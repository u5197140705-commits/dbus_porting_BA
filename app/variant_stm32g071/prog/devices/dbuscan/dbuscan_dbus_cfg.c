/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBCDBUS
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    dbuscan_dbus_cfg.c
 *
 *  \ingroup dbuscan
 *
 *  \brief   Public configurable definitions related to DBus part of the DBusCAN chip.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_dbus.h"
#include "dbuscan_drv.h"
#include "dbuscan_types.h"
#include "dbuscan.h"
#ifdef DBAL_INCLUDED
#include "DBal_cfg.h"
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

union DBCDBUS_AddressList DBCDBUS_getAddressList(void)
{
    DBC_Cfg_t cfg = DBCDRV_getConfig();
    uint16_t defaultSubsystMask = (uint16_t)1u << cfg.SUBNODE_ID;
#ifdef DBAL_INCLUDED
    defaultSubsystMask |= 1u << DBAL_DBUS_HANDLER_SUBSYSTEM;
#endif
    union DBCDBUS_AddressList dbusAddrList =
    {
        .addr0 = {cfg.NODE_ID, defaultSubsystMask}, // default DBus address
        /* In order to use also another DBus address than default one,
        e.g. to use address 0x21 replace following line like this: {2u, DBC_DBUS_NF_SUBSYS1_MASK},*/
        .addr1  = {1u, DBC_DBUS_NF_SUBSYS1_MASK},
        .addr2  = {0, 0},
        .addr3  = {0, 0},
        .addr4  = {0, 0},
        .addr5  = {0, 0},
        .addr6  = {0, 0},
        .addr7  = {0, 0},
        .addr8  = {0, 0},
        .addr9  = {0, 0},
        .addr10 = {0, 0},
        .addr11 = {0, 0},
        .addr12 = {0, 0},
        .addr13 = {0, 0},
        .addr14 = {0, 0},
    };

    return dbusAddrList;
}


/** \brief   Interface for additional (application specific) handling of
 *           interrupt events from the DBusCAN chip.
 *
 *  \note    This function is called from ISR, therefore only essential code should be added here.
 *
 *  \param   irqBuf: interrupt flags read from the DBusCAN chip
 */
void DBCDBUS_handleIrqExt(union DBC_IrqBuf irqBuf)
{
    /* Example of handling WUP interrupt event from the DBusCAN chip */
    if (0u != (DBC_DBUS_IR_WUP_MASK & irqBuf.irq.dbusFlags)) // Wake Up Pulse (break signal) detected on DBus line
    {
    /*  Trigger sending WakeupSentRequest msg unless WakeupSentResponse msg is received
        if (!isWakeupSentResponseReceived)
        {
            (void)DBPL_bSendWakeupSentRequest();
        }
    */
    }
}


#ifdef DBUSCAN_SPI_CRC_USED
/** \brief   Notifies user that DBus message received by the DBusCAN chip has been lost.
 *
 *  \details This function informs user about situation when a DBus message is successfully received
 *           by the DBusCAN chip but due to communication error on SPI bus the message is
 *           not transferred to the MCU and thus not handled. As a result, the message is considered
 *           as lost.
*/
void DBCDBUS_notifyRxMsgLost(void)
{
    // Place for user code to handle the message loss.
}
#endif // DBUSCAN_SPI_CRC_USED
