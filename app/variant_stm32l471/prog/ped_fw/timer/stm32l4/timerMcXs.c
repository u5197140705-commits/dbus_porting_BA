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
/** \file
    \brief STM32L4 specific configuration template for timer library for STM32L4.

*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "timerlib.h"
#include "processor.h"

//lint -esym( 750, TIMERLIB_MODULE) "message: symbol mot used..."

/*         Block diagram of the Stm32 clock generation unit


     ______________________        ____HCLK______________                               
    |                      |      |                      |                              
    |  External oscilator  |      |         _____________V_______________               
    |     (e.g. 8MHz)      |      |        |APB1                         |              
    |______________________|      |        |          PCLOCK             |
               |                  |        |     (from build process)    |              
             PLLSRC               |        |_____________________________|              
               |                  |                      |                              
  _____________V_______________   |                    PCLK1 (max. 80 MHz)              
 |                             |  |                      |                              
 |   CORE_CLOCK = 80,72 .. 16  |__|        ______________V_________________             
 |     (from build process)    |  |       |                                |            
 |_____________________________|  |       | USART2/3, UART4/5, LPUART1,    |            
                                  |       | TIM2..TIM7, LPTIM1/2, I2C1..3, |            
                                  |       | DAC1, PWR, OPAMP, CAN1, RTC,   |            
                                  |       | IWDG, WWDG, SPI2/3, SWPMI1     |            
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
                                                       PCLK2 (max. 80 MHz)              
                                                         |                              
                                           ______________V_________________             
                                          |                                |            
                                          |  USART1, TIM1/8, TIM15..17,    |
                                          |  EXTI, COMP, VREFBUF, SPI1,    |            
                                          |  SAI1/2, DFSDM1, SYSCFG,       |
                                          |  FIREWALL, SDMMC1              |
                                          |________________________________|            

*/
/* How to set CORE_CLOCK and PCLOCK,PCLOCK1 in the build process.
If there is 8MHz external oscillator and we want the CPU to run at 40MHz
CORE_CLOCK define will be CORE_CLOCK = 40. Now look into the tables given for the PCLOCK,PCLOCK1
and from the row for the corresponding CORE_CLOCK frequency pick the number you want the bus will be 
running at, e.g. :

 ---------------------- PCLOCK ----------------------------
# Possible values for 8MHz external crystal :                          
# --------------------------------------------------------------       
# CORE_CLOCK frequency (HCLK) divided by    1   2   4   8  16           
# --------------------------------------------------------------       
# If CORE_CLOCK = 80, PCLOCK may be :       80, 40, 20,10, 5
# If CORE_CLOCK = 72, PCLOCK may be :       72, 36, 18, 9, 4.5
# If CORE_CLOCK = 64, PCLOCK may be :       64, 32, 16, 8, 4
# If CORE_CLOCK = 56, PCLOCK may be :       56, 28, 14, 7, 3.5
# If CORE_CLOCK = 48, PCLOCK may be :       48, 24, 12, 6, 3
# If CORE_CLOCK = 40, PCLOCK may be :       40, 20, 10, 5, 2.5   <------- this is us
# If CORE_CLOCK = 32, PCLOCK may be :       32, 16, 8,  4, 2
# If CORE_CLOCK = 24, PCLOCK may be :       24, 12, 6,  3, 1.5
# If CORE_CLOCK = 16, PCLOCK may be :       16, 8,  4,  2, 1


 ---------------------- PCLOCK1 ----------------------------
# Possible values for 8MHz external crystal :                          
# --------------------------------------------------------------       
# CORE_CLOCK frequency (HCLK) divided by   1   2   4   8  16           
# --------------------------------------------------------------       
# If CORE_CLOCK = 80, PCLOCK1 may be :     80, 40, 20,10, 5
# If CORE_CLOCK = 72, PCLOCK2 may be :     72, 36, 18, 9, 4.5
# If CORE_CLOCK = 64, PCLOCK2 may be :     64, 32, 16, 8, 4
# If CORE_CLOCK = 56, PCLOCK2 may be :     56, 28, 14, 7, 3.5
# If CORE_CLOCK = 48, PCLOCK2 may be :     48, 24, 12, 6, 3
# If CORE_CLOCK = 40, PCLOCK2 may be :     40, 20, 10, 5, 2.5  <------- this is us
# If CORE_CLOCK = 32, PCLOCK2 may be :     32, 16, 8,  4, 2
# If CORE_CLOCK = 24, PCLOCK2 may be :     24, 12, 6,  3, 1.5
# If CORE_CLOCK = 16, PCLOCK2 may be :     16, 8,  4,  2, 1

*/


/* DEFINITIONS ****************************************************************************************/

/** The TIMER_PRESCALER macro is used to adjust the timer to increment every microsecond. The main CLK is divided by TIMER_PRESCALER+1.
For CLOCK_CORE=80 prescaler for APB1 must be 2 as APB1 max freq is 40 MHz but if APB1 prescaler if >1 timersTIM1-TIM6 are fed by x2 the
frequency of the APB1. */
/*  */
#if (CORE_CLOCK <= 80U) && ((CORE_CLOCK % PCLOCK) == 0U)
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
    RCC_APB1ENR1_TIM2EN = 1;
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

