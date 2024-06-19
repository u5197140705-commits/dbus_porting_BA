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
#include "ssbf_common_c.h"


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
void ATSSB_doForHubCAPICallback(    uint16_t eventToken,
                                    const uint8_t *eventDataPtr,
                                    uint8_t eventDataLen )
{
    (void)*eventDataPtr;
    (void)eventDataLen;

    if( (eventToken & SSB_EVT_CLIENT_MASK) == SSB_EVT_CLIENT_0 )
    {
        if( (eventToken & SSB_EVT_TYPE_MASK) == SSB_EVT_LOOP_RESULTS )
        {
            //TODO: Add output datastream
        }
        else
        {
            //nothing to do for the moment
        }
    }
    else
    {
        //nothing to do for the moment
    }
}


uint8_t ATSSB_handleTask(void)
{
    switch (ATSSB_taskState)
    {
        case TASK_NOT_INITIALISED:
        {
            //TODO:add notification for callback function, e.g. notifyAPI( ATSSB_doForHubCAPICallback )

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

