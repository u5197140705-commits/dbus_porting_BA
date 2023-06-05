/*******************************************************************************
*   Copyright (c) 2018 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   COMP_ABBREV      HCCT
*   PROCESSOR        MKV5X
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief     This is configuration C file for HCCT library. Here is Config structure defined and can be expanded for more configurations.
*              Here we also set which PORT is selected for each respective channel and HCCT unit. Only one option should be chosen for each
*              channel. If used more than one option, the later will be used. For interrupts, here user can define HandleEvent functions.
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hcct.h"
#include "processor.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/*======================================================================
   Config structure settings
======================================================================*/
const HCCT_Config_t HCCT0_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT1_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT2_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT3_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT4_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT6_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT8_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT9_atConfig[]   = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT10_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
const HCCT_Config_t HCCT11_atConfig[]  = {{ HCCT_CONFIG_PRESCALE_128, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_LOW, HCCT_CFG_CHANNEL_COMPARE_NO_OUTPUT_MODE, HCCT_CFG_CHANNEL_PIN_LEVEL_HIGH, (uint8_t) (HCCT_CFG_CHANNEL0_ACTIVE | HCCT_CFG_CHANNEL1_ACTIVE) }};
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void HCCT0_vHandleEvent0(void){}
void HCCT0_vHandleEvent1(void){}
void HCCT1_vHandleEvent0(void){}
void HCCT1_vHandleEvent1(void){}
void HCCT2_vHandleEvent0(void){}
void HCCT2_vHandleEvent1(void){}
void HCCT3_vHandleEvent0(void){}
void HCCT3_vHandleEvent1(void){}
void HCCT4_vHandleEvent0(void){}
void HCCT4_vHandleEvent1(void){}
void HCCT6_vHandleEvent0(void){}
void HCCT6_vHandleEvent1(void){}
void HCCT8_vHandleEvent0(void){}
void HCCT8_vHandleEvent1(void){}
void HCCT9_vHandleEvent0(void){}
void HCCT9_vHandleEvent1(void){}
void HCCT10_vHandleEvent0(void){}
void HCCT10_vHandleEvent1(void){}
void HCCT11_vHandleEvent0(void){}
void HCCT11_vHandleEvent1(void){}


void HCCT0_vHandleTimerEvent(void){}
void HCCT1_vHandleTimerEvent(void){}
void HCCT2_vHandleTimerEvent(void){}
void HCCT3_vHandleTimerEvent(void){}
void HCCT4_vHandleTimerEvent(void){}
void HCCT6_vHandleTimerEvent(void){}
void HCCT8_vHandleTimerEvent(void){}
void HCCT9_vHandleTimerEvent(void){}
void HCCT10_vHandleTimerEvent(void){}
void HCCT11_vHandleTimerEvent(void){}

/*lint -save -e929 Note 929: cast from pointer to pointer [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required] 
  tested to work correctly on MKV56x */
void HCCT0_vPortRemap0(void)
{
    /* HCCT0_CH0 - FTM0_CH0 - option 1: PTC1 */
    SIM_SCGC5_PORTC = 1u;
    PORTC_PCR1_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT0)->tPort.tPortCh0 = HCCT_PORTC;
    ((HCCT_Module_t *)HCCT0)->tPort.u32PinCh0 = PORT_BIT1_MASK;

    /* HCCT0_CH0 - FTM0_CH0 - option 2: PTD0 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR0_MUX = HCCT_ALTERNATE_FUNCTION_5;
//    ((HCCT_Module_t *)HCCT0)->tPort.tPortCh1 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT0)->tPort.u32PinCh1 = PORT_BIT0_MASK;

    /* HCCT0_CH0 - FTM0_CH0 - option 3: PTE24 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR24_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT0)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT0)->tPort.u32PinCh0 = PORT_BIT24_MASK;

    /****** !!! Don't choose this option as it is debug (SWD_CLK / JTAG_TCLK) pin !!! *******/
    /* HCCT0_CH0 - FTM0_CH0 - option 4: PTA3 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR3_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT0)->tPort.tPortCh0 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT0)->tPort.u32PinCh0 = PORT_BIT3_MASK;
}

void HCCT0_vPortRemap1(void)
{
    /* HCCT0_CH1 - FTM0_CH1 - option 1: PTC2 */
    SIM_SCGC5_PORTC = 1u;
    PORTC_PCR2_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT0)->tPort.tPortCh1 = HCCT_PORTC;
    ((HCCT_Module_t *)HCCT0)->tPort.u32PinCh1 = PORT_BIT2_MASK;

    /* HCCT0_CH1 - FTM0_CH1 - option 2: PTD1 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR1_MUX = HCCT_ALTERNATE_FUNCTION_5;
//    ((HCCT_Module_t *)HCCT0)->tPort.tPortCh1 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT0)->tPort.u32PinCh1 = PORT_BIT1_MASK;

    /* HCCT0_CH1 - FTM0_CH1 - option 3: PTE25 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR25_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT0)->tPort.tPortCh1 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT0)->tPort.u32PinCh1 = PORT_BIT25_MASK;

    /****** This pin is default mapped as NMI_b *******/
    /* HCCT0_CH1 - FTM0_CH1 - option 4: PTA4 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR4_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT0)->tPort.tPortCh1 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT0)->tPort.u32PinCh1 = PORT_BIT4_MASK;
}

void HCCT1_vPortRemap0(void)
{
    /* HCCT1_CH0 - FTM0_CH2 - option 1: PTA5 */
    SIM_SCGC5_PORTA = 1u;
    PORTA_PCR5_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh1 = HCCT_PORTA;
    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh0 = PORT_BIT5_MASK;

    /* HCCT1_CH0 - FTM0_CH2 - option 2: PTC3 */
