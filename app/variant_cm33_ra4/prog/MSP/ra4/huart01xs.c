/*******************************************************************************
*   Copyright (c) 2022 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PED-Framework
*   COMP_ABBREV      HUART
*   CREATED          07.01.2022  12:00:00
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This module contains the configuration for RA4 HUART.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "huart.h"
//#include "hdio.h"
#ifdef DBUS2_INCLUDED
#include "dbusmapping.h"
#endif

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                */
/******************************************************************************/

/* Configuration structure for HUART peripherals. For more details see huart.h*/
#ifdef BOOTLOADER_VARIANT 

    #if (BP2_UART_CHANNEL == 0) || (BP2_UART_CHANNEL == 2)
        const T_HUART_Config HUART0_atConfig[]={{ PCLOCKA /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
        const T_HUART_Config HUART2_atConfig[]={{ PCLOCKA /* clock in MHz */, DBUS_DEFAULT_BAUDRATE /* baudrate in 100 bps */, HUART_PARITY_EVEN, HUART_STOP_BITS_1}};
    #else
        #error "Wrong UART selected. UART0 is only possible."
    #endif


#else // this set of configuration is used in both product application and Programmer

    #if (DBUS_UART_CHANNEL == 0) || (DBUS_UART_CHANNEL == 2)
        const T_HUART_Config HUART0_atConfig[]={{ PCLOCKA /* Source clock for SCI [MHz] */, DBUS_DEFAULT_BAUDRATE /* baudrate [*100bps] */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
        const T_HUART_Config HUART2_atConfig[]={{ PCLOCKA /* Source clock for SCI [MHz] */, DBUS_DEFAULT_BAUDRATE /* baudrate [*100bps] */, HUART_PARITY_NONE, HUART_STOP_BITS_1}};
    #else
        #error "Wrong UART selected. UART0 is only possible."
    #endif
#endif


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

/* In case you need to use some UART channel for custom purposes, please define corresponding HUARTx_USED and HUARTx_xx_Pxx switches to activate respective port mapping. */

/******************************************************************************/
/* HUART0 Port Remap                                                          */
/******************************************************************************/

void HUART0_vPortRemap(void)
{
    /* currently, by default, there are P100, P101 used for RX, TX pins */
    ;
}
