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

#ifndef DBUSCAN_BBL_HPP
#define DBUSCAN_BBL_HPP


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    dbuscan_bbl.hpp
 *
 *  \ingroup dbuscan
 *
 *  \brief   Public definitions and declarations for DBusCAN with BBL (Building Blocks) SPI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "dbuscan_drv.h"
#include "drv/digital/digital_interface.hpp"
#include "drv/spi/spi_link.hpp"


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/

namespace dbc
{

extern drv::IDigital& csPin;

/* Add define DBUSCAN_BBL_SPI_BUS_DEFINED to omit definition of this instance
(defined in dbuscan_bbl.cpp) if you have it defined somewhere else. There must be
only one BBL SPI bus instance defined for one SPI channel. */
extern drv::spi::BusBase& spiBus;

/* Singleton instance of SPI link class used for the DBusCAN chip */
class SpiLink : protected drv::ITransferCB
{
public:
    static SpiLink& getInstance(void)
    {
        static SpiLink instance(spiBus, csPin);
        return instance;
    }

    srv::Response init(void)
    {
        return spiBus.enable();
    }

    srv::Response startTransfer(const uint8_t *const writeBuf, const uint16_t writeLen, uint8_t *const readBuf, const uint16_t readLen)
    {
        return link.startTransfer(writeBuf, writeLen, readBuf, readLen);
    }

    srv::Response transferBlocking(const uint8_t *const writeBuf, const uint16_t writeLen, uint8_t *const readBuf, const uint16_t readLen)
    {
        return link.transferBlocking(writeBuf, writeLen, readBuf, readLen);
    }

    srv::Response busAcquire(void)
    {
        return link.busAcquire();
    }

    srv::Response busRelease(void)
    {
        return link.busRelease();
    }

    void endTransferCB(void)   override;
    void errorTransferCB(void) override;
    void forcedReleaseCB(void) override;

    SpiLink(const SpiLink&)             = delete; /* Copy constructor */
    SpiLink(const SpiLink&&)            = delete; /* Move constructor */
    SpiLink& operator=(const SpiLink&)  = delete; /* Assignment operator */
    SpiLink& operator=(const SpiLink&&) = delete; /* Move operator */

    virtual ~SpiLink(void) = default;

private:
    drv::spi::Link link;

    SpiLink(drv::spi::BusBase& bus, drv::IDigital& cs)
    : link(bus, cs)
    {
        link.registerOwner(this);
    }
};

} // namespace dbc

#endif // DBUSCAN_BBL_HPP
