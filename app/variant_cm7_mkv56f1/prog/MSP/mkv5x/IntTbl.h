/*******************************************************************************
*   Copyright (c) 2018 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        MKV5X
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

/** \file
*
*   \brief    external definitions for interrupt service routines.
*
*   \details  Definitions of features for HAL. Include header files here to define compiler defines that are used
*             in the IntTbl.c file in framework to override default ISR.
*
*/

#ifndef __INTTBLDEF_H
#define __INTTBLDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "IntTblArmCM.h"

/*lint -esym(526,*IRQHandler,*ISR_HandleEvent,HINT_ISR_HandlePort*) Warning 526: symbol 'xx' is not defined */

/* Platform specific IRQs */
extern void DMA0_DMA16_IRQHandler(void);
extern void DMA1_DMA17_IRQHandler(void);
extern void DMA2_DMA18_IRQHandler(void);
extern void DMA3_DMA19_IRQHandler(void);
extern void DMA4_DMA20_IRQHandler(void);
extern void DMA5_DMA21_IRQHandler(void);
extern void DMA6_DMA22_IRQHandler(void);
extern void DMA7_DMA23_IRQHandler(void);
extern void DMA8_DMA24_IRQHandler(void);
extern void DMA9_DMA25_IRQHandler(void);
extern void DMA10_DMA26_IRQHandler(void);
extern void DMA11_DMA27_IRQHandler(void);
extern void DMA12_DMA28_IRQHandler(void);
extern void DMA13_DMA29_IRQHandler(void);
extern void DMA14_DMA30_IRQHandler(void);
extern void DMA15_DMA31_IRQHandler(void);
extern void DMA_Error_IRQHandler(void);
extern void MCM_IRQHandler(void);
extern void FTFE_IRQHandler(void);
extern void Read_Collision_IRQHandler(void);
extern void PMC_IRQHandler(void);
extern void LLWU_IRQHandler(void);
extern void WDOG_EWM_IRQHandler(void);
extern void TRNG0_IRQHandler(void);
extern void I2C0_IRQHandler(void);
extern void I2C1_IRQHandler(void);
extern void SPI0_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void UART5_RX_TX_IRQHandler(void);
extern void UART5_ERR_IRQHandler(void);
extern void UART0_RX_TX_IRQHandler(void);
extern void UART0_ERR_IRQHandler(void);
extern void UART1_RX_TX_IRQHandler(void);
extern void UART1_ERR_IRQHandler(void);
extern void UART2_RX_TX_IRQHandler(void);
extern void UART2_ERR_IRQHandler(void);
extern void ADC0_IRQHandler(void);
extern void HSADC_ERR_IRQHandler(void);
extern void HSADC0_CCA_IRQHandler(void);
extern void CMP0_IRQHandler(void);
extern void CMP1_IRQHandler(void);
extern void FTM0_IRQHandler(void);
extern void FTM1_IRQHandler(void);
extern void UART3_RX_TX_IRQHandler(void);
extern void UART3_ERR_IRQHandler(void);
extern void UART4_RX_TX_IRQHandler(void);
extern void UART4_ERR_IRQHandler(void);
extern void PIT0_IRQHandler(void);
extern void PIT1_IRQHandler(void);
extern void PIT2_IRQHandler(void);
extern void PIT3_IRQHandler(void);
extern void PDB0_IRQHandler(void);
extern void FTM2_IRQHandler(void);
extern void XBARA_IRQHandler(void);
extern void PDB1_IRQHandler(void);
extern void DAC0_IRQHandler(void);
extern void MCG_IRQHandler(void);
extern void LPTMR0_IRQHandler(void);
extern void PORTA_IRQHandler(void);
extern void PORTB_IRQHandler(void);
extern void PORTC_IRQHandler(void);
extern void PORTD_IRQHandler(void);
extern void PORTE_IRQHandler(void);
extern void SPI2_IRQHandler(void);
extern void ENC_COMPARE_IRQHandler(void);
extern void ENC_HOME_IRQHandler(void);
extern void ENC_WDOG_SAB_IRQHandler(void);
extern void ENC_INDEX_IRQHandler(void);
extern void CMP2_IRQHandler(void);
extern void FTM3_IRQHandler(void);
extern void HSADC0_CCB_IRQHandler(void);
extern void HSADC1_CCA_IRQHandler(void);
extern void CAN0_ORed_Message_buffer_IRQHandler(void);
extern void CAN0_Bus_Off_IRQHandler(void);
extern void CAN0_Error_IRQHandler(void);
extern void CAN0_Tx_Warning_IRQHandler(void);
extern void CAN0_Rx_Warning_IRQHandler(void);
extern void CAN0_Wake_Up_IRQHandler(void);
extern void PWM0_CMP0_IRQHandler(void);
extern void PWM0_RELOAD0_IRQHandler(void);
extern void PWM0_CMP1_IRQHandler(void);
extern void PWM0_RELOAD1_IRQHandler(void);
extern void PWM0_CMP2_IRQHandler(void);
extern void PWM0_RELOAD2_IRQHandler(void);
extern void PWM0_CMP3_IRQHandler(void);
extern void PWM0_RELOAD3_IRQHandler(void);
extern void PWM0_CAP_IRQHandler(void);
extern void PWM0_RERR_IRQHandler(void);
extern void PWM0_FAULT_IRQHandler(void);
extern void CMP3_IRQHandler(void);
extern void HSADC1_CCB_IRQHandler(void);
extern void CAN1_ORed_Message_buffer_IRQHandler(void);
extern void CAN1_Bus_Off_IRQHandler(void);
extern void CAN1_Error_IRQHandler(void);
extern void CAN1_Tx_Warning_IRQHandler(void);
extern void CAN1_Rx_Warning_IRQHandler(void);
extern void CAN1_Wake_Up_IRQHandler(void);
extern void PWM1_CMP0_IRQHandler(void);
extern void PWM1_RELOAD0_IRQHandler(void);
extern void PWM1_CMP1_IRQHandler(void);
extern void PWM1_RELOAD1_IRQHandler(void);
extern void PWM1_CMP2_IRQHandler(void);
extern void PWM1_RELOAD2_IRQHandler(void);
extern void PWM1_CMP3_IRQHandler(void);
extern void PWM1_RELOAD3_IRQHandler(void);
extern void PWM1_CAP_IRQHandler(void);
extern void PWM1_RERR_IRQHandler(void);
extern void PWM1_FAULT_IRQHandler(void);



