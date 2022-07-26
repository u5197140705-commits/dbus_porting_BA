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

/*lint -esym(526,*IRQHandler,*ISR_HandleEvent,*vIsrHandle*) Warning 526: symbol 'xx' is not defined */

/* Platfom specific IRQs - Stm32G030xx */
#if defined (STM32G030)
extern void WWDG_IRQHandler                   (void);
extern void RTC_TAMP_IRQHandler               (void);
extern void FLASH_IRQHandler                  (void);
extern void RCC_IRQHandler                    (void);
extern void EXTI0_1_IRQHandler                (void);
extern void EXTI2_3_IRQHandler                (void);
extern void EXTI4_15_IRQHandler               (void);
extern void DMA_Channel1_IRQHandler           (void);
extern void DMA_Channel2_3_IRQHandler         (void);
extern void DMA_Channel4_5_6_7_IRQHandler     (void);
extern void ADC_COMP_IRQHandler               (void);
extern void TIM1_BRK_UP_TRG_COMP_IRQHandler   (void);
extern void TIM1_CC_IRQHandler                (void);
extern void TIM3_IRQHandler                   (void);
extern void TIM14_IRQHandler                  (void);
extern void TIM16_IRQHandler                  (void);
extern void TIM17_IRQHandler                  (void);
extern void I2C1_IRQHandler                   (void);
extern void I2C2_IRQHandler                   (void);
extern void SPI1_IRQHandler                   (void);
extern void SPI2_IRQHandler                   (void);
extern void USART1_IRQHandler                 (void);
extern void USART2_IRQHandler                 (void);

/* Platfom specific IRQs - Stm32G0B0xx */
#elif defined(STM32G0B0)
extern void WWDG_IRQHandler                   (void);
extern void RTC_STAMP_IRQHandler              (void);
extern void FLASH_IRQHandler                  (void);
extern void RCC_IRQHandler                    (void);
extern void EXTI0_1_IRQHandler                (void);
extern void EXTI2_3_IRQHandler                (void);
extern void EXTI4_15_IRQHandler               (void);
extern void USB_IRQHandler                    (void);
extern void DMA_Channel1_IRQHandler           (void);
extern void DMA_Channel2_3_IRQHandler         (void);
extern void DMA_Channel4_5_6_7_IRQHandler     (void);
extern void ADC_IRQHandler                    (void);
extern void TIM1_BRK_UP_TRG_COMP_IRQHandler   (void);
extern void TIM1_CC_IRQHandler                (void);
extern void TIM3_TIM4_IRQHandler              (void);
extern void TIM6_IRQHandler                   (void);
extern void TIM7_IRQHandler                   (void);
extern void TIM14_IRQHandler                  (void);
extern void TIM15_IRQHandler                  (void);
extern void TIM16_IRQHandler                  (void);
extern void TIM17_IRQHandler                  (void);
extern void I2C1_IRQHandler                   (void);
extern void I2C2_I2C3_IRQHandler              (void);
extern void SPI1_IRQHandler                   (void);
extern void SPI2_SPI3_IRQHandler              (void);
extern void USART1_IRQHandler                 (void);
extern void USART2_IRQHandler                 (void);
extern void USART3_USART4_USART5_USART6_IRQHandler(void);

/* Platfom specific IRQs - Stm32G031xx */
#elif defined(STM32G031)
extern void WWDG_IRQHandler                   (void);
extern void PVD_IRQHandler                    (void);
extern void RTC_TAMP_IRQHandler               (void);
extern void FLASH_IRQHandler                  (void);
extern void RCC_IRQHandler                    (void);
extern void EXTI0_1_IRQHandler                (void);
extern void EXTI2_3_IRQHandler                (void);
extern void EXTI4_15_IRQHandler               (void);
extern void DMA_Channel1_IRQHandler           (void);
extern void DMA_Channel2_3_IRQHandler         (void);
extern void DMA_Channel4_5_6_7_IRQHandler     (void);
extern void ADC_COMP_IRQHandler               (void);
extern void TIM1_BRK_UP_TRG_COMP_IRQHandler   (void);
extern void TIM1_CC_IRQHandler                (void);
extern void TIM2_IRQHandler                   (void);
extern void TIM3_IRQHandler                   (void);
extern void TIM14_IRQHandler                  (void);
extern void TIM16_IRQHandler                  (void);
extern void TIM17_IRQHandler                  (void);
extern void I2C1_IRQHandler                   (void);
extern void I2C2_IRQHandler                   (void);
extern void SPI1_IRQHandler                   (void);
extern void SPI2_IRQHandler                   (void);
extern void USART1_IRQHandler                 (void);
extern void USART2_IRQHandler                 (void);
extern void USART3_USART4_LPUART1_IRQHandler  (void);
extern void CEC_IRQHandler                    (void);
extern void AES_RNG_IRQHandler                (void);

