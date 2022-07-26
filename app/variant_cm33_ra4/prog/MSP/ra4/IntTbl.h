/*******************************************************************************
 *   Copyright (c) 2022 BSH Hausgeraete GmbH,
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
 *   \brief    external declarations for interrupt service routines.
 *
 *   \details  Definitions of features for HAL.
 *             Include header files here to define compiler defines that are used
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
#include "vector_data.h"


/* Function defined in the HUART module - huart01i.obj */
extern void USART0_RX_IRQHandler(void);
extern void USART0_TX_IRQHandler(void);
extern void USART1_RX_IRQHandler(void);
extern void USART1_TX_IRQHandler(void);
extern void USART2_RX_IRQHandler(void);
extern void USART2_TX_IRQHandler(void);
extern void USART3_TX_IRQHandler(void);
extern void USART3_RX_IRQHandler(void);
extern void USART9_RX_IRQHandler(void);
extern void USART9_TX_IRQHandler(void);


extern void ADC0_IRQHandler(void);

extern void GPT0_CCMPA_IRQHandler(void);
extern void GPT0_CCMPB_IRQHandler(void);
extern void GPT0_CMPC_IRQHandler(void);
extern void GPT0_CMPD_IRQHandler(void);
extern void GPT0_CMPE_IRQHandler(void);
extern void GPT0_CMPF_IRQHandler(void);
extern void GPT0_OVF_IRQHandler(void);
extern void GPT0_UDF_IRQHandler(void);
extern void GPT0_PC_IRQHandler(void);
extern void GPT1_CCMPA_IRQHandler(void);
extern void GPT1_CCMPB_IRQHandler(void);
extern void GPT1_CMPC_IRQHandler(void);
extern void GPT1_CMPD_IRQHandler(void);
extern void GPT1_CMPE_IRQHandler(void);
extern void GPT1_CMPF_IRQHandler(void);
extern void GPT1_OVF_IRQHandler(void);
extern void GPT1_UDF_IRQHandler(void);
extern void GPT1_PC_IRQHandler(void);
extern void GPT2_CCMPA_IRQHandler(void);
extern void GPT2_CCMPB_IRQHandler(void);
extern void GPT2_CMPC_IRQHandler(void);
extern void GPT2_CMPD_IRQHandler(void);
extern void GPT2_CMPE_IRQHandler(void);
extern void GPT2_CMPF_IRQHandler(void);
extern void GPT2_OVF_IRQHandler(void);
extern void GPT2_UDF_IRQHandler(void);
extern void GPT3_CCMPA_IRQHandler(void);
extern void GPT3_CCMPB_IRQHandler(void);
extern void GPT3_CMPC_IRQHandler(void);
extern void GPT3_CMPD_IRQHandler(void);
extern void GPT3_CMPE_IRQHandler(void);
extern void GPT3_CMPF_IRQHandler(void);
extern void GPT3_OVF_IRQHandler(void);
extern void GPT3_UDF_IRQHandler(void);

/* Function defined in the HUART module - huart01.lib (huart01i.obj) */
extern void HUART0_vIsrHandleRxEvent(void);
extern void HUART0_vIsrHandleTxEvent(void);
extern void HUART2_vIsrHandleRxEvent(void);
extern void HUART2_vIsrHandleTxEvent(void);


/* Function defined in the MCAL MUART module - muart_mc.c */
extern void MUART0_IRQ_Handler(void);
extern void MUART1_IRQ_Handler(void);
extern void MUART2_IRQ_Handler(void);
extern void MUART3_IRQ_Handler(void);
extern void MUART4_IRQ_Handler(void);
extern void MUART9_IRQ_Handler(void);


// MCAL EXTI
extern void EXTI00_IRQHandler(void);
extern void EXTI01_IRQHandler(void);
extern void EXTI02_IRQHandler(void);
extern void EXTI03_IRQHandler(void);
extern void EXTI04_IRQHandler(void);
extern void EXTI05_IRQHandler(void);
extern void EXTI06_IRQHandler(void);
extern void EXTI07_IRQHandler(void);
extern void EXTI08_IRQHandler(void);
extern void EXTI09_IRQHandler(void);
extern void EXTI10_IRQHandler(void);
extern void EXTI11_IRQHandler(void);
extern void EXTI12_IRQHandler(void);
extern void EXTI13_IRQHandler(void);
extern void EXTI14_IRQHandler(void);
extern void EXTI15_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif
