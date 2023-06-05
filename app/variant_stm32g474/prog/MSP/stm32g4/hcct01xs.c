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
 *   PROCESSOR        STM32G4
 *******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This module contains the configuration for STM32G4 HCCT.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hcct.h"
#include "hdio.h"


/* VARIABLES ******************************************************************************************/

/*lint -esym(552,*ucPortRemapConfig*) Warning 552: external variable 'xx' not accessed */
/*lint -esym(9003,*ucPortRemapConfig*) could define global variable 'xx' within function xx [MISRA 2012 Rule 8.9, advisory] */

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
uint8_t HCCT6_ucPortRemapConfig0;
uint8_t HCCT6_ucPortRemapConfig1;
uint8_t HCCT7_ucPortRemapConfig0;
uint8_t HCCT7_ucPortRemapConfig1;
uint8_t HCCT8_ucPortRemapConfig0;
uint8_t HCCT8_ucPortRemapConfig1;
uint8_t HCCT9_ucPortRemapConfig0;
uint8_t HCCT9_ucPortRemapConfig1;
uint8_t HCCT10_ucPortRemapConfig0;
uint8_t HCCT10_ucPortRemapConfig1;
uint8_t HCCT11_ucPortRemapConfig0;
uint8_t HCCT11_ucPortRemapConfig1;
uint8_t HCCT19_ucPortRemapConfig0;
uint8_t HCCT19_ucPortRemapConfig1;
uint8_t HCCT20_ucPortRemapConfig0;
uint8_t HCCT21_ucPortRemapConfig0;
uint8_t HCCT22_ucPortRemapConfig0;
uint8_t HCCT22_ucPortRemapConfig1;
uint8_t HCCT23_ucPortRemapConfig0;
uint8_t HCCT23_ucPortRemapConfig1;

/* CONSTANTS ******************************************************************************************/

/* Please note, that currently the HCCT definitions/configurations below are merely copies of HCCT0,
 to avoid problems with the linker... If used, please check the configurations! */

const T_HCCT_Config HCCT0_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT1_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT2_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE)  }};
const T_HCCT_Config HCCT3_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT4_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT5_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT6_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT7_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT8_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE)  }};
const T_HCCT_Config HCCT9_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT10_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT11_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT19_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT20_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT21_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT22_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};
const T_HCCT_Config HCCT23_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_512, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE|HCCT_CFG_CHANNEL1_ACTIVE) }};


/* FUNCTIONS ******************************************************************************************/
/* This function inits 32 bit timer mode
 * Please note, that only HW TIM2 and TIM5 are 32 bits
 * */

void HCCT_vEnable32BitMode(void)
{
    //TIM2_ARR = 0xFFFFFFFF;  ///< uncomment in case you want to use HCCT2/3 (TIM2) in 32 bit mode
    //TIM5_ARR = 0xFFFFFFFF;  ///< uncomment in case you want to use HCCT8/9 (TIM5) in 32 bit mode
}

