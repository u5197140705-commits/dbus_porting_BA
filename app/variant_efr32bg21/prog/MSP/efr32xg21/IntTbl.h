/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW / SiLabs EFR32xG21
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    external declarations for interrupt service routines.
*
*   \details  Definitions of features for HAL. Include header files here to define compiler defines that are used
*             in the IntTbl.c file in framework to override default ISR.
*
*/

#ifndef INTTBL_H
#define INTTBL_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "IntTblArmCM.h"
#include "processor.h" /* declarations of ISR */
#include "int_prio.h"

/*lint -esym(526,*_HandleEvent) Warning 526: symbol 'xx' is not defined */

/* uncomment if necessary */
//#define PRIORITY_HIGH    INT_PRIO_0
//#define PRIORITY_MEDIUM  INT_PRIO_7
//#define PRIORITY_LOW     INT_PRIO_13
#if defined(RTOS)
#define PRIORITY_RTOS_SERVICES_LOW     INT_PRIO_14
#define PRIORITY_RTOS_SERVICES_LOWEST  INT_PRIO_15    /* Reserved for RTOS only */
#define RTOS_SVCALL_IRQN_NAME          SVCall_IRQn
#endif

/* Axivion Next Line MisraC2012-8.11: "Array declaration with unknown size" */
extern const ITBL_fptr ITBL_peripheralVectors[];

extern void UART0_ISR_RX_HandleEvent(void);
extern void UART0_ISR_TX_HandleEvent(void);
extern void UART1_ISR_RX_HandleEvent(void);
extern void UART1_ISR_TX_HandleEvent(void);
extern void UART2_ISR_RX_HandleEvent(void);
extern void UART2_ISR_TX_HandleEvent(void);

/* Function defined in the HUART module - huart01.lib (huart01i.obj) */
extern void HUART0_ISR_RX_HandleEvent(void);      /**< USART0_RX IRQ Handler */
extern void HUART0_ISR_TX_HandleEvent(void);      /**< USART0_TX IRQ Handler */
extern void HUART1_ISR_RX_HandleEvent(void);      /**< USART1_RX IRQ Handler */
extern void HUART1_ISR_TX_HandleEvent(void);      /**< USART1_TX IRQ Handler */
extern void HUART2_ISR_RX_HandleEvent(void);      /**< USART2_RX IRQ Handler */
extern void HUART2_ISR_TX_HandleEvent(void);      /**< USART2_TX IRQ Handler */

extern void ITBL_setCustomInterruptPriority(void);

#ifdef __cplusplus
}
#endif

#endif
