/***************************************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 ***************************************************************************************************
 *   PROJECT          RTOS RTD (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          25.01.2018
 **************************************************************************************************/


/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/**
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
#include "rtos_msg_queue.h"
#include "rtos_ref_queue.h"
#include "rtos_memory_management.h"
#include "rtos_mailbox.h"
#include "rtd/rtd_defines.h"
#include "rtd/real_time_dispatcher_cfg.h"
#include "rtd/real_time_dispatcher.h"
#include "rtd/rtd_timer.h"
#include "rtd/rtd_event_flag.h"
#include "dbus_rtos_interface.h"
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
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    #include "rtos_functional_safety.h"
#endif
#ifdef BLUETOOTH_MODULE
/*! place bluetooth module includes here */


#endif
#include "atssb_handle_task.h"

/***************************************************************************************************
 * RTD: Task List Readability
 ***************************************************************************************************
 * \brief: For better configuration readability of RTD_TasksList below, Thread_IDs can be redefined
 *         here by user if needed or the ThreadIDs can also be directly used.
 * *************************************************************************************************
 * \note:  Please comment out or remove the definitions of the ThreadIDs if not in use.
 ***************************************************************************************************/
#if(FRAMEWORK_THREAD_BLUEBIRD == THREAD_ACTIVATED)
    #ifdef RTOS_SINGLE_THREAD_APPLICATION_ENABLED
        #define THREAD_BLUEBIRD         PT_ID_STA                //< RESERVED for Framework Tasks only
    #else
        #define THREAD_BLUEBIRD         RTOS_THREAD_ID_FW        //< RESERVED for Framework Tasks only
    #endif
#endif
#if(FRAMEWORK_THREAD_DBUS == THREAD_ACTIVATED)
    #ifdef RTOS_SINGLE_THREAD_APPLICATION_ENABLED
        #define THREAD_DBUS             PT_ID_STA                //< RESERVED for DBUS Tasks only
    #else
        #define THREAD_DBUS             RTOS_THREAD_ID_DBUS      //< RESERVED for DBUS Tasks only
    #endif
#endif
//#define THREAD_ULTRA_FAST_1MS     PT_ID_ULTRA_FAST
//#define THREAD_SUPER_FAST_2MS     PT_ID_SUPER_FAST
//#define THREAD_FAST_5MS           PT_ID_FAST
#define THREAD_MEDIUM_10MS        PT_ID_MEDIUM
//#define THREAD_SLOW_100MS         PT_ID_SLOW
//#define THREAD_DCM_1MS            PT_ID_DCM
//#define THREAD_SPIA_1MS           PT_ID_SPIA
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    #define THREAD_FSF             PT_ID_FSF
#endif
#ifdef RTOS_SINGLE_THREAD_APPLICATION_ENABLED
    #define THREAD_STA              PT_ID_STA
#endif
#ifdef BLUETOOTH_MODULE
    /* Bluetooth Module Threads */
    //#define THREAD_BLE_LL         ET_ID_BLE_LL
    //#define THREAD_BLE_HIGH       ET_ID_BLE_HIGH
    //#define THREAD_BLE_MEDIUM     ET_ID_BLE_MEDIUM
    //#define THREAD_BLE_LOW        ET_ID_BLE_LOW
#endif
//#define THREAD_ORYX               ET_ID_ORYX
//#define THREAD_SPIX_MACAW         ET_ID_SPIX_MACAW
//#define THREAD_VAQUITA            ET_ID_VAQUITA
//#define THREAD_KAKAPO             ET_ID_KAKAPO
//#define THREAD_WEKA               ET_ID_WEKA


/***************************************************************************************************
 * RTD: Task List Scheduling Interval
 ***************************************************************************************************
 * \brief: These are standard definitions for the task scheduling intervals
 ***************************************************************************************************
 * \detail: For usage please see definition of \see RTD_TasksList below
 ***************************************************************************************************/
#define RTD_DEFAULT_INTERVAL        (1U)
#define RTD_INTERVAL_COUNTER_INIT   (0U)

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
static const TaskID BAL_dep[]      =  {DBR_DEPLIB_BAL   END_STAMP};
static const TaskID DBPL_dep[]     =  {DBR_DEPLIB_DBPL  END_STAMP};
static const TaskID DLL_dep[]      =  {DBR_DEPLIB_DLL   END_STAMP};
static const TaskID TIM_dep[]      =  {DEP_TIM          END_STAMP};
static const TaskID WDT_dep[]      =  {DEP_WDT          END_STAMP};
#ifdef CCSS
static const TaskID CCSS_dep[]     =  {DEP_CCSS         END_STAMP};
#endif
#ifdef CCSS_DOM_REF
static const TaskID CCDOM_dep[]    =  {DEP_CCDOM        END_STAMP};
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
static const TaskID FSF_dep[]      =  {DEP_FSF          END_STAMP};
#endif
static const TaskID SYSL_dep[]     =  {                 END_STAMP};
/*! place your module dependencies here */
static const TaskID ATSSB_dep[]    = {DEP_ATSSB END_STAMP};

#ifdef BLUETOOTH_MODULE
/*! place BTM dependencies here */

