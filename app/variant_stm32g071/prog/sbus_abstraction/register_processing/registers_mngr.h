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
 *  COMP_ABBREV      REGS
 ******************************************************************************/

#ifndef REGISTERS_MNGR_H
#define REGISTERS_MNGR_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     registers_mngr.h
 *
 *  \ingroup  sbus_abstraction/register_processing
 *
 *  \brief    Interfacing of the ASIC-register processing manager of the
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
#include "registers.h"
#include "register_access_mngr.h"

/******************************************************************************/
/* CLASS-IF DEFINITION                                                        */
/******************************************************************************/
namespace SSBAL
{
    namespace SSBR
    {
        class RegistersMngr_c : public Registers_c, public RegisterAccessMngr_c
        {
        private:

        public:
            /** \brief  Creates the object of this class and of the inherited
            *           classes and initializes the generic part of the register
            *           processing
            *
            *   \details
            */
            RegistersMngr_c(void);

            /** \brief  Initializes the layer of this manager class
            *           and starts initializing the next layer below 
            *
            *   \details
            */
            void initRegistersMngr(uint8_t i2cAddrOffsets);
        };
    }
}

#endif // From: #ifndef REGISTERS_MNGR_H

