/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PED-Framework
*   COMP_ABBREV      HUART
*   CREATED          11.08.2017  14:22:53
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This module contains the configuration for STM32G0 HUART.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "huart.h"
#include "hdio.h"
#ifdef DBUS2_INCLUDED
#include "dbusmapping.h"
#endif
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

/* Configuration structure for HUART peripherals. For more details see huart.h*/
#ifdef BOOTLOADER_VARIANT
    const T_HUART_Config HUART0_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART1_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART2_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART3_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART4_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART5_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART10_atConfig[] = {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART11_atConfig[] = {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
#else // this set of configuration is used in both product application and Programmer
    const T_HUART_Config HUART0_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART1_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART2_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART3_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART4_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART5_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART10_atConfig[] = {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART11_atConfig[] = {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#ifndef HUART0_EXTERN_INTERRUPT_HANDLER
void HUART0_vHandleTxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_0
    DBM_UART_vHandleTxEvent();
#endif
}
void HUART0_vHandleRxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_0
    DBM_UART_vHandleRxEvent();
#endif
}
#endif // HUART0_EXTERN_INTERRUPT_HANDLER

#ifndef HUART1_EXTERN_INTERRUPT_HANDLER
void HUART1_vHandleTxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_1
    DBM_UART_vHandleTxEvent();
#endif
}
void HUART1_vHandleRxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_1
    DBM_UART_vHandleRxEvent();
#endif
}
#endif // HUART1_EXTERN_INTERRUPT_HANDLER

#ifndef HUART2_EXTERN_INTERRUPT_HANDLER
void HUART2_vHandleTxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_2
    DBM_UART_vHandleTxEvent();
#endif
}
void HUART2_vHandleRxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_2
    DBM_UART_vHandleRxEvent();
#endif
}
#endif // HUART2_EXTERN_INTERRUPT_HANDLER

#ifndef HUART3_EXTERN_INTERRUPT_HANDLER
void HUART3_vHandleTxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_3
    DBM_UART_vHandleTxEvent();
#endif
}
void HUART3_vHandleRxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_3
    DBM_UART_vHandleRxEvent();
#endif
}
#endif // HUART3_EXTERN_INTERRUPT_HANDLER

#ifndef HUART4_EXTERN_INTERRUPT_HANDLER
void HUART4_vHandleTxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_4
    DBM_UART_vHandleTxEvent();
#endif
}
void HUART4_vHandleRxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_4
    DBM_UART_vHandleRxEvent();
#endif
}
#endif // HUART4_EXTERN_INTERRUPT_HANDLER

#ifndef HUART5_EXTERN_INTERRUPT_HANDLER
void HUART5_vHandleTxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_5
    DBM_UART_vHandleTxEvent();
#endif
}
void HUART5_vHandleRxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_5
    DBM_UART_vHandleRxEvent();
#endif
}
#endif // HUART5_EXTERN_INTERRUPT_HANDLER

#ifndef HUART10_EXTERN_INTERRUPT_HANDLER
void HUART10_vHandleTxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_10
    DBM_UART_vHandleTxEvent();
#endif
}
void HUART10_vHandleRxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_10
    DBM_UART_vHandleRxEvent();
#endif
}
#endif // HUART10_EXTERN_INTERRUPT_HANDLER

#ifndef HUART11_EXTERN_INTERRUPT_HANDLER
void HUART11_vHandleTxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_11
    DBM_UART_vHandleTxEvent();
#endif
}
void HUART11_vHandleRxEvent(void)
{
#ifdef DBUS_UART_CHANNEL_11
    DBM_UART_vHandleRxEvent();
#endif
}
#endif // HUART11_EXTERN_INTERRUPT_HANDLER
/* In case you need to use some UART channel for custom purposes, please define corresponding HUARTx_USED and HUARTx_xx_Pxx switches to activate respective port mapping. */

/******************************************************************************/
/* HUART0 Port Remap                                                          */
/******************************************************************************/

