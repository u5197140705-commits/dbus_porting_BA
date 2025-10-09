#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      build include devices, subcomponent: spi_flash
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef spi_flash_build_mak
spi_flash_build_mak := 1

ifeq ($(marked_comp),devices/spi_flash)
    obj_test = abs_spi_flash
endif


ut_lib_src  += devices/spi_flash/abs_spi_flash.c
ut_mock_src += devices/spi_flash/test/spi_flash.mock.cpp

ifneq ($(unit_test_variant), true)
    cfg_src += devices/spi_flash/spi_flash_cfg.c
endif

#end of protection guard preventing multiple inclusion
endif
