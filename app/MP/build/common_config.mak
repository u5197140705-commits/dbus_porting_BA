#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MP
#*******************************************************************************
#  Description      Common build settings for all variants
#*******************************************************************************

# build type (DEVELOP / RELEASE)
build_type    ?= DEVELOP


# list of application variants, first is default
app_variants   = MV


# compiler selection
cc_build_path ?= keilArm


# microcontroller configuration
platform      := stm32l4
derivative    := STM32L431VC
core_clock ?= 80
pclock ?= 80
pclock1 ?= 80
ext_osc ?= 8


# communication configuration
UDA                ?= UDA-01
UART_CHANNEL        = 0
NODE_ADDRESS        = 1
DEFAULT_BAUDRATE    = 96
WAKEUP_PIN          = B10
DBUS_TX_PIN         = A9
DBUS_RX_PIN         = A10
TESTMSGSRV_SUBNODE_ADDRESS = 10


# firmware update type (FWU1 / FWU3)
FW_update_type = FWU3

# flex partition table settings (ENABLED / DISABLED)
fwu_flex_partition_used = DISABLED

# functional safety settings (ENABLED / DISABLED)
feature_functional_safety ?= DISABLED

# ---------- security stuff -----------------
# authentic boot settings (ENABLED / DISABLED)
feature_authentic_boot ?= DISABLED
ifeq ($(feature_authentic_boot),ENABLED)
    # compilation contains approve flag if set (true / false)
    auto_approved_flag     ?= true
    # approve flag will be flashed by application if set (true / false)
    set_approved_flag      ?= true
endif
# --------- end security stuff ---------------

# global definitions
defines += 


# global build settings

rtos=TRUE

