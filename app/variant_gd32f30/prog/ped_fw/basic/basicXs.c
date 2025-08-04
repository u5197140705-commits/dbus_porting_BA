/*******************************************************************************
 *   Copyright (c) 2016 BSH Hausgeraete GmbH,
 *   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *   All rights reserved. This program and the accompanying materials
 *   are protected by international copyright laws.
 *   Please contact copyright holder for licensing information.
 *
 ********************************************************************************
 *   PROJECT          PP PED-Framework
 *   MODULE-PREFIX    BASIC
 *   AUTHOR           emka
 *******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
    \brief Implementation of application specific functions declared in basic.h
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "basic.h"
#include "watchdogtimer/watchdogtimer.h"

#if defined (SYSTICK_TIMER_INCLUDED)
    #include "systick.h"
#endif

#if defined(RTOS)
    #include "os/rtos_kernel_enter.h"
    #if defined(RTOS_MODE_DEBUG)
        #include "timerlib.h"
    #endif
#elif defined (ZRTOS)
    #include "os_port/osp_fwToOs.h"
#elif defined(SCHEDULER_OLD)
    #include "schedule/schedDef.h"
#elif defined(SCHEDULER)
    #include "scheduler/scheduler.h"  // New Scheduler
#else
    #error "No Scheduler defined"
#endif // defined(RTOS)

#ifdef REMOTE_FIRMWARE_UPDATE
    #include "firmware_update/mal/moduleAdministration.h"
#endif


/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/
/**
 *  \brief      Init function called before entering the scheduler/rtos
 *
 *  \param      void
 *
 *  \return     void
 *
 *  \details    User application initializations can be done here before the system
 *              enters scheduler/rtos
 */
void BASIC_vUsrInitAppl (void)
{
    /* Do any pre initialization here: ...*/
#if defined(SYSTICK_TIMER_INCLUDED)
    (void)SysTick_Init();
#endif
#if defined(REMOTE_FIRMWARE_UPDATE)
    (void)MAL_writeFirmwareIDs();
#endif

    // Calls TIM_bHandleTask16 early to avoid HardFaults if circular timer is used before scheduler.
    TIM_initializeModule();

#if defined(RTOS)
    /* RTOS Initializations */
#elif defined (ZRTOS)
    /* ZRTOS Initializations */
#elif defined(SCHEDULER_OLD)
    SCH_vSetMainStateTable(MainTaskTbl); // register tasks with allotted priorities
#elif defined(SCHEDULER)
    SCH_setupScheduler(&SCH_DefaultProjectCatalogue); // User Project Catalog passed on to the scheduler
#else
    #error "No Scheduler defined"
#endif

    BASIC_vEnableAllInterrupts();

#ifdef UNIT_TEST
    TM_vInit();
#endif
}


/**
 *  \brief      Scheduler/rtos entry to schedule user tasks
 *
 *  \param      void
 *
 *  \return     void
 *
 *  \details    System enters the scheduler/rtos and control never returns!
 */
void BASIC_vUsrSchedTasks (void)
{
#if defined(RTOS)
    RTOS_kernel_entry();
#elif defined (ZRTOS)
    OSP_startOS();
#elif defined SCHEDULER_OLD
    (void)SCH_bHandleTask(); // handle project tasks
#else
    SCH_scheduler_entry();
#endif
    WDT_trigger();
}
