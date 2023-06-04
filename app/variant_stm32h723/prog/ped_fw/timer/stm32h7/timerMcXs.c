/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      TIM
 ******************************************************************************/
 
 
 /*****************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
    \brief STM32H7 specific configuration template for timer library.

*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "timerlib.h"
#include "processor.h"


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/** The TIMER_PRESCALER macro is used to adjust the timer to increment every microsecond. 
    The main CLK is divided by TIMER_PRESCALER+1.
*/

#if (PCLOCK <= 275U)
    #define TIMER_PRESCALER             (PCLOCK - 1U)
#else
    #error "No valid peripheral clock defined. PCLOCK must be correctly defined"
#endif


/******************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                */
/******************************************************************************/

BOOL _TIM_bGetInitConfig(void)
{
    /* Enable clock for timer unit. */
    RCC_APB1LENR_TIM2EN = 1;
    /* readout of RCC register necessary to make sure that previous write was finished */
    (void)RCC_APB1LENR_TIM2EN;
    /* readout of TIM register necessary to make sure that peripheral is ready */
    (void)TIM2_PSC;
    /* Set the prescaler for microsecond base. */
    TIM2_PSC = TIMER_PRESCALER;
    TIM2_ARR = 0xFFFF;
    /* generate update event (UEV) to immediately set new prescaler */
    TIM2_EGR_UG = 1;
    /* clear timer update event flag <- maybe not needed in timerMcXs.c, as no interrupts are used there */
    TIM2_SR_UIF = 0;
    /* Enable the timer. */
    TIM2_CR1 = 1;

    return TRUE;
}

uint16 TIM_uiGetCircleMicroSeconds(void)
{
    return (uint16) (TIM2_CNT);
}
