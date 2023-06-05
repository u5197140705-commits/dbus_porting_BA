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

/*lint -esym(526,*IRQHandler,*_ISR_Handle*) Warning 526: symbol 'xx' is not defined */

/* Platform specific IRQs */
extern void WWDG_IRQHandler                   (void);
extern void PVD_PVM_IRQHandler                (void);
extern void RTC_TAMP_LSECSS_IRQHandler        (void);
extern void RTC_WKUP_IRQHandler               (void);
extern void FLASH_IRQHandler                  (void);
extern void RCC_IRQHandler                    (void);
extern void EXTI0_IRQHandler                  (void);
extern void EXTI1_IRQHandler                  (void);
extern void EXTI2_IRQHandler                  (void);
extern void EXTI3_IRQHandler                  (void);
extern void EXTI4_IRQHandler                  (void);
extern void DMA1_Channel1_IRQHandler          (void);
extern void DMA1_Channel2_IRQHandler          (void);
extern void DMA1_Channel3_IRQHandler          (void);
extern void DMA1_Channel4_IRQHandler          (void);
extern void DMA1_Channel5_IRQHandler          (void);
extern void DMA1_Channel6_IRQHandler          (void);
extern void DMA1_Channel7_IRQHandler          (void);
extern void ADC1_2_IRQHandler                 (void);
extern void USB_HP_IRQHandler                 (void);
extern void USB_LP_IRQHandler                 (void);
extern void FDCAN1_IT0_IRQHandler             (void);
extern void FDCAN1_IT1_IRQHandler             (void);
extern void EXTI9_5_IRQHandler                (void);
extern void TIM1_BRK_TIM15_IRQHandler         (void);
extern void TIM1_UP_TIM16_IRQHandler          (void);
extern void TIM1_TRG_COM_TIM17_IRQHandler     (void);
extern void TIM1_CC_IRQHandler                (void);
extern void TIM2_IRQHandler                   (void);
extern void TIM3_IRQHandler                   (void);
extern void TIM4_IRQHandler                   (void);
extern void I2C1_EV_IRQHandler                (void);
extern void I2C1_ER_IRQHandler                (void);
extern void I2C2_EV_IRQHandler                (void);
extern void I2C2_ER_IRQHandler                (void);
extern void SPI1_IRQHandler                   (void);
extern void SPI2_IRQHandler                   (void);
extern void USART1_IRQHandler                 (void);
extern void USART2_IRQHandler                 (void);
extern void USART3_IRQHandler                 (void);
extern void EXTI15_10_IRQHandler              (void);
extern void RTC_Alarm_IRQHandler              (void);
extern void USBWakeUp_IRQHandler              (void);
extern void TIM8_BRK_IRQHandler               (void);
extern void TIM8_UP_IRQHandler                (void);
extern void TIM8_TRG_COM_IRQHandler           (void);
extern void TIM8_CC_IRQHandler                (void);
extern void ADC3_IRQHandler                   (void);
extern void FMC_IRQHandler                    (void);
extern void LPTIM1_IRQHandler                 (void);
extern void TIM5_IRQHandler                   (void);
extern void SPI3_IRQHandler                   (void);
extern void UART4_IRQHandler                  (void);
extern void UART5_IRQHandler                  (void);
extern void TIM6_DAC_IRQHandler               (void);
extern void TIM7_DAC_IRQHandler               (void);
extern void DMA2_Channel1_IRQHandler          (void);
extern void DMA2_Channel2_IRQHandler          (void);
extern void DMA2_Channel3_IRQHandler          (void);
extern void DMA2_Channel4_IRQHandler          (void);
extern void DMA2_Channel5_IRQHandler          (void);
extern void ADC4_IRQHandler                   (void);
extern void ADC5_IRQHandler                   (void);
extern void UCPD1_IRQHandler                  (void);
extern void COMP1_2_3_IRQHandler              (void);
extern void COMP4_5_6_IRQHandler              (void);
extern void COMP7_IRQHandler                  (void);
extern void HRTIM1_Master_IRQHandler          (void);
extern void HRTIM1_TIMA_IRQHandler            (void);
extern void HRTIM1_TIMB_IRQHandler            (void);
extern void HRTIM1_TIMC_IRQHandler            (void);
extern void HRTIM1_TIMD_IRQHandler            (void);
extern void HRTIM1_TIME_IRQHandler            (void);
extern void HRTIM1_FLT_IRQHandler             (void);
extern void HRTIM1_TIMF_IRQHandler            (void);
extern void CRS_IRQHandler                    (void);
extern void SAI1_IRQHandler                   (void);
extern void TIM20_BRK_IRQHandler              (void);
extern void TIM20_UP_IRQHandler               (void);
extern void TIM20_TRG_COM_IRQHandler          (void);
extern void TIM20_CC_IRQHandler               (void);
extern void FPU_IRQHandler                    (void);
extern void I2C4_EV_IRQHandler                (void);
extern void I2C4_ER_IRQHandler                (void);
extern void SPI4_IRQHandler                   (void);
extern void AES_IRQHandler                    (void);
extern void FDCAN2_IT0_IRQHandler             (void);
extern void FDCAN2_IT1_IRQHandler             (void);
extern void FDCAN3_IT0_IRQHandler             (void);
extern void FDCAN3_IT1_IRQHandler             (void);
extern void RNG_IRQHandler                    (void);
extern void LPUART1_IRQHandler                (void);
extern void I2C3_EV_IRQHandler                (void);
extern void I2C3_ER_IRQHandler                (void);
extern void DMAMUX_OVR_IRQHandler             (void);
extern void QUADSPI_IRQHandler                (void);
extern void DMA1_Channel8_IRQHandler          (void);
extern void DMA2_Channel6_IRQHandler          (void);
extern void DMA2_Channel7_IRQHandler          (void);
extern void DMA2_Channel8_IRQHandler          (void);
extern void CORDIC_IRQHandler                 (void);
extern void FMAC_IRQHandler                   (void);

/* Function defined in the HUART module - huart01.lib (huart01i.obj) */
extern void HUART0_ISR_HandleEvent            (void);
extern void HUART1_ISR_HandleEvent            (void);
extern void HUART2_ISR_HandleEvent            (void);
extern void HUART3_ISR_HandleEvent            (void);
extern void HUART4_ISR_HandleEvent            (void);
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
extern void HTIM1_ISR_HandleEvent             (void);
extern void HTIM116_ISR_HandleTimerEvent      (void);
extern void HTIM2_ISR_HandleEvent             (void);
extern void HTIM3_ISR_HandleEvent             (void);
extern void HTIM4_ISR_HandleEvent             (void);
extern void HTIM5_ISR_HandleEvent             (void);
extern void HTIM8_ISR_HandleEvent             (void);
extern void HTIM8_ISR_HandleTimerEvent        (void);
extern void HTIM15_ISR_HandleEvent            (void);
extern void HTIM17_ISR_HandleEvent            (void);
extern void HTIM20_ISR_HandleEvent            (void);
extern void HTIM20_ISR_HandleTimerEvent       (void);

/* Function defined in the HSPI module - hspi01.lib (hspi01i.obj) */
extern void HSPI0_ISR_HandleEvent             (void);
extern void HSPI1_ISR_HandleEvent             (void);
extern void HSPI2_ISR_HandleEvent             (void);
extern void HSPI3_ISR_HandleEvent             (void);

#ifdef __cplusplus
}
#endif

#endif
