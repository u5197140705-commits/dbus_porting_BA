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

ifeq ($(testUnit),ATSSB)
    obj     += ATSSB/Test/ATSSB_testframe
else
    obj     += ATSSB/atssb_handle_task
endif

