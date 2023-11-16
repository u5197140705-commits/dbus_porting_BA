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
 *   COMP_ABBREV      HCCT
 *   PROCESSOR        STM32G0
 *******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This module contains the configuration for STM32G0 HCCT.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hcct.h"
#include "hdio.h"


/* DEFINITIONS ****************************************************************************************/
/* VARIABLES ******************************************************************************************/
/* Note 9003: could define global variable 'xx' within function yy [MISRA 2012 Rule 8.9, advisory] */
/*lint -esym(9003,*ucPortRemapConfig*) */
/* Variable HCCT<unit>_ucPortRemapConfig<channel> holds port remapping configuration needed for HCCTx_bGetPinx */
uint8_t HCCT0_ucPortRemapConfig0;
uint8_t HCCT0_ucPortRemapConfig1;
uint8_t HCCT1_ucPortRemapConfig0;
uint8_t HCCT1_ucPortRemapConfig1;
uint8_t HCCT2_ucPortRemapConfig0;
uint8_t HCCT2_ucPortRemapConfig1;
uint8_t HCCT3_ucPortRemapConfig0;
uint8_t HCCT3_ucPortRemapConfig1;
uint8_t HCCT4_ucPortRemapConfig0;
uint8_t HCCT4_ucPortRemapConfig1;
uint8_t HCCT5_ucPortRemapConfig0;
uint8_t HCCT5_ucPortRemapConfig1;
uint8_t HCCT19_ucPortRemapConfig0;
uint8_t HCCT20_ucPortRemapConfig0;
uint8_t HCCT20_ucPortRemapConfig1;
uint8_t HCCT21_ucPortRemapConfig0;
uint8_t HCCT22_ucPortRemapConfig0;


/* CONSTANTS ******************************************************************************************/

/* Please note, that currently the HCCT definitions/configurations below are merely copies of HCCT0,
 to avoid problems with the linker... If used, please check the configurations! */

const T_HCCT_Config HCCT0_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT1_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT2_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_CAPTURE_INPUT_MODE,  HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT3_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT4_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT5_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT19_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT20_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT21_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT22_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uchar)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};


/* FUNCTIONS ******************************************************************************************/

void HCCT0_vHandleTimerEvent(void)  {}
void HCCT1_vHandleTimerEvent(void)  {}
void HCCT2_vHandleTimerEvent(void)  {}
void HCCT3_vHandleTimerEvent(void)  {}
void HCCT4_vHandleTimerEvent(void)  {}
void HCCT5_vHandleTimerEvent(void)  {}
void HCCT19_vHandleTimerEvent(void) {}
void HCCT20_vHandleTimerEvent(void) {}
void HCCT21_vHandleTimerEvent(void) {}
void HCCT22_vHandleTimerEvent(void) {}

void HCCT0_vHandleEvent0(void)  {}
void HCCT0_vHandleEvent1(void)  {}
void HCCT1_vHandleEvent0(void)  {}
void HCCT1_vHandleEvent1(void)  {}
void HCCT2_vHandleEvent0(void)  {}
void HCCT2_vHandleEvent1(void)  {}
void HCCT3_vHandleEvent0(void)  {}
void HCCT3_vHandleEvent1(void)  {}
void HCCT4_vHandleEvent0(void)  {}
void HCCT4_vHandleEvent1(void)  {}
void HCCT5_vHandleEvent0(void)  {}
void HCCT5_vHandleEvent1(void)  {}
void HCCT19_vHandleEvent0(void) {}
void HCCT20_vHandleEvent0(void) {}
void HCCT20_vHandleEvent1(void) {}
void HCCT21_vHandleEvent0(void) {}
void HCCT22_vHandleEvent0(void) {}


/*---------------------------------------------------------------------------------*/
/* Select your desired port mapping configuration by uncommenting respective lines */

