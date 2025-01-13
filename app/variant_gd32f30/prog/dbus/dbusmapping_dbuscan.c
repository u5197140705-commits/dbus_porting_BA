/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          IC Dbus2
 *  COMP_ABBREV      DBM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     Mapping library for D-Bus-2 data link layer using one specific UART channel
 *     (remains only due to compatibility with upper DBus layers).
 *
 *     \li DBM_UART_vDisableRxEvent(void)
 *     \li DBM_UART_vEnableRxEvent(void)
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "dbusmapping.h"
#include "dbuscan_drv.h"


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBM_UART_vDisableRxEvent(void)
{
    HSUP_vNop(); // dummy function definition due to compatibility with upper DBus layers
}

void DBM_UART_vEnableRxEvent(void)
{
    HSUP_vNop(); // dummy function definition due to compatibility with upper DBus layers
}

void DBM_PERIPH_vSetBaudRate(uint8_t ucIndex, uint16_t uiBaudRate)
{
    (void)ucIndex;
    (void)DBCDRV_setDbusBaudrate(uiBaudRate);
}
