/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          STM32G0 PED Framework
 *  COMP_ABBREV      HSPI
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of HSPI configuration template
 *
 *  \details  This module is interface between the HSPI HAL library and application.
 *
 */




/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hspi.h"
#include "hdio.h"


/******************************************************************************/
/* SPI Configuration structure */
/******************************************************************************/
/**
 *
 * \brief Configuration structure to set SPI unit\n
 *
 * \b description: \n
 * Please check header file hspi.h to proper configuring of this structure
 *
 *  - HSPI_CONFIG_COM_8BIT
 *  - HSPI_CONFIG_COM_MSB_FIRST
 *  - HSPI_CONFIG_COM_DATA_NOT_INVERTED
 *  - HSPI_CONFIG_COM_64MHZ_250KBAUD
 *  - HSPI_CONFIG_COM_CLOCK_HI_RISE
 *
 * \note nSS pin is used as normal output pin which is configured in this way and must be handled by user.
 *  By default is the pin falling to respective SPI unit as nSS !
 */
const T_HSPI_ConfigCom HSPI0_atConfigCom []  =
{
        {HSPI_DATA_8_BITS,
         HSPI_CONFIG_COM_MSB_FIRST,
         HSPI_CONFIG_COM_DATA_NOT_INVERTED,
         HSPI_CONFIG_COM_64MHZ_250KBAUD,
         HSPI_CONFIG_COM_CLOCK_HI_RISE }
};

const T_HSPI_ConfigCom HSPI1_atConfigCom []  =
{
        {HSPI_DATA_8_BITS,
         HSPI_CONFIG_COM_MSB_FIRST,
         HSPI_CONFIG_COM_DATA_NOT_INVERTED,
         HSPI_CONFIG_COM_64MHZ_250KBAUD,
         HSPI_CONFIG_COM_CLOCK_HI_RISE }
};

const T_HSPI_ConfigCom HSPI2_atConfigCom []  =
{
        {HSPI_DATA_8_BITS,
         HSPI_CONFIG_COM_MSB_FIRST,
         HSPI_CONFIG_COM_DATA_NOT_INVERTED,
         HSPI_CONFIG_COM_64MHZ_250KBAUD,
         HSPI_CONFIG_COM_CLOCK_HI_RISE }
};

/* DEFINITIONS ****************************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#ifdef HSPI0_PRESENT
void HSPI0_vHandleTxEvent(void)
{

}

void HSPI0_vHandleRxEvent(void)
{

}
#endif

#ifdef HSPI1_PRESENT
void HSPI1_vHandleTxEvent(void)
{

}

void HSPI1_vHandleRxEvent(void)
{

}
#endif

#ifdef HSPI2_PRESENT
void HSPI2_vHandleTxEvent(void)
{

}

void HSPI2_vHandleRxEvent(void)
{

}
#endif


/* Select your desired port mapping configuration by uncommenting respective lines.
   This remap here is for STM32G0 derivatives */
#ifdef HSPI0_PRESENT
void HSPI0_vPortRemap(void)
{
    /* SELECT ONE OF CLOCK OPTIONS */
    /* SPI1_SCK - option 1: PA1 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA1(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA1(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA1(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA1(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI1_SCK - option 2: PA5 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA5(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA5(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA5(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_SCK - option 3: PB3 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB3(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB3(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortB3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB3(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_SCK - option 4: PD8 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD8(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionD8(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortD8(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD8(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_SCK - option 5: PE13 */
/*  HDIO_vEnablePortE();
    HDIO_vSetModePortE13(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionE13(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortE13(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE13(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF SLAVE SELECT OPTIONS */
    /* SPI1_NSS - option 1: PA4 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA4(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortA4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA4(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortA4(HDIO_PUSH_PULL);

    /* SPI1_NSS - option 2: PA15 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA15(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortA15(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA15(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortA15(HDIO_PUSH_PULL);  */

    /* SPI1_NSS - option 3: PB0 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB0(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortB0(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB0(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortB0(HDIO_PUSH_PULL);  */

    /* SPI1_NSS - option 4: PD9 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD9(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortD9(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD9(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortD9(HDIO_PUSH_PULL);  */

    /* SPI1_NSS - option 5: PE12 */
/*  HDIO_vEnablePortE();
    HDIO_vSetModePortE12(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortE12(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE12(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortE12(HDIO_PUSH_PULL);  */

    /* SELECT ONE OF MISO OPTIONS */
    /* SPI1_MISO - option 1: PA6 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA6(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA6(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA6(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA6(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI1_MISO - option 2: PA11 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA11(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA11(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA11(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA11(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_MISO - option 3: PB4 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB4(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB4(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortB4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB4(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_MISO - option 4: PD5 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD5(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionD5(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortD5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD5(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF MOSI OPTIONS */
    /* SPI1_MOSI - option 1: PA2 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA2(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA2(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA2(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA2(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI1_MOSI - option 2: PA7 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA7(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA7(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA7(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA7(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_MOSI - option 3: PA12 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA12(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA12(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA12(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA12(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_MOSI - option 4: PB5 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB5(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB5(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortB5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB5(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_MOSI - option 5: PD6 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD6(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionD6(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortD6(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD6(HDIO_OUTPUT_MEDIUM_SPEED);  */

}
#endif

