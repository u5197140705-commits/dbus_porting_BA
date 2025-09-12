/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          IC Dbus2
 ******************************************************************************/


#ifndef DBUS_RTOS_INTERFACE_H__
#define DBUS_RTOS_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *
 *  \brief    Implementation of interface to scheduling, header file
 *
 *  \details  This layer handles the interface to RTOS
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#ifdef RTOS
#include "rtos_api.h"
#include "rtd/real_time_dispatcher.h"
#endif
#include "bal.h"
#include "dbuspresentation.h"
#include "dbusdll.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** \brief    Depedencies of DBUS lib
 *
 *  \details  Map existing dependencies, if eventdriven solution is not used
 */
#ifdef RTOS_DBUS_EVENTDRIVEN
#define DBR_DEPLIB_BAL //No dependencies
#define DBR_DEPLIB_DBPL MOD_TIM,
#define DBR_DEPLIB_DLL  MOD_TIM,
#else
#define DBR_DEPLIB_BAL  DEPLIB_BAL
#define DBR_DEPLIB_DBPL DEPLIB_DBPL
#define DBR_DEPLIB_DLL  DEPLIB_DLL
#endif


/** \brief    Eventflags for dedicated function events
 *
 *  \details  If RTOS eventdriven solution used, map dedicated flag,
 *            which is used to set a event within RTOS DBUS thread.
 *            If RTOS eventdriven solution is NOT used, map a dummy
 *            for unused flags instead.
 *
 */
#define DBR_BAL_FLAG                (uint32_t)DBR_MAP_EVENTFLAG_RTOS( FLAG_BAL )
#define DBR_DBPL_FLAG               (uint32_t)DBR_MAP_EVENTFLAG_RTOS( FLAG_DBPL )
#define DBR_DLL_FLAG                (uint32_t)DBR_MAP_EVENTFLAG_RTOS( FLAG_DLL )
#define DBR_DBPL_RX_FLAG            (uint32_t)DBR_MAP_EVENTFLAG_RTOS_ED( FLAG_DBPL_RX )
#define DBR_DBPL_TX_FLAG            (uint32_t)DBR_MAP_EVENTFLAG_RTOS_ED( FLAG_DBPL_TX )
#define DBR_DBPL_PWR_MSG_FLAG       (uint32_t)DBR_MAP_EVENTFLAG_RTOS_ED( FLAG_DBPL_PWR_MSG )
#define DBR_DLL_TX_INIT_FLAG        (uint32_t)DBR_MAP_EVENTFLAG_RTOS_ED( FLAG_DLL_TX_INIT )
#define DBR_DLL_TX_DONE_FLAG        (uint32_t)DBR_MAP_EVENTFLAG_RTOS_ED( FLAG_DLL_TX_DONE )
#define DBR_DLL_ISR_FLAG            (uint32_t)DBR_MAP_EVENTFLAG_RTOS_ED( FLAG_DLL_ISR )

#define DBR_EVENTFLAG_DUMMY         0x00000000


/** \brief    Task IDs, used for eventdriven tasks
 *
 *  \details  If RTOS eventdriven solution used, map dedicated Task ID
 *            of realtime dispatcher.
 *            If RTOS eventdriven solution is NOT used, some tasks are
 *            unused and a dummy is mapped instead.
 */
#define DBR_ED_TASK_BAL             (uint8_t)DBR_MAP_TASK_RTOS( MOD_BAL )
#define DBR_ED_TASK_DBPL            (uint8_t)DBR_MAP_TASK_RTOS( MOD_DBPL )
#define DBR_ED_TASK_DLL             (uint8_t)DBR_MAP_TASK_RTOS( MOD_DLL )
#define DBR_ED_TASK_DBPL_RX         (uint8_t)DBR_MAP_TASK_RTOS_ED( MOD_DBPL_RX )
#define DBR_ED_TASK_DBPL_TX         (uint8_t)DBR_MAP_TASK_RTOS_ED( MOD_DBPL_TX )
#define DBR_ED_TASK_DBPL_PWR        (uint8_t)DBR_MAP_TASK_RTOS_ED( MOD_DBPL_PWR )
#define DBR_ED_TASK_DLL_TX_INIT     (uint8_t)DBR_MAP_TASK_RTOS_ED( MOD_DLL_TX_INIT )
#define DBR_ED_TASK_DLL_ISR         (uint8_t)DBR_MAP_TASK_RTOS_ED( MOD_DLL_ISR )

