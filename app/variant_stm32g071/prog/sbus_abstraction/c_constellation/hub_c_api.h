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
 *  COMP_ABBREV      SSB_HUBC
 ******************************************************************************/

#ifndef HUB_C_API_H
#define HUB_C_API_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     hub_c_api.h
 *
 *  \ingroup  sbus_abstraction/constellation
 *
 *  \brief    Interface header of the C-API for the C++ Hub class (with the
 *            processing for the Hubs and the functionality for the
 *            Hub-connected Clients and Devices)
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ssbf_common_ctypes.h"

/******************************************************************************/
/* C-PREPROCESSOR DEFINITIONS                                                 */
/******************************************************************************/

/******************************************************************************/
/* FUNCTION PROTOTYPES                                                        */
/******************************************************************************/
extern void SSB_HUBC_setupHubs(uint8_t i2cAddrOffsets,
                uint8_t configurationToBeLoaded);
extern void SSB_HUBC_notifyCallbackToCapi(SSBF_CallbackFct_t callBackToApiFctPtr);
extern void SSB_HUBC_writeHubRamRegisters_C(
                uint8_t hubIdx, uint8_t pageSelectOption, uint8_t registerAddr,
                uint8_t numberOfWriteBytes, uint8_t *writeBytesPtr);

extern void SSB_HUBC_readHubRamRegisters_C(
                uint8_t hubIdx, uint8_t pageSelectOption, uint8_t registerAddr,
                uint8_t numberOfReadBytes);
extern void SSB_HUBC_writeClientRamRegisters_C(
                uint8_t hubIdx, uint8_t clientIdx, uint8_t pageSelectOption, 
                uint8_t registerAddr,
                uint8_t numberOfWriteBytes, uint8_t *writeBytesPtr);
extern void SSB_HUBC_readClientRamRegisters_C(
                uint8_t hubIdx, uint8_t clientIdx, uint8_t pageSelectOption,
                uint8_t registerAddr,
                uint8_t numberOfReadBytes);
extern void SSB_HUBC_transferDeviceFrame_C(
                uint8_t hubIdx, uint8_t clientIdx, uint8_t deviceIdx, 
                uint8_t numberOfSendBytes, uint8_t numberOfReceiveBytes,
                uint8_t *sendBytesPtr);
extern void SSB_HUBC_startMeasurementLoop_C(uint8_t hubIdx,
                uint8_t singleNotInfinite);
extern void SSB_HUBC_stopMeasurementLoop_C(uint8_t hubIdx);
extern void SSB_HUBC_readDebugTest_C(uint8_t hubIdx, uint8_t *data);

#endif // From: #ifndef HUB_C_API_H

