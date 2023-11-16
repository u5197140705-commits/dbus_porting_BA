#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          variant_efr32bg21
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
cc_build_path ?= gccArm


# microcontroller configuration
platform      := efr32xg21
derivative    := EFR32BG21A010F1024IM32
core_clock_khz ?= 38400
pclock ?= 38400
pclock1 ?= 32000


# communication configuration
UDA                  ?= UDA-01
dbus_uart_channel     = 0
dbus_node_address     = 1
dbus_default_baudrate = 96
dbus_tx_pin           = A5
dbus_rx_pin           = A6
TESTMSGSRV_SUBNODE_ADDRESS = 10


# firmware update type (FWU1 / FWU3)
FW_update_type = FWU1

# flex partition table settings (ENABLED / DISABLED)
fwu_flex_partition_used = DISABLED

# functional safety settings (ENABLED / DISABLED)
feature_functional_safety ?= DISABLED

# veeprom settings (ENABLED / DISABLED)
feature_veeprom ?= DISABLED

# global definitions
defines += 


# global build settings
rtos=TRUE

ifeq ($(FW_update_type),FWU4)
    # start of GBL image in external flash
    FWU4_GBL_IMAGE_START_ADDRESS ?= 10008

    # FWU4 Version settings (needed for gecko_bl version)
    uid_gbl              = 12345
    version_major_gbl    = 1
    version_minor_gbl    = 2
    version_revision_gbl = 3
endif
