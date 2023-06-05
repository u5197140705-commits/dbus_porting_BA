/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / SiLabs EFR32xG21
 *  COMP_ABBREV      HINT
 ******************************************************************************/
 
 
/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file hint01xs.c
*
*   \brief    This module contains the configuration for SiLabs EFR32xG21 HINT.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hint.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
/*lint -e9023 -e9024 -e9026 #/## usage, function like macro */

#define _HINT_ATCONFIG_PORT(port)    HINT_MAP_TO_PORT##port
#define HINT_ATCONFIG_PORT(port)    _HINT_ATCONFIG_PORT(port)
#define HINT_DBUS_MAPPING    HINT_ATCONFIG_PORT(DBUS_IDLE_PORT)

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/
/**
 * Configuration structures for interrupt channels are all set with DBUS pin/port
 * as default (in order to make DBUS idle detection work "out of the box").
 * If any channel has to be used for other purposes than DBUS, user has to change
 * corresponding channel port mapping, as shown in the example below.
 *
 *----------------------------  Example start ----------------------------------
 * How to set configuration structure for external interrupt using pin A3:
 *
 * Pin A3 uses interrupt channel 3, so replace this line
 * \code{.c}
 * const T_HINT_Config HINT3_atConfig[] = {{HINT_DBUS_MAPPING}};
 * \endcode
 * with
 * \code{.c}
 * const T_HINT_Config HINT3_atConfig[] = {{HINT03_ON_PIN_PORTA03}};
 * \endcode
 *----------------------------  Example end ------------------------------------
 *
 * Please refer to hint.h file for example of EXTI call initialization and macros
 * for each pin/port setting.
 */
const T_HINT_Config HINT0_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT1_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT2_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT3_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT4_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT5_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT6_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT7_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT8_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT9_atConfig[]  = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT10_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT11_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT12_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT13_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT14_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT15_atConfig[] = {{HINT_DBUS_MAPPING}};


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
void HINT0_vHandleEvent(void)
{

}
void HINT1_vHandleEvent(void)
{

}
void HINT2_vHandleEvent(void)
{

}
void HINT3_vHandleEvent(void)
{

}
void HINT4_vHandleEvent(void)
{

}
void HINT5_vHandleEvent(void)
{

}
void HINT6_vHandleEvent(void)
{

}
void HINT7_vHandleEvent(void)
{

}
void HINT8_vHandleEvent(void)
{

}
void HINT9_vHandleEvent(void)
{

}
void HINT10_vHandleEvent(void)
{

}
void HINT11_vHandleEvent(void)
{

}
void HINT12_vHandleEvent(void)
{

}
void HINT13_vHandleEvent(void)
{

}
void HINT14_vHandleEvent(void)
{

}
void HINT15_vHandleEvent(void)
{

}