#define DBR_TASK_DUMMY              0xFF


/******************************************************************************/
/* PUBLIC MACRO FUNCTION DEFINITIONS                                          */
/******************************************************************************/

/** \brief    Maps RTOS eventflag
 *
 *  \details  If RTOS is used, map the dedicated flag of RTOS EDS.
 *            If not, a dummy is used.
 *
 */
#ifdef RTOS
#define DBR_MAP_EVENTFLAG_RTOS( flag )   ( flag )
#else
#define DBR_MAP_EVENTFLAG_RTOS( flag )   ( DBR_EVENTFLAG_DUMMY )
#endif


/** \brief    Maps RTOS eventflag for eventdriven solution
 *
 *  \details  If RTOS eventdriven solution is used,
 *            map the dedicated flag of RTOS EDS.
 *            If not, a dummy is used.
 *
 */
#ifdef RTOS_DBUS_EVENTDRIVEN
#define DBR_MAP_EVENTFLAG_RTOS_ED( flag )   ( flag )
#else
#define DBR_MAP_EVENTFLAG_RTOS_ED( flag )   ( DBR_EVENTFLAG_DUMMY )
#endif


/** \brief    Maps RTOS task ID
 *
 *  \details  If RTOS is used, map the dedicated task ID of RTOS RTD.
 *            If not, a dummy is used.
 *
 */
#ifdef RTOS
#define DBR_MAP_TASK_RTOS( task )        ( task )
#else
#define DBR_MAP_TASK_RTOS( task )        ( DBR_TASK_DUMMY )
#endif


/** \brief    Maps RTOS task ID for eventdriven solution
 *
 *  \details  If RTOS eventdriven solution is used,
 *            map the dedicated task ID of RTOS RTD.
 *            If not, a dummy is used.
 *
 */
#ifdef RTOS_DBUS_EVENTDRIVEN
#define DBR_MAP_TASK_RTOS_ED( task )        ( task )
#else
#define DBR_MAP_TASK_RTOS_ED( task )        ( DBR_TASK_DUMMY )
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Handles the interface of RTOS OS and BAL
 *
 *  \details  If eventflag DBR_BAL_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED: It's required, to call function again
 *                      TASK_TERMINATE: Handling done for current iteration, do not call again
 *                                      (only for eventdriven RTOS-DBUS solution)
 *
 */
extern uint8_t DBR_Handle_BAL( void );

/** \brief    Handles the interface of RTOS OS and DBPL
 *
 *  \details  If eventflag DBR_DBPL_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *            If RTOS event-driven DBUS implementation is used:
 *            Handles the initialization of DBPL
 *
 *            else:
 *            function simply maps the call of DBPL_Handletask()
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED: It's required, to call function again
 *                      TASK_TERMINATE: Handling done for current iteration, do not call again
 *                                      (only for eventdriven RTOS-DBUS solution)
 *                      TASK_NOT_INITIALISED: Initialization is ongoing
 *                                            (only for eventdriven RTOS-DBUS solution)
 *
 */
extern uint8_t DBR_Handle_DBPL( void );

/** \brief    Handles the interface of RTOS OS and DLL
 *
 *  \details  If eventflag DBR_DLL_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *            If RTOS event-driven DBUS implementation is used:
 *            Handles the initialization of DLL
 *
 *            else:
 *            function simply maps the call of DLL_Handletask()
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED: It's required, to call function again
 *                      TASK_TERMINATE: Handling done for current iteration, do not call again
 *                                      (only for eventdriven RTOS-DBUS solution)
 *                      TASK_NOT_INITIALISED: Initialization is ongoing
 *                                            (only for eventdriven RTOS-DBUS solution)
 *
 */
