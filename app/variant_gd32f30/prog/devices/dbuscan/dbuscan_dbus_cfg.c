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
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Configurable implementation of drivers controlling DBus part of the DBusCAN chip.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_dbus.h"
#include "dbuscan_drv.h"
#include "dbuscan.h"


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBCDBUS_handleIrqExt(union DBC_IrqBuf irqBuf)
{
    /* Example for handling some interrupt flag from DBusCAN chip */
    if (0u != (DBC_IF_MODE_SLEEP_MASK & irqBuf.irq.globalFlags))
    {
        /* send MCU to sleep as well
        isSleeping = true;
        MPCM_changePowerMode(MPCM_PMODE_SLEEP);
        */
    }
}
