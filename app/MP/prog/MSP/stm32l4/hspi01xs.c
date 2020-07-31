/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          STM32L4 HAL PED Framework
 *  COMP_ABBREV      HSPI
 *  AUTHOR           Pusztai Pavol
 *  CREATED          22.02.2017
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

/* CONSTANTS ******************************************************************************************/

/* Please note, that currently the HSPI definitions/configurations below are merely copies of HSPI0,
 to avoid problems with the linker... If used, please check the configurations! */
#ifdef HSPI0_PRESENT
const T_HSPI_ConfigCom HSPI0_atConfigCom []  = {{HSPI_CONFIG_COM_TX_8BIT, HSPI_CONFIG_COM_RX_8BIT, HSPI_CONFIG_COM_LSB_FIRST, HSPI_CONFIG_COM_DATA_NOT_INVERTED, HSPI_CONFIG_COM_80MHZ_625KBAUD, HSPI_CONFIG_COM_CLOCK_LO_RISE }}; // HSPI0 (STm naming SPI1) is connected to APB2 clock (maximum clock is 80MHz for APB2)
#endif
#ifdef HSPI1_PRESENT
const T_HSPI_ConfigCom HSPI1_atConfigCom []  = {{HSPI_CONFIG_COM_TX_8BIT, HSPI_CONFIG_COM_RX_8BIT, HSPI_CONFIG_COM_LSB_FIRST, HSPI_CONFIG_COM_DATA_NOT_INVERTED, HSPI_CONFIG_COM_80MHZ_625KBAUD, HSPI_CONFIG_COM_CLOCK_LO_RISE }}; // HSPI1 (STm naming SPI2) is connected to APB1 clock (maximum clock is 80MHz for APB1)
#endif
#ifdef HSPI2_PRESENT
const T_HSPI_ConfigCom HSPI2_atConfigCom []  = {{HSPI_CONFIG_COM_TX_8BIT, HSPI_CONFIG_COM_RX_8BIT, HSPI_CONFIG_COM_LSB_FIRST, HSPI_CONFIG_COM_DATA_NOT_INVERTED, HSPI_CONFIG_COM_80MHZ_625KBAUD, HSPI_CONFIG_COM_CLOCK_LO_RISE }}; // HSPI2 (STm naming SPI3) is connected to APB1 clock (maximum clock is 80MHz for APB1)
#endif

/* DEFINITIONS ****************************************************************************************/
/*lint -esym(750,*MODE,IO*,ALTERNATE_*) "local macro not referenced [MISRA 2012 Rule 2.5, advisory]" */
#define INPUT_MODE                  0x0
#define OUTPUT_MODE                 0x1
#define ALTERNATE_FUNCTION_MODE     0x2
#define ANALOG_MODE                 0x3

#define ALTERNATE_FUNCTION_0        0x0
#define ALTERNATE_FUNCTION_1        0x1
#define ALTERNATE_FUNCTION_2        0x2
#define ALTERNATE_FUNCTION_3        0x3
#define ALTERNATE_FUNCTION_4        0x4
#define ALTERNATE_FUNCTION_5        0x5
#define ALTERNATE_FUNCTION_6        0x6

#define IO_LOW_SPEED                0x0
#define IO_MEDIUM_SPEED             0x1
#define IO_HIGH_SPEED               0x3

#define IO_PULL_NONE                0x0
#define IO_PULL_UP                  0x1
#define IO_PULL_DOWN                0x2


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


/* Select your desired port mapping configuration by uncommenting respective lines */

