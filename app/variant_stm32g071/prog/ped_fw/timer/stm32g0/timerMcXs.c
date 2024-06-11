/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
    \brief STM32G0 specific configuration template for timer library.

*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "timerlib.h"
#include "processor.h"
#ifdef MCAL_MPCM_INCLUDED
#include "mcal/mpcm.h"
#endif
/*
    These macros ensure compatibility between Stm32G071xx and Stm32G081xx platforms.
*/
#ifndef RCC_APBENR1_LPTIM1EN
#define RCC_APBENR1_LPTIM1EN     RCC_APB1ENR_LPTIM1EN
#endif

#if defined(MTIMLP1_PRESENT)
/******************************************************************************/
/* CODE FOR DERIVATIVES WITH LOW POWER TIMERS                                 */
/******************************************************************************/

BOOL _TIM_bGetInitConfig(void)
{
    /* Enable HSI clock */
    RCC_CR_HSION = 1;
    /* Enable clock for timer unit. */
    RCC_APBENR1_LPTIM1EN = 1;
    /* Low power timer 1 clock source selection, 2 = HSI16 */
    RCC_CCIPR_LPTIM1SEL = 2;
    /* Clock selector - internal osc */
    LPTIM1_CFGR_CKSEL = 0;
    /* Set the prescaler for microsecond base. */
    LPTIM1_CFGR_PRESC = 4;

    /* LPTIM1 Enable */
    LPTIM1_CR_ENABLE = 1;
    /* Auto reload value */
    LPTIM1_ARR_ARR = 0xFFFF;
    /* Counter start */
    LPTIM1_CR_CNTSTRT = 1;

    return TRUE;
}

uint16 TIM_uiGetCircleMicroSeconds(void)
{
/*
*   Following implementation is a workaround to solve the problem documented in STM32G0 errata.
*   However, this solution may lead to infinite loop in case of very low CORE_CLOCK frequencies (e.g. 4 MHz and less) used.
*   In such case please contact MCU Framework support.
*/
    uint16_t syncTimer;
    do
    {
        syncTimer = (uint16)LPTIM1_CNT_CNT;         // The LPTIM timer works with asynchronous clock.
    } while((uint16)LPTIM1_CNT_CNT != syncTimer);   // This check is necessary to correctly result from timer.
    return syncTimer;
}

#elif defined(MTIM14_PRESENT)
/******************************************************************************/
/* CODE FOR DERIVATIVES WITHOUT LOW POWER TIMERS - TIM14 IS USED              */
/******************************************************************************/
#if (CORE_CLOCK <= 170U) && ((CORE_CLOCK % PCLOCK) == 0U)
    #if ((CORE_CLOCK/PCLOCK) == 1U)
        #define TIMER_PRESCALER             (PCLOCK - 1U)
    #else
        #define TIMER_PRESCALER             ((PCLOCK * 2U) - 1U)
    #endif
#else
    #error "No valid peripheral clock defined. PCLOCK must be correctly defined"
#endif

BOOL _TIM_bGetInitConfig(void)
{

    /* Enable clock for timer unit. */
    RCC_APBENR2_TIM14EN = 1;
    /* Set the prescaler for microsecond base. */
    TIM14_PSC = TIMER_PRESCALER;
    /* Auto reload value */
    TIM14_ARR_ARR = 0xFFFF;
    /* generate update event (UEV) to immediately set new prescaler */
    TIM14_EGR_UG = 1;
    /* clear timer update event flag <- maybe not needed in timerMcXs.c, as no interrupts are used there */
    TIM14_SR_UIF = 0;
    /* Enable the timer. */
    TIM14_CR1_CEN = 1;

    return TRUE;
}

uint16 TIM_uiGetCircleMicroSeconds(void)
{
    return (uint16) (TIM14_CNT_CNT);
}

#else
    #error "No valid peripheral TIMER is present. HW TIMER must be defined."
#endif


#ifdef MCAL_MPCM_INCLUDED
void TIM_MpcmEventCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void) obj;
    (void) flags;
    (void) eventResponse;
    // re-initialize timer after MPCM power mode change
}
#endif
