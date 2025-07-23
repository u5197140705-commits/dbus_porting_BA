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
 *  COMP_ABBREV      HUB
 ******************************************************************************/

#ifndef HUB_MNGR_H
#define HUB_MNGR_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     hub_mngr.h
 *
 *  \ingroup  sbus_abstraction/constellation
 *
 *  \brief    Interfacing of the Hub manager processing (and Hub-pending Client
 *            processing and Hub-pending Device processing) of the
 *            SSB-abstraction layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
extern "C" {

#include "bsh_stdinc.h"
#include "debug_mapping.h"
}

#include "errors.h"
#include "ssbf_mngr_common.h"
#include "constellation/hub.h"
#include "register_processing/registers_mngr.h"
#include "constellation/ssb_config_auto.h"

/******************************************************************************/
/* CLASS-IF DEFINITION                                                        */
/******************************************************************************/
namespace SSBAL
{
    namespace SSBCO
    {
        class HubMngr_c : public Hub_c, public SSBAL::SSBR::RegistersMngr_c
        {
        private:
            bool HubInitialized = false;
                     ///< Flag, whether the Hub has been initialized

        public:
            /** \brief  Creates the object of this class and of the inherited
            *           classes and initializes the generic part of the Hub
            *           processing
            *
            *   \details
            */
            HubMngr_c(void);

            /** \brief  Initializes the layer of this manager class
            *           and starts initializing the next layer below 
            *
            *   \details
            */
            void initHubMngr(uint16_t cfgIdx, uint8_t i2cAddrOffsets, 
                             uint8_t configurationToBeLoaded);
        };
    }
}

#endif // From: #ifndef HUB_MNGR_H