void HCCT0_vHandleTimerEvent(void)  {}
void HCCT1_vHandleTimerEvent(void)  {}
void HCCT2_vHandleTimerEvent(void)  {}
void HCCT3_vHandleTimerEvent(void)  {}
void HCCT4_vHandleTimerEvent(void)  {}
void HCCT5_vHandleTimerEvent(void)  {}
void HCCT6_vHandleTimerEvent(void)  {}
void HCCT7_vHandleTimerEvent(void)  {}
void HCCT8_vHandleTimerEvent(void)  {}
void HCCT9_vHandleTimerEvent(void)  {}
void HCCT10_vHandleTimerEvent(void) {}
void HCCT11_vHandleTimerEvent(void) {}
void HCCT19_vHandleTimerEvent(void) {}
void HCCT20_vHandleTimerEvent(void) {}
void HCCT21_vHandleTimerEvent(void) {}
void HCCT22_vHandleTimerEvent(void) {}
void HCCT23_vHandleTimerEvent(void) {}

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
void HCCT6_vHandleEvent0(void)  {}
void HCCT6_vHandleEvent1(void)  {}
void HCCT7_vHandleEvent0(void)  {}
void HCCT7_vHandleEvent1(void)  {}
void HCCT8_vHandleEvent0(void)  {}
void HCCT8_vHandleEvent1(void)  {}
void HCCT9_vHandleEvent0(void)  {}
void HCCT9_vHandleEvent1(void)  {}
void HCCT10_vHandleEvent0(void) {}
void HCCT10_vHandleEvent1(void) {}
void HCCT11_vHandleEvent0(void) {}
void HCCT11_vHandleEvent1(void) {}
void HCCT19_vHandleEvent0(void) {}
void HCCT19_vHandleEvent1(void) {}
void HCCT20_vHandleEvent0(void) {}
void HCCT21_vHandleEvent0(void) {}
void HCCT22_vHandleEvent0(void) {}
void HCCT22_vHandleEvent1(void) {

}
void HCCT23_vHandleEvent0(void) {}
void HCCT23_vHandleEvent1(void) {}

/*---------------------------------------------------------------------------------*/
/* Select your desired port mapping configuration by uncommenting respective lines */

