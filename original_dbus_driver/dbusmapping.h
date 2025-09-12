/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
*   PROJECT          IC DBUS
*   COMP_ABBREV      DBM
 ******************************************************************************/
 
 
/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  IC DBUS
 *
 *  \brief    Mapping for D-Bus-2 data link layer
 *
 */

/**
\if dbus2mapping
\file 
     See Mainpage for a description of the mapping layer
\endif
*/

/**
\if dbus2mapping
\mainpage dbus2mapping
\else
\file
     Mapping for D-Bus-2 data link layer.
\endif

\section introdbus2mapping Mapping to UART

    Mapping library for D-Bus-2 data link layer using a dedicated UART (e.g. UART_0).

    For Freescale HC08 the mapping layer needs about 40 byte extra ROM (RAM unchanged) for implementation of the following mapping
    functions (compared to using macros, hence no mapping layer):
    \li DBM_UART_vInit(uint8_t ucIndex)
    \li DBM_UART_vEnable(void)
    \li DBM_UART_vDisableTxEvent(void)
    \li DBM_UART_vEnableTxEvent(void)
    \li DBM_UART_vDisableRxEvent(void)
    \li DBM_UART_vEnableRxEvent(void)
    \li DBM_UART_bIsRxError(void)
    \li DBM_UART_ucGetStatus(void)
    \li DBM_UART_ucGetRxData(void)
    \li DBM_UART_vSetTxData(uint8_t ucData)
    \li DBM_UART_bIsRxOnGoing(void)\n\n
    And in the \b opposite direction, mapping of:
    \li DLL_vHandleRxEvent(void)
    \li DLL_vHandleTxEvent(void)\n
    through HUARTn_vHandleRxEvent() and HUARTn_vHandleTxEvent() (n having the meaning of the dedicated UART channel).

    \image html mapping.gif
*/

#ifndef DBUSMAPPING_H__
#define DBUSMAPPING_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "LibTypes.h"
#include "LibDefines.h"
#include "hsup.h"
#include "dbusdll.h"

#ifdef DBM_MCAL
#include "mcal/mcal_includes.h"
#endif

/******************************************************************************/
/* MACRO DEFINITIONS                                                          */
/******************************************************************************/

#ifdef DBM_MCAL
    #define DBM_UART_NOISE_ERROR                MCAL_ERROR_NOISE
    #define DBM_UART_RX_OVERRUN_ERROR           MCAL_ERROR_OVERFLOW
    #define DBM_UART_FRAMING_ERROR              MCAL_ERROR_FRAMING
    #define DBM_UART_PARITY_ERROR               MCAL_ERROR_PARITY
    #ifdef MUART_EVENT_LIN_BREAK                // some platforms does not provide UART RX Break signal detection
        #define DBM_UART_RX_BREAK               MUART_EVENT_LIN_BREAK
    #endif //MUART_EVENT_LIN_BREAK
#else
/* Axivion Disable Style MisraC2012-5.4: "Macro identifiers shall be distinct." - It works correctly */
    #ifdef  HUART_NOISE_ERROR
        #define DBM_UART_NOISE_ERROR       HUART_NOISE_ERROR //!<Mapping of definition from huart library.
    #else
        #define DBM_UART_NOISE_ERROR 0u
    #endif
    #ifdef  HUART_RX_OVERRUN_ERROR
        #define DBM_UART_RX_OVERRUN_ERROR  HUART_RX_OVERRUN_ERROR //!<Mapping of definition from huart library.
    #else
        #define DBM_UART_RX_OVERRUN_ERROR 0u
    #endif
    #ifdef  HUART_FRAMING_ERROR
        #define DBM_UART_FRAMING_ERROR     HUART_FRAMING_ERROR //!<Mapping of definition from huart library.
    #else
        #define DBM_UART_FRAMING_ERROR 0u
    #endif
    #ifdef  HUART_PARITY_ERROR
        #define DBM_UART_PARITY_ERROR     HUART_PARITY_ERROR //!<Mapping of definition from huart library.
    #else
        #define DBM_UART_PARITY_ERROR 0u
    #endif
    #ifdef  HUART_RX_BREAK
        #define DBM_UART_RX_BREAK          HUART_RX_BREAK //!<Mapping of definition from huart library.
    #else
        #define DBM_UART_RX_BREAK 0u
    #endif
    #ifdef  HUART_RX_IDLE
        #define DBM_UART_RX_IDLE           HUART_RX_IDLE //!<Mapping of definition from huart library.
    #else
    #ifdef  HUART_IDLE
        #define DBM_UART_RX_IDLE           HUART_IDLE //!<Mapping of definition from huart library.
    #else
        #define DBM_UART_RX_IDLE 0u
    #endif
    #endif
    #define FS_ISR_DBUS2_TX 0u       //!< Identification for the D-Bus-2 transmit interrupt used by functional safety.
    #define FS_ISR_DBUS2_RX 1u       //!< Identification for the D-Bus-2 receive interrupt used by functional safety.
