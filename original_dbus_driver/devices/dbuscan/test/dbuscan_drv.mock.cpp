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


#include "dbuscan_drv.mock.h"

namespace dbuscan_drv_m
{
    dbuscan_drv_mock_class dbuscan_drv_mock;

    const struct MEXTI_Channel* DBCDRV_getMextiChannel(void)
    {
        return dbuscan_drv_mock.DBCDRV_getMextiChannel();
    }

    uint32_t DBCDRV_getMspiFrequency(void)
    {
        return dbuscan_drv_mock.DBCDRV_getMspiFrequency();
    }

    DBC_Cfg_t DBCDRV_getConfig(void)
    {
        return dbuscan_drv_mock.DBCDRV_getConfig();
    }

    enum DBC_Error DBCDRV_initComChannels(MCAL_CallbackFunction_t irqHandleCbFunc)
    {
        return dbuscan_drv_mock.DBCDRV_initComChannels(irqHandleCbFunc);
    }

    enum DBC_Error DBCDRV_init(MCAL_CallbackFunction_t irqHandleCbFunc)
    {
        return dbuscan_drv_mock.DBCDRV_init(irqHandleCbFunc);
    }

    void DBCDRV_disableIrq(void)
    {
        dbuscan_drv_mock.DBCDRV_disableIrq();
    }

    uint8_t DBCDRV_HandleTask(void)
    {
        return dbuscan_drv_mock.DBCDRV_HandleTask();
    }

    bool DBCDRV_isPowerOnReset(void)
    {
        return dbuscan_drv_mock.DBCDRV_isPowerOnReset();
    }

    void DBCDRV_notifyPowerOnReset(void)
    {
        dbuscan_drv_mock.DBCDRV_notifyPowerOnReset();
    }

    enum DBC_Error DBCDRV_reset(enum DBC_Reset rstType)
    {
        return dbuscan_drv_mock.DBCDRV_reset(rstType);
    }

    DBC_Error DBCDRV_spiReset(void)
    {
        return dbuscan_drv_mock.DBCDRV_spiReset();
    }

    bool DBCDRV_isResetFinished(void)
    {
        return dbuscan_drv_mock.DBCDRV_isResetFinished();
    }

    enum DBC_Error DBCDRV_setPowerMode(enum DBC_PowerMode mode)
    {
        return dbuscan_drv_mock.DBCDRV_setPowerMode(mode);
    }

    enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data)
    {
        return dbuscan_drv_mock.DBCDRV_writeReg32(addr, data);
    }

    enum DBC_Error DBCDRV_writeReg32Nbl(enum DBC_RegAddr addr, uint32_t data)
    {
        return dbuscan_drv_mock.DBCDRV_writeReg32Nbl(addr, data);
    }

    uint32_t DBCDRV_readReg32(enum DBC_RegAddr addr)
    {
        return dbuscan_drv_mock.DBCDRV_readReg32(addr);
    }

    enum DBC_Error DBCDRV_readReg32Nbl(enum DBC_RegAddr addr)
    {
        return dbuscan_drv_mock.DBCDRV_readReg32Nbl(addr);
    }

    uint32_t DBCDRV_getReadReg32Nbl(void)
    {
        return dbuscan_drv_mock.DBCDRV_getReadReg32Nbl();
    }

    enum DBC_Error DBCDRV_writeRegIpec(uint32_t bitVal, uint32_t bitPos, uint32_t bitMask)
    {
        return dbuscan_drv_mock.DBCDRV_writeRegIpec(bitVal, bitPos, bitMask);
    }

    bool DBCDRV_isIrqEvent(void)
    {
        return dbuscan_drv_mock.DBCDRV_isIrqEvent();
    }

    enum DBC_Error DBCDRV_readIrq(union DBC_IrqBuf *irqBuf)
    {
        return dbuscan_drv_mock.DBCDRV_readIrq(irqBuf);
    }

    enum DBC_Error DBCDRV_enableCfgDbus(void)
    {
        return dbuscan_drv_mock.DBCDRV_enableCfgDbus();
    }

    enum DBC_Error DBCDRV_disableCfgDbus(void)
    {
        return dbuscan_drv_mock.DBCDRV_disableCfgDbus();
    }

    uint16_t DBCDRV_convertDbusBaudValue(uint16_t baudrate)
    {
        return dbuscan_drv_mock.DBCDRV_convertDbusBaudValue(baudrate);
    }

    enum DBC_Error DBCDRV_setDbusBaudrate(uint16_t baudrate)
    {
        return dbuscan_drv_mock.DBCDRV_setDbusBaudrate(baudrate);
    }
}

