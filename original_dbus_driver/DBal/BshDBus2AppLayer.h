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
 
#ifndef BSH_DBUS2_APP_LAYER_H__
#define BSH_DBUS2_APP_LAYER_H__

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public definitions and declarations for units BshDBus2AppLayer and IoConnectionHandling.
 *
 *  \details  This header provides the public functions for DBal. It is the communication
 *            protocol which is used by symana. The interface in this file is mainly
 *            for the standard communication uc <-> SMM. The exceptions are the
 *            connection to the Dbus2 interface and the error notifications for users.
 *            For detailed information, please read the md file which is included in
 *            the doc folder.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "bustypes.h"
#include "BshDBus2AppLayer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
extern const TbusReceiveObject DBAL_DBUS_Handler_RxObject[];//!< Rx Object for DBal subsystem Entry in balXS.c
extern TbusTransmitObject DBAL_DBUS_Handler_TxObject[];//!< Tx Object for DBal subsystem Entry in balXS.c
extern const uint8_t DBAL_DBUS_Handler_NumberOfTxObjects;//!< For Bal to know, how many handlers we have for transmitting messages
extern uint8_t DBAL_DBUS_Handler_TxFlags[];//!< Transmission flags for Bal
extern uint8_t DBAL_DBUS_CommunicationPartner;//!< Address of communication partner (SMM)
extern const struct DBAL_ObjectTableEntry DBAL_ObjectTable[];//!<Table is used for managing incoming messages to related callback.
extern const uint8_t DBAL_ObjectTableSize;//!< Length of DBAL_ObjectTable
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief Initializes the dbus application layer.
 *
 * \param  TargetAddress DBus2 address of communication partner. MUST be a valid address in range 0x10-0xFF.
 *
 * \note   Initialization will be performed only once during runtine.
 * \note   Alternatively, if TargetAddress is unknown at first boot, initialization is possible with address 0x00.
 * \note   In this case, each call of DBAL_enable will return false, until communication partner has sent a request
 *         to enable the connection with its' correct address.
 * \note   This correct address value will also redirected to DBAL_storeTargetAddress (DBal_cfg.c)
 *         for the application to save persistently.
 * \note   In such a case, the application also implements DBAL_getTargetAddress (DBal_cfg.c)
 *         to read the persistently saved address value at each init with address 0x00.
*/
void DBAL_init(uint8_t TargetAddress);

/** \brief       Enables the dbal communication.
 *
 * \details      Internally a sequence is called to exchange version number and DBal protocol version.
 * *
 * \return       Returns true, if it is possible to enable the communication.
 *
 * \note         Only possible, if protocol version is compatible on both sides.
 * \note         Only possible, if communication is not already enabled.
 * \note         Only possible, if communication is not already being enabled,
 *               or the timeout for all attempts to send the connection frame has passed.
 * \note         Only possible, if DBal is initialized with a valid communication partner address in range 0x10-0xFF,
 *               see description of DBAL_init.
*/
bool DBAL_enable(void);

/** \brief  Disables the dbal communication.
 *
 * \details Communication partner is notified, that communication will be disabled.
 *
 * \return  Returns true, if it is possible to disable the communication, otherwise false.
 *
 * \note    Only possible, if not already (being) disabled.
 * \note    The user application must NOT use the silent mode in order to achieve similar effects.
*/
bool DBAL_disable(void);

/** \brief Returns the current communication status.
 *
 * \return Current state of communication.\n
*/
enum DBAL_CommState DBAL_getCommState(void);

/** \brief   Registers a callback to be notified, whenever the communication status changes.
 *
 * \param Cb Callback which is called, whenever the communication status changes.
 *
 * \return   Returns true, if it worked to register the callback, otherwise false.
*/
bool DBAL_registerCommState(DBAL_ioCommStateCb Cb);

