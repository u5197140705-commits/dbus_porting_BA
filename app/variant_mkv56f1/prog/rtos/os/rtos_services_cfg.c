/**************************************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 **************************************************************************************************
 *   PROJECT          RTOS (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          23.11.2017
 **************************************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Configurations for module rtos_services_config.


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "BSH_stdinc.h"
#include "rtos_api.h"
#include "os/rtos_services_cfg.h"
#include "os/rtos_power_management.h"
#include "os/rtos_mutex.h"
#include "rtd/real_time_dispatcher.h"


/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/
#ifdef RTOS_TRACING_ENABLED

    #if !defined(RTOS_TRACING_MODE_STREAMING)
        #define RTOS_TRACE_BUFFER_SIZE (RTOS_NUMBER_OF_TRACE_EVENTS * 32)  /* Trace buffer size. Each event requires 32 byte. */
        /* Memory used by ThreadX for tracing. Needs the same alignment qualification as the member with the highest alignment qualifications in TX_TRACE_OBJECT_ENTRY. */
        uint8_t RTOS_traceBuffer[RTOS_TRACE_BUFFER_SIZE] __attribute__((aligned (4))); /*lint !e2666 The parameter of macro '__attribute__' is referenced in the expansion */
    #endif

/*lint -save -e10 -e16 'warning' (preprocessor directive) is known (e16) and intentionally used, also it does not requires an 'end of line' (e10) */
#warning "RTOS tracing is ON, please disable in case of a release build!"
/*lint -restore -e10 -e16 */
#endif

/**************************************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                                         */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                                    */
/**************************************************************************************************/
#ifdef RTOS_TRACING_ENABLED
static uint16_t RTOS_enableTraceLogging(void);
#endif

static uint16_t RTOS_createThreads(void);
static uint16_t RTOS_createTimers(void);
static uint16_t RTOS_createMutexes(void);
static uint16_t RTOS_createSemaphores(void);
static uint16_t RTOS_createEventFlags(void);
static uint16_t RTOS_createMessageQueues(void);
static uint16_t RTOS_createMemoryPools(void);
static void RTOS_ErrorAppCreationHandler(uint16_t overall_status);


/**************************************************************************************************/
/* LOCAL VARIABLE DEFINTIONS                                                           			  */
/**************************************************************************************************/
uint16_t RTOS_Error_AppCreation = 0U;

/* Threads(THR) */

/* Timers(TIM) */

/* Mutexes(MUT) */

/* Semaphores(SEM) */

/* EventFlagsGroup(EFG) */

/* MessageQueues(MSQ) */

/* MemoryPools(MBP) */

/* Allot individual Stack-areas for Threads */


/**************************************************************************************************/
/* EXTERN VARIABLE DEFINTIONS                                                              		  */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                                     */
/**************************************************************************************************/

#ifdef RTOS_TRACING_ENABLED
/*!
 *  \brief      A function for enabling RTOS trace logging.
 *
 *  \param      void
 *
 *  \return     uint16_t status
 *
 *  \details    The tx_application_define calls this function.
 */
static uint16_t RTOS_enableTraceLogging(void)
{
    uint16_t status;

    #if !defined(RTOS_TRACING_MODE_STREAMING)
        status = (uint16_t)tx_trace_enable(RTOS_traceBuffer, RTOS_TRACE_BUFFER_SIZE, RTOS_NUMBER_OF_TRACE_OBJECTS);
    #else
        status = (uint16_t)xTraceEnable(TRC_START_FROM_HOST);
    #endif

    return status;
}
#endif


/*!
 *  \brief      A function for creating threads during ThreadX setup process.
 *
 *  \param      void
 *
 *  \return     uint16_t status
 *
 *  \details    The tx_application_define calls this function for initial thread creation.
 */
static uint16_t RTOS_createThreads(void)
{
    uint16_t status = 0U;

    /* Create threads here */
    RTOS_createPowerManagementThread(); // Create default thread for Power Management

    return status;
}


/**
 *  \brief      A function for creating timers during ThreadX setup process.
 *
 *  \param      void
 *
 *  \return     uint16_t status
 *
 *  \details    The tx_application_define calls this function for initial timers creation.
 */
