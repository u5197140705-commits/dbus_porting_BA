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

#ifndef REAL_TIME_DISPATCHER_CONFIG_H_
#define REAL_TIME_DISPATCHER_CONFIG_H_

#ifdef REAL_TIME_DISPATCHER_CONFIG_AUTO
#include "rtos_test_app/rtd_cfg_auto/real_time_dispatcher_auto_cfg.h"
#else

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "os/rtos_defines.h"
#include "rtos_types.h"

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

/* User Periodic Threads*/
#define PERIODIC_THREAD_ULTRA_FAST      THREAD_DEACTIVATED
#define PERIODIC_THREAD_SUPER_FAST      THREAD_DEACTIVATED
#define PERIODIC_THREAD_FAST            THREAD_DEACTIVATED
#define PERIODIC_THREAD_MEDIUM          THREAD_DEACTIVATED
#define PERIODIC_THREAD_SLOW            THREAD_DEACTIVATED

/* Bluetooth Module Threads */
#define EVENT_THREAD_BLE_LINK_LAYER     THREAD_DEACTIVATED
#define EVENT_THREAD_BLE_HIGH           THREAD_DEACTIVATED
#define EVENT_THREAD_BLE_MEDIUM         THREAD_DEACTIVATED
#define EVENT_THREAD_BLE_LOW            THREAD_DEACTIVATED

/* User Event Threads */
#define EVENT_THREAD_ORYX               THREAD_DEACTIVATED
#define EVENT_THREAD_SPIX_MACAW         THREAD_DEACTIVATED
#define EVENT_THREAD_VAQUITA            THREAD_DEACTIVATED
#define EVENT_THREAD_KAKAPO             THREAD_DEACTIVATED
#define EVENT_THREAD_WEKA               THREAD_DEACTIVATED

/* Framework Thread - Reserved for Framework Tasks */
#define FRAMEWORK_THREAD_BLUEBIRD       THREAD_ACTIVATED    ///< If deactivated, please make sure that the Framework Tasks are scheduled by User Threads!
#define FRAMEWORK_THREAD_DBUS           THREAD_ACTIVATED    ///< If deactivated, please make sure that the DBUS Tasks are scheduled by User Threads!

/* DC motors thread */
#define PERIODIC_THREAD_DCM             THREAD_DEACTIVATED

/* SPIA thread */
#define PERIODIC_THREAD_SPIA            THREAD_DEACTIVATED

/***************************************************************************************************
 * CONFIG: RTD THREAD ASSIGNMENT
 * *************************************************************************************************
 * \brief: OSAL FW- and Dbus-tasks are assigned by default to their predefined threads.
 *         It's also possible, to assign them to a user ED- or a Periodic Thread.
 *         In case of assigning to a user eventthread, please enter the
 *         correct ID value with pattern RTD_ET_ID_xxx_VAL.
 *         In case of assigning to a periodic thread, please enter RTD_ANY_PT_THREAD_ASSIGNED_VAL.
 *
 *         For more informations, please read ext/rtos/doc/DbusBuildOptions.md
 ***************************************************************************************************/
#define RTOS_THREAD_ID_FW               RTD_FT_ID_BLUEBIRD_VAL
#define RTOS_THREAD_ID_DBUS             RTD_FT_ID_DBUS_VAL

/***************************************************************************************************
 * CONFIG: RTD THREAD PERIODS
 * *************************************************************************************************
 * \brief: The Period of the Periodic Threads can be set here.
 *         when Period thread is Activated, then corresponding period must be set to non zero value.
 ***************************************************************************************************/
#define PT_PERIOD_ULTRA_FAST            TICKS_1MS           ///< Select Thread Period
#define PT_PERIOD_SUPER_FAST            TICKS_2MS           ///< Select Thread Period
#define PT_PERIOD_FAST                  TICKS_5MS           ///< Select Thread Period
#define PT_PERIOD_MEDIUM                TICKS_10MS          ///< Select Thread Period
#define PT_PERIOD_SLOW                  TICKS_100MS         ///< Select Thread Period
#define PT_PERIOD_FSF                   TICKS_1MS           ///< Select Thread Period
#define PT_PERIOD_DCM                   TICKS_1MS           ///< Select Thread Period
#define PT_PERIOD_SPIA                  TICKS_1MS           ///< Select Thread Period

