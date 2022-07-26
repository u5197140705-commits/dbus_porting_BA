/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PP PED-Framework
*   COMP_ABBREV      TIM
*   AUTHOR           VOLCKO Tomas
*   CREATED          10.10.2018
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Implementation of MKV56F512 specific part of timer library
*
*   \details    Implementation of 16b free running timer with 1us base period
                is based on two 32b down counting PIT channels. CH0 is
                triggered by selected clock (Bus/Flash clock),
                CH1 is triggered by CH0 counted down to 0.
                Load value for CH0 is calculated adequately for generating
                overflow each micro second.
                Load value = CoreClock/OUTDIV4 (in MHz) - 1
                Example: 200(MHz)/8 - 1 = 24 (Load value)
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "timerlib.h"
#include "processor.h"


BOOL _TIM_bGetInitConfig(void)
{
    uint16_t countVal = (uint16_t)((uint16_t)CORE_CLOCK / (uint16_t)(SIM_CLKDIV1_OUTDIV4+(uint16_t)1u));
    countVal--;
    SIM_SCGC6_PIT = ON;          ///<Enable peripheral clock for PIT
    PIT_MCR_MDIS = OFF;          ///<Enable clock for PIT module by turning off disable bit
    PIT_LDVAL0_TSV = countVal;   ///<CH0 load value, 1 micro second prescaling
    PIT_TCTRL1_CHN = ON;         ///<Enable chain mode of Timer CH1
    PIT_LDVAL1_TSV = 0xFFFF;

    PIT_TCTRL1_TEN = ON;        ///<Enable Timer CH1
    PIT_TCTRL0_TEN = ON;        ///<Enable Timer CH0
    return TRUE;
}

uint16 TIM_uiGetCircleMicroSeconds(void)
{
    return (uint16_t) (~PIT_CVAL1_TVL);       ///<exposing CH1 counter value, typecasted as 16b and inverted
}