//    SIM_SCGC5_PORTC = 1u;
//    PORTC_PCR3_MUX = HCCT_ALTERNATE_FUNCTION_4;
//    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh0 = HCCT_PORTC;
//    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh0 = PORT_BIT3_MASK;

    /* HCCT1_CH0 - FTM0_CH2 - option 3: PTC5 */
//    SIM_SCGC5_PORTC = 1u;
//    PORTC_PCR5_MUX = HCCT_ALTERNATE_FUNCTION_7;
//    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh0 = HCCT_PORTC;
//    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh0 = PORT_BIT5_MASK;

    /* HCCT1_CH0 - FTM0_CH2 - option 4: PTD2 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR2_MUX = HCCT_ALTERNATE_FUNCTION_5;
//    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh0 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh0 = PORT_BIT2_MASK;

    /* HCCT1_CH0 - FTM0_CH2 - option 5: PTE29 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR29_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh0 = PORT_BIT29_MASK;
}

void HCCT1_vPortRemap1(void)
{
    /* HCCT1_CH1 - FTM0_CH3 - option 1: PTA6 */
    SIM_SCGC5_PORTA = 1u;
    PORTA_PCR6_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh1 = HCCT_PORTA;
    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh1 = PORT_BIT6_MASK;

    /* HCCT1_CH1 - FTM0_CH3 - option 2: PTC4 */
