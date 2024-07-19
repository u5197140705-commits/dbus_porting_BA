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

#ifndef TIMERS_MNGR_H
#define TIMERS_MNGR_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     timers_mngr.h
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    Interfacing of the timers manager processing of the SSB-framework
 *            access layer
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
extern "C" {

#include "bsh_stdinc.h"
#include "debug_extended/api_cfg.h"
}

#include "errors.h"
#include "timers.h"

/******************************************************************************/
/* CLASS-IF DEFINITION                                                        */
/******************************************************************************/
namespace SSBF
{
    class TimersMngr_c : public Timers_c
    {
    private:

    public:
        /** \brief  Creates the object of this class and of the inherited classes
        *           and initializes the generic part of the timers
        */
        TimersMngr_c(void);

        /** \brief  Initializes the layer of this manager class
        */
        void initTimersMngr(void);
    };
}

#endif // From: #ifndef TIMERS_MNGR_H

