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

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>


#include "spi_flash.h"
using std::vector;

class SpiFlashCfg
{
public:
    MAKE_MOCK0(ABSSF_initInterface,  void());
    MAKE_MOCK2(ABSSF_readData,  void(uint32_t size, vector<uint8_t> & buffer));
    MAKE_MOCK2(ABSSF_writeData, void(uint32_t size, vector<uint8_t> const & buffer));
    MAKE_MOCK0(ABSSF_selectChip,   void());
    MAKE_MOCK0(ABSSF_unselectChip, void());
};

extern SpiFlashCfg flashCfgMock;
