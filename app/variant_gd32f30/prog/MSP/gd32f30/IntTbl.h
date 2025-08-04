/*******************************************************************************
*   Copyright (c) 2023 BSH Hausgeraete GmbH,
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
/** \file    IntTbl.h
*
*   \brief    external declarations for interrupt service routines.
*
*   \details  Definitions of features for HAL. Include header files here to define compiler defines that are used
*             in the IntTbl.c file in framework to override default ISR.
*
*/


/* AXIVION Disable Style MisraC2012-8.5 : Function declared in more than one file. */
/* AXIVION Disable Style MisraC2012-8.6 : Type, object or function without definition. */

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

/* Platfom specific IRQs - GD32F303 */
#ifdef GD32F30
extern void WWDG_IRQHandler(void);
extern void EXTI_LVD_IRQHandler(void);
extern void TMPR_IRQHandler(void);
extern void RTC_IRQHandler(void);
extern void FMC_IRQHandler(void);
extern void RCU_CTC_IRQHandler(void);
extern void EXTI0_IRQHandler(void);
extern void EXTI1_IRQHandler(void);
extern void EXTI2_IRQHandler(void);
extern void EXTI3_IRQHandler(void);
extern void EXTI4_IRQHandler(void);
extern void DMA0_CH0_IRQHandler(void);
extern void DMA0_CH1_IRQHandler(void);
extern void DMA0_CH2_IRQHandler(void);
extern void DMA0_CH3_IRQHandler(void);
extern void DMA0_CH4_IRQHandler(void);
extern void DMA0_CH5_IRQHandler(void);
extern void DMA0_CH6_IRQHandler(void);
extern void ADC0_ADC1_IRQHandler(void);
extern void USBD_HP_OR_CAN0_TX_IRQHandler(void);
extern void USBD_LP_OR_CAN0_RX0_IRQHandler(void);
extern void CAN0_RX1_IRQHandler(void);
extern void CAN0_EWMC_IRQHandler(void);
extern void EXTI5_9_IRQHandler(void);
extern void TIM0_BRK_TIM8_IRQHandler(void);
extern void TIM0_UP_TIM9_IRQHandler(void);
extern void TIM0_TRG_CMT_TIM10_IRQHandler(void);
extern void TIM0_CHN_IRQHandler(void);
extern void TIM1_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void I2C0_EV_IRQHandler(void);
extern void I2C0_ER_IRQHandler(void);
extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);
extern void SPI0_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void USART0_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void EXTI10_15_IRQHandler(void);
extern void EXTI_RTC_ALM_IRQHandler(void);
extern void EXTI_USBD_WKUP_IRQHandler(void);
extern void TIM7_BRK_TIM11_IRQHandler(void);
extern void TIM7_UP_TIM12_IRQHandler(void);
extern void TIM7_TRG_CMT_TIM13_IRQHandler(void);
extern void TIM7_CHN_IRQHandler(void);
extern void ADC2_IRQHandler(void);
extern void EXMC_IRQHandler(void);
extern void SDIO_IRQHandler(void);
extern void TIM4_IRQHandler(void);
extern void SPI2_IRQHandler(void);
extern void UART3_IRQHandler(void);
extern void UART4_IRQHandler(void);
extern void TIM5_IRQHandler(void);
extern void TIM6_IRQHandler(void);
extern void DMA1_CH0_IRQHandler(void);
extern void DMA1_CH1_IRQHandler(void);
extern void DMA1_CH2_IRQHandler(void);
extern void DMA1_CH3_4_IRQHandler(void);
#endif

#ifdef __cplusplus
}
#endif

#endif //INTTBL_H

/* AXIVION Enable Style MisraC2012-8.5*/
/* AXIVION Enable Style MisraC2012-8.6*/
