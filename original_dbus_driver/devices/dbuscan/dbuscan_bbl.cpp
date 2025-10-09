/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBCBBL
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    dbuscan_bbl.cpp
 *
 *  \ingroup dbuscan
 *
 *  \brief   Wrapper definitions and declarations for DBusCAN with BBL (Building Blocks) SPI.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "dbuscan_types.h"
#include "dbuscan_dbus.h"
#include "dbuscan_bbl.h"
#include "dbuscan_bbl.hpp"
#include "drv/digital/digital_mcal.hpp"
#include "drv/spi/spi_bus_mcal.hpp"


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static DBC_Error convertResponseToDbcError(const srv::Response state)
{
    return (srv::Response::ok != state) ? DBC_ERROR : DBC_OK;
}


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
drv::DigitalMCAL DBCBBL_csPinMcal(*DBCDRV_csPin);
drv::IDigital& dbc::csPin(DBCBBL_csPinMcal);

drv::spi::BusMcal DBCBBL_spiBusMcal(&DBCDRV_mspiChannel, DBCDRV_mspiCfg);
/* Add define DBUSCAN_BBL_SPI_BUS_DEFINED to omit this instance definition
if you have it defined somewhere else. There must be only one
BBL SPI bus instance defined for one SPI channel. */
#ifndef DBUSCAN_BBL_SPI_BUS_DEFINED
drv::spi::BusBase& dbc::spiBus(DBCBBL_spiBusMcal);
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
enum DBC_Error DBCBBL_init(void)
{
    return convertResponseToDbcError(dbc::SpiLink::getInstance().init());
}

enum DBC_Error DBCBBL_transfer(const uint8_t *const writeBuf, const uint16_t writeLen, uint8_t *const readBuf, const uint16_t readLen)
{
    return convertResponseToDbcError(dbc::SpiLink::getInstance().startTransfer(writeBuf, writeLen, readBuf, readLen));
}

enum DBC_Error DBCBBL_transferBlocking(const uint8_t *const writeBuf, const uint16_t writeLen, uint8_t *const readBuf, const uint16_t readLen)
{
    return convertResponseToDbcError(dbc::SpiLink::getInstance().transferBlocking(writeBuf, writeLen, readBuf, readLen));
}

enum DBC_Error DBCBBL_spiBusAcquire(void)
{
    return convertResponseToDbcError(dbc::SpiLink::getInstance().busAcquire());
}

enum DBC_Error DBCBBL_spiBusRelease(void)
{
    return convertResponseToDbcError(dbc::SpiLink::getInstance().busRelease());
}

void dbc::SpiLink::endTransferCB(void)
{
    if (nullptr != DBCDBUS_cbFunctionPtr)
    {
        DBCDBUS_cbFunctionPtr(nullptr, 0u, nullptr);
    }
}

void dbc::SpiLink::errorTransferCB(void)
{
    //tbd
}

void dbc::SpiLink::forcedReleaseCB(void)
{
    //tbd
}
