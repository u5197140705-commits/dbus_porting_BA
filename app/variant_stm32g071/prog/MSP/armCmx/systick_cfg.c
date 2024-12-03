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
    #include "rtos_api.h"
#endif

#ifdef SYSTEM_TIMER_SYSTICK
    #include "system_timer.h"
#endif
#ifdef DC_COMPONENT_PRESENT
#include "DC_api.h"
#endif

#include "IntTblArmCM.h"

/* Info 765: external symbol 'SysTick_Handler' could be made static [MISRA 2012 Rule 8.7, advisory]
 * it needs to be external! */
/*lint -esym(765,SysTick_Handler) */

/******************************************************************************/
/* PUBLIC FUNCTIONS DECLARATIONS                                              */
/******************************************************************************/

void SysTick_Handler(void)
{
#ifdef RTOS

    #ifdef RTOS_MODE_DEBUG
        RTOS_startMeasureISRTime();
    #endif //RTOS_MODE_DEBUG

    /* RTOS timer interrupt handler */
    RTOS_SysTickInterruptHandler();

    /* System timer interrupt handler */
    #ifdef SYSTEM_TIMER_SYSTICK
        STIM_InterruptHandler(SysTick_GetPeriod());
    #endif

    #ifdef RTOS_MODE_DEBUG
        RTOS_stopMeasureISRTime();
    #endif //RTOS_MODE_DEBUG

#elif defined(DC_COMPONENT_PRESENT)
    DC_systickInterrupHandler();
#else
    #ifdef SYSTEM_TIMER_SYSTICK
        /* System timer interrupt handler */
        STIM_InterruptHandler(SysTick_GetPeriod());
    #endif
#endif //!RTOS
}
