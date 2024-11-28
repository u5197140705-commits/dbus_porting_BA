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
 ******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Public User application specific definitions and declarations for scheduler

#ifndef SCHEDULER_AUTO_H_
#define SCHEDULER_AUTO_H_


/**************************************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                                        */
/**************************************************************************************************/
/***************************************************************************************************
 * MODULE ENUMERATIONS
 * *************************************************************************************************
 * \brief: Create the IDs that represent each module here, use naming convention MOD_<ModAbbr>
 * *************************************************************************************************
 * \note: maintain this same order of module IDs in DEPENDENCY TABLE in scheduler_auto.c
 ***************************************************************************************************/
typedef enum SCH_ModuleIDs
{
    MOD_SSBF,
    MOD_BAL,
    MOD_DBPL,
    MOD_DLL,
    MOD_TIM,
    MOD_WDT,
    MOD_STK,

    /* USER CODE BEGIN ProjectModules */
    /*! place your projects modules here */
    MOD_ATSSB,
    /* USER CODE END ProjectModules */

    TOTAL_MODULES
} TaskID;

#endif
