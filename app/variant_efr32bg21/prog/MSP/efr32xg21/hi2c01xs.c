/*******************************************************************************
 *  Copyright (c) 2020 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          efr32bg21 HAL PED Framework
 *  COMP_ABBREV      HI2C
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief Configuration for I2C.
 *  Function definitions to create software controlled communication on I2C bus
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hi2c.h"
#include "hi2c01xs.h"
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
#define NOP_COUNT_400kHz   200u

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

#define SCL_PIN      HDIO_PIN3 //EXP_HEADER_9
#define SCL_PORT     HDIOD
#define SCL_PINMACRO D3


#define SDA_PIN      HDIO_PIN2 //EXP_HEADER_7
#define SDA_PORT     HDIOD
#define SDA_PINMACRO D2

#define RW_PIN       HDIO_PIN1 //EXP_HEADER_6
#define RW_PORT      HDIOC
#define RW_PINMACRO  C1


/* Note 9026: function like macro, Note 9024: usage of #/## */
/*lint -save -e9024 -e9026 */
#define _HDIO_vSetOpenDrainPushPullPort(pin, value) HDIO_vSetOpenDrainPushPullPort##pin(value)
#define HDIO_vSetOpenDrainPushPullPort(pin, value)  _HDIO_vSetOpenDrainPushPullPort(pin, value)

#define _HDIO_vSetPort(pin, value) HDIO_vSetPort##pin(value)
#define HDIO_vSetPort(pin, value)  _HDIO_vSetPort(pin, value)

#define _HDIO_bGetPort(pin) HDIO_bGetPort##pin()
#define HDIO_bGetPort(pin)  _HDIO_bGetPort(pin)
/*lint -restore -e9024 -e9026 */

void HI2C0_vInitPort(void)
{
    uint8_t u8Status = ON;
    HSUP_vDisableInt();

    /* Setting SCL pin */
    HDIO_vSetOpenDrainPushPullPort(SCL_PINMACRO, HDIO_OPEN_DRAIN);
    HDIO_vSetPort(SCL_PINMACRO, u8Status);
    HDIO_vSetMode(SCL_PORT, SCL_PIN, HDIO_OUTPUT, NULL);

    /* Setting SDA pin */
    HDIO_vSetOpenDrainPushPullPort(SDA_PINMACRO, HDIO_OPEN_DRAIN);
    HDIO_vSetPort(SDA_PINMACRO, u8Status);
    HDIO_vSetMode(SDA_PORT, SDA_PIN, HDIO_OUTPUT, NULL);

    HSUP_vEnableInt();
}


void HI2C0_vOutputSCL(void)
{
    HDIO_vSetMode(SCL_PORT, SCL_PIN, HDIO_OUTPUT, NULL);
}

void HI2C0_vInputSCL(void)
{
    HDIO_vSetMode(SCL_PORT, SCL_PIN, HDIO_INPUT, NULL);
}

void HI2C0_vClrSCL(void)
{
    HDIO_vSetPort( SCL_PINMACRO, 0u );
}

void HI2C0_vSetSCL(void)
{
    HDIO_vSetPort( SCL_PINMACRO, 1u );
}

bool HI2C0_bGetSCL(void)
{
    return (bool)HDIO_bGetPort(SCL_PINMACRO);
}

void HI2C0_vOutputSDA(void)
{
    HDIO_vSetMode(SDA_PORT, SDA_PIN, HDIO_OUTPUT, NULL);
}

void HI2C0_vInputSDA(void)
{
    HDIO_vSetMode(SDA_PORT, SDA_PIN, HDIO_INPUT, NULL);
}

void HI2C0_vClrSDA(void)
{
    HDIO_vSetPort( SDA_PINMACRO, 0u );
}

void HI2C0_vSetSDA(void)
{
    HDIO_vSetPort( SDA_PINMACRO, 1u );
}

bool HI2C0_bGetSDA(void)
{
    return (bool)HDIO_bGetPort(SDA_PINMACRO);
}


void HI2C0_vBitDelayH(void)
{
    /** \todo Delay must be adjusted to get not more than 400Khz */
    uint16_t u16Counter;
    for(u16Counter = 0u; u16Counter < NOP_COUNT_400kHz; u16Counter++)
    {
        HSUP_vNop();
    }
}

void HI2C0_vBitDelayL(void)
{
    /** \todo Delay must be adjusted to get not more than 400Khz */
    uint16_t u16Counter;
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
    HDIO_vSetOpenDrainPushPullPort(RW_PINMACRO, HDIO_OPEN_DRAIN);
    HDIO_vSetPort(RW_PINMACRO, 1u);
    HDIO_vSetMode(RW_PORT, RW_PIN, HDIO_OUTPUT, NULL);
    HSUP_vEnableInt();
}

void HI2C0_vWriteEnable(void)
{
    HDIO_vSetPort(RW_PINMACRO, 0u);
}

void HI2C0_vWriteDisable(void)
{
    HDIO_vSetPort(RW_PINMACRO, 1u);
}