void HCCT0_vPortRemap0(void)
{
   /* HCCT0_CH0 - TIM1_CH1 - option 1: PA8 */
   HDIO_vEnablePortA();                          // Enable clock for port A
   HDIO_vSetModePortA8(HDIO_ALTERNATE_FUNCTION); // Use PA8 as alternate function pin
   HDIO_vSetAlternateFunctionA8(HDIO_ALTERNATE_FUNCTION_2);     // Use alternate function 2
   HCCT0_ucPortRemapConfig0 = 0; // Remember selected port remapping configuration

   /* HCCT0_CH0 - TIM1_CH1 - option 2: PC8 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC8(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT0_ucPortRemapConfig0 = 1;

   /* HCCT0_CH0 - TIM1_CH1 - option 3: PE9 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE9(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT0_ucPortRemapConfig0 = 2;
}

void HCCT0_vPortRemap1(void)
{
   /* HCCT0_CH1 - TIM1_CH2 - option 1: PA9 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA9(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA9(HDIO_ALTERNATE_FUNCTION_2);
   HCCT0_ucPortRemapConfig1 = 0;

   /* HCCT0_CH1 - TIM1_CH2 - option 2: PB3 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB3(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB3(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT0_ucPortRemapConfig1 = 1;

   /* HCCT0_CH1 - TIM1_CH2 - option 3: PC9 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC9(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT0_ucPortRemapConfig1 = 2;

   /* HCCT0_CH1 - TIM1_CH2 - option 4: PE11 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE11(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE11(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT0_ucPortRemapConfig1 = 3;
}

void HCCT1_vPortRemap0(void)
{
   /* HCCT1_CH0 - TIM1_CH3 - option 1: PA10 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA10(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA10(HDIO_ALTERNATE_FUNCTION_2);
   HCCT1_ucPortRemapConfig0 = 0;

   /* HCCT1_CH0 - TIM1_CH3 - option 2: PB6 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB6(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT1_ucPortRemapConfig0 = 1;

   /* HCCT1_CH0 - TIM1_CH3 - option 3: PC10 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC10(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC10(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT1_ucPortRemapConfig0 = 2;

   /* HCCT1_CH0 - TIM1_CH3 - option 4: PE13 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE13(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE13(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT1_ucPortRemapConfig0 = 3;
}

void HCCT1_vPortRemap1(void)
{
   /* HCCT1_CH1 - TIM1_CH4 - option 1: PA11 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA11(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA11(HDIO_ALTERNATE_FUNCTION_2);
   HCCT1_ucPortRemapConfig1 = 0;

   /* HCCT1_CH1 - TIM1_CH4 - option 2: PC11 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC11(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC11(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT1_ucPortRemapConfig1 = 1;

   /* HCCT1_CH1 - TIM1_CH4 - option 3: PE14 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE14(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE14(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT1_ucPortRemapConfig1 = 2;
}

void HCCT2_vPortRemap0(void)
{
   /* HCCT2_CH0 - TIM2_CH1 - option 1: PA0 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA0(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA0(HDIO_ALTERNATE_FUNCTION_2);
   HCCT2_ucPortRemapConfig0 = 0;

   /* HCCT2_CH0 - TIM2_CH1 - option 2: PA5 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA5(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA5(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig0 = 1;

   /* HCCT2_CH0 - TIM2_CH1 - option 3: PA15 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA15(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA15(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig0 = 2;

   /* HCCT2_CH0 - TIM2_CH1 - option 4: PC4 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC4(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC4(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig0 = 3;
}

void HCCT2_vPortRemap1(void)
{
   /* HCCT2_CH1 - TIM2_CH2 - option 1: PA1 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA1(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA1(HDIO_ALTERNATE_FUNCTION_2);
   HCCT2_ucPortRemapConfig1 = 0;

   /* HCCT2_CH1 - TIM2_CH2 - option 2: PB3 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB3(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB3(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig1 = 1;

   /* HCCT2_CH1 - TIM2_CH2 - option 3: PC5 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC5(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC5(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig1 = 2;
}

void HCCT3_vPortRemap0(void)
{
   /* HCCT3_CH0 - TIM2_CH3 - option 1: PA2 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA2(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA2(HDIO_ALTERNATE_FUNCTION_2);
   HCCT3_ucPortRemapConfig0 = 0;

   /* HCCT3_CH0 - TIM2_CH3 - option 2: PB10 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB10(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB10(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT3_ucPortRemapConfig0 = 1;

   /* HCCT3_CH0 - TIM2_CH3 - option 3: PC6 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC6(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT3_ucPortRemapConfig0 = 2;
}

void HCCT3_vPortRemap1(void)
{
   /* HCCT3_CH1 - TIM2_CH4 - option 1: PA3 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA3(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA3(HDIO_ALTERNATE_FUNCTION_2);
   HCCT3_ucPortRemapConfig1 = 0;

   /* HCCT3_CH1 - TIM2_CH4 - option 2: PB11 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB11(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB11(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT3_ucPortRemapConfig1 = 1;

   /* HCCT3_CH1 - TIM2_CH4 - option 3: PC7 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC7(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT3_ucPortRemapConfig1 = 2;
}

void HCCT4_vPortRemap0(void)
{
   /* HCCT4_CH0 - TIM3_CH1 - option 1: PA6 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA6(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA6(HDIO_ALTERNATE_FUNCTION_1);
   HCCT4_ucPortRemapConfig0 = 0;

   /* HCCT4_CH0 - TIM3_CH1 - option 2: PB4 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB4(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB4(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT4_ucPortRemapConfig0 = 1;

   /* HCCT4_CH0 - TIM3_CH1 - option 3: PC6 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC6(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT4_ucPortRemapConfig0 = 2;

   /* HCCT4_CH0 - TIM3_CH1 - option 4: PE3 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE3(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE3(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT4_ucPortRemapConfig0 = 3;
}

void HCCT4_vPortRemap1(void)
{
   /* HCCT4_CH1 - TIM3_CH2 - option 1: PA7 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA7(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA7(HDIO_ALTERNATE_FUNCTION_1);
   HCCT4_ucPortRemapConfig1 = 0;

   /* HCCT4_CH1 - TIM3_CH2 - option 2: PB5 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB5(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB5(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT4_ucPortRemapConfig1 = 1;

   /* HCCT4_CH1 - TIM3_CH2 - option 3: PC7 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC7(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT4_ucPortRemapConfig1 = 2;

   /* HCCT4_CH1 - TIM3_CH2 - option 4: PE4 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE4(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE4(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT4_ucPortRemapConfig1 = 3;
}

void HCCT5_vPortRemap0(void)
{
   /* HCCT5_CH0 - TIM3_CH3 - option 1: PB0 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB0(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB0(HDIO_ALTERNATE_FUNCTION_1);
   HCCT5_ucPortRemapConfig0 = 0;

   /* HCCT5_CH0 - TIM3_CH3 - option 2: PC8 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC8(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT5_ucPortRemapConfig0 = 1;

   /* HCCT5_CH0 - TIM3_CH3 - option 3: PE5 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE5(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE5(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT5_ucPortRemapConfig0 = 2;
}

void HCCT5_vPortRemap1(void)
{
   /* HCCT5_CH1 - TIM3_CH4 - option 1: PB1 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB1(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB1(HDIO_ALTERNATE_FUNCTION_1);
   HCCT5_ucPortRemapConfig1 = 0;

   /* HCCT5_CH1 - TIM3_CH4 - option 2: PC9 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC9(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT5_ucPortRemapConfig1 = 1;

   /* HCCT5_CH1 - TIM3_CH4 - option 3: PE6 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE6(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT5_ucPortRemapConfig1 = 2;
}

void HCCT19_vPortRemap0(void)
{
   /* HCCT19_CH0 - TIM14_CH1 - option 1: PA4 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA4(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA4(HDIO_ALTERNATE_FUNCTION_4);
   HCCT19_ucPortRemapConfig0 = 0;

   /* HCCT19_CH0 - TIM14_CH1 - option 2: PA7 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA7(HDIO_ALTERNATE_FUNCTION_4);
//   HCCT19_ucPortRemapConfig0 = 1;

   /* HCCT19_CH0 - TIM14_CH1 - option 3: PB1 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB1(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB1(HDIO_ALTERNATE_FUNCTION_0);
//   HCCT19_ucPortRemapConfig0 = 2;

   /* HCCT19_CH0 - TIM14_CH1 - option 4: PC12 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC12(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC12(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT19_ucPortRemapConfig0 = 3;

   /* HCCT19_CH0 - TIM14_CH1 - option 5: PF0 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF0(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF0(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT19_ucPortRemapConfig0 = 4;
}

void HCCT20_vPortRemap0(void)
{
   /* HCCT20_CH0 - TIM15_CH1 - option 1: PA2 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA2(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA2(HDIO_ALTERNATE_FUNCTION_5);
   HCCT20_ucPortRemapConfig0 = 0;

   /* HCCT20_CH0 - TIM15_CH1 - option 2: PB14 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB14(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB14(HDIO_ALTERNATE_FUNCTION_5);
//   HCCT20_ucPortRemapConfig0 = 1;

   /* HCCT20_CH0 - TIM15_CH1 - option 3: PC1 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC1(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC1(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT20_ucPortRemapConfig0 = 2;

   /* HCCT20_CH0 - TIM15_CH1 - option 4: PF12 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF12(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF12(HDIO_ALTERNATE_FUNCTION_0);
//   HCCT20_ucPortRemapConfig0 = 3;
}

void HCCT20_vPortRemap1(void)
{
   /* HCCT20_CH1 - TIM15_CH2 - option 1: PA3 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA3(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA3(HDIO_ALTERNATE_FUNCTION_5);
   HCCT20_ucPortRemapConfig1 = 0;

   /* HCCT20_CH1 - TIM15_CH2 - option 2: PB15 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB15(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB15(HDIO_ALTERNATE_FUNCTION_5);
//   HCCT20_ucPortRemapConfig1 = 1;

   /* HCCT20_CH1 - TIM15_CH2 - option 3: PC2 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC2(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC2(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT20_ucPortRemapConfig1 = 2;

   /* HCCT20_CH1 - TIM15_CH2 - option 4: PF13 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF13(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF13(HDIO_ALTERNATE_FUNCTION_0);
//   HCCT20_ucPortRemapConfig1 = 3;
}

void HCCT21_vPortRemap0(void)
{
   /* HCCT21_CH0 - TIM16_CH1 - option 1: PA6 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA6(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA6(HDIO_ALTERNATE_FUNCTION_5);
   HCCT21_ucPortRemapConfig0 = 0;

   /* HCCT21_CH0 - TIM16_CH1 - option 2: PB8 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB8(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT21_ucPortRemapConfig0 = 1;
//
   /* HCCT21_CH0 - TIM16_CH1 - option 3: PD0 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD0(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD0(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT21_ucPortRemapConfig0 = 2;

   /* HCCT21_CH0 - TIM16_CH1 - option 4: PE0 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE0(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE0(HDIO_ALTERNATE_FUNCTION_0);
//   HCCT21_ucPortRemapConfig0 = 3;
}

void HCCT22_vPortRemap0(void)
{
   /* HCCT22_CH0 - TIM17_CH1 - option 1: PA7 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA7(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA7(HDIO_ALTERNATE_FUNCTION_5);
   HCCT22_ucPortRemapConfig0 = 0;

   /* HCCT22_CH0 - TIM17_CH1 - option 2: PB9 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB9(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT22_ucPortRemapConfig0 = 1;
//

   /* HCCT22_CH0 - TIM17_CH1 - option 3: PD1 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD1(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD1(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT22_ucPortRemapConfig0 = 2;

   /* HCCT22_CH0 - TIM17_CH1 - option 4: PE1 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE1(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE1(HDIO_ALTERNATE_FUNCTION_0);
//   HCCT22_ucPortRemapConfig0 = 3;
}