/* Platfom specific IRQs - Stm32G051xx */
#elif defined(STM32G051)
extern void WWDG_IRQHandler(void);
extern void PVD_IRQHandler(void);
extern void RTC_STAMP_IRQHandler(void);
extern void FLASH_IRQHandler(void);
extern void RCC_IRQHandler(void);
extern void EXTI0_1_IRQHandler(void);
extern void EXTI2_3_IRQHandler(void);
extern void EXTI4_15_IRQHandler(void);
extern void DMA_Channel1_IRQHandler(void);
extern void DMA_Channel2_3_IRQHandler(void);
extern void DMA_Channel4_5_6_7_IRQHandler(void);
extern void ADC_COMP_IRQHandler(void);
extern void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
extern void TIM1_CC_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void TIM6_DAC_IRQHandler(void);
extern void TIM7_IRQHandler(void);
extern void TIM14_IRQHandler(void);
extern void TIM15_IRQHandler(void);
extern void TIM16_IRQHandler(void);
extern void TIM17_IRQHandler(void);
extern void I2C1_IRQHandler(void);
extern void I2C2_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void SPI2_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);

/* Platfom specific IRQs - Stm32G071xx */
#elif defined(STM32G071)
extern void WWDG_IRQHandler                   (void);
extern void PVD_IRQHandler                    (void);
extern void RTC_STAMP_IRQHandler              (void);
extern void FLASH_IRQHandler                  (void);
extern void RCC_IRQHandler                    (void);
extern void EXTI0_1_IRQHandler                (void);
extern void EXTI2_3_IRQHandler                (void);
extern void EXTI4_15_IRQHandler               (void);
extern void UCPD1_UCPD2_IRQHandler            (void);
extern void DMA_Channel1_IRQHandler           (void);
extern void DMA_Channel2_3_IRQHandler         (void);
extern void DMA_Channel4_5_6_7_IRQHandler     (void);
extern void ADC_COMP_IRQHandler               (void);
extern void TIM1_BRK_UP_TRG_COMP_IRQHandler   (void);
extern void TIM1_CC_IRQHandler                (void);
extern void TIM2_IRQHandler                   (void);
extern void TIM3_IRQHandler                   (void);
extern void TIM6_DAC_LPTIM1_IRQHandler        (void);
extern void TIM7_LPTIM2_IRQHandler            (void);
extern void TIM14_IRQHandler                  (void);
extern void TIM15_IRQHandler                  (void);
extern void TIM16_IRQHandler                  (void);
extern void TIM17_IRQHandler                  (void);
extern void I2C1_IRQHandler                   (void);
extern void I2C2_IRQHandler                   (void);
extern void SPI1_IRQHandler                   (void);
extern void SPI2_IRQHandler                   (void);
extern void USART1_IRQHandler                 (void);
extern void USART2_IRQHandler                 (void);
extern void USART3_USART4_LPUART1_IRQHandler  (void);
extern void CEC_IRQHandler                    (void);
extern void AES_RNG_IRQHandler                (void);

/* Platfom specific IRQs - Stm32G0B1xx or Stm32G0C1xx*/
#elif defined(STM32G0B1) || defined(STM32G0C1)
extern void WWDG_IRQHandler                   (void);
extern void PVD_IRQHandler                    (void);
extern void RTC_STAMP_IRQHandler              (void);
extern void FLASH_IRQHandler                  (void);
extern void RCC_IRQHandler                    (void);
extern void EXTI0_1_IRQHandler                (void);
extern void EXTI2_3_IRQHandler                (void);
extern void EXTI4_15_IRQHandler               (void);
extern void UCPD1_UCPD2_IRQHandler            (void);
extern void DMA_Channel1_IRQHandler           (void);
extern void DMA_Channel2_3_IRQHandler         (void);
extern void DMA_Channel4_5_6_7_IRQHandler     (void);
extern void ADC_COMP_IRQHandler               (void);
extern void TIM1_BRK_UP_TRG_COMP_IRQHandler   (void);
extern void TIM1_CC_IRQHandler                (void);
extern void TIM2_IRQHandler                   (void);
extern void TIM3_TIM4_IRQHandler              (void);
extern void TIM6_DAC_LPTIM1_IRQHandler        (void);
extern void TIM7_LPTIM2_IRQHandler            (void);
extern void TIM14_IRQHandler                  (void);
extern void TIM15_IRQHandler                  (void);
extern void TIM16_FDCAN_IT0_IRQHandler        (void);
extern void TIM17_FDCAN_IT1_IRQHandler        (void);
extern void I2C1_IRQHandler                   (void);
extern void I2C2_I2C3_IRQHandler              (void);
extern void SPI1_IRQHandler                   (void);
extern void SPI2_SPI3_IRQHandler              (void);
extern void USART1_IRQHandler                 (void);
extern void USART2_LPUART2_IRQHandler         (void);
extern void USART3_USART4_USART5_USART6_LPUART1_IRQHandler(void);
extern void CEC_IRQHandler                    (void);
extern void AES_RNG_IRQHandler                (void);

