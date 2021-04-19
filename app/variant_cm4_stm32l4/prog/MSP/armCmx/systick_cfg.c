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
 *  COMP_ABBREV      SysTick
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public definitions and declarations for unit SysTick.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "systick.h"

#ifdef RTOS
    #include "os/rtos_interrupts.h"
#endif

#ifdef SYSTEM_TIMER_SYSTICK
    #include "system_timer.h"
#endif
#include "IntTblArmCM.h"


/******************************************************************************/
/* PUBLIC FUNCTIONS DECLARATIONS                                              */
/******************************************************************************/

void SysTick_Handler(void)
{
#ifdef RTOS
    /* RTOS timer interrupt handler */
    RTOS_SysTickInterruptHandler();
#endif
    
#ifdef SYSTEM_TIMER_SYSTICK
    /* System timer interrupt handler */
    STIM_InterruptHandler(SysTick_GetPeriod());
#endif
}
