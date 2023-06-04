/*******************************************************************************
 *   Copyright (c) 2016 BSH Hausgeraete GmbH,
 *   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *   All rights reserved. This program and the accompanying materials
 *   are protected by international copyright laws.
 *   Please contact copyright holder for licensing information.
 *
 ********************************************************************************
 *   PROJECT          Generic SW
 *   COMP_ABBREV      HADC
 *   PROCESSOR        STM32G4
 *******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This module contains the stubs of HAL ADC functions.
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hadc.h"
#include "hsup.h"

/* DEFINITIONS ****************************************************************************************/
/* VARIABLES ******************************************************************************************/
/* FUNCTIONS ******************************************************************************************/
/* CONSTANTS ******************************************************************************************/

/* Please note, that currently the HADC definitions/configurations below are merely copies of HADC0,
 to avoid problems with the linker... If used, please check the configurations! */
 
const T_HADC_Config HADC01_atConfig[]  = {{HADC_CONFIG_CKMODE_PCLK_DIV_2}};
const T_HADC_Config HADC234_atConfig[]  = {{HADC_CONFIG_CKMODE_PCLK_DIV_2}};


// Internal voltage regulator delay function 
void HADC_vEnableVoltageRegulator(void)
{
    HSUP_vDelay(ADC_DELAY_INTERNAL_REGUL_STAB_US);
}
