/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW / SiLabs EFR32xG21
*   COMP_ABBREV      HUART
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This module contains the configuration for EFR32xG21 HUART.
*
*/

/* --------------------------HUART setup notice ------------------------------*/
/* Following settings are required for HUART:
 * RX, TX pins and UART channel in common_config.mak
 * port and pins setting for RX interrupt (HINTx_atConfig) in hint01xs.c,
 *    depending on the used RX pin
 * the corresponding DBUS_HINT_CHANNEL for the used UART channel in dbusmappingMc.c
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
#ifdef BOOTLOADER_VARIANT
    #define HUART_PARITY_TYPE HUART_PARITY_EVEN
#else
    #define HUART_PARITY_TYPE HUART_PARITY_NONE
#endif

/* Configuration structure for HUART peripherals. For more details see huart.h*/
const T_HUART_Config HUART0_atConfig[] ={{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_TYPE, HUART_STOP_BITS_1}};
const T_HUART_Config HUART1_atConfig[] ={{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_TYPE, HUART_STOP_BITS_1}};
const T_HUART_Config HUART2_atConfig[] ={{PCLOCK  /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_TYPE, HUART_STOP_BITS_1}};


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

/******************************************************************************/
/* HUART0 Port Remap                                                          */
/******************************************************************************/
void HUART0_vPortRemap(void)
{
    // Configure pin as an output (TX) for DBUS (TX pin)
    HDIO_FUNC_ARG(HUART0_TX_PIN, _vSetPort, HDIO_ON);
    HDIO_vSetModeHUART0_TX(HDIO(HUART0_TX_PIN), HDIO_OUTPUT);
    // Configure pin as an input (RX) for DBUS (RX pin)
    HDIO_FUNC_ARG(HUART0_RX_PIN, _vSetPort, HDIO_OFF);
    HDIO_vSetModeHUART0_RX(HDIO(HUART0_RX_PIN), HDIO_INPUT);
    // Set alternative function UART0 for RX and TX pins
    // and enable routing between UART0 peripheral resource and RX, TX pins
    HDIO_vSetModeHUART0_TX(HDIO(HUART0_TX_PIN), HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetModeHUART0_RX(HDIO(HUART0_RX_PIN), HDIO_ALTERNATE_FUNCTION);
}

/******************************************************************************/
/* HUART1 Port Remap                                                          */
/******************************************************************************/
void HUART1_vPortRemap(void)
{
    // Configure pin as an output (TX) for DBUS (TX pin)
    HDIO_FUNC_ARG(HUART1_TX_PIN, _vSetPort, HDIO_ON);
    HDIO_vSetModeHUART1_TX(HDIO(HUART1_TX_PIN), HDIO_OUTPUT);
    // Configure pin as an input (RX) for DBUS (RX pin)
    HDIO_FUNC_ARG(HUART1_RX_PIN, _vSetPort, HDIO_OFF);
    HDIO_vSetModeHUART1_RX(HDIO(HUART1_RX_PIN), HDIO_INPUT);
    // Set alternative function UART0 for RX and TX pins
    // and enable routing between UART0 peripheral resource and RX, TX pins
    HDIO_vSetModeHUART1_TX(HDIO(HUART1_TX_PIN), HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetModeHUART1_RX(HDIO(HUART1_RX_PIN), HDIO_ALTERNATE_FUNCTION);
}

/******************************************************************************/
/* HUART2 Port Remap                                                          */
/******************************************************************************/
void HUART2_vPortRemap(void)
{
    // Configure pin as an output (TX) for DBUS (TX pin)
    HDIO_FUNC_ARG(HUART2_TX_PIN, _vSetPort, HDIO_ON);
    HDIO_vSetModeHUART2_TX(HDIO(HUART2_TX_PIN), HDIO_OUTPUT);
    // Configure pin as an input (RX) for DBUS (RX pin)
    HDIO_FUNC_ARG(HUART2_RX_PIN, _vSetPort, HDIO_OFF);
    HDIO_vSetModeHUART2_RX(HDIO(HUART2_RX_PIN), HDIO_INPUT);
    // Set alternative function UART0 for RX and TX pins
    // and enable routing between UART0 peripheral resource and RX, TX pins
    HDIO_vSetModeHUART2_TX(HDIO(HUART2_TX_PIN), HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetModeHUART2_RX(HDIO(HUART2_RX_PIN), HDIO_ALTERNATE_FUNCTION);
}

