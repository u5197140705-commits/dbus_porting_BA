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
 *  COMP_ABBREV      HCCT
 *  AUTHOR           Pusztai Pavol
 *  CREATED          21.03.2017
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief    This module contains the stubs of HAL CCT functions.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hcct.h"


/* DEFINITIONS ****************************************************************/
//lint -esym(750,*_MODE,ALTERNATE_*)   possibly unused definitions 

#define INPUT_MODE                  0x0u
#define OUTPUT_MODE                 0x1u
#define ALTERNATE_FUNCTION_MODE     0x2u
#define ANALOG_MODE                 0x3u

#define ALTERNATE_FUNCTION_0        0x0
#define ALTERNATE_FUNCTION_1        0x1
#define ALTERNATE_FUNCTION_2        0x2
#define ALTERNATE_FUNCTION_3        0x3
#define ALTERNATE_FUNCTION_4        0x4
#define ALTERNATE_FUNCTION_5        0x5
#define ALTERNATE_FUNCTION_6        0x6
#define ALTERNATE_FUNCTION_7        0x7
#define ALTERNATE_FUNCTION_14       0xE


/* VARIABLES ******************************************************************************************/

/* Variable HCCT<unit>_ucPortRemapConfig<channel> holds port remapping configuration needed for HCCTx_bGetPinx */
uchar HCCT0_ucPortRemapConfig0;
uchar HCCT0_ucPortRemapConfig1;
uchar HCCT1_ucPortRemapConfig0;
uchar HCCT1_ucPortRemapConfig1;
uchar HCCT2_ucPortRemapConfig0;
uchar HCCT2_ucPortRemapConfig1;
uchar HCCT3_ucPortRemapConfig0;
uchar HCCT3_ucPortRemapConfig1;
uchar HCCT4_ucPortRemapConfig0;
uchar HCCT4_ucPortRemapConfig1;
uchar HCCT5_ucPortRemapConfig0;
uchar HCCT5_ucPortRemapConfig1;
uchar HCCT6_ucPortRemapConfig0;
uchar HCCT6_ucPortRemapConfig1;
uchar HCCT7_ucPortRemapConfig0;
uchar HCCT7_ucPortRemapConfig1;
uchar HCCT8_ucPortRemapConfig0;
uchar HCCT8_ucPortRemapConfig1;
uchar HCCT9_ucPortRemapConfig0;
uchar HCCT9_ucPortRemapConfig1;
uchar HCCT20_ucPortRemapConfig0;
uchar HCCT20_ucPortRemapConfig1;
uchar HCCT21_ucPortRemapConfig0;
uchar HCCT22_ucPortRemapConfig0;


/* CONSTANTS ******************************************************************************************/

/* Please note, that currently the HCCT definitions/configurations below are merely copies of HCCT0,
 to avoid problems with the linker... If used, please check the configurations! */

const T_HCCT_Config HCCT0_atConfig[] = {
   {
      HCCT_CONFIG_PRESCALE_2,
      HCCT_CFG_CHANNEL_CAPTURE_INPUT_MODE,     HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH,
      HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW,
      (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE)
   }, {
      HCCT_CONFIG_PRESCALE_16,
      HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE,    HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH,
      HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE,    HCCT_CFG_CHANNEL_PIN_LEVEL_LOW,
      (uint8_t)(HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE)
   }
};
const T_HCCT_Config HCCT1_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT2_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT3_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT4_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT5_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT6_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT7_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT8_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT9_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT12_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT13_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT20_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT21_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};
const T_HCCT_Config HCCT22_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_2, HCCT_CFG_CHANNEL_COMPARE_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, 0, 0, HCCT_CFG_CHANNEL0_ACTIVE }};


/* FUNCTIONS ******************************************************************************************/

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
void HCCT12_vHandleTimerEvent(void) {}
void HCCT13_vHandleTimerEvent(void) {}
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
void HCCT6_vHandleEvent0(void)  {}
void HCCT6_vHandleEvent1(void)  {}
void HCCT7_vHandleEvent0(void)  {}
void HCCT7_vHandleEvent1(void)  {}
void HCCT8_vHandleEvent0(void)  {}
void HCCT8_vHandleEvent1(void)  {}
void HCCT9_vHandleEvent0(void)  {}
void HCCT9_vHandleEvent1(void)  {}
void HCCT12_vHandleEvent0(void) {}
void HCCT12_vHandleEvent1(void) {}
void HCCT13_vHandleEvent0(void) {}
void HCCT13_vHandleEvent1(void) {}
void HCCT20_vHandleEvent0(void) {}
void HCCT20_vHandleEvent1(void) {}
void HCCT21_vHandleEvent0(void) {}
void HCCT22_vHandleEvent0(void) {}


/*---------------------------------------------------------------------------------*/
/* Select your desired port mapping configuration by uncommenting respective lines */

void HCCT0_vPortRemap0(void)
{
    /* HCCT0_CH0 - TIM1_CH1 - option 1: PA8 */
    RCC_AHB2ENR_GPIOAEN  = 1;                         // Enable clock for port A
    GPIOA_MODER_MODER8   = ALTERNATE_FUNCTION_MODE;   // Use PA8 as alternate function pin
    GPIOA_AFRH_AFRH8     = ALTERNATE_FUNCTION_1;      // Use alternate function 1
    HCCT0_ucPortRemapConfig0 = 0;                     // Remember selected port remapping configuration

    /* HCCT0_CH0 - TIM1_CH1 - option 2: PE9 */
/*  RCC_AHB2ENR_GPIOEEN  = 1;
    GPIOE_MODER_MODER9   = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRH_AFRH9     = ALTERNATE_FUNCTION_1;
    HCCT0_ucPortRemapConfig0 = 1; */
}

void HCCT0_vPortRemap1(void)
{
    /* HCCT0_CH1 - TIM1_CH2 - option 1: PA9 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER9   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRH_AFRH9     = ALTERNATE_FUNCTION_1;
    HCCT0_ucPortRemapConfig1 = 0;

    /* HCCT0_CH1 - TIM1_CH2 - option 2: PE11 */
/*  RCC_AHB2ENR_GPIOEEN  = 1;
    GPIOE_MODER_MODER11  = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRH_AFRH11    = ALTERNATE_FUNCTION_1;
    HCCT0_ucPortRemapConfig1 = 1; */
}

void HCCT1_vPortRemap0(void)
{
    /* HCCT1_CH0 - TIM1_CH3 - option 1: PA10 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER10  = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRH_AFRH10    = ALTERNATE_FUNCTION_1;
    HCCT1_ucPortRemapConfig0 = 0;

    /* HCCT1_CH0 - TIM1_CH3 - option 2: PE13 */
/*  RCC_AHB2ENR_GPIOEEN  = 1;
    GPIOE_MODER_MODER13  = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRH_AFRH13    = ALTERNATE_FUNCTION_1;
    HCCT1_ucPortRemapConfig0 = 1; */
}

void HCCT1_vPortRemap1(void)
{
    /* HCCT1_CH1 - TIM1_CH4 - option 1: PA11 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER11  = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRH_AFRH11    = ALTERNATE_FUNCTION_1;
    HCCT1_ucPortRemapConfig1 = 0;

    /* HCCT1_CH1 - TIM1_CH4 - option 2: PE14 */
/*  RCC_AHB2ENR_GPIOEEN  = 1;
    GPIOE_MODER_MODER14  = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRH_AFRH14    = ALTERNATE_FUNCTION_1;
    HCCT1_ucPortRemapConfig1 = 1; */
}

void HCCT2_vPortRemap0(void)
{
    /* HCCT2_CH0 - TIM2_CH1 - option 1: PA0 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER0   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL0     = ALTERNATE_FUNCTION_1;
    HCCT2_ucPortRemapConfig0 = 0;

    /* HCCT2_CH0 - TIM2_CH1 - option 2: PA5 */
/*  RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER5   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL5     = ALTERNATE_FUNCTION_1;
    HCCT2_ucPortRemapConfig0 = 1; */

    /* HCCT2_CH0 - TIM2_CH1 - option 3: PA15 */
/*  RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER15  = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRH_AFRH15    = ALTERNATE_FUNCTION_1;
    HCCT2_ucPortRemapConfig0 = 2; */
}

void HCCT2_vPortRemap1(void)
{
    /* HCCT2_CH1 - TIM2_CH2 - option 1: PA1 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER1   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL1     = ALTERNATE_FUNCTION_1;
    HCCT2_ucPortRemapConfig1 = 0;

    /* HCCT2_CH1 - TIM2_CH2 - option 2: PB3 */
/*  RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER3   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRL_AFRL3     = ALTERNATE_FUNCTION_1;
    HCCT2_ucPortRemapConfig1 = 1; */
}

void HCCT3_vPortRemap0(void)
{
    /* HCCT3_CH0 - TIM2_CH3 - option 1: PA2 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER2   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL2     = ALTERNATE_FUNCTION_1;
    HCCT3_ucPortRemapConfig0 = 0;

    /* HCCT3_CH0 - TIM2_CH3 - option 2: PB10 */
/*  RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER10  = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRH_AFRH10    = ALTERNATE_FUNCTION_1;
    HCCT3_ucPortRemapConfig0 = 1; */
}

void HCCT3_vPortRemap1(void)
{
    /* HCCT3_CH1 - TIM2_CH4 - option 1: PA3 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER3   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL3     = ALTERNATE_FUNCTION_1;
    HCCT3_ucPortRemapConfig1 = 0;

    /* HCCT3_CH1 - TIM2_CH4 - option 2: PB11 */
/*  RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER11  = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRH_AFRH11    = ALTERNATE_FUNCTION_1;
    HCCT3_ucPortRemapConfig1 = 1; */
}

void HCCT4_vPortRemap0(void)
{
    /* HCCT4_CH0 - TIM3_CH1 - option 1: PA6 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER6   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL6     = ALTERNATE_FUNCTION_2;
    HCCT4_ucPortRemapConfig0 = 0;

    /* HCCT4_CH0 - TIM3_CH1 - option 2: PB4 */
/*  RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER4   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRL_AFRL4     = ALTERNATE_FUNCTION_2;
    HCCT4_ucPortRemapConfig0 = 1; */

    /* HCCT4_CH0 - TIM3_CH1 - option 3: PC6 */
/*  RCC_AHB2ENR_GPIOCEN  = 1;
    GPIOC_MODER_MODER6   = ALTERNATE_FUNCTION_MODE;
    GPIOC_AFRL_AFRL6     = ALTERNATE_FUNCTION_2;
    HCCT4_ucPortRemapConfig0 = 2; */

    /* HCCT4_CH0 - TIM3_CH1 - option 4: PE3 */
/*  RCC_AHB2ENR_GPIOEEN  = 1;
    GPIOE_MODER_MODER3   = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRL_AFRL3     = ALTERNATE_FUNCTION_2;
    HCCT4_ucPortRemapConfig0 = 3; */

}

void HCCT4_vPortRemap1(void)
{
    /* HCCT4_CH1 - TIM3_CH2 - option 1: PA7 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER7   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL7     = ALTERNATE_FUNCTION_2;
    HCCT4_ucPortRemapConfig1 = 0;

    /* HCCT4_CH1 - TIM3_CH2 - option 2: PB5 */
/*  RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER5   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRL_AFRL5     = ALTERNATE_FUNCTION_2;
    HCCT4_ucPortRemapConfig1 = 1; */

    /* HCCT4_CH1 - TIM3_CH2 - option 3: PC7 */
/*  RCC_AHB2ENR_GPIOCEN  = 1;
    GPIOC_MODER_MODER7   = ALTERNATE_FUNCTION_MODE;
    GPIOC_AFRL_AFRL7     = ALTERNATE_FUNCTION_2;
    HCCT4_ucPortRemapConfig1 = 2; */

    /* HCCT4_CH1 - TIM3_CH2 - option 4: PE4 */
/*  RCC_AHB2ENR_GPIOEEN  = 1;
    GPIOE_MODER_MODER4   = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRL_AFRL4     = ALTERNATE_FUNCTION_2;
    HCCT4_ucPortRemapConfig1 = 3; */
}

void HCCT5_vPortRemap0(void)
{
    /* HCCT5_CH0 - TIM3_CH3 - option 1: PB0 */
    RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER0   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRL_AFRL0     = ALTERNATE_FUNCTION_2;
    HCCT5_ucPortRemapConfig0 = 0;

    /* HCCT5_CH0 - TIM3_CH3 - option 2: PC8 */
/*  RCC_AHB2ENR_GPIOCEN  = 1;
    GPIOC_MODER_MODER8   = ALTERNATE_FUNCTION_MODE;
    GPIOC_AFRH_AFRH8     = ALTERNATE_FUNCTION_2;
    HCCT5_ucPortRemapConfig0 = 1; */

    /* HCCT5_CH0 - TIM3_CH3 - option 3: PE5 */
/*  RCC_AHB2ENR_GPIOEEN  = 1;
    GPIOE_MODER_MODER5   = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRL_AFRL5     = ALTERNATE_FUNCTION_2;
    HCCT5_ucPortRemapConfig0 = 2; */
}