/***************************************************************************************************
 * CONFIG: HYPERPERIOD
 * *************************************************************************************************
 * \brief: The Hyperperiod is defined as the least common multiple (LCM) of the periods of all the periodic tasks.
 *         System load is measured each Hyperperiod.
 *         Please calculate the LCM of all the periods and select the number Hyperperiod in terms of TICKS.
 * ************************************************************************************************/
#define HYPERPERIOD                     TICKS_100MS


/***************************************************************************************************
 * CONFIG: RTOS DRIVE MODES
 * *************************************************************************************************
 * \brief: Please select one of the 2 available modes - RTOS_ECO_MODE or RTOS_PERFORMANCE_MODE
 * *       For THREADX only RTOS_PERFORMANCE_MODE is available
 *         RTOS_ECO_MODE : In Eco mode, the periodic threads are triggered using low energy 
 *                         hardware timer, which facilitates power saving.
 *                         But this hinders the accuracy of the OS Timer Ticks. Approximately 
 *                         7% deviation is expected for each OS Ticks in accuracy therefore the
 *                         1ms Ticks are not generated exactly at 1ms, but at 1.07ms and the
 *                         timing is NOT corrected.
 *
 *                         Note: In Eco mode, please do not use the OS Ticks for any accurate time 
 *                         measurements.
 *
 *         RTOS_PERFORMANCE_MODE : In Performance mode, the periodic threads are triggered by high
 *                                 accuracy SysTick Timer. This improves the timing accuracy but may
 *                                 significantly affect Power Saving.
 * ************************************************************************************************/
#ifdef OS_THREADX
   #define RTOS_DRIVE_MODE                 RTOS_PERFORMANCE_MODE
#else /* OS_MICRIUM */
   #define RTOS_DRIVE_MODE                 RTOS_ECO_MODE
#endif


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
 *         If more Priorities are needed, Priorities can be tuned, which makes it total of 18 Priorities available for user.
 *         
 *         Between HIGHEST_PRIORITY to ABOVE_HIGH_PRIORITY, there is one more priority available that can be tuned.
 *         And between BELOW_HIGH_PRIORITY to ABOVE_MEDIUM_PRIORITY & BELOW_MEDIUM_PRIORITY to ABOVE_LOW_PRIORITY;
 *         there are each 3 more available priorities that can also be tuned.
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
#define PT_PRIORITY_MEDIUM                  MEDIUM_PRIORITY         ///< Periodic Thread MEDIUM
#define PT_PRIORITY_SLOW                    LOW_PRIORITY            ///< Periodic Thread SLOW
#define PT_PRIORITY_BM_COMPAT               LOWEST_PRIORITY         ///< Periodic Thread BM_COMPAT
#define PT_PRIORITY_STA                     LOWEST_PRIORITY         ///< Periodic Thread STA
#define ET_PRIORITY_ORYX                    ABOVE_MEDIUM_PRIORITY   ///< Event Thread ORYX
#define ET_PRIORITY_SPIX_MACAW              ABOVE_MEDIUM_PRIORITY   ///< Event Thread SPIX_MACAW
#define ET_PRIORITY_VAQUITA                 ABOVE_MEDIUM_PRIORITY   ///< Event Thread VAQUITA
#define ET_PRIORITY_KAKAPO                  ABOVE_MEDIUM_PRIORITY   ///< Event Thread KAKAPO
#define ET_PRIORITY_WEKA                    ABOVE_MEDIUM_PRIORITY   ///< Event Thread WEKA
/* Bluetooth Module Threads */
#define ET_PRIORITY_BLE_LINK_LAYER          BLE_HIGHEST_PRIORITY    ///< Event Thread BLE_LINK_LAYER
#define ET_PRIORITY_BLE_HIGH                BLE_HIGH_PRIORITY       ///< Event Thread BLE_HIGH
#define ET_PRIORITY_BLE_MEDIUM              BLE_MEDIUM_PRIORITY     ///< Event Thread BLE_MEDIUM
#define ET_PRIORITY_BLE_LOW                 BLE_LOW_PRIORITY        ///< Event Thread BLE_LOW