/** \brief   Registers a callback to be notified, whenever a message could not be delivered.
 *
 * \details  Logical Response was not received.
 *
 * \param Cb Callback, which is called, whenever a message could not be delivered,
 *
 * \return   Returns true, if it worked to register the callback, otherwise false.
 */
bool DBAL_registerLostMsg(DBAL_ioMsgLostCb Cb);

/** \brief  To be called by application, whenever a Dbus2 wake up break had been detected.
 *
 * \details Will look for connection enable requests by partners, with which connection
 *          is currently not enabled. If no such enable request is received within
 *          DBAL_DBUS_RECOVERY_TIME_MS, user is informed about state NOT_READY
 *          for all communication partners, with which no connection was enabled.
 */
void DBAL_breakSignalDetected(void);

/** \brief          Sends a DBal Command Ack to the communication partner, which has been set by calling DBAL_init.
 *
 * \details         The design requires, that one request-response pair is completed, before continuing
 *                  with the next one.
 * \details         Therefore, send the answer as quickly as possible.
 *
 * \param ServiceId Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId Command identifier for dbal communication. Each IO function block can support multiple commands.
 * \param Bytes     Pointer to payload.
 * \param DataLen   Length of payload.
 *
 * \return          True, if the sending of the Command Ack was successfully triggered, otherwise false.
*/
bool DBAL_sendCmdResponse(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);

/** \brief           Sends a DBal Query Ack to the communication partner, which has been set by calling DBAL_init.
 *
 * \details          The design requires, that one request-response pair is completed, before continuing
 *                   with the next one.
 * \details          Therefore, send the answer as quickly as possible.
 *
 * \param ServiceId  Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId  Command identifier for dbal communication. Each IO function block can support multiple commands.
 * \param Bytes      Pointer to payload.
 * \param DataLen    Length of payload.
 *
 * \return           True, if the sending of the Query Ack was successfully triggered, otherwise false.
*/
bool DBAL_sendQueryResponse(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const  Bytes, uint8_t DataLen);

/** \brief           Sends a DBal Event to the communication partner, which has been set by calling DBAL_init.
 *
 * \param ServiceId  Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId  Command identifier for dbal communication. Each IO function block can support multiple commands.
 * \param Bytes      Pointer to payload.
 * \param DataLen    Length of payload.
 *
 * \return           True, if the sending of the Event was successfully triggered, otherwise false.\n
*/
bool DBAL_sendEvent(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);

/** \brief                  Implemented in DBal_cfg.c.
 *
 * \details                 Is called, whenever a dbal frame has been received, that is not defined in DBAL_ObjectTable
 *                          or DBALCR_ObjectTable, or if the DBalType of the frame has no valid value.
 * \details                 MUST NOT EVER be called in regular operation of a device.
 * \details                 Before releasing your software, make sure that this function is NEVER called
 *                          while testing regular device functionality.
 *
 * \param SenderNodeAddress Dbus2 Node Address of sender; Main and Subnode.
 * \param DBalType          DBAL_TYPE_CMD, DBAL_TYPE_CMD_ACK, DBAL_TYPE_QUERY, DBAL_TYPE_QUERY_ACK,
 *                          DBAL_TYPE_EVENT, DBAL_TYPE_EVENT_ACK, DBAL_TYPE_UNKNOWN
 * \param ServiceId         Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId         Command identifier for dbal communication. Each IO function block
 *                          can support multiple commands.
 * \param Bytes             Pointer to payload.
 * \param DataLen           Length of payload.
 */
void DBAL_ntfUnknownDBalFrameReceived(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);