void HCCT5_vPortRemap1(void)
{
    /* HCCT5_CH1 - TIM3_CH4 - option 1: PB1 */
    RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER1   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRL_AFRL1     = ALTERNATE_FUNCTION_2;
    HCCT5_ucPortRemapConfig1 = 0;

    /* HCCT5_CH1 - TIM3_CH4 - option 2: PC9 */
/*  RCC_AHB2ENR_GPIOCEN  = 1;
    GPIOC_MODER_MODER9   = ALTERNATE_FUNCTION_MODE;
    GPIOC_AFRH_AFRH9     = ALTERNATE_FUNCTION_2;
    HCCT5_ucPortRemapConfig1 = 1; */

    /* HCCT5_CH1 - TIM3_CH4 - option 3: PE6 */
/*  RCC_AHB2ENR_GPIOEEN  = 1;
    GPIOE_MODER_MODER6   = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRL_AFRL6     = ALTERNATE_FUNCTION_2;
    HCCT5_ucPortRemapConfig1 = 2; */
}

void HCCT6_vPortRemap0(void)
{
    /* HCCT6_CH0 - TIM4_CH1 - option 1: PB6 */
    RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER6   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRL_AFRL6     = ALTERNATE_FUNCTION_2;
    HCCT6_ucPortRemapConfig0 = 0;

    /* HCCT6_CH0 - TIM4_CH1 - option 2: PD12 */
/*  RCC_AHB2ENR_GPIODEN  = 1;
    GPIOD_MODER_MODER12  = ALTERNATE_FUNCTION_MODE;
    GPIOD_AFRH_AFRH12    = ALTERNATE_FUNCTION_2;
    HCCT6_ucPortRemapConfig0 = 1; */
}

void HCCT6_vPortRemap1(void)
{
    /* HCCT6_CH1 - TIM4_CH2 - option 1: PB7 */
    RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER7   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRL_AFRL7     = ALTERNATE_FUNCTION_2;
    HCCT6_ucPortRemapConfig1 = 0;

    /* HCCT6_CH1 - TIM4_CH2 - option 2: PD13 */
/*  RCC_AHB2ENR_GPIODEN  = 1;
    GPIOD_MODER_MODER13  = ALTERNATE_FUNCTION_MODE;
    GPIOD_AFRH_AFRH13    = ALTERNATE_FUNCTION_2;
    HCCT6_ucPortRemapConfig1 = 1; */
}

void HCCT7_vPortRemap0(void)
{
    /* HCCT7_CH0 - TIM4_CH3 - option 1: PB8 */
    RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER8   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRH_AFRH8     = ALTERNATE_FUNCTION_2;
    HCCT7_ucPortRemapConfig0 = 0;

    /* HCCT7_CH0 - TIM4_CH3 - option 2: PD14 */
/*  RCC_AHB2ENR_GPIODEN  = 1;
    GPIOD_MODER_MODER14  = ALTERNATE_FUNCTION_MODE;
    GPIOD_AFRH_AFRH14    = ALTERNATE_FUNCTION_2;
    HCCT7_ucPortRemapConfig0 = 1; */
}

void HCCT7_vPortRemap1(void)
{
    /* HCCT7_CH1 - TIM4_CH4 - option 1: PB9 */
    RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER9   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRH_AFRH9     = ALTERNATE_FUNCTION_2;
    HCCT7_ucPortRemapConfig1 = 0;

    /* HCCT7_CH1 - TIM4_CH4 - option 2: PD15 */
/*  RCC_AHB2ENR_GPIODEN  = 1;
    GPIOD_MODER_MODER15  = ALTERNATE_FUNCTION_MODE;
    GPIOD_AFRH_AFRH15    = ALTERNATE_FUNCTION_2;
    HCCT7_ucPortRemapConfig1 = 1; */
}

void HCCT8_vPortRemap0(void)
{
    /* HCCT8_CH0 - TIM5_CH1 - option 1: PA0 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER0   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL0     = ALTERNATE_FUNCTION_2;
    HCCT8_ucPortRemapConfig0 = 0;

    /* HCCT8_CH0 - TIM5_CH1 - option 2: PF6 */
/*  RCC_AHB2ENR_GPIOFEN  = 1;
    GPIOF_MODER_MODER6   = ALTERNATE_FUNCTION_MODE;
    GPIOF_AFRL_AFRL6     = ALTERNATE_FUNCTION_2;
    HCCT8_ucPortRemapConfig0 = 1; */
}

