#*******************************************************************************
#  Copyright (c) 2022 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          Generic SW
#*******************************************************************************
#  Description      build include for devices, subcomponent: dbuscan
#*******************************************************************************

# protection guard preventing multiple inclusion
ifndef dbuscan_build_mak
    dbuscan_build_mak := 1

search_path += common/prog/devices/dbuscan

code_gen_cfg += devices/dbuscan/config.json

dbuscan_with_bbl_spi ?= false
dbuscan_spi_crc_used ?= true

# list of platforms supporting DMA for SPI communication with the DBusCAN chip
ifneq (,$(filter $(platform),stm32g0 stm32g4))
    dbuscan_dma_used ?= true
endif

ifeq ($(std_variant),true)
    obj += devices/dbuscan/dbuscan_drv
    ifdef bootLoader
        obj += devices/dbuscan/dbuscan_bl
    else
        obj += devices/dbuscan/dbuscan_dbus
    endif

    cfg_obj += devices/dbuscan/$(platform)/dbuscan_drv_cfg
    ifndef bootLoader
        cfg_obj += devices/dbuscan/dbuscan_dbus_cfg
    endif

    ifeq ($(dbuscan_with_bbl_spi),true)
        src += devices/dbuscan/dbuscan_bbl.cpp
        defines_cl += DBUSCAN_WITH_BBL_SPI
    endif
endif
ifeq ($(unit_test_variant), true)
#TODO remove when dbus is part of the unit test application
    defines += DLL_TRANSMIT_BUFFER_DATA_LENGTH=200
    ut_mock_src += devices/dbuscan/test/dbuscan_bl.mock.cpp
    ut_mock_src += devices/dbuscan/test/dbuscan_drv.mock.cpp
else
    dep_component += MSP/mcal math
endif

# end of protection guard preventing multiple inclusion
endif