/* DC motors threads */
#define PT_PRIORITY_DCM                     DCM_PRIORITY            ///< Periodic Thread DC motors

/* SPIA thread */
#define PT_PRIORITY_SPIA                    SPIA_PRIORITY           ///< Periodic Thread SPIA

/***************************************************************************************************
 * CONFIG: RTD THREAD PREEMPTION THRESHOLD
 * *************************************************************************************************
 * \brief: Once a Preemption Threshold is assigned to a Thread, then only Threads with higher priority
 *         than the Preemption Threshold Value are allowed to Preempt the thread.
 *         A value equal to the thread priority disables the preemption threshold, therefore
 *         by default Preemption Threshold is DISABLED!
 * *************************************************************************************************
 * \note:  If Preemption Threshold is activated for a Thread and if the Priority of a Thread is changed
 *         above, please DO NOT forget to assign appropriate value to Preemption Threshold, else it
 *         might affect the preemptive system behavior.
 ***************************************************************************************************/
#define PT_PREEMPTION_THRESHOLD_ULTRA_FAST      PT_PRIORITY_ULTRA_FAST  ///< Periodic Thread ULTRA FAST
#define PT_PREEMPTION_THRESHOLD_SUPER_FAST      PT_PRIORITY_SUPER_FAST  ///< Periodic Thread SUPER FAST
#define PT_PREEMPTION_THRESHOLD_FAST            PT_PRIORITY_FAST        ///< Periodic Thread FAST
#define PT_PREEMPTION_THRESHOLD_MEDIUM          PT_PRIORITY_MEDIUM      ///< Periodic Thread MEDIUM
#define PT_PREEMPTION_THRESHOLD_SLOW            PT_PRIORITY_SLOW        ///< Periodic Thread SLOW
#define PT_PREEMPTION_THRESHOLD_BM_COMPAT       PT_PRIORITY_BM_COMPAT   ///< Periodic Thread BM_COMPAT
#define PT_PREEMPTION_THRESHOLD_STA             PT_PRIORITY_STA         ///< Periodic Thread STA
#define ET_PREEMPTION_THRESHOLD_ORYX            ET_PRIORITY_ORYX        ///< Event Thread ORYX
#define ET_PREEMPTION_THRESHOLD_SPIX_MACAW      ET_PRIORITY_SPIX_MACAW  ///< Event Thread SPIX_MACAWX
#define ET_PREEMPTION_THRESHOLD_VAQUITA         ET_PRIORITY_VAQUITA     ///< Event Thread VAQUITA
#define ET_PREEMPTION_THRESHOLD_KAKAPO          ET_PRIORITY_KAKAPO      ///< Event Thread KAKAPO
#define ET_PREEMPTION_THRESHOLD_WEKA            ET_PRIORITY_WEKA        ///< Event Thread WEKA
/* Bluetooth Module Threads */
#define ET_PREEMPTION_THRESHOLD_BLE_LINK_LAYER  ET_PRIORITY_BLE_LINK_LAYER  ///< Event Thread BLE_LINK_LAYER
#define ET_PREEMPTION_THRESHOLD_BLE_HIGH        ET_PRIORITY_BLE_HIGH        ///< Event Thread BLE_HIGH
#define ET_PREEMPTION_THRESHOLD_BLE_MEDIUM      ET_PRIORITY_BLE_MEDIUM      ///< Event Thread BLE_MEDIUM
#define ET_PREEMPTION_THRESHOLD_BLE_LOW         ET_PRIORITY_BLE_LOW         ///< Event Thread BLE_LOW

/* DC motors thread */
#define PT_PREEMPTION_THRESHOLD_DCM             DCM_PRIORITY  ///< Periodic Thread DC motors

