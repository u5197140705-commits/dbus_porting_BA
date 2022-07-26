/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          RTOS RTD (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          25.01.2018
 ******************************************************************************/


/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/*!
 *
 *  \file
 *  \brief User application specific configurations for real time dispatcher.
 *
 */


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "rtd/rtd_types.h"
#include "rtd/rtd_defines.h"
#include "rtd/real_time_dispatcher_cfg.h"
#include "rtd/real_time_dispatcher.h"
#include "bal.h"
#include "dbuspresentation.h"
#include "dbusdll.h"
#include "timerlib.h"
#include "watchdogtimer/watchdogtimer.h"
#ifdef RTOS_MODE_DEBUG
    #include "rtd/rtd_time_monitor.h"
#endif
#ifdef CCSS
    #include "CCSS_SyStTask.h"
#endif
#ifdef CCSS_DOM_REF
    #include "CCDOM_DomainRefImpl.h"
#endif


/***************************************************************************************************
 * RTD: Task List Readability
 ***************************************************************************************************
 * \brief: For better configuration readability of RTD_TasksList below, Thread_IDs can be redefined
 *         here by user if needed or the ThreadIDs can also be directly used.
 * *************************************************************************************************
 * \note:  1. Please fill up the appropriate <period> before using the redefinition of periodic thread.
 *         2. Please comment out or remove the definitions of the ThreadIDs if not in use.
 ***************************************************************************************************/
#define THREAD_BLUEBIRD                 FT_ID_BLUEBIRD        //< RESERVED for Framework Tasks only
//#define THREAD_ULTRA_FAST_1MS         PT_ID_ULTRA_FAST
//#define THREAD_SUPER_FAST_2MS         PT_ID_SUPER_FAST
//#define THREAD_FAST_<Period>          PT_ID_FAST
//#define THREAD_MEDIUM_10MS            PT_ID_MEDIUM
//#define THREAD_SLOW_100MS             PT_ID_SLOW
//#define THREAD_ORYX                   ET_ID_ORYX


/***************************************************************************************************
 * CONFIG: DEPENDENCY ARRAYS
 * *************************************************************************************************
 * \brief: Create Module dependency arrays for each module here, terminate the array with END_STAMP
 * *************************************************************************************************
 * \note: 1. No Circular Dependencies allowed!
          2. Example:-
             #define DEP_BAL    MOD_TIM, MOD_DLL, -> Register Module Dependencies, in module header files
             const TaskID BAL_dep[]  =  {DEP_BAL  END_STAMP}; -> Create the dependency array below,
                                                                 with the Dependency MACRO
 ***************************************************************************************************/
const TaskID BAL_dep[]      =  {DEPLIB_BAL  END_STAMP};
const TaskID DBPL_dep[]     =  {DEPLIB_DBPL END_STAMP};
const TaskID DLL_dep[]      =  {DEPLIB_DLL  END_STAMP};
const TaskID TIM_dep[]      =  {DEP_TIM     END_STAMP};
const TaskID WDT_dep[]      =  {DEP_WDT     END_STAMP};
#ifdef CCSS
const TaskID CCSS_dep[] = {DEP_CCSS    END_STAMP};
#endif
#ifdef CCSS_DOM_REF
const TaskID CCDOM_dep[] = {DEP_CCDOM   END_STAMP};
#endif
const TaskID SYSL_dep[]     =  {END_STAMP};
/*! place your module dependencies here */



/**!
 * \brief       Main Project Catalog Structure
 *
 * \details     It contains following Tables,
 *              TasksList           - Task line-up for scheduling
 *              TaskHandlersList    - TaskHandler functions
 *              DependenciesList    - Module Dependency
 */
const struct RTD_ProjectInventory RTD_DefaultProjectCatalogue =
{
    /***************************************************************************************************
    * CONFIG: MAIN TASK LIST
    * *************************************************************************************************
    * \brief: Add ModuleIDs from RTD_ModuleIDs and assign them appropriate Thread IDs here.
    *
    *         Scheduling Mode Options are as follows -
    *         THREAD_ULTRA_FAST_<Period> / (PT_ID_ULTRA_FAST) -> For ULTRA FAST Thread
    *         THREAD_SUPER_FAST_<Period> / (PT_ID_SUPER_FAST) -> For SUPER FAST Thread
    *         THREAD_FAST_<Period>       / (PT_ID_FAST)       -> For FAST Thread
    *         THREAD_MEDIUM_<Period>     / (PT_ID_MEDIUM)     -> For MEDIUM Thread
    *         THREAD_SLOW_<Period>       / (PT_ID_SLOW)       -> For SLOW Thread
    *         THREAD_ORYX                / (ET_ID_ORYX)       -> For Event Thread ORYX
    ***************************************************************************************************/
    .RTD_TasksList =
    {
        {MOD_BAL,       THREAD_BLUEBIRD},
        {MOD_DBPL,      THREAD_BLUEBIRD},
        {MOD_DLL,       THREAD_BLUEBIRD},
        {MOD_TIM,       THREAD_BLUEBIRD},
        {MOD_SYSL,      THREAD_BLUEBIRD},
        {MOD_WDT,       THREAD_BLUEBIRD},
#ifdef CCSS
        {MOD_CCSS,      THREAD_BLUEBIRD},
#endif
#ifdef CCSS_DOM_REF
        {MOD_CCDOM,     THREAD_BLUEBIRD},
#endif
        /*! place project tasks and their thread IDs here */


        {END_STAMP,     SCH_NONE}
    },

    /***************************************************************************************************
    * CONFIG: TASK HANDLER TABLE
    * *************************************************************************************************
    * \brief: Add corresponding task handlers for each module here
    ***************************************************************************************************/
    .RTD_TaskHandlersList =
    {
        [MOD_BAL]       =  BAL_HandleTask,
        [MOD_DBPL]      =  DBPL_HandleTask,
        [MOD_DLL]       =  DLL_HandleTask,
        [MOD_TIM]       =  TIM_handleTask16,  /* TIM_handleTask if no 16Bit timer used */
        [MOD_SYSL]      =  RTD_monitorSystemLoad,
        [MOD_WDT]       =  WDT_handleTask,
#ifdef CCSS
        [MOD_CCSS]      =  CCSS_RunSyStTask,
#endif
#ifdef CCSS_DOM_REF
        [MOD_CCDOM]     =  CCDOM_RunDomRefTask,
#endif
        /*! place project task handlers here */


    },

    /***************************************************************************************************
    * CONFIG: DEPENDENCY TABLE
    * *************************************************************************************************
    * \brief: Add above module dependency arrays in the following Table
    ***************************************************************************************************/
    .RTD_DependenciesList =
    {
        [MOD_BAL]   =  BAL_dep,
        [MOD_DBPL]  =  DBPL_dep,
        [MOD_DLL]   =  DLL_dep,
        [MOD_TIM]   =  TIM_dep,
        [MOD_WDT]   =  WDT_dep,
        [MOD_SYSL]  =  SYSL_dep,
#ifdef CCSS
        [MOD_CCSS]  =  CCSS_dep,
#endif
#ifdef CCSS_DOM_REF
        [MOD_CCDOM] =  CCDOM_dep,
#endif
        /* place module dependency arrays here */


    },
};

#ifdef EVENT_DRIVEN_SCHEDULER
/***************************************************************************************************
 * CONFIG: EVENT TASKS LIST - MAX 32 TASKS
 * *************************************************************************************************
 * \brief: Register a Event Driven Task in the Table below.
 * *************************************************************************************************
 * \note: Each Registered Event Driven Task gets a unique Task Flag.
 *        To Run this Task, simply use the following API from rtos_api.h:
 *        uint32_t RTOS_RunEventDrivenTask(uint32_t task_flag, ThreadID event_thread_id);
 *        
 *        Maximum allowed Events per Thread for Application are limited to 32, ranging from [0] to [31].
 *        
 *        Please use following format:
 *        MOD_ID1,
 *        MOD_ID2, ...
 *        
 *        The Order at which the Tasks are registered below, determine the Priority of the Task.
 *        The lower the FLAG number, higher the priority.
 *        Therefore if at the same time numerous flags are set, then the Modules will be called
 *        in the same order as they are registered below.
 ***************************************************************************************************/
 #if(EVENT_THREAD_ORYX == THREAD_ACTIVATED)
const TaskID RTD_EventTasksList_ORYX[] =
{
    /* Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_oryx_tasks = (uint8_t)(sizeof(RTD_EventTasksList_ORYX)/sizeof(RTD_EventTasksList_ORYX[0]));
#endif
#endif


/***************************************************************************************************
 * CONFIG: MANAGED ISR FUNCTIONS LIST
 * *************************************************************************************************
 * \brief: Add Managed ISR Functions here, please use void-void functions
 * *************************************************************************************************
 * \note: Each position in the following array represents the Flag of that ISR Function, to call the
 *        specific ISR function, please set the corresponding Flag using the following API from rtos_api.h,
 *        uint32_t RTOS_RunISRFromThread(uint32_t isr_flag);
 *
 *        The Sequence at which the ISR functions are registered in the table, determine the Priority.
 *        The lower the FLAG number, higher the priority.
 *        Therefore if at the same time numerous flags are set, then the Functions will be called
 *        in the same order as they are registered below.
 ***************************************************************************************************/
const RTD_ICSHandlers RTD_ISRFunctionsList[] =
{
    /* Place Your ISR Critical Section Functions here */
    RTD_ExampleISR_Handler, /* FLAG_0 */  //< Example! Please replace it with user ISR Handler


    /*! DO NOT exceed 32 Interrupt Functions */
};

/* Auto Determination of number of application ISRs */
const uint8_t RTD_number_of_interrupts = (uint8_t)(sizeof(RTD_ISRFunctionsList)/sizeof(RTD_ISRFunctionsList[0]));


/**************************************************************************************************/
/* CONFIG: RTD RTOS SERVICES                                                                      */
/**************************************************************************************************/

#if (RTD_SERVICE_MUTEX_EN == FEATURE_ACTIVATED)
/***************************************************************************************************
 * MUTEXES (MUT)
 * *************************************************************************************************
 *  \brief Create Application MUTEXES below.
 *         To create MUTEXES somewhere else in the application, include 'real_time_dispatcher_types.h'
 *         header file to use below Formats.
 *
 *         Examples/Templates for MUTEX Creation and Initialization:-
 *
 *         1. RTOS_MUTEX mutex1 =
 *            {
 *                .mutex_name = "mutex_example",   --> Give a MUTEX Human Readable name.
 *                                                     priority inheritance is always turned on,

 *            };
 *
 *         2. RTOS_MUTEX mutex1, mutex2;           --> In this case, MUTEX is not assigned

 * *************************************************************************************************
 * \note   1. Register the MUTEXES below in RTD_MutexList[].
 *         2. To use the MUTEXES in Application, use following APIs:-
 *            uint8_t RTOS_EnterCriticalSection(RTOS_MUTEX* mutex); --> Get MUTEX
 *            uint8_t RTOS_ExitCriticalSection(RTOS_MUTEX* mutex);  --> Release MUTEX
 ***************************************************************************************************/
/* Create MUTEXES here */


/* Import mutexes created in other modules here */
    extern RTOS_MUTEX mutex_BAL;
    extern RTOS_MUTEX mutex_CCCM;
    extern RTOS_MUTEX mutex_CCPO;


/***************************************************************************************************
 * CONFIG: RTD MUTEX LIST
 * *************************************************************************************************
 * \brief: Register all the created MUTEX in the following list.
 * *************************************************************************************************
 * \note:    Use following format;
 *           &mutex_example1,
 *           &mutex_example2, ...
 **************************************************************************************************/
    const RTOS_MUTEX* RTD_MutexList[] =
    {
        &mutex_BAL,
    #ifdef CCSS
        &mutex_CCCM,
        &mutex_CCPO,
    #endif

    /*! place your mutexes here */


};

/* Auto Determination of number of application MUTEXES */
    const uint8_t number_of_mutexes = (uint8_t)(sizeof(RTD_MutexList)/sizeof(RTD_MutexList[0]));

#endif

#if (RTD_SERVICE_FUTEX_EN == FEATURE_ACTIVATED)
/***************************************************************************************************
 * FUTEXES (FUT) Futex is same as Mutex, only difference is the Priority Inheritance is deactivated.
 * *************************************************************************************************
 *  \brief Create Application FUTEXES below.
 *         To create FUTEXES somewhere else in the application, include 'real_time_dispatcher_types.h'
 *         header file to use below Formats.
 *
 *         Examples/Templates for FUTEX Creation and Initialization:-
 *
 *         1. RTOS_MUTEX futex1 =
 *            {
 *                .mutex_name = "mutex_example",   --> Give a FUTEX Human Readable name.
 *                                                     priority inheritance is always turned off.
 *            };
 *
 *         2. RTOS_MUTEX futex1, futex2;           --> In this case, FUTEX is not assigned
 *
 * *************************************************************************************************
 * \note   1. Register the MUTEXES below in RTD_MutexList[].
 *         2. To use the MUTEXES in Application, use following APIs:-
 *            uint8_t RTOS_EnterCriticalSection(RTOS_MUTEX* futex); --> Get MUTEX
 *            uint8_t RTOS_ExitCriticalSection(RTOS_MUTEX* futex);  --> Release MUTEX
 ***************************************************************************************************/
/* Create MUTEXES here */


/* Import futexes created in other modules here */
//extern RTOS_MUTEX futex_XXX;
//extern RTOS_MUTEX futex_XXX;


/***************************************************************************************************
 * CONFIG: RTD FUTEX LIST
 * *************************************************************************************************
 * \brief: Register all the created FUTEX in the following list.
 * *************************************************************************************************
 * \note:    Use following format;
 *           &futex_example1,
 *           &futex_example2, ...
 **************************************************************************************************/
    const RTOS_MUTEX* RTD_FutexList[] =
    {

        /*! place your futexes here */


    };



/* Auto Determination of number of application FUTEXES */
    const uint8_t number_of_futexes = (uint8_t)(sizeof(RTD_FutexList)/sizeof(RTD_FutexList[0]));

#endif
/***************************************************************************************************
 * CONFIG: RTD TIMEOUT CALLBACK
 * *************************************************************************************************
 * \brief: If timing deadline of any Thread is not met, then the following callback function is called.
 *         Thread for which the timeout has occurred, can be identified with the parameter 'thread_id'.
 * *************************************************************************************************/
void RTD_WarningExecutionTimeout(const ThreadID thread_id)
{
    (void)thread_id;

    /* Do your stuff here */

}


/***************************************************************************************************
 * DEBUG FEATURE: STACK OVERFLOW ERROR NOTIFICATION
 * *************************************************************************************************
 * \brief: If a Stack Overflow occurs for any Thread, the following Callback will be called and the
 *         Error is notified by setting the variable RTD_RuntimeCatalogue.RuntimeInfo.Error_StackOverflow 'true'.
 *         The Thread is then Terminated and the System is RESET or in Developer Mode a Hard Fault
 *         is Activated.
 *
 *         The Parameter 'thread_id' identifies Thread for which the Stack Error has occurred.
 *
 *         STACK_OVERFLOW_DEVELOPER_MODE can be activated from the section DEVELOPER MODE SETTINGS
 *         in file real_time_dispatcher_cfg.h. Please check the same section also for detailed
 *         information about DEVELOPER MODE and SYSTEM MODE.
 * *************************************************************************************************
 * \note:  To Fix the Stack Error, the Stack Size of the Thread must be increased and can be done in
 *         CONFIG: RTD THREAD STACK SIZE in real_time_dispatcher_cfg.h header file.
 * *************************************************************************************************/
void RTD_ErrorStackOverflowHandler(const ThreadID thread_id)
{
    (void)thread_id;

    /*!
     *  ERROR - Stack Overflow!
     *
     *  Stack Overflow occurred for the parameter 'thread_id', the Thread is terminated!
     *  In System Mode the System will be Reset after this callback, while in Developer Mode
     *  a Hard fault as an endless loop will be generated!
     *  Therefore please make sure to turn off all the safety critical components such as
     *  Heaters, Motors etc here.
     */

    /* Handle the Error Here */


}


#ifdef RTOS_MODE_DEBUG
/***************************************************************************************************
 * DEBUG FEATURE - SYSTEM OVERLOADED ERROR NOTIFICATION
 * *************************************************************************************************
 * \brief: Following Callback is called whenever System Overload is detected.
 *         The System Overloaded Error MUST be handled here by the user.
 *
 *         System Load is calculated per Hyperperiod.
 *         To make sure that the System remains Feasible and Deterministic, MAXIMUM allowed
 *         System Load is 70%, please make sure your System does not overshoot this Limit.
 *         In case of an overshoot, following Error Condition is Activated and user is notified
 *         about the error with 'RTD_RuntimeCatalogue.RuntimeInfo.Error_SystemOverloaded'.
 *
 *         Check the Variable 'RTD_RuntimeCatalogue.RuntimeInfo.SystemLoad' to obtain the exact System Load in Percentage.
 *
 *         SYSTEM_OVERLOAD_DEVELOPER_MODE can be activated from the section DEVELOPER MODE SETTINGS
 *         in file real_time_dispatcher_cfg.h. Please check the same section also for detailed
 *         information about DEVELOPER MODE and SYSTEM MODE.
 * *************************************************************************************************/
void RTDTM_ErrorSystemOverloadedHandler(void)
{
    /*!
     * ERROR: System Overloaded!
     *
     * Maximum allowed System Load is 70% to guarantee the System Feasibility!
     * Please check the value of Variable 'RTD_RuntimeCatalogue.RuntimeInfo.SystemLoad' to determine your system load in Percentage.
     * Make appropriate changes to reduce the load below 70%.
     *
     * Tip: The tasks configured at 1ms Period add the highest System Load.
     *
     */

    /* Handle the Error Here */
#if(SYSTEM_OVERLOAD_DEVELOPER_MODE == FEATURE_ACTIVATED)
    /* Error Handling for Developer Mode
     *
     * In DEVELOPER MODE, a Hard Fault as an endless loop is generated after this callback.
     * Therefore please make sure to turn off all the safety critical components such as
     * Heaters, Motors etc here.
     */

#else
    /*Error Handling for System Mode*/

#endif

}
#endif

/********************************************* End of File ******************************************************/
