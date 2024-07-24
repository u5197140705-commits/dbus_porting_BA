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
#include "utility.h"
#include "system_timer.h"
#include "debug_extended/api_cfg.h"
#include "rtos_ref_queue.h"
#include "rtos_api.h"


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
#define ATSSB_CALLBACK_LOG_DATA_MAX             (uint8_t) 25

/**
 * \brief   Data which can maximal transmitted at once
 *
 */
#define ATSSB_CALLBACK_LOG_DATA_LEN             (uint8_t) 82

/**
 * \brief   Data which can maximal transmitted at once
 *
 */
#define ATSSB_CALLBACK_LOG_DATA_MSG_NR          (uint8_t) 5

/******************************************************************************/
/* STATIC TYPEDEFINITIONS                                                     */
/******************************************************************************/
/**
 * \brief   Data which can maximal transmitted at once
 *
 */
typedef struct
{
    uint16_t eventToken;
    uint8_t eventData[ATSSB_CALLBACK_LOG_DATA_LEN];
    uint8_t eventDataLen;

}ATSSB_REF_QUEUE_ELEMENT_t;


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
const uint8_t ATSSB_callbackSimulationData[ATSSB_CALLBACK_LOG_DATA_LEN] =
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

/**
 * \brief   control blocks for RTOS reference queue
 *
 */
extern RTOS_REF_QUEUE RTOS_atssbRefQueue;
RTOS_DEFINE_REF_QUEUE_AUTO( RTOS_atssbRefQueue,
                            ATSSB_CALLBACK_LOG_DATA_MSG_NR,
                            sizeof(ATSSB_REF_QUEUE_ELEMENT_t) );

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

    eventToken |= 0x0001u;
    eventToken |= 0x8000u;

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
    void *msgPtr_void; //gets valid adress in first fct call
    ATSSB_REF_QUEUE_ELEMENT_t *msgPtr;
    bool queueError = false;

    //allocate memory for a new queue element
    if(RTOS_allocateMemoryForRefQueue(  &RTOS_atssbRefQueue, &msgPtr_void)
                                            != RTOS_REF_QUEUE_OK)
    {
        queueError = true;
    }
    else
    {
        msgPtr = (ATSSB_REF_QUEUE_ELEMENT_t*)msgPtr_void;

        //Fill queue
        msgPtr->eventToken = eventToken;
        msgPtr->eventDataLen = eventDataLen;
        UTI_vMemCopy( eventDataPtr,  msgPtr->eventData, eventDataLen );

        //set element to end of queue
        if( RTOS_putReferenceAtRefQueueEnd( &RTOS_atssbRefQueue, msgPtr_void,
                                            sizeof(ATSSB_REF_QUEUE_ELEMENT_t),
                                            RTOS_NO_WAIT )
                                                != RTOS_REF_QUEUE_OK )
        {
            queueError = true;
        }
    }

    //Set flag to trigger ATSSB_releaseQueue()
    if( queueError == false )
    {
        (void)RTD_RunEventDrivenTask( FLAG_0, ET_ID_ORYX );
    }
}


uint8_t ATSSB_releaseQueue(void)
{
    void *msgPtr_void;
    uint32_t msgLen;
    uint8_t index;
    RTOS_REF_QUEUE_STATUS status;
    ATSSB_REF_QUEUE_ELEMENT_t *msgPtr;
    uint16_t eventToken;
    uint8_t *eventDataPtrTemp;
    uint8_t eventDataLen;
    uint8_t eventDataLenTemp;


    //get reference from queue
    status = RTOS_getReferenceFromRefQueue( &RTOS_atssbRefQueue,
                                            &msgPtr_void,
                                            &msgLen, RTOS_NO_WAIT );


    if( (status != RTOS_REF_QUEUE_OK) && (status != RTOS_REF_QUEUE_EMPTY) )
    {
        //error or empty queue
    }
    else if( status == RTOS_REF_QUEUE_OK )
    {
        //take element out of the queue
        msgPtr = (ATSSB_REF_QUEUE_ELEMENT_t*)msgPtr_void;
        eventToken = msgPtr->eventToken;
        eventDataPtrTemp = msgPtr->eventData;
        eventDataLen = msgPtr->eventDataLen;

        //Set msg to debug component

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

        for( index = 0u; index < 4u; index++ ) //max 4 iterations to send 4x 25 bytes = 100 bytes
        {
            if( eventDataLen > ATSSB_CALLBACK_LOG_DATA_MAX )
            {
                eventDataLenTemp = ATSSB_CALLBACK_LOG_DATA_MAX;
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

        //free memory
        if(RTOS_releaseMemoryForRefQueue( &RTOS_atssbRefQueue, msgPtr_void )
                                            != RTOS_REF_QUEUE_OK)
        {
            //error
        }
    }
    else
    {
        //noting to do
    }

    return TASK_INITIALISED;
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

