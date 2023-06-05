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
 *  COMP_ABBREV      HDIO
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file configuration for HDIO EFR32xG21
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
{  // Port A initialization - pins PA0..PA6
   // Warning: Pins PA1, PA2, PA3 and PA4 are connect to debugger. Be careful when configuring these pins!
     (uint16_t) (HDIO_PIN0_ACTIVE   |HDIO_PIN5_ACTIVE   |HDIO_PIN6_ACTIVE  )
    ,(uint16_t) (HDIO_PIN0_LOW      |HDIO_PIN5_LOW      |HDIO_PIN6_LOW     )
    ,(uint16_t) (HDIO_PIN0_PUSHPULL |HDIO_PIN5_PUSHPULL |HDIO_PIN6_PUSHPULL)
    ,(uint32_t) (HDIO_PIN0_INPUT    |HDIO_PIN5_INPUT    |HDIO_PIN6_INPUT   )
    ,(uint32_t) (HDIO_PIN0_PULLUP   |HDIO_PIN5_PULLUP   |HDIO_PIN6_PULLUP  )
},
#endif
#ifdef HDIO_PB_PRESENT
{  // Port B initialization - pins PB0, PB1
     (uint16_t) (0u)   // Port will not initialized. Previous settings will be retained.
    ,(uint16_t) (HDIO_PIN0_LOW      |HDIO_PIN1_LOW     )
    ,(uint16_t) (HDIO_PIN0_PUSHPULL |HDIO_PIN1_PUSHPULL)
    ,(uint32_t) (HDIO_PIN0_INPUT    |HDIO_PIN1_INPUT   )
    ,(uint32_t) (HDIO_PIN0_PULLUP   |HDIO_PIN1_PULLUP  )
},
#endif
#ifdef HDIO_PC_PRESENT
{  // Port C initialization - pins PC0..PC5
     (uint16_t) (HDIO_PIN0_ACTIVE   |HDIO_PIN1_ACTIVE    |HDIO_PIN2_ACTIVE    |HDIO_PIN3_ACTIVE   |HDIO_PIN4_ACTIVE   |HDIO_PIN5_ACTIVE  )
    ,(uint16_t) (HDIO_PIN0_LOW      |HDIO_PIN1_LOW       |HDIO_PIN2_LOW       |HDIO_PIN3_LOW      |HDIO_PIN4_LOW      |HDIO_PIN5_LOW     )
    ,(uint16_t) (HDIO_PIN0_PUSHPULL |HDIO_PIN1_PUSHPULL  |HDIO_PIN2_PUSHPULL  |HDIO_PIN3_PUSHPULL |HDIO_PIN4_PUSHPULL |HDIO_PIN5_PUSHPULL)
    ,(uint32_t) (HDIO_PIN0_INPUT    |HDIO_PIN1_INPUT     |HDIO_PIN2_INPUT     |HDIO_PIN3_INPUT    |HDIO_PIN4_INPUT    |HDIO_PIN5_INPUT   )
    ,(uint32_t) (HDIO_PIN0_PULLUP   |HDIO_PIN1_PULLUP    |HDIO_PIN2_PULLUP    |HDIO_PIN3_PULLUP   |HDIO_PIN4_PULLUP   |HDIO_PIN5_PULLUP  )
},
#endif
#ifdef HDIO_PD_PRESENT
{  // Port D initialized - pins PD0..PD4
     (uint16_t) (HDIO_PIN0_ACTIVE   |HDIO_PIN1_ACTIVE    |HDIO_PIN2_ACTIVE    |HDIO_PIN3_ACTIVE    |HDIO_PIN4_ACTIVE  )
    ,(uint16_t) (HDIO_PIN0_LOW      |HDIO_PIN1_LOW       |HDIO_PIN2_LOW       |HDIO_PIN3_LOW       |HDIO_PIN4_LOW     )
    ,(uint16_t) (HDIO_PIN0_PUSHPULL |HDIO_PIN1_PUSHPULL  |HDIO_PIN2_PUSHPULL  |HDIO_PIN3_PUSHPULL  |HDIO_PIN4_PUSHPULL)
    ,(uint32_t) (HDIO_PIN0_INPUT    |HDIO_PIN1_INPUT     |HDIO_PIN2_INPUT     |HDIO_PIN3_INPUT     |HDIO_PIN4_INPUT   )
    ,(uint32_t) (HDIO_PIN0_PULLUP   |HDIO_PIN1_PULLUP    |HDIO_PIN2_PULLUP    |HDIO_PIN3_PULLUP    |HDIO_PIN4_PULLUP  )
}
#endif
} };
