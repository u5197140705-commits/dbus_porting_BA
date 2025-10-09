/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  This file is generated automatically
 *  It might be adapted manually
 ******************************************************************************/


#ifndef DBUSCAN_DRV_MOCK_H
#define DBUSCAN_DRV_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>
#include "../dbuscan_types.h"
namespace dbuscan_drv_m
{
    #include "../dbuscan_drv.h"

    class dbuscan_drv_mock_class
    {
    public:
        MAKE_MOCK0(DBCDRV_getMextiChannel        , const MEXTI_Channel* ( void ));
        MAKE_MOCK0(DBCDRV_getMspiFrequency       , uint32_t          ( void ));
        MAKE_MOCK0(DBCDRV_getConfig              , DBC_Cfg_t         ( void ));
        MAKE_MOCK1(DBCDRV_initComChannels        , DBC_Error         ( MCAL_CallbackFunction_t irqHandleCbFunc ));
        MAKE_MOCK1(DBCDRV_init                   , DBC_Error         ( MCAL_CallbackFunction_t irqHandleCbFunc ));
        MAKE_MOCK0(DBCDRV_disableIrq             , void              ( void ));
        MAKE_MOCK0(DBCDRV_HandleTask             , uint8_t           ( void ));
        MAKE_MOCK0(DBCDRV_isPowerOnReset         , bool              ( void ));
        MAKE_MOCK0(DBCDRV_notifyPowerOnReset     , void              ( void ));
        MAKE_MOCK1(DBCDRV_reset                  , DBC_Error         ( DBC_Reset rstType ));
        MAKE_MOCK0(DBCDRV_spiReset               , DBC_Error         ( void ));
        MAKE_MOCK0(DBCDRV_isResetFinished        , bool              ( void ));
        MAKE_MOCK1(DBCDRV_setPowerMode           , DBC_Error         ( DBC_PowerMode mode ));
        MAKE_MOCK2(DBCDRV_writeReg32             , DBC_Error         ( DBC_RegAddr addr, uint32_t data ));
        MAKE_MOCK2(DBCDRV_writeReg32Nbl          , DBC_Error         ( DBC_RegAddr addr, uint32_t data ));
        MAKE_MOCK1(DBCDRV_readReg32              , uint32_t          ( DBC_RegAddr addr ));
        MAKE_MOCK1(DBCDRV_readReg32Nbl           , DBC_Error         ( DBC_RegAddr addr ));
        MAKE_MOCK0(DBCDRV_getReadReg32Nbl        , uint32_t          ( void ));
        MAKE_MOCK3(DBCDRV_writeRegIpec           , DBC_Error         ( uint32_t bitVal, uint32_t bitPos, uint32_t bitMask ));
        MAKE_MOCK0(DBCDRV_isIrqEvent             , bool              ( void ));
        MAKE_MOCK1(DBCDRV_readIrq                , DBC_Error         ( union DBC_IrqBuf *irqBuf ));
        MAKE_MOCK0(DBCDRV_enableCfgDbus          , DBC_Error         ( void ));
        MAKE_MOCK0(DBCDRV_disableCfgDbus         , DBC_Error         ( void ));
        MAKE_MOCK1(DBCDRV_convertDbusBaudValue   , uint16_t          ( uint16_t baudrate ));
        MAKE_MOCK1(DBCDRV_setDbusBaudrate        , DBC_Error         ( uint16_t baudrate ));
    };

    extern dbuscan_drv_mock_class dbuscan_drv_mock;
}

using namespace dbuscan_drv_m;

#endif  // DBUSCAN_DRV_MOCK_H