/* Axivion Enable Style MisraC2012-5.4 */
#endif //DBM_MCAL

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

void DBM_UART_vInit(uint8_t ucIndex); //!<Mapping from data link layer to UART.
void DBM_UART_vEnable(void); //!<Mapping from data link layer to UART.
void DBM_UART_vDisableTxEvent(void); //!<Mapping from data link layer to UART.
void DBM_UART_vEnableTxEvent(void); //!<Mapping from data link layer to UART.
void DBM_UART_vDisableRxEvent(void); //!<Mapping from data link layer to UART.
void DBM_UART_vEnableRxEvent(void); //!<Mapping from data link layer to UART.
bool DBM_UART_bIsRxError(void); //!<Mapping from data link layer to UART.
uint8_t DBM_UART_ucGetStatus(void); //!<Mapping from data link layer to UART.
uint8_t DBM_UART_ucGetRxData(void); //!<Mapping from data link layer to UART.
void DBM_UART_vSetTxData(uint8_t ucData); //!<Mapping from data link layer to UART.
bool DBM_UART_bIsRxOnGoing(void); //!<Mapping from data link layer to UART.

void DBM_PERIPH_vSetBaudRate(uint8_t ucIndex, uint16_t uiBaudRate); //!<Function needed to set baudrate, both from dbuspresentation_updateXS and from balXS


/* Statistics functions */
uint32_t DBM_UART_ulGetNoiseCounter(void); //!< Interface function for accessing the noise counter.
uint8_t DBM_UART_ucGetUartOverrunCounter(void);//!< Interface function for accessing the overrun counter.
uint32_t DBM_UART_ulGetUartFramingErrorCounter(void);//!< Interface function for accessing the framing error counter.
uint32_t DBM_UART_ulGetUartBreakCounter(void);//!< Interface function for accessing the break counter.

/* The following two functions are called in the interrupt. */
void DBM_UART_vHandleRxEvent(void); //!<Mapping from UART receive interrupt to the corresponding data link layer receive handler.
void DBM_UART_vHandleTxEvent(void); //!<Mapping from UART transmit interrupt to the corresponding data link layer transmit handler.

/* Timer functions - timer(s) can be provided by timerlib, or e.g. OS */
/**
 \brief Function called by data link layer for getting the circulating timer tick.
        This value runs from 0x00 to 0xFFFFFFFF and then starts at 0x00 again.
 \return Circulating 32bit counter value\n
 type       : #uint32_t\n
 range      : 0x00000000...0xFFFFFFFF
*/
uint32_t DBM_ulGetCircleMicroSeconds(void);

#ifndef DBM_EXTERNAL_INTERRUPT_MAPPING
static inline void DBM_DISABLE_INT(void){HSUP_vDisableInt();} //!<Disable interrupts
static inline void DBM_ENABLE_INT(void){HSUP_vEnableInt();} //!<Enable interrupts
#else
extern void DBM_DISABLE_INT(void);//!<Disable interrupts
extern void DBM_ENABLE_INT(void);//!<Enable interrupts
#endif

#if defined(__DBM_INITIAL_MANUAL_TX_INT_CALL_NEEDED)
    static inline void DBM_UART_SET_TX_INTERRUPT(void){DLL_vHandleTxEvent();}
#endif

#ifdef __DBM_USE_IRQ_FOR_IDLE_DETECTION
void DBM_INT_EnableIrq(void);  //!< Enable event for falling edge in idle detection
void DBM_INT_DisableIrq(void); //!< Disable event for falling edge in idle detection
void DBM_INT_ClearIrq(void);   //!< Clear event "falling edge" in idle detection
void DBM_INT_Init(uint8_t ucIndex); //!< Init external pin for idle detection
bool DBM_INT_IsIrqEvent(void); //!< Check wheter event "falling edge" occured in idle detection
void DBM_INT_SetIrqModeFalling(void); //!< Set edge event to either falling or rising
bool DBM_INT_bGetPin(void); //!< Get state of edge event pin in idle detection
#endif
void DBM_GPIO_SetBreakPinState(bool pinState); //!< Set logical state of break pin
void DBM_GPIO_SetBreakPinModeOutput(void); //!< Set break pin mode to digital output
void DBM_GPIO_SetBreakPinModeAlternateFunction(void); //!< Set break pin mode to alternate function

#ifdef __cplusplus
}
#endif

#endif
