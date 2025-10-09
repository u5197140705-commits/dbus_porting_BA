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


#include "dbuscan_bl.mock.h"

namespace dbuscan_bl_m
{
    dbuscan_bl_mock_class dbuscan_bl_mock;

    uint8_t* DBCBL_readData(uint16_t dataSize)
    {
        return dbuscan_bl_mock.DBCBL_readData(dataSize);
    }

    enum DBC_Error DBCBL_sendData(const uint8_t *dataBuf, uint16_t dataSize)
    {
        return dbuscan_bl_mock.DBCBL_sendData(dataBuf, dataSize);
    }

    bool DBCBL_isTxOngoing(void)
    {
        return dbuscan_bl_mock.DBCBL_isTxOngoing();
    }

    bool DBCBL_clearIrq(void)
    {
        return dbuscan_bl_mock.DBCBL_clearIrq();
    }

    void DBCBL_readTxStatus(void)
    {
        dbuscan_bl_mock.DBCBL_readTxStatus();
    }

    void DBCBL_clearRxFifo(void)
    {
        dbuscan_bl_mock.DBCBL_clearRxFifo();
    }

    uint16_t DBCBL_getRxDataSize(void)
    {
        return dbuscan_bl_mock.DBCBL_getRxDataSize();
    }
}

