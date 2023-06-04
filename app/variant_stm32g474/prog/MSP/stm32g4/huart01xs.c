/*******************************************************************************
 *  Copyright (c) 2018 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB_HAL_STM32G4
 *  COMP_ABBREV      HUART
 *  CREATED          14.05.2018  15:08:55
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    This module contains the configuration for STM32G4 HUART.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "huart.h"
#include "processor.h"
#ifdef DBUS2_INCLUDED
#include "dbusmapping.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
//lint -esym( 750, *_MODE) "message: symbol mot used..."
#define INPUT_MODE                  0x0U
#define OUTPUT_MODE                 0x1U
#define ALTERNATE_FUNCTION_MODE     0x2U
#define ANALOG_MODE                 0x3U

//lint -esym( 750, ALTERNATE_FUNCTION_*) "message: symbol mot used..."
#define ALTERNATE_FUNCTION_0        0x0U
#define ALTERNATE_FUNCTION_1        0x1U
#define ALTERNATE_FUNCTION_2        0x2U
#define ALTERNATE_FUNCTION_3        0x3U
#define ALTERNATE_FUNCTION_4        0x4U
#define ALTERNATE_FUNCTION_5        0x5U
#define ALTERNATE_FUNCTION_6        0x6U
#define ALTERNATE_FUNCTION_7        0x7U
#define ALTERNATE_FUNCTION_8        0x8U
#define ALTERNATE_FUNCTION_9        0x9U
#define ALTERNATE_FUNCTION_10       0xAU
#define ALTERNATE_FUNCTION_11       0xBU
#define ALTERNATE_FUNCTION_12       0xCU
#define ALTERNATE_FUNCTION_13       0xDU
#define ALTERNATE_FUNCTION_14       0xEU
#define ALTERNATE_FUNCTION_15       0xFU


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

/* Configuration structure for HUART peripherals. For more details see huart.h*/
#ifdef BOOTLOADER_VARIANT
    const T_HUART_Config HUART0_atConfig[] =  {{PCLOCK1 /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART1_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART2_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART3_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART4_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART10_atConfig[] = {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};


#else // this set of configuration is used in both product application and Programmer
    const T_HUART_Config HUART0_atConfig[] =  {{PCLOCK1 /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART1_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART2_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART3_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART4_atConfig[] =  {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART10_atConfig[] = {{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
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
#endif

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
#endif

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
#endif

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
#endif

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
#endif

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
#endif


/******************************************************************************/
/* HUARTx Port Remap                                                          */
/******************************************************************************/

/* In case you need to use some UART channel for custom purposes, please define corresponding HUARTx_USED and HUARTx_xx_Pxx switches to activate respective port mapping. */

void HUART0_vPortRemap(void)
{
#ifdef HUART0_USED
#if defined(HUART0_TX_PA9)
   /* USART1_TX - option 1: PA9 */
   RCC_AHB2ENR_GPIOAEN = 1U;
   GPIOA_AFRH_AFRH9    = ALTERNATE_FUNCTION_7;
   GPIOA_MODER_MODER9  = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART0_TX_PB6)
   /* USART1_TX - option 2: PB6 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRL_AFRL6    = ALTERNATE_FUNCTION_7;
   GPIOB_MODER_MODER6  = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART0_TX_PC4)
   /* USART1_TX - option 3: PC4 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRL_AFRL4    = ALTERNATE_FUNCTION_7;
   GPIOC_MODER_MODER4  = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART0_TX_PE0)
   /* USART1_TX - option 4: PE0 */
   RCC_AHB2ENR_GPIOEEN = 1U;
   GPIOE_AFRL_AFRL0    = ALTERNATE_FUNCTION_7;
   GPIOE_MODER_MODER0  = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART0_TX_PG9)
   /* USART1_TX - option 5: PG9 */
   RCC_AHB2ENR_GPIOGEN = 1U;
   GPIOG_AFRH_AFRH9    = ALTERNATE_FUNCTION_7;
   GPIOG_MODER_MODER9  = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART0_RX_PA10)
   /* USART1_RX - option 1: PA10 */
   RCC_AHB2ENR_GPIOAEN = 1U;
   GPIOA_AFRH_AFRH10   = ALTERNATE_FUNCTION_7;
   GPIOA_MODER_MODER10 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART0_RX_PB7)
   /* USART1_RX - option 2: PB7 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRL_AFRL7    = ALTERNATE_FUNCTION_7;
   GPIOB_MODER_MODER7  = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART0_RX_PC5)
   /* USART1_RX - option 3: PC5 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRL_AFRL5    = ALTERNATE_FUNCTION_7;
   GPIOC_MODER_MODER5  = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART0_RX_PE1)
   /* USART1_RX - option 4: PE1 */
   RCC_AHB2ENR_GPIOEEN = 1U;
   GPIOE_AFRL_AFRL1    = ALTERNATE_FUNCTION_7;
   GPIOE_MODER_MODER1  = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}

void HUART1_vPortRemap(void)
{
#ifdef HUART1_USED
#if defined(HUART1_TX_PA2)
   /* USART2_TX - option 1: PA2 */
   RCC_AHB2ENR_GPIOAEN = 1U;
   GPIOA_AFRL_AFRL2 =   ALTERNATE_FUNCTION_7;
   GPIOA_MODER_MODER2 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART1_TX_PA14)
   /* USART2_TX - option 2: PA14 */
   RCC_AHB2ENR_GPIOAEN = 1U;
   GPIOA_AFRH_AFRH14 =   ALTERNATE_FUNCTION_7;
   GPIOA_MODER_MODER14 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART1_TX_PB3)
   /* USART2_TX - option 3: PB3 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRL_AFRL3 =   ALTERNATE_FUNCTION_7;
   GPIOB_MODER_MODER3 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART1_TX_PD5)
   /* USART2_TX - option 4: PD5 */
   RCC_AHB2ENR_GPIODEN = 1U;
   GPIOD_AFRL_AFRL5 =   ALTERNATE_FUNCTION_7;
   GPIOD_MODER_MODER5 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART1_RX_PA3)
   /* USART2_RX - option 1: PA3 */
   RCC_AHB2ENR_GPIOAEN = 1U;
   GPIOA_AFRL_AFRL3 =   ALTERNATE_FUNCTION_7;
   GPIOA_MODER_MODER3 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART1_RX_PA15)
   /* USART2_RX - option 2: PA15 */
   RCC_AHB2ENR_GPIOAEN = 1U;
   GPIOA_AFRH_AFRH15 =   ALTERNATE_FUNCTION_7;
   GPIOA_MODER_MODER15 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART1_RX_PB4)
   /* USART2_RX - option 3: PB4 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRL_AFRL4 =   ALTERNATE_FUNCTION_7;
   GPIOB_MODER_MODER4 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART1_RX_PD6)
   /* USART2_RX - option 4: PD6 */
   RCC_AHB2ENR_GPIODEN = 1U;
   GPIOD_AFRL_AFRL6 =   ALTERNATE_FUNCTION_7;
   GPIOD_MODER_MODER6 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}

