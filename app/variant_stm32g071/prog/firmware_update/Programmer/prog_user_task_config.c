/*******************************************************************************
 *  Copyright (c) 2020 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    configuration file for user-defined handle function inside Programmer
 *
 *  \details  section shall be filled by developers
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "firmware_update/Programmer/prog_user_task.h"
//#include "watchdogtimer/watchdogtimer.h"

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
bool PRG_bUserHandleTask(void)
{
    /*
    BE CAREFUL!
    - SIZE OF FUNCTION
       User-specific code will inflate Programmer's size which may 
       result in overlapping with other memory sections. In such cases 
       enlargement of Programmer's reserved space might be needed.
    - DURATION TIME OF FUNCTION
       Be aware of keeping optimized code in user specific function. Not well
       optimized user specific function can affect a runtime of Programmer.
    - STATES OF FUNCTION
       Be aware of fact that return values of user specific function are ignored.
       The user specific function call is performed in simple endless loop unlike
       a call in the scheduler.
    */
    
    /*
    Add user specific code there.
    */
    
    //WDT_trigger();

    return true; // return value is ignored in the Programmer's scheduler
}
