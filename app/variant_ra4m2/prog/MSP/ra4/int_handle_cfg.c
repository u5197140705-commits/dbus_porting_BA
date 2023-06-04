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
*   PROCESSOR        RA4

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

SYMBOL_IRQ void USART0_RX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART0_IRQ_Handler();
  #else
    HUART0_vIsrHandleRxEvent();
  #endif
}

SYMBOL_IRQ void USART0_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART0_IRQ_Handler();
  #else
    HUART0_vIsrHandleTxEvent();
  #endif
}

SYMBOL_IRQ void USART1_RX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART1_IRQ_Handler();
  #endif
}

SYMBOL_IRQ void USART1_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART1_IRQ_Handler();
  #endif
}

SYMBOL_IRQ void USART2_RX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART2_IRQ_Handler();
  #else
    HUART2_vIsrHandleRxEvent();
  #endif
}

SYMBOL_IRQ void USART2_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART2_IRQ_Handler();
  #else
    HUART2_vIsrHandleTxEvent();
  #endif
}

SYMBOL_IRQ void USART3_RX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART3_IRQ_Handler();
  #endif
}

SYMBOL_IRQ void USART3_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART3_IRQ_Handler();
  #endif
}

SYMBOL_IRQ void USART9_RX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART9_IRQ_Handler();
  #endif
}

SYMBOL_IRQ void USART9_TX_IRQHandler(void)
{
  #ifdef MCAL_MUART_INCLUDED
    MUART9_IRQ_Handler();
  #endif
}

SYMBOL_IRQ void ADC0_IRQHandler(void)
{
  #ifdef MCAL_MADC_INCLUDED
    MADC0_IRQ_Handler();
  #endif
}

