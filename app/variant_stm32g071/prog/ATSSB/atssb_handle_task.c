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
#include "system_timer.h"


/******************************************************************************/
/* DEFINITIONS AND DECLARATIONS                                               */
/******************************************************************************/
/**
 * \brief   Time in which the callback from SSB stack is simulated
 *
 */
#define ATSSB_CALLBACK_SIMUATION_TIME_MS        (uint8_t) 100  // 100ms


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
 * \brief   control blocks for timer to simulate SSB callback
 *
 */
static struct STIM_Timer ATSSB_callbackSimulationTimer;
static struct STDCB_Callback ATSSB_callbackSimulationTimerCb;

/******************************************************************************/
/* STATIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
static int32_t ATSSB_simulateDatastream(void *obj, uint32_t flags, int32_t data);


/******************************************************************************/
/* OBJECTS                                                                    */
/******************************************************************************/

/******************************************************************************/
/* STATIC FUNCTION DEFINITION                                                 */
/******************************************************************************/
/**
 * \brief   This function is registrated in timer ATSSB_callbackSimulationTimer.
 *          It simulates the callback from SSB sw and sets dummy data.
 *
 * \param   input from timerlib - usused
 *
 * \return  default return value - 0
 */
static int32_t ATSSB_simulateDatastream( void *obj, uint32_t flags, int32_t data )
{
    (void)flags;
    (void)data;

    uint16_t eventToken = 0u;
    uint8_t eventDataLen;

    eventToken |= SSB_EVT_CLIENT_0;
    eventToken |= SSB_EVT_LOOP_RESULTS;

    eventDataLen = (uint8_t)(sizeof(ATSSB_callbackSimulationData) /
                             sizeof(ATSSB_callbackSimulationData[0]));

    ATSSB_doForHubCAPICallback(     eventToken,
                                    ATSSB_callbackSimulationData,
                                    eventDataLen );

    return 0;
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
            //initialize and start timer to simulate SSB callback
            (void)STIM_InitCallback(    &ATSSB_callbackSimulationTimerCb,
                                        ATSSB_simulateDatastream,
                                        NULL, STIM_STATUS_TRIGGERED);
            (void)STIM_InitTimer(   &ATSSB_callbackSimulationTimer,
                                    STIM_PROCESSING_INTERRUPT,
                                    STIM_TIME_MS(ATSSB_CALLBACK_SIMUATION_TIME_MS),
                                    STIM_MODE_PERIODIC,
                                    true,
                                    &ATSSB_callbackSimulationTimerCb);

            ATSSB_taskState = TASK_INITIALISED;
            break;
        }
        case TASK_INITIALISED:
        {
            //Nothing to do
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