/* Platfom specific IRQs - Stm32G081xx */
#elif defined(STM32G081)
extern void WWDG_IRQHandler                   (void);
extern void PVD_IRQHandler                    (void);
extern void RTC_IRQHandler                    (void);
extern void FLASH_IRQHandler                  (void);
extern void RCC_IRQHandler                    (void);
extern void EXTI0_1_IRQHandler                (void);
extern void EXTI2_3_IRQHandler                (void);
extern void EXTI4_15_IRQHandler               (void);
extern void USBPD1_USBPD2_IRQHandler          (void);
extern void USBPD_IRQHandler                  (void);
extern void DMA_Channel1_IRQHandler           (void);
extern void DMA_Channel2_3_IRQHandler         (void);
extern void DMA_Channel4_5_6_7_DMAMUX_IRQHandler (void);
extern void DMA_Channel4_5_6_7_IRQHandler     (void);
extern void DMA1_Channel1_IRQHandler          (void);
extern void DMA1_Channel2_3_IRQHandler        (void);
extern void ADC_COMP_IRQHandler               (void);
extern void TIM1_BRK_UP_TRG_COMP_IRQHandler   (void);
extern void TIM1_CC_IRQHandler                (void);
extern void TIM2_IRQHandler                   (void);
extern void TIM3_IRQHandler                   (void);
extern void TIM6_DAC_LPTIM1_IRQHandler        (void);
extern void TIM7_LPTIM2_IRQHandler            (void);
extern void TIM14_IRQHandler                  (void);
extern void TIM15_IRQHandler                  (void);
extern void TIM16_IRQHandler                  (void);
extern void TIM17_IRQHandler                  (void);
extern void I2C1_IRQHandler                   (void);
extern void I2C2_IRQHandler                   (void);
extern void SPI1_IRQHandler                   (void);
extern void SPI2_IRQHandler                   (void);
extern void USART1_IRQHandler                 (void);
extern void USART2_IRQHandler                 (void);
extern void USART3_USART4_LPUART1_IRQHandler  (void);
extern void CEC_IRQHandler                    (void);
extern void AES_RNG_IRQHandler                (void);
#else
#endif


/* Function defined in the HUART module - huart01.lib (huart01i.obj) */
extern void HUART0_ISR_HandleEvent            (void);
extern void HUART1_11_ISR_HandleEvent         (void);
extern void HUART2_3_10_ISR_HandleEvent       (void);
/* Function defined for STM32G0B0 */
extern void HUART2_3_4_5_ISR_HandleEvent      (void);

/* Function defined in the HINT module - hint01.lib (hint01i.obj) */
extern void HINT01_ISR_HandleEvent            (void);
extern void HINT23_ISR_HandleEvent            (void);
extern void HINT415_ISR_HandleEvent           (void);

/* Function defined in the HCCT module - hcct01.lib (hcct01i.obj) */
extern void HTIM1_ISR_HandleEvent             (void);
extern void HTIM1_TimerEvent_ISR_HandleEvent  (void);
extern void HTIM2_ISR_HandleEvent             (void);
extern void HTIM3_ISR_HandleEvent             (void);
extern void HTIM14_ISR_HandleEvent            (void);
extern void HTIM15_ISR_HandleEvent            (void);
extern void HTIM16_ISR_HandleEvent            (void);
extern void HTIM17_ISR_HandleEvent            (void);

/* Function defined in the HSPI module - hspi01.lib (hspi01i.obj) */
extern void HSPI0_vIsrHandleTxRxEvent         (void);
extern void HSPI1_2_vIsrHandleTxRxEvent       (void);


#ifdef __cplusplus
}
#endif

#endif
