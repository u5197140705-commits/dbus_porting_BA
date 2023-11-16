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
*   PROCESSOR        STM32G4
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

#ifdef MCAL_INCLUDED
#include "mcal/mcal_includes.h"
#endif

/******************************************************************************/
/* IRQ FUNCTIONS DEFINITIONS                                                  */
/******************************************************************************/

SYMBOL_IRQ void USART1_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART1_IRQ_INDEX);
  #endif
    HUART0_ISR_HandleEvent();
}

SYMBOL_IRQ void USART2_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART2_IRQ_INDEX);
  #endif
    HUART1_ISR_HandleEvent();
}

SYMBOL_IRQ void USART3_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART3_IRQ_INDEX);
  #endif
    HUART2_ISR_HandleEvent();
}

SYMBOL_IRQ void UART4_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART4_IRQ_INDEX);
  #endif
    HUART3_ISR_HandleEvent();
}

SYMBOL_IRQ void UART5_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART5_IRQ_INDEX);
  #endif
    HUART4_ISR_HandleEvent();
}

SYMBOL_IRQ void LPUART1_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(LPMUART1_IRQ_INDEX);
  #endif
    HUART10_ISR_HandleEvent();
}

SYMBOL_IRQ void EXTI0_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI0_IRQ_INDEX);
  #endif
    HINT0_ISR_HandleEvent();
}

SYMBOL_IRQ void EXTI1_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI1_IRQ_INDEX);
  #endif
    HINT1_ISR_HandleEvent();
}

SYMBOL_IRQ void EXTI2_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI2_IRQ_INDEX);
  #endif
    HINT2_ISR_HandleEvent();
}

SYMBOL_IRQ void EXTI3_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI3_IRQ_INDEX);
  #endif
    HINT3_ISR_HandleEvent();
}

SYMBOL_IRQ void EXTI4_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI4_IRQ_INDEX);
  #endif
    HINT4_ISR_HandleEvent();
}

SYMBOL_IRQ void EXTI9_5_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI9_5_IRQ_INDEX);
  #endif
    HINT95_ISR_HandleEvent();
}

SYMBOL_IRQ void EXTI15_10_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI15_10_IRQ_INDEX);
  #endif
    HINT1510_ISR_HandleEvent();
}

SYMBOL_IRQ void ADC1_2_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC1_2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void ADC3_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC3_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void ADC4_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC4_IRQ_INDEX);
  #endif
}
SYMBOL_IRQ void ADC5_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC5_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void TIM1_BRK_TIM15_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM15_IRQ_INDEX);
  #endif
    HTIM15_ISR_HandleEvent();
}

SYMBOL_IRQ void TIM1_UP_TIM16_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM1_IRQ_INDEX);
    MTIM_IRQ_Handler(MTIM16_IRQ_INDEX);
  #endif
    HTIM116_ISR_HandleTimerEvent();
}

SYMBOL_IRQ void TIM1_TRG_COM_TIM17_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM17_IRQ_INDEX);
  #endif
    HTIM17_ISR_HandleEvent();
}

SYMBOL_IRQ void TIM1_CC_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM1_IRQ_INDEX);
  #endif
    HTIM1_ISR_HandleEvent();
}

SYMBOL_IRQ void TIM2_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM2_IRQ_INDEX);
  #endif
    HTIM2_ISR_HandleEvent();
}

SYMBOL_IRQ void TIM3_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM3_IRQ_INDEX);
  #endif
    HTIM3_ISR_HandleEvent();
}

SYMBOL_IRQ void TIM4_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM4_IRQ_INDEX);
  #endif
    HTIM4_ISR_HandleEvent();
}

SYMBOL_IRQ void TIM5_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM5_IRQ_INDEX);
  #endif
    HTIM5_ISR_HandleEvent();
}

SYMBOL_IRQ void TIM8_UP_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM8_IRQ_INDEX);
  #endif
    HTIM8_ISR_HandleTimerEvent();
}

SYMBOL_IRQ void TIM8_CC_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM8_IRQ_INDEX);
  #endif
    HTIM8_ISR_HandleEvent();
}

SYMBOL_IRQ void TIM20_UP_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM20_IRQ_INDEX);
  #endif
    HTIM20_ISR_HandleTimerEvent();
}

SYMBOL_IRQ void TIM20_CC_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM20_IRQ_INDEX);
  #endif
    HTIM20_ISR_HandleEvent();
}

SYMBOL_IRQ void LPTIM1_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIMLP1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C1_EV_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C1_ER_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C2_EV_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C2_ER_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C3_EV_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C3_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C3_ER_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C3_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C4_EV_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C4_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C4_ER_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C4_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void SPI1_IRQHandler(void)
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI1_IRQ_INDEX);
  #endif
    HSPI0_ISR_HandleEvent();
}

SYMBOL_IRQ void SPI2_IRQHandler(void)
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI2_IRQ_INDEX);
  #endif
    HSPI1_ISR_HandleEvent();
}

SYMBOL_IRQ void SPI3_IRQHandler(void)
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI3_IRQ_INDEX);
  #endif
    HSPI2_ISR_HandleEvent();
}

SYMBOL_IRQ void SPI4_IRQHandler(void)
{
  #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI4_IRQ_INDEX);
  #endif
    HSPI3_ISR_HandleEvent();
}

SYMBOL_IRQ void DMA1_Channel1_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA1_Channel2_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA1_Channel3_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA1_Channel4_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA1_Channel5_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA1_Channel6_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA1_Channel7_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA1_Channel8_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA2_Channel1_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA2_Channel2_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA2_Channel3_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA2_Channel4_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA2_Channel5_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA2_Channel6_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA2_Channel7_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void DMA2_Channel8_IRQHandler(void)
{
  #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA2_IRQ_INDEX);
  #endif
}
