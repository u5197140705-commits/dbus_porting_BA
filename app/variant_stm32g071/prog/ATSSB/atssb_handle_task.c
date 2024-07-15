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
#include "debug_extended/api_cfg.h"


/******************************************************************************/
/* DEFINITIONS AND DECLARATIONS                                               */
/******************************************************************************/
/**
 * \brief   Time in which the callback from SSB stack is simulated
 *
 */
#define ATSSB_CALLBACK_SIMUATION_TIME_MS        (uint8_t) 100  // 100ms

/**
 * \brief   Data which can maximal transmitted at once
 *
 */
#define ATSSB_CALLBACK_SIMUATION_TX_LEN_MAX     (uint8_t) 28

/**
 * \brief   Offset to match a unit8 value with a ascii character
 *
 */
#define ATSSB_OFFSET_ASCCI                      (uint8_t) 0x30u
/******************************************************************************/
/* STATIC VARIABLES                                                           */
/******************************************************************************/
/**
 * \brief   state of handle task of this module
 *
 */
static uint8_t ATSSB_taskState = TASK_NOT_INITIALISED;

/**
 * \brief   dummy testdata to send
 *
 */
const uint8_t ATSSB_callbackSimulationData[] = //length 82
{
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
        0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1,
        0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2,
        0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3,
        0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4,
        0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5,
        0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6,
        0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7,
        0xf8, 0xf8
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
    (void)obj;
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
    uint8_t index;
    uint8_t eventDataLenTemp;
    uint8_t *eventDataPtrTemp = (uint8_t*)eventDataPtr; //lint !e926 !e954 !e9005 convert in non-const done intentionally

    if( (eventToken & SSB_EVT_CLIENT_MASK) == SSB_EVT_CLIENT_0 )
    {
        if( (eventToken & SSB_EVT_TYPE_MASK) == SSB_EVT_LOOP_RESULTS )
        {
            DBGX_logStr_INFO_SCN_SSB_CBACK_APP("\n");
            /********************* eventToken *********************/
            DBGX_logStr_INFO_SCN_SSB_CBACK_APP(
                    "ATSSB_doForHubCAPICallback: eventToken:" );
            DBGX_logInt_INFO_SCN_SSB_CBACK_APP  (
                    eventToken,
                    DBGX_UINT8_HEXADECIMAL      );

            /********************* eventDataLen *******************/
            DBGX_logStr_INFO_SCN_SSB_CBACK_APP(
                    "ATSSB_doForHubCAPICallback: eventDataLen:" );
            DBGX_logInt_INFO_SCN_SSB_CBACK_APP  (
                    eventDataLen,
                    DBGX_UINT8_HEXADECIMAL      );

            /********************* *eventDataPtr ******************/
            DBGX_logStr_INFO_SCN_SSB_CBACK_APP(
                    "ATSSB_doForHubCAPICallback: *eventDataPtr:" );

            for( index = 0u; index < 3u; index++ ) //max 3 iterations to send 4x 21 bytes = 84 bytes
            {
                if( eventDataLen > ATSSB_CALLBACK_SIMUATION_TX_LEN_MAX )
                {
                    eventDataLenTemp = ATSSB_CALLBACK_SIMUATION_TX_LEN_MAX;
                }
                else
                {
                    eventDataLenTemp = eventDataLen;
                }

                DBGX_logIntArr_INFO_SCN_SSB_CBACK_APP   (
                        eventDataPtrTemp,
                        eventDataLenTemp,
                        DBGX_UINT8_HEXADECIMAL          );

                eventDataPtrTemp += eventDataLenTemp;   //increase pointer by already sent
                eventDataLen -= eventDataLenTemp;       //update length for next iteration

                if( eventDataLen == 0u )
                {
                    break;
                }
            }
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

            DBGX_init();

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

