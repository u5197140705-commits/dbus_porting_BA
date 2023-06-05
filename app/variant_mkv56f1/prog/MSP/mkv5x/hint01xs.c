/*******************************************************************************
*   Copyright (C) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   COMP_ABBREV      HINT
*   PROCESSOR        MKV5X
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file hint01xs.c
 *
 *  \brief Configuration file for HINT
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "hint.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifndef DBUS_IDLE_PORT
    #define DBUS_IDLE_PORT    A  // default definition
#endif

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
const T_HINT_Config HINT16_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT17_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT18_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT19_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT20_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT21_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT22_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT23_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT24_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT25_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT26_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT27_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT28_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT29_atConfig[] = {{HINT_DBUS_MAPPING}};
const T_HINT_Config HINT30_atConfig[] = {{HINT_DBUS_MAPPING}};

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/* Handlers that are called when interrupt from HINT occurs */

void HINT0_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT1_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT2_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT3_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT4_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT5_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT6_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT7_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT8_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT9_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT10_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT11_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT12_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT13_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT14_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT15_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT16_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT17_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT18_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT19_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT20_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT21_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT22_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT23_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT24_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT25_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT26_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT27_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT28_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT29_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
void HINT30_vHandleEvent(void)
{
    /* Here project specific functionality has to be added */
}
