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
/// \file Public User application specific definitions and declarations for scheduler

#ifndef SCHEDULER_CFG_H_
#define SCHEDULER_CFG_H_


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "scheduler/scheduler_defines.h"



/**************************************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                                        */
/**************************************************************************************************/
/***************************************************************************************************
 * MODULE ENUMERATIONS
 * *************************************************************************************************
 * \brief: Create the IDs that represent each module here, use naming convention MOD_<ModAbbr>
 * *************************************************************************************************
 * \note: maintain this same order of module IDs in DEPENDENCY TABLE in tick_schedDef.c
 ***************************************************************************************************/
#ifdef SCHEDULER_CONFIG_AUTO
#include LD_STRINGIZE(scheduler/VARIANT/scheduler_auto.h)
#else
typedef enum SCH_ModuleIDs
{
        MOD_TIM,            ///< module ID of timer library
        MOD_BAL,            ///< module ID of dbus bus layer
        MOD_DBPL,           ///< module ID of dbus presentation layer
        MOD_DLL,            ///< module ID of dbus data link layer
        MOD_WDT,            ///< module ID of watchdogtimer
    #ifdef DBUSCAN_INCLUDED
        MOD_DBC,            ///< module ID of task handler for DBusCAN chip
    #endif
    #ifdef STACK_MONITOR_ENABLED
        MOD_STK,            ///< module ID of task handler for Stack Monitor
    #endif
    #ifdef CCSS
        MOD_CCSS,           ///< module ID of SystemStates Taskhandler
    #endif

        /*! place your project modules here */
        MOD_ATSSB,          ///< module ID of Smart Sensor Bus application task

    #ifdef CCSS_DOM_REF
        MOD_CCDOM,          ///< module ID of SystemStates Domain Reference implementation. to be replaced with an application specific implementation.
    #endif
    #ifdef FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
        MOD_REG,            ///< Functional Safety - register test
    #endif
    #ifdef FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
        MOD_RAM,            ///< Functional Safety - ram test
    #endif
    #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
        MOD_ROM,            ///< Functional Safety - rom test
    #endif
    #ifdef FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
        MOD_STACK,          ///< Functional Safety - stack test
    #endif
    #ifdef FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
        MOD_ISR,            ///< Functional Safety - ISR test
    #endif
    #ifdef FUNCTIONAL_SAFETY_SYS_CLK_CHECK_ACTIVE
        MOD_CLK,            ///< Functional Safety - SYS clock test
    #endif
    #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
        MOD_FSF,
    #endif

        TOTAL_MODULES
} TaskID;
#endif /* SCHEDULER_AUTO */


/**************************************************************************************************/
/* PUBLIC DEFINITIONS                                                                             */
/**************************************************************************************************/
#define SCH_ACTIVATE_DEBUG_MODE


/***************************************************************************************************
 * SELECT DEBUG MODE
 * *************************************************************************************************
 * \brief: Two Configuration Modes possible; 'DEBUG_SINGLE_MODULE' or 'DEBUG_ALL_MODULES'
 *         By Default DEBUG_ALL_MODULES mode is selected!
 * *************************************************************************************************
 * \note: Please select the desired Debug Mode below, 'DEBUG_SINGLE_MODULE' or 'DEBUG_ALL_MODULES'
 ***************************************************************************************************/
#define DEBUG_MODE  DEBUG_ALL_MODULES       //< Select Debug Mode


/***************************************************************************************************
 * SELECT MODULE ID for DEBUG_SINGLE_MODULE MODE
 * *************************************************************************************************
 * \brief: If DEBUG_SINGLE_MODULE mode is selected, the Module ID which needs to be debugged,
 *         must be selected below
 ***************************************************************************************************/
#if(DEBUG_MODE == DEBUG_SINGLE_MODULE)
#define DEBUG_MOD_ID                        //< Select Module ID, example MOD_TIM
#endif


/***************************************************************************************************
 * SELECT NUMBER OF TIMING SAMPLES
 * *************************************************************************************************
 * \brief: Number of Timing Samples for each module to be saved, by default 3 samples!
 ***************************************************************************************************/
#define NUMBER_OF_SAMPLES   2U              //< Select number of samples


/***************************************************************************************************
 * SELECT MODE OF PERFORMANCE
 * *************************************************************************************************
 * \brief: To run the Scheduler in Optimize Performance mode the below's define must be active 
           which increase the use of RAM space and also increase the performance and vise versa.                                               
  ***************************************************************************************************/
//#define SCH_OPTIMIZE_PERFORMANCE

/***************************************************************************************************
 * SCHEDULER - PROJECT INVENTORY
 * *************************************************************************************************
 * \brief: If other than the Default Project Inventory, which is configured in scheduler_config.c, is
 *         needed then please add it here (can also be included via a header file) and it can be passed
 *         onto the scheduler with a function call SCH_setupScheduler(&<project_catalog>)' in
 *         BASIC_vUsrInitAppl().
 ***************************************************************************************************/
extern struct SCH_ProjectInventory SCH_DefaultProjectCatalogue;
#endif

/********************************************* End of File ******************************************************/
