/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          RTOS (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          23.11.2017
 ******************************************************************************/


/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Implementation of module rtos_services_config.

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "rtos_api.h"
#include "tx_api.h"
#include "os/rtos_services_cfg.h"
#include "os/rtos_defines.h"
#include "os/rtos_types.h"
#ifdef REAL_TIME_DISPATCHER
#include "rtd/real_time_dispatcher.h"
    extern struct RTD_ProjectInventory RTD_DefaultProjectCatalogue;
#endif


/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                                         */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                                    */
/**************************************************************************************************/
static uint16_t RTOS_createThreads(void);
static uint16_t RTOS_createTimers(void);
static uint16_t RTOS_createMutexes(void);
static uint16_t RTOS_createSemaphores(void);
static uint16_t RTOS_createEventFlags(void);
static uint16_t RTOS_createMessageQueues(void);
static uint16_t RTOS_createMemoryPools(void);

#if(RTOS_THREAD_IDLE == THREAD_ACTIVATED)
static void RTOS_IdleThread(ULONG thread_input);
#endif

/**************************************************************************************************/
/* LOCAL VARIABLE DEFINTIONS                                                           			  */
/**************************************************************************************************/
uint16_t RTOS_Error_AppCreation = 0U;

/* Threads(THR) */
#if(RTOS_THREAD_IDLE == THREAD_ACTIVATED)
TX_THREAD   RTOS_THR_IDLE;
#endif

/* Timers(TIM) */

/* Mutexes(MUT) */

/* Semaphores(SEM) */

/* EventFlagsGroup(EFG) */

/* MessageQueues(MSQ) */

/* MemoryPools(MBP) */


/* Allot individual Stack-areas for Threads */
#if(RTOS_THREAD_IDLE == THREAD_ACTIVATED)
UCHAR RTOS_STACK_IdleThread[DEFAULT_STACK_SIZE/4U];
#endif


/*! Names for ThreadX Components should be given here
 ** This is to avoid MISRA Required:- Info 1776: Converting a string literal to CHAR * is not const safe (arg. no. 2) [MISRA 2012 Rule 7.4, required] **
 */
#if(RTOS_THREAD_IDLE == THREAD_ACTIVATED)
CHAR RTOS_name_THR_Idle[5] = "Idle";
#endif


/**************************************************************************************************/
/* EXTERN VARIABLE DEFINTIONS                                                              		  */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                                     */
/**************************************************************************************************/
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
#if(RTOS_THREAD_IDLE == THREAD_ACTIVATED)
    //Default - Idle Thread with lowest priority
    status += (uint16_t)tx_thread_create(&RTOS_THR_IDLE, RTOS_name_THR_Idle, RTOS_IdleThread, DEFAULT_INPUT, RTOS_STACK_IdleThread,
            DEFAULT_STACK_SIZE, (LOWEST_PRIORITY _TUNE_LOWER), (LOWEST_PRIORITY _TUNE_LOWER), TX_NO_TIME_SLICE, TX_AUTO_START);
#endif


    return status;
}


/*!
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


/*!
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


/*!
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


/*!
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


/*!
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


/*!
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


#if(RTOS_THREAD_IDLE == THREAD_ACTIVATED)
/*!
 *  \brief      An Idle thread with lowest Priority that executes only when there is no other Application Ready Thread to execute.
 *
 *  \param      ULONG  thread_input
 *
 *  \return void
 *
 *  \details
 */
static void RTOS_IdleThread(ULONG thread_input)
{
    (void)thread_input;

    for(;;)
    {
        /* Your code here */
    }
}
#endif

/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/
/*!
 *  \brief      Sets up Real time environment
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    Called from basicXS
 */
void RTOS_setupRealTimeEnvironment(void)
{
#ifdef REAL_TIME_DISPATCHER
    RTD_setupRealTimeDispatcher(&RTD_DefaultProjectCatalogue);
#endif
}


/*!
 *  \brief      A default ThreadX function called during the setup process for creating
 *              initial ThreadX objects.
 *  
 *  \param      void *first_unused_memory - pointer to the first unused memory.
 *  
 *  \return     void
 *  
 *  \details    The tx_application_define function executes after the basic ThreadX initialization
 *              is complete. It is responsible for setting up all of the initial system resources,
 *              including threads, queues, semaphores, mutexes, event flags, and memory pools.
 */
void tx_application_define(const VOID *first_unused_memory)
{
    uint32_t unused_var = *(const uint32_t*)first_unused_memory;
    (void)unused_var;   // To avoid MISRA Warning

    uint16_t overall_status = 0U;

#ifdef REAL_TIME_DISPATCHER
    overall_status += RTD_createOSservices();
#endif
    overall_status += RTOS_createThreads();

    overall_status += RTOS_createTimers();

    overall_status += RTOS_createMutexes();

    overall_status += RTOS_createSemaphores();

    overall_status += RTOS_createEventFlags();

    overall_status += RTOS_createMessageQueues();

    overall_status += RTOS_createMemoryPools();

    if(overall_status != RTOS_SUCCESS)
    {
        RTOS_Error_AppCreation = overall_status;
    }
}


/********************************************* End of File ******************************************************/
