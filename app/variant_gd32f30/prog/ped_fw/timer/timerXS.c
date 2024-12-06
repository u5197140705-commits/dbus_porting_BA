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
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file timerXS.c
    \brief Configuration template for timer library.

       This is the common project specific platform independent configuration.
*/



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "hsup.h"
#include "SegmentDef.h"
#include "timerlib.h"
#ifdef USE_32BIT_HWTIMER
   #include "timerMcXS.h"
#endif // USE_32BIT_HWTIMER

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/* Timerlib configuration switches comment in or out*/

/* comment in to use 16bis timer.
 * Note: To enable 16Bit timers call task handler TIM_bHandleTask16 must be called also
 */
#define TIM_16_BIT_TIMER_USED ///< Switches on the usage of 16Bit timers




#define BASE_TICKS_PER_US           (10000U) ///< base time in µs


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

#ifndef USE_32BIT_HWTIMER
typedef uint32_t Tclock;  ///< Type for circulating timer is 32 Bit
#endif // USE_32BIT_HWTIMER


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
SDEF_SetSegmentRW(START8_1)
/** Prescaler of 8-bit timer segment 1 */
static Ttimer8 TIM_uc8Start1;

SDEF_SetSegmentRW(START8_2)
/** Prescaler of 8-bit timer segment 2 */
static Ttimer8 TIM_ucStart2;

SDEF_SetSegmentRW(START8_3)
/** Prescaler of 8-bit timer segment 3 */
static Ttimer8 TIM_ucStart3;

SDEF_SetSegmentRW(START8_4)
/** Prescaler of 8-bit timer segment 4 */
static Ttimer8 TIM_ucStart4;

SDEF_SetSegmentRW(START8_5)
/** Prescaler of 8-bit timer segment 5 */
static Ttimer8 TIM_ucStart5;

SDEF_SetSegmentRW(START8_6)
/** Prescaler of 8-bit timer segment 6 */
static Ttimer8 TIM_ucStart6;

SDEF_SetSegmentRW(START8_7)
/** Prescaler of 8-bit timer segment 7 */
static Ttimer8 TIM_ucStart7;

SDEF_SetSegmentRW(START8_8)
/** Prescaler of 8-bit timer segment 8 */
static Ttimer8 TIM_ucStart8;

SDEF_SetSegmentRW(START8_9)
/** Prescaler of 8-bit timer segment 9 */
static Ttimer8 TIM_ucStart9;

SDEF_SetSegmentRW(START8_10)
/** Prescaler of 8-bit timer segment 10 */
static Ttimer8 TIM_ucStart10;

SDEF_SetSegmentRW(END8)
/** Status for timer library, end address of last 8 bit timer segment */
Ttimer8 TIM_uc8End;


#ifdef TIM_16_BIT_TIMER_USED

SDEF_SetSegmentRW(START16_1)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 1 */
static Ttimer8 TIM_uc16Start1;

SDEF_SetSegmentRW(START16_2)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 2 */
static Ttimer8 TIM_uc16Start2;

SDEF_SetSegmentRW(START16_3)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 3 */
static Ttimer8 TIM_uc16Start3;

SDEF_SetSegmentRW(START16_4)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 4 */
static Ttimer8 TIM_uc16Start4;

SDEF_SetSegmentRW(START16_5)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 5 */
static Ttimer8 TIM_uc16Start5;

SDEF_SetSegmentRW(START16_6)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 6 */
static Ttimer8 TIM_uc16Start6;

SDEF_SetSegmentRW(START16_7)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 7 */
static Ttimer8 TIM_uc16Start7;

SDEF_SetSegmentRW(START16_8)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 8 */
static Ttimer8 TIM_uc16Start8;

SDEF_SetSegmentRW(START16_9)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 9 */
static Ttimer8 TIM_uc16Start9;

SDEF_SetSegmentRW(START16_10)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 10 */
static Ttimer8 TIM_uc16Start10;

SDEF_SetSegmentRW(START16_11)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 11 */
static Ttimer8 TIM_uc16Start11;

