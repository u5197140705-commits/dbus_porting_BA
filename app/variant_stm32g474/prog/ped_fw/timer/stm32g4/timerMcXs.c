/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
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
    \brief STM32G4 specific configuration template for timer library.

*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "timerlib.h"
#include "processor.h"

/*         Block diagram of the Stm32 clock generation unit


     ______________________        ____HCLK______________                               
    |                      |      |                      |                              
    |  External oscillator |      |         _____________V_______________               
    |     (e.g. 8MHz)      |      |        |APB1                         |              
    |______________________|      |        |          PCLOCK             |
               |                  |        |     (from build process)    |              
             PLLSRC               |        |_____________________________|              
               |                  |                      |                              
  _____________V_______________   |                    PCLK1 (max. 170 MHz)              
 |                             |  |                      |                              
 |   CORE_CLOCK = 170,80.. 16  |__|        ______________V_________________             
 |     (from build process)    |  |       |                                |            
 |_____________________________|  |       | LPUART1 ,USART2-3, UART4-5     |            
                                  |       | PWR ,FDCAN , I2C1-4 , SPI2-3   |            
                                  |       | IWDG , WWDG , RTC , LPTIM1,2   |            
                                  |       |________________________________|            
                                  |                                                     
                                  |                                                     
                                  |                                                     
                                  |____HCLK______________                               
                                                         |                              
                                            _____________V_______________               
                                           |APB2                         |              
                                           |         PCLOCK1             |
                                           |     (from build process)    |              
                                           |_____________________________|              
                                                         |                              
                                                       PCLK2                            
                                                         |                              
                                           ______________V_________________             
                                          |                                |            
                                          |  TIM1,8,20,15,16,17, USART1,   |            
                                          |  SPI1/4,                       |            
                                          |________________________________|            

*/
/* How to set CORE_CLOCK and PCLOCK,PCLOCK1 in the build process.
If there is 8MHz external oscillator and we want the CPU to run at 48MHz
CORE_CLOCK define will be CORE_CLOCK = 48. Now look into the tables given for the PCLOCK,PCLOCK1
and from the row for the corresponding CORE_CLOCK frequency pick the number you want the bus will be 
running at, e.g. :

 ---------------------- PCLOCK ----------------------------
# Possible values for 8MHz external crystal :                          
# --------------------------------------------------------------       
# CORE_CLOCK frequency (HCLK) divided by   1   2   4   8  16           
# --------------------------------------------------------------       
# If CORE_CLOCK = 170, PCLOCK may be :    170  todo: lower PCLOCKs
# If CORE_CLOCK = 84, PCLOCK may be :      84
# If CORE_CLOCK = 80, PCLOCK may be :      80
# If CORE_CLOCK = 72, PCLOCK may be :          36, 18, 9, 4.5
# If CORE_CLOCK = 64, PCLOCK may be :          32, 16, 8, 4
# If CORE_CLOCK = 56, PCLOCK may be :          28, 14, 7, 3.5
# If CORE_CLOCK = 48, PCLOCK may be :          24, 12, 6, 3
# If CORE_CLOCK = 40, PCLOCK may be :          20, 10, 5, 2.5   <------- this is us
# If CORE_CLOCK = 32, PCLOCK may be :      32, 16, 8,  4, 2
# If CORE_CLOCK = 24, PCLOCK may be :      24, 12, 6,  3, 1.5
# If CORE_CLOCK = 16, PCLOCK may be :      16, 8,  4,  2, 1



 ---------------------- PCLOCK1 ----------------------------
# Possible values for 8MHz external crystal :                          
# --------------------------------------------------------------       
# CORE_CLOCK frequency (HCLK) divided by   1   2   4   8  16           
# --------------------------------------------------------------       
# If CORE_CLOCK = 170, PCLOCK1 may be :   170  todo: lower PCLOCKs
# If CORE_CLOCK = 84, PCLOCK1 may be :     84
# If CORE_CLOCK = 80, PCLOCK1 may be :     80
# If CORE_CLOCK = 72, PCLOCK1 may be :      72, 36, 18, 9, 4.5
# If CORE_CLOCK = 64, PCLOCK1 may be :      64, 32, 16, 8, 4
# If CORE_CLOCK = 56, PCLOCK1 may be :      56, 28, 14, 7, 3.5
# If CORE_CLOCK = 48, PCLOCK1 may be :      48, 24, 12, 6, 3
# If CORE_CLOCK = 40, PCLOCK1 may be :      40, 20, 10, 5, 2.5  <------- this is us
# If CORE_CLOCK = 32, PCLOCK1 may be :      32, 16, 8,  4, 2
# If CORE_CLOCK = 24, PCLOCK1 may be :      24, 12, 6,  3, 1.5
# If CORE_CLOCK = 16, PCLOCK1 may be :      16, 8,  4,  2, 1

*/


/* DEFINITIONS ****************************************************************************************/

/** The TIMER_PRESCALER macro is used to adjust the timer to increment every microsecond. The main CLK is divided by TIMER_PRESCALER+1.
Note: If PCLOCK is not equal to CORE_CLOCK (APB prescaler is >1), timers TIM2-TIM7 = 2x PCLOCK, otherwise timer clock = PCLOCK */

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
    RCC_APB1LENR_TIM2EN = 1;
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

