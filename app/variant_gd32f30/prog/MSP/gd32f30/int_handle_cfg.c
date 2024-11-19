/*******************************************************************************
*   Copyright (c) 2024 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        GD32F303
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     int_handle_cfg.c
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

/************************ UART ************************/
SYMBOL_IRQ void USART0_IRQHandler(void)
{
    #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART0_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void USART1_IRQHandler(void)
{
    #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART1_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void USART2_IRQHandler(void)
{
    #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART2_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void UART3_IRQHandler(void)
{
    #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART3_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void UART4_IRQHandler(void)
{
    #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART4_IRQ_INDEX);
    #endif
}

/************************ I2C ************************/
void I2C0_EV_IRQHandler(void)
{
    #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C0_IRQ_INDEX);
    #endif
}


void I2C0_ER_IRQHandler(void)
{
    #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C0_IRQ_INDEX);
    #endif
}


void I2C1_EV_IRQHandler(void)
{
    #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C1_IRQ_INDEX);
    #endif
}


void I2C1_ER_IRQHandler(void)
{
    #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C1_IRQ_INDEX);
    #endif
}

/************************ TIMER ************************/
SYMBOL_IRQ void TIM0_UP_TIM9_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM0_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM0_CHN_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM0_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM1_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM1_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM2_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM2_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM3_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM3_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM4_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM4_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM5_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM5_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM6_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM6_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM7_UP_TIM12_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM7_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void TIM7_CHN_IRQHandler(void)
{
    #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM7_IRQ_INDEX);
    #endif
}

/************************ SPI ************************/
void SPI0_IRQHandler(void)
{
    #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI0_IRQ_INDEX);
    #endif
}

void SPI1_IRQHandler(void)
{
    #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI1_IRQ_INDEX);
    #endif
}

void SPI2_IRQHandler(void)
{
    #ifdef MCAL_MSPI_INCLUDED
    MSPI_IRQ_Handler(MSPI2_IRQ_INDEX);
    #endif
}

/************************ ADC ************************/
void ADC0_ADC1_IRQHandler(void)
{
    #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC0_1_IRQ_INDEX);
    #endif
}

void ADC2_IRQHandler(void)
{
    #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC2_IRQ_INDEX);
    #endif
}

/************************ EXTI ************************/
void EXTI0_IRQHandler(void)
{
    #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI0_IRQ_INDEX);
    #endif
}

void EXTI1_IRQHandler(void)
{
    #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI1_IRQ_INDEX);
    #endif
}

void EXTI2_IRQHandler(void)
{
    #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI2_IRQ_INDEX);
    #endif
}

void EXTI3_IRQHandler(void)
{
    #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI3_IRQ_INDEX);
    #endif
}

void EXTI4_IRQHandler(void)
{
    #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI4_IRQ_INDEX);
    #endif
}

void EXTI5_9_IRQHandler(void)
{
    #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI5_9_IRQ_INDEX);
    #endif
}

void EXTI10_15_IRQHandler(void)
{
    #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTT10_15_IRQ_INDEX);
    #endif
}


/************************ DMA ************************/
SYMBOL_IRQ void DMA0_CH0_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA0_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA0_CH1_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA0_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA0_CH2_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA0_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA0_CH3_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA0_IRQ_INDEX);
    #endif
}

SYMBOL_IRQ void DMA0_CH4_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA0_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA0_CH5_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA0_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA0_CH6_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA0_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA1_CH0_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA1_CH1_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA1_CH2_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
    #endif
}


SYMBOL_IRQ void DMA1_CH3_4_IRQHandler(void)
{
    #ifdef MCAL_MDMA_INCLUDED
    MDMA_IRQ_Handler(MDMA1_IRQ_INDEX);
    #endif
}
