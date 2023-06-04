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
common_components = $(msp) dbus ped_fw firmware_update stack_monitor 


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


# uVision ini file generator:
# make target: uvision_gen_ini
preproc_file            += uvision_gen_ini
uvision_gen_ini_copt     = -undef -imacros $(varHeader)
uvision_gen_ini_template = ./uv/Templates/uvision_template.ini
uvision_gen_ini_output   = ./uv/$(project)/project_path.ini
uvision_gen_ini_defines  = ELF_LOCATION=\"..\\..\\$(subst /,\\,$(dwarf_file))\"

