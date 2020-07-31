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
 *  COMP_ABBREV      HI2C
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief Configuration for I2C.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hi2c.h"
#include "hdio.h"
#include "hsup.h"

/******************************************************************************/
/* TYPE DEFINITIONS                                                           */
/******************************************************************************/
//lint -e506 Constant value boolean
//lint -e774 boolean within 'if' always evaluates to ...

/******************************************************************************/
/* PROTOTYPES                                                                 */
/******************************************************************************/

/******************************************************************************/
/* VARIABLES                                                                  */
/******************************************************************************/
uint32_t HI2C_ulNewPortStatus;

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/
/* maximum waitstate delay */
const uint8_t HI2C0_ucMaxWaitState = 0xFFu;

/******************************************************************************/
/* DEFINITIONS                                                                */
/******************************************************************************/
#define NOP_COUNT_400kHz   14u

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#define SCL_PIN  A7
#define SCL_PORT A

#define SDA_PIN  A6
#define SDA_PORT A

#define RW_PIN   A5


/* Note 9026: function like macro, Note 9024: usage of #/## */
/*lint -save -e9024 -e9026 */
#define _HDIO_vEnablePort(port) HDIO_vEnablePort##port()
#define HDIO_vEnablePort(port)  _HDIO_vEnablePort(port)

#define _HDIO_vSetOpenDrainPushPullPort(pin, value) HDIO_vSetOpenDrainPushPullPort##pin(value)
#define HDIO_vSetOpenDrainPushPullPort(pin, value)  _HDIO_vSetOpenDrainPushPullPort(pin, value)

#define _HDIO_vSetSpeedPort(pin, value) HDIO_vSetSpeedPort##pin(value)
#define HDIO_vSetSpeedPort(pin, value)  _HDIO_vSetSpeedPort(pin, value)

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
    HSUP_vDisableInt();

    /* Setting SCL pin */
    HDIO_vEnablePort(SCL_PORT);
    HDIO_vSetOpenDrainPushPullPort(SCL_PIN, HDIO_PULL_NONE);
    HDIO_vSetSpeedPort(SCL_PIN, HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetPort(SCL_PIN, u8Status);
    HDIO_vSetModePort(SCL_PIN, HDIO_INPUT);

    /* Setting SDA pin */
    HDIO_vEnablePort(SDA_PORT);
    HDIO_vSetOpenDrainPushPullPort(SDA_PIN, HDIO_PULL_NONE);
    HDIO_vSetSpeedPort(SDA_PIN, HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetPort(SDA_PIN, u8Status);
    HDIO_vSetModePort(SDA_PIN, HDIO_INPUT);

    HSUP_vEnableInt();
}


void HI2C0_vOutputSCL(void)
{
    HDIO_vSetModePort(SCL_PIN, HDIO_OUTPUT);
}

void HI2C0_vInputSCL(void)
{
    HDIO_vSetModePort(SCL_PIN, HDIO_INPUT);
}

void HI2C0_vClrSCL(void)
{
    HDIO_vSetPort( SCL_PIN, 0u );
}

void HI2C0_vSetSCL(void)
{
    HDIO_vSetPort( SCL_PIN, 1u );
}

bool HI2C0_bGetSCL(void)
{
    return (bool)HDIO_bGetPort(SCL_PIN);
}

void HI2C0_vOutputSDA(void)
{
    HDIO_vSetModePort(SDA_PIN, HDIO_OUTPUT);
}

void HI2C0_vInputSDA(void)
{
    HDIO_vSetModePort(SDA_PIN, HDIO_INPUT);
}

void HI2C0_vClrSDA(void)
{
    HDIO_vSetPort( SDA_PIN, 0u );
}

void HI2C0_vSetSDA(void)
{
    HDIO_vSetPort( SDA_PIN, 1u );
}

bool HI2C0_bGetSDA(void)
{
    return (bool)HDIO_bGetPort(SDA_PIN);
}


void HI2C0_vBitDelayH(void)
{
    /** \todo Delay must be adjusted to get not more than 400Khz */
    uint8_t u16Counter;
    for(u16Counter = 0u; u16Counter < NOP_COUNT_400kHz; u16Counter++)
    {
        HSUP_vNop();
    }
}

void HI2C0_vBitDelayL(void)
{
    /** \todo Delay must be adjusted to get not more than 400Khz */
    uint8_t u16Counter;
    for(u16Counter = 0; u16Counter < NOP_COUNT_400kHz; u16Counter++)
    {
        HSUP_vNop();
    }
}

/* event handler */
void HI2C0_vHandleEvent(void)
{

}

void HI2C0_vInitPin(void)
{
    HSUP_vDisableInt();
    HDIO_vEnablePort(SDA_PORT);
    HDIO_vSetOpenDrainPushPullPort(RW_PIN, HDIO_PULL_NONE);
    HDIO_vSetSpeedPort(RW_PIN, HDIO_OUTPUT_MEDIUM_SPEED);
    HDIO_vSetPort(RW_PIN, 1u);
    HDIO_vSetModePort(RW_PIN, HDIO_OUTPUT);

    HSUP_vEnableInt();
}

void HI2C0_vWriteEnable(void)
{
    HDIO_vSetPort(RW_PIN, 0u);
}

void HI2C0_vWriteDisable(void)
{
    HDIO_vSetPort(RW_PIN, 1u);
}
