/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / SiLabs EFR32xG21
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
    \brief EFR32xG21 specific configuration template for timer library.

*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "timerlib.h"
#include "processor.h"
#include "emlib/em_timer.h"
#include "emlib/em_cmu.h"


// Default prescaler value for HFRCOEM23 = 32MHz
#define TIMER1_PRESCALER timerPrescale32

BOOL _TIM_bGetInitConfig(void)
{
    /* Select HFRCOEM23 as input clock for TIMERx*/
    CMU_ClockSelectSet((uint8_t)cmuClock_EM01GRPACLK, (uint32_t)cmuSelect_HFRCOEM23);
    // Setup & enable TIMER1 for defined prescaler
    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
    timerInit.prescale = TIMER1_PRESCALER;
    /*lint -e934 "Taking address of near auto variable [MISRA 2012 Rule 1.3, required]", relevant for DLLs only */
    TIMER_Init(TIMER1, &timerInit);
    // Set Top value for TIMER1 counter
    TIMER_TopSet(TIMER1, 0xFFFFU);
    return TRUE;
}

uint16 TIM_uiGetCircleMicroSeconds(void)
{
    return (uint16) TIMER_CounterGet(TIMER1);
}
