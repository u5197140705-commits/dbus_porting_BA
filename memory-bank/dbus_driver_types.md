# DBus Driver Type Definitions (`BshDBus2AppLayer_types.h`)

This document contains the content of `common/prog/dbus/DBal/BshDBus2AppLayer_types.h`, which defines the public type definitions for the DBus Application Layer (DBAL) and IoConnectionHandling. It outlines the communication protocol used by symana.

```c
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

#ifndef BSH_DBUS2_APP_LAYER_TYPES_H__
#define BSH_DBUS2_APP_LAYER_TYPES_H__

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public type definitions for unit BshDBus2AppLayer and IoConnectionHandling.
 *
 *  \details  This header provides the public datatypes for DBal. It is the communication
 *            protocol which is used by symana. For detailed information, please
 *            read the md file which is included in the doc folder.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#ifdef DOXY_DEVELOPERS_DOC
/** Compiler switch to activate cross connection between microcontrollers.
 */ #define DBAL_CROSS_CONNECTION
#endif

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \enum   DBAL_CommState
 *
 * \brief   Value that represents the current communication status.
 *
 * \details For meaning of values see general DBal specification.
**/
enum DBAL_CommState
{
    DBAL_COMMSTATE_NOT_READY = 0,
    DBAL_COMMSTATE_READY
};

/** \enum   DBAL_MessageType
 *
 * \brief   Value that represents the DBal specific message type for communication.
 *
 * \details For meaning of values see general DBal specification.
**/
enum DBAL_MessageType
{
    DBAL_TYPE_CMD,
    DBAL_TYPE_CMD_ACK,
    DBAL_TYPE_QUERY,
    DBAL_TYPE_QUERY_ACK,
    DBAL_TYPE_EVENT,
    DBAL_TYPE_EVENT_ACK,
    DBAL_TYPE_UNKNOWN
};

/** \enum   DBAL_ConnectionMessageType
 *
 * \brief   Value that represents the action in the connection handling.
 *
 * \details For meaning of values see general DBal specification.
**/
enum DBAL_ConnectionMessageType
{
    DBAL_CON_ENABLE_REQUEST = 0,
    DBAL_CON_ENABLE_RESPONSE,
    DBAL_CON_DISABLE_REQUEST,
    DBAL_CON_DISABLE_RESPONSE,
    DBAL_CON_PING_REQUEST,
    DBAL_CON_PING_RESPONSE,
    DBAL_CON_TEMP_ENABLE_REQUEST,
    DBAL_CON_TEMP_ENABLE_RESPONSE,
    DBAL_CON_TEMP_DISABLE_REQUEST,
    DBAL_CON_TEMP_DISABLE_RESPONSE,
    DBAL_CON_MSG_TYPE_COUNT
};

typedef void (*DBAL_ioCommStateCb)(enum DBAL_CommState CommState);//!< Prototype of user callback to receive change of CommState

typedef void (*DBAL_ioMsgLostCb)(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);//!< Prototype of user callback to receive notification, that a message could not be delivered

/** \struct DBAL_Identifier
 *
 * \brief   Structure for the identification of a DBal message with a given ServiceId.
 *
 * \details For meaning of parameters see general DBal specification.
**/
struct DBAL_Identifier
{
    uint16_t                        CommandId;
    enum DBAL_MessageType           Type;
};

typedef void (*DBAL_Service)(const uint8_t* const Data, uint8_t DataLen);//!< Prototype of user callback to receive DBal message

/** \struct DBAL_ReceiveObject
 *
 * \brief   Structure for handling an incoming dbal message.
**/
struct DBAL_ReceiveObject
{
    struct DBAL_Identifier      Identifier;
    DBAL_Service                Service;//!< Callback, where user receives Payload of DBal frame
};

/** \struct DBAL_ObjectTableEntry
 *
 * \brief   Structure describing a receiving object table for all the DBal frames defined under a specific ServiceId.
 *
 * \details For meaning of parameter "ServiceId" see general DBal specification.
**/
struct DBAL_ObjectTableEntry
{
    const struct DBAL_ReceiveObject *ReceiveObject;//!< Pointer to table with callbacks for given CommandId and DBAL_MessageType
    const uint16_t                  ServiceId;
    const uint8_t                   ReceiveObjectCount;//!< Number of entries in Table behind pointer ReceiveObject
};

#ifdef DBAL_CROSS_CONNECTION
/** \enum DBALCR_ParticipantType
 *
 * \brief Describes the role of a participant in ecu to ecu communication.
 */
enum DBALCR_ParticipantType
{
    DBALCR_PART_TYPE_NONE,//!< No valid participant or not initialized.
    DBALCR_PART_TYPE_CLIENT,//!< Can send commands and queries + confirm events.
    DBALCR_PART_TYPE_SERVER//!< Can send events + confirm commands and queries.
};

typedef void (*DBALCR_Service)(uint8_t Index, const uint8_t* const Data, uint8_t DataLen);//!< Prototype of user callback to receive DBal message in ecu to ecu communication

/** \struct DBALCR_ReceiveObject
 *
 * \brief   Structure for handling an incoming dbal message in ecu to ecu communication.
**/
struct DBALCR_ReceiveObject
{
    struct DBAL_Identifier        Identifier;
    DBALCR_Service                Service;//!< Callback, where user receives Payload of DBal frame
};

/** \struct DBALCR_ObjectTableEntry
 *
 * \brief   Structure describing a receiving object table for all the DBal frames defined under a specific ServiceId in ecu to ecu communication.
 *
 * \details For meaning of parameter "ServiceId" see general DBal specification.
**/
struct DBALCR_ObjectTableEntry
{
    const struct DBALCR_ReceiveObject *ReceiveObject;//!< Pointer to table with callbacks for given CommandId and DBAL_MessageType
    const uint16_t                    ServiceId;
    const uint8_t                     ReceiveObjectCount;//!< Number of entries in Table behind pointer ReceiveObject
};
#endif /*DBAL_CROSS_CONNECTION*/

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif