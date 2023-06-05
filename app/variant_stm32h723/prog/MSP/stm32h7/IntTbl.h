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
*   PROCESSOR        STM32H7
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
#include "int_prio.h"

/*lint -esym(526,*IRQHandler,*_ISR_Handle*) Warning 526: symbol 'xx' is not defined */

/* uncomment if necessary */
//#define PRIORITY_HIGH    INT_PRIO_0
//#define PRIORITY_MEDIUM  INT_PRIO_7
//#define PRIORITY_LOW     INT_PRIO_13
#if defined(RTOS)
#define PRIORITY_RTOS_SERVICES_LOW     INT_PRIO_14
#define PRIORITY_RTOS_SERVICES_LOWEST  INT_PRIO_15    /* Reserved for RTOS only */
#define RTOS_SVCALL_IRQN_NAME          SVCall_IRQn
#endif

/* Platform specific IRQs */
extern void WWDG1_IRQHandler(void);
extern void PVD_PVM_IRQHandler(void);
extern void RTC_TAMP_STAMP_CSS_LSE_IRQHandler(void);
extern void RTC_WKUP_IRQHandler(void);
extern void FLASH_IRQHandler(void);
extern void RCC_IRQHandler(void);
extern void EXTI0_IRQHandler(void);
extern void EXTI1_IRQHandler(void);
extern void EXTI2_IRQHandler(void);
extern void EXTI3_IRQHandler(void);
extern void EXTI4_IRQHandler(void);
extern void DMA_STR0_IRQHandler(void);
extern void DMA_STR1_IRQHandler(void);
extern void DMA_STR2_IRQHandler(void);
extern void DMA_STR3_IRQHandler(void);
extern void DMA_STR4_IRQHandler(void);
extern void DMA_STR5_IRQHandler(void);
extern void DMA_STR6_IRQHandler(void);
extern void ADC1_2_IRQHandler(void);
extern void FDCAN1_IT0_IRQHandler(void);
extern void FDCAN2_IT0_IRQHandler(void);
extern void FDCAN1_IT1_IRQHandler(void);
extern void FDCAN2_IT1_IRQHandler(void);
extern void EXTI9_5_IRQHandler(void);
extern void TIM1_BRK_IRQHandler(void);
extern void TIM1_UP_IRQHandler(void);
extern void TIM1_TRG_COM_IRQHandler(void);
extern void TIM1_CC_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void TIM4_IRQHandler(void);
extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);
extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void SPI2_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void EXTI15_10_IRQHandler(void);
extern void RTC_ALARM_IRQHandler(void);
extern void TIM8_BRK_TIM12_IRQHandler(void);
extern void TIM8_UP_TIM13_IRQHandler(void);
extern void TIM8_TRG_COM_TIM14_IRQHandler(void);
extern void TIM8_CC_IRQHandler(void);
extern void DMA1_STR7_IRQHandler(void);
extern void FMC_IRQHandler(void);
extern void SDMMC1_IRQHandler(void);
extern void TIM5_IRQHandler(void);
extern void SPI3_IRQHandler(void);
extern void UART4_IRQHandler(void);
extern void UART5_IRQHandler(void);
extern void TIM6_DAC_IRQHandler(void);
extern void TIM7_IRQHandler(void);
extern void DMA2_STR0_IRQHandler(void);
extern void DMA2_STR1_IRQHandler(void);
extern void DMA2_STR2_IRQHandler(void);
extern void DMA2_STR3_IRQHandler(void);
extern void DMA2_STR4_IRQHandler(void);
extern void ETH_IRQHandler(void);
extern void ETH_WKUP_IRQHandler(void);
extern void FDCAN_CAL_IRQHandler(void);
extern void DMA2_STR5_IRQHandler(void);
extern void DMA2_STR6_IRQHandler(void);
extern void DMA2_STR7_IRQHandler(void);
extern void USART6_IRQHandler(void);
extern void I2C3_EV_IRQHandler(void);
extern void I2C3_ER_IRQHandler(void);
extern void DCMI_IRQHandler(void);
extern void FPU_IRQHandler(void);
extern void UART7_IRQHandler(void);
extern void UART8_IRQHandler(void);
extern void SPI4_IRQHandler(void);
extern void SPI5_IRQHandler(void);
extern void SPI6_IRQHandler(void);
extern void SAI1_IRQHandler(void);
extern void LTDC_IRQHandler(void);
extern void LTDC_ER_IRQHandler(void);
extern void DMA2D_IRQHandler(void);
extern void OCTOSPI1_IRQHandler(void);
extern void LPTIM1_IRQHandler(void);
extern void CEC_IRQHandler(void);
extern void I2C4_EV_IRQHandler(void);
extern void I2C4_ER_IRQHandler(void);
extern void SPDIF_IRQHandler(void);
extern void DMAMUX1_OV_IRQHandler(void);
extern void DFSDM1_FLT0_IRQHandler(void);
extern void DFSDM1_FLT1_IRQHandler(void);
extern void DFSDM1_FLT2_IRQHandler(void);
extern void DFSDM1_FLT3_IRQHandler(void);
extern void SWPMI1_IRQHandler(void);
extern void TIM15_IRQHandler(void);
extern void TIM16_IRQHandler(void);
extern void TIM17_IRQHandler(void);
extern void MDIOS_WKUP_IRQHandler(void);
extern void MDIOS_IRQHandler(void);
extern void MDMA_IRQHandler(void);
extern void SDMMC2_IRQHandler(void);
extern void HSEM0_IRQHandler(void);
extern void ADC3_IRQHandler(void);
extern void DMAMUX2_OVR_IRQHandler(void);
extern void COMP_IRQHandler(void);
extern void LPTIM2_IRQHandler(void);
extern void LPTIM3_IRQHandler(void);
extern void LPTIM4_IRQHandler(void);
extern void LPTIM5_IRQHandler(void);
extern void LPUART_IRQHandler(void);
extern void CRS_IRQHandler(void);
extern void SAI4_IRQHandler(void);
extern void WKUP_IRQHandler(void);
extern void OCTOSPI2_IRQHandler(void);
extern void FMAC_IRQHandler(void);
extern void CORDIC_IT_IRQHandler(void);
extern void USART10_IRQHandler(void);
extern void I2C5_EV_IRQHandler(void);
extern void I2C5_ER_IRQHandler(void);
extern void FDCAN3_IT0_IRQHandler(void);
extern void FDCAN3_IT1_IRQHandler(void);
extern void TIM23_IRQHandler(void);
extern void TIM24_IRQHandler(void);



