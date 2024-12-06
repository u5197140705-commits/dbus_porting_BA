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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     hub_mngr.cpp
 *
 *  \ingroup  sbus_abstraction/constellation
 *
 *  \brief    Hub manager processing (and Hub-pending Client processing and
 *            Hub-pending Device processing) of the SSB-abstraction layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "constellation/hub_mngr.h"

using namespace ::SSBAL::SSBCO;
using namespace ::SSBF;

/*lint -e40 Usage of nullptr does not cause errors and is recommended @@ */

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* STATIC ATTRIBUTE DECLARATIONS                                              */
/******************************************************************************/
extern "C" {

static SSBF_CallbackFromTaskFctWrap_t 
    SSBCO_CallbackToHubFromTaskFctPtrArray[SSBF_MNGR_NUMBER_OF_HUBS 
                                    * SSBF_MNGR_NUMBER_OF_LAYERS_NOTIFIED]
#if   SSBF_MNGR_NUMBER_OF_HUBS == 1U
                      = {nullptr, nullptr, nullptr};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 2U
                      = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 3U
                      = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 4U
                      = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
#else
    #error SSBF_MNGR_NUMBER_OF_HUBS has wrong value
#endif

static void *SSBCO_CalleeObjPtrToHubFromTaskArray[SSBF_MNGR_NUMBER_OF_HUBS 
                                    * SSBF_MNGR_NUMBER_OF_LAYERS_NOTIFIED]
#if   SSBF_MNGR_NUMBER_OF_HUBS == 1U
                      = {nullptr, nullptr, nullptr};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 2U
                      = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 3U
                      = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 4U
                      = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
#else
    #error SSBF_MNGR_NUMBER_OF_HUBS has wrong value
#endif
}

/******************************************************************************/
/* METHODS                                                                    */
/******************************************************************************/
HubMngr_c::HubMngr_c(void)
{
    DBGX_logStrInt_SCN_SSB_CONSTR0("Constructor: HubMngr_c: ", (uint32_t)this,
                                   DBGX_UNSIGNED_HEXADECIMAL);
}

void HubMngr_c::initHubMngr(uint16_t cfgIdx, uint8_t i2cAddrOffsets, 
                            uint8_t configurationToBeLoaded)
{
    class Registers_c *registersObjPtr;

    #ifdef DBGX_INCLUDED
        DBGX_init();
    #endif

    DBGX_logStr_SCN_SSB_INIT0("INI initHubMngr");

    if (HubInitialized != true)
    {
        HubInitialized = true;

        /* ---------------------------------------------------------- */

        SSBF_locateCallbacksFromTask(
                SSBCO_CallbackToHubFromTaskFctPtrArray,
                SSBCO_CalleeObjPtrToHubFromTaskArray,
                (uint8_t)SSBF_MNGR_NUMBER_OF_HUBS,
                SSBF_MNGR_NUMBER_OF_LAYERS_NOTIFIED);

        /* ---------------------------------------------------------- */

        registersObjPtr = getRegistersObjPtr();
                                         // From SSBAL::SSBR::Registers_c::

        SSBERR_handleErrDbgIf(registersObjPtr == nullptr,
                              SSB_ERR_HUBMNGR_GET_OBJPTR);

        associateRegistersObjPtr(registersObjPtr); 
                                         // From Hub_c::

        /* ---------------------------------------------------------- */

        initHubProcessing((uint8_t)SSBF_MNGR_NUMBER_OF_HUBS, cfgIdx,
                          configurationToBeLoaded);
                                         // From Hub_c::

        initRegistersMngr(i2cAddrOffsets);
                                         // From SSBAL::SSBR::RegistersMngr_c::

        setSsbConfigurations(
            SsbConfigurations_c::SsbExplictiteConfigurationParameters,
            SsbConfigurations_c::SsbConfigurationBytes,
            SSB_NUMBER_OF_CONFIGURATION_BYTES);
                                         // Origin of values: ssb_config_auto.cpp
    }
}

/*lint -e40 @@ */

