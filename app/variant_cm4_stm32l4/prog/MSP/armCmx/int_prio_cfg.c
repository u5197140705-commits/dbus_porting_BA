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
*   PROCESSOR        ARM CortexMx
*   COMP_ABBREV      INTP    
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Configuration file for customize Interrupt Levels
*
*/

#include "BSH_stdinc.h" 
#include "int_prio.h"
#include <stdint.h>
#include "processor.h" // find out cortex core 


void INTP_setApplicationPriority(void)
{
    /* Set Interrupt Priority of configurable interrupts          */
    /* Allowed values CM0: INT_PRIO_0 ... INT_PRIO_3              */
    /* Allowed values CM3, CM4, CM4F: INT_PRIO_0 ... INT_PRIO_15  */

    /* e.g. Set System Tick Interrupt to Level 1                  */
    /* NVIC_SetPriority(SysTick_IRQn, INT_PRIO_1)                 */
}
