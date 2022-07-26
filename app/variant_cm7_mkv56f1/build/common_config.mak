#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          variant_cm7_mkv56f1
#*******************************************************************************
#  Description      Common build settings for all variants
#*******************************************************************************

# build type (DEVELOP / RELEASE)
# For more details refer to file common/build/help/make_variables.md
# or run 'make help' and click on the link 'List of variables' in main help page
build_type    ?= DEVELOP


# list of application variants, first is default
app_variants   = MyVariant


# compiler selection
cc_build_path ?= armclang


# microcontroller configuration
platform      := mkv5x
derivative    := MKV56F1M0VLL24
core_clock ?= 240
pclock ?= 120
ext_osc ?= 24


# communication configuration
UDA                  ?= UDA-01
dbus_uart_channel     = 0
dbus_node_address     = 1
dbus_default_baudrate = 96
dbus_tx_pin           = A14
dbus_rx_pin           = A15
TESTMSGSRV_SUBNODE_ADDRESS = 10


# firmware update type (FWU1 / FWU3)
FW_update_type = FWU1

# flex partition table settings (ENABLED / DISABLED)
fwu_flex_partition_used = DISABLED

# functional safety settings (ENABLED / DISABLED)
feature_functional_safety ?= DISABLED


# global definitions
defines += 


# global build settings

rtos=TRUE

