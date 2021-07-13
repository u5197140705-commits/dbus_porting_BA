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
/*!
 *
 *  \file
 *  \brief User application specific configurations for real time dispatcher.
 *
 */


#ifndef REAL_TIME_DISPATCHER_CONFIG_H_
#define REAL_TIME_DISPATCHER_CONFIG_H_


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "os/rtos_defines.h"


/***************************************************************************************************
 * CONFIG: WORLD THREAD CENTER
 * *************************************************************************************************
 * \brief:  Activate the needed RTD Threads for User Application.
 *
 *          Hereafter following abbreviations will be used to refer the threads:
 *          IT - Interrupt Thread
 *          FT - Framework Thread (Event Driven)
 *          PT - Periodic Thread
 *          ET - Event Thread
 ***************************************************************************************************/
/* Interrupt Servicing Thread */
#define INTERRUPT_SERVICING_THREAD      ///< Activated by default, can not be deactivated!

/* Framework Thread - Reserved for Framework Tasks */
#define FRAMEWORK_THREAD_BLUEBIRD       THREAD_ACTIVATED    ///< If deactivated, please make sure that the Framework Tasks are scheduled by User Threads!

/* User Periodic Threads*/
#define PERIODIC_THREAD_ULTRA_FAST      THREAD_DEACTIVATED
#define PERIODIC_THREAD_SUPER_FAST      THREAD_DEACTIVATED
#define PERIODIC_THREAD_FAST            THREAD_DEACTIVATED
#define PERIODIC_THREAD_MEDIUM          THREAD_DEACTIVATED
#define PERIODIC_THREAD_SLOW            THREAD_DEACTIVATED

/* User Event Threads */
#define EVENT_THREAD_ORYX               THREAD_DEACTIVATED


/***************************************************************************************************
 * CONFIG: RTD THREAD PERIODS
 * *************************************************************************************************
 * \brief: The Period of the Periodic Threads can be set here.
 * *************************************************************************************************
 * \note: Fastest possible Thread Period is 1ms, therefore ultra fast thread is exclusive for thread 1ms.
 ***************************************************************************************************/
#define PT_PERIOD_ULTRA_FAST            TICKS_1MS           ///< Select Thread Period
#define PT_PERIOD_SUPER_FAST            TICKS_2MS           ///< Select Thread Period
#define PT_PERIOD_FAST                  TICKS_5MS           ///< Select Thread Period
#define PT_PERIOD_MEDIUM                TICKS_10MS          ///< Select Thread Period
#define PT_PERIOD_SLOW                  TICKS_100MS         ///< Select Thread Period


/***************************************************************************************************
 * CONFIG: HYPERPERIOD
 * *************************************************************************************************
 * \brief: The Hyperperiod is defined as the least common multiple (LCM) of the periods of all the periodic tasks.
 *         System load is measured each Hyperperiod.
 *         Please calculate the LCM of all the periods and select the number Hyperperiod in terms of TICKS.
 * ************************************************************************************************/
#define HYPERPERIOD                     TICKS_100MS


