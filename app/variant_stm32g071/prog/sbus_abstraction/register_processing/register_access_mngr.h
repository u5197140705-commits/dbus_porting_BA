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
 *  COMP_ABBREV      REGXS
 ******************************************************************************/

#ifndef REGISTER_ACCESS_MNGR_H
#define REGISTER_ACCESS_MNGR_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     register_access_mngr.h
 *
 *  \ingroup  sbus_abstraction/register_processing
 *
 *  \brief    Interfacing of the ASIC-register access manager of the
 *            SSB-abstraction layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
extern "C" {

#include "bsh_stdinc.h"
#include "debug_extended/api_cfg.h"
}

#include "errors.h"
#include "register_access.h"
#include "ssbf_mngr_common.h"
#include "si_data_mngr.h"
#include "timers_mngr.h"
#include "ssb_task.h"

/******************************************************************************/
/* CLASS-IF DEFINITION                                                        */
/******************************************************************************/
namespace SSBAL
{
    namespace SSBR
    {
        class RegisterAccessMngr_c : public RegisterAccess_c, public SSBF::SiDataMngr_c, public SSBF::TimersMngr_c
        {
        private:

        public:
            /** \brief  Creates the object of this class and of the inherited
            *           classes and initializes the generic part of the register
            *           access
            *
            *   \details
            */
            RegisterAccessMngr_c(void);

            /** \brief  Initializes the layer of this manager class
            *           and starts initializing the next layer below 
            *
            *   \details
            */
            void initRegisterAccessMngr(uint8_t i2cAddrOffsets);
        };
    }
}

#endif // From: #ifndef REGISTER_ACCESS_MNGR_H

