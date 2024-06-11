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
 *  COMP_ABBREV      DBGI
 ******************************************************************************/

#ifndef DEBUG_EXTENDED_INTERFACE_CFG_H
#define DEBUG_EXTENDED_INTERFACE_CFG_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of configuration part of
 *            the debug_extended component: Interface layer
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mcal_channels.h"
#include "debug_extended/interface.h"
#include "debug_extended/user_cfg.h"
#include "debug_extended/dbus/dbgx_msg_handlers.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - ACTIVATION SWITCHES                      */
/******************************************************************************/

/* Do not change the activation switches here. Use the global switches from user_cfg.h  */
#ifdef DBGX_USR_TEXT_DAT_FUNCTIONS_ENABLED
    #define DBGI_TEXT_DAT_FUNCTIONS_ENABLED
#endif

#ifdef DBGP_USR_BIN_DAT_FUNCTIONS_ENABLED
    #define DBGI_BIN_DAT_FUNCTIONS_ENABLED
#endif


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - USER CONFIGURATION                       */
/******************************************************************************/

// Interface parameters - UART
#define DBGI_SIZE_OF_UART_RING_BUFFER        1
#define DBGI_UART_BAUDRATE                   19200
#define DBGI_UART_RX_TX_PIN_USR              1 // replace with your PIN according to mcal_channels.h, e.g.: MUART3_RX_PB11_TX_PB10 (STM32G0)

// Interface parameters - RAM
#define DBGI_SIZE_OF_RAM_RING_BUFFER         1

// Interface parameters - DBus
#define DBGI_SIZE_OF_DBUS_RING_BUFFER        1


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - INTERNAL CONFIGURATION                   */
/******************************************************************************/

#if defined(DBGI_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGI_BIN_DAT_FUNCTIONS_ENABLED)
    #define DBGI_INTERFACES            DBGX_USR_PROTOCOLS
#endif

#if defined(DBGX_INPUT_FUNCTIONS_ENABLED)
    #define DBGI_PROTOCOL_INPUT_DATA   DBGP_PROTOCOL_INPUT_DATA
#endif

#if defined(DBGI_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGI_BIN_DAT_FUNCTIONS_ENABLED)

    /* Interface parameters - UART */
    #if (DBGI_SIZE_OF_UART_RING_BUFFER != 1) && (defined DEBUG_INTERFACE_SERIAL)
        #warning Debug and debug extended component both define the UART. See the debug_extended user manual for further information.
    #endif

    /* Default configuration for internal usage */
    #if DBGI_UART_RX_TX_PIN_USR != 1
        #define DBGI_UART_RX_TX_PIN                  &DBGI_UART_RX_TX_PIN_USR
    #elif ((defined STM32G0) || (defined STS_SIM_TEST_CI))
        #define DBGI_UART_RX_TX_PIN                  &MUART3_RX_PB11_TX_PB10
    #elif defined STM32G4
        #define DBGI_UART_RX_TX_PIN                  &MUART3_RX_PB11_TX_PB10
    #elif defined RA4
        #define DBGI_UART_RX_TX_PIN                  &MUART2_RX_P301_TX_P302
    #elif defined MKV5X
        #define DBGI_UART_RX_TX_PIN                  &MUART2_RX_PD2_TX_PD3
    #endif

    /* Interface parameters - DBus */
    #if DBGI_SIZE_OF_DBUS_RING_BUFFER == 1
        #define DBGI_SIZE_OF_TMP_DBUS_BUFFER     (uint8_t)1
    #else
        #if DLL_TRANSMIT_BUFFER_DATA_LENGTH < 98
            #warning Please increase your dbus_transmit_buffer_size to at least 98 bytes. See the debug_extended user manual for further information.
        #endif

        #define DBGI_SIZE_OF_TMP_DBUS_BUFFER     (uint8_t)96
    #endif

#endif // From: if defined(DBGI_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGI_BIN_DAT_FUNCTIONS_ENABLED)


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

#if defined(DBGI_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGI_BIN_DAT_FUNCTIONS_ENABLED)
    extern uint8_t DBGI_UartBuffer[DBGI_SIZE_OF_UART_RING_BUFFER];
    extern uint8_t DBGI_RamBuffer[DBGI_SIZE_OF_RAM_RING_BUFFER];
    extern uint8_t DBGI_DBusBuffer[DBGI_SIZE_OF_DBUS_RING_BUFFER];
    extern uint8_t DBGI_TmpDBusBuffer[DBGI_SIZE_OF_TMP_DBUS_BUFFER];
#endif // From: if defined(DBGI_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGI_BIN_DAT_FUNCTIONS_ENABLED)


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

extern void DBGI_initInterfaces_Cfg( void );


#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_INTERFACE_CFG_H */

