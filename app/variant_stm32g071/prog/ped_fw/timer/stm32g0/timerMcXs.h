/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT         Generic SW
 *  COMP_ABBREV     TIM
 ******************************************************************************/


#ifndef TIMER_MC_XS_H
#define TIMER_MC_XS_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     timerMcXs.h
 *
 *  \ingroup  PED-FW
 *
 *  \brief    Public inline functions and macro definitions specific for
 *            STM32G0 TIMER.
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "processor.h"
#include "timerlib.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/** \brief    The macro function returns the current 32 bit timer
 *            value in microseconds.
 *
 *  \details  The macro could be used to get direct value of the 32 bit timer
 *            in cases when using standard function TIM_u32GetCircleMicroSeconds
 *            is not suitable.
 */
#ifdef USE_32BIT_HWTIMER
    #if defined(TIM2_CR1)
        #define TIM_getDirectTimer32BitMicroseconds()   (TIM2_CNT)
    #else
        #error "No 32bit HW timer source for this derivative!"
    #endif // TIM2_CR1
#endif // USE_32BIT_HWTIMER

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC INLINE FUNCTION DEFINITIONS                                         */
/******************************************************************************/
/** \brief    The function returns a direct 32bit counter value.
 *            Interface must be implemented platform specific to STM32G0.
 *
 *  \details  The counter counts with 1MHz frequency upward by default from 0
 *            to 0xFFFFFFFF and auto-reloads.\n
 *            This is an optional library function. It is defined if a 32bit HW
 *            timer is required.\n
 *            Usually, this version of the circulating counter is used when 
 *            time measurement with periods of more than 65ms is required.
 *
 *  \return   32 bit timer value in microseconds in range 0 to 0xFFFFFFFF
 */

#ifdef USE_32BIT_HWTIMER
static inline uint32_t TIM_u32GetCircleMicroSeconds(void)
{
    return (uint32_t) (TIM2_CNT);
}
#endif // USE_32BIT_HWTIMER


#ifdef __cplusplus
}
#endif

#endif // TIMER_MC_XS_H