/** \brief                   Implemented in DBal_cfg.c.
 *
 *  \details                 Is called, whenever a response to a dbal command, query or event has been received,
 *                           but has NOT been expected, or if the expected response to an event contains payload.\n
 *                           The error can mostly occur:\n
 *                           - 1: if a command, query or event (or the response to it) has been delivered twice
 *                           on dbus2 with different sequence Ids.\n (Can happen, if the message to sent is updated by
 *                           an Command/Query/Event or an Ack to a received Command/Query/Event,
 *                           while already being sent.)\n
 *                           - 2: if you are testing your software with a faulty/buggy communication partner
 *                           (script or controller) and the command/query/event has NOT been sent AT ALL (and also,
 *                           if the confirmation to an event contains payload).\n
 *                           This callback might ALWAYS be called for reason "1" and
 *                           MUST NOT EVER be called for reason "2" in regular operation.\n
 *
 * \param SenderNodeAddress  Dbus2 Node Address of sender; Main and Subnode.
 * \param DBalType           DBAL_TYPE_CMD, DBAL_TYPE_CMD_ACK, DBAL_TYPE_QUERY, DBAL_TYPE_QUERY_ACK,
 *                           DBAL_TYPE_EVENT, DBAL_TYPE_EVENT_ACK, DBAL_TYPE_UNKNOWN
 * \param ServiceId          Service identifier for dbal communication. Each IO function block has an own service id.
 * \param CommandId          Command identifier for dbal communication. Each IO function block
 *                           can support multiple commands.
 * \param Bytes              Pointer to payload.
 * \param DataLen            Length of payload.
 */
void DBAL_ntfUnexpectedDBalFrameReceived(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);

/**\brief         Implemented in DBal_cfg.c.
 *
 * \details       It is called, whenever:\n
 *                - a received dbus2 "Request" (MsgId 0xB981) or "Response" (MsgId 0xB982) frame is corrupt in the way,
 *                that the overall length of the received data and the declared payload lengths in the contained dbal frames
 *                contradict each other (, so it does not exactly add up to the overall length of the dbus2 frame).\n
 *                - the dbus2 frame contains the generic frame header and the sequence ID, but no full dbal frame.\n
 *                (Frames, that are even shorter than that or have wrong sequence Ids are completely ignored anyway.)\n
 *                - a message has been received from an invalid or unknown communication partner.\n
 *                - the generic header contains an invalid protocol type.\n
 *                MUST NOT EVER be called in regular operation of a device.\n
 *                The error can mostly occur, if you are testing your software by a faulty script.\n
 *
 * \param Bytes   Pointer to payload of the dbus2 frame.
 * \param DataLen Length of the named payload.
 */
void DBAL_ntfCorruptReqRespDbus2FrameReceived(const uint8_t* const Bytes, uint8_t DataLen);

/**\brief         Implemented in DBal_cfg.c.
 *
 * \details       It is called, whenever:\n
 *                - a received connection frame (MsgId 0xB980) has a wrong length or Dbal Protocol Version.\n
 *                - a received connection frame (MsgId 0xB980) contains an invalid protocol type.\n
 *                - a received connection frame (MsgId 0xB980) is from an invalid or unknown communication partner.\n
 *                MUST NOT EVER be called in regular operation of a device.\n
 *                The error can mostly occur, if you are testing your software by a faulty script.\n
 *
 * \param Bytes   Pointer to payload of the dbus2 frame.
 * \param DataLen Length of the named payload.
 */
void DBAL_ntfCorruptConDbus2FrameReceived(const uint8_t* const Bytes, uint8_t DataLen);

/**\brief   Implemented in DBal_cfg.c.
 *
 * \details If DBAL_init is called with TargetAddress==0x00, this function is called to check,
 *          whether a different value has been saved during a previous boot.
 * \details To be implemented by application.
 *
 * \return  Address of communication partner.
 */
uint8_t DBAL_getTargetAddress(void);

/**\brief               Implemented in DBal_cfg.c.
 *
 * \details             If DBAL_init was called with TargetAddress==0x00 at first boot and
 *                      the communication partner requests connection with its' proper address,
 *                      this address will be saved persistently in this function. To be implemented by application.
 *
 * \param TargetAddress Address of communication partner.
 */
void DBAL_storeTargetAddress(uint8_t TargetAddress);

#ifdef __cplusplus
}
#endif

#endif
