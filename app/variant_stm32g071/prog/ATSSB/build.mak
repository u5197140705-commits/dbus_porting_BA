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

ssb_use_c_instead_of_cpp_api = false

ifeq ($(testUnit),ATSSB)
    #no unittest implemented
else
    ifeq ($(ssb_use_c_instead_of_cpp_api),true)
        src += ATSSB/atssb_handle_task_c.c
        defines += SSB_USE_C_INSTEAD_OF_CPP_API
    else
        src += ATSSB/atssb_handle_task_cpp.cpp
    endif
endif

code_gen_cfg += $(app_prog_path)/ATSSB/dbgx_filters.json
dbgx_activated_filters = DBGX_FILTERS_SSB_APP
