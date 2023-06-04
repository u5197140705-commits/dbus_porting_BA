/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB_HAL_STM32H7
 *  COMP_ABBREV      HDIO
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *
 *  \brief    Implementation of HAL Discrete Input and Output.
 *
 *  \details  The HAL DIO function supports the control of discrete input and output ports. It provides interfaces to
 *            read from and write to discrete input/output ports P<port><pin>. Furthermore it contains methods to
 *            change the port direction and the port driver (push-pull / open drain) during operation. The pin state is
 *            always the state at the microprocessor port pin (low or high).
 *
 *  \note     Pins without HDIO_PINx_ACTIVE (x=0-15) mask (in configuration structure) is not set and
 *            values of these pins are not changed after call HDIO_vInit() function!
 *            This mask does not affect no other functions HDIO library.
 */
 
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hdio.h"

/* Example of filling the structure for port A, B and C: */
/*lint -save -e835 A zero has been given as right/left argument to | in a constant expression */
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
/*lint -restore -e835 */
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
#ifdef HDIO_PJ_PRESENT
{  // Port J not initialized
    0u, 0u, 0u, 0u, 0u, 0u
},
#endif
#ifdef HDIO_PK_PRESENT
{  // Port K not initialized
    0u, 0u, 0u, 0u, 0u, 0u
},
#endif
} };
