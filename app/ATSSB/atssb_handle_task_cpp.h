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
 *  COMP_ABBREV      SSB_HUBC
 ******************************************************************************/

#ifndef ATSSB_HANDLE_TASK_CPP_H
#define ATSSB_HANDLE_TASK_CPP_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     atssb_handle_task_cpp.h
 *
 *  \ingroup  app/ATSSB
 *
 *  \brief    Interface of Application Task for Smart Sensor Bus, CPP-variant
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "bsh_stdinc.h"
#include <stdint.h>
#ifdef RTOS
    #include "rtos_types.h"
#endif


/******************************************************************************/
/* C-PREPROCESSOR DEFINITIONS                                                 */
/******************************************************************************/
/**
 * \brief   Dependencies for scheduler
 *
 */
#define DEP_ATSSB

/**
 * \brief   Auto-define ATSSB RTOS api
 *
 */
#ifdef RTOS
    #define ATSSB_RTOS_IS_USED
#endif

/**
 * \brief   Data which can maximal transmitted at once
 *
 */
#define ATSSB_CALLBACK_LOG_DATA_PART_LEN        (uint8_t) 25

/**
 * \brief   Data which can maximal transmitted at once
 *
 */
#define ATSSB_CALLBACK_LOG_DATA_LEN             (uint8_t) 82

/**
 * \brief   Data which can maximal transmitted at once
 *
 */
#define ATSSB_NUMBER_OF_CALLBACK_LOG_DATA_PARTS (uint8_t) 5
/******************************************************************************/
/* GLOBAL VARIABLES                                                           */
/******************************************************************************/
#ifdef ATSSB_RTOS_IS_USED
/**
 * \brief   control blocks for RTOS reference queue
 *
 */
extern RTOS_REF_QUEUE RTOS_atssbRefQueue;
#endif //ATSSB_RTOS_IS_USED

/******************************************************************************/
/* C FUNCTION PROTOTYPES                                                      */
/******************************************************************************/

#ifdef ATSSB_RTOS_IS_USED
/**
 * \brief   Releases all messages from queue, notified by eventflag
 *
 * \param   none
 *
 * \return  Taskstate
 *          -TASK_INITIALISED     - Task initialized and in run state
 *
 */
extern uint8_t ATSSB_getDataFromRefQueueReleaseMem(void);
#endif //ATSSB_RTOS_IS_USED


/**
 * \brief   Handletask for ATSSB
 *
 * \param   none
 *
 * \return  Taskstate
 *          -TASK_NOT_INITIALISED - Handler not initialized
 *          -TASK_INITIALISED     - Task initialized and in run state
 *
 */
extern uint8_t ATSSB_handleTask(void);

#ifdef __cplusplus
} // extern "C"


/******************************************************************************/
/* CLASS DEFINITION                                                           */
/******************************************************************************/
namespace ATSSB
{
    class Atssb_c
    {
    private:
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
        void setLoopCallbackDataToDebugcomponent(   uint16_t eventToken,
                                                    const uint8_t *eventDataPtr,
                                                    uint8_t eventDataLen );

    public:
        /**
         * \brief   State of initilization of underlaying SSB
         */
        bool SsbInitIsPassed = false;

        /**
         * \brief   Counter of calls of callback function
         */
        uint8_t CounterOfCallBackCalls = static_cast<uint8_t>(0U);

        /** \brief  Creates the object of this class and of the inherited
        *           classes
        */
        Atssb_c(void);

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
        void doForCallbackToApi(    uint16_t eventToken,
                                    const uint8_t *eventDataPtr,
                                    uint8_t eventDataLen);
    };
}
#endif

#endif //ATSSB_HANDLE_TASK_CPP_H

