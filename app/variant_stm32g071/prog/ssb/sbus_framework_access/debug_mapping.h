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
 *  COMP_ABBREV      TIM
 ******************************************************************************/

#ifndef DEBUG_MAPPING_H
#define DEBUG_MAPPING_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     debug_mapping.h
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    Compatibility header to debug extended component
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "bsh_stdinc.h"

#ifdef DBGX_INCLUDED
    #include "debug_extended/api_cfg.h"
#else
    #include "filters_aut_dummy.h"
#endif

#ifdef __cplusplus
}
#endif

#endif // From: #ifndef DEBUG_MAPPING_H

