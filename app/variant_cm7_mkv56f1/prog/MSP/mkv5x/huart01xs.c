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
 *  COMP_ABBREV      HUART
 *  PROCESSOR        MKV5X
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    This module contains the configuration for NXP MKV5X HUART.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "huart.h"
#ifdef DBUS2_INCLUDED
#include "dbusmapping.h"
#endif
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

/* Configuration structure for HUART peripherals. For more details see huart.h*/
#ifdef BOOTLOADER_VARIANT
    const T_HUART_Config HUART0_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART1_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART2_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART3_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART4_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART5_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};


#else // this set of configuration is used in both product application and Programmer
    const T_HUART_Config HUART0_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART1_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART2_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART3_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART4_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    const T_HUART_Config HUART5_atConfig[] ={{PCLOCK /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
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
#endif


/******************************************************************************/
/* HUART0 Port Remap                                                          */
/******************************************************************************/

/* In case you need to use some UART channel for custom purposes, please define corresponding HUARTx_USED and HUARTx_xx_Pxx switches to activate respective port mapping. */

/*lint -save -e923 "cast from unsigned int to pointer"*/
void HUART0_vPortRemap()
{
#ifdef HUART0_USED
#if defined(HUART0_TX_PA2)
    /* UART0_TX - option 1: PA2 */
    SIM_SCGC5_PORTA = ON;
    PORTA_PCR2_MUX = 2U;
#elif defined(HUART0_TX_PA14)
    /* UART0_TX - option 2: PA14 */
    SIM_SCGC5_PORTA = ON;
    PORTA_PCR14_MUX = 3U;
#elif defined(HUART0_TX_PB1)
    /* UART0_TX - option 3: PB1 */
    SIM_SCGC5_PORTB = ON;
    PORTB_PCR1_MUX = 7U;
#elif defined(HUART0_TX_PB17)
    /* UART0_TX - option 4: PB17 */
    SIM_SCGC5_PORTB = ON;
    PORTB_PCR17_MUX = 3U;
#elif defined(HUART0_TX_PC7)
    /* UART0_TX - option 5: PC7 */
    SIM_SCGC5_PORTC = ON;
    PORTC_PCR7_MUX = 5U;
#elif defined(HUART0_TX_PD7)
    /* UART0_TX - option 6: PD7 */
    SIM_SCGC5_PORTD = ON;
    PORTD_PCR7_MUX = 3U;
#elif defined(HUART0_TX_PE20)
    /* UART0_TX - option 7: PE20 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR20_MUX = 4U;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART0_RX_PA1)
    /* UART0_RX - option 1: PA1 */
    SIM_SCGC5_PORTA = ON;
    PORTA_PCR1_MUX = 2U;
#elif defined(HUART0_RX_PA15)
    /* UART0_RX - option 2: PA15 */
    SIM_SCGC5_PORTA = ON;
    PORTA_PCR15_MUX = 3U;
#elif defined(HUART0_RX_PB0)
    /* UART0_RX - option 3: PB0 problem with GPIO*/
    SIM_SCGC5_PORTB = ON;
    PORTB_PCR0_MUX = 7U;
#elif defined(HUART0_RX_PB16)
    /* UART0_RX - option 4: PB16 */
    SIM_SCGC5_PORTB = ON;
    PORTB_PCR16_MUX = 3U;
#elif defined(HUART0_RX_PC6)
    /* UART0_RX - option 5: PC6 */
    SIM_SCGC5_PORTC = ON;
    PORTC_PCR6_MUX = 5U;
#elif defined(HUART0_RX_PD6)
    /* UART0_RX - option 6: PD6 */
    SIM_SCGC5_PORTD = ON;
    PORTD_PCR6_MUX = 3U;
#elif defined(HUART0_RX_PE21)
    /* UART0_RX - option 7: PE21 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR21_MUX = 4U;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}


/******************************************************************************/
/* HUART1 Port Remap                                                          */
/******************************************************************************/
void HUART1_vPortRemap()
{
#ifdef HUART1_USED
#if defined(HUART1_TX_PC4)
    /* UART1_TX - option 1: PC4 */
    SIM_SCGC5_PORTC = ON;
    PORTC_PCR4_MUX = 3U;
#elif defined(HUART1_TX_PE0)
    /* UART1_TX - option 2: PE0 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR0_MUX = 3U;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART1_RX_PC3)
    /* UART1_RX - option 1: PC3 */
    SIM_SCGC5_PORTC = ON;
    PORTC_PCR3_MUX = 3U;
#elif defined(HUART1_RX_PE1)
    /* UART1_RX - option 2: PE1 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR1_MUX = 3U;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}


/******************************************************************************/
/* HUART2 Port Remap                                                          */
/******************************************************************************/
void HUART2_vPortRemap()
{
#ifdef HUART2_USED
#if defined(HUART2_TX_PD3)
    /* UART2_TX - option 1: PD3 */
    SIM_SCGC5_PORTD = ON;
    PORTD_PCR3_MUX = 3U;
#elif defined(HUART2_TX_PE16)
    /* UART2_TX - option 2: PE16 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR16_MUX = 3U;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART2_RX_PD2)
    /* UART2_RX - option 1: PD2 */
    SIM_SCGC5_PORTD = ON;
    PORTD_PCR2_MUX = 3U;
#elif defined(HUART2_RX_PE17)
    /* UART2_RX - option 2: PE17 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR17_MUX = 3U;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}


/******************************************************************************/
/* HUART3 Port Remap                                                          */
/******************************************************************************/
void HUART3_vPortRemap()
{
#ifdef HUART3_USED
#if defined(HUART3_TX_PB11)
    /* UART3_TX - option 1: PB11 */
    SIM_SCGC5_PORTB = ON;
    PORTB_PCR11_MUX = 3U;
#elif defined(HUART3_TX_PC17)
    /* UART3_TX - option 2: PC17 */
    SIM_SCGC5_PORTC = ON;
    PORTC_PCR17_MUX = 3U;
#elif defined(HUART3_TX_PE4)
    /* UART3_TX - option 3: PE4 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR4_MUX = 3U;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART3_RX_PB10)
    /* UART3_RX - option 1: PB10 */
    SIM_SCGC5_PORTB = ON;
    PORTB_PCR10_MUX = 3U;
#elif defined(HUART3_RX_PC16)
    /* UART3_RX - option 2: PC16 */
    SIM_SCGC5_PORTC = ON;
    PORTC_PCR16_MUX = 3U;
#elif defined(HUART3_RX_PE5)
    /* UART3_RX - option 3: PE5 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR5_MUX = 3U;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}


/******************************************************************************/
/* HUART4 Port Remap                                                          */
/******************************************************************************/
void HUART4_vPortRemap()
{
#ifdef HUART4_USED
#if defined(HUART4_TX_PC15)
    /* UART4_TX - option 1: PC15 */
    SIM_SCGC5_PORTC = ON;
    PORTC_PCR15_MUX = 9U;
#elif defined(HUART4_TX_PE24)
    /* UART4_TX - option 2: PE24 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR24_MUX = 8U;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART4_RX_PC14)
    /* UART4_RX - option 1: PC14 */
    SIM_SCGC5_PORTC = ON;
    PORTC_PCR14_MUX = 9U;
#elif defined(HUART4_RX_PE25)
    /* UART4_RX - option 2: PE25 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR25_MUX = 8U;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}


/******************************************************************************/
/* HUART5 Port Remap                                                          */
/******************************************************************************/
void HUART5_vPortRemap()
{
#ifdef HUART5_USED
#if defined(HUART5_TX_PD9)
    /* UART5_TX - option 1: PD9 */
    SIM_SCGC5_PORTD = ON;
    PORTD_PCR9_MUX = 3U;
#elif defined(HUART5_TX_PE8)
    /* UART5_TX - option 2: PE8 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR8_MUX = 3U;
#else
    #error "Unsupported TX pin selected!"
#endif

#if defined(HUART5_RX_PD8)
    /* UART5_RX - option 1: PD8 */
    SIM_SCGC5_PORTD = ON;
    PORTD_PCR8_MUX = 3U;
#elif defined(HUART5_RX_PE9)
    /* UART5_RX - option 2: PE9 */
    SIM_SCGC5_PORTE = ON;
    PORTE_PCR9_MUX = 3U;
#else
    #error "Unsupported RX pin selected!"
#endif
#endif
}
/*lint -restore -e923 */