/* SPIA thread */
#define PT_PREEMPTION_THRESHOLD_SPIA            SPIA_PRIORITY ///< Periodic Thread SPIA

/***************************************************************************************************
 * CONFIG: RTD THREAD STACK SIZE
 * *************************************************************************************************
 * \brief: Stack sizes for each Individual Thread can be assigned here.
 *
 *         The Stack size varys for each thread depending on the content of its Tasks, ISRs, etc.
 *         Excess stack can waste memory where insufficient stack can cause stack overflow,
 *         therefore user is responsible for finding out appropriate stack size for each Thread.
 *         
 *         By default, 1024 Bytes are assigned to each thread, except Interrupt thread, which is
 *         allocated 512 bytes, as ISRs are relatively smaller, the stack requirements are usually
 *         not as high as scheduling threads, but if extra memory is needed, it can be configured here.
 * *************************************************************************************************
 * \note:  If a Stack Overflow occurs for any Thread, Error is notified by setting the variable
 *         'RTD_RuntimeCatalogue.RuntimeInfo.Error_StackOverflow' true.
 *         The Callback void RTD_ErrorStackOverflowHandler(const ThreadID thread_id) is called.
 *         The Stack Size has to be always multiple of 4 like 256x4 = 1024.
 *         There can be a minimum Stacksize which depends on the OS.
 *
 *         Currently supported:
 *         ThreadX v61: TX_MINIMUM_STACK (=200)
 ***************************************************************************************************/
#define IT_STACK_SIZE_IST                   (DEFAULT_STACK_SIZE/2U)     ///< Interrupt Servicing Thread
#define PT_STACK_SIZE_ULTRA_FAST            DEFAULT_STACK_SIZE          ///< Periodic Thread ULTRA FAST
#define PT_STACK_SIZE_SUPER_FAST            DEFAULT_STACK_SIZE          ///< Periodic Thread SUPER FAST
#define PT_STACK_SIZE_FAST                  DEFAULT_STACK_SIZE          ///< Periodic Thread FAST
#define PT_STACK_SIZE_MEDIUM                DEFAULT_STACK_SIZE          ///< Periodic Thread MEDIUM
#define PT_STACK_SIZE_SLOW                  DEFAULT_STACK_SIZE          ///< Periodic Thread SLOW
#define PT_STACK_SIZE_BM_COMPAT             DEFAULT_STACK_SIZE          ///< Periodic Thread BM_COMPAT
#define PT_STACK_SIZE_STA                   DEFAULT_STACK_SIZE          ///< Periodic Thread STA
#define ET_STACK_SIZE_BLUEBIRD              DEFAULT_STACK_SIZE          ///< Framework Thread
#define ET_STACK_SIZE_DBUS                  DEFAULT_STACK_SIZE          ///< Framework DBUS Thread
#define ET_STACK_SIZE_ORYX                  DEFAULT_STACK_SIZE          ///< Event Thread ORYX
#define ET_STACK_SIZE_SPIX_MACAW            DEFAULT_STACK_SIZE          ///< Event Thread SPIX_MACAW
#define ET_STACK_SIZE_VAQUITA               DEFAULT_STACK_SIZE          ///< Event Thread VAQUITA
#define ET_STACK_SIZE_KAKAPO                DEFAULT_STACK_SIZE          ///< Event Thread KAKAPO
#define ET_STACK_SIZE_WEKA                  DEFAULT_STACK_SIZE          ///< Event Thread WEKA

/* Bluetooth Module Threads */
#define ET_STACK_SIZE_BLE_LINK_LAYER        DEFAULT_STACK_SIZE          ///< Event Thread BLE_LINK_LAYER
#define ET_STACK_SIZE_BLE_HIGH              DEFAULT_STACK_SIZE          ///< Event Thread BLE_HIGH
#define ET_STACK_SIZE_BLE_MEDIUM            DEFAULT_STACK_SIZE          ///< Event Thread BLE_MEDIUM
#define ET_STACK_SIZE_BLE_LOW               DEFAULT_STACK_SIZE          ///< Event Thread BLE_LOW