/***************************************************************************************************
 * CONFIG: RTD THREAD PRIORITIES
 * *************************************************************************************************
 * \brief: The priorities of the RTD Threads can be configured here.
 *
 *         Event thread priorities can vary depending on its Tasks Relative Deadlines, therefore
 *         User is responsible for appropriate assignment of its Priorities.
 *         
 *         By Default, Periodic Thread priorities are based on EDF(Earliest Deadline First) and RMS
 *         (Rate Monotonic Scheduling) Concepts, user is allowed to change it only with reasoning.
 *         
 *         Following Priorities are available for User Applications:-
 *           HIGHEST_PRIORITY
 *           ABOVE_HIGH_PRIORITY
 *           HIGH_PRIORITY
 *           BELOW_HIGH_PRIORITY
 *           ABOVE_MEDIUM_PRIORITY
 *           MEDIUM_PRIORITY
 *           BELOW_MEDIUM_PRIORITY
 *           ABOVE_LOW_PRIORITY
 *           LOW_PRIORITY
 *           BELOW_LOW_PRIORITY
 *           LOWEST_PRIORITY
 *         
 *         User applications must be assigned priorities between HIGHEST_PRIORITY to LOWEST_PRIORITY.
 *         
 *         If more Priorities are needed, Priorities can be tuned, which makes it total of 22 Priorities available for user.
 *         
 *         Between HIGHEST_PRIORITY to ABOVE_HIGH_PRIORITY & BELOW_LOW_PRIORITY to LOWEST_PRIORITY;
 *         there are each 2 more available priorities that can be tuned.
 *         And between BELOW_HIGH_PRIORITY to ABOVE_MEDIUM_PRIORITY & BELOW_MEDIUM_PRIORITY to ABOVE_LOW_PRIORITY;
 *         there are each 4 more available priorities that can also be tuned.
 *         
 *         To tune a priority Higher or Lower by one unit, please add following MACROS to the End:-
 *           _TUNE_HIGHER
 *           _TUNE_LOWER
 *         For example, to get a priority higher than ABOVE_MEDIUM_PRIORITY by one unit;
 *         (ABOVE_MEDIUM_PRIORITY _TUNE_HIGHER) would obtain a higher priority than ABOVE_MEDIUM_PRIORITY,
 *         but lower than BELOW_HIGH_PRIORITY.
 *         
 *         All ABOVE_ and BELOW_ Priorities are adjacent to the Main (HIGH, MEDIUM, LOW) Priorities.
 *         Therefore Main (HIGH, MEDIUM, LOW) Priorities can not be tuned.
 * *************************************************************************************************
 * \notes: 1. Interrupt Thread needs to have higher priority than all other Application Threads to work
 *            effectively, so change the priority only with reasoning!
 *         2. Periodic Threads are/should be assigned Priorities based on EDF/RMS.
 *         3. Event Threads should be assigned priority according to the Relative Deadlines of its
 *            Task content.
 *         4. User application priorities range from HIGHEST_PRIORITY to LOWEST_PRIORITY, therefore
 *            DO NOT tune higher the HIGHEST_PRIORITY or tune lower the LOWEST_PRIORITY.
 *         5. If the Priority of a Thread is changed, please DO NOT forget to assign appropriate
 *            value to Preemption Threshold below.
 *         6. When in doubt, chicken out.
 ***************************************************************************************************/
#define PT_PRIORITY_ULTRA_FAST              ABOVE_HIGH_PRIORITY     ///< Periodic Thread ULTRA FAST
#define PT_PRIORITY_SUPER_FAST              HIGH_PRIORITY           ///< Periodic Thread SUPER FAST
#define PT_PRIORITY_FAST                    BELOW_HIGH_PRIORITY     ///< Periodic Thread FAST
#define ET_PRIORITY_ORYX                    ABOVE_MEDIUM_PRIORITY   ///< Event Thread ORYX
#define PT_PRIORITY_MEDIUM                  MEDIUM_PRIORITY         ///< Periodic Thread MEDIUM
#define PT_PRIORITY_SLOW                    LOW_PRIORITY            ///< Periodic Thread SLOW


/***************************************************************************************************
 * CONFIG: RTD THREAD PREEMPTION THRESHOLD
 * *************************************************************************************************
 * \brief: Once a Preemption Threshold is assigned to a Thread, then only Threads with higher priority
 *         than the Preemption Threshold Value are allowed to Preempt the thread.
 *         A Value equal to the above thread priority disables the preemption threshold, therefore
 *         by default Preemption Threshold is DISABLED!
 * *************************************************************************************************
 * \note:  If Preemption Threshold is activated for a Thread and if the Priority of a Thread is changed
 *         above, please DO NOT forget to assign appropriate value to Preemption Threshold, else it
 *         might affect the preemptive system behavior.
 ***************************************************************************************************/
#define PT_PREEMPTION_THRESHOLD_ULTRA_FAST  PT_PRIORITY_ULTRA_FAST  ///< Periodic Thread ULTRA FAST
#define PT_PREEMPTION_THRESHOLD_SUPER_FAST  PT_PRIORITY_SUPER_FAST  ///< Periodic Thread SUPER FAST
#define PT_PREEMPTION_THRESHOLD_FAST        PT_PRIORITY_FAST        ///< Periodic Thread FAST
#define ET_PREEMPTION_THRESHOLD_ORYX        ET_PRIORITY_ORYX        ///< Event Thread ORYX
#define PT_PREEMPTION_THRESHOLD_MEDIUM      PT_PRIORITY_MEDIUM      ///< Periodic Thread MEDIUM
#define PT_PREEMPTION_THRESHOLD_SLOW        PT_PRIORITY_SLOW        ///< Periodic Thread SLOW


