/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Smart Sensor Bus
 *  COMP_ABBREV      SSBF
 ******************************************************************************/

#ifndef SSBF_MNGR_COMMON_H
#define SSBF_MNGR_COMMON_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     ssbf_mngr_common.h
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    Common definitions for the modules in the configuration area
 *            of the framework access layer
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"

/******************************************************************************/
/* TYPE DEFINITIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* C-PREPROCESSOR DEFINITIONS                                                 */
/******************************************************************************/

#define SSBF_MNGR_NUMBER_OF_HUBS   1U // No cast operator at the definition
                                      // because of preprocessor comparison
                                      // with no cast allowed

#define SSBF_MNGR_NUMBER_OF_I2C               ((uint8_t)2U)
#define SSBF_MNGR_NUMBER_OF_LAYERS_NOTIFIED   ((uint8_t)3U)

#endif // From: #ifndef SSBF_MNGR_COMMON_H
