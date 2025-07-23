/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        STM32G0
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This file defines the interrupt functions.
*
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "processor.h"
#include "IntTbl.h"
#include "SegmentDef.h"

#ifdef MCAL_INCLUDED
#include "mcal/mcal_includes.h"
#endif

/******************************************************************************/
/* IRQ FUNCTIONS DEFINITIONS                                                  */
/******************************************************************************/

SYMBOL_IRQ void USART1_IRQHandler(void)
{
  #if defined(HUART0_USED)
    HUART0_ISR_HandleEvent();
  #elif defined(MCAL_MUART_INCLUDED)
    MUART_IRQ_Handler(MUART1_IRQ_INDEX);
  #endif
}

#if defined(STM32G0B1) || defined(STM32G0C1)
    SYMBOL_IRQ void USART2_LPUART2_IRQHandler(void)
#else
    SYMBOL_IRQ void USART2_IRQHandler(void)
#endif
{
  #if defined(HUART1_USED) || defined(HUART11_USED)
    HUART1_11_ISR_HandleEvent();
  #elif defined(MCAL_MUART_INCLUDED)
    MUART_IRQ_Handler(MUART2_LP2_IRQ_INDEX);
  #endif
}

#if defined(STM32G0B1) || defined(STM32G0C1)
    SYMBOL_IRQ void USART3_USART4_USART5_USART6_LPUART1_IRQHandler(void)
#elif defined(STM32G070)
    SYMBOL_IRQ void USART3_USART4_IRQHandler(void)
#elif defined(STM32G0B0)
    SYMBOL_IRQ void USART3_USART4_USART5_USART6_IRQHandler(void)
#else
    SYMBOL_IRQ void USART3_USART4_LPUART1_IRQHandler(void)
#endif
{
  #if defined(HUART2_USED) || defined(HUART3_USED) || defined(HUART4_USED) || defined(HUART5_USED) || defined(HUART10_USED)
    #if defined(STM32G0B0)
      HUART2_3_4_5_ISR_HandleEvent();
    #else
      HUART2_3_10_ISR_HandleEvent();
    #endif
  #elif defined(MCAL_MUART_INCLUDED)
    MUART_IRQ_Handler(MUART3_4_5_6_LP1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void EXTI0_1_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI0_1_IRQ_INDEX);
  #else
    HINT01_ISR_HandleEvent();
  #endif
}

SYMBOL_IRQ void EXTI2_3_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI2_3_IRQ_INDEX);
  #else
    HINT23_ISR_HandleEvent();
  #endif
}

SYMBOL_IRQ void EXTI4_15_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI4_15_IRQ_INDEX);
  #else
    HINT415_ISR_HandleEvent();
  #endif
}

SYMBOL_IRQ void ADC_COMP_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void TIM1_BRK_UP_TRG_COMP_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM1_IRQ_INDEX);
  #else
    HTIM1_TimerEvent_ISR_HandleEvent();
  #endif
}

SYMBOL_IRQ void TIM1_CC_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM1_IRQ_INDEX);
  #else
    HTIM1_ISR_HandleEvent();
  #endif
}

SYMBOL_IRQ void TIM2_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM2_IRQ_INDEX);
  #else
    HTIM2_ISR_HandleEvent();
  #endif
}


#if defined(STM32G0B1) || defined(STM32G0C1)
    SYMBOL_IRQ void TIM3_TIM4_IRQHandler(void)
#else
    SYMBOL_IRQ void TIM3_IRQHandler(void)
#endif
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM3_4_IRQ_INDEX);
  #else
    HTIM3_ISR_HandleEvent();
  #endif
}

#if defined(MTIM6_PRESENT) && defined(MTIMLP1_PRESENT)
SYMBOL_IRQ void TIM6_DAC_LPTIM1_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM6_LP1_IRQ_INDEX);
  #endif
}
#endif

#if defined(MTIM7_PRESENT) && defined(MTIMLP2_PRESENT)
SYMBOL_IRQ void TIM7_LPTIM2_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM7_LP2_IRQ_INDEX);
  #endif
}
#endif

SYMBOL_IRQ void TIM14_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM14_IRQ_INDEX);
  #else
    HTIM14_ISR_HandleEvent();
  #endif
}

#ifdef MTIM15_PRESENT
SYMBOL_IRQ void TIM15_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM15_IRQ_INDEX);
  #else
    HTIM15_ISR_HandleEvent();
  #endif
}
#endif

#if defined(STM32G0B1) || defined(STM32G0C1)
    SYMBOL_IRQ void TIM16_FDCAN_IT0_IRQHandler(void)
#else
    SYMBOL_IRQ void TIM16_IRQHandler(void)
#endif
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM16_IRQ_INDEX);
  #else
    HTIM16_ISR_HandleEvent();
  #endif
}

#if defined(STM32G0B1) || defined(STM32G0C1)
    SYMBOL_IRQ void TIM17_FDCAN_IT1_IRQHandler(void)
#else
    SYMBOL_IRQ void TIM17_IRQHandler(void)
#endif
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM17_IRQ_INDEX);
  #else
    HTIM17_ISR_HandleEvent();
  #endif
}

SYMBOL_IRQ void I2C1_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C1_IRQ_INDEX);
  #endif
}

#if defined(STM32G0B1) || defined(STM32G0C1)
    SYMBOL_IRQ void I2C2_I2C3_IRQHandler(void)
#else
    SYMBOL_IRQ void I2C2_IRQHandler(void)
#endif
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C2_3_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void SPI1_IRQHandler(void)
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI1_IRQ_INDEX);
  #else
    HSPI0_vIsrHandleTxRxEvent();
  #endif
}

#if defined(STM32G0B1) || defined(STM32G0C1)
    SYMBOL_IRQ void SPI2_SPI3_IRQHandler(void)
#else
    SYMBOL_IRQ void SPI2_IRQHandler(void)
#endif
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI2_3_IRQ_INDEX);
  #else
    HSPI1_2_vIsrHandleTxRxEvent();
  #endif
}

SYMBOL_IRQ void DMA_Channel1_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA_Channel2_3_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA_Channel4_5_6_7_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}