//    SIM_SCGC5_PORTC = 1u;
//    PORTC_PCR4_MUX = HCCT_ALTERNATE_FUNCTION_4;
//    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh1 = HCCT_PORTC;
//    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh1 = PORT_BIT4_MASK;

    /* HCCT1_CH1 - FTM0_CH3 - option 3: PTD3 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR3_MUX = HCCT_ALTERNATE_FUNCTION_5;
//    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh1 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh1 = PORT_BIT3_MASK;

    /* HCCT1_CH1 - FTM0_CH3 - option 4: PTE30 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR30_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT1)->tPort.tPortCh1 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT1)->tPort.u32PinCh1 = PORT_BIT30_MASK;
}

void HCCT2_vPortRemap0(void)
{
    /* HCCT2_CH0 - FTM0_CH4 - option 1: PTA7 */
    SIM_SCGC5_PORTA = 1u;
    PORTA_PCR7_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT2)->tPort.tPortCh0 = HCCT_PORTA;
    ((HCCT_Module_t *)HCCT2)->tPort.u32PinCh0 = PORT_BIT7_MASK;

    /* HCCT2_CH0 - FTM0_CH4 - option 2: PTD4 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR4_MUX = HCCT_ALTERNATE_FUNCTION_4;
//    ((HCCT_Module_t *)HCCT2)->tPort.tPortCh0 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT2)->tPort.u32PinCh0 = PORT_BIT4_MASK;

    /* HCCT2_CH0 - FTM0_CH4 - option 3: PTE26 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR26_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT2)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT2)->tPort.u32PinCh0 = PORT_BIT26_MASK;
}

void HCCT2_vPortRemap1(void)
{
    /* HCCT2_CH1 - FTM0_CH5 - option 1: PTD5 */
    SIM_SCGC5_PORTD = 1u;
    PORTD_PCR5_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT2)->tPort.tPortCh1 = HCCT_PORTD;
    ((HCCT_Module_t *)HCCT2)->tPort.u32PinCh1 = PORT_BIT5_MASK;

    /****** !!! Don't choose this option as it is debug (SWD_CLK / JTAG_TCLK) pin !!! *******/
    /* HCCT2_CH1 - FTM0_CH5 - option 2: PTA0 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR0_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT2)->tPort.tPortCh1 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT2)->tPort.u32PinCh1 = PORT_BIT0_MASK;
}

void HCCT3_vPortRemap0(void)
{
    /* HCCT3_CH0 - FTM0_CH6 - option 1: PTD6 */
    SIM_SCGC5_PORTD = 1u;
    PORTD_PCR6_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT3)->tPort.tPortCh0 = HCCT_PORTD;
    ((HCCT_Module_t *)HCCT3)->tPort.u32PinCh0 = PORT_BIT6_MASK;

    /****** This pin is default mapped as JTAG_TDI *******/
    /* HCCT3_CH0 - FTM0_CH6 - option 2: PTA1 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR1_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT3)->tPort.tPortCh0 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT3)->tPort.u32PinCh0 = PORT_BIT1_MASK;
}

void HCCT3_vPortRemap1(void)
{
    /* HCCT3_CH1 - FTM0_CH7 - option 1: PTD7 */
    SIM_SCGC5_PORTD = 1u;
    PORTD_PCR7_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT3)->tPort.tPortCh1 = HCCT_PORTD;
    ((HCCT_Module_t *)HCCT3)->tPort.u32PinCh1 = PORT_BIT7_MASK;

    /****** This pin is default mapped as TRACE_SWO / JTAG_TDO *******/
    /* HCCT3_CH1 - FTM0_CH7 - option 2: PTA2 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR2_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT3)->tPort.tPortCh1 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT3)->tPort.u32PinCh1 = PORT_BIT2_MASK;
}

void HCCT4_vPortRemap0(void)
{
    /* HCCT4_CH0 - FTM1_CH0 - option 1: PTA8 */
    SIM_SCGC5_PORTA = 1u;
    PORTA_PCR8_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh0 = HCCT_PORTA;
    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh0 = PORT_BIT8_MASK;

    /* HCCT4_CH0 - FTM1_CH0 - option 2: PTA12 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR12_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh0 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh0 = PORT_BIT12_MASK;

    /* HCCT4_CH0 - FTM1_CH0 - option 3: PTB0 */
//    SIM_SCGC5_PORTB = 1u;
//    PORTB_PCR0_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh0 = HCCT_PORTB;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh0 = PORT_BIT0_MASK;

    /* HCCT4_CH0 - FTM1_CH0 - option 4: PTD6 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR6_MUX = HCCT_ALTERNATE_FUNCTION_5;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh0 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh0 = PORT_BIT6_MASK;

    /* HCCT4_CH0 - FTM1_CH0 - option 5: PTE20 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR20_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh0 = PORT_BIT20_MASK;

    /****** This pin is default mapped as TRACE_SWO / JTAG_TDO *******/
    /* HCCT4_CH0 - FTM1_CH0 - option 6: PTA2 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR2_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh0 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh0 = PORT_BIT2_MASK;
}

void HCCT4_vPortRemap1(void)
{
    /* HCCT4_CH1 - FTM1_CH1 - option 1: PTA9 */
    SIM_SCGC5_PORTA = 1u;
    PORTA_PCR9_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh1 = HCCT_PORTA;
    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh1 = PORT_BIT9_MASK;

    /* HCCT4_CH1 - FTM1_CH1 - option 2: PTA13 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR13_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh1 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh1 = PORT_BIT13_MASK;

    /* HCCT4_CH1 - FTM1_CH1 - option 3: PTB1 */