void HCCT8_vPortRemap1(void)
{
    /* HCCT8_CH1 - TIM5_CH2 - option 1: PA1 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER1   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL1     = ALTERNATE_FUNCTION_2;
    HCCT8_ucPortRemapConfig1 = 0;

    /* HCCT8_CH1 - TIM5_CH2 - option 2: PF7 */
/*  RCC_AHB2ENR_GPIOFEN  = 1;
    GPIOF_MODER_MODER7   = ALTERNATE_FUNCTION_MODE;
    GPIOF_AFRL_AFRL7     = ALTERNATE_FUNCTION_2;
    HCCT8_ucPortRemapConfig1 = 1; */
}

void HCCT9_vPortRemap0(void)
{
    /* HCCT9_CH0 - TIM5_CH3 - option 1: PA2 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER2   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL2     = ALTERNATE_FUNCTION_2;
    HCCT9_ucPortRemapConfig0 = 0;

    /* HCCT9_CH0 - TIM5_CH3 - option 2: PF8 */
/*  RCC_AHB2ENR_GPIOFEN  = 1;
    GPIOF_MODER_MODER8   = ALTERNATE_FUNCTION_MODE;
    GPIOF_AFRH_AFRH8     = ALTERNATE_FUNCTION_2;
    HCCT9_ucPortRemapConfig0 = 1; */
}

void HCCT9_vPortRemap1(void)
{
    /* HCCT9_CH1 - TIM5_CH4 - option 1: PA3 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER3   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL3     = ALTERNATE_FUNCTION_2;
    HCCT9_ucPortRemapConfig1 = 0;

    /* HCCT9_CH1 - TIM5_CH4 - option 2: PF9 */
/*  RCC_AHB2ENR_GPIOFEN  = 1;
    GPIOF_MODER_MODER9   = ALTERNATE_FUNCTION_MODE;
    GPIOF_AFRH_AFRH9     = ALTERNATE_FUNCTION_2;
    HCCT9_ucPortRemapConfig1 = 1; */
}

void HCCT12_vPortRemap0(void)
{
    /* HCCT12_CH0 - TIM8_CH1 - option 1: PC6 */
    RCC_AHB2ENR_GPIOCEN  = 1;
    GPIOC_MODER_MODER6   = ALTERNATE_FUNCTION_MODE;
    GPIOC_AFRL_AFRL6     = ALTERNATE_FUNCTION_3;
}

void HCCT12_vPortRemap1(void)
{
    /* HCCT12_CH1 - TIM8_CH2 - option 1: PC7 */
    RCC_AHB2ENR_GPIOCEN  = 1;
    GPIOC_MODER_MODER7   = ALTERNATE_FUNCTION_MODE;
    GPIOC_AFRL_AFRL7     = ALTERNATE_FUNCTION_3;
}

void HCCT13_vPortRemap0(void)
{
    /* HCCT13_CH0 - TIM8_CH3 - option 1: PC8 */
    RCC_AHB2ENR_GPIOCEN  = 1;
    GPIOC_MODER_MODER8   = ALTERNATE_FUNCTION_MODE;
    GPIOC_AFRH_AFRH8     = ALTERNATE_FUNCTION_3;
}

void HCCT13_vPortRemap1(void)
{
    /* HCCT13_CH1 - TIM8_CH4 - option 1: PC9 */
    RCC_AHB2ENR_GPIOCEN  = 1;
    GPIOC_MODER_MODER9   = ALTERNATE_FUNCTION_MODE;
    GPIOC_AFRH_AFRH9     = ALTERNATE_FUNCTION_3;
}

