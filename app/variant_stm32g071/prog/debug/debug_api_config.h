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
 *  COMP_ABBREV      DBG
 ******************************************************************************/


#ifndef DEBUG_API_CONFIG_H
#define DEBUG_API_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug
 *
 *  \brief    Configuration definitions for unit debug.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#define BSP_HAL     // MIKO TODO, put this in some .mak file.


/** \brief    Debug console type selection
 *
 *  \details  Uncomment one of the following lines to select which interface
 *            to use for debug console input/output. ITM is available only for
 *            ARM Cortex-M3 and above.
 */
//#define DEBUG_INTERFACE_SERIAL  ///< UART serial port
//#define DEBUG_INTERFACE_ITM     ///< ITM (Instrumentation Trace Macrocell)
//#define DEBUG_INTERFACE_STDIO   ///< stdio library (intel)
#define DEBUG_INTERFACE_RTT     ///< SEGGER RTT


/** \brief    Line ending selection
 *
 *  \details  Uncomment to select carriage return and line feed on new line
 */
#define DEBUG_ENDLINE_CR_LF


/** \brief    Echo configuration
 *
 *  \details  Uncomment to enable echo - sending back received characters.
 */
//#define DEBUG_ECHO_ENABLE


/** \brief    Macro for selecting output style
 *
 *  \details  Uncomment to select colored text output. When using colored output,
 *            use serial terminal which recognizes ANSI color escape sequences.
 */
#define DEBUG_CONSOLE_PRETTY


/** \brief    UART Serial interface configuration
 *
 *  \details  UART channel and baudrate configuration in case serial interface is
 *            selected. When using HAL, pin remaps for selected UART are 
 *            configured in huart01xs.c
 */
#define DEBUG_UART_CHANNEL        2
#define DEBUG_UART_BAUTRATE       115200


/** \brief    Retargeting of system calls do debug interface
 *
 *  \details  Definitions for configuring retarget of system calls to debug console.
 *            If STDIO retarget is selected, printf and scanf functions are 
 *            retargeted to debug interface. Also when STDIO retarget is active,
 *            output from assert and other system calls use it for output. When
 *            using asserts, make sure optimization is disabled, otherwise SIGABRT
 *            is called on failed assertion.
 */
// Uncomment this to retarget standard IO functions (printf/scanf) to debug interface.
#define DEBUG_RETARGET_STDIO    ///< uncomment to retarget stdio functions to debug console
#define DEBUG_RETARGET_ASSERT   ///< uncomment to retarget assert failed function to debug console


#ifdef __cplusplus
}
#endif

#endif // DEBUG_API_CONFIG_H
