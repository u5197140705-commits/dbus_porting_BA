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
#include "scheduler_cfg.h"
#include "scheduler_types.h"
#include "bal.h"
#include "dbuspresentation.h"
#include "dbusdll.h"
#include "timerlib.h"
#include "ped_fw/watchdogtimer/watchdogtimer.h"
#ifdef DBUSCAN_INCLUDED
#include "devices/dbuscan/dbuscan_drv.h"
#endif
#ifdef STACK_MONITOR_ENABLED
#include "stack_monitor/stack_monitor.h"
#endif
#ifdef CCSS
#include "CCSS_SyStTask.h"
#endif
#ifdef CCSS_DOM_REF
#include "CCDOM_DomainRefImpl.h"
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
#include "FuncSafe/fSafe.h"      // for handle task's
#include "FuncSafe/fsafeXs.h"    // needed for DEP_xxx
#endif

/***************************************************************************************************
 * DEPENDENCY ARRAYS
 * *************************************************************************************************
 * \brief: Create Module dependency arrays for each module here, terminate the array with END_STAMP
 * *************************************************************************************************
 * \note: No Circular Dependencies allowed
 ***************************************************************************************************/
static const TaskID BAL_dep[]                 = {DEPLIB_BAL  END_STAMP};
static const TaskID DBPL_dep[]                = {DEPLIB_DBPL END_STAMP};
static const TaskID DLL_dep[]                 = {DEPLIB_DLL  END_STAMP};
static const TaskID TIM_dep[]                 = {DEP_TIM     END_STAMP};
static const TaskID WDT_dep[]                 = {DEP_WDT     END_STAMP};
#ifdef DBUSCAN_INCLUDED
    static const TaskID DBC_dep[]             = {DEP_DBC     END_STAMP};
#endif
#ifdef STACK_MONITOR_ENABLED
    static const TaskID STK_dep[]             = {DEP_STK     END_STAMP};
#endif
#ifdef CCSS
    static const TaskID CCSS_dep[]            = {DEP_CCSS    END_STAMP};
#endif

/*! place your module dependencies here */

#ifdef CCSS_DOM_REF
    static const TaskID CCDOM_dep[]           = {DEP_CCDOM   END_STAMP};
