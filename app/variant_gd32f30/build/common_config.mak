#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          variant_gd32f30
#*******************************************************************************
#  Description      Common build settings for all variants
#*******************************************************************************

#SSB config, uncomment if RTOS used. Else baremetal scheduler used
ssb_build_variant ?= bms
ssb_dbus_variant ?= mcal

# build type (DEVELOP / RELEASE)
# For more details refer to file common/build/help/make_variables.md
# or run 'make help' and click on the link 'List of variables' in main help page
build_type    ?= DEVELOP


# list of application variants, first is default
app_variants   = MyVariant


# compiler selection
cc_build_path ?= armclang


# microcontroller configuration
platform      := gd32f30
derivative    := GD32F303CBT6
core_clock ?= 120
pclock1 ?= 60
pclock2 ?= 120
ext_osc ?= 8


# communication configuration
dbus_mapping ?= mcal
ifeq ($(ssb_dbus_variant),dbuscan)
    dbus_mapping = dbuscan
endif    
UDA                  ?= UDA-01
dbus_node_address     = 1
dbus_default_baudrate = 96
TESTMSGSRV_SUBNODE_ADDRESS = 10

ifeq ($(dbus_mapping), ssb_dbus_variant) 
    dbus_uart_channel     = #DBusCAN chip is used instead, configure its communication channel settings in app/<project>/prog/devices/dbuscan/<platform>/dbuscan_drv_cfg.c
    dbus_tx_pin           = #DBUS pin of DBusCAN chip is used instead
    dbus_rx_pin           = #DBUS pin of DBusCAN chip is used instead
else
    dbus_uart_channel    ?= 0
    dbus_tx_pin           = A9
    dbus_rx_pin           = A10
endif


# firmware update type (FWU1 / FWU3)
FW_update_type = FWU1

# functional safety settings (ENABLED / DISABLED)
feature_functional_safety ?= DISABLED

# veeprom settings (ENABLED / DISABLED)
feature_veeprom ?= DISABLED

# global definitions
defines += 


# global build settings
ifeq ($(ssb_build_variant),rtos)
    rtos=TRUE
endif
scheduler_config=auto


ifeq ($(FW_update_type),FWU4)
    # start of GBL image in external flash
    FWU4_GBL_IMAGE_START_ADDRESS ?= 10008

    # FWU4 Version settings (needed for gecko_bl version)
    uid_gbl              = 12345
    version_major_gbl    = 1
    version_minor_gbl    = 2
    version_revision_gbl = 3
endif

# set if tooldrive information output will be suppressed (true) or not (false)
suppress_tooldrive_info ?= false
ifneq ($(suppress_tooldrive_info),true)
$(info Using external tooldrive $(tooldrive))
endif
