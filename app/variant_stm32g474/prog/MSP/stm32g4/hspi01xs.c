/*******************************************************************************
 *  Copyright (c) 2021 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          STM32G4 PED Framework
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
         HSPI_CONFIG_COM_170MHZ_664_0625KBAUD,
         HSPI_CONFIG_COM_CLOCK_HI_RISE }
};

const T_HSPI_ConfigCom HSPI1_atConfigCom []  =
{
        {HSPI_DATA_8_BITS,
         HSPI_CONFIG_COM_MSB_FIRST,
         HSPI_CONFIG_COM_DATA_NOT_INVERTED,
         HSPI_CONFIG_COM_170MHZ_664_0625KBAUD,
         HSPI_CONFIG_COM_CLOCK_HI_RISE }
};

const T_HSPI_ConfigCom HSPI2_atConfigCom []  =
{
        {HSPI_DATA_8_BITS,
         HSPI_CONFIG_COM_MSB_FIRST,
         HSPI_CONFIG_COM_DATA_NOT_INVERTED,
         HSPI_CONFIG_COM_170MHZ_664_0625KBAUD,
         HSPI_CONFIG_COM_CLOCK_HI_RISE }
};

const T_HSPI_ConfigCom HSPI3_atConfigCom []  =
{
        {HSPI_DATA_8_BITS,
         HSPI_CONFIG_COM_MSB_FIRST,
         HSPI_CONFIG_COM_DATA_NOT_INVERTED,
         HSPI_CONFIG_COM_170MHZ_664_0625KBAUD,
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

#ifdef HSPI3_PRESENT
void HSPI3_vHandleTxEvent(void)
{

}

void HSPI3_vHandleRxEvent(void)
{

}
#endif


/* Select your desired port mapping configuration by uncommenting respective lines.
   This remap here is for STM32G0 derivatives */
#ifdef HSPI0_PRESENT
void HSPI0_vPortRemap(void)
{
    /* SELECT ONE OF CLOCK OPTIONS */
    /* SPI1_SCK - option 1: PA5 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA5(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA5(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortA5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA5(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI1_SCK - option 2: PB3 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB3(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB3(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortB3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB3(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_SCK - option 3: PG2 */
/*  HDIO_vEnablePortG();
    HDIO_vSetModePortG2(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionG2(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortG2(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortG2(HDIO_OUTPUT_MEDIUM_SPEED);  */

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

    /* SPI1_NSS - option 3: PG5 */
/*  HDIO_vEnablePortG();
    HDIO_vSetModePortG5(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortG5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortG5(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortG5(HDIO_PUSH_PULL);  */

    /* SELECT ONE OF MISO OPTIONS */
    /* SPI1_MISO - option 1: PA6 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA6(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA6(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortA6(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA6(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI1_MISO - option 2: PB4 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB4(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB4(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortB4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB4(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_MISO - option 3: PG3 */
/*  HDIO_vEnablePortG();
    HDIO_vSetModePortG3(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionG3(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortG3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortG3(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF MOSI OPTIONS */
    /* SPI1_MOSI - option 1: PA7 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA7(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA7(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortA7(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA7(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI1_MOSI - option 2: PB5 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB5(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB5(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortB5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB5(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI1_MOSI - option 3: PG4 */
/*  HDIO_vEnablePortG();
    HDIO_vSetModePortG4(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionG4(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortG4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortG4(HDIO_OUTPUT_MEDIUM_SPEED);  */
}
#endif

