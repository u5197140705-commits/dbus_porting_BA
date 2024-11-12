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
//#include "ssbf_common_c.h" //TODO: activate as soon as SSB stack is available
#ifdef ATSSB_RTOS_IS_USED
    #include "rtos_ref_queue.h"
    #include "rtos_api.h"
#endif //ATSSB_RTOS_IS_USED
/******************************************************************************/
/* DEFINITIONS AND DECLARATIONS                                               */
/******************************************************************************/
/**
 * \brief   Time in which the callback from SSB stack is simulated
 *
 */
#define ATSSB_CALLBACK_SIMUATION_TIME_MS        (uint8_t) 100  // 100ms


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

#ifdef ATSSB_RTOS_IS_USED
/**
 * \brief   control blocks for RTOS reference queue
 *
 */
RTOS_REF_QUEUE RTOS_atssbRefQueue;
RTOS_DEFINE_REF_QUEUE_AUTO( RTOS_atssbRefQueue,
                            ATSSB_NUMBER_OF_CALLBACK_LOG_DATA_PARTS,
                            sizeof(ATSSB_REF_QUEUE_ELEMENT_t) );
#endif //ATSSB_RTOS_IS_USED
/******************************************************************************/
/* STATIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
static int32_t ATSSB_simulateDatastream(void *obj, uint32_t flags, int32_t data);
static void ATSSB_setDataToDebugcomponent(  uint16_t eventToken,
                                            const uint8_t *eventDataPtr,
                                            uint8_t eventDataLen );


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

    eventToken |= 0x0001u; //TODO:Replace with SSB_EVT_CLIENT_0 as soon as SSB is available
    eventToken |= 0x8000u; //TODO:Replace with SSB_EVT_LOOP_RESULTS as soon as SSB is available

    eventDataLen = (uint8_t)(sizeof(ATSSB_callbackSimulationData) /
                             sizeof(ATSSB_callbackSimulationData[0]));

    ATSSB_doForCallbackToApi(     eventToken,
                                    ATSSB_callbackSimulationData,
                                    eventDataLen );

    return 0;
}


/**
 * \brief   Sends data via debug extended component
 *
 * \param   eventToken      Contains Hub-Index and further elements according to
 *                          ssbf_common_c.h
 *          eventDataPtr    Pointer to the data delivered (e.g. the loop results)
 *          eventDataLen    Number of bytes delivered via eventDataPtr
 *
 * \return  none
 */
static void ATSSB_setDataToDebugcomponent(  uint16_t eventToken,
                                            const uint8_t *eventDataPtr,
                                            uint8_t eventDataLen )
{
    uint8_t index;
    uint8_t eventDataLenTemp;
    uint8_t *eventDataPtrTemp = (uint8_t*)eventDataPtr; //lint !e926 !e954 !e9005 convert in non-const done intentionally

    DBGX_logStr_INFO_SCN_SSB_CBACK_APP("\n");
    /********************* eventToken *********************/
    DBGX_logStr_INFO_SCN_SSB_CBACK_APP(
            "ATSSB_doForCallbackToApi: eventToken:" );
    DBGX_logInt_INFO_SCN_SSB_CBACK_APP  (
            eventToken,
            DBGX_UINT8_HEXADECIMAL      );

    /********************* eventDataLen *******************/
    DBGX_logStr_INFO_SCN_SSB_CBACK_APP(
            "ATSSB_doForCallbackToApi: eventDataLen:" );
    DBGX_logInt_INFO_SCN_SSB_CBACK_APP  (
            eventDataLen,
            DBGX_UINT8_HEXADECIMAL      );

    /********************* *eventDataPtr ******************/
    DBGX_logStr_INFO_SCN_SSB_CBACK_APP(
            "ATSSB_doForCallbackToApi: *eventDataPtr:" );

    for( index = 0u; index < 4u; index++ ) //max 4 iterations to send 4x 25 bytes = 100 bytes
    {
        if( eventDataLen > ATSSB_CALLBACK_LOG_DATA_PART_LEN )
        {
            eventDataLenTemp = ATSSB_CALLBACK_LOG_DATA_PART_LEN;
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
/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
void ATSSB_doForCallbackToApi(  uint16_t eventToken,
                                const uint8_t *eventDataPtr,
                                uint8_t eventDataLen )
{
    #ifdef ATSSB_RTOS_IS_USED //Allocate memory and fill data in ref queue, if rtos used

        ATSSB_REF_QUEUE_ELEMENT_t *msgPtr;

        //allocate memory for a new queue element
        if(RTOS_allocateMemoryForRefQueue( &RTOS_atssbRefQueue, (void*)&msgPtr )
                                    != RTOS_REF_QUEUE_OK)
        {
            return;
        }
        else
        {
            //Fill queue if there is free memory available
            msgPtr->eventToken = eventToken;
            msgPtr->eventDataLen = eventDataLen;
            UTI_vMemCopy( eventDataPtr,  msgPtr->eventData, eventDataLen );

            if( RTOS_putReferenceAtRefQueueEnd( &RTOS_atssbRefQueue, msgPtr,
                                                sizeof(ATSSB_REF_QUEUE_ELEMENT_t),
                                                RTOS_NO_WAIT )
                                    != RTOS_REF_QUEUE_OK )
            {
                return;
            }
        }

        //Set flag to trigger ATSSB_getDataFromRefQueueReleaseMem()
        (void)RTD_RunEventDrivenTask( FLAG_0, ET_ID_ORYX );

    #else  //ATSSB_RTOS_IS_USED
        //Send data directly with debug comp, if baremetal scheduler used
        ATSSB_setDataToDebugcomponent( eventToken, eventDataPtr, eventDataLen );
    #endif
}

#ifdef ATSSB_RTOS_IS_USED
uint8_t ATSSB_getDataFromRefQueueReleaseMem(void)
{
    uint32_t msgLenDummy;
    ATSSB_REF_QUEUE_ELEMENT_t *msgPtr;

    //get all references from queue
    while( RTOS_getReferenceFromRefQueue(   &RTOS_atssbRefQueue,
                                            (void*)&msgPtr,
                                            &msgLenDummy, RTOS_NO_WAIT)

                                == RTOS_REF_QUEUE_OK )
    {
        //take element out of the queue and set to debug comp
        ATSSB_setDataToDebugcomponent(  msgPtr->eventToken, msgPtr->eventData,
                                        msgPtr->eventDataLen );

        //free memory
        if(RTOS_releaseMemoryForRefQueue( &RTOS_atssbRefQueue, msgPtr )
                                != RTOS_REF_QUEUE_OK)
        {
            break;
        }

    }

    return TASK_INITIALISED;
}
#endif //ATSSB_RTOS_IS_USED

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
