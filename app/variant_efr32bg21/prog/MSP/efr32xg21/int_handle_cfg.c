/*******************************************************************************
*   Copyright (c) 2020 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        EFR32BG21
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

#if defined(MCAL_INCLUDED) 
#include "mcal/mcal_includes.h"
#endif


/******************************************************************************/
/* IRQ FUNCTIONS DEFINITIONS                                                  */
/******************************************************************************/

SYMBOL_IRQ void UART0_ISR_TX_HandleEvent(void)
{
    HUART0_ISR_TX_HandleEvent();
    // MUART and MSPI
    if(USART0->CTRL & USART_CTRL_SYNC)
    {
      #if defined(MCAL_MSPI_INCLUDED)
        MSPI_IRQ_Handler(MSPI0_IRQ_INDEX);     // Synchronous SPI mode
      #endif
    }
    else
    {
      #if defined(MCAL_MUART_INCLUDED)
        MUART_IRQ_Handler(MUART0_IRQ_INDEX);   // Asynchronous USART mode
      #endif
    }
}

SYMBOL_IRQ void UART0_ISR_RX_HandleEvent(void)
{
    HUART0_ISR_RX_HandleEvent();
    // MUART and MSPI
    if(USART0->CTRL & USART_CTRL_SYNC)
    {
      #if defined(MCAL_MSPI_INCLUDED)
        MSPI_IRQ_Handler(MSPI0_IRQ_INDEX);     // Synchronous SPI mode
      #endif
    }
    else
    {
      #if defined(MCAL_MUART_INCLUDED)
        MUART_IRQ_Handler(MUART0_IRQ_INDEX);   // Asynchronous USART mode
      #endif
    }
}

SYMBOL_IRQ void UART1_ISR_TX_HandleEvent(void)
{
    HUART1_ISR_TX_HandleEvent();
    // MUART and MSPI
    if(USART1->CTRL & USART_CTRL_SYNC)
    {
      #if defined(MCAL_MSPI_INCLUDED)
        MSPI_IRQ_Handler(MSPI1_IRQ_INDEX);     // Synchronous SPI mode
      #endif
    }
    else
    {
      #if defined(MCAL_MUART_INCLUDED)
        MUART_IRQ_Handler(MUART1_IRQ_INDEX);   // Asynchronous USART mode
      #endif
    }
}

SYMBOL_IRQ void UART1_ISR_RX_HandleEvent(void)
{
    HUART1_ISR_RX_HandleEvent();
    // MUART and MSPI
    if(USART1->CTRL & USART_CTRL_SYNC)
    {
      #if defined(MCAL_MSPI_INCLUDED)
        MSPI_IRQ_Handler(MSPI1_IRQ_INDEX);     // Synchronous SPI mode
      #endif
    }
    else
    {
      #if defined(MCAL_MUART_INCLUDED)
        MUART_IRQ_Handler(MUART1_IRQ_INDEX);   // Asynchronous USART mode
      #endif
    }
}

SYMBOL_IRQ void UART2_ISR_TX_HandleEvent(void)
{
    HUART2_ISR_TX_HandleEvent();
    // MUART and MSPI
    if(USART2->CTRL & USART_CTRL_SYNC)
    {
      #if defined(MCAL_MSPI_INCLUDED)
        MSPI_IRQ_Handler(MSPI2_IRQ_INDEX);     // Synchronous SPI mode
      #endif
    }
    else
    {
      #if defined(MCAL_MUART_INCLUDED)
        MUART_IRQ_Handler(MUART2_IRQ_INDEX);   // Asynchronous USART mode
      #endif
    }
}

SYMBOL_IRQ void UART2_ISR_RX_HandleEvent(void)
{
    HUART2_ISR_RX_HandleEvent();
    // MUART and MSPI
    if(USART2->CTRL & USART_CTRL_SYNC)
    {
      #if defined(MCAL_MSPI_INCLUDED)
        MSPI_IRQ_Handler(MSPI2_IRQ_INDEX);     // Synchronous SPI mode
      #endif
    }
    else
    {
      #if defined(MCAL_MUART_INCLUDED)
        MUART_IRQ_Handler(MUART2_IRQ_INDEX);   // Asynchronous USART mode
      #endif
    }
}

SYMBOL_IRQ void GPIO_ODD_IRQHandler(void)
{
#ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI_ODD_IRQ_INDEX);
#endif
}

SYMBOL_IRQ void GPIO_EVEN_IRQHandler(void)
{
#ifdef MCAL_MEXTI_INCLUDED
    MEXTI_IRQ_Handler(MEXTI_EVEN_IRQ_INDEX);
#endif
}

SYMBOL_IRQ void IADC_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC_IRQ_Handler(MADC1_IRQ_INDEX);
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