extern uint8_t DBR_Handle_DLL( void );

#ifdef RTOS_DBUS_EVENTDRIVEN
/** \brief    Function, which realizes eventdriven call of the abstacted
 *            function DBPL_ReceiveMessage() out of DBPL_HandleTask()
 *
 *  \details  If eventflag DBR_DBPL_RX_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED: It's required, to call function again
 *                      TASK_TERMINATE: Handling done for current iteration, do not call again
 *
 */
extern uint8_t DBR_ReceiveMessage( void );

/** \brief    Function, which realizes eventdriven call of the abstacted
 *            function DBPL_SendServiceMessage() out of DBPL_HandleTask()
 *
 *  \details  If eventflag DBR_DBPL_TX_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED: It's required, to call function again
 *                      TASK_TERMINATE: Handling done for current iteration, do not call again
 *
 */
extern uint8_t DBR_SendServiceMessage( void );

/** \brief    Function, which realizes eventdriven call of the abstacted
 *            function DBPL_SendPowerMessage() out of DBPL_HandleTask()
 *
 *  \details  If eventflag DBR_DBPL_PWR_MSG_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED: It's required, to call function again
 *                      TASK_TERMINATE: Handling done for current iteration, do not call again
 *
 */
extern uint8_t DBR_SendPowerMessage( void );

/** \brief    Function, which realizes eventdriven call of the abstacted
 *            function DBR_TransmitFrameInit() out of DLL_HandleTask()
 *
 *  \details  If eventflag DBR_DLL_TX_INIT_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED: It's required, to call function again
 *                      TASK_TERMINATE: Handling done for current iteration, do not call again
 *
 */
extern uint8_t DBR_TransmitFrameInit( void );

/** \brief    Function, which realizes eventdriven call of the abstacted
 *            function DBR_TransmitFrameDone() out of DLL_HandleTask()
 *
 *  \details  If eventflag DBR_DLL_TX_DONE_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED
 *
 */
extern uint8_t DBR_TransmitFrameDone( void );

/** \brief    Function, which realizes eventdriven call of the abstracted
 *            function DBR_HandleInterrupt() out of DLL_HandleTask()
 *
 *  \details  If eventflag DBR_DLL_ISR_FLAG in RTOS EDS is set, this function
 *            will be called.
 *
 *  \param    none
 *
 *  \return   uint8_t   TASK_INITIALISED: It's required, to call function again
 *                      TASK_TERMINATE: Handling done for current iteration, do not call again
 *
 */
extern uint8_t DBR_HandleInterrupt( void );

#endif // RTOS_DBUS_EVENTDRIVEN

/**
 *  \brief    If RTOS eventdriven solution used:
 *            API to set eventflag witin DBUS EFG thread. Dedicated eventfunction in this module
 *            Will be called from OS. Function is used, to call eventfunction one time with no
 *            repetitions.
 *
 *  \param    uint32_t task_flag    -  Flag that represents the Event Driven Task
 *                                     in DBUS EFG thread
 *
 *  \return   none
 *
 *  \details
 */
extern void DBR_SetEventflag( uint32_t task_flag );

/**
 *  \brief    If RTOS eventdriven solution used:
 *            API to start runnig an eventdriven task of DBUS EFG thread.
 *            Eventfunctions are called till TASK_TERMINATED is returned.
 *
 *  \param    uint32_t task_flag    -  Flag that represents the Event Driven Task
 *                                     in DBUS EFG thread
 *
 *  \return   none
 *
 *  \details
 */
extern void DBR_RunEventdrivenDbusTask( uint8_t task_id );

#ifdef __cplusplus
}
#endif

#endif // DBUS_RTOS_INTERFACE_H__
