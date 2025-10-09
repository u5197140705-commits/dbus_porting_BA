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
#  Description      build include for devices component
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef devices_build_mak
devices_build_mak := 1

devices_subcomponent_list ?= spi_flash dbuscan

# include the specified subcomponents
include $(call prepost, $(sdk_prog_path)/devices/,$(devices_subcomponent_list),/build.mak)


#end of protection guard preventing multiple inclusion
endif

