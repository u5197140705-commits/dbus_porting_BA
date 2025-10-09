/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW Test
 ******************************************************************************/

#include "spi_flash.mock.h"
#include <vector>
#include <algorithm>


SpiFlashCfg flashCfgMock;

extern "C" {
    void ABSSF_initInterface()
    {
        flashCfgMock.ABSSF_initInterface();
    }
    
    void ABSSF_readData(uint32_t size, uint8_t* buffer)
    {
        vector<uint8_t> vbuf(size);
        flashCfgMock.ABSSF_readData(size, vbuf);
        std::copy_n(vbuf.begin(),size, buffer);
    }

    void ABSSF_writeData(uint32_t size, const uint8_t* buffer)
    {
        vector<uint8_t> vbuf;
        vbuf.assign(buffer, buffer + size);
        flashCfgMock.ABSSF_writeData(size, vbuf);
    }

    void ABSSF_selectChip(void)
    {
        flashCfgMock.ABSSF_selectChip();
    }
    
    void ABSSF_unselectChip(void)
    {
        flashCfgMock.ABSSF_unselectChip();
    }

}

