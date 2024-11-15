/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          Scheduler NEW (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          12.05.2018
 ******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Private User application specific definitions and declarations for scheduler_config


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "scheduler_auto.h"
#include "scheduler_types.h"

#include "dbus/bal.h"
#include "dbus/dbuspresentation.h"
#include "dbusdll.h"
#include "timer/timerlib.h"
#include "watchdogtimer/watchdogtimer.h"
#include "stack_monitor/stack_monitor.h"

/* USER CODE BEGIN Include */
/*! place your module includes here */
/* USER CODE END Include */


/***************************************************************************************************
 * DEPENDENCY ARRAYS
 * *************************************************************************************************
 * \brief: Create Module dependency arrays for each module here, terminate the array with END_STAMP
 * *************************************************************************************************
 * \note: No Circular Dependencies allowed
 ***************************************************************************************************/
const TaskID BAL_dep[]	= {DEPLIB_BAL	END_STAMP};
const TaskID DBPL_dep[]	= {DEPLIB_DBPL	END_STAMP};
const TaskID DLL_dep[]	= {DEPLIB_DLL	END_STAMP};
const TaskID TIM_dep[]	= {DEP_TIM	END_STAMP};
const TaskID WDT_dep[]	= {DEP_WDT	END_STAMP};
const TaskID STK_dep[]	= {DEP_STK	END_STAMP};

/* USER CODE BEGIN Dependency */
/*! place your module dependencies here */
/* USER CODE END Dependency */


/**!
 * \brief       Main Project Catalog Structure
 *
 * \details     It contains following Tables,
 *              TasksList           - Task line-up for scheduling
 *              TaskHandlersList    - TaskHandler functions
 *              DependenciesList    - Module Dependency
 */
struct SCH_ProjectInventory SCH_DefaultProjectCatalogue =
{
    /***************************************************************************************************
     * MAIN TASKS LIST
     * *************************************************************************************************
     * \brief: Add ModuleIDs from SCH_ModuleIDs and assign them appropriate priorities here
     ***************************************************************************************************/
    .TasksList =
    {
        {MOD_BAL, HIGH_PRIORITY},
        {MOD_DBPL, HIGH_PRIORITY},
        {MOD_DLL, LOW_PRIORITY},
        {MOD_TIM, HIGH_PRIORITY},
        {MOD_WDT, LOW_PRIORITY},
        {MOD_STK, LOW_PRIORITY},

        /* USER CODE BEGIN ModulePriority */
        /*! place your project modules and their priorities here */
        /* USER CODE END ModulePriority */

        {END_STAMP, PRIO_NONE}
    },


    /***************************************************************************************************
     * TASK HANDLERS LIST
     * *************************************************************************************************
     * \brief: Add corresponding task handlers for each module here
     ***************************************************************************************************/
    .TaskHandlersList =
    {
        [MOD_BAL] = BAL_HandleTask,
        [MOD_DBPL] = DBPL_HandleTask,
        [MOD_DLL] = DLL_HandleTask,
        [MOD_TIM] = TIM_bHandleTask16,
        [MOD_WDT] = WDT_handleTask,
        [MOD_STK] = STK_HandleTask,

        /* USER CODE BEGIN TaskHandlers */
        /*! place your task handlers here */
        /* USER CODE END TaskHandlers */
    },


    /***************************************************************************************************
     * DEPENDENCIES LIST
     * *************************************************************************************************
     * \brief: Add above module dependency arrays in the following Table
     * *************************************************************************************************
     * \note: maintain the same order as enumeration SCH_ModuleIDs in scheduler_config.h
     ***************************************************************************************************/
    .DependenciesList =
    {
        [MOD_BAL] = BAL_dep,
        [MOD_DBPL] = DBPL_dep,
        [MOD_DLL] = DLL_dep,
        [MOD_TIM] = TIM_dep,
        [MOD_WDT] = WDT_dep,
        [MOD_STK] = STK_dep,

        /* USER CODE BEGIN ModuleDependency */
        /*! place your module dependencies arrays here */
        /* USER CODE END ModuleDependency */
    },
};


/********************************************* End of File ******************************************************/
