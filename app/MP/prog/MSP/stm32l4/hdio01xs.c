/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB_FRAMEWORK_NG
 *  COMP_ABBREV      HDIO
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file configuration for HDIO STM32L4
 *
 */
 
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hdio.h"

/* Example of filling the structure for port A, B and C: */
const THDIO_Config HDIO_atConfig[] = { {
#ifdef HDIO_PA_PRESENT
{  // Port A initialization
     (uint16_t) (HDIO_PIN0_ACTIVE        |HDIO_PIN1_ACTIVE         |HDIO_PIN4_ACTIVE)
    ,(uint16_t) (HDIO_PIN0_LOW           |HDIO_PIN1_HIGH           |HDIO_PIN4_HIGH)
    ,(uint16_t) (HDIO_PIN0_OPENDRAIN     |HDIO_PIN1_OPENDRAIN      |HDIO_PIN4_PUSHPULL)
    ,(uint32_t) (HDIO_PIN0_OUTPUT        |HDIO_PIN1_INPUT          |HDIO_PIN4_INPUT)
    ,(uint32_t) (HDIO_PIN0_MEDIUM_SPEED  |HDIO_PIN1_MEDIUM_SPEED   |HDIO_PIN4_HIGH_SPEED)
    ,(uint32_t) (HDIO_PIN0_NOPUPD        |HDIO_PIN1_PULLUP         |HDIO_PIN4_PULLDOWN)
},
#endif
#ifdef HDIO_PB_PRESENT
{  // Port B initialization
     (uint16_t) (HDIO_PIN0_ACTIVE)
    ,(uint16_t) (HDIO_PIN0_LOW)
    ,(uint16_t) (HDIO_PIN0_PUSHPULL)
    ,(uint32_t) (HDIO_PIN0_OUTPUT)
    ,(uint32_t) (HDIO_PIN0_MEDIUM_SPEED)
    ,(uint32_t) (HDIO_PIN0_NOPUPD)
},
#endif
#ifdef HDIO_PC_PRESENT
{  // Port C initialization
     (uint16_t) (HDIO_PIN0_ACTIVE        |HDIO_PIN1_ACTIVE         |HDIO_PIN3_ACTIVE)
    ,(uint16_t) (HDIO_PIN0_LOW           |HDIO_PIN1_HIGH           |HDIO_PIN3_HIGH)
    ,(uint16_t) (HDIO_PIN0_PUSHPULL      |HDIO_PIN1_OPENDRAIN      |HDIO_PIN3_PUSHPULL)
    ,(uint32_t) (HDIO_PIN0_OUTPUT        |HDIO_PIN1_INPUT          |HDIO_PIN3_INPUT)
    ,(uint32_t) (HDIO_PIN0_LOW_SPEED     |HDIO_PIN1_MEDIUM_SPEED   |HDIO_PIN3_HIGH_SPEED)
    ,(uint32_t) (HDIO_PIN0_NOPUPD        |HDIO_PIN1_PULLUP         |HDIO_PIN3_PULLDOWN)
},
#endif
#ifdef HDIO_PD_PRESENT
{  // Port D not initialized
    0u, 0u, 0u, 0u, 0u, 0u
},
#endif
#ifdef HDIO_PE_PRESENT
{  // Port E not initialized
    0u, 0u, 0u, 0u, 0u, 0u
},
#endif
#ifdef HDIO_PF_PRESENT
{  // Port F not initialized
    0u, 0u, 0u, 0u, 0u, 0u
},
#endif
#ifdef HDIO_PG_PRESENT
{  // Port G not initialized
    0u, 0u, 0u, 0u, 0u, 0u
},
#endif
#ifdef HDIO_PH_PRESENT
{  // Port H not initialized
    0u, 0u, 0u, 0u, 0u, 0u
},
#endif
#ifdef HDIO_PI_PRESENT
{  // Port I not initialized
    0u, 0u, 0u, 0u, 0u, 0u
},
#endif
} };