void HUART2_vPortRemap(void)
{
#ifdef HUART2_USED
#if defined(HUART2_TX_PB9)
   /* USART3_TX - option 1: PB9 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRH_AFRH9 =   ALTERNATE_FUNCTION_7;
   GPIOB_MODER_MODER9 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART2_TX_PB10)
   /* USART3_TX - option 2: PB10 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRH_AFRH10 =   ALTERNATE_FUNCTION_7;
   GPIOB_MODER_MODER10 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART2_TX_PC10)
   /* USART3_TX - option 3: PC10 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRH_AFRH10 =   ALTERNATE_FUNCTION_7;
   GPIOC_MODER_MODER10 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART2_TX_PD8)
   /* USART3_TX - option 4: PD8 */
   RCC_AHB2ENR_GPIODEN = 1U;
   GPIOD_AFRH_AFRH8 =   ALTERNATE_FUNCTION_7;
   GPIOD_MODER_MODER8 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART2_RX_PB8)
   /* USART3_RX - option 1: PB8 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRH_AFRH8 =   ALTERNATE_FUNCTION_7;
   GPIOB_MODER_MODER8 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART2_RX_PB11)
   /* USART3_RX - option 2: PB11 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRH_AFRH11 =   ALTERNATE_FUNCTION_7;
   GPIOB_MODER_MODER11 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART2_RX_PC11)
   /* USART3_RX - option 3: PC11 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRH_AFRH11 =   ALTERNATE_FUNCTION_7;
   GPIOC_MODER_MODER11 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART2_RX_PD9)
   /* USART3_RX - option 4: PD9 */
   RCC_AHB2ENR_GPIODEN = 1U;
   GPIOD_AFRH_AFRH9 =   ALTERNATE_FUNCTION_7;
   GPIOD_MODER_MODER9 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART2_RX_PE15)
   /* USART3_RX - option 4: PE15 */
   RCC_AHB2ENR_GPIOEEN = 1U;
   GPIOE_AFRH_AFRH15 =   ALTERNATE_FUNCTION_7;
   GPIOE_MODER_MODER15 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}

