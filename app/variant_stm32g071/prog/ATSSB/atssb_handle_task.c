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
#include "timer/timerlib.h"
#include "segmentdef.h"


/******************************************************************************/
/* DEFINITIONS AND DECLARATIONS                                               */
/******************************************************************************/
/**
 * \brief   Time in which the callback from SSB stack is simulated
 *
 */
#define ATSSB_CALLBACK_SIMUATION_TIME_X10MS     (uint8_t) 10  // 100ms


/******************************************************************************/
/* STATIC VARIABLES                                                           */
/******************************************************************************/
static uint8_t ATSSB_taskState = TASK_NOT_INITIALISED;

const uint8_t ATSSB_callbackSimulationData[] =
{
        0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA,
        0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55
};

/**
 * \brief   This timer variable is loaded and decreased in the dedicated
 *          timesteps
 *
 */
SDEF_SetSegmentRW(TIMER8_10MS)
static uint8_t ATSSB_callbackSimulationTimerId = 0u;
SDEF_SetSegmentRW_Default()

/******************************************************************************/
/* STATIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
static void ATSSB_simulateDatastream( void );

/******************************************************************************/
/* OBJECTS                                                                    */
/******************************************************************************/

/******************************************************************************/
/* STATIC FUNCTION DEFINITION                                                 */
/******************************************************************************/
/**
 * \brief   simulate callback call from SSB sw and sets dummy data
 *
 * \param   none
 *
 * \return  none
 */
static void ATSSB_simulateDatastream( void )
{
    uint16_t eventToken = 0u;
    uint8_t eventDataLen;

    eventToken |= SSB_EVT_CLIENT_0;
    eventToken |= SSB_EVT_LOOP_RESULTS;

    eventDataLen = (uint8_t)(sizeof(ATSSB_callbackSimulationData) /
                             sizeof(ATSSB_callbackSimulationData[0]));

    ATSSB_doForHubCAPICallback(     eventToken,
                                    ATSSB_callbackSimulationData,
                                    eventDataLen );
}


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
            TIM_vLoadTimer( ATSSB_callbackSimulationTimerId,
                            ATSSB_CALLBACK_SIMUATION_TIME_X10MS );

            ATSSB_taskState = TASK_INITIALISED;
            break;
        }
        case TASK_INITIALISED:
        {
            if( TIM_ucIsTimerDown(ATSSB_callbackSimulationTimerId) )
            {
                ATSSB_simulateDatastream();
                TIM_vLoadTimer( ATSSB_callbackSimulationTimerId,
                                ATSSB_CALLBACK_SIMUATION_TIME_X10MS );
            }

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

