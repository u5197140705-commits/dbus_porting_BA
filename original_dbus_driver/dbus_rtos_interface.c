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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *
 *  \brief    Implementation of interface to scheduling
 *
 *  \details  This layer handles the interface to RTOS
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbus_rtos_interface.h"
#include "hsup.h"

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
uint8_t DBR_Handle_BAL( void )
{
#ifdef RTOS_DBUS_EVENTDRIVEN
    return (BAL_SendMessage() == false) ? TASK_INITIALISED : TASK_TERMINATE;
#else
    return( BAL_HandleTask() );
#endif
}


uint8_t DBR_Handle_DBPL( void )
{
    uint8_t RetVal;

#ifdef RTOS_DBUS_EVENTDRIVEN
    static bool WasInit = false;

    //Ensure TASK_INITIALISED was returned once
    //Return TASK_TERMINATE in the following function-call
    if( WasInit == false )
    {
        DBPL_ModuleInit();

        //Terminate tasks for now. Will be reactivated if required
        /*lint -save -e9034 tested to work correctly */
        RTOS_terminateTask( DBR_ED_TASK_DBPL_RX );
        RTOS_terminateTask( DBR_ED_TASK_DBPL_TX );
        RTOS_terminateTask( DBR_ED_TASK_DBPL_PWR );
        /*lint -restore -e9034 */

        WasInit = true;
        RetVal = TASK_INITIALISED;
    }
    else
    {
        RetVal = TASK_TERMINATE;
    }
#else
    RetVal = DBPL_HandleTask();
#endif //RTOS_DBUS_EVENTDRIVEN

    return RetVal;
}


uint8_t DBR_Handle_DLL( void )
{
    uint8_t RetVal;

#ifdef RTOS_DBUS_EVENTDRIVEN
    static bool WasInit = false;

    //Ensure TASK_INITIALISED was returned once
    //Return TASK_TERMINATE in the following function-call
    if( WasInit == false )
    {
        //Terminate task for now. Will be reactivated if required
        /*lint -save -e9034 tested to work correctly */
        RTOS_terminateTask( DBR_ED_TASK_DLL_TX_INIT );
        RTOS_terminateTask( DBR_ED_TASK_DLL_ISR );
        /*lint -restore -e9034 */

        if( DLL_moduleInit() == false )
        {
            RetVal = TASK_NOT_INITIALISED;
        }
        else
        {
            WasInit = true;
            RetVal = TASK_INITIALISED;
        }
    }
    else
    {
        WasInit = false; //Prepare for a later reinit
        RetVal = TASK_TERMINATE;
    }
#else
    RetVal = DLL_HandleTask();
#endif //RTOS_DBUS_EVENTDRIVEN

    return RetVal;
}


#ifdef RTOS_DBUS_EVENTDRIVEN
uint8_t DBR_ReceiveMessage( void )
{
    return (DBPL_ReceiveMessage() == false) ? TASK_INITIALISED : TASK_TERMINATE;
}


uint8_t DBR_SendServiceMessage( void )
{
    return (DBPL_SendServiceMessage() == false) ? TASK_INITIALISED : TASK_TERMINATE;
}


uint8_t DBR_SendPowerMessage( void )
{
    return (DBPL_SendPowerMessage() == false) ? TASK_INITIALISED : TASK_TERMINATE;
}


uint8_t DBR_TransmitFrameInit( void )
{
    return (DLL_sendingInit() == false) ? TASK_INITIALISED : TASK_TERMINATE;
}


uint8_t DBR_TransmitFrameDone( void )
{
    DLL_sendingDone();
    return TASK_INITIALISED;
}


uint8_t DBR_HandleInterrupt( void )
{
    return (DLL_handleInterrupt() == false) ? TASK_INITIALISED : TASK_TERMINATE;
}
#endif //RTOS_DBUS_EVENTDRIVEN


void DBR_SetEventflag( uint32_t task_flag )
{
#ifdef RTOS_DBUS_EVENTDRIVEN
    //A possible error would be set on lower layer
    (void)RTOS_runEventDrivenTask( task_flag, RTOS_THREAD_ID_DBUS );
#else
    (void)task_flag;
    HSUP_vNop();
#endif
}


void DBR_RunEventdrivenDbusTask( uint8_t task_id )
{
#ifdef RTOS_DBUS_EVENTDRIVEN
    RTOS_reactivateTask( task_id ); /*lint -e9034 tested to work correctly */
#else
    (void)task_id;
    HSUP_vNop();
#endif
}
