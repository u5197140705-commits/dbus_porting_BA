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

#ifndef BSH_DBUS2_APP_LAYER_CROSS_H__
#define BSH_DBUS2_APP_LAYER_CROSS_H__

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public definitions and declarations for units BshDBus2AppLayer and IoConnectionHandling.
 *
 *  \details  This header provides the functions public for DBal. It is the communication
 *            protocol which is used by symana. The interface in this file is for ecu
 *            to ecu communication, which is also named cross communication. For
 *            detailed information, please read the md file which is included in the
 *            doc folder.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "BshDBus2AppLayer_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#ifdef DOXY_DEVELOPERS_DOC
/** Compiler switch to activate cross connection between microcontrollers.
 */ #define DBAL_CROSS_CONNECTION

/** Compiler switch to define number of cross connections between microcontrollers.
 */ #define DBAL_CROSS_CONNECT_COUNT
#endif

#ifdef DBAL_CROSS_CONNECTION
extern const struct DBALCR_ObjectTableEntry DBALCR_ObjectTable[];//!< Table is used for managing incoming messages to related callback.
extern const uint8_t DBALCR_ObjectTableSize;//!< Length of DBALCR_ObjectTable.
extern uint8_t DBALCR_ComPartners[DBAL_CROSS_CONNECT_COUNT];//!< Dbus2 addresses of cross communication partners for initialization.
extern enum DBALCR_ParticipantType DBALCR_RolesTowardPartners[DBAL_CROSS_CONNECT_COUNT];//!< Are we client or server for comm partner? For initialization.
extern uint16_t DBALCR_PingTimeMs;//!< If there is no communication for a certain time and the communication is still enabled, a ping is sent to check if the communication partner is still available.
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief  Initializes the dbus application layer for all cross connections in one go.
 *
 * \return  Returns true, if it was possible to initialize all cross connections.
 *          Only possible, if DBALCR_ComPartners and DBALCR_RolesTowardPartners are filled
 *          with DBAL_CROSS_CONNECT_COUNT correct values.
 *
 * \note    Initialization will be performed only once during runtime. Later, you just get a positive return value.
 * \note    Minimum ping time interval is 5 seconds. If you choose a smaller value of DBALCR_PingTimeMs,
 *          ping mechanism will not be active.
 * \note    Pinging can only be done by client to see, that the server is still available.
*/
bool DBALCR_init(void);

/** \brief       Enables the dbal communication for a given cross connection.
 *
 * \details      Internally a sequence is called to exchange version number and DBal protocol version.
 *
 * \param Index  Index of the cross connection. (Same as Index of communication partner address in DBALCR_ComPartners.)
 *
 * \return       Returns true, if it is possible to enable the communication.
 *
 * \note         Only possible, if protocol version is compatible on both sides.
 * \note         Only possible, if communication is not already enabled.
 * \note         Only possible, if communication is not already being enabled,
 *               or the timeout for all attempts to send the connection frame has passed.
*/
bool DBALCR_enable(uint8_t Index);

/** \brief      Disables the dbal communication for a given cross connection.
 *
 * \details     Communication partner is notified, that communication will be disabled.
 *
 * \param Index Index of the cross connection. (Same as Index of communication partner address in DBALCR_ComPartners.)
 *
 * \return      Returns true, if it is possible to disable the communication, otherwise false.
 *
 * \note        Only possible, if not already (being) disabled.
 * \note        The user application must NOT use the silent mode in order to achieve similar effects.
*/
bool DBALCR_disable(uint8_t Index);

/** \brief      Returns the current communication status for a given cross connection.
 *
 * \param Index Index of the cross connection. (Same as Index of communication partner address in DBALCR_ComPartners.)
 *
 * \return      Current state of communication.
*/
enum DBAL_CommState DBALCR_getCommState(uint8_t Index);

/** \brief      For a given cross connection: Registers a callback to be notified,
 *              whenever the communication status changes.
 *
 * \param Index Index of the cross connection. (Same as Index of communication partner address in DBALCR_ComPartners.)
 * \param Cb    Callback which is called, whenever the communication status changes.
 *
 * \return      Returns true, if it worked to register the callback, otherwise false.
*/
bool DBALCR_registerCommState(uint8_t Index, DBAL_ioCommStateCb Cb);