/* DC motors thread */
#define PT_STACK_SIZE_DCM                   DEFAULT_STACK_SIZE          ///< Periodic DC motors thread

/* SPIA thread */
#define PT_STACK_SIZE_SPIA                  DEFAULT_STACK_SIZE          ///< Periodic SPIA thread


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
    MOD_BAL,           ///< module ID of dbus bus layer
    MOD_DBPL,          ///< module ID of dbus presentation layer
    MOD_DLL,           ///< module ID of dbus data link layer
#ifdef RTOS_DBUS_EVENTDRIVEN
    MOD_DBPL_RX,       ///< module ID of dbus presentation layer, receive messages
    MOD_DBPL_TX,       ///< module ID of dbus presentation layer, send service messages
    MOD_DBPL_PWR,      ///< module ID of dbus presentation layer, send power message
    MOD_DLL_TX_INIT,   ///< module ID of dbus data link layer, initialization of a transmission
    MOD_DLL_TX_DONE,   ///< module ID of dbus data link layer, transmission finished
    MOD_DLL_ISR,       ///< module ID of dbus data link layer, DBUS-CAN chip ISR tiggered
#endif
    MOD_TIM,           ///< module ID of timer library
    MOD_SYSL,          ///< module ID for System Load measurement
    MOD_WDT,
    /*! place your projects modules here */
#ifdef CCSS
    MOD_CCSS,         ///< module ID of SystemStates Taskhandler
#endif
#ifdef CCSS_DOM_REF
    MOD_CCDOM,        ///< module ID of SystemStates Domain Reference implementation. to be replaced with an application specific implementation.
#endif
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    MOD_FSF,
#endif
#ifdef BLUETOOTH_MODULE
    /*! place bluetooth connectivity modules here */


#endif

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
 * \brief:  If needed, activate the RTOS tracing here.
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
 *
 * *************************************************************************************************
 * \note:   1. By default, all these features are activated. User can DEACTIVATE them below if needed.
 *          2. To obtain information about all above features, please check the section DEBUG INFORMATION
 *             in rtos_api.h.
 ***************************************************************************************************/
#define TASK_TIME_MONITORING            FEATURE_ACTIVATED
#define ICS_CONFLICTS_MONITORING        FEATURE_DEACTIVATED
#ifdef SYSTICK_1MS
    #define THREAD_1MS_LOAD_MONITORING  FEATURE_ACTIVATED
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
 * \brief: Number of Timing Samples for each module to be saved, by default 1 samples!
 * *************************************************************************************************
 * \note:  With more number of Samples, the RAM Memory Requirements go higher.
 ***************************************************************************************************/
#define NUMBER_OF_SAMPLES       1U      //< Select number of samples


/***************************************************************************************************
 * CONFIG: SELECT MOVING AVERAGE WINDOW SIZE
 * *************************************************************************************************
 * \brief: The moving average is calculated continuously, the window size influences the weight
 *         between the historic and the new value 
 * *************************************************************************************************
 * \note:  Variables for below calculation 
 *              RTDTM_MOVING_AVERAGE_WINDOW_SIZE = n, 
 *              previously calculated average = p.
 *              current value = c
 *         The new moving average is then calculated by 
 *              ((p*(n-1) / n) + (c/n))
 ***************************************************************************************************/
#define RTDTM_MOVING_AVERAGE_WINDOW_SIZE (20U)  //< weight of newest value will be 1/RTDTM_MOVING_AVERAGE_WINDOW_SIZE 

#endif



/***************************************************************************************************
 * EVENT FLAG GROUP
 * *************************************************************************************************
 *  \brief  Extern declaration of a dummy efg to provide it to the customer
 *
 ***************************************************************************************************/
extern RTOS_EVENT_FLAG_GROUP EFG_DUMMY;

#endif // #ifdef REAL_TIME_DISPATCHER_CONFIG_AUTO
#endif // #ifndef REAL_TIME_DISPATCHER_CONFIG_H_
/*************************************** End of File **********************************************/
