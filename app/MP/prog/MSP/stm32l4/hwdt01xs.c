/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      HUART
 *  PROCESSOR        STM32L4
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief  Configuration for STM32L4xx Watchdog
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hwdt.h"

/******************************************************************************/
/* PUBLIC DATA DEFINTIONS                                                     */
/******************************************************************************/

/* Predefined structure for IWDG */
/*const T_HWDT_Config HWDT_atConfig[] = { HWDT_USE_IWDG, HWDT_IWDT_DIVIDER_32, HWDT_IWDT_WINDOW_MAX_VALUE, HWDT_IWDT_MAX_RELOAD_VALUE }; */

/* Predefined structure for WWDG */
/* const T_HWDT_Config HWDT_atConfig[] = { HWDT_USE_WWDG, HWDT_COUNTER_CLOCK_DIV_128, HWDT_WINDOW_MAX_VALUE, HWDT_MAX_RELOAD_VALUE }; */

/* Predefined structures for IWDG and WWDG */
/* Use HWDT_vInit(0); for IWDG or HWDT_vInit(1); for use WWDG */
const T_HWDT_Config HWDT_atConfig[] = { {HWDT_USE_IWDG, HWDT_IWDT_DIVIDER_8, HWDT_IWDT_WINDOW_MAX_VALUE, HWDT_IWDT_MAX_RELOAD_VALUE},
                                       {HWDT_USE_WWDG, HWDT_COUNTER_CLOCK_DIV_8, HWDT_WINDOW_MAX_VALUE, HWDT_MAX_RELOAD_VALUE} };