#endif


/**!
 * \brief       Main Project Catalog Structure
 *
 * \details     It contains following Tables,
 *              TasksList           - Task line-up for scheduling
 *              TaskHandlersList    - TaskHandler functions
 *              DependenciesList    - Module Dependency
 */
struct RTD_ProjectInventory RTD_DefaultProjectCatalogue =
{
    /***************************************************************************************************
    * CONFIG: MAIN TASK LIST
    * *************************************************************************************************
    * \brief: Add ModuleIDs from RTD_ModuleIDs and assign them appropriate Thread IDs here.
    *
    *  Members of the struct:
    *  - Task_ID:
    *       The task's own ID (from enum RTD_ModuleIDs)
    *
    *  - SchedThread_ID:
    *       Thread ID of the thread corresponding to this task
    *         Scheduling Mode Options are as follows -
    *         THREAD_FAST_10MS    (PT_ID_FAST)       -> For FAST Thread
    *         THREAD_MEDIUM_100MS (PT_ID_MEDIUM)     -> For MEDIUM Thread
    *         THREAD_SLOW_1S      (PT_ID_SLOW)       -> For SLOW Thread
    *         THREAD_BLE_LL       (ET_ID_BLE_LL)     -> For Event Thread BLE_LL
    *         THREAD_BLE_HIGH     (ET_ID_BLE_HIGH)   -> For Event Thread BLE_HIGH
    *         THREAD_BLE_MEDIUM   (ET_ID_BLE_MEDIUM) -> For Event Thread BLE_MEDIUM
    *         THREAD_BLE_LOW      (ET_ID_BLE_LOW)    -> For Event Thread BLE_LOW
    *
    *  - SchedulingInterval:
    *       SchedulingInterval determines the interval at which the specific task should be scheduled.
    *
    *       Scheduling Interval = Task Period/Thread Period
    *       Example:
    *           If the tasks needs to be scheduled every 50ms (Task Period) in a Thread with Period 10ms (Thread Period),
    *           then according to above formula, the Scheduling Interval would be 5.
    *
    *       If the task needs to be scheduled each thread period i.e. task period = thread period, the value should be set
    *       to RTD_DEFAULT_INTERVAL.
    *       Please note that the task period would be measured in OS Ticks and not milliseconds and could only be configured
    *       in multiples of the thread base ticks.
    *
    *  - SchedulingIntervalCount:
    *       Only applicable to periodic(cyclic) threads
    *       Counts the number of thread schedules since last task schedule
    *       Should always be set to 0 (RTD_INTERVAL_COUNTER_INIT)
    ***************************************************************************************************/
/*Axivion Disable MisraC2012-10.3 : conversion from uint8_t to RTD_Thread_IDs is needed here */
    .RTD_TasksList =
    {
        [MOD_BAL]           = {MOD_BAL,           THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_DBPL]          = {MOD_DBPL,          THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_DLL]           = {MOD_DLL,           THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
    #ifdef RTOS_DBUS_EVENTDRIVEN
        [MOD_DBPL_RX]       = {MOD_DBPL_RX,       THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_DBPL_TX]       = {MOD_DBPL_TX,       THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_DBPL_PWR]      = {MOD_DBPL_PWR,      THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_DLL_TX_INIT]   = {MOD_DLL_TX_INIT,   THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_DLL_TX_DONE]   = {MOD_DLL_TX_DONE,   THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_DLL_ISR]       = {MOD_DLL_ISR,       THREAD_DBUS,     RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
    #endif
        [MOD_TIM]           = {MOD_TIM,           THREAD_BLUEBIRD, RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_SYSL]          = {MOD_SYSL,          THREAD_BLUEBIRD, RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
        [MOD_WDT]           = {MOD_WDT,           THREAD_BLUEBIRD, RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
    #ifdef CCSS
        [MOD_CCSS]          = {MOD_CCSS,          THREAD_BLUEBIRD, RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
    #endif
    #ifdef CCSS_DOM_REF
        [MOD_CCDOM]         = {MOD_CCDOM,         THREAD_BLUEBIRD, RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
    #endif
    #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
        [MOD_FSF]           = {MOD_FSF,           THREAD_FSF,      RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT},
    #endif
 
        /*! place project tasks and their thread IDs here */
        [MOD_ATSSB]         = {MOD_ATSSB,         THREAD_MEDIUM_10MS, RTD_DEFAULT_INTERVAL,   RTD_INTERVAL_COUNTER_INIT},
        [MOD_ATSSBQ]        = {MOD_ATSSBQ,        ET_ID_ORYX,         RTD_DEFAULT_INTERVAL,   RTD_INTERVAL_COUNTER_INIT},

#ifdef BLUETOOTH_MODULE
        /*! place BTM tasks and their thread IDs here */

#endif
        [TOTAL_MODULES]     = {END_STAMP,     SCH_NONE,        RTD_DEFAULT_INTERVAL, RTD_INTERVAL_COUNTER_INIT}
    },
/*Axivion Enable MisraC2012-10.3 */
    /***************************************************************************************************
    * CONFIG: TASK HANDLER TABLE
    * *************************************************************************************************
    * \brief: Add corresponding task handlers for each module here
    ***************************************************************************************************/
    .RTD_TaskHandlersList =
    {
        [MOD_BAL]           = DBR_Handle_BAL,
        [MOD_DBPL]          = DBR_Handle_DBPL,
        [MOD_DLL]           = DBR_Handle_DLL,
    #ifdef RTOS_DBUS_EVENTDRIVEN
        [MOD_DBPL_RX]       = DBR_ReceiveMessage,
        [MOD_DBPL_TX]       = DBR_SendServiceMessage,
        [MOD_DBPL_PWR]      = DBR_SendPowerMessage,
        [MOD_DLL_TX_INIT]   = DBR_TransmitFrameInit,
        [MOD_DLL_TX_DONE]   = DBR_TransmitFrameDone,
        [MOD_DLL_ISR]       = DBR_HandleInterrupt,
    #endif
        [MOD_TIM]           = TIM_handleTask16,  /* TIM_handleTask if no 16Bit timer used */
        [MOD_SYSL]          = RTD_monitorSystemLoad,
        [MOD_WDT]           = WDT_handleTask,
    #ifdef CCSS
        [MOD_CCSS]          = CCSS_RunSyStTask,
    #endif
    #ifdef CCSS_DOM_REF
        [MOD_CCDOM]         = CCDOM_RunDomRefTask,
    #endif
    #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
        [MOD_FSF]           = FSF_handleTask,
    #endif
      
        /*! place project task handlers here */
        [MOD_ATSSB]         =  ATSSB_handleTask,
        [MOD_ATSSBQ]        =  ATSSB_releaseQueue

#ifdef BLUETOOTH_MODULE
        /*! place BTM task handlers here */

#endif

    },

    /***************************************************************************************************
    * CONFIG: DEPENDENCY TABLE
    * *************************************************************************************************
    * \brief: Add above module dependency arrays in the following Table
    ***************************************************************************************************/
    .RTD_DependenciesList =
    {
        [MOD_BAL]           = BAL_dep,
        [MOD_DBPL]          = DBPL_dep,
        [MOD_DLL]           = DLL_dep,
    #ifdef RTOS_DBUS_EVENTDRIVEN
        [MOD_DBPL_RX]       = DBPL_dep,
        [MOD_DBPL_TX]       = DBPL_dep,
        [MOD_DBPL_PWR]      = DBPL_dep,
        [MOD_DLL_TX_INIT]   = DLL_dep,
        [MOD_DLL_TX_DONE]   = DLL_dep,
        [MOD_DLL_ISR]       = DLL_dep,
    #endif
        [MOD_TIM]           = TIM_dep,
        [MOD_SYSL]          = SYSL_dep,
        [MOD_WDT]           = WDT_dep,
    #ifdef CCSS
        [MOD_CCSS]          = CCSS_dep,
    #endif
    #ifdef CCSS_DOM_REF
        [MOD_CCDOM]         = CCDOM_dep,
    #endif
    #ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
        [MOD_FSF]           = FSF_dep,
    #endif

        /*! Place module dependency arrays here */
        [MOD_ATSSB]  = ATSSB_dep,
        [MOD_ATSSBQ] = ATSSB_dep,

#ifdef BLUETOOTH_MODULE
        /*! place BTM dependency arrays here */

#endif
    },
};

/***************************************************************************************************
 * CONFIG: EVENT TASKS LIST - MAX 32 TASKS
 * *************************************************************************************************
 * \brief: Register a Event Driven Task in the Table below.
 * *************************************************************************************************
 * \note: Each Registered Event Driven Task gets a unique Task Flag.
 *        To Run this Task, simply use the following API from rtos_api.h:
 *        uint32_t RTOS_runEventDrivenTask(uint32_t task_flag, ThreadID event_thread_id);
 *        
 *        Maximum allowed Events per Thread for Application are limited to 32, ranging from [0] to [31].
 *        
 *        Please use following format:
 *        MOD_ID1,
 *        MOD_ID2, ...
 *        
 *        The order in which the Tasks are registered below, determine the Priority of the Task
 *        within the EFG. The lower the FLAG number, the higher the priority.
 *        Therefore if at the same time numerous flags are set, then the Tasks will be called
 *        in the same order as they are registered below.
 ***************************************************************************************************/
#if(EVENT_THREAD_ORYX == THREAD_ACTIVATED)
/***************************
 * Thread: EDS_THR_ORYX    *
 * *************************/
const TaskID RTD_EventTasksList_ORYX[] =
{
    /*! Place Your Task IDs Here */
    MOD_ATSSBQ  /* FLAG_0 */   //< Smart Sensor Bus application task release queue


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_oryx = (uint8_t)(sizeof(RTD_EventTasksList_ORYX)/sizeof(RTD_EventTasksList_ORYX[0]));
#endif

#if(EVENT_THREAD_SPIX_MACAW == THREAD_ACTIVATED)
/*******************************
 * Thread: EDS_THR_SPIX_MACAW  *
 * *****************************/
const TaskID RTD_EventTasksList_SPIX_MACAW[] =
{
    /*! Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_spix_macaw_tasks = (uint8_t)(sizeof(RTD_EventTasksList_SPIX_MACAW)/sizeof(RTD_EventTasksList_SPIX_MACAW[0]));
#endif

#if(EVENT_THREAD_VAQUITA == THREAD_ACTIVATED)
/***************************
 * Thread: EDS_THR_VAQUITA *
 * *************************/
const TaskID RTD_EventTasksList_VAQUITA[] =
{
    /*! Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_vaquita_tasks = (uint8_t)(sizeof(RTD_EventTasksList_VAQUITA)/sizeof(RTD_EventTasksList_VAQUITA[0]));
#endif

#if(EVENT_THREAD_KAKAPO == THREAD_ACTIVATED)
/***************************
 * Thread: EDS_THR_KAKAPO  *
 * *************************/
const TaskID RTD_EventTasksList_KAKAPO[] =
{
    /*! Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_kakapo_tasks = (uint8_t)(sizeof(RTD_EventTasksList_KAKAPO)/sizeof(RTD_EventTasksList_KAKAPO[0]));
#endif

#if(EVENT_THREAD_WEKA == THREAD_ACTIVATED)
/***************************
 * Thread: EDS_THR_WEKA    *
 * *************************/
const TaskID RTD_EventTasksList_WEKA[] =
{
    /*! Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_weka_tasks = (uint8_t)(sizeof(RTD_EventTasksList_WEKA)/sizeof(RTD_EventTasksList_WEKA[0]));
#endif

#ifdef BLUETOOTH_MODULE
/***************************
 * Thread: BLE LINK LAYER  *
 * *************************/
const TaskID RTD_EventTasksList_BLE_LL[] =
{
    /*! Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_ble_ll_tasks = (uint8_t)(sizeof(RTD_EventTasksList_BLE_LL)/sizeof(RTD_EventTasksList_BLE_LL[0]));

/******************************
 * Thread: BLE HIGH PRIORITY  *
 * ****************************/
const TaskID RTD_EventTasksList_BLE_High[] =
{
    /*! Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_ble_high_tasks = (uint8_t)(sizeof(RTD_EventTasksList_BLE_High)/sizeof(RTD_EventTasksList_BLE_High[0]));

/********************************
 * Thread: BLE MEDIUM PRIORITY  *
 * ******************************/
const TaskID RTD_EventTasksList_BLE_Medium[] =
{
    /*! Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_ble_med_tasks = (uint8_t)(sizeof(RTD_EventTasksList_BLE_Medium)/sizeof(RTD_EventTasksList_BLE_Medium[0]));

/*****************************
 * Thread: BLE LOW PRIORITY  *
 * ***************************/
const TaskID RTD_EventTasksList_BLE_Low[] =
{
    /*! Place Your Task IDs Here */
    MOD_BAL  /* FLAG_0 */   //< Example! Please Replace MOD_BAL with your Event Task Module ID


    /*! DO NOT exceed 32 Tasks */
};

/* Auto Determination of number of application Event Tasks */
const uint8_t RTD_number_of_ble_low_tasks = (uint8_t)(sizeof(RTD_EventTasksList_BLE_Low)/sizeof(RTD_EventTasksList_BLE_Low[0]));

#endif


/***************************************************************************************************
 * CONFIG: MANAGED ISR FUNCTIONS LIST
 * *************************************************************************************************
 * \brief: Add Managed ISR Functions here, please use void-void functions
 * *************************************************************************************************
 * \note: Each position in the following array represents the Flag of that ISR Function, to call the
 *        specific ISR function, please set the corresponding Flag using the following API from rtos_api.h,
 *        uint32_t RTOS_runISRFromThread(uint32_t isr_flag);
 *
 *        The Sequence at which the ISR functions are registered in the table, determine the Priority.
 *        The lower the FLAG number, higher the priority.
 *        Therefore if at the same time numerous flags are set, then the Functions will be called
 *        in the same order as they are registered below.
 ***************************************************************************************************/
const RTD_ICSHandlers RTD_ISRFunctionsList[] =
{

    RTD_ExampleISR_Handler, /* FLAG_0 */  //< Example! Please replace it with user ISR Handler
    /*! DO NOT exceed 32 Interrupt Functions */
};

/* Auto Determination of number of application ISRs */
const uint8_t RTD_number_of_interrupts = (uint8_t)(sizeof(RTD_ISRFunctionsList)/sizeof(RTD_ISRFunctionsList[0]));


/**************************************************************************************************/
/* CONFIG: RTD RTOS SERVICES                                                                      */
/**************************************************************************************************/
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
 *            uint8_t RTOS_enterCriticalSection(RTOS_MUTEX* mutex); --> Get MUTEX
 *            uint8_t RTOS_exitCriticalSection(RTOS_MUTEX* mutex);  --> Release MUTEX
 ***************************************************************************************************/
/* Create MUTEXES here */


/* Import mutexes created in other modules here */
    extern RTOS_MUTEX mutex_BAL;
#ifdef CCSS
    extern RTOS_MUTEX mutex_CCCM;
    extern RTOS_MUTEX mutex_CCPO;
#endif
#ifdef RTOS_FLASH_DRIVER
    extern RTOS_MUTEX mutex_flash_access;
#endif

/***************************************************************************************************
 * CONFIG: RTD MUTEX LIST
 * *************************************************************************************************
 * \brief: Register all the created MUTEX in the following list.
 * *************************************************************************************************
 * \note:    Use following format;
 *           &mutex_example1,
 *           &mutex_example2, ...
 **************************************************************************************************/
    RTOS_MUTEX *const RTD_MutexList[] =
    {
        &mutex_BAL,
    #ifdef CCSS
        &mutex_CCCM,
        &mutex_CCPO,
    #endif
    #ifdef RTOS_FLASH_DRIVER
        &mutex_flash_access,
    #endif
    /*! place your mutexes here */


    };/*lint !e785 too few initializers - RTOS_MUTEX struct is initialized by RTOS */

/* Auto Determination of number of application MUTEXES */
    const uint8_t RTD_number_of_mutexes = (uint8_t)(sizeof(RTD_MutexList)/sizeof(RTD_MutexList[0]));



/***************************************************************************************************
 * FUTEXES (FUT) Futex is same as Mutex, only difference is the Priority Inheritance is deactivated.
 *         Activating Priority Inheritance would eliminate any unbounded 'Priority Inversion' issues.
 *
 * \note:  Priority inheritance might add some System Load.
 *         For Micrium both Mutex and Futex are same
 * *************************************************************************************************
 *  \brief Create Application FUTEXES below.
 *         To create FUTEXES somewhere else in the application, include 'real_time_dispatcher_types.h'
 *         header file to use below Formats.
 *
 *         Examples/Templates for FUTEX Creation and Initialization:-
 *
 *         1. RTOS_FUTEX futex_example =
 *            {
 *                .futex_name = "futex_example",   --> Give a FUTEX Human Readable name.
 *                                                     priority inheritance is always turned off.
 *            };
 *
 *         2. RTOS_FUTEX futex1, futex2;           --> In this case, FUTEX is not assigned
 *
 * *************************************************************************************************
 * \note   1. Register the FUTEXES below in RTD_FutexList[].
 *         2. To use the FUTEXES in Application, use following APIs:-
 *            uint8_t RTOS_enterCriticalSectionFutex(RTOS_FUTEX* futex); --> Get FUTEX
 *            uint8_t RTOS_exitCriticalSectionFutex(RTOS_FUTEX* futex);  --> Release FUTEX
 ***************************************************************************************************/
/* Create FUTEXES here */


/* Import futexes created in other modules here */
//extern RTOS_FUTEX futex_XXX;
//extern RTOS_FUTEX futex_XXX;
    static char futex_Name[6] = "DUMMY";
    static RTOS_FUTEX futex_dummy =
    {
        .futex_name = futex_Name,

    };

/***************************************************************************************************
 * CONFIG: RTD FUTEX LIST
 * *************************************************************************************************
 * \brief: Register all the created FUTEX in the following list.
 * *************************************************************************************************
 * \note:    Use following format;
 *           &futex_example1,
 *           &futex_example2, ...
 **************************************************************************************************/
    RTOS_FUTEX *const RTD_FutexList[] =
    {
        &futex_dummy, // Example Futex, user need to remove
        /*! place your futexes here */


    };/*lint !e785 too few initializers - RTOS_MUTEX struct is initialized by RTOS */

/* Auto Determination of number of application FUTEXES */
    const uint8_t RTD_number_of_futexes = (uint8_t)(sizeof(RTD_FutexList)/sizeof(RTD_FutexList[0]));


/***************************************************************************************************
 * SEMAPHORE (SEM)
 * *************************************************************************************************
 *  \brief Create Application SEMAPHORE below.
 *         To create SEMAPHORE somewhere else in the application, include 'real_time_dispatcher_types.h'
 *         header file to use below Formats.
 *
 *         Examples/Templates for SEMAPHORE Creation and Initialization:-
 *
 *         1. RTOS_SEMAPHORE semaphore1 =
 *            {
 *                .semaphore_name = "semaphore_example",   --> Give a SEMAPHORE Human Readable name.
 *            };
 *
 *         2. RTOS_SEMAPHORE semaphore1, semaphore2;      --> In this case, SEMAPHORE is not assigned
 *                                                            a human readable name

 * *************************************************************************************************
 * \note   1. Register the SEMAPHORE below in RTD_SemaphoreList[].
 ***************************************************************************************************/
/*! Create Semaphores here */
static char sem_name_dummy[8] = "Example";
static RTOS_SEMAPHORE sem_dummy=
{
    .semaphore_name = sem_name_dummy
};/*lint !e785 too few initializers - RTOS_SEMAPHORE struct is initialized by RTOS */
/*! Import Semaphores created in other modules here */

#ifdef BLUETOOTH_MODULE
/*! Create or import bluetooth module Semaphores here */

#endif


/***************************************************************************************************
 * CONFIG: RTD SEMAPHORE LIST
 * *************************************************************************************************
 * \brief: Register all the created SEMAPHORE in the following list.
 * *************************************************************************************************
 * \note: 1. Use following format;
 *           &semaphore1,
 *           &semaphore2, ...
 *
 **************************************************************************************************/
RTOS_SEMAPHORE *const RTD_SemaphoreList[] =
{
    /*! Place Your Semaphores here */
    &sem_dummy
#ifdef BLUETOOTH_MODULE
    /*! Place Bluetooth Module Semaphores here */
 
#endif
};

/* Auto Determination of number of application Semaphore */
const uint8_t RTD_number_of_semaphores = (uint8_t)(sizeof(RTD_SemaphoreList)/sizeof(RTD_SemaphoreList[0]));


/***************************************************************************************************
 * Message Queues
 * *************************************************************************************************
 *  \brief Create Application Message Queue below.
 *         To create a Message Queue somewhere else in the application, include 'rtos_msg_queue.h'
 *         header file to use below Formats.
 *
 * * \detail  There are two ways of initializing message queues. For general-purpose message queues
 *            of arbitrary message size, it is advised to use the define RTOS_DEFINE_GENERIC_MSG_QUEUE.
 *            To define a generic message queue with the name EXA_msgQueue,
 *            a message size of 8 bytes and a maximum number of 10 messages, use:

 *              RTOS_DEFINE_GENERIC_MSG_QUEUE(EXA_msgQueue, 10, 8);
 *            
 *            For full runtime optimization and memory minimization there is a second implementation
 *            of message queues. This implementation has the following constraints:
 *              - only available if ThreadX is used as RTOS
 *              - message size must be divisible by 4
 *              - message size must be <= 64 bytes
 *            To define the same message queue as in the example above, use:

 *              RTOS_DEFINE_OPTIMIZED_MSG_QUEUE(EXA_msgQueue, 10, 8);
 * 
 *            To use it in an application module, the message queues can be declared like this:

 *              extern RTOS_MSG_QUEUE EXA_msgQueue;
 *            This declaration can be used for both, optimized as well as generic, message queues.

 *            The type RTOS_MSG_QUEUE is also used for the API message, i.e. both generic and optimized
 *            message queues can be passed to the API functions in the same way.
 *
 * *************************************************************************************************
 * \note   1. Register the message queue  below in RTD_MsgQueueList[].
 ***************************************************************************************************/
/*AXIVION Disable Style MisraC2012-8.4 : Missing declaration for object definition, Function like macro */
/*! Create message queues here */
extern RTOS_MSG_QUEUE EXA_msgQueue;
RTOS_DEFINE_GENERIC_MSG_QUEUE(EXA_msgQueue, 4U, 1U);
/*Axivion Enable MisraC2012-8.4 */
/***************************************************************************************************
 * CONFIG: RTD Message Queue List
 * *************************************************************************************************
 * \brief: Register all the created message queues in the following list.
 * *************************************************************************************************
 * \note: 1. Use following format;
 *           &queue1,
 *           &queue2 ...
 *
 **************************************************************************************************/
RTOS_MSG_QUEUE *const RTD_MsgQueueList[] =
{
    /*! Place Your Queues here */
    &EXA_msgQueue,
#ifdef BLUETOOTH_MODULE
    /*! Place Bluetooth Module Queues here */

#endif
};

/* Auto Determination of number of application Queues */
const uint8_t RTD_NumberOfMsgQueues = (uint8_t)(sizeof(RTD_MsgQueueList)/sizeof(RTD_MsgQueueList[0]));


/***************************************************************************************************
 * Mailboxes
 * *************************************************************************************************
 *  \brief Create Application mailbox below.
 *         To create mailbox somewhere else in the application, include 'rtos_mailbox.h'
 *         header file to use below Formats.
 *
 * * \detail  To define a mailbox with the name EXA_mailbox use
 *              RTOS_DEFINE_MAILBOX(EXA_mailbox, sizeof(message));
 *            Note:
 *                - Currently only messages from 4-64 bytes are supported and the size has to be a
 *                  multiple of 4!
 *            to use it in another module declare it as:
 *              RTOS_DECLARE_MAILBOX(EXA_mailbox);
 *
 * *************************************************************************************************
 * \note   1. Register the mailboxes below in RTD_MailboxList[].
 ***************************************************************************************************/
/*AXIVION Disable Style MisraC2012-8.4 : Missing declaration for object definition, Function like macro */
/*lint -save -e2666 function-like macro usage preprocessor macros accepted here */
/*! Create Mailboxes here */
RTOS_DECLARE_MAILBOX(EXA_mailbox);
RTOS_DEFINE_MAILBOX(EXA_mailbox, 4U);
/*Axivion Enable MisraC2012-8.4 */
/*lint -restore -e2666 */
/***************************************************************************************************
 * CONFIG: RTD MAILBOX LIST
 * *************************************************************************************************
 * \brief: Register all the created MAILBOXES in the following list.
 * *************************************************************************************************
 * \note: 1. Use following format;
 *           &mailbox1,
 *           &mailbox2 ...
 *
 **************************************************************************************************/
RTOS_MAILBOX *const RTD_MailboxList[] =
{
    /*! Place Your Mailboxes here */
    &EXA_mailbox,
#ifdef BLUETOOTH_MODULE
    /*! Place Bluetooth Module Mailboxes here */

#endif
};

/* Auto Determination of number of application Mailboxes */
const uint8_t RTD_NumberOfMailboxes = (uint8_t)(sizeof(RTD_MailboxList)/sizeof(RTD_MailboxList[0]));

/***************************************************************************************************
 * MEMORY BLOCK POOL
 * *************************************************************************************************
 *  \brief Create Application MEMORY BLOCK POOL below.
 *         To create MEMORY BLOCK POOL somewhere else in the application, include 'rtos_memory_management.h'
 *         header file to use below Formats.
 *
 *  \detail To define a memory pool with the name Example_Pool with entries of 'type struct Example'
 *          and a pool sizeo of 10 entries use:
 *          RTOS_DEFINE_MEMORY_POOL(Example_Pool, 10U, sizeof(struct Example));
 *          to use it in another module declare it as:
 *          extern RTOS_MEMORY_POOL Example_Pool;

 * *************************************************************************************************
 * \note   1. Register the RTOS_MEMORY_POOL below in RTD_Memory_PoolList[].
 ***************************************************************************************************/
/*! Create Memory Pool here */
extern RTOS_MEMORY_POOL EXA_pool;
RTOS_DEFINE_MEMORY_POOL(EXA_pool, 1U, sizeof(uint8_t));

/*! Import Memory Pool created in other modules here */

#ifdef BLUETOOTH_MODULE
/*! Create or import bluetooth module Memory Pool here */

#endif


/***************************************************************************************************
 * CONFIG: RTD MEMORY BLOCK POOL LIST
 * *************************************************************************************************
 * \brief: Register all the created MEMORY BLOCK POOL in the following list.
 * *************************************************************************************************
 * \note: 1. Use following format;
 *           &memorypool1,
 *           &memorypool2, ...
 *
 **************************************************************************************************/
RTOS_MEMORY_POOL *const RTD_Memory_PoolList[] =
{
    /*! Place Your Memory Pool here */
    &EXA_pool
#ifdef BLUETOOTH_MODULE
    /*! Place Bluetooth Module Memory Pool here */

#endif
};

/* Auto Determination of number of application Memory Pool */
const uint8_t RTD_number_of_memory_pool = (uint8_t)(sizeof(RTD_Memory_PoolList)/sizeof(RTD_Memory_PoolList[0]));

/***************************************************************************************************
 * Reference Queues
 * *************************************************************************************************
 *  \brief Create application reference queues below.
 *         To create a reference queue somewhere else in the application, include 'rtos_ref_queue.h'
 *         header file to use below constructs.
 *
 * * \detail  To define a reference queue the macros:
 *              - RTOS_DEFINE_REF_QUEUE_AUTO
 *              - RTOS_DEFINE_REF_QUEUE_MANUAL
 *            can be used. For the usage of the macros please have a look at the documentation in
 *            either the file `rtos_ref_queue.h` or the documentation located at:
 *            https://github-bshg.boschdevcloud.com/pages/RealTimeOS/rtos-docs/latest/rtos/doc/RefQueue.html
 *            Please make sure to select the correct release of the documentation for your RTOS release.
 *
 * *************************************************************************************************
 * \note   1. Register the reference queues below in RTD_RefQueueList[].
 ***************************************************************************************************/
/*AXIVION Disable Style MisraC2012-8.4 : Missing declaration for object definition, Function like macro */
/*! Create reference queues here */
extern RTOS_REF_QUEUE EXA_refQueue;
RTOS_DEFINE_REF_QUEUE_AUTO(EXA_refQueue, 1U, 1U);

/*! Import reference queues created in other modules here */
/*Axivion Enable MisraC2012-8.4 */
#ifdef BLUETOOTH_MODULE
/*! Create or import bluetooth module reference queues here */

#endif


/***************************************************************************************************
 * CONFIG: RTD REFERENCE QUEUE LIST
 * *************************************************************************************************
 * \brief: Register all the created reference queues in the following list.
 * *************************************************************************************************
 * \note: 1. Use following format;
 *           &refQueue1,
 *           &refQueue2 ...
 *
 **************************************************************************************************/
RTOS_REF_QUEUE *const RTD_RefQueueList[] =
{
    /*! Place Your reference queues here */
    &EXA_refQueue,
    &RTOS_atssbRefQueue
#ifdef BLUETOOTH_MODULE
    /*! Place Bluetooth Module reference queues here */
 
#endif
};

/* Auto Determination of number of application reference queues */
const uint8_t RTD_NumberOfRefQueues = (uint8_t)(sizeof(RTD_RefQueueList)/sizeof(RTD_RefQueueList[0]));



/***************************************************************************************************
 * EVENT FLAG GROUP
 * *************************************************************************************************
 *  \brief Create Application EVENT FLAG GROUP below.
 *
 *         Please see the documentation Eventflags.md. There is a description how to configure
 *         a EFG here or in a customer module.
 *
 * *************************************************************************************************
 * \note   Register the RTOS_EVENT_FLAG_GROUP below in RTD_EventFlagList[].
 ***************************************************************************************************/
/*! Create Event Flags here */
static char efg_name_dummy[8] = "Example";
RTOS_EVENT_FLAG_GROUP EFG_DUMMY =
{
    .event_flag_name = efg_name_dummy
}; /*lint !e785 too few initializers - RTOS_EVENT_FLAG_GROUP struct is initialized by RTOS */

/***************************************************************************************************
 * CONFIG: RTD EVENT FLAG GROUP LIST
 * *************************************************************************************************
 * \brief: Register all the created EVENT FLAG GROUPE in the following list.
 * *************************************************************************************************
 * \note: 1. Use following format;
 *           &eventflag1,
 *           &eventflag2, ...
 *
 **************************************************************************************************/
RTOS_EVENT_FLAG_GROUP *const RTD_EventFlagList[] =
{
    /*! Place Your Event Flags here */
    &EFG_DUMMY,
#if(EVENT_THREAD_ORYX == THREAD_ACTIVATED)
    &EDS_EFG_ORYX,
#endif
#if(EVENT_THREAD_SPIX_MACAW == THREAD_ACTIVATED)
    &EDS_EFG_SPIX_MACAW,
#endif
#if(EVENT_THREAD_VAQUITA == THREAD_ACTIVATED)
    &EDS_EFG_VAQUITA,
#endif
#if(EVENT_THREAD_KAKAPO == THREAD_ACTIVATED)
    &EDS_EFG_KAKAPO,
#endif
#if(EVENT_THREAD_WEKA == THREAD_ACTIVATED)
    &EDS_EFG_WEKA,
#endif
#ifdef BLUETOOTH_MODULE
    /*! Place Bluetooth Module Event Flags here */
    #if(EVENT_THREAD_BLE_HIGH == THREAD_ACTIVATED)
        &EDS_EFG_BLE_HIGH,
    #endif
    #if(EVENT_THREAD_BLE_MEDIUM == THREAD_ACTIVATED)
        &EDS_EFG_BLE_MEDIUM,
    #endif
    #if(EVENT_THREAD_BLE_LOW == THREAD_ACTIVATED)
        &EDS_EFG_BLE_LOW,
    #endif
    #if(EVENT_THREAD_BLE_LINK_LAYER == THREAD_ACTIVATED)
        &EDS_EFG_BLE_LL
    #endif
#endif /* BLUETOOTH_MODULE */
};

/* Auto Determination of number of application Event Flags Group */
const uint8_t RTD_number_of_event_flags = (uint8_t)(sizeof(RTD_EventFlagList)/sizeof(RTD_EventFlagList[0]));

/***************************************************************************************************
 * APPLICATION TIMER (TMR)
 * *************************************************************************************************
 *  \brief Create Application Timer below.
 *         To create Timer somewhere else in the application, include 'real_time_dispatcher_types.h'and 'rtos_types.h'
 *         Examples/Templates for TIMER Creation and Initialization:-
 *
 * \note: 1. RTOS_TIMER timer1 =
 *           {
 *                .timer_name = timer_1ms         --> Give a SEMAPHORE Human Readable name.
 *                .callback   = RTD_handleOSTick, --> call back function
 *                .timer_mode_oneshot = true,     --> only if timer is one shot
 *                .timer_period = 1U              --> timer period in ms
 *           };
 *
 *        2. RTOS_TIMER timer1 =
 *           {
 *                .callback   = RTD_handleOSTick, --> timer expiry callback
 *                .timer_period = 10U             --> timer period in ms
 *           };                                   
 *           
 *           These 2 fields are a minimum requirement to create a timer.
 ***************************************************************************************************
 * \note   1. Register the TIMER below in RTD_TimerList[].
 ***************************************************************************************************/
/*! Create Timers here */

/*! Import Timers created in other modules here */


#ifdef BLUETOOTH_MODULE
/*! Create or import bluetooth module Timers here */

#endif


/***************************************************************************************************
 * CONFIG: RTD TIMER LIST
 * *************************************************************************************************
 * \brief: Register all the created TIMER in the following list.
 * *************************************************************************************************
 * \note: 1. Use following format;
 *           &timer1,
 *           &timer2, ...
 *
 **************************************************************************************************/
RTOS_TIMER *const RTD_TimerList[] =
{
    &timer_systimer,
 
    /*! Place Your Timers Here */

#ifdef BLUETOOTH_MODULE
    /*! Place Bluetooth Module Timers here */

 #endif
};

/* Auto Determination of number of application Timers */
const uint8_t RTD_number_of_timers = (uint8_t)(sizeof(RTD_TimerList)/sizeof(RTD_TimerList[0]));


/***************************************************************************************************
 * CONFIG: RTD RTOS Services creation
 * *************************************************************************************************
 * \brief: If any additional RTOS services such as Timers, Message Queues etc are needed, can be
 *         created here.
 * *************************************************************************************************
 * \note:  Create Timers always at the end
 * *************************************************************************************************/
void RTD_CreateAdditionalRTOSServices(void)
{
    /*! Create additional RTOS RTD Services if needed here */
 
}


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
/*************************************** End of File **********************************************/