/* Function defined in the HUART module - huart01.lib (huart01i.obj) */
extern void HUART0_ISR_HandleEvent     (void);
extern void HUART1_ISR_HandleEvent     (void);
extern void HUART2_ISR_HandleEvent     (void);
extern void HUART3_ISR_HandleEvent     (void);
extern void HUART4_ISR_HandleEvent     (void);
extern void HUART5_ISR_HandleEvent     (void);

/* Function defined in the HINT module - hint01.lib (hint01i.obj) */
extern void HINT_ISR_HandlePortA       (void);
extern void HINT_ISR_HandlePortB       (void);
extern void HINT_ISR_HandlePortC       (void);
extern void HINT_ISR_HandlePortD       (void);
extern void HINT_ISR_HandlePortE       (void);

/* Function defined in the HCCT module - hcct01.lib (hcct01i.obj) */
extern void HFTM0_ISR_HandleEvent      (void);
extern void HFTM1_ISR_HandleEvent      (void);
extern void HFTM2_ISR_HandleEvent      (void);
extern void HFTM3_ISR_HandleEvent      (void);

/* Function defined in the HSPI module - hspi01.lib (hspi01i.obj) */
extern void HSPI0_ISR_HandleEvent      (void);
extern void HSPI1_ISR_HandleEvent      (void);
extern void HSPI2_ISR_HandleEvent      (void);

#ifdef __cplusplus
}
#endif

#endif