void HSPI0_vPortRemap()
{
   /* SPI1_SCK - option 1: PB3 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER3  =    ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRL_AFRL3    =    ALTERNATE_FUNCTION_5;
   GPIOB_PUPDR_PUPDR3  =    IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR3 = IO_MEDIUM_SPEED;

    /* SPI1_SCK - option 2: PA5 */
/* RCC_AHB2ENR_GPIOAEN = 1;
   GPIOA_MODER_MODER5  =    ALTERNATE_FUNCTION_MODE;
   GPIOA_AFRL_AFRL5    =    ALTERNATE_FUNCTION_5;
   GPIOA_PUPDR_PUPDR5  =    IO_PULL_DOWN;
   GPIOA_OSPEEDR_OSPEEDR5 = IO_MEDIUM_SPEED; */

   /* SPI1_SCK - option 4: PA1 */
/* RCC_AHB2ENR_GPIOAEN = 1;
   GPIOA_MODER_MODER1  =    ALTERNATE_FUNCTION_MODE;
   GPIOA_AFRL_AFRL1    =    ALTERNATE_FUNCTION_5;
   GPIOA_PUPDR_PUPDR1  =    IO_PULL_DOWN;
   GPIOA_OSPEEDR_OSPEEDR1 = IO_MEDIUM_SPEED; */

   /* SPI1_SCK - option 3: PE13 */
/* RCC_AHB2ENR_GPIOEEN = 1;
   GPIOE_MODER_MODER13 =     ALTERNATE_FUNCTION_MODE;
   GPIOE_AFRH_AFRH13   =     ALTERNATE_FUNCTION_5;
   GPIOE_PUPDR_PUPDR13 =     IO_PULL_DOWN;
   GPIOE_OSPEEDR_OSPEEDR13 = IO_MEDIUM_SPEED; */


   /* SPI1_MISO - option 1: PB4 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER4  =    ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRL_AFRL4    =    ALTERNATE_FUNCTION_5;
   GPIOB_PUPDR_PUPDR4  =    IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR4 = IO_MEDIUM_SPEED;

   /* SPI1_MISO - option 2: PA6 */
/* RCC_AHB2ENR_GPIOAEN = 1;
   GPIOA_MODER_MODER6  =    ALTERNATE_FUNCTION_MODE;
   GPIOA_AFRL_AFRL6    =    ALTERNATE_FUNCTION_5;
   GPIOA_PUPDR_PUPDR6  =    IO_PULL_DOWN;
   GPIOA_OSPEEDR_OSPEEDR6 = IO_MEDIUM_SPEED; */

   /* SPI1_MISO - option 3: PA11 */
/* RCC_AHB2ENR_GPIOAEN = 1;
   GPIOA_MODER_MODER11 =     ALTERNATE_FUNCTION_MODE;
   GPIOA_AFRH_AFRH11   =     ALTERNATE_FUNCTION_5;
   GPIOA_PUPDR_PUPDR11 =     IO_PULL_DOWN;
   GPIOA_OSPEEDR_OSPEEDR11 = IO_MEDIUM_SPEED; */

   /* SPI1_MISO - option 4: PE14 */
/* RCC_AHB2ENR_GPIOEEN = 1;
   GPIOE_MODER_MODER14 =     ALTERNATE_FUNCTION_MODE;
   GPIOE_AFRH_AFRH14   =     ALTERNATE_FUNCTION_5;
   GPIOE_PUPDR_PUPDR14 =     IO_PULL_DOWN;
   GPIOE_OSPEEDR_OSPEEDR14 = IO_MEDIUM_SPEED; */

   /* SPI1_MOSI - option 1: PB5 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER5  =    ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRL_AFRL5    =    ALTERNATE_FUNCTION_5;
   GPIOB_PUPDR_PUPDR5  =    IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR5 = IO_MEDIUM_SPEED;

   /* SPI1_MOSI - option 2: PA7 */
/* RCC_AHB2ENR_GPIOAEN = 1;
   GPIOA_MODER_MODER7  =    ALTERNATE_FUNCTION_MODE;
   GPIOA_AFRL_AFRL7    =    ALTERNATE_FUNCTION_5;
   GPIOA_PUPDR_PUPDR7  =    IO_PULL_DOWN;
   GPIOA_OSPEEDR_OSPEEDR7 = IO_MEDIUM_SPEED; */

   /* SPI1_MOSI - option 3: PA12 */
/* RCC_AHB2ENR_GPIOAEN = 1;
   GPIOA_MODER_MODER12 =     ALTERNATE_FUNCTION_MODE;
   GPIOA_AFRH_AFRH12   =     ALTERNATE_FUNCTION_5;
   GPIOA_PUPDR_PUPDR12 =     IO_PULL_DOWN;
   GPIOA_OSPEEDR_OSPEEDR12 = IO_MEDIUM_SPEED; */
    
   /* SPI1_MOSI - option 4: PE15 */
/* RCC_AHB2ENR_GPIOEEN = 1;
   GPIOE_MODER_MODER15 =     ALTERNATE_FUNCTION_MODE;
   GPIOE_AFRH_AFRH15   =     ALTERNATE_FUNCTION_5;
   GPIOE_PUPDR_PUPDR15 =     IO_PULL_DOWN;
   GPIOE_OSPEEDR_OSPEEDR15 = IO_MEDIUM_SPEED; */
}


