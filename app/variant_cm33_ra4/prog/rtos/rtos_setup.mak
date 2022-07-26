
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

#USER CONFIG: Select RTOS
os_micrium_present              = FALSE
os_threadx_present              = TRUE
#USER CONFIG: OS tracing supported only by ThreadX for TX v58 
os_tracing_enable               = TRUE

ifeq ($(os_micrium_present), TRUE)
    #USER CONFIG: Select Micrium Version

endif

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

    # USER CONFIG: RTOS Debug Mode
    rtos_debug_mode_present = TRUE

    # USER CONFIG: Real Time Dispatcher
    real_time_dispatcher_present = TRUE

    #** Please note that following features can only be used with Real Time Dispatcher
    ifeq ($(real_time_dispatcher_present), TRUE)

        # USER CONFIG: Tick Scheduler
        tick_scheduler_present = TRUE

        # USER CONFIG: Event Driven Scheduler
        event_driven_scheduler_present = TRUE

    endif

    # Auto Define
    defines += SYSTICKS_PER_SECOND=$(systicks_per_second)

endif
