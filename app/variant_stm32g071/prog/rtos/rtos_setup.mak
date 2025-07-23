
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

#USER CONFIG: OS tracing supported by Micrium (streaming) and ThreadX (snapshot & streaming)
os_tracing_enable = FALSE
os_tracing_mode_streaming = FALSE
# Setting this variable to TRUE disables tracing RTOS events and only user events will 
# appear in the trace. (Only applicable to ThreadX-Streaming combination at the moment)
os_tracing_exclude_os_events = FALSE

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
rtos_lib_develop ?= false 
rtos_lib_copy    ?= false
rtos_lib_name    ?= rtos_lib$(addprefix _,$(project))

#USER CONFIG: Eventdriven DBUS solution for RTOS.
    #/*
    # It's only possible to use this option, if Dbus CAN chip is present.
    # Otherwise, this switch has no effect.
    # For more informations, please read ext/rtos/doc/DbusBuildOptions.md
    #*/
os_dbus_eventdriven_active = TRUE

#USER CONFIG: Thread Stack usage warnings 
    #/*
    # There are two warning thresholds for Stack usage, Red- and Yellowzone.
    # The check is done asynchronous after every run of a Thread.
    # The thresholds are percentvalue from 1 ... 100%.
    # They should be set to a appropriate value in terms of getting notified not 
    # to early and not to late. 
    # The Yellowzone is an area which indicated the Stack has already a high usage, but not critical.
    # Writing data to the Redzone means, the Stackusage is to high and should be checked. Maybe it
    # should be considered to increase the Stacksize. 
    #
    # Warnings can be read from variable RTD_RuntimeCatalogue.RuntimeInfo.Warning_Stack<zone>FirstId
    #
    # Set 100 (=100%) to one or both values means, the check is deactivated.
    #*/
os_stackusage_warning_threshold_yellowzone  = 60
os_stackusage_warning_threshold_redzone     = 80

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
   
    # USER CONFIG: Single Thread Application
    # Setting this variable to ENABLED will activate a single thread application
    rtos_single_thread_application = DISABLED
    
    # USER CONFIG: Single Thread Memory Minimization
    # Setting this variable to ENABLED will activate single thread memory minimization
    # This feature is only available if rtos_single_thread_application is ENABLED
    rtos_single_thread_memory_minimization = DISABLED

endif

