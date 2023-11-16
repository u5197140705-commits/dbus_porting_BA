
#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          PP RTOS
#*******************************************************************************
#  Description      build setup RTOS
#*******************************************************************************

# USER CONFIG: RTOS
rtos_present = TRUE

#USER CONFIG: OS tracing supported by Micrium (streaming) and ThreadX for v58 and later (snapshot)
os_tracing_enable = FALSE
os_tracing_mode_streaming = FALSE

#USER CONFIG: Timer process in ISR
os_tx_timer_process_in_isr = FALSE

#USER CONFIG: RTOS lib gen
    #/*  
    # rtos_lib_develop = false/true (default = false)
    #  false - Exclude library source files from dependency scanning and static analysis. 
    #          If copy feature is enabled, change of source files would print a message to console.
    #  true  - Dependency scanning, static analysis and automatic rebuild on source files change is active
    #
    # rtos_lib_copy = false/true (default = false)
    #  false - Library is linked directly from output folder
    #  true  - Library is copied from out folder to lib folder and will be linked from there. 
    #          This preserves library when performing "make clean"
    #
    # rtos_lib_name (default = <library_name>_<platform>_<cc_build_path>)
    #  Library name of the copy of output library without extension, 
    #  relevant only if <library_name>_copy is set to true
    #  this name should reflect compatibility with platform/core/compiler/build type/
    #  to prevent including incompatible library during linking
    # */
rtos_lib_develop = false 
rtos_lib_copy    = false
rtos_lib_name    = rtos_lib$(addprefix _,$(project))

#** Please note that following features can only be used if RTOS is activated
ifeq ($(rtos_present), TRUE)

    # USER CONFIG: SYSTICK
    #/*  Select System SysTick
    # *  For SysTick every 1MS   = 1000
    # *  For SysTick every 10MS  = 100
    # *  For SysTick every 100MS = 10
    # *  For SysTick every 1S    = 1
    # *  
    # *  By default, SysTick every 1MS is selected!
    # */
    systicks_per_second = 1000

    # USER CONFIG: Micrium FLASH DRIVER
    micrium_flash_driver_present = FALSE

    # USER CONFIG: RTOS Debug Mode
    rtos_debug_mode_present = TRUE

    # Auto Define
    defines += SYSTICKS_PER_SECOND=$(systicks_per_second)

endif