#ifdef HSPI1_PRESENT
void HSPI1_vPortRemap(void)
{
    /* SELECT ONE OF CLOCK OPTIONS */
    /* SPI2_SCK - option 1: PA0 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA0(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA0(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA0(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA0(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_SCK - option 2: PB8 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB8(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB8(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortB8(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB8(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI2_SCK - option 3: PB10 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB10(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB10(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortB10(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB10(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_SCK - option 4: PB13 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB13(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB13(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortB13(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB13(HDIO_OUTPUT_MEDIUM_SPEED); */

    /* SPI2_SCK - option 5: PD1 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD1(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionD1(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortD1(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD1(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF SLAVE SELECT OPTIONS */
    /* SPI2_NSS - option 1: PA8 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA8(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortA8(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA8(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortA8(HDIO_PUSH_PULL);  */

    /* SPI2_NSS - option 2: PB9 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB9(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortB9(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB9(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortB9(HDIO_PUSH_PULL);

    /* SPI2_NSS - option 3: PB12 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB12(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortB12(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB12(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortB12(HDIO_PUSH_PULL);  */

    /* SPI2_NSS - option 4: PD0 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD0(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortD0(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD0(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortD0(HDIO_PUSH_PULL);  */

    /* SELECT ONE OF MISO OPTIONS */
    /* SPI2_MISO - option 1: PA3 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA3(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA3(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA3(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MISO - option 2: PA9 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA9(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA9(HDIO_ALTERNATE_FUNCTION_4);
    HDIO_vSetPullUpPullDownPortA9(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA9(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MISO - option 3: PB2 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB2(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB2(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortB2(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB2(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MISO - option 4: PB6 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB6(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB6(HDIO_ALTERNATE_FUNCTION_4);
    HDIO_vSetPullUpPullDownPortB6(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB6(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI2_MISO - option 5: PB14 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB14(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB14(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortB14(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB14(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MISO - option 6: PC2 */
/*  HDIO_vEnablePortC();
    HDIO_vSetModePortC2(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionC2(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortC2(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortC2(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MISO - option 7: PD3 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD3(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionD3(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortD3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD3(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF MOSI OPTIONS */
    /* SPI2_MOSI - option 1: PA4 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA4(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA4(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortA4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA4(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MOSI - option 2: PA10 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA10(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA10(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortA10(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA10(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MOSI - option 3: PB7 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB7(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB7(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortB7(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB7(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI2_MOSI - option 4: PB11 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB11(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB11(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortB11(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB11(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MOSI - option 5: PB15 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB15(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB15(HDIO_ALTERNATE_FUNCTION_0);
    HDIO_vSetPullUpPullDownPortB15(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB15(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MOSI - option 6: PC3 */
/*  HDIO_vEnablePortC();
    HDIO_vSetModePortC3(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionC3(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortC3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortC3(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_MOSI - option 7: PD4 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD4(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionD4(HDIO_ALTERNATE_FUNCTION_1);
    HDIO_vSetPullUpPullDownPortD4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD4(HDIO_OUTPUT_MEDIUM_SPEED);  */

}
#endif

#ifdef HSPI2_PRESENT
void HSPI2_vPortRemap(void)
{
    /* SELECT ONE OF CLOCK OPTIONS */
    /* SPI3_SCK - option 1: PB3 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB3(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB3(HDIO_ALTERNATE_FUNCTION_9);
    HDIO_vSetPullUpPullDownPortB3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB3(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI3_SCK - option 2: PC10 */
/*  HDIO_vEnablePortC();
    HDIO_vSetModePortC10(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionC10(HDIO_ALTERNATE_FUNCTION_4);
    HDIO_vSetPullUpPullDownPortC10(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortC10(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF SLAVE SELECT OPTIONS */
    /* SPI3_NSS - option 1: PA4 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA4(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortA4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA4(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortA4(HDIO_PUSH_PULL);

    /* SPI3_NSS - option 2: PA15 */
/*  HDIO_vEnablePortA();
    HDIO_vSetModePortA15(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortA15(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA15(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortA15(HDIO_PUSH_PULL);  */

    /* SELECT ONE OF MISO OPTIONS */
    /* SPI3_MISO - option 1: PB4 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB4(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB4(HDIO_ALTERNATE_FUNCTION_9);
    HDIO_vSetPullUpPullDownPortB4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB4(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI3_MISO - option 2: PC11 */
/*  HDIO_vEnablePortC();
    HDIO_vSetModePortC11(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionC11(HDIO_ALTERNATE_FUNCTION_4);
    HDIO_vSetPullUpPullDownPortC11(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortC11(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF MOSI OPTIONS */
    /* SPI3_MOSI - option 1: PB5 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB5(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB5(HDIO_ALTERNATE_FUNCTION_9);
    HDIO_vSetPullUpPullDownPortB5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB5(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI3_MOSI - option 2: PC12 */
/*  HDIO_vEnablePortC();
    HDIO_vSetModePortC12(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionC12(HDIO_ALTERNATE_FUNCTION_4);
    HDIO_vSetPullUpPullDownPortC12(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortC12(HDIO_OUTPUT_MEDIUM_SPEED);  */

}
#endif
