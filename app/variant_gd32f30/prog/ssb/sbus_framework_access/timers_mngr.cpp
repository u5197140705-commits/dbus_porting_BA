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
 *  COMP_ABBREV      TIM
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     timers_mngr.cpp
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    Timers manager processing of the SSB-framework access layer
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "timers_mngr.h"

using namespace ::SSBF;

/*lint -e40 Usage of nullptr does not cause errors and is recommended */

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* STATIC ATTRIBUTE DECLARATIONS                                              */
/******************************************************************************/

/******************************************************************************/
/* METHODS                                                                    */
/******************************************************************************/
TimersMngr_c::TimersMngr_c(void)
{
    DBGX_logStrInt_SCN_SSB_CONSTR("Constructor: TimersMngr_c: ", static_cast<uint32_t>(this),
                                  DBGX_UNSIGNED_HEXADECIMAL);
}

void TimersMngr_c::initTimersMngr(void)
{
    DBGX_logStr_SCN_SSB_INIT("INI initTimersMngr");

    initTimers();
}

/*lint +e40 */

