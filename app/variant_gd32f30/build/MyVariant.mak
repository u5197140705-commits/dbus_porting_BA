#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  VARIANT          MyVariant
#*******************************************************************************
#  Description      Application variant build file
#*******************************************************************************

# common components
ifeq ($(ssb_build_variant),rtos)
    common_components = $(msp) MSP/mcal dbus ped_fw firmware_update stack_monitor mem_utility debug_extended debug devices/dbuscan dbus/DBal
else
    common_components = $(msp) MSP/mcal dbus ped_fw firmware_update stack_monitor mem_utility debug_extended debug schedulers_bm/scheduler devices/dbuscan dbus/DBal
endif

# list of included PED_FW subcomponents
#ped_fw_subcomponent_list = basic timer schedule utility

mcal_modules = $(mcal_supported_modules_$(platform))
# If you need to reduce the size of the application, you can specify a subset of MCAL modules. Please note that some modules might not be available for your platform.
# DBusCAN requires at least mdio mexti mspi modules
# mcal_modules = mpcm mdio muart mexti msup mwdt mtim madc mi2c mspi mdma mdac

# external components
ifeq ($(ssb_build_variant),rtos)
    ext_components = rtos ssb services bsp
else
    ext_components = ssb services bsp
endif

dbus_mapping = mcal
dbus_uart_channel = 1
HEAPSIZE ?= 10
search_path += ext

# application specific components
app_components = /../../ATSSB


# translation units not related to a component
src += 


# project libraries not related to a component
app_libs += 


# application specific definitions
defines += 


# additional search paths
search_path += $(app_path)/../ATSSB


# Version settings (needed for ModuleHeader)
# if 'version_build' is not defined git hash will be used
uid              = 12345
version_major    = 1
version_minor    = 2
version_revision = 3
#version_build    = 4711
dbus_target_node = 0x$(dbus_node_address)0
release_note     = "Make short hint for this release"


#Vectorcast_Related_variables
dyntconfig_project_template_use   ?= FALSE
dyntconfig_project_startup_use    ?= FALSE

#debug component
dbgx_activated_filters += DBGX_FILTERS_SSB DBGX_FILTERS