#ifdef HSPI1_PRESENT
void HSPI1_vPortRemap(void)
{
    /* SELECT ONE OF CLOCK OPTIONS */
    /* SPI2_SCK - option 1: PB13 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB13(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB13(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortB13(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB13(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI2_SCK - option 2: PF1 */
/*  HDIO_vEnablePortF();
    HDIO_vSetModePortF1(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionF1(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortF1(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortF1(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_SCK - option 3: PF9 */
/*  HDIO_vEnablePortF();
    HDIO_vSetModePortF9(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionF9(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortF9(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortF9(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI2_SCK - option 4: PF10 */
/*  HDIO_vEnablePortF();
    HDIO_vSetModePortF10(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionF10(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortF10(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortF10(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF SLAVE SELECT OPTIONS */
    /* SPI2_NSS - option 1: PB12 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB12(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortB12(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB12(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortB12(HDIO_PUSH_PULL);

    /* SPI2_NSS - option 2: PD15 */
/*  HDIO_vEnablePortD();
    HDIO_vSetModePortD15(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortD15(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortD15(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortD15(HDIO_PUSH_PULL);  */

    /* SPI2_NSS - option 3: PF0 */
/*  HDIO_vEnablePortF();
    HDIO_vSetModePortF0(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortF0(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortF0(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortF0(HDIO_PUSH_PULL);  */

    /* SELECT ONE OF MISO OPTIONS */
    /* SPI2_MISO - option 1: PA10 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA10(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA10(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortA10(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA10(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI2_MISO - option 2: PB14 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB14(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB14(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortB14(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB14(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF MOSI OPTIONS */
    /* SPI2_MOSI - option 1: PA11 */
    HDIO_vEnablePortA();
    HDIO_vSetModePortA11(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionA11(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortA11(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortA11(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI2_MOSI - option 2: PB15 */
/*  HDIO_vEnablePortB();
    HDIO_vSetModePortB15(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB15(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortB15(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB15(HDIO_OUTPUT_MEDIUM_SPEED);  */
}
#endif

#ifdef HSPI2_PRESENT
void HSPI2_vPortRemap(void)
{
    /* SELECT ONE OF CLOCK OPTIONS */
    /* SPI3_SCK - option 1: PB3 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB3(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB3(HDIO_ALTERNATE_FUNCTION_6);
    HDIO_vSetPullUpPullDownPortB3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB3(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI3_SCK - option 2: PC10 */
/*  HDIO_vEnablePortC();
    HDIO_vSetModePortC10(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionC10(HDIO_ALTERNATE_FUNCTION_6);
    HDIO_vSetPullUpPullDownPortC10(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortC10(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SPI3_SCK - option 3: PG9 */
/*  HDIO_vEnablePortG();
    HDIO_vSetModePortG9(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionG9(HDIO_ALTERNATE_FUNCTION_6);
    HDIO_vSetPullUpPullDownPortG9(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortG9(HDIO_OUTPUT_MEDIUM_SPEED);  */

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
    HDIO_vSetAlternateFunctionB4(HDIO_ALTERNATE_FUNCTION_6);
    HDIO_vSetPullUpPullDownPortB4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB4(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI3_MISO - option 2: PC11 */
/*  HDIO_vEnablePortC();
    HDIO_vSetModePortC11(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionC11(HDIO_ALTERNATE_FUNCTION_6);
    HDIO_vSetPullUpPullDownPortC11(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortC11(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF MOSI OPTIONS */
    /* SPI3_MOSI - option 1: PB5 */
    HDIO_vEnablePortB();
    HDIO_vSetModePortB5(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionB5(HDIO_ALTERNATE_FUNCTION_6);
    HDIO_vSetPullUpPullDownPortB5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortB5(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI3_MOSI - option 2: PC12 */
/*  HDIO_vEnablePortC();
    HDIO_vSetModePortC12(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionC12(HDIO_ALTERNATE_FUNCTION_6);
    HDIO_vSetPullUpPullDownPortC12(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortC12(HDIO_OUTPUT_MEDIUM_SPEED);  */
}
#endif

#ifdef HSPI3_PRESENT
void HSPI3_vPortRemap(void)
{
    /* SELECT ONE OF CLOCK OPTIONS */
    /* SPI4_SCK - option 1: PE2 */
    HDIO_vEnablePortE();
    HDIO_vSetModePortE2(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionE2(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortE2(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE2(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI4_SCK - option 2: PE12 */
/*  HDIO_vEnablePortE();
    HDIO_vSetModePortE12(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionE12(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortE12(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE12(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF SLAVE SELECT OPTIONS */
    /* SPI4_NSS - option 1: PE3 */
    HDIO_vEnablePortE();
    HDIO_vSetModePortE3(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortE3(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE3(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortE3(HDIO_PUSH_PULL);

    /* SPI4_NSS - option 2: PE4 */
/*  HDIO_vEnablePortE();
    HDIO_vSetModePortE4(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortE4(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE4(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortE4(HDIO_PUSH_PULL);  */

    /* SPI4_NSS - option 3: PE11 */
/*  HDIO_vEnablePortE();
    HDIO_vSetModePortE11(HDIO_OUTPUT);
    HDIO_vSetPullUpPullDownPortE11(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE11(HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetOpenDrainPushPullPortE11(HDIO_PUSH_PULL);  */

    /* SELECT ONE OF MISO OPTIONS */
    /* SPI4_MISO - option 1: PE5 */
    HDIO_vEnablePortE();
    HDIO_vSetModePortE5(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionE5(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortE5(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE5(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI4_MISO - option 2: PE13 */
/*  HDIO_vEnablePortE();
    HDIO_vSetModePortE13(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionE13(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortE13(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE13(HDIO_OUTPUT_MEDIUM_SPEED);  */

    /* SELECT ONE OF MOSI OPTIONS */
    /* SPI4_MOSI - option 1: PE6 */
    HDIO_vEnablePortE();
    HDIO_vSetModePortE6(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionE6(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortE6(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE6(HDIO_OUTPUT_MEDIUM_SPEED);

    /* SPI4_MOSI - option 2: PE14 */
/*  HDIO_vEnablePortE();
    HDIO_vSetModePortE14(HDIO_ALTERNATE_FUNCTION);
    HDIO_vSetAlternateFunctionE14(HDIO_ALTERNATE_FUNCTION_5);
    HDIO_vSetPullUpPullDownPortE14(HDIO_PULL_NONE);
    HDIO_vSetSpeedPortE14(HDIO_OUTPUT_MEDIUM_SPEED);  */
}
#endif
