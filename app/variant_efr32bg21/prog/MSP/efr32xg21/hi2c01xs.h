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

#ifndef HI2C01XS_H
#define HI2C01XS_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief Configuration for I2C.
 *  This interface is used to create software controlled communication on I2C bus
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include "hdio.h"
#include "hsup.h"

/******************************************************************************/
/* DEFINITIONS                                                                */
/******************************************************************************/
/*lint -save -e9026 -e755 Function-like macro uses least resources.
 * Not referenced because it is intended for users of the module. */

#define HI2C0_DataSetupTimeDelay() do{HSUP_vNop();HSUP_vNop();}while(0)

/*lint -restore -e9026 -e755 */

/******************************************************************************/
/* TYPE DEFINITIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* GLOBAL FUNCTIONS                                                           */
/******************************************************************************/

extern uint32_t HI2C_ulNewPortStatus;

/**
 * \brief    Initializes the port of the HI2C.
 * 
 *           This function initializes the special features for the port of the HI2C<unit>. For instance in case
 *           of the Stm32 the clock for the port is enabled in this function.
 *
 * \param    none
 * 
 * \return   none
 */
#define HI2C_SPECIAL_INIT //lint !e755 global macro not referenced because it's intended for the users
extern void HI2C0_vInitPort(void);

/* HI2C<unit>_vOutputSCL */
/**
 * \brief    User defined function that shall set the SCL pin to output
 *
 * \param    none
 * 
 * \return   none
 */
extern void HI2C0_vOutputSCL(void);

/* HI2C<unit>_vOutputSDA */
/**
 * \brief    User defined function that shall set the SDA pin to output
 *
 * \param    none
 * 
 * \return   none
 */
extern void HI2C0_vOutputSDA(void);

/* HI2C<unit>_vInputSCL */
/**
 * \brief    User defined function that shall set the SCL pin to input
 *
 * \param    none
 * 
 * \return   none
 */
extern void HI2C0_vInputSCL(void);

/* HI2C<unit>_vInputSDA */
/**
 * \brief    User defined function that shall set the SDA pin to input
 *
 * \param    none
 * 
 * \return   none
 */
extern void HI2C0_vInputSDA(void);

/* HI2C<unit>_vClrSCL */
/**
 * \brief    User defined function that shall set the SCL pin to a low level
 *
 * \param    none
 * 
 * \return   none
 */
extern void HI2C0_vClrSCL(void);

/* HI2C<unit>_vSetSCL */
/**
 * \brief    User defined function that shall set the SCL pin to a high level
 *
 * \param    none
 * 
 * \return   none
 */
extern void HI2C0_vSetSCL(void);

/* HI2C<unit>_vClrSDA */
/**
 * \brief    User defined function that shall set the SDA pin to a low level
 *
 * \param    none
 * 
 * \return   none
 */
extern void HI2C0_vClrSDA(void);

/* HI2C<unit>_vSetSDA */
/**
 * \brief    User defined function that shall set the SDA pin to a high level
 *
 * \param    none
 * 
 * \return   none
 */
extern void HI2C0_vSetSDA(void);

/* HI2C<unit>_bGetSCL */
/**
 * \brief    User defined function that shall return the current pin level of SCL
 *
 * \param    none
 * 
 * \return   pin level
 */
extern bool HI2C0_bGetSCL(void);

/* HI2C<unit>_bGetSDA */
/**
 * \brief    User defined function that shall return the current pin level of SDA
 *
 * \param    none
 * 
 * \return   pin level
 */
extern bool HI2C0_bGetSDA(void);

extern void HI2C0_vWriteEnable(void);

extern void HI2C0_vWriteDisable(void);

extern void HI2C0_vInitPin(void);

#ifdef __cplusplus
}
#endif

#endif