//    SIM_SCGC5_PORTB = 1u;
//    PORTB_PCR1_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh1 = HCCT_PORTB;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh1 = PORT_BIT1_MASK;

    /* HCCT4_CH1 - FTM1_CH1 - option 4: PTD7 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR7_MUX = HCCT_ALTERNATE_FUNCTION_5;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh1 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh1 = PORT_BIT7_MASK;

    /* HCCT4_CH1 - FTM1_CH1 - option 5: PTE21 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR21_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh1 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh1 = PORT_BIT21_MASK;

    /****** This pin is default mapped as JTAG_TDI *******/
    /* HCCT4_CH1 - FTM1_CH1 - option 6: PTA1 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR1_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT4)->tPort.tPortCh1 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT4)->tPort.u32PinCh1 = PORT_BIT1_MASK;
}

void HCCT6_vPortRemap0(void)
{
    /* HCCT6_CH0 - FTM2_CH0 - option 1: PTA10 */
    SIM_SCGC5_PORTA = 1u;
    PORTA_PCR10_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT6)->tPort.tPortCh0 = HCCT_PORTA;
    ((HCCT_Module_t *)HCCT6)->tPort.u32PinCh0 = PORT_BIT10_MASK;

    /* HCCT6_CH0 - FTM2_CH0 - option 2: PTB18 */
//    SIM_SCGC5_PORTB = 1u;
//    PORTB_PCR18_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT6)->tPort.tPortCh0 = HCCT_PORTB;
//    ((HCCT_Module_t *)HCCT6)->tPort.u32PinCh0 = PORT_BIT18_MASK;

    /* HCCT6_CH0 - FTM2_CH0 - option 3: PTE22 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR22_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT6)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT6)->tPort.u32PinCh0 = PORT_BIT22_MASK;
}

void HCCT6_vPortRemap1(void)
{
    /* HCCT6_CH1 - FTM2_CH1 - option 1: PTA11 */
    SIM_SCGC5_PORTA = 1u;
    PORTA_PCR11_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT6)->tPort.tPortCh1 = HCCT_PORTA;
    ((HCCT_Module_t *)HCCT6)->tPort.u32PinCh1 = PORT_BIT11_MASK;

    /* HCCT6_CH1 - FTM2_CH1 - option 2: PTB19 */
//    SIM_SCGC5_PORTB = 1u;
//    PORTB_PCR19_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT6)->tPort.tPortCh1 = HCCT_PORTB;
//    ((HCCT_Module_t *)HCCT6)->tPort.u32PinCh1 = PORT_BIT19_MASK;

    /* HCCT6_CH1 - FTM2_CH1 - option 3: PTE23 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR23_MUX = HCCT_ALTERNATE_FUNCTION_3;
//    ((HCCT_Module_t *)HCCT6)->tPort.tPortCh1 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT6)->tPort.u32PinCh1 = PORT_BIT23_MASK;
}

void HCCT8_vPortRemap0(void)
{
    /* HCCT8_CH0 - FTM3_CH0 - option 1: PTD0 */
    SIM_SCGC5_PORTD = 1u;
    PORTD_PCR0_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT8)->tPort.tPortCh0 = HCCT_PORTD;
    ((HCCT_Module_t *)HCCT8)->tPort.u32PinCh0 = PORT_BIT0_MASK;

    /* HCCT8_CH0 - FTM3_CH0 - option 2: PTE5 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR5_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT8)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT8)->tPort.u32PinCh0 = PORT_BIT5_MASK;
}

void HCCT8_vPortRemap1(void)
{
    /* HCCT8_CH1 - FTM3_CH1 - option 1: PTD1 */
    SIM_SCGC5_PORTD = 1u;
    PORTD_PCR1_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT8)->tPort.tPortCh1 = HCCT_PORTD;
    ((HCCT_Module_t *)HCCT8)->tPort.u32PinCh1 = PORT_BIT1_MASK;

    /* HCCT8_CH1 - FTM3_CH1 - option 2: PTE6 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR6_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT8)->tPort.tPortCh1 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT8)->tPort.u32PinCh1 = PORT_BIT6_MASK;
}

void HCCT9_vPortRemap0(void)
{
    /* HCCT9_CH0 - FTM3_CH2 - option 1: PTB18 */
    SIM_SCGC5_PORTB = 1u;
    PORTB_PCR18_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT9)->tPort.tPortCh0 = HCCT_PORTB;
    ((HCCT_Module_t *)HCCT9)->tPort.u32PinCh0 = PORT_BIT18_MASK;

    /* HCCT9_CH0 - FTM3_CH2 - option 2: PTD2 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR2_MUX = HCCT_ALTERNATE_FUNCTION_4;
//    ((HCCT_Module_t *)HCCT9)->tPort.tPortCh0 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT9)->tPort.u32PinCh0 = PORT_BIT2_MASK;

    /* HCCT9_CH0 - FTM3_CH2 - option 3: PTE7 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR7_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT9)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT9)->tPort.u32PinCh0 = PORT_BIT7_MASK;

    /****** This pin is default mapped as EXTAL0 (oscillator) *******/
    /* HCCT9_CH0 - FTM3_CH2 - option 4: PTA18 */
