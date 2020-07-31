/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          STM32L4 HAL PED Framework
 *  COMP_ABBREV      HADC
 *  AUTHOR           Pusztai Pavol
 *  CREATED          13.04.2017
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief  This module contains the stubs of HAL ADC functions.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hadc.h"



/* Please note, that currently the HADC definitions/configurations below are merely copies of HADC0,
 to avoid problems with the linker... If used, please check the configurations! */
 
const T_HADC_Config HADC0_atConfig[]   = {{ HADC_CONFIG_CKMODE_PCLK_DIV_1 }};
const T_HADC_Config HADC1_atConfig[]   = {{ HADC_CONFIG_CKMODE_PCLK_DIV_1 }};
const T_HADC_Config HADC2_atConfig[]   = {{ HADC_CONFIG_CKMODE_PCLK_DIV_1 }};
