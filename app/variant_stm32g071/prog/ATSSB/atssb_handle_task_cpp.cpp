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
/** \file     atssb_handle_task.cpp
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
extern "C" {

#include "atssb_handle_task_cpp.h"
#include "utility.h"
#include "debug_mapping.h"
#include "ssbf_common_c.h"
}

#include "../sbus_abstraction/constellation/hub_mngr.h"


using namespace ::SSBAL::SSBCO;
/******************************************************************************/
/* DEFINITIONS AND DECLARATIONS                                               */
/******************************************************************************/


/******************************************************************************/
/* STATIC TYPEDEFINITIONS                                                     */
/******************************************************************************/


/******************************************************************************/
/* OBJECTS                                                                    */
/******************************************************************************/
namespace SSBAL
{
    namespace SSBCC
    {
        class HubMngr_c ATSSB_HubObject;
    }
}


/******************************************************************************/
/* STATIC VARIABLES                                                           */
/******************************************************************************/
/**
 * \brief   state of handle task of this module
 *
 */
static uint8_t ATSSB_taskState = TASK_NOT_INITIALISED;
static bool    ATSSB_SsbInitIsPassed = false;

/******************************************************************************/
/* STATIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
extern "C" {

/**
 * \brief   Callback function of SSB
 *
 * \param   calleeObjPtrToHere  Object pointer
 *          eventToken          Contains Hub-Index and further elements according to
 *                              ssbf_common_c.h
 *          eventDataPtr        Pointer to the data delivered (e.g. the loop results)
 *          eventDataLen        Number of bytes delivered via eventDataPtr
 *
 * \return  none
 */
static void ATSSB_doForCallbackToApi(
                   void *calleeObjPtrToHere, uint16_t eventToken,
                   const uint8_t *eventDataPtr, uint8_t eventDataLen);


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
static void ATSSB_setLoopCallbackDataToDebugcomponent(uint16_t eventToken,
                                                      const uint8_t *eventDataPtr,
                                                      uint8_t eventDataLen );


/******************************************************************************/
/* OBJECTS                                                                    */
/******************************************************************************/

/******************************************************************************/
/* STATIC FUNCTION DEFINITION                                                 */
/******************************************************************************/
static void ATSSB_setLoopCallbackDataToDebugcomponent(uint16_t eventToken,
                                                      const uint8_t *eventDataPtr,
                                                      uint8_t eventDataLen )
{
    uint8_t index;
    uint8_t eventDataLenTemp;
    uint8_t *eventDataPtrTemp = (uint8_t*)eventDataPtr; //lint !e926 !e954 !e9005 !e1773 convert in non-const done intentionally

    DBGX_logStr_SCN_SSB_CBACK_APP("\n");
    /********************* eventToken *********************/
    DBGX_logStr_SCN_SSB_CBACK_APP(
            "ATSSB_doForCallbackToApi: eventToken:" );
    DBGX_logInt_SCN_SSB_CBACK_APP  (
            eventToken,
            DBGX_UINT8_HEXADECIMAL      );

    /********************* eventDataLen *******************/
    DBGX_logStr_SCN_SSB_CBACK_APP(
            "ATSSB_doForCallbackToApi: eventDataLen:" );
    DBGX_logInt_SCN_SSB_CBACK_APP  (
            eventDataLen,
            DBGX_UINT8_HEXADECIMAL      );

    /********************* *eventDataPtr ******************/
    DBGX_logStr_SCN_SSB_CBACK_APP(
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

        DBGX_logIntArr_SCN_SSB_CBACK_APP   (
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


static void ATSSB_doForCallbackToApi(
                   void *calleeObjPtrToHere, uint16_t eventToken,
                   const uint8_t *eventDataPtr, uint8_t eventDataLen)
{
    (void)calleeObjPtrToHere;

    // Especially the measurement data are available in the data under
    // eventDataPtr for the callback of ATSSB_HubObject.startLoop.

    // Call a member function of your application object here.

    // The calleeObjPtrToHere is the passed object pointer from notifyCallbackToApi.
    // (Is not needed with only one Hub instantiated.)

    // The eventToken tells the instance index (0 ... 3) of the instantiated Hub in
    // the bits 14 and 13. (Is not needed with only one Hub instantiated.)
    // Definitions for getting the Client index from eventToken:
    //     SSB_EVT_HUB_SHIFT 13
    //     SSB_EVT_HUB_MASK
    //     SSB_EVT_HUB_0
    //     SSB_EVT_HUB_1
    //     SSB_EVT_HUB_2
    //     SSB_EVT_HUB_3

    //Debug output

    switch (eventToken)
    {
        case SSB_EVT_LOAD_CFG_UP:
            ATSSB_SsbInitIsPassed = true;

            DBGX_logStr_SCN_SSB_CBACK_APP("Init callback");
            break;

        case SSB_EVT_LOOP_START_UP:
            DBGX_logStr_SCN_SSB_CBACK_APP("Loop callback");
            ATSSB_setLoopCallbackDataToDebugcomponent(
                eventToken, eventDataPtr, eventDataLen);
            break;

        default:
            break;
    }
}


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
uint8_t ATSSB_handleTask(void)
{
    switch (ATSSB_taskState)
    {
        case TASK_NOT_INITIALISED:
        {
            #ifdef DBGX_INCLUDED
                DBGX_init();
            #endif

            SSBAL::SSBCC::ATSSB_HubObject.notifyCallbackToApi(nullptr, ATSSB_doForCallbackToApi);

            DBGX_logStr_SCN_SSB_CDIRECT_APP("Init call");
            SSBAL::SSBCC::ATSSB_HubObject.initHubMngr(
                SSB_CFG_IDX,                    // As defined in hub_mngr.h
                                                // Is filled in in the array of
                                                // configurations in ssb_config_auto.cpp

                SSB_I2C_ADDR_OFFSETS_DEFAULT,   //SSBAL_I2C_ADDR_OFFSETS_DEFAULT, // Correspondents to the following
                                                // EEPROM-configurations of the Hubs,
                                                // burned by the configuration tool:
                                                // Offset 0 for Hub 0
                                                // Offset 1 for Hub 1
                                                // Offset 2 for Hub 2
                                                // Offset 3 for Hub 3
                                                // Normally to be used for SW-applications
                                                // of SSB

                SSB_CFG_TO_BE_LOADED            // Normally to be used for SW-applications
                                                // of SSB
            );

            ATSSB_taskState = TASK_INITIALISED;
            break;
        }
        case TASK_INITIALISED:
        {
            if (ATSSB_SsbInitIsPassed != false)
            {
                ATSSB_SsbInitIsPassed = false;

                DBGX_logStr_SCN_SSB_CDIRECT_APP("Loop start call");
                SSBAL::SSBCC::ATSSB_HubObject.startLoop(SSB_INFINITE_LOOP);
            }

            // Recommendation for user's actions:
            //
            // if (ATSSB_SsbNewLoopDataAreAvailable != false)
            // {
            //    ATSSB_SsbNewLoopDataAreAvailable = false;
            //
            //    # Longer lasting actions (over 10 us) can be done here with the loop data,
            //    # with data transferred via a pointer.
            //
            //    # Reason: We have the actions on the right task priority of application
            //    # task here (callback has mostly a higher priority of SSB-provider-SW).
            // }

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

} //extern "C"

