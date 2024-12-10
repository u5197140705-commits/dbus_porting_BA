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
 *  COMP_ABBREV      DBCDRV
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     dbuscan_drv_cfg.c
 *
 *  \ingroup  dbuscan
 *
 *  \brief    Public configurable definitions for unit dbuscan
 *
 *  \details  Contains basic configurable settings of the communication channels
 *            and initialization values used for the DBusCAN chip.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_drv.h"
#include "dbuscan_types.h"
#include "mcal/mcal_includes.h"
#include "mcal_channels.h"


/******************************************************************************/
/* PUBLIC DATA AND FUNCTION DEFINITIONS                                       */
/******************************************************************************/

// SPI peripheral unit and pins used for communication with the DBusCAN chip
const struct MSPI_Channel DBCDRV_mspiChannel =
{
    .scfg =
    {
        .clkSrc    = MSPI_DEFAULT_CLOCK_SOURCE,
        .remapMask = NO_REMAP
    },
    .mspi = &MSPI1,
    .sclk = &MDIOB13_MSPI1_SCK,
    .miso = &MDIOB14_MSPI1_MISO,
    .mosi = &MDIOB15_MSPI1_MOSI,
    .cs   = &MDIOB7
};

// GPIO pin used for the external interrupt from interrupt pin (nINT) of the DBusCAN chip
const struct MEXTI_Channel* DBCDRV_getMextiChannel(void)
{
    return &MEXTIA3;
}

// Clock frequency used for SPI communication with the DBusCAN chip
uint32_t DBCDRV_getMspiFrequency(void)
{
    return MSPI_FREQUENCY_1M;
}

// Values used for DBusCAN chip configuration and stored in its EEPROM
DBC_Cfg_t DBCDRV_getConfig(void)
{
    DBC_Cfg_t cfg = {{0}};

    cfg.CLKIN        = (uint8_t)DBC_CLKIN_20M;
    cfg.GP_MEM       = 0u;

    cfg.DBUS_EN      = 1u;
    cfg.ADV_PWR_MGMT = 0u;
    cfg.DBR          = DBCDRV_convertDbusBaudValue(DBUS_DEFAULT_BAUDRATE);
    cfg.NODE_ID      = DBC_DBUS_NODE_ADDRESS;
    cfg.SUBNODE_ID   = 0u;
    cfg.BVD_WAIT_EN  = 0u;
    cfg.BVD_THLD     = 0u;
    cfg.BVD_TO_NWKRQ = 0u;
    cfg.DBUS2CAN     = 0u;

    cfg.MCAN_EN      = 0u;
    cfg.CAN_BIAS     = 0u;
    cfg.FD_DR        = 0u;
    cfg.CAN_DR       = 0u;

    DBCDRV_eepromWriteEnable = true; // Enable write to the chip EEPROM via DBCDRV_HandleTask()

    return cfg;
}
