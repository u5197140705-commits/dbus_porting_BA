#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  VARIANT          MV
#*******************************************************************************
#  Description      Application variant build file
#*******************************************************************************

# common components
common_components = $(msp) dbus ped_fw firmware_update 


# list of included PED_FW subcomponents
#ped_fw_subcomponent_list = basic timer schedule utility


# external components
ext_components = rtos 


# application specific components
app_components = 


# translation units not related to a component
src += 


# project libraries not related to a component
app_libs += 


# application specific definitions
defines += 


# additional search paths
search_path += 
