/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
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
/** \file
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
#include "mcal_channels.h"


/******************************************************************************/
/* PUBLIC DATA AND FUNCTION DEFINITIONS                                       */
/******************************************************************************/

// Configuration of SPI communication with the DBusCAN chip
const struct MSPI_Config DBCDRV_mspiCfg =
{
    .dataBits         = MSPI_DATA_BITS_8,            //do not modify
    .clockPolarity    = MSPI_CLOCK_POLARITY_0,       //do not modify
    .clockPhase       = MSPI_CLOCK_PHASE_0,          //do not modify
    .dataInvert       = MSPI_DATA_INVERT_DISABLED,   //do not modify
    .frameFormat      = MSPI_FRAME_FORMAT_MSB_FIRST, //do not modify
    .chipSelMode      = MSPI_CHIP_SELECT_AUTO_LOW,   //do not modify
    .clockFreq        = MSPI_FREQUENCY_1M,
    .misoPullResistor = MDIO_PULL_UP                 //do not modify
};

// SPI peripheral unit and pins used for communication with the DBusCAN chip
const struct MSPI_Channel DBCDRV_mspiChannel =
{
    .mspi = &MSPI0,
    .sclk = &MDIOE17_MSPI0_SCK,
    .miso = &MDIOE19_MSPI0_SIN,
    .mosi = &MDIOE18_MSPI0_SOUT,
#ifndef DBUSCAN_WITH_BBL_SPI
    .cs   = &MDIOE16
#else
    .cs   = NULL // must be NULL here- separate channel @DBCDRV_csPin is used with BBL for the chip select pin
#endif
};

#ifdef DBUSCAN_WITH_BBL_SPI
// GPIO pin used as chip select for the DBusCAN chip
const struct MDIO_Channel* DBCDRV_csPin = &MDIOE16;
#endif

#ifdef DBUSCAN_DMA_USED
const struct MDMA_Periph DBCDRV_dmaPeriph = {&MDMA0_Descriptor};

struct MDMA_Channel DBCDRV_dmaTxChannel =
{
    .desc = &MDMA0_CH31_Descriptor,
    .scfg =
    {
        .eventSource = MDMA_EVENT_SOURCE_SPI0_TX,
        .priority    = MDMA_CHANNEL_PRIORITY_0,
        .preemption  = MDMA_CHANNEL_PREEMPTION_ENABLED,
        .preemptionAbility = MDMA_CHANNEL_PREEMPTION_ABILITY_DISABLED
    }
};

struct MDMA_Channel DBCDRV_dmaRxChannel =
{
    .desc = &MDMA0_CH30_Descriptor,
    .scfg =
    {
        .eventSource = MDMA_EVENT_SOURCE_SPI0_RX,
        .priority    = MDMA_CHANNEL_PRIORITY_0,
        .preemption  = MDMA_CHANNEL_PREEMPTION_ENABLED,
        .preemptionAbility = MDMA_CHANNEL_PREEMPTION_ABILITY_DISABLED
    }
};
#endif // DBUSCAN_DMA_USED

// GPIO pin used for the external interrupt from interrupt pin (nINT) of the DBusCAN chip
const struct MEXTI_Channel* DBCDRV_getMextiChannel(void)
{
    return &MEXTIE20;
}

// Enable for programming chip EEPROM via DBCDRV_HandleTask()
bool DBCDRV_eepromWriteEnable = true;

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

    return cfg;
}
