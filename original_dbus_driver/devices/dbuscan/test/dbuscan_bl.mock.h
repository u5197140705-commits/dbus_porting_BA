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


#ifndef DBUSCAN_BL_MOCK_H
#define DBUSCAN_BL_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace dbuscan_bl_m
{
    #include "../dbuscan_bl.h"

    class dbuscan_bl_mock_class
    {
    public:
        MAKE_MOCK1(DBCBL_readData                , uint8_t*          ( uint16_t dataSize ));
        MAKE_MOCK2(DBCBL_sendData                , DBC_Error         ( const uint8_t *dataBuf, uint16_t dataSize ));
        MAKE_MOCK0(DBCBL_isTxOngoing             , bool              ( void ));
        MAKE_MOCK0(DBCBL_clearIrq                , bool              ( void ));
        MAKE_MOCK0(DBCBL_readTxStatus            , void              ( void ));
        MAKE_MOCK0(DBCBL_clearRxFifo             , void              ( void ));
        MAKE_MOCK0(DBCBL_getRxDataSize           , uint16_t          ( void ));
    };

    extern dbuscan_bl_mock_class dbuscan_bl_mock;
}

using namespace dbuscan_bl_m;

#endif  // DBUSCAN_BL_MOCK_H