SDEF_SetSegmentRW(START16_12)
/** TIM_uc16Start1; Prescaler of 16-bit timer segment 12 */
static Ttimer8 TIM_uc16Start12;

SDEF_SetSegmentRW(END16)
Ttimer8 TIM_uc16End;

#endif
SDEF_SetSegmentRW_Default() // switch back to default RW segment

static uint16_t TIM_uiTimeOld = 0;///< used to calculate time difference

#ifndef USE_32BIT_HWTIMER
static Tclock TIM_tCircleTime = 0;
#endif // USE_32BIT_HWTIMER

/******************************************************************************/
/* GLOBAL DATA                                                                */
/******************************************************************************/

/* QAC Error: Msg(5:6007)
   Comment  : Error belongs to an external library linked to the code.
              External linkage is required here!
*/

/*****************************************************************/
/* Note: The prescalers TIM_uc8Startxx must be in the same order */
/*       the timer segments defined by linker options             */
/*****************************************************************/

/* Table of 8 Bit Timer prescalers */
const TtimerConfig TIM_ptTimer8Config[] =
{
           {&TIM_uc8Start1,    10}, /* 10ms, change for other base times */
           {&TIM_ucStart2,     2},  /* 20ms  */
           {&TIM_ucStart3,     5},  /* 100ms */
           {&TIM_ucStart4,     2},  /* 200ms */
           {&TIM_ucStart5,     5},  /* 1s    */
           {&TIM_ucStart6,     2},  /* 2s    */
           {&TIM_ucStart7,     5},  /* 10s   */
           {&TIM_ucStart8,     2},  /* 20s   */
           {&TIM_ucStart9,     3},  /* 1min  */
           {&TIM_ucStart10,    2},  /* 2min  */
           {&TIM_uc8End,       0}   /* Do not change! Internal use and endmark of last segment */
};

/* Table of 16 Bit Timer prescalers */
const TtimerConfig TIM_ptTimer16Config[] ={

           {&TIM_uc16Start1,       10}, // 10ms, change for other base times
           {&TIM_uc16Start2,       2},  // 20ms
           {&TIM_uc16Start3,       5},  // 100ms
           {&TIM_uc16Start4,       2},  // 200ms
           {&TIM_uc16Start5,       5},  // 1s
           {&TIM_uc16Start6,       2},  // 2s
           {&TIM_uc16Start7,       5},  // 10s
           {&TIM_uc16Start8,       2},  // 20s
           {&TIM_uc16Start9,       3},  // 1min
           {&TIM_uc16Start10,      2},  // 2min
           {&TIM_uc16Start11,      5},  // 10min
           {&TIM_uc16Start12,      2},  // 20min
           {&TIM_uc16End,          0}   // do not change, internal use and endmark of last segment
};

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

BOOL _TIM_bIsBaseTimerDown(void)
{
   /* Is base time down? */
   while ( (uint16_t)( TIM_uiGetCircleMicroSeconds() - TIM_uiTimeOld ) >= BASE_TICKS_PER_US )
   {
      /* HW-Timer elapsed */
      HSUP_vDisableInt();
      TIM_uiTimeOld  += BASE_TICKS_PER_US;
#ifndef USE_32BIT_HWTIMER
      TIM_tCircleTime += BASE_TICKS_PER_US;
#endif // USE_32BIT_HWTIMER
      HSUP_vEnableInt();
      if( (uint16_t)( TIM_uiGetCircleMicroSeconds() - TIM_uiTimeOld ) < BASE_TICKS_PER_US )
      {
          return TRUE;
      }
   }
   return FALSE;
}

uint32_t TIM_u32GetCircleMicroSeconds(void)
{
#ifdef USE_32BIT_HWTIMER
   return TIM_getDirectTimer32BitMicroseconds();
#else
   uint16_t elapsedTime = (uint16_t)(TIM_uiGetCircleMicroSeconds() - TIM_uiTimeOld);
   return TIM_tCircleTime + (uint32_t)elapsedTime;
#endif // USE_32BIT_HWTIMER
}