/***************************************************************************************************
 * CONFIG: RTD THREAD STACK SIZE
 * *************************************************************************************************
 * \brief: Stack sizes for each Individual Thread can be assigned here.
 *
 *         Stack sizes can vary thread to thread depending on content of its Tasks or ISRs etc,
 *         excess stack can waste memory whereas insufficient stack can cause stack overflow,
 *         therefore user is responsible for finding out appropriate stack size for each Thread.
 *         
 *         By default, 1024 Bytes are assigned to each thread, except Interrupt thread, which is
 *         allocated 512 bytes, as ISRs are relatively smaller, the stack requirements are usually
 *         not as high as scheduling threads, but if extra memory is needed, it can be configured here.
 * *************************************************************************************************
 * \note:  If a Stack Overflow occurs for any Thread, Error is notified by setting the variable
 *         'RTD_RuntimeCatalogue.RuntimeInfo.Error_StackOverflow' true.
 *         The Callback void RTD_ErrorStackOverflowHandler(const ThreadID thread_id) is called.
 ***************************************************************************************************/
#define IT_STACK_SIZE_IST                   (DEFAULT_STACK_SIZE/2U)     ///< Interrupt Servicing Thread
#define FT_STACK_SIZE_BLUEBIRD              DEFAULT_STACK_SIZE          ///< Framework Bluebird Thread
#define ET_STACK_SIZE_ORYX                  DEFAULT_STACK_SIZE          ///< Event Thread ORYX
#define PT_STACK_SIZE_ULTRA_FAST            DEFAULT_STACK_SIZE          ///< Periodic Thread ULTRA FAST
#define PT_STACK_SIZE_SUPER_FAST            DEFAULT_STACK_SIZE          ///< Periodic Thread SUPER FAST
#define PT_STACK_SIZE_FAST                  DEFAULT_STACK_SIZE          ///< Periodic Thread FAST
#define PT_STACK_SIZE_MEDIUM                DEFAULT_STACK_SIZE          ///< Periodic Thread MEDIUM
#define PT_STACK_SIZE_SLOW                  DEFAULT_STACK_SIZE          ///< Periodic Thread SLOW


/***************************************************************************************************
 * CONFIG: RTD MUTEX PRIORITY INHERITANCE
 * *************************************************************************************************
 * \brief: Priority Inheritance for all the Mutexes in RTD can be activated or deactivated here.
 *
 *         Activating Priority Inheritance would eliminate any unbounded 'Priority Inversion' issues.
 *         By default, Priority Inheritance is activated and should not be deactivated without a reasoning.
 * *************************************************************************************************
 * \note:  Priority inversion might add some System Load.
 ***************************************************************************************************/
#define RTD_MUTEX_PRIORITY_INHERITANCE     FEATURE_ACTIVATED


/***************************************************************************************************
 * CONFIG: MODULE/TASK ENUMERATIONS
 * *************************************************************************************************
 * \brief: Create a unique ID for each Application module, use naming convention MOD_<ModAbbr>.
 *         For example, ID 'MOD_TIM' for Timer Module.
 * *************************************************************************************************
 * \note:
 ***************************************************************************************************/
typedef enum RTD_ModuleIDs
{
    MOD_BAL,          ///< module ID of dbus bus layer
    MOD_DBPL,         ///< module ID of dbus presentation layer
    MOD_DLL,          ///< module ID of dbus data link layer
    MOD_TIM,          ///< module ID of timer library
    MOD_SYSL,         ///< module ID for System Load measurement
    MOD_WDT,
#ifdef CCSS
    MOD_CCSS,         ///< module ID of SystemStates Taskhandler
#endif
#ifdef CCSS_DOM_REF
    MOD_CCDOM,        ///< module ID of SystemStates Domain Reference implementation. to be replaced with an application specific implementation.
#endif
    /*! place your projects modules here */


    TOTAL_MODULES
}TaskID;