//    SIM_SCGC5_PORTA = 1u;
//    PORTA_PCR18_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT9)->tPort.tPortCh0 = HCCT_PORTA;
//    ((HCCT_Module_t *)HCCT9)->tPort.u32PinCh0 = PORT_BIT18_MASK;
}

void HCCT9_vPortRemap1(void)
{
    /* HCCT9_CH1 - FTM3_CH3 - option 1: PTB19 */
    SIM_SCGC5_PORTB = 1u;
    PORTB_PCR19_MUX = HCCT_ALTERNATE_FUNCTION_4;
    ((HCCT_Module_t *)HCCT9)->tPort.tPortCh1 = HCCT_PORTB;
    ((HCCT_Module_t *)HCCT9)->tPort.u32PinCh1 = PORT_BIT19_MASK;

    /* HCCT9_CH1 - FTM3_CH3 - option 2: PTD3 */
//    SIM_SCGC5_PORTD = 1u;
//    PORTD_PCR3_MUX = HCCT_ALTERNATE_FUNCTION_4;
//    ((HCCT_Module_t *)HCCT9)->tPort.tPortCh1 = HCCT_PORTD;
//    ((HCCT_Module_t *)HCCT9)->tPort.u32PinCh1 = PORT_BIT3_MASK;

    /* HCCT9_CH1 - FTM3_CH3 - option 3: PTE8 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR8_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT9)->tPort.tPortCh1 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT9)->tPort.u32PinCh1 = PORT_BIT8_MASK;
}

void HCCT10_vPortRemap0(void)
{
    /* HCCT10_CH0 - FTM3_CH4 - option 1: PTC8 */
    SIM_SCGC5_PORTC = 1u;
    PORTC_PCR8_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT10)->tPort.tPortCh0 = HCCT_PORTC;
    ((HCCT_Module_t *)HCCT10)->tPort.u32PinCh0 = PORT_BIT8_MASK;

    /* HCCT10_CH0 - FTM3_CH4 - option 2: PTE9 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR9_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT10)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT10)->tPort.u32PinCh0 = PORT_BIT9_MASK;
}

void HCCT10_vPortRemap1(void)
{
    /* HCCT10_CH1 - FTM3_CH5 - option 1: PTC9 */
    SIM_SCGC5_PORTC = 1u;
    PORTC_PCR9_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT10)->tPort.tPortCh1 = HCCT_PORTC;
    ((HCCT_Module_t *)HCCT10)->tPort.u32PinCh1 = PORT_BIT9_MASK;

    /* HCCT10_CH1 - FTM3_CH5 - option 2: PTE10 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR10_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT10)->tPort.tPortCh1 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT10)->tPort.u32PinCh1 = PORT_BIT10_MASK;
}

void HCCT11_vPortRemap0(void)
{
    /* HCCT11_CH0 - FTM3_CH6 - option 1: PTC10 */
    SIM_SCGC5_PORTC = 1u;
    PORTC_PCR10_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT11)->tPort.tPortCh0 = HCCT_PORTC;
    ((HCCT_Module_t *)HCCT11)->tPort.u32PinCh0 = PORT_BIT10_MASK;

    /* HCCT11_CH0 - FTM3_CH6 - option 2: PTE11 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR11_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT11)->tPort.tPortCh0 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT11)->tPort.u32PinCh0 = PORT_BIT11_MASK;
}

void HCCT11_vPortRemap1(void)
{
    /* HCCT11_CH1 - FTM3_CH7 - option 1: PTC11 */
    SIM_SCGC5_PORTC = 1u;
    PORTC_PCR11_MUX = HCCT_ALTERNATE_FUNCTION_3;
    ((HCCT_Module_t *)HCCT11)->tPort.tPortCh1 = HCCT_PORTC;
    ((HCCT_Module_t *)HCCT11)->tPort.u32PinCh1 = PORT_BIT11_MASK;

    /* HCCT11_CH1 - FTM3_CH7 - option 2: PTE12 */
//    SIM_SCGC5_PORTE = 1u;
//    PORTE_PCR12_MUX = HCCT_ALTERNATE_FUNCTION_6;
//    ((HCCT_Module_t *)HCCT11)->tPort.tPortCh1 = HCCT_PORTE;
//    ((HCCT_Module_t *)HCCT11)->tPort.u32PinCh1 = PORT_BIT12_MASK;
}
/*lint -restore -e929 */