void HUART0_vPortRemap(void)
{
#if defined(HUART0_USED) && !defined(DBM_MCAL)
#if defined(HUART0_TX_PA9)
   /* USART1_TX - option 1: PA9 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA9(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortA9(HDIO_ALTERNATE_FUNCTION);
#ifdef STM32G0_LESS_THAN_32PINS
   SYSCFG_CFGR1_PA11_RMP = 1;
#endif
#elif defined(HUART0_TX_PB6)
   /* USART1_TX - option 2: PB6 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB6(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortB6(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART0_TX_PC4)
   /* USART1_TX - option 3: PC4 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC4(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortC4(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART0_RX_PA10)
   /* USART1_RX - option 1: PA10 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA10(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortA10(HDIO_ALTERNATE_FUNCTION);
#ifdef STM32G0_LESS_THAN_32PINS
   SYSCFG_CFGR1_PA12_RMP = 1;
#endif
#elif defined(HUART0_RX_PB7)
   /* USART1_RX - option 2: PB7 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB7(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortB7(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART0_RX_PC5)
   /* USART1_RX - option 3: PC5 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC5(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortC5(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported RX pin selected!"
#endif
#endif // HUART0_USED
}

void HUART1_vPortRemap(void)
{
#if defined(HUART1_USED) && !defined(DBM_MCAL)
#if defined(HUART1_TX_PA2)
   /* USART2_TX - option 1: PA2 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA2(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortA2(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART1_TX_PA14)
   /* USART2_TX - option 2: PA14 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA14(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortA14(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART1_TX_PD5)
   /* USART2_TX - option 3: PD5 */
   HDIO_vEnablePortD();
   HDIO_vSetAlternateFunctionD5(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortD5(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART1_RX_PA3)
   /* USART2_RX - option 1: PA3 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA3(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortA3(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART1_RX_PA15)
   /* USART2_RX - option 2: PA15 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionA15(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortA15(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART1_RX_PD6)
   /* USART2_RX - option 3: PD6 */
   HDIO_vEnablePortD();
   HDIO_vSetAlternateFunctionD6(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortD6(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported RX pin selected!"
#endif
#endif // HUART1_USED
}

void HUART2_vPortRemap(void)
{
#if defined(HUART2_USED) && !defined(DBM_MCAL)
#if defined(HUART2_TX_PA5)
   /* USART3_TX - option 1: PA5 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA5(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortA5(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_TX_PB2)
   /* USART3_TX - option 2: PB2 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB2(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortB2(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_TX_PB8)
   /* USART3_TX - option 3: PB8 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB8(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortB8(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_TX_PB10)
   /* USART3_TX - option 4: PB10 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB10(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortB10(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_TX_PC4)
   /* USART3_TX - option 5: PC4 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC4(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortC4(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_TX_PC10)
   /* USART3_TX - option 6: PC10 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC10(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortC10(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_TX_PD8)
   /* USART3_TX - option 7: PD8 */
   HDIO_vEnablePortD();
   HDIO_vSetAlternateFunctionD8(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortD8(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART2_RX_PB0)
   /* USART3_RX - option 1: PB0 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB0(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortB0(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_RX_PB9)
   /* USART3_RX - option 2: PB9 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB9(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortB9(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_RX_PB11)
   /* USART3_RX - option 3: PB11 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB11(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortB11(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_RX_PC5)
   /* USART3_RX - option 4: PC5 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC5(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortC5(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_RX_PC11)
   /* USART3_RX - option 5: PC11 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC11(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortC11(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART2_RX_PD9)
   /* USART3_RX - option 6: PD9 */
   HDIO_vEnablePortD();
   HDIO_vSetAlternateFunctionD9(HDIO_ALTERNATE_FUNCTION_0);
   HDIO_vSetModePortD9(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported RX pin selected!"
#endif
#endif // HUART2_USED
}

void HUART3_vPortRemap(void)
{
#if defined(HUART3_USED) && !defined(DBM_MCAL)
#if defined(HUART3_TX_PA0)
   /* USART4_TX - option 1: PA0 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA0(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortA0(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART3_TX_PC10)
   /* USART4_TX - option 2: PC10 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC10(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortC10(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART3_RX_PA1)
   /* USART4_RX - option 1: PA1 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA1(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortA1(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART3_RX_PC11)
   /* USART4_RX - option 2: PC11 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC11(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortC11(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported RX pin selected!"
#endif
#endif // HUART3_USED
}

void HUART4_vPortRemap(void)
{
#ifdef HUART4_USED
#if defined(HUART4_TX_PB3)
   /* USART5_TX - option 1: PB3 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB3(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortB3(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART4_TX_PB0)
   /* USART5_TX - option 2: PB0 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB0(HDIO_ALTERNATE_FUNCTION_8);
   HDIO_vSetModePortB0(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART4_TX_PC12)
   /* USART5_TX - option 3: PC12 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC12(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortC12(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART4_TX_PD3)
   /* USART5_TX - option 4: PD3 */
   HDIO_vEnablePortD();
   HDIO_vSetAlternateFunctionD3(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortD3(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART4_TX_PE10)
   /* USART5_TX - option 5: PE10 */
   HDIO_vEnablePortE();
   HDIO_vSetAlternateFunctionE10(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortE10(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART4_RX_PB4)
   /* USART5_RX - option 1: PB4 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB4(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortB4(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART4_RX_PB1)
   /* USART5_RX - option 2: PB1 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB1(HDIO_ALTERNATE_FUNCTION_8);
   HDIO_vSetModePortB1(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART4_RX_PD2)
   /* USART5_RX - option 3: PD2 */
   HDIO_vEnablePortD();
   HDIO_vSetAlternateFunctionD2(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortD2(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART4_RX_PE11)
   /* USART5_RX - option 4: PE11 */
   HDIO_vEnablePortE();
   HDIO_vSetAlternateFunctionE11(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortE11(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported RX pin selected!"
#endif
#endif // HUART4_USED
}

void HUART5_vPortRemap(void)
{
#ifdef HUART5_USED
#if defined(HUART5_TX_PA4)
   /* USART6_TX - option 1: PA4 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA4(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortA4(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART5_TX_PB8)
   /* USART6_TX - option 2: PB8 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB8(HDIO_ALTERNATE_FUNCTION_8);
   HDIO_vSetModePortB8(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART5_TX_PC0)
   /* USART6_TX - option 3: PC0 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC0(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortC0(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART5_TX_PF9)
   /* USART6_TX - option 4: PF9 */
   HDIO_vEnablePortF();
   HDIO_vSetAlternateFunctionF9(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortF9(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART5_RX_PA5)
   /* USART6_RX - option 1: PA5 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA5(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortA5(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART5_RX_PB9)
   /* USART6_RX - option 2: PB9 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB9(HDIO_ALTERNATE_FUNCTION_8);
   HDIO_vSetModePortB9(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART5_RX_PC1)
   /* USART6_RX - option 3: PC1 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC1(HDIO_ALTERNATE_FUNCTION_4);
   HDIO_vSetModePortC1(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART5_RX_PF10)
   /* USART6_RX - option 4: PF10 */
   HDIO_vEnablePortF();
   HDIO_vSetAlternateFunctionF10(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortF10(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported RX pin selected!"
#endif
#endif // HUART5_USED
}

void HUART10_vPortRemap(void)
{
#ifdef HUART10_USED
#if defined(HUART10_TX_PA2)
   /* LPUART1_TX - option 1: PA2 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA2(HDIO_ALTERNATE_FUNCTION_6);
   HDIO_vSetModePortA2(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART10_TX_PB11)
   /* LPUART1_TX - option 2: PB11 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB11(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortB11(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART10_TX_PC1)
   /* LPUART1_TX - option 3: PC1 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC1(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortC1(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART10_TX_PF4)
   /* LPUART1_TX - option 4: PF4 */
   HDIO_vEnablePortF();
   HDIO_vSetAlternateFunctionF4(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortF4(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART10_RX_PA3)
   /* LPUART1_RX - option 1: PA3 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA3(HDIO_ALTERNATE_FUNCTION_6);
   HDIO_vSetModePortA3(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART10_RX_PB10)
   /* LPUART1_RX - option 2: PB10 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB10(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortB10(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART10_RX_PC0)
   /* LPUART1_RX - option 3: PC0 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC0(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortC0(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART10_RX_PF5)
   /* LPUART1_RX - option 4: PF5 */
   HDIO_vEnablePortF();
   HDIO_vSetAlternateFunctionF5(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortF5(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported RX pin selected!"
#endif
#endif // HUART10_USED
}

void HUART11_vPortRemap(void)
{
#ifdef HUART11_USED
#if defined(HUART11_TX_PA14)
   /* LPUART2_TX - option 1: PA14 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA14(HDIO_ALTERNATE_FUNCTION_10);
   HDIO_vSetModePortA14(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART11_TX_PB6)
   /* LPUART2_TX - option 2: PB6 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB6(HDIO_ALTERNATE_FUNCTION_10);
   HDIO_vSetModePortB6(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART11_TX_PC0)
   /* LPUART2_TX - option 3: PC0 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC0(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortC0(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART11_TX_PC6)
   /* LPUART2_TX - option 4: PC6 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC6(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortC6(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART11_TX_PF2)
   /* LPUART2_TX - option 5: PF2 */
   HDIO_vEnablePortF();
   HDIO_vSetAlternateFunctionF2(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortF2(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART11_RX_PA13)
   /* LPUART2_RX - option 1: PA13 */
   HDIO_vEnablePortA();
   HDIO_vSetAlternateFunctionA13(HDIO_ALTERNATE_FUNCTION_10);
   HDIO_vSetModePortA13(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART11_RX_PB7)
   /* LPUART2_RX - option 2: PB7 */
   HDIO_vEnablePortB();
   HDIO_vSetAlternateFunctionB7(HDIO_ALTERNATE_FUNCTION_10);
   HDIO_vSetModePortB7(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART11_RX_PC1)
   /* LPUART2_RX - option 3: PC1 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC1(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortC1(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART11_RX_PC7)
   /* LPUART2_RX - option 4: PC7 */
   HDIO_vEnablePortC();
   HDIO_vSetAlternateFunctionC7(HDIO_ALTERNATE_FUNCTION_3);
   HDIO_vSetModePortC7(HDIO_ALTERNATE_FUNCTION);
#elif defined(HUART11_RX_PF3)
   /* LPUART2_RX - option 5: PF3 */
   HDIO_vEnablePortF();
   HDIO_vSetAlternateFunctionF3(HDIO_ALTERNATE_FUNCTION_1);
   HDIO_vSetModePortF3(HDIO_ALTERNATE_FUNCTION);
#else
    #error "Unsupported RX pin selected!"
#endif
#endif // HUART11_USED
}