/** \brief      Registers a callback to be notified, whenever a message could not be delivered.
 *
 * \detals      Logical Response was not received.
 *
 * \param Index Index of the cross connection. (Same as Index of communication partner address in DBALCR_ComPartners.)
 * \param Cb    Callback, which is called, whenever a message could not be delivered,
 *
 * \return      Returns true, if it worked to register the callback, otherwise false.
 */
bool DBALCR_registerLostMsg(uint8_t Index, DBAL_ioMsgLostCb Cb);

/** \brief          For a given cross connection:
 *
 * \details         Sends a DBal Command to the communication partner, which has been set by calling DBALCR_init.
 *                  The design requires, that one request-response pair is completed,
 *                  before continuing with the next one. Therefore, wait for the answer,
 *                  before sending the next Command/Query to the same communication partner.
 *                  Only available for the direction client->server.
 *
 * \param Index     Index of the cross connection.
 *                  (Same as Index of communication partner address in DBALCR_ComPartners.)
 * \param ServiceId Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId Command identifier for dbal communication. Each IO function block can support multiple commands.
 * \param Bytes     Pointer to payload.
 * \param DataLen   Length of payload.
 *
 * \return          True, if the sending of the Command was successfully triggered, otherwise false.
*/
bool DBALCR_sendCmd(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);

/** \brief          For a given cross connection:
 *
 * \details         Sends a DBal Query to the communication partner, which has been set by calling DBALCR_init.
 *                  The design requires, that one request-response pair is completed,
 *                  before continuing with the next one. Therefore, wait for the answer,
 *                  before sending the next Command/Query to the same communication partner.
 *                  Only available for the direction client->server.
 *
 * \param Index     Index of the cross connection.
 *                  (Same as Index of communication partner address in DBALCR_ComPartners.)
 * \param ServiceId Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId Command identifier for dbal communication. Each IO function block can support multiple commands.
 * \param Bytes     Pointer to payload.
 * \param DataLen   Length of payload.
 *
 * \return          True, if the sending of the Query was successfully triggered, otherwise false.
*/
bool DBALCR_sendQuery(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const  Bytes, uint8_t DataLen);

/** \brief          For a given cross connection:
 *
 * \details         Sends a DBal Command Ack to the communication partner, which has been set by calling DBALCR_init.
 *                  The design requires, that one request-response pair is completed,
 *                  before continuing with the next one. Therefore, send the answer as quickly as possible.
 *                  Only available for the direction server->client.
 *
 * \param Index     Index of the cross connection.
 *                  (Same as Index of communication partner address in DBALCR_ComPartners.)
 * \param ServiceId Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId Command identifier for dbal communication. Each IO function block can support multiple commands.
 * \param Bytes     Pointer to payload.
 * \param DataLen   Length of payload.
 *
 * \return          True, if the sending of the Command Ack was successfully triggered, otherwise false.
*/
bool DBALCR_sendCmdResponse(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);

/** \brief          For a given cross connection:
 *
 * \details         Sends a DBal Query Ack to the communication partner, which has been set by calling DBALCR_init.
 *                  The design requires, that one request-response pair is completed,
 *                  before continuing with the next one. Therefore, send the answer as quickly as possible.
 *                  Only available for the direction server->client.
 *
 * \param Index     Index of the cross connection.
 *                  (Same as Index of communication partner address in DBALCR_ComPartners.)
 * \param ServiceId Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId Command identifier for dbal communication. Each IO function block can support multiple commands.
 * \param Bytes     Pointer to payload.
 * \param DataLen   Length of payload.
 *
 * \return          True, if the sending of the Query Ack was successfully triggered, otherwise false.
*/
bool DBALCR_sendQueryResponse(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const  Bytes, uint8_t DataLen);

/** \brief          For a given cross connection:
 *
 * \details         Sends a DBal Event to the communication partner, which has been set by calling DBALCR_init.
 *                  Only available for the direction server->client.
 *
 * \param Index     Index of the cross connection.
 *                  (Same as Index of communication partner address in DBALCR_ComPartners.)
 * \param ServiceId Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId Command identifier for dbal communication. Each IO function block can support multiple commands.
 * \param Bytes     Pointer to payload.
 * \param DataLen   Length of payload.
 *
 * \return          True, if the sending of the Event was successfully triggered, otherwise false.
*/
bool DBALCR_sendEvent(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);
#endif /*DBAL_CROSS_CONNECTION*/

#ifdef __cplusplus
}
#endif

#endif /*BSH_DBUS2_APP_LAYER_CROSS_H_*/