static uint16_t RTOS_createTimers(void)
{
    uint16_t status = 0U;

    /* Create timers here */

    return status;
}


/**
 *  \brief      A function for creating Mutexes during ThreadX setup process.
 *
 *  \param      void
 *
 *  \return     uint16_t status
 *
 *  \details    The tx_application_define calls this function for initial Mutexes creation.
 */
static uint16_t RTOS_createMutexes(void)
{
    uint16_t status = 0U;

    /* Create Mutexes here */

    return status;
}


/**
 *  \brief      A function for creating Semaphores during ThreadX setup process.
 *
 *  \param      void
 *
 *  \return     uint16_t status
 *
 *  \details    The tx_application_define calls this function for initial Semaphores creation.
 */
static uint16_t RTOS_createSemaphores(void)
{
    uint16_t status = 0U;

    /* Create Semaphores here */

    return status;
}


/**
 *  \brief      A function for creating Event Flags during ThreadX setup process.
 *
 *  \param      void
 *
 *  \return     uint16_t status
 *
 *  \details    The tx_application_define calls this function for initial Event Flags creation.
 */
static uint16_t RTOS_createEventFlags(void)
{
    uint16_t status = 0U;

    /* Create Event Flags here */

    return status;
}


/**
 *  \brief      A function for creating Message Queues during ThreadX setup process.
 *
 *  \param      void
 *
 *  \return     uint16_t status
 *
 *  \details    The tx_application_define calls this function for initial Message Queues creation.
 */
static uint16_t RTOS_createMessageQueues(void)
{
    uint16_t status = 0U;

    /* Create Message Queues here */

    return status;
}


/**
 *  \brief      A function for creating Memory Pools during ThreadX setup process.
 *
 *  \param      void
 *
 *  \return     uint16_t status
 *
 *  \details    The tx_application_define calls this function for initial Memory Pools creation.
 */
static uint16_t RTOS_createMemoryPools(void)
{
    uint16_t status = 0U;

    /* Create MemoryPools here */

    return status;
}


/**
 *  \brief      Callback for RTOS Services Creation Error
 *
 *  \param      void
 *
 *  \return     
 *
 *  \details
 */
static void RTOS_ErrorAppCreationHandler(uint16_t overall_status)
{
    /* Handle the Error Here */
    RTOS_Error_AppCreation += overall_status;
    
    for(;;)
    {
        //! Hard Fault - Error during RTOS Services Creation
    }
}


/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/
/**
 *  \brief      Sets up Real time environment
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    Called from rtos_power_management for MicriumOS/µC/OS-III; else for other RTOS from RTOS_createRTOSservices
 */
void RTOS_setupRealTimeEnvironment(void)
{

    uint16_t overall_status = 0U;
    
    RTD_setupRealTimeDispatcher(&RTD_DefaultProjectCatalogue);
    overall_status += RTD_createRTDservices();
    
    if(overall_status != 0U)
    {
        RTOS_ErrorAppCreationHandler(overall_status);
    }

}


/**
 *  \brief      Create all the needed RTOS Services
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    Called from high level initialization
 */
void RTOS_createRTOSservices(void)
{
    uint16_t overall_status = 0U;

#ifdef RTOS_TRACING_ENABLED
    overall_status += RTOS_enableTraceLogging();
#endif

   overall_status += RTOS_createMutexes();

    overall_status += RTOS_createSemaphores();

    overall_status += RTOS_createEventFlags();

    overall_status += RTOS_createMessageQueues();

    overall_status += RTOS_createMemoryPools();

    overall_status += RTOS_createTimers();
    
    overall_status += RTOS_createThreads();

    if(overall_status != 0U)
    {
        RTOS_ErrorAppCreationHandler(overall_status);
    }
    
#ifdef OS_THREADX
    RTOS_setupRealTimeEnvironment();
#else
    //As per Micrium Requirements, further threads must be created from a Thread, therefore Thread Power Management creates further Threads for RTD 
#endif
}

/*************************************** End of File **********************************************/
