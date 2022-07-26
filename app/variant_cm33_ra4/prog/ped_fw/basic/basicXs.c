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
#include "hwdt.h"

#if defined(RTOS)
    #include "os/rtos_kernel_enter.h"
    #if defined(RTOS_MODE_DEBUG)
        #include "timerlib.h"
    #endif
#elif defined(SCHEDULER_OLD)
    #include "schedule/schedDef.h"
#elif defined(SCHEDULER)
    #include "scheduler/scheduler.h"  // New Scheduler
#else
    #error "No Scheduler defined"
#endif

#ifdef REMOTE_FIRMWARE_UPDATE
    #include "firmware_update/mal/moduleAdministration.h"
#endif

/***************************************************************************************************
 * SCHEDULER - PROJECT INVENTORY
 * *************************************************************************************************
 * \brief: If other than the Default Project Inventory, which is configured in scheduler_config.c, is
 *         needed then please add it here (can also be included via a header file) and it can be passed
 *         onto the scheduler with a function call 'SCH_setupScheduler(&<project_catalog>)' below in
 *         BASIC_vUsrInitAppl().
 ***************************************************************************************************/
#if defined(SCHEDULER)
extern struct SCH_ProjectInventory SCH_DefaultProjectCatalogue; // Default Project Catalog
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
#if defined(REMOTE_FIRMWARE_UPDATE)
    (void)MAL_writeFirmwareIDs();
#endif

#if defined(RTOS)
    /* RTOS Initializations */
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
    #if defined(RTOS_MODE_DEBUG)
        (void)TIM_handleTask16(); /* Early initialization of timerlib required for tx_execution_profile. */
    #endif
    RTOS_kernel_entry();
#elif defined SCHEDULER_OLD
    (void)SCH_bHandleTask(); // handle project tasks
#else
    SCH_scheduler_entry();
#endif
    HWDT_vTrigger();
}
