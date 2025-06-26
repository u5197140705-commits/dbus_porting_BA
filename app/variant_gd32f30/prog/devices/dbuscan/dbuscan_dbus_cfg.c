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
#include "dbuscan_types.h"
#include "dbuscan.h"


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
/** \brief   DBus addresses to communicate with
 */
const struct DBCDBUS_Address DBCDBUS_addressList[DBC_DBUS_NF_COUNT] =
{
    {DBC_DBUS_NODE_ADDRESS, DBC_DBUS_NF_SUBSYS_ALL_MASK}, // default DBus address
    /* In order to use also another DBus address than default one,
    e.g. to use address 0x21 replace following line like this: {2u, DBC_DBUS_NF_SUBSYS1_MASK},*/
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u},
    {0u, 0u}
};

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

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