void HCCT0_vPortRemap0(void)
{
   /* HCCT0_CH0 - TIM1_CH1 - option 1: PA8 */
   HDIO_vEnablePortA();                          // Enable clock for port A
   HDIO_vSetModePortA8(HDIO_ALTERNATE_FUNCTION); // Use PA8 as alternate function pin
   HDIO_vSetAlternateFunctionA8(HDIO_ALTERNATE_FUNCTION_6);     // Use alternate function 2
   HCCT0_ucPortRemapConfig0 = 0; // Remember selected port remapping configuration

  /* HCCT0_CH0 - TIM1_CH1 - option 2: PC0 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC0(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC8(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT0_ucPortRemapConfig0 = 1;

  /* HCCT0_CH0 - TIM1_CH1 - option 3: PE9 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE9(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT0_ucPortRemapConfig0 = 2;
}

void HCCT0_vPortRemap1(void)
{
   /* HCCT0_CH1 - TIM1_CH2 - option 1: PA9 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA9(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA9(HDIO_ALTERNATE_FUNCTION_6);
   HCCT0_ucPortRemapConfig1 = 0;

  /* HCCT0_CH1 - TIM1_CH2 - option 2: PC1 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC1(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC1(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT0_ucPortRemapConfig1 = 1;

  /* HCCT0_CH1 - TIM1_CH2 - option 3: PE11 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE11(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE11(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT0_ucPortRemapConfig1 = 2;
}

void HCCT1_vPortRemap0(void)
{
   /* HCCT1_CH0 - TIM1_CH3 - option 1: PA10 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA10(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA10(HDIO_ALTERNATE_FUNCTION_6);
   HCCT1_ucPortRemapConfig0 = 0;

  /* HCCT1_CH0 - TIM1_CH3 - option 2: PC2 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC2(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC2(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT1_ucPortRemapConfig0 = 1;

  /* HCCT1_CH0 - TIM1_CH3 - option 3: PE13 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE13(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE13(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT1_ucPortRemapConfig0 = 2;
}

void HCCT1_vPortRemap1(void)
{
   /* HCCT1_CH1 - TIM1_CH4 - option 1: PC3 */
   HDIO_vEnablePortC();
   HDIO_vSetModePortC3(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionC3(HDIO_ALTERNATE_FUNCTION_2);
   HCCT1_ucPortRemapConfig1 = 0;

  /* HCCT1_CH1 - TIM1_CH4 - option 2: PE14 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE14(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE14(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT1_ucPortRemapConfig1 = 1;
}

void HCCT2_vPortRemap0(void)
{
   /* HCCT2_CH0 - TIM2_CH1 - option 1: PA0 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA0(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA0(HDIO_ALTERNATE_FUNCTION_1);
   HCCT2_ucPortRemapConfig0 = 0;

  /* HCCT2_CH0 - TIM2_CH1 - option 2: PA5 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA5(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA5(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT2_ucPortRemapConfig0 = 1;

  /* HCCT2_CH0 - TIM2_CH1 - option 3: PA15 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA15(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA15(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT2_ucPortRemapConfig0 = 2;

  /* HCCT2_CH0 - TIM2_CH1 - option 4: PD3 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD3(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD3(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig0 = 3;
}

void HCCT2_vPortRemap1(void)
{
   /* HCCT2_CH1 - TIM2_CH2 - option 1: PA1 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA1(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA1(HDIO_ALTERNATE_FUNCTION_1);
   HCCT2_ucPortRemapConfig1 = 0;

  /* HCCT2_CH1 - TIM2_CH2 - option 2: PB3 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB3(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB3(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT2_ucPortRemapConfig1 = 1;

  /* HCCT2_CH1 - TIM2_CH2 - option 3: PD4 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD4(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD4(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig0 = 2;
}

void HCCT3_vPortRemap0(void)
{
   /* HCCT3_CH0 - TIM2_CH3 - option 1: PA2 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA2(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA2(HDIO_ALTERNATE_FUNCTION_1);
   HCCT3_ucPortRemapConfig0 = 0;

  /* HCCT3_CH0 - TIM2_CH3 - option 2: PA9 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA9(HDIO_ALTERNATE_FUNCTION_10);
//   HCCT3_ucPortRemapConfig0 = 1;

  /* HCCT3_CH0 - TIM2_CH3 - option 3: PB10 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB10(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB10(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT2_ucPortRemapConfig0 = 2;

  /* HCCT3_CH0 - TIM2_CH3 - option 4: PD7 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD7(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig0 = 3;
}

void HCCT3_vPortRemap1(void)
{
   /* HCCT3_CH1 - TIM2_CH4 - option 1: PA3 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA3(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA3(HDIO_ALTERNATE_FUNCTION_1);
   HCCT3_ucPortRemapConfig1 = 0;

  /* HCCT3_CH1 - TIM2_CH4 - option 2: PA10 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA10(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA10(HDIO_ALTERNATE_FUNCTION_10);
//   HCCT3_ucPortRemapConfig1 = 1;

  /* HCCT3_CH1 - TIM2_CH4 - option 3: PB11 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB11(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB11(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT2_ucPortRemapConfig0 = 2;

  /* HCCT3_CH0 - TIM2_CH4 - option 4: PD6 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD6(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT2_ucPortRemapConfig0 = 3;
}

void HCCT4_vPortRemap0(void)
{
   /* HCCT4_CH0 - TIM3_CH1 - option 1: PA6 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA6(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA6(HDIO_ALTERNATE_FUNCTION_2);
   HCCT4_ucPortRemapConfig0 = 0;

  /* HCCT4_CH0 - TIM3_CH1 - option 2: PB4 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB4(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB4(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT4_ucPortRemapConfig0 = 1;

  /* HCCT4_CH0 - TIM3_CH1 - option 3: PC6 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC6(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT4_ucPortRemapConfig0 = 2;

  /* HCCT4_CH0 - TIM3_CH1 - option 4: PE2 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE2(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE2(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT4_ucPortRemapConfig0 = 3;
}

void HCCT4_vPortRemap1(void)
{
   /* HCCT4_CH1 - TIM3_CH2 - option 1: PA4 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA4(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA4(HDIO_ALTERNATE_FUNCTION_2);
   HCCT4_ucPortRemapConfig1 = 0;

  /* HCCT4_CH1 - TIM3_CH2 - option 2: PA7 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA7(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT4_ucPortRemapConfig1 = 1;

  /* HCCT4_CH1 - TIM3_CH2 - option 3: PB5 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB5(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB5(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT4_ucPortRemapConfig1 = 2;

  /* HCCT4_CH1 - TIM3_CH2 - option 4: PC7 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC7(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT4_ucPortRemapConfig1 = 3;

  /* HCCT4_CH1 - TIM3_CH2 - option 5: PE3 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE3(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE3(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT4_ucPortRemapConfig1 = 4;
}

void HCCT5_vPortRemap0(void)
{
   /* HCCT5_CH0 - TIM3_CH3 - option 1: PB0 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB0(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB0(HDIO_ALTERNATE_FUNCTION_2);
   HCCT5_ucPortRemapConfig0 = 0;

  /* HCCT5_CH0 - TIM3_CH3 - option 2: PC8 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC8(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT5_ucPortRemapConfig0 = 1;

  /* HCCT5_CH0 - TIM3_CH3 - option 3: PE4 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE4(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC8(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT5_ucPortRemapConfig0 = 2;
}

void HCCT5_vPortRemap1(void)
{
   /* HCCT5_CH1 - TIM3_CH4 - option 1: PB1 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB1(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB1(HDIO_ALTERNATE_FUNCTION_2);
   HCCT5_ucPortRemapConfig1 = 0;

  /* HCCT5_CH1 - TIM3_CH4 - option 2: PB7 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB7(HDIO_ALTERNATE_FUNCTION_10);
//   HCCT5_ucPortRemapConfig1 = 1;

  /* HCCT5_CH1 - TIM3_CH4 - option 3: PC9 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC9(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT5_ucPortRemapConfig1 = 2;

  /* HCCT5_CH1 - TIM3_CH4 - option 4: PE5 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE5(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE5(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT5_ucPortRemapConfig1 = 3;
}

void HCCT6_vPortRemap0(void)
{
   /* HCCT6_CH0 - TIM4_CH1 - option 1: PA11 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA11(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA11(HDIO_ALTERNATE_FUNCTION_10);
   HCCT6_ucPortRemapConfig0 = 0;

  /* HCCT6_CH0 - TIM4_CH1 - option 2: PB6 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB6(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT6_ucPortRemapConfig0 = 1;

  /* HCCT6_CH0 - TIM4_CH1 - option 3: PD12 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD12(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD12(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT6_ucPortRemapConfig0 = 2;
}

void HCCT6_vPortRemap1(void)
{
   /* HCCT6_CH1 - TIM4_CH2 - option 1: PA12 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA12(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA12(HDIO_ALTERNATE_FUNCTION_10);
   HCCT6_ucPortRemapConfig1 = 0;

  /* HCCT6_CH1 - TIM4_CH2 - option 2: PB7 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB7(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT6_ucPortRemapConfig1 = 1;

  /* HCCT6_CH1 - TIM4_CH2 - option 3: PD13 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD13(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD13(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT6_ucPortRemapConfig1 = 2;
}

void HCCT7_vPortRemap0(void)
{
   /* HCCT7_CH0 - TIM4_CH3 - option 1: PA13 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA13(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA13(HDIO_ALTERNATE_FUNCTION_10);
   HCCT7_ucPortRemapConfig0 = 0;

  /* HCCT7_CH0 - TIM4_CH3 - option 2: PB8 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB8(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT7_ucPortRemapConfig0 = 1;

  /* HCCT7_CH0 - TIM4_CH3 - option 3: PD14 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD14(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD14(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT7_ucPortRemapConfig0 = 2;
}

void HCCT7_vPortRemap1(void)
{
   /* HCCT7_CH1 - TIM4_CH4 - option 1: PB9 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB9(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB9(HDIO_ALTERNATE_FUNCTION_2);
   HCCT7_ucPortRemapConfig1 = 0;

  /* HCCT7_CH1 - TIM4_CH4 - option 2: PD15 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD15(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD15(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT7_ucPortRemapConfig1 = 1;

  /* HCCT7_CH1 - TIM4_CH4 - option 3: PF6 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF6(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT7_ucPortRemapConfig1 = 2;
}

void HCCT8_vPortRemap0(void)
{
   /* HCCT8_CH0 - TIM5_CH1 - option 1: PA0 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA0(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA0(HDIO_ALTERNATE_FUNCTION_2);
   HCCT8_ucPortRemapConfig0 = 0;

  /* HCCT8_CH0 - TIM5_CH1 - option 2: PB2 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB2(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB2(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT8_ucPortRemapConfig0 = 1;

  /* HCCT8_CH0 - TIM5_CH1 - option 3: PF6 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF6(HDIO_ALTERNATE_FUNCTION_6);
//   HCCT8_ucPortRemapConfig0 = 2;
}

void HCCT8_vPortRemap1(void)
{
   /* HCCT8_CH1 - TIM5_CH2 - option 1: PA1 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA1(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA1(HDIO_ALTERNATE_FUNCTION_2);
   HCCT8_ucPortRemapConfig1 = 0;

  /* HCCT8_CH1 - TIM5_CH2 - option 2: PC12 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC12(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC12(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT8_ucPortRemapConfig1 = 1;

  /* HCCT8_CH1 - TIM5_CH2 - option 3: PF7 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF7(HDIO_ALTERNATE_FUNCTION_6);
//   HCCT8_ucPortRemapConfig1 = 2;
}

void HCCT9_vPortRemap0(void)
{
   /* HCCT9_CH0 - TIM5_CH3 - option 1: PA2 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA2(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA2(HDIO_ALTERNATE_FUNCTION_2);
   HCCT9_ucPortRemapConfig0 = 0;

  /* HCCT9_CH0 - TIM5_CH3 - option 2: PE8 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE8(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT9_ucPortRemapConfig0 = 1;

  /* HCCT9_CH0 - TIM5_CH3 - option 3: PF8 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF8(HDIO_ALTERNATE_FUNCTION_6);
//   HCCT9_ucPortRemapConfig0 = 2;
}

void HCCT9_vPortRemap1(void)
{
   /* HCCT9_CH1 - TIM5_CH4 - option 1: PA3 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA3(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA3(HDIO_ALTERNATE_FUNCTION_2);
   HCCT9_ucPortRemapConfig1 = 0;

  /* HCCT9_CH1 - TIM5_CH4 - option 2: PE9 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE9(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT9_ucPortRemapConfig1 = 1;

  /* HCCT9_CH1 - TIM5_CH4 - option 3: PF9 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF9(HDIO_ALTERNATE_FUNCTION_6);
//   HCCT9_ucPortRemapConfig1 = 2;
}

void HCCT10_vPortRemap0(void)
{
   /* HCCT10_CH0 - TIM8_CH1 - option 1: PA15 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA15(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA15(HDIO_ALTERNATE_FUNCTION_2);
   HCCT10_ucPortRemapConfig0 = 0;

  /* HCCT10_CH0 - TIM8_CH1 - option 2: PB6 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB6(HDIO_ALTERNATE_FUNCTION_5);
//   HCCT10_ucPortRemapConfig0 = 1;

  /* HCCT10_CH0 - TIM8_CH1 - option 3: PC6 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC6(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC6(HDIO_ALTERNATE_FUNCTION_4);
//   HCCT10_ucPortRemapConfig0 = 2;
}

void HCCT10_vPortRemap1(void)
{
   /* HCCT10_CH1 - TIM8_CH2 - option 1: PA14 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA14(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA14(HDIO_ALTERNATE_FUNCTION_5);
   HCCT10_ucPortRemapConfig1 = 0;

  /* HCCT10_CH1 - TIM8_CH2 - option 2: PB8 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB8(HDIO_ALTERNATE_FUNCTION_10);
//   HCCT10_ucPortRemapConfig1 = 1;

  /* HCCT10_CH1 - TIM8_CH2 - option 3: PC7 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC7(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC7(HDIO_ALTERNATE_FUNCTION_4);
//   HCCT10_ucPortRemapConfig1 = 2;
}

void HCCT11_vPortRemap0(void)
{
   /* HCCT11_CH0 - TIM8_CH3 - option 1: PB9 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB9(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB9(HDIO_ALTERNATE_FUNCTION_10);
   HCCT11_ucPortRemapConfig0 = 0;

  /* HCCT11_CH0 - TIM8_CH3 - option 2: PC8 */
//   HDIO_vEnablePortC();
//   HDIO_vSetModePortC8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionC8(HDIO_ALTERNATE_FUNCTION_4);
//   HCCT11_ucPortRemapConfig0 = 1;
}

void HCCT11_vPortRemap1(void)
{
   /* HCCT11_CH1 - TIM8_CH4 - option 1: PC9 */
   HDIO_vEnablePortC();
   HDIO_vSetModePortC9(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionC9(HDIO_ALTERNATE_FUNCTION_4);
   HCCT11_ucPortRemapConfig1 = 0;

  /* HCCT11_CH1 - TIM8_CH4 - option 2: PD1 */
//   HDIO_vEnablePortD();
//   HDIO_vSetModePortD1(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionD1(HDIO_ALTERNATE_FUNCTION_4);
//   HCCT11_ucPortRemapConfig1 = 1;
}

void HCCT19_vPortRemap0(void)
{
   /* HCCT19_CH0 - TIM15_CH1 - option 1: PB14 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB14(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB14(HDIO_ALTERNATE_FUNCTION_1);
   HCCT19_ucPortRemapConfig0 = 0;

  /* HCCT19_CH0 - TIM15_CH1 - option 2: PF9 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF9(HDIO_ALTERNATE_FUNCTION_3);
//   HCCT19_ucPortRemapConfig0 = 1;
}

void HCCT19_vPortRemap1(void)
{
   /* HCCT19_CH1 - TIM15_CH2 - option 1: PB15 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB15(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB15(HDIO_ALTERNATE_FUNCTION_1);
   HCCT19_ucPortRemapConfig1 = 0;

  /* HCCT19_CH1 - TIM15_CH2 - option 2: PF10 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF10(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF10(HDIO_ALTERNATE_FUNCTION_3);
//   HCCT19_ucPortRemapConfig1 = 1;
}


void HCCT20_vPortRemap0(void)
{
   /* HCCT20_CH0 - TIM16_CH1 - option 1: PA6 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA6(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA6(HDIO_ALTERNATE_FUNCTION_1);
   HCCT20_ucPortRemapConfig0 = 0;

  /* HCCT20_CH0 - TIM16_CH1 - option 2: PA12 */
//   HDIO_vEnablePortA();
//   HDIO_vSetModePortA12(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionA12(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT20_ucPortRemapConfig0 = 1;
//
  /* HCCT20_CH0 - TIM16_CH1 - option 3: PB4 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB4(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB4(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT20_ucPortRemapConfig0 = 2;

  /* HCCT20_CH0 - TIM16_CH1 - option 4: PB8 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB8(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB8(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT20_ucPortRemapConfig0 = 3;

  /* HCCT20_CH0 - TIM16_CH1 - option 5: PE0 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE0(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE0(HDIO_ALTERNATE_FUNCTION_4);
//   HCCT20_ucPortRemapConfig0 = 4;
}

void HCCT21_vPortRemap0(void)
{
   /* HCCT21_CH0 - TIM17_CH1 - option 1: PA7 */
   HDIO_vEnablePortA();
   HDIO_vSetModePortA7(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionA7(HDIO_ALTERNATE_FUNCTION_1);
   HCCT21_ucPortRemapConfig0 = 0;

  /* HCCT21_CH0 - TIM17_CH1 - option 2: PB5 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB5(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB5(HDIO_ALTERNATE_FUNCTION_10);
//   HCCT21_ucPortRemapConfig0 = 1;
//
  /* HCCT21_CH0 - TIM17_CH1 - option 3: PB9 */
//   HDIO_vEnablePortB();
//   HDIO_vSetModePortB9(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionB9(HDIO_ALTERNATE_FUNCTION_1);
//   HCCT21_ucPortRemapConfig0 = 2;

  /* HCCT21_CH0 - TIM17_CH1 - option 4: PE1 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE1(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE1(HDIO_ALTERNATE_FUNCTION_4);
//   HCCT21_ucPortRemapConfig0 = 3;
}

void HCCT22_vPortRemap0(void)
{
   /* HCCT22_CH0 - TIM20_CH1 - option 1: PB2 */
   HDIO_vEnablePortB();
   HDIO_vSetModePortB2(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionB2(HDIO_ALTERNATE_FUNCTION_3);
   HCCT22_ucPortRemapConfig0 = 0;

  /* HCCT22_CH0 - TIM20_CH1 - option 2: PE2 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE2(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE2(HDIO_ALTERNATE_FUNCTION_6);
//   HCCT22_ucPortRemapConfig0 = 1;

  /* HCCT22_CH0 - TIM20_CH1 - option 3: PF12 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF12(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF12(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT22_ucPortRemapConfig0 = 2;
}

void HCCT22_vPortRemap1(void)
{
   /* HCCT22_CH1 - TIM20_CH2 - option 1: PC2 */
   HDIO_vEnablePortC();
   HDIO_vSetModePortC2(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionC2(HDIO_ALTERNATE_FUNCTION_6);
   HCCT22_ucPortRemapConfig1 = 0;

  /* HCCT22_CH1 - TIM20_CH2 - option 2: PE3 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortE3(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionE3(HDIO_ALTERNATE_FUNCTION_6);
//   HCCT22_ucPortRemapConfig1 = 1;

  /* HCCT22_CH1 - TIM20_CH2 - option 3: PF13 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF13(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF13(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT22_ucPortRemapConfig1 = 2;
}

void HCCT23_vPortRemap0(void)
{
   /* HCCT23_CH0 - TIM20_CH3 - option 1: PC8 */
   HDIO_vEnablePortC();
   HDIO_vSetModePortC8(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionC8(HDIO_ALTERNATE_FUNCTION_6);
   HCCT23_ucPortRemapConfig0 = 0;

  /* HCCT23_CH0 - TIM20_CH3 - option 2: PF2 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF2(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF2(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT23_ucPortRemapConfig0 = 1;

  /* HCCT23_CH0 - TIM20_CH3 - option 3: PF14 */
//   HDIO_vEnablePortE();
//   HDIO_vSetModePortF14(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF14(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT23_ucPortRemapConfig1 = 2;
}

void HCCT23_vPortRemap1(void)
{
   /* HCCT23_CH1 - TIM20_CH4 - option 1: PE1 */
   HDIO_vEnablePortE();
   HDIO_vSetModePortE1(HDIO_ALTERNATE_FUNCTION);
   HDIO_vSetAlternateFunctionE1(HDIO_ALTERNATE_FUNCTION_6);
   HCCT1_ucPortRemapConfig1 = 0;

  /* HCCT23_CH1 - TIM20_CH4 - option 2: PF3 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF3(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF3(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT23_ucPortRemapConfig1 = 1;

  /* HCCT23_CH1 - TIM20_CH4 - option 3: PF15 */
//   HDIO_vEnablePortF();
//   HDIO_vSetModePortF15(HDIO_ALTERNATE_FUNCTION);
//   HDIO_vSetAlternateFunctionF15(HDIO_ALTERNATE_FUNCTION_2);
//   HCCT23_ucPortRemapConfig1 = 2;
}