void HCCT20_vPortRemap0(void)
{
    /* HCCT20_CH0 - TIM15_CH1 - option 1: PA2 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER2   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL2     = ALTERNATE_FUNCTION_14;
    HCCT20_ucPortRemapConfig0 = 0;

    /* HCCT20_CH0 - TIM15_CH1 - option 2: PB14 */
/*  RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER14  = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRH_AFRH14    = ALTERNATE_FUNCTION_14;
    HCCT20_ucPortRemapConfig0 = 1; */

    /* HCCT20_CH0 - TIM15_CH1 - option 3: PF9 */
/*  RCC_AHB2ENR_GPIOFEN  = 1;
    GPIOF_MODER_MODER9   = ALTERNATE_FUNCTION_MODE;
    GPIOF_AFRH_AFRH9     = ALTERNATE_FUNCTION_14;
    HCCT20_ucPortRemapConfig0 = 2; */

    /* HCCT20_CH0 - TIM15_CH1 - option 4: PG10 */
/*  RCC_AHB2ENR_GPIOGEN  = 1;
    GPIOG_MODER_MODER10  = ALTERNATE_FUNCTION_MODE;
    GPIOG_AFRH_AFRH10    = ALTERNATE_FUNCTION_14;
    HCCT20_ucPortRemapConfig0 = 3; */
}

void HCCT20_vPortRemap1(void)
{
    /* HCCT20_CH1 - TIM15_CH2 - option 1: PA3 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER3   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL3     = ALTERNATE_FUNCTION_14;
    HCCT20_ucPortRemapConfig1 = 0;

    /* HCCT20_CH1 - TIM15_CH2 - option 2: PB15 */
/*  RCC_AHB2ENR_GPIOBEN  = 1;
    GPIOB_MODER_MODER15  = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRH_AFRH15    = ALTERNATE_FUNCTION_14;
    HCCT20_ucPortRemapConfig1 = 1; */

    /* HCCT20_CH1 - TIM15_CH2 - option 3: PF10 */
/*  RCC_AHB2ENR_GPIOFEN  = 1;
    GPIOF_MODER_MODER10  = ALTERNATE_FUNCTION_MODE;
    GPIOF_AFRH_AFRH10    = ALTERNATE_FUNCTION_14;
    HCCT20_ucPortRemapConfig1 = 2; */

    /* HCCT20_CH1 - TIM15_CH2 - option 4: PG11 */
/*  RCC_AHB2ENR_GPIOGEN  = 1;
    GPIOG_MODER_MODER11  = ALTERNATE_FUNCTION_MODE;
    GPIOG_AFRH_AFRH11    = ALTERNATE_FUNCTION_14;
    HCCT20_ucPortRemapConfig0 = 3; */
}

void HCCT21_vPortRemap0(void)
{
    /* HCCT21_CH0 - TIM16_CH1 - option 1: PA6 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER6   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL6     = ALTERNATE_FUNCTION_14;
    HCCT21_ucPortRemapConfig0 = 0;

    /* HCCT21_CH0 - TIM16_CH1 - option 2: PB8 */
/*  RCC_AHB2ENR_GPIOBEN = 1;
    GPIOB_MODER_MODER8   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRH_AFRH8     = ALTERNATE_FUNCTION_14;
    HCCT21_ucPortRemapConfig0 = 1; */

    /* HCCT21_CH0 - TIM16_CH1 - option 3: PE0 */
/*  RCC_AHB2ENR_GPIOEEN = 1;
    GPIOE_MODER_MODER0   = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRL_AFRL0     = ALTERNATE_FUNCTION_14;
    HCCT21_ucPortRemapConfig0 = 2; */
}

void HCCT22_vPortRemap0(void)
{
    /* HCCT22_CH0 - TIM17_CH1 - option 1: PA7 */
    RCC_AHB2ENR_GPIOAEN  = 1;
    GPIOA_MODER_MODER7   = ALTERNATE_FUNCTION_MODE;
    GPIOA_AFRL_AFRL7     = ALTERNATE_FUNCTION_14;
    HCCT22_ucPortRemapConfig0 = 0;

    /* HCCT22_CH0 - TIM17_CH1 - option 2: PB9 */
/*  RCC_AHB2ENR_GPIOBEN = 1;
    GPIOB_MODER_MODER9   = ALTERNATE_FUNCTION_MODE;
    GPIOB_AFRH_AFRH9     = ALTERNATE_FUNCTION_14;
    HCCT22_ucPortRemapConfig0 = 1; */

    /* HCCT22_CH0 - TIM17_CH1 - option 3: PE1 */
/*  RCC_AHB2ENR_GPIOEEN = 1;
    GPIOE_MODER_MODER1   = ALTERNATE_FUNCTION_MODE;
    GPIOE_AFRL_AFRL1     = ALTERNATE_FUNCTION_14;
    HCCT22_ucPortRemapConfig0 = 2; */
}
