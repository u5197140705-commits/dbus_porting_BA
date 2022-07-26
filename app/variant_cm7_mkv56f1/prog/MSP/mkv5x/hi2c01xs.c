/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          MKV5x PED-Framework
*   COMP_ABBREV      HI2C
*   AUTHOR           HrasokT
*   CREATED          21.09.2017  14:22:08
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief   This module contains configuration for I2C for NXP MKV5x.
 */     

    
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hi2c.h"
#include "hdio.h"
#include "hsup.h"


/* CONSTANTS ******************************************************************************************/
/* maximum waitstate delay */
const uchar HI2C0_ucMaxWaitState = 0xFF;


/* DEFINITIONS ****************************************************************************************/
#define SCL_PIN  C7
#define SCL_PORT C

#define SDA_PIN  C6
#define SDA_PORT C

#define RW_PIN   C5
#define RW_PORT  C

#define NOP_COUNT_400kHz   90u

/* Note 9026: function like macro, Note 9024: usage of #/## */
/*lint -save -e9024 -e9026 */

#define _HDIO_vSetAltFuncPort(pin,value) HDIO_vSetAltFuncPort##pin(value)
#define HDIO_vSetAltFuncPort(pin,value)  _HDIO_vSetAltFuncPort(pin,value)

#define _HDIO_vSetPullUpPullDownPort(pin,value) HDIO_vSetPullUpPullDownPort##pin(value)
#define HDIO_vSetPullUpPullDownPort(pin,value) _HDIO_vSetPullUpPullDownPort(pin,value)

#define _HDIO_vEnablePort(port) HDIO_vEnablePort##port()
#define HDIO_vEnablePort(port)  _HDIO_vEnablePort(port)

#define _HDIO_vSetPort(pin, value) HDIO_vSetPort##pin(value)
#define HDIO_vSetPort(pin, value)  _HDIO_vSetPort(pin, value)

#define _HDIO_vSetModePort(pin, value) HDIO_vSetModePort##pin(value)
#define HDIO_vSetModePort(pin, value)  _HDIO_vSetModePort(pin, value)

#define _HDIO_bGetPort(pin) HDIO_bGetPort##pin()
#define HDIO_bGetPort(pin)  _HDIO_bGetPort(pin)
/*lint -restore -e9024 -e9026 */

void HI2C0_vInitPort(void)
{
    uint8_t u8Status = ON;

    /* Setting SCL pin */
    HDIO_vEnablePort(SCL_PORT);      /* enable clock for GPIO */
    HDIO_vSetAltFuncPort(SCL_PIN,HDIO_ALT1);
    // pull up resistor status on SCL line (optional)
    HDIO_vSetPullUpPullDownPort(SCL_PIN,HDIO_PULL_DISABLED);
    HDIO_vSetPort(SCL_PIN, u8Status);
    HDIO_vSetModePort(SCL_PIN, HDIO_INPUT);

    /* Setting SDA pin */
    HDIO_vEnablePort(SDA_PORT);
    // set corresponding pins to alternate function of type GPIO
    HDIO_vSetAltFuncPort(SDA_PIN,HDIO_ALT1);
    // pull up resistor status on SDA line (optional)
    HDIO_vSetPullUpPullDownPort(SDA_PIN,HDIO_PULL_DISABLED);
    HDIO_vSetPort(SDA_PIN, u8Status);
    HDIO_vSetModePort(SDA_PIN, HDIO_INPUT);
}

void HI2C0_vBitDelayH(void)
{
    /** \todo Delay must be adjusted to get not more than 400Khz */
    uint8_t u8Counter;
    for(u8Counter = 0u; u8Counter < NOP_COUNT_400kHz; u8Counter++)
    {
        HSUP_vNop();
    }
}

void HI2C0_vBitDelayL(void)
{
    /** \todo Delay must be adjusted to get not more than 400Khz */
    uint8_t u8Counter;
    for(u8Counter = 0u; u8Counter < NOP_COUNT_400kHz; u8Counter++)
    {
        HSUP_vNop();
    }
}

/* event handler */
void HI2C0_vHandleEvent(void)
{

}

/* configure SCL pin as output */
void HI2C0_vOutputSCL(void)
{
    HDIO_vSetModePort(SCL_PIN, HDIO_OUTPUT);
}

/* configure SDA pin as output */
void HI2C0_vOutputSDA(void)
{
    HDIO_vSetModePort(SDA_PIN, HDIO_OUTPUT);
}

/* configure SCL pin as input */
void HI2C0_vInputSCL(void)
{
    HDIO_vSetModePort(SCL_PIN, HDIO_INPUT);
}

/* configure SDA pin as input */
void HI2C0_vInputSDA(void)
{
    HDIO_vSetModePort(SDA_PIN, HDIO_INPUT);
}

/* set SCL pin low */
void HI2C0_vClrSCL(void)
{
    HDIO_vSetPort( SCL_PIN, 0u );
}

/* set SDA pin low */
void HI2C0_vClrSDA(void)
{
    HDIO_vSetPort( SDA_PIN, 0u );
}

/* set SCL pin high */
void HI2C0_vSetSCL(void)
{
    HDIO_vSetPort( SCL_PIN, 1u );
}

/* set SDA pin high */
void HI2C0_vSetSDA(void)
{
    HDIO_vSetPort( SDA_PIN, 1u );
}

/* return current SCL pin state */
bool HI2C0_bGetSCL(void)
{
    return (bool)HDIO_bGetPort(SCL_PIN);
}

/* return current SDA pin state */
bool HI2C0_bGetSDA(void)
{
    return (bool)HDIO_bGetPort(SDA_PIN);
}

void HI2C0_vInitPin(void)
{
    HSUP_vDisableInt();

    HDIO_vEnablePort(RW_PORT);
    HDIO_vSetPort(RW_PIN, 1u);
    HDIO_vSetModePort(RW_PIN, HDIO_OUTPUT);

    HSUP_vEnableInt();
}

/* set chip write control pin low */
void HI2C0_vWriteEnable(void)
{
    HDIO_vSetPort(RW_PIN, 0u);
}

/* set chip write control pin high */
void HI2C0_vWriteDisable(void)
{
    HDIO_vSetPort(RW_PIN, 1u);
}
