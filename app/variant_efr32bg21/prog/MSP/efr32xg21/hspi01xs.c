/*******************************************************************************
 *  Copyright (c) 2020 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          EFR32 HAL PED Framework
 *  COMP_ABBREV      HSPI
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief    This module is interface between the HSPI library and application.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hspi.h"
#include "hdio.h"

/* CONSTANTS ******************************************************************************************/
/* DEFINITIONS ****************************************************************************************/
/*
\b description: \n
 These definitions are used to setup the communication parameters in the HSPI<unit>_atConfigCom.
 It sets the baudrate for the transmission/reception in Hz.
\note
 When the USART operates in master mode, the highest possible bit rate is half the peripheral clock rate.
 BAUDRATE_MAX = fPCLK / 2
*/
#define HSPI0_CONFIG_COM_BAUD    (1000000u) // SPI_SCK will generate frequency 1MHz
#define HSPI1_CONFIG_COM_BAUD    (1000000u)
#define HSPI2_CONFIG_COM_BAUD    (1000000u)


/* Please note, that currently the HSPI definitions/configurations below are merely copies of HSPI0,
 to avoid problems with the linker... If used, please check the configurations! */
#ifdef HSPI0_PRESENT
const T_HSPI_ConfigCom HSPI0_atConfigCom []  = {{HSPI_CONFIG_COM_8BIT, HSPI_CONFIG_COM_MSB_FIRST, HSPI_CONFIG_COM_DATA_NOT_INVERTED, HSPI0_CONFIG_COM_BAUD, HSPI_CONFIG_COM_CLOCK_LO_RISE }};
#endif
#ifdef HSPI1_PRESENT
const T_HSPI_ConfigCom HSPI1_atConfigCom []  = {{HSPI_CONFIG_COM_8BIT, HSPI_CONFIG_COM_MSB_FIRST, HSPI_CONFIG_COM_DATA_NOT_INVERTED, HSPI1_CONFIG_COM_BAUD, HSPI_CONFIG_COM_CLOCK_LO_RISE }};
#endif
#ifdef HSPI2_PRESENT
const T_HSPI_ConfigCom HSPI2_atConfigCom []  = {{HSPI_CONFIG_COM_8BIT, HSPI_CONFIG_COM_MSB_FIRST, HSPI_CONFIG_COM_DATA_NOT_INVERTED, HSPI2_CONFIG_COM_BAUD, HSPI_CONFIG_COM_CLOCK_LO_RISE }};
#endif

/* FUNCTIONS ******************************************************************************************/

void HSPI0_vHandleTxEvent(void)
{

}

void HSPI1_vHandleTxEvent(void)
{

}

void HSPI2_vHandleTxEvent(void)
{

}


void HSPI0_vHandleRxEvent(void)
{

}

void HSPI1_vHandleRxEvent(void)
{

}

void HSPI2_vHandleRxEvent(void)
{

}


/* Select your desired port mapping configuration by modification of parameters */

/*
Note:
 * HSPI0 can be mapping to any pin combination for ports PA, PB, PC and PD.
Warning:
 * Pins PA1, PA2, PA3 and PA4 are connect to debugger. Be careful when configuring these pins!
 */
void HSPI0_vPortRemap(void)
{
    /* SPI_SCK - Pin clock */
    HDIO_vSetMode          (HDIOC, HDIO_PIN2, HDIO_OUTPUT, NULL);
    HDIO_vSetModeHUART0_CLK(HDIOC, HDIO_PIN2, HDIO_ALTERNATE_FUNCTION);
    /* SPI_MOSI - Pin master output slave input */
    HDIO_vSetMode          (HDIOC, HDIO_PIN0, HDIO_OUTPUT, NULL);
    HDIO_vSetModeHUART0_TX (HDIOC, HDIO_PIN0, HDIO_ALTERNATE_FUNCTION);
    /* SPI_MISO - Pin master input slave output */
    HDIO_vSetMode          (HDIOC, HDIO_PIN1, HDIO_INPUT, NULL);
    HDIO_vSetModeHUART0_RX (HDIOC, HDIO_PIN1, HDIO_ALTERNATE_FUNCTION);
    /* SPI_CS - Pin chip select */
    HDIO_vSetMode          (HDIOC, HDIO_PIN3, HDIO_OUTPUT, NULL);
    HDIO_vSetPortC3(HDIO_ON);
}

/*
Note:
 * HSPI1 can be mapping to any pin combination for ports PA and PB.
Warning:
 * Pins PA1, PA2, PA3 and PA4 are connect to debugger. Be careful when configuring these pins!
 */
void HSPI1_vPortRemap(void)
{
    /* SPI_SCK - Pin clock */
    HDIO_vSetMode          (HDIOA, HDIO_PIN0, HDIO_OUTPUT, NULL);
    HDIO_vSetModeHUART1_CLK(HDIOA, HDIO_PIN0, HDIO_ALTERNATE_FUNCTION);
    /* SPI_MOSI - Pin master output slave input */
    HDIO_vSetMode          (HDIOA, HDIO_PIN5, HDIO_OUTPUT, NULL);
    HDIO_vSetModeHUART1_TX (HDIOA, HDIO_PIN5, HDIO_ALTERNATE_FUNCTION);
    /* SPI_MISO - Pin master input slave output */
    HDIO_vSetMode          (HDIOA, HDIO_PIN6, HDIO_INPUT, NULL);
    HDIO_vSetModeHUART1_RX (HDIOA, HDIO_PIN6, HDIO_ALTERNATE_FUNCTION);
    /* SPI_CS - Pin chip select */
    HDIO_vSetMode          (HDIOC, HDIO_PIN3, HDIO_OUTPUT, NULL);
    HDIO_vSetPortC3(HDIO_ON);
}

/*
Note:
 * HSPI2 can be mapping to any pin combination for ports PC and PD.
 */
void HSPI2_vPortRemap(void)
{   /* +++++ Default setting for BTM external flash !! +++++ */
    /* SPI_SCK - Pin clock */
    HDIO_vSetMode          (HDIOC, HDIO_PIN3, HDIO_OUTPUT, NULL);
    HDIO_vSetModeHUART2_CLK(HDIOC, HDIO_PIN3, HDIO_ALTERNATE_FUNCTION);
    /* SPI_MOSI - Pin master output slave input */
    HDIO_vSetMode          (HDIOC, HDIO_PIN5, HDIO_OUTPUT, NULL);
    HDIO_vSetModeHUART2_TX (HDIOC, HDIO_PIN5, HDIO_ALTERNATE_FUNCTION);
    /* SPI_MISO - Pin master input slave output */
    HDIO_vSetMode          (HDIOC, HDIO_PIN4, HDIO_INPUT, NULL);
    HDIO_vSetModeHUART2_RX (HDIOC, HDIO_PIN4, HDIO_ALTERNATE_FUNCTION);
    /* SPI_CS - Pin chip select */
    HDIO_vSetMode          (HDIOC, HDIO_PIN0, HDIO_OUTPUT, NULL);
    HDIO_vSetPortC0(HDIO_ON);
}
