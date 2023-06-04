/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / SiLabs EFR32xG21
 *  COMP_ABBREV      HCCT
 ******************************************************************************/

#ifndef HCCTXS_H
#define HCCTXS_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief The HCCT pin mapping configuration.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hdio.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/**
*   \brief    These definitions are used to determine pin mapping
*
*   \details  Changing these defines below to define which pin will be used according to
*             application pin remapping and in HCCTX_bGetPinX() functions. 
*             Make sure that the selected pin is not used by another peripheral units,
*             or does not have a special functionality such as e.g. debug!
*
*             To assign application specific port to HCCTx channel please use "gpioPortX" (x=A, B, C, ... another ports) define.
*             To assign application specific pin to HCCTx channel please use numbers in range 0 to 15 for HCCTx_CHy_REMAP_PIN define.
*             E.g. if you want to use PB3 pin for HCCT0 for channel 1 then you must select:
*             "#define HCCT0_CH1_REMAP_PORT gpioPortB" and "#define HCCT0_CH1_REMAP_PIN 3".
*
*   \note:    Not all ports can be used for all timers. See comments below.
*             Pins PA1, PA2, PA3 and PA4 are connected to debugger. Be careful when configuring these pins!
*
*/
/* Hardware TIMER0 - HCCT0 and HCCT1 - (All pins are available) */
#define HCCT0_CH0_REMAP_PORT    gpioPortD
#define HCCT0_CH0_REMAP_PIN     0
#define HCCT0_CH1_REMAP_PORT    gpioPortD
#define HCCT0_CH1_REMAP_PIN     1

#define HCCT1_CH0_REMAP_PORT    gpioPortD
#define HCCT1_CH0_REMAP_PIN     2

/* Hardware TIMER1 - HCCT2 and HCCT3 - (All pins are available) */
#define HCCT2_CH0_REMAP_PORT    gpioPortC
#define HCCT2_CH0_REMAP_PIN     0
#define HCCT2_CH1_REMAP_PORT    gpioPortC
#define HCCT2_CH1_REMAP_PIN     1

#define HCCT3_CH0_REMAP_PORT    gpioPortC
#define HCCT3_CH0_REMAP_PIN     3

/* Hardware TIMER2 - HCCT4 and HCCT5 - (Only pins at Ports A and B are available) */
#define HCCT4_CH0_REMAP_PORT    gpioPortB
#define HCCT4_CH0_REMAP_PIN     0
#define HCCT4_CH1_REMAP_PORT    gpioPortB
#define HCCT4_CH1_REMAP_PIN     1

#define HCCT5_CH0_REMAP_PORT    gpioPortA
#define HCCT5_CH0_REMAP_PIN     5

/* Hardware TIMER3 - HCCT6 and HCCT7 - (Only pins at Ports C and D are available) */
#define HCCT6_CH0_REMAP_PORT    gpioPortC
#define HCCT6_CH0_REMAP_PIN     0
#define HCCT6_CH1_REMAP_PORT    gpioPortC
#define HCCT6_CH1_REMAP_PIN     1

#define HCCT7_CH0_REMAP_PORT    gpioPortC
#define HCCT7_CH0_REMAP_PIN     2

#ifdef __cplusplus
}
#endif

#endif // HCCTXS_H
