#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          variant_cm4_stm32l4
#*******************************************************************************
#  Description      Common build settings for all variants
#*******************************************************************************

# build type (DEVELOP / RELEASE)
build_type    ?= DEVELOP


# list of application variants, first is default
app_variants   = MyVariant


# compiler selection
cc_build_path ?= keilArm


# microcontroller configuration
platform      := stm32l4
derivative    := STM32L471VE
core_clock ?= 80
pclock ?= 80
pclock1 ?= 80
ext_osc ?= 8


# communication configuration
UDA                  ?= UDA-01
dbus_uart_channel     = 0
dbus_node_address     = 1
dbus_default_baudrate = 96
dbus_tx_pin           = A9
dbus_rx_pin           = A10
TESTMSGSRV_SUBNODE_ADDRESS = 10


# firmware update type (FWU1 / FWU3)
FW_update_type = FWU1

# flex partition table settings (ENABLED / DISABLED)
fwu_flex_partition_used = DISABLED

# functional safety settings (ENABLED / DISABLED)
feature_functional_safety ?= DISABLED

# ---------- security stuff -----------------
# compilation contains approve flag if set (true / false)
auto_approved_flag     ?= true
# approve flag will be flashed by application if set (true / false)
set_approved_flag      ?= true
# --------- end security stuff ---------------

# global definitions
defines += 


# global build settings

rtos=TRUE

