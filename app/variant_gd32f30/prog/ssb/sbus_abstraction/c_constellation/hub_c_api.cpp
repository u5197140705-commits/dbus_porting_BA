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
 *  COMP_ABBREV      HUBC
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     hub_c_api.cpp
 *
 *  \ingroup  sbus_abstraction/c_constellation
 *
 *  \brief    C-API for the C++ Hub class (with the processing for the Hubs
 *            and the functionality for the Hub-connected Clients and Devices)
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "c_constellation/hub_c_api_internal.h"
//lint -e553 Warning 553: undefined preprocessor variable '__STDC_VERSION__',
//                        assumed 0 [MISRA 2012 Rule 20.9, required]
//                        The message is absolutely incomprehensible, 
//                        because __STDC_VERSION__ is a predefined standard macro!
//                       (used in dbus/bustypes.h)
//lint +e553

extern "C" {
#include "c_constellation/hub_c_api.h"
}

using namespace ::SSBAL::SSBCO;

/******************************************************************************/
/* STATIC ATTRIBUTE DECLARATIONS                                              */
/******************************************************************************/

/******************************************************************************/
/* STATIC VARIABLES                                                           */
/******************************************************************************/
#ifndef SSB_USE_CPP_INSTEAD_OF_C_API
static SSBF_CallbackFct_t CallbackToCapiFctPtr;
#endif

/******************************************************************************/
/* STATIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
extern "C" {

#ifndef SSB_USE_CPP_INSTEAD_OF_C_API
    static void SSB_HUBC_setupHub(uint8_t hubIdx, uint16_t cfgIdx,
                       uint8_t i2cAddrOffsets, uint8_t configurationToBeLoaded);

    static void doForCallbackToApiWrap(
                       void *objPtr, uint16_t eventToken,
                       const uint8_t *eventDataPtr, uint8_t eventDataLen);
#endif
}

/******************************************************************************/
/* OBJECTS                                                                    */
/******************************************************************************/

