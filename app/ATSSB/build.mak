#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MT_Plus
#*******************************************************************************
#  Description      build include for ATSSB component
#*******************************************************************************

# ssb_use_cpp_instead_of_c_api = false
ssb_use_cpp_instead_of_c_api = true

ifeq ($(testUnit),ATSSB)
    #no unittest implemented
else
    ifeq ($(ssb_use_cpp_instead_of_c_api),true)
        src += $(app_path)/../ATSSB/atssb_handle_task_cpp.cpp
        defines += SSB_USE_CPP_INSTEAD_OF_C_API
    else
        src += $(app_path)/../ATSSB/atssb_handle_task_c.c
    endif
endif

code_gen_cfg += $(app_path)/../ATSSB/dbgx_filters.json
dbgx_activated_filters += DBGX_FILTERS_SSB_APP