/* Function defined in the HUART module - huart01.lib (huart01i.obj) */
extern void HUART0_ISR_HandleEvent            (void);
extern void HUART1_ISR_HandleEvent            (void);
extern void HUART2_ISR_HandleEvent            (void);
extern void HUART3_ISR_HandleEvent            (void);
extern void HUART4_ISR_HandleEvent            (void);
extern void HUART5_ISR_HandleEvent            (void);
extern void HUART6_ISR_HandleEvent            (void);
extern void HUART7_ISR_HandleEvent            (void);
extern void HUART8_ISR_HandleEvent            (void);
extern void HUART9_ISR_HandleEvent            (void);
extern void HUART10_ISR_HandleEvent           (void);

/* Function defined in the HINT module - hint01.lib (hint01i.obj) */
extern void HINT0_ISR_HandleEvent             (void);
extern void HINT1_ISR_HandleEvent             (void);
extern void HINT2_ISR_HandleEvent             (void);
extern void HINT3_ISR_HandleEvent             (void);
extern void HINT4_ISR_HandleEvent             (void);
extern void HINT95_ISR_HandleEvent            (void);
extern void HINT1510_ISR_HandleEvent          (void);

/* Function defined in the HCCT module - hcct01.lib (hcct01i.obj) */
extern void HTIM1_ISR_HandleTimerEvent        (void);
extern void HTIM1_ISR_HandleEvent             (void);
extern void HTIM2_ISR_HandleEvent             (void);
extern void HTIM3_ISR_HandleEvent             (void);
extern void HTIM4_ISR_HandleEvent             (void);
extern void HTIM5_ISR_HandleEvent             (void);
extern void HTIM6_ISR_HandleEvent             (void);
extern void HTIM7_ISR_HandleEvent             (void);
extern void HTIM8_ISR_HandleTimerEvent        (void);
extern void HTIM8_ISR_HandleEvent             (void);
extern void HTIM12_ISR_HandleTimerEvent       (void);
extern void HTIM13_ISR_HandleTimerEvent       (void);
extern void HTIM14_ISR_HandleTimerEvent       (void);
extern void HTIM15_ISR_HandleEvent            (void);
extern void HTIM16_ISR_HandleEvent            (void);
extern void HTIM17_ISR_HandleEvent            (void);
extern void HTIM23_ISR_HandleEvent            (void);
extern void HTIM24_ISR_HandleEvent            (void);

/* Function defined in the HSPI module - hspi01.lib (hspi01i.obj) */
extern void HSPI0_ISR_HandleEvent             (void);
extern void HSPI1_ISR_HandleEvent             (void);
extern void HSPI2_ISR_HandleEvent             (void);
extern void HSPI3_ISR_HandleEvent             (void);
extern void HSPI4_ISR_HandleEvent             (void);
extern void HSPI5_ISR_HandleEvent             (void);
extern void HSPI6_ISR_HandleEvent             (void);

extern void ITBL_setCustomInterruptPriority   (void);

#ifdef __cplusplus
}
#endif

#endif