#ifndef SSB_USE_CPP_INSTEAD_OF_C_API
namespace SSBAL
{
    namespace SSBCC
    {
        class HubMngr_c HubObject[SSBF_MNGR_NUMBER_OF_HUBS];
    }
}
#endif

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
extern "C" {

#ifndef SSB_USE_CPP_INSTEAD_OF_C_API
    void SSB_HUBC_setupHubs(uint8_t i2cAddrOffsets, uint8_t configurationToBeLoaded)
                                    // Is called in the DBus 
                                    // message handler of 
                                    // MsgSsbTestModeStartRequest
    {
#if   SSBF_MNGR_NUMBER_OF_HUBS == 1U
       SSB_HUBC_setupHub(static_cast<uint8_t>(0x00U), static_cast<uint16_t>(0x3311U), i2cAddrOffsets,
                          configurationToBeLoaded);
#elif SSBF_MNGR_NUMBER_OF_HUBS == 2U
       SSB_HUBC_setupHub(static_cast<uint8_t>(0x01U), static_cast<uint16_t>(0x3312U), i2cAddrOffsets,
                          configurationToBeLoaded);
#elif SSBF_MNGR_NUMBER_OF_HUBS == 3U
       SSB_HUBC_setupHub(static_cast<uint8_t>(0x02U), static_cast<uint16_t>(0x3313U), i2cAddrOffsets,
                          configurationToBeLoaded);
#elif SSBF_MNGR_NUMBER_OF_HUBS == 4U
       SSB_HUBC_setupHub(static_cast<uint8_t>(0x03U), static_cast<uint16_t>(0x3314U), i2cAddrOffsets,
                          configurationToBeLoaded);
#else
    #error SSBF_MNGR_NUMBER_OF_HUBS has wrong value
#endif
    }

    //lint -e715 objPtr is not used here, because the info regarding
    //           hub instance is the hubIdx implizit in the eventToken parameter
    static void doForCallbackToApiWrap(
                       void *objPtr, uint16_t eventToken,
                       const uint8_t *eventDataPtr, uint8_t eventDataLen)
    {
        DBGX_logStrInt_SCN_SSB_CBACK0("doForCallbackToApiWrap: eventToken: ",
                                      eventToken, DBGX_UNSIGNED_HEXADECIMAL);

        DBGX_logStrIntArr_SCN_SSB_CBACK0("doForCallbackToApiWrap: *eventDataPtr: ",
                                         eventDataPtr, eventDataLen,
                                         DBGX_UNSIGNED_HEXADECIMAL);

        CallbackToCapiFctPtr (eventToken, eventDataPtr, eventDataLen);
          // After incoming MsgSsbTestModeStartRequest:
          //   Calls SSBD_CallbackFct in the DBus testmode
          // Without incoming MsgSsbTestModeStartRequest:
          //   Calls API-function of the CPP-application of appliance project
    }

    static void SSB_HUBC_setupHub(uint8_t hubIdx, uint16_t cfgIdx,
                       uint8_t i2cAddrOffsets, uint8_t configurationToBeLoaded)
    {
        SSBAL::SSBCC::HubObject[hubIdx].initHubMngr(cfgIdx, i2cAddrOffsets, 
                                               configurationToBeLoaded);
                                                          // From SSBAL::SSBCC::
        SSBAL::SSBCC::HubObject[hubIdx].notifyCallbackToApi(nullptr,
                                               doForCallbackToApiWrap);
    }
    //lint +e715

    /* ---------------------------------------------------------------------- */

    void SSB_HUBC_notifyCallbackToCapi(SSBF_CallbackFct_t callBackToCapiFctPtr)
                                     // Will be called in the DBus message handler
                                     // of MsgSsbTestModeStartRequest after calling
                                     // SSB_HUBC_setupHubs
    {
        CallbackToCapiFctPtr = callBackToCapiFctPtr;
    }

    /* ---------------------------------------------------------------------- */

    //lint -e778 the result '0' is OK!
    void SSB_HUBC_writeHubRamRegisters_C(
                uint8_t hubIdx, uint8_t pageSelectOption, uint8_t registerAddr,
                uint8_t numberOfWriteBytes, uint8_t *writeBytesPtr)
    {
        if (hubIdx > (SSBF_MNGR_NUMBER_OF_HUBS - 1U)) {
            hubIdx = SSBF_MNGR_NUMBER_OF_HUBS - 1U;
        }
        SSBAL::SSBCC::HubObject[hubIdx].writeHubRamRegisters(
                pageSelectOption, registerAddr,
                numberOfWriteBytes, writeBytesPtr);
    }
    //lint +e778
    
    //lint -e778 the result '0' is OK!
    void SSB_HUBC_readHubRamRegisters_C(
                uint8_t hubIdx, uint8_t pageSelectOption, uint8_t registerAddr,
                uint8_t numberOfReadBytes)
    {
        if (hubIdx > (SSBF_MNGR_NUMBER_OF_HUBS - 1U)) {
            hubIdx = SSBF_MNGR_NUMBER_OF_HUBS - 1U;
        }
        SSBAL::SSBCC::HubObject[hubIdx].readHubRamRegisters(
                pageSelectOption, registerAddr,
                numberOfReadBytes);
    }
    //lint +e778

   //lint -e778 the result '0' is OK!
   void SSB_HUBC_writeClientRamRegisters_C(
                uint8_t hubIdx, uint8_t clientIdx, uint8_t pageSelectOption, 
                uint8_t registerAddr,
                uint8_t numberOfWriteBytes, uint8_t *writeBytesPtr)
    {
        if (hubIdx > (SSBF_MNGR_NUMBER_OF_HUBS - 1U)) {
            hubIdx = SSBF_MNGR_NUMBER_OF_HUBS - 1U;
        }
        SSBAL::SSBCC::HubObject[hubIdx].writeClientRamRegisters(
                clientIdx, pageSelectOption, registerAddr,
                numberOfWriteBytes, writeBytesPtr);
   }
    //lint +e778
    
    //lint -e778 the result '0' is OK!
    void SSB_HUBC_readClientRamRegisters_C(
                uint8_t hubIdx, uint8_t clientIdx, uint8_t pageSelectOption,
                uint8_t registerAddr,
                uint8_t numberOfReadBytes)
    {
        if (hubIdx > (SSBF_MNGR_NUMBER_OF_HUBS - 1U)) {
            hubIdx = SSBF_MNGR_NUMBER_OF_HUBS - 1U;
        }
        SSBAL::SSBCC::HubObject[hubIdx].readClientRamRegisters(
                clientIdx, pageSelectOption, registerAddr,
                numberOfReadBytes);
    }
    //lint +e778
 
    //lint -e778 the result '0' is OK!
    void SSB_HUBC_transferDeviceFrame_C(
                uint8_t hubIdx, uint8_t clientIdx, uint8_t deviceIdx, 
                uint8_t numberOfSendBytes, uint8_t numberOfReceiveBytes,
                uint8_t *sendBytesPtr)
    {
        if (hubIdx > (SSBF_MNGR_NUMBER_OF_HUBS - 1U)) {
            hubIdx = SSBF_MNGR_NUMBER_OF_HUBS - 1U;
        }
        SSBAL::SSBCC::HubObject[hubIdx].transferDeviceFrame(
                clientIdx, deviceIdx,
                numberOfSendBytes, numberOfReceiveBytes,
                sendBytesPtr);
    }
    //lint +e778
    
    //lint -e778 the result '0' is OK!
    //lint -e438 SSBAL::SSBCC::HubObject[hubIdx].StopLoop()is not yet implemented
    //lint -e715 SSBAL::SSBCC::HubObject[hubIdx].StartLoop() is not yet implemented
    //                         therefore is singleNotInfinite not used
    void SSB_HUBC_startMeasurementLoop_C(uint8_t hubIdx, uint8_t singleNotInfinite) {

        if (hubIdx > (SSBF_MNGR_NUMBER_OF_HUBS - 1U)) {
            hubIdx = SSBF_MNGR_NUMBER_OF_HUBS - 1U;
        }
        SSBAL::SSBCC::HubObject[hubIdx].startLoop(singleNotInfinite);
    }
    //lint +e778 +e438 +e715
  
    //lint -e778 the result '0' is OK!
    //lint -e438 SSBAL::SSBCC::HubObject[hubIdx].StopLoop()is not yet implemented
    void SSB_HUBC_stopMeasurementLoop_C(uint8_t hubIdx) {
        if (hubIdx > (SSBF_MNGR_NUMBER_OF_HUBS - 1U)) {
            hubIdx = SSBF_MNGR_NUMBER_OF_HUBS - 1U;
        }
        SSBAL::SSBCC::HubObject[hubIdx].stopLoop();
    }
    //lint +e778 +e438 +e715
   
    //lint -e778 the result '0' is OK!
    void SSB_HUBC_readDebugTest_C(uint8_t hubIdx, uint8_t *data) {
         if (hubIdx > (SSBF_MNGR_NUMBER_OF_HUBS - 1U)) {
            hubIdx = SSBF_MNGR_NUMBER_OF_HUBS - 1U;
        }
        SSBAL::SSBCC::HubObject[hubIdx].readDebugTest(data);
    }
    //lint +e778
#endif // From ifndef SSB_USE_CPP_INSTEAD_OF_C_API

}   // From: extern "C"

