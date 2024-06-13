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
 *  COMP_ABBREV      ATSSB
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     atssb_handle_task.c
 *
 *  \ingroup  app/ATSSB
 *
 *  \brief    Implementation of Application Task for Smart Sensor Bus
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "atssb_handle_task.h"


/******************************************************************************/
/* STATIC VARIABLES                                                           */
/******************************************************************************/
static uint8_t ATSSB_taskState = TASK_NOT_INITIALISED;

/******************************************************************************/
/* STATIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/******************************************************************************/
/* OBJECTS                                                                    */
/******************************************************************************/


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
uint8_t ATSSB_handleTask(void)
{
    switch (ATSSB_taskState)
    {
        case TASK_NOT_INITIALISED:
        {
            ATSSB_taskState = TASK_INITIALISED; //TODO: Modify. Kept for now to prevent compiler warnings
            break;
        }
        case TASK_INITIALISED:
        {
            break;
        }
        default:
        {
            //Nothing to do
            break;
        }
    }

    return(ATSSB_taskState);
}

