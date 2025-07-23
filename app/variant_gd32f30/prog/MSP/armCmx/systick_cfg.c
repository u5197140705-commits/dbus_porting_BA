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
#elif defined (ZRTOS)
    #include "os_port/osp_fwToOs.h"
#endif

#ifdef SYSTEM_TIMER_ENABLED
    #include "system_timer.h"
#endif
#ifdef DC_COMPONENT_PRESENT
#include "DC_api.h"
#endif

#include "IntTblArmCM.h"

#if defined (RTOS)
    #if defined (RTOS_MODE_DEBUG)
        #define OS_MODE_DEBUG
        #define OS_startMeasureISRTime    RTOS_startMeasureISRTime
        #define OS_stopMeasureISRTime     RTOS_stopMeasureISRTime
    #endif
    #define OS_sysTickHandler         RTOS_SysTickInterruptHandler
#elif defined (ZRTOS)
    #define OS_MODE_DEBUG
    #define OS_startMeasureISRTime    OSP_startMeasuerISRTime
    #define OS_stopMeasureISRTime     OSP_stopMeasuerISRTime
    #define OS_sysTickHandler         OSP_sysTickHandler
#endif

/* Info 765: external symbol 'SysTick_Handler' could be made static [MISRA 2012 Rule 8.7, advisory]
 * it needs to be external! */
/*lint -esym(765,SysTick_Handler) */

/******************************************************************************/
/* PUBLIC FUNCTIONS DECLARATIONS                                              */
/******************************************************************************/

void SysTick_Handler(void)
{
#ifdef OS_MODE_DEBUG
    OS_startMeasureISRTime();
#endif // OS_MODE_DEBUG

#if defined (RTOS) || defined (ZRTOS)
    /* OS timer interrupt handler */
    OS_sysTickHandler();
#endif // RTOS || ZRTOS

    /* System timer interrupt handler */
#ifdef SYSTEM_TIMER_ENABLED
    STIM_InterruptHandler();
#endif

#if defined(DC_COMPONENT_PRESENT)
    DC_systickInterrupHandler();
#endif // DC_COMPONENT_PRESENT

#ifdef OS_MODE_DEBUG
    OS_stopMeasureISRTime();
#endif // OS_MODE_DEBUG
}