void HUART3_vPortRemap(void)
{
#ifdef HUART3_USED
#if defined(HUART3_TX_PC10)
   /* UART4_TX - option 1: PC10 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRH_AFRH10 =   ALTERNATE_FUNCTION_5;
   GPIOC_MODER_MODER10 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART3_RX_PC11)
   /* UART4_RX - option 1: PC11 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRH_AFRH11 =   ALTERNATE_FUNCTION_5;
   GPIOC_MODER_MODER11 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}

void HUART4_vPortRemap(void)
{
#ifdef HUART4_USED
#if defined(HUART4_TX_PC12)
   /* UART5_TX - option 1: PC12 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRH_AFRH12 =   ALTERNATE_FUNCTION_5;
   GPIOC_MODER_MODER12 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART4_RX_PD2)
   /* UART5_RX - option 1: PD2 */
   RCC_AHB2ENR_GPIODEN = 1U;
   GPIOD_AFRL_AFRL2 =   ALTERNATE_FUNCTION_5;
   GPIOD_MODER_MODER2 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}

void HUART10_vPortRemap(void)
{
#ifdef HUART10_USED
#if defined(HUART10_TX_PA2)
   /* USART10_TX - option 1: PA2 */
   RCC_AHB2ENR_GPIOAEN = 1U;
   GPIOA_AFRL_AFRL2 =   ALTERNATE_FUNCTION_12;
   GPIOA_MODER_MODER2 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART10_TX_PB11)
   /* USART10_TX - option 2: PB11 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRH_AFRH11 =   ALTERNATE_FUNCTION_8;
   GPIOB_MODER_MODER11 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART10_TX_PC1)
   /* USART10_TX - option 3: PC1 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRL_AFRL1 =   ALTERNATE_FUNCTION_8;
   GPIOC_MODER_MODER1 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART10_TX_PG7)
   /* USART10_TX - option 4: PG7 */
   RCC_AHB2ENR_GPIOGEN = 1U;
   GPIOG_AFRL_AFRL7 =   ALTERNATE_FUNCTION_8;
   GPIOG_MODER_MODER7 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART10_RX_PA3)
   /* USART10_RX - option 1: PA3 */
   RCC_AHB2ENR_GPIOAEN = 1U;
   GPIOA_AFRL_AFRL3 =   ALTERNATE_FUNCTION_12;
   GPIOA_MODER_MODER3 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART10_RX_PB10)
   /* USART10_RX - option 2: PB10 */
   RCC_AHB2ENR_GPIOBEN = 1U;
   GPIOB_AFRH_AFRH10 =   ALTERNATE_FUNCTION_8;
   GPIOB_MODER_MODER10 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART10_RX_PC0)
   /* USART10_RX - option 3: PC0 */
   RCC_AHB2ENR_GPIOCEN = 1U;
   GPIOC_AFRL_AFRL0 =   ALTERNATE_FUNCTION_8;
   GPIOC_MODER_MODER0 = ALTERNATE_FUNCTION_MODE;
#elif defined(HUART10_RX_PG8)
   /* USART10_RX - option 4: PG8 */
   RCC_AHB2ENR_GPIOGEN = 1U;
   GPIOG_AFRH_AFRH8 =   ALTERNATE_FUNCTION_8;
   GPIOG_MODER_MODER8 = ALTERNATE_FUNCTION_MODE;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}