/*! Auto Define - Number of modules */
#define NO_OF_MODULES               ((uint8_t)TOTAL_MODULES)
#define NO_OF_MODULES_WRAPPED       ((NO_OF_MODULES + MAX_BIT_INDEX) / BITS_IN_A_BYTE)
#define INVALID_TASK_ID             TOTAL_MODULES


/***************************************************************************************************
 * CONFIG: DEVELOPER MODE SETTINGS
 * *************************************************************************************************
 * \brief: 1. STACK_OVERFLOW_MONITORING
 *            By Default Stack Overflow Monitoring Feature is activated for Real Time Dispatcher.
 *            If a Stack Overflow occurs for any Thread, the ,RTD_ErrorStackOverflowHandler' Callback
 *            is called and the parameter 'thread_id' identifies the Thread for which the Stack Error has
 *            occurred.
 *            
 *            The Developer Mode setting determines what happens after the callback is called.
 *            
 *            If STACK_OVERFLOW_DEVELOPER_MODE is set ACTIVATE then after the Callback, the Hard Fault
 *            is activated as an endless loop, and the Developer is notified by setting the variable
 *            RTD_RuntimeCatalogue.RuntimeInfo.Error_StackOverflow 'true'. Developer can then identify for which Thread the stack
 *            overflow occurred and by increasing the stack size of the Thread, this Error can be Fixed.
 *            
 *            If STACK_OVERFLOW_DEVELOPER_MODE is set DEACTIVATE, then the SYSTEM MODE gets activated in
 *            which no Hard Fault is generated but the System Reset takes place right after the Callback.
 *            
 *            The DEVELOPER MODE should be activated only for development phase, and must be deactivated
 *            thereafter.
 *
 *         2. SYSTEM_LOAD_MONITORING
 *            With activation of RTOS_MODE_DEBUG, System Load Monitoring is also activated by default.
 *
 *            If the maximum allowed System Load of 70% is overshot, then the User Error Callback
 *            ,RTDTM_ErrorSystemOverloadedHandler' is called.
 *            
 *            If SYSTEM_OVERLOAD_DEVELOPER_MODE is ACTIVATED, then after the Callback a Hard Fault as an
 *            endless loop is activated and the Developer is notified about the error with
 *            'RTD_RuntimeCatalogue.RuntimeInfo.Error_SystemOverloaded' and the exact System Load of the system with 'SystemLoad'.
 *            
 *            It is very important to detect System Overload to maintain System Feasibility, System
 *            efficiency and to determine whether would it be possible to add future Feature updates.
 *            
 *            SYSTEM MODE is activated when the SYSTEM_OVERLOAD_DEVELOPER_MODE is set DEACTIVATE.
 *            In System Mode whenever a System Overload occurs, user is still notified with
 *            'RTD_RuntimeCatalogue.RuntimeInfo.Error_SystemOverloaded' and 'SystemLoad' and also the callback is called but
 *            no Hard Fault or no System Reset is generated. The Software flow is NOT interrupted.
 * *************************************************************************************************
 * \note:    Please make sure to turn off all the critical components such as Heaters, Motors etc.
 *           from inside the Error Callbacks before any Hard Fault is generated with Developer Mode.
 ***************************************************************************************************/
#define STACK_OVERFLOW_DEVELOPER_MODE   FEATURE_DEACTIVATED  //< If Activated HardFault will be generated after Callback, else System Reset
#ifdef RTOS_MODE_DEBUG
#define SYSTEM_OVERLOAD_DEVELOPER_MODE  FEATURE_DEACTIVATED  //< If Activated HardFault will be generated after Callback, else no action
#endif


/***************************************************************************************************
 * CONFIG: RTOS TRACING
 * *************************************************************************************************
 * \brief: 	If needed, activate the RTOS tracing here.
 ***************************************************************************************************/
#ifdef RTOS_TRACING_ENABLED
#define RTOS_NUMBER_OF_TRACE_EVENTS     500     //< Defines the number of events that can be buffered.
#define RTOS_NUMBER_OF_TRACE_OBJECTS    40      //< The number of ThreadX objects to keep in the trace registry. This allows for displaying objects names, instead of just addresses.
#endif


/***************************************************************************************************
 * *************************************************************************************************
 * DEBUG CONFIGURATIONS
 * *************************************************************************************************
 ***************************************************************************************************/
