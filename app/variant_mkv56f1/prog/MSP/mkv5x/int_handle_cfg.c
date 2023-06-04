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

#ifdef MCAL_INCLUDED
#include "mcal/mcal_includes.h"
#endif

/******************************************************************************/
/* IRQ FUNCTIONS DEFINITIONS                                                  */
/******************************************************************************/

SYMBOL_IRQ void UART0_RX_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART0_IRQ_INDEX);
  #endif
    HUART0_ISR_HandleEvent();
}

SYMBOL_IRQ void UART1_RX_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART1_IRQ_INDEX);
  #endif
    HUART1_ISR_HandleEvent();
}

SYMBOL_IRQ void UART2_RX_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART2_IRQ_INDEX);
  #endif
    HUART2_ISR_HandleEvent();
}

SYMBOL_IRQ void UART3_RX_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART3_IRQ_INDEX);
  #endif
    HUART3_ISR_HandleEvent();
}

SYMBOL_IRQ void UART4_RX_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART4_IRQ_INDEX);
  #endif
    HUART4_ISR_HandleEvent();
}

SYMBOL_IRQ void UART5_RX_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART_IRQ_Handler(MUART5_IRQ_INDEX);
  #endif
    HUART5_ISR_HandleEvent();
}

SYMBOL_IRQ void PORTA_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTIA_IRQ_INDEX);
  #endif
    HINT_ISR_HandlePortA();
}

SYMBOL_IRQ void PORTB_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTIB_IRQ_INDEX);
  #endif
    HINT_ISR_HandlePortB();
}

SYMBOL_IRQ void PORTC_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTIC_IRQ_INDEX);
  #endif
    HINT_ISR_HandlePortC();
}

SYMBOL_IRQ void PORTD_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTID_IRQ_INDEX);
  #endif
    HINT_ISR_HandlePortD();
}

SYMBOL_IRQ void PORTE_IRQHandler(void)
{
  #ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTIE_IRQ_INDEX);
  #endif
    HINT_ISR_HandlePortE();
}

SYMBOL_IRQ void ADC0_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC0_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void HSADC0_CCA_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADCHS0A_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void HSADC0_CCB_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADCHS0B_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void HSADC1_CCA_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADCHS1A_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void HSADC1_CCB_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADCHS1B_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void PIT0_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_PIT0_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void PIT1_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_PIT1_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void PIT2_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_PIT2_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void PIT3_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_PIT3_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void FTM0_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_FTM0_IRQ_INDEX);
  #endif
    HFTM0_ISR_HandleEvent();
}

SYMBOL_IRQ void FTM1_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_FTM1_IRQ_INDEX);
  #endif
    HFTM1_ISR_HandleEvent();
}

SYMBOL_IRQ void FTM2_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_FTM2_IRQ_INDEX);
  #endif
    HFTM2_ISR_HandleEvent();
}

SYMBOL_IRQ void FTM3_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_FTM3_IRQ_INDEX);
  #endif
    HFTM3_ISR_HandleEvent();
}

SYMBOL_IRQ void LPTMR0_IRQHandler(void)
{
  #ifdef MCAL_MTIM_INCLUDED
    MTIM_IRQ_Handler(MTIM_LPTMR0_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C0_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C0_IRQ_INDEX);
  #endif
}

SYMBOL_IRQ void I2C1_IRQHandler(void)
{
  #ifdef MCAL_MI2C_INCLUDED
    MI2C_IRQ_Handler(MI2C1_IRQ_INDEX);
  #endif
}

#ifdef MCAL_MSPI_INCLUDED
SYMBOL_IRQ void HSPI0_ISR_HandleEvent(void)
{
    MSPI_IRQ_Handler(MSPI0_IRQ_INDEX);
}
#endif

#ifdef MCAL_MSPI_INCLUDED
SYMBOL_IRQ void HSPI1_ISR_HandleEvent(void)
{
    MSPI_IRQ_Handler(MSPI1_IRQ_INDEX);
}
#endif

#ifdef MCAL_MSPI_INCLUDED
SYMBOL_IRQ void HSPI2_ISR_HandleEvent(void)
{
    MSPI_IRQ_Handler(MSPI2_IRQ_INDEX);
}
#endif