#endif
#if !defined(TH_FSCOM) && defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED)
    #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
        static const TaskID FSF_REG_dep[]     = {DEP_REG     END_STAMP};       ///< FSF REG task dependency structure
    #endif
    #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
        static const TaskID FSF_RAM_dep[]     = {DEP_FSF     END_STAMP};       ///< dependency list of module #MOD_RAM
    #endif
    #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
        static const TaskID FSF_ROM_dep[]     = {DEP_ROM     END_STAMP};       ///< dependency list of module #MOD_ROM
    #endif
    #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
        static const TaskID FSF_STACK_dep[]   = {DEP_STACK   END_STAMP};       ///< dependency list of module #MOD_STACK
    #endif
    #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
        static const TaskID FSF_ISR_dep[]     = {DEP_ISR     END_STAMP};       ///< dependency list of module #MOD_ISR
    #endif
    #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
        static const TaskID FSF_FSCLK_dep[]   = {DEP_FSCLK   END_STAMP};
    #endif
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    static const TaskID FSF_dep[]             = {DEP_FSF     END_STAMP};
#endif


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
        {MOD_TIM,       HIGH_PRIORITY},
        {MOD_BAL,       HIGH_PRIORITY},
        {MOD_DBPL,      HIGH_PRIORITY},
        {MOD_DLL,       LOW_PRIORITY},
        {MOD_WDT,       LOW_PRIORITY},
    #ifdef DBUSCAN_INCLUDED
        {MOD_DBC,       LOW_PRIORITY},
    #endif
    #ifdef STACK_MONITOR_ENABLED
        {MOD_STK,       LOW_PRIORITY},
    #endif
    #ifdef CCSS
        {MOD_CCSS,      LOW_PRIORITY},
    #endif

        /*! place your project modules and their priorities here */

    #ifdef CCSS_DOM_REF
        {MOD_CCDOM,     LOW_PRIORITY},
    #endif
    #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
        {MOD_REG,       HIGH_PRIORITY},          ///< Functional Safety - register test
    #endif
    #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
        {MOD_RAM,       HIGH_PRIORITY},          ///< Functional Safety - ram test
    #endif
    #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
        {MOD_ROM,       HIGH_PRIORITY},          ///< Functional Safety - rom test
    #endif
    #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
        {MOD_STACK,     HIGH_PRIORITY},          ///< Functional Safety - stack test
    #endif
    #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
        {MOD_ISR,       HIGH_PRIORITY},
    #endif
    #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
        {MOD_CLK,       HIGH_PRIORITY},
    #endif
    #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
        {MOD_FSF,       HIGH_PRIORITY},
    #endif

        {END_STAMP,     PRIO_NONE}
    },


    /***************************************************************************************************
     * TASK HANDLERS LIST
     * *************************************************************************************************
     * \brief: Add corresponding task handlers for each module here
     ***************************************************************************************************/
    .TaskHandlersList =
    {
        [MOD_TIM]       = TIM_handleTask16,  /* TIM_handleTask if no 16Bit timer used */
        [MOD_BAL]       = BAL_HandleTask,
        [MOD_DBPL]      = DBPL_HandleTask,
        [MOD_DLL]       = DLL_HandleTask,
        [MOD_WDT]       = WDT_handleTask,
    #ifdef DBUSCAN_INCLUDED
        [MOD_DBC]       = DBCDRV_HandleTask,
    #endif
    #ifdef STACK_MONITOR_ENABLED
        [MOD_STK]       = STK_HandleTask,
    #endif
    #ifdef CCSS
        [MOD_CCSS]      = CCSS_RunSyStTask,
    #endif

    /*! place your task handlers here */

    #ifdef CCSS_DOM_REF
        [MOD_CCDOM]     = CCDOM_RunDomRefTask,
    #endif
    #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
        [MOD_REG]       = FSF_ucRegHandleTask,
    #endif
    #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
        [MOD_RAM]       = FSF_ucRamHandleTask,
    #endif
    #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
        [MOD_ROM]       = FSF_ucRomHandleTask,
    #endif
    #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
        [MOD_STACK]     = FSF_ucStackHandleTask,
    #endif
    #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
        [MOD_ISR]       = FSF_ucIsrHandleTask,
    #endif
    #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
        [MOD_CLK]       = FSCLK_u8HandleTask,
    #endif
    #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
        [MOD_FSF]       = FSF_ucComHandleTask,
    #endif

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
        [MOD_TIM]       = TIM_dep,
        [MOD_BAL]       = BAL_dep,
        [MOD_DBPL]      = DBPL_dep,
        [MOD_DLL]       = DLL_dep,
        [MOD_WDT]       = WDT_dep,
    #ifdef DBUSCAN_INCLUDED
        [MOD_DBC]       = DBC_dep,
    #endif
    #ifdef STACK_MONITOR_ENABLED
        [MOD_STK]       = STK_dep,
    #endif
    #ifdef CCSS
        [MOD_CCSS]      = CCSS_dep,
    #endif

         /* place your module dependencies arrays here */

    #ifdef CCSS_DOM_REF
        [MOD_CCDOM]     = CCDOM_dep,
    #endif
    #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
        [MOD_REG]       = FSF_REG_dep,
    #endif
    #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
        [MOD_RAM]       = FSF_RAM_dep,
    #endif
    #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
        [MOD_ROM]       = FSF_ROM_dep,
    #endif
    #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
        [MOD_STACK]     = FSF_STACK_dep,
    #endif
    #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
        [MOD_ISR]       = FSF_ISR_dep,
    #endif
    #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
        [MOD_CLK]       = FSF_FSCLK_dep,
    #endif
    #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
        [MOD_FSF]       = FSF_dep,
    #endif

    },
};


/********************************************* End of File ******************************************************/
