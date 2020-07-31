/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  AUTHOR           LehmannJe/code generator
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
   Definitions of features for HAL. Include header files here to define compiler defines that are used
   in the IntTbl.c file in framework to override default ISR.
*/

#ifndef INTTBL_H
#define INTTBL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "IntTblArmCM.h"

/* Platform specific IRQs */
extern void WWDG_IRQHandler(void);
extern void PVD_PVM_IRQHandler(void);
extern void RTC_TAMP_STAMP_IRQHandler(void);
extern void RTC_WKUP_IRQHandler(void);
extern void FLASH_IRQHandler(void);
extern void RCC_IRQHandler(void);
extern void EXTI0_IRQHandler(void);
extern void EXTI1_IRQHandler(void);
extern void EXTI2_IRQHandler(void);
extern void EXTI3_IRQHandler(void);
extern void EXTI4_IRQHandler(void);
extern void DMA1_CH1_IRQHandler(void);
extern void DMA1_CH2_IRQHandler(void);
extern void DMA1_CH3_IRQHandler(void);
extern void DMA1_CH4_IRQHandler(void);
extern void DMA1_CH5_IRQHandler(void);
extern void DMA1_CH6_IRQHandler(void);
extern void DMA1_CH7_IRQHandler(void);
extern void ADC1_IRQHandler(void);
extern void CAN1_TX_IRQHandler(void);
extern void CAN1_RX0_IRQHandler(void);
extern void CAN1_RX1_IRQHandler(void);
extern void CAN1_SCE_IRQHandler(void);
extern void EXTI9_5_IRQHandler(void);
extern void TIM1_BRK_TIM15_IRQHandler(void);
extern void TIM1_UP_TIM16_IRQHandler(void);
extern void TIM1_TRG_COM_IRQHandler(void);
extern void TIM1_CC_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
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
extern void DFSDM1_FLT3_IRQHandler(void);
extern void ADC3_IRQHandler(void);
extern void SDMMC1_IRQHandler(void);
extern void SPI3_IRQHandler(void);
extern void UART4_IRQHandler(void);
extern void TIM6_DACUNDER_IRQHandler(void);
extern void TIM7_IRQHandler(void);
extern void DMA2_CH1_IRQHandler(void);
extern void DMA2_CH2_IRQHandler(void);
extern void DMA2_CH3_IRQHandler(void);
extern void DMA2_CH4_IRQHandler(void);
extern void DMA2_CH5_IRQHandler(void);
extern void DFSDM1_IRQHandler(void);
extern void DFSDM2_IRQHandler(void);
extern void DFSDM1_FLT2_IRQHandler(void);
extern void COMP_IRQHandler(void);
extern void LPTIM1_IRQHandler(void);
extern void LPTIM2_IRQHandler(void);
extern void USB_FS_IRQHandler(void);
extern void DMA2_CH6_IRQHandler(void);
extern void DMA2_CH7_IRQHandler(void);
extern void LPUART1_IRQHandler(void);
extern void QUADSPI_IRQHandler(void);
extern void I2C3_EV_IRQHandler(void);
extern void I2C3_ER_IRQHandler(void);
extern void SAI1_IRQHandler(void);
extern void SWPMI1_IRQHandler(void);
extern void TSC_IRQHandler(void);
extern void LCD_IRQHandler(void);
extern void AES_IRQHandler(void);
extern void RNG_IRQHandler(void);
extern void FPU_IRQHandler(void);
extern void CRS_IRQHandler(void);
extern void I2C4_EV_IRQHandler(void);
extern void I2C4_ER_IRQHandler(void);



/* Function defined in the HUART module - huart01.lib (huart01i.obj) */
extern void HUART0_vIsrHandleRxTxEvent(void) ;
extern void HUART1_vIsrHandleRxTxEvent(void) ;
extern void HUART2_vIsrHandleRxTxEvent(void) ;
extern void HUART3_vIsrHandleRxTxEvent(void) ;
extern void HUART4_vIsrHandleRxTxEvent(void) ;
extern void HUART10_vIsrHandleRxTxEvent(void);

/* Function defined in the HINT module - hint01.lib (hint01i.obj) */
extern void HINT0_vIsrExtiIrqHandler(void)   ;
extern void HINT1_vIsrExtiIrqHandler(void)   ;
extern void HINT2_vIsrExtiIrqHandler(void)   ;
extern void HINT3_vIsrExtiIrqHandler(void)   ;
extern void HINT4_vIsrExtiIrqHandler(void)   ;
extern void HINT95_vIsrExtiIrqHandler(void)  ;
extern void HINT1510_vIsrExtiIrqHandler(void);

/* Function defined in the HSPI module - hspi01.lib (hspi01i.obj) */
extern void HSPI0_vIsrHandleTxRxEvent(void)  ;
extern void HSPI1_vIsrHandleTxRxEvent(void)  ;
extern void HSPI2_vIsrHandleTxRxEvent(void)  ;

/* Function defined in the HCCT module - hcct01.lib (hcct01i.obj) */
extern void HTIM1_vIsrHandleEvent(void)      ;
extern void HTIM116_vIsrHandleTimerEvent(void);
extern void HTIM2_vIsrHandleTimerEvent(void) ;
extern void HTIM3_vIsrHandleTimerEvent(void) ;
extern void HTIM4_vIsrHandleTimerEvent(void) ;
extern void HTIM5_vIsrHandleTimerEvent(void) ;
extern void HTIM8_vIsrHandleEvent(void)      ;
extern void HTIM8_vIsrHandleTimerEvent(void) ;
extern void HTIM15_vIsrHandleTimerEvent(void);
extern void HTIM17_vIsrHandleTimerEvent(void);
 

#ifdef __cplusplus
}
#endif

#endif
