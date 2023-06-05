/******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *   All rights reserved. This program and the accompanying materials
 *   are protected by international copyright laws.
 *   Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          Generic SW / SiLabs EFR32xG21
 *   COMP_ABBREV      HCCT
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                               */
/******************************************************************************/
/** \file
*
*   \brief    This module contains the configuration for EFR32xG21 HCCT.
*
*/
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hcct.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
/*lint -esym(750,ALTERNATE_*,*_MODE)  "local macro .. not referenced [MISRA 2012 Rule 2.5, advisory] */


/* VARIABLES ******************************************************************************************/
/* CONSTANTS ******************************************************************************************/

/* Please note, that currently the HCCT definitions/configurations below are merely copies of HCCT0,
 to avoid problems with the linker... If used, please check the configurations! */
const T_HCCT_Config HCCT0_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT1_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT2_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT3_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT4_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT5_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT6_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT7_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};


/* FUNCTIONS ******************************************************************************************/
void HCCT0_vHandleTimerEvent(void)  {}
void HCCT1_vHandleTimerEvent(void)  {}
void HCCT2_vHandleTimerEvent(void)  {}
void HCCT3_vHandleTimerEvent(void)  {}
void HCCT4_vHandleTimerEvent(void)  {}
void HCCT5_vHandleTimerEvent(void)  {}
void HCCT6_vHandleTimerEvent(void)  {}
void HCCT7_vHandleTimerEvent(void)  {}

void HCCT0_vHandleEvent0(void)  {}
void HCCT0_vHandleEvent1(void)  {}
void HCCT1_vHandleEvent0(void)  {}
void HCCT2_vHandleEvent0(void)  {}
void HCCT2_vHandleEvent1(void)  {}
void HCCT3_vHandleEvent0(void)  {}
void HCCT4_vHandleEvent0(void)  {}
void HCCT4_vHandleEvent1(void)  {}
void HCCT5_vHandleEvent0(void)  {}
void HCCT6_vHandleEvent0(void)  {}
void HCCT6_vHandleEvent1(void)  {}
void HCCT7_vHandleEvent0(void)  {}