void HSPI1_vPortRemap()
{
   /* SPI2_SCK - option 1: PB13 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER13 =     ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRH_AFRH13   =     ALTERNATE_FUNCTION_5;
   GPIOB_PUPDR_PUPDR13 =     IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR13 = IO_MEDIUM_SPEED;    

   /* SPI2_SCK - option 2: PB10 */
/* RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER10 =     ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRH_AFRH10   =     ALTERNATE_FUNCTION_5;
   GPIOB_PUPDR_PUPDR10 =     IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR10 = IO_MEDIUM_SPEED; */

   /* SPI2_SCK - option 3: PD1 */
/* RCC_AHB2ENR_GPIODEN = 1;     
   GPIOD_MODER_MODER1  =    ALTERNATE_FUNCTION_MODE;
   GPIOD_AFRL_AFRL1    =    ALTERNATE_FUNCTION_5;
   GPIOD_PUPDR_PUPDR1  =    IO_PULL_DOWN;
   GPIOD_OSPEEDR_OSPEEDR1 = IO_MEDIUM_SPEED; */


   /* SPI2_MISO - option 1: PB14 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER14 =     ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRH_AFRH14   =     ALTERNATE_FUNCTION_5;
   GPIOB_PUPDR_PUPDR14 =     IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR14 = IO_MEDIUM_SPEED;   

   /* SPI2_MISO - option 2: PC2 */
/* RCC_AHB2ENR_GPIOCEN = 1;
   GPIOC_MODER_MODER2  =    ALTERNATE_FUNCTION_MODE;
   GPIOC_AFRL_AFRL2    =    ALTERNATE_FUNCTION_5;
   GPIOC_PUPDR_PUPDR2  =    IO_PULL_DOWN;
   GPIOC_OSPEEDR_OSPEEDR2 = IO_MEDIUM_SPEED; */

   /* SPI2_MISO - option 3: PD3 */
/* RCC_AHB2ENR_GPIODEN = 1;        
   GPIOD_MODER_MODER3  =    ALTERNATE_FUNCTION_MODE;     
   GPIOD_AFRL_AFRL3    =    ALTERNATE_FUNCTION_5;
   GPIOD_PUPDR_PUPDR3  =    IO_PULL_DOWN;
   GPIOD_OSPEEDR_OSPEEDR3 = IO_MEDIUM_SPEED; */

   
   /* SPI2_MOSI - option 1: PB15 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER15 =     ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRH_AFRH15   =     ALTERNATE_FUNCTION_5;
   GPIOB_PUPDR_PUPDR15 =     IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR15 = IO_MEDIUM_SPEED; 

   /* SPI2_MOSI - option 2: PC3 */
/* RCC_AHB2ENR_GPIOCEN = 1;
   GPIOC_MODER_MODER3  =    ALTERNATE_FUNCTION_MODE;
   GPIOC_AFRL_AFRL3    =    ALTERNATE_FUNCTION_5;
   GPIOC_PUPDR_PUPDR3  =    IO_PULL_DOWN;
   GPIOC_OSPEEDR_OSPEEDR3 = IO_MEDIUM_SPEED; */

   /* SPI2_MOSI - option 3: PD4 */
/* RCC_AHB2ENR_GPIODEN = 1;     
   GPIOD_MODER_MODER4  =    ALTERNATE_FUNCTION_MODE;
   GPIOD_AFRL_AFRL4    =    ALTERNATE_FUNCTION_5;
   GPIOD_PUPDR_PUPDR4  =    IO_PULL_DOWN;
   GPIOD_OSPEEDR_OSPEEDR4 = IO_MEDIUM_SPEED; */
}


void HSPI2_vPortRemap()
{
   /* SPI3_SCK - option 1: PB3 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER3  =    ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRL_AFRL3    =    ALTERNATE_FUNCTION_6;
   GPIOB_PUPDR_PUPDR3  =    IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR3 = IO_MEDIUM_SPEED;    

   /* SPI3_SCK - option 2: PC10 */
/* RCC_AHB2ENR_GPIOCEN = 1;
   GPIOC_MODER_MODER10 =     ALTERNATE_FUNCTION_MODE;
   GPIOC_AFRH_AFRH10   =     ALTERNATE_FUNCTION_6;
   GPIOC_PUPDR_PUPDR10 =     IO_PULL_DOWN;
   GPIOC_OSPEEDR_OSPEEDR10 = IO_MEDIUM_SPEED; */


   /* SPI3_MISO - option 1: PB4 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER4  =    ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRL_AFRL4    =    ALTERNATE_FUNCTION_6;
   GPIOB_PUPDR_PUPDR4  =    IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR4 = IO_MEDIUM_SPEED;   

   /* SPI3_MISO - option 2: PC11 */
/* RCC_AHB2ENR_GPIOCEN = 1;
   GPIOC_MODER_MODER11  =    ALTERNATE_FUNCTION_MODE;
   GPIOC_AFRH_AFRH11    =    ALTERNATE_FUNCTION_6;
   GPIOC_PUPDR_PUPDR11  =    IO_PULL_DOWN;
   GPIOC_OSPEEDR_OSPEEDR11 = IO_MEDIUM_SPEED; */


   /* SPI3_MOSI - option 1: PB5 */
   RCC_AHB2ENR_GPIOBEN = 1;
   GPIOB_MODER_MODER5 =     ALTERNATE_FUNCTION_MODE;
   GPIOB_AFRL_AFRL5   =     ALTERNATE_FUNCTION_6;
   GPIOB_PUPDR_PUPDR5 =     IO_PULL_DOWN;
   GPIOB_OSPEEDR_OSPEEDR5 = IO_MEDIUM_SPEED; 

   /* SPI3_MOSI - option 2: PC12 */
/* RCC_AHB2ENR_GPIOCEN = 1;
   GPIOC_MODER_MODER12  =    ALTERNATE_FUNCTION_MODE;
   GPIOC_AFRH_AFRH12    =    ALTERNATE_FUNCTION_6;
   GPIOC_PUPDR_PUPDR12  =    IO_PULL_DOWN;
   GPIOC_OSPEEDR_OSPEEDR12 = IO_MEDIUM_SPEED; */
}
