/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PP PED-Framework
*   PROCESSOR        ARM CortexMx
*   COMP_ABBREV      INTP    
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Configuration file for customize Interrupt Levels
*
*/

#include "bsh_stdinc.h" 
#include "int_prio.h"
#include <stdint.h>
#include "processor.h" // find out cortex core
#include "IntTbl.h"
#if defined(RTOS)
    #include "os/rtos_initialize_low_level.h"
#endif

//lint -e765 external symbol 'INTP_setApplicationPriority' could be made static [MISRA 2012 Rule 8.7, advisory]
//lint -e714 external symbol 'INTP_setApplicationPriority' was defined but not referenced

void INTP_setApplicationPriority(void)
{
    /* Set Interrupt Priority of configurable interrupts          */
    /* Allowed values CM0: INT_PRIO_0 ... INT_PRIO_3              */
    /* Allowed values CM3, CM4, CM4F: INT_PRIO_0 ... INT_PRIO_15  */

    INTP_setDefaultInterruptPriority();
#if !defined(PSOC4)
    ITBL_setCustomInterruptPriority();
#endif
#if defined(RTOS)
    RTOS_setRtosInterruptPriority();
#endif
}
