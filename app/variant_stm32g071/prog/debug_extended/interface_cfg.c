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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Definitions and declarations of configuration part of the
 *            debug_extended component: Interface layer
 *
 *  \details  See also FNG_DebugExtendedDataUsersManual.md
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdint.h>
#include "debug_extended/user_cfg.h"


/******************************************************************************/
/* PRIVATE PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

#if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED)
    uint8_t DBGI_UartBuffer[DBGI_SIZE_OF_UART_RING_BUFFER];
    uint8_t DBGI_RamBuffer[DBGI_SIZE_OF_RAM_RING_BUFFER];
    uint8_t DBGI_DBusBuffer[DBGI_SIZE_OF_DBUS_RING_BUFFER];
    uint8_t DBGI_TmpDBusBuffer[DBGI_SIZE_OF_TMP_DBUS_BUFFER];

    static uint8_t DBGI_Interface_Cfg[] = DBGI_INTERFACES;
#endif


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE DATA DECLARATIONS                                                  */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBGI_initInterfaces_Cfg( void )
{
#if (defined(DBGI_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGI_BIN_DAT_FUNCTIONS_ENABLED)) && (!defined STS_SIM_TEST_CI)
    for (uint8_t index = 0; index < sizeof(DBGI_Interface_Cfg); index++)
    {
        if(DBGI_Interface_Cfg[index] != DBGI_INTERFACE_NONE)
        {
            switch(index){
                case DBGI_INTERFACE_NONE:
                    break;

                case DBGI_INTERFACE_DBGX_UART:
                    DBGI_SetRingBuffer(&DBGI_UartRingBuffer, DBGI_UartBuffer, DBGI_SIZE_OF_UART_RING_BUFFER);
#if DBGI_UART_RX_TX_PIN_USR != 1
                    DBGI_SetUartConfig(DBGI_UART_RX_TX_PIN, DBGI_UART_BAUDRATE);
#endif
                    break;

                case DBGI_INTERFACE_DBUS:
                    DBGI_SetRingBuffer(&DBGI_DBusRingBuffer, DBGI_DBusBuffer, DBGI_SIZE_OF_DBUS_RING_BUFFER);
                    DBGX_SetTmpDBusBuffer(DBGI_TmpDBusBuffer, DBGI_SIZE_OF_TMP_DBUS_BUFFER);
                    break;

                case DBGI_INTERFACE_RAM:
                    DBGI_SetRingBuffer(&DBGI_RamRingBuffer, DBGI_RamBuffer, DBGI_SIZE_OF_RAM_RING_BUFFER);
                    break;

                case DBGI_INTERFACE_SIMPLE_DEBUG:
                    break;

                default:
                    // No interface selected - no output will be sent
                    break;
            }
        }
    }
#endif

#if defined(DBGX_INPUT_FUNCTIONS_ENABLED) && (!defined STS_SIM_TEST_CI)
    DBGI_SetReceiveInterface(DBGI_PROTOCOL_INPUT_DATA);
    #if (DBGP_PROTOCOL_INPUT_DATA == DBGI_INTERFACE_DBGX_UART) && (DBGI_UART_RX_TX_PIN_USR != 1)
        DBGI_SetUartConfig(DBGI_UART_RX_TX_PIN, DBGI_UART_BAUDRATE);
    #endif
#endif
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