#ifdef RTOS_MODE_DEBUG
/***************************************************************************************************
 * CONFIG: SELECT DEBUG FEATURES
 * *************************************************************************************************
 * \brief:  Following Features are by default activated for Real Time Dispatcher and RTOS_MODE_DEBUG.
 *          1. STACK_OVERFLOW_MONITORING        <- RTD Default Feature
 *          2. SYSTEM_LOAD_MONITORING           <- RTD RTOS_MODE_DEBUG Default Feature
 *          3. EXECUTION_TIMEOUT_MONITORING     <- RTD RTOS_MODE_DEBUG Default Feature
 *
 *          Following are additional Debug features which can be configured below.
 *          
 *          1. TASK_TIME_MONITORING - Time Values such as Runtime, Period, Jitter of each individual
 *                                    Task can be monitored with this feature.
 *                                    To read these values, please check the section DEBUG INFORMATION
 *                                    in rtos_api.h.
 *          
 *          2. ICS_CONFLICTS_MONITORING - If any Critical Section conflicts occur for Interrupts and the Interrupt has
 *                                        to be called from a Thread to avoid the Data inconsistency, then by using this
 *                                        feature the number of times the conflict occurred for each individual ISR
 *                                        can be obtained.
 *                                        To obtain the information, please check the section DEBUG INFORMATION
 *                                        in rtos_api.h.
 *
 *          3. THREAD_1MS_LOAD_MONITORING - Load on Thread 1MS can be monitored using this feature.
 *                                          As 1ms Thread adds the highest System Load and is a
 *                                          critical thread, maximum recommended Thread Load is 20%.
 *                                          To get 1ms thread load, please check the section DEBUG INFORMATION
 *                                          in rtos_api.h.
 * *************************************************************************************************
 * \note:   1. By default, all these features are activated. User can DEACTIVATE them below if needed.
 *          2. To obtain information about all above features, please check the section DEBUG INFORMATION
 *             in rtos_api.h.
 ***************************************************************************************************/
#define TASK_TIME_MONITORING            FEATURE_ACTIVATED
#define ICS_CONFLICTS_MONITORING        FEATURE_ACTIVATED
#ifdef SYSTICK_1MS
#define THREAD_1MS_LOAD_MONITORING      FEATURE_ACTIVATED
#endif


/***************************************************************************************************
 * CONFIG: SELECT TASK TIME MONITORING MODE
 * *************************************************************************************************
 * \brief: Two Configuration Modes possible; 'MONITOR_SINGLE_MODULE' or 'MONITOR_ALL_MODULES'
 *
 *         With MONITOR_ALL_MODULES, the time of all modules will be monitored by default, while
 *         with MONITOR_SINGLE_MODULE, only the time of the selected module will be monitored.
 *
 *         The desired Debug Mode, 'MONITOR_SINGLE_MODULE' or 'MONITOR_ALL_MODULES' can be
 *         selected below. By Default MONITOR_ALL_MODULES mode is selected!
 * *************************************************************************************************
 * \note:  Selecting MONITOR_ALL_MODULES increases the RAM requirement.
 ***************************************************************************************************/
#define TASK_TIME_MONITORING_MODE       MONITOR_ALL_MODULES  //< 'MONITOR_SINGLE_MODULE' or 'MONITOR_ALL_MODULES'


/*************************************************************************************************** 
 * CONFIG: SELECT MODULE ID for MONITOR_SINGLE_MODULE MODE
 * *************************************************************************************************
 * \brief: If MONITOR_SINGLE_MODULE mode is selected, the Module ID which needs to be debugged,
 *         must be selected below.
 ***************************************************************************************************/
#if(TASK_TIME_MONITORING_MODE == MONITOR_SINGLE_MODULE)
#define MONITOR_MODULE_ID       <Place Module ID here>  //< Select Module ID, example: MOD_TIM
#endif


/***************************************************************************************************
 * CONFIG: SELECT NUMBER OF TIMING SAMPLES
 * *************************************************************************************************
 * \brief: Number of Timing Samples for each module to be saved, by default 2 samples!
 * *************************************************************************************************
 * \note:  With more number of Samples, the RAM Memory Requirements go higher.
 ***************************************************************************************************/
#define NUMBER_OF_SAMPLES       2U      //< Select number of samples

#endif
#endif

/********************************************* End of File ******************************************************/
