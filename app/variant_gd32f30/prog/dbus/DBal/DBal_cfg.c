/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          DBus Application Layer
 *  COMP_ABBREV      DBAL
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    User configurable part of DBus application layer
 *
 *  \note     This file contains user specific DBal messages defined by project.
 *
 */ 


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "BshDBus2AppLayer.h"
#include "BshDBus2AppLayerCross.h"
#include "BshDBus2AppLayer_debug.h"
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
/*lint -save -e552  : external variable 'DBAL_ObjectTable/DBALCR_ObjectTable' not accessed
 * 'DBAL_ObjectTable/DBALCR_ObjectTable' is accessed in BshDBus2AppLayer.c
 */
const struct DBAL_ObjectTableEntry DBAL_ObjectTable[] = {
    //DBAL_ReceiveObject,                      Module SERVICE_ID,      number of elements in ReceiveObject
    {(const void *) NULL,                      0x00u,                  0x01u                     }  // dummy data to make compiler happy
};

// calculation of receive objects count in DBAL_ObjectTable
const uint8_t DBAL_ObjectTableSize = (uint8_t)(sizeof(DBAL_ObjectTable) / sizeof(struct DBAL_ObjectTableEntry));

#ifdef DBAL_CROSS_CONNECTION
// List of Dbus2 addresses of communication partners in ecu to ecu communication.
// Fill with valid values >= 0x10, in the correct order.
// (If the partner in cross connection zero is to have address 0x2F, fill in this value at index zero in the array below.)
uint8_t DBALCR_ComPartners[DBAL_CROSS_CONNECT_COUNT] = {0x23};

// List of definitions, whether this controller here is server or client toward communication partners.
// Enter info at same index as in DBALCR_ComPartners.
enum DBALCR_ParticipantType DBALCR_RolesTowardPartners[DBAL_CROSS_CONNECT_COUNT] = {DBALCR_PART_TYPE_CLIENT};

// Time interval for pinging in direction client->server
uint16_t DBALCR_PingTimeMs = 0; //0 -> Disabled

const struct DBALCR_ObjectTableEntry DBALCR_ObjectTable[] = {
        //DBALCR_ReceiveObject,                    Module SERVICE_ID,      number of elements in ReceiveObject
        {(const void *) NULL,                      0x00u,                  0x01u                     }  // dummy data to make compiler happy
};
const uint8_t DBALCR_ObjectTableSize = (uint8_t)(sizeof(DBALCR_ObjectTable) / sizeof(struct DBALCR_ObjectTableEntry));
#endif
/*lint -restore -e552 */
/******************************************************************************/
/* FUNCTION DEFINITIONS                                                       */
/******************************************************************************/

void DBAL_ntfUnknownDBalFrameReceived(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    (void)SenderNodeAddress;
    (void)DBalType;
    (void)ServiceId;
    (void)CommandId;
    (void)*Bytes;/*lint !e920 Pointer not needed, until implemented by user.*/
    (void)DataLen;
}

void DBAL_ntfUnexpectedDBalFrameReceived(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    (void)SenderNodeAddress;
    (void)DBalType;
    (void)ServiceId;
    (void)CommandId;
    (void)*Bytes;/*lint !e920 Pointer not needed, until implemented by user.*/
    (void)DataLen;
}

void DBAL_ntfCorruptReqRespDbus2FrameReceived(const uint8_t* const Bytes, uint8_t DataLen)
{
    (void)*Bytes;/*lint !e920 Pointer not needed, until implemented by user.*/
    (void)DataLen;
}

void DBAL_ntfCorruptConDbus2FrameReceived(const uint8_t* const Bytes, uint8_t DataLen)
{
    (void)*Bytes;/*lint !e920 Pointer not needed, until implemented by user.*/
    (void)DataLen;
}

uint8_t DBAL_getTargetAddress(void)
{
    return 0x00;
}

void DBAL_storeTargetAddress(uint8_t TargetAddress)
{
    (void)TargetAddress;
}
