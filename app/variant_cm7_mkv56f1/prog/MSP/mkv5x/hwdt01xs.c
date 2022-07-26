/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PED-Framework
*   COMP_ABBREV      HWDT
*   AUTHOR           Volcko Tomas
*   CREATED          11.10.2018  16:13:25
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Implementation of interrupt service routines for hwdt01xs.
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hwdt.h"


/******************************************************************************/
/* DEFINITIONS OF INTERRUPT SERVICES                                          */
/******************************************************************************/
/**
 *
 *   \brief Configuration structure to set WDOG\n
 *
 *    - const HWDT_Config_t HWDT_atConfig[X] = {
 *          {
 *              bool       A,                window mode enabled
 *              wdogClockPrescaler_t   B,    wdog clock prescaler
 *              uint16_t   C,                window value
 *              uint16_t   D                 timeout value
 *          }
 *      };
 *    - A - window mode, true - ON, false - OFF
 *    - B - 3bit prescaler
 *    - C - WINDOW VALUE, MAX - 0xFFFFU
 *    - D - TIMEOUT VALUE, MAX - 0xFFFFU
 *
 *
 */
const HWDT_Config_t HWDT_atConfig[1] = 
{
   {
     false,                           /* window mode   */
     HWDOG_CLOCK_PRESCALER_DIVIDE1,   /* Prescaler     */
     0U,                              /* Window value  */
     1000U                            /* Timeout value */
    }
};
