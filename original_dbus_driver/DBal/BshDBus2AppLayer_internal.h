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
 
#ifndef BSH_DBUS2_APP_LAYER_INTERNAL_H__
#define BSH_DBUS2_APP_LAYER_INTERNAL_H__

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Private definitions and declarations for units BshDBus2AppLayer and IoConnectionHandling.
 *
 *  \details  This header provides the functions for DBal. It is the communication
 *            protocol which is used by symana. For detailed information, please
 *            read the general DBal specification.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "stdint.h"
#include "bustypes.h"
#include "timer/system_timer.h"
#include "BshDBus2AppLayer_types.h"
#include "dbus/DBal/DBal_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#ifdef DOXY_DEVELOPERS_DOC
/** Compiler switch to activate cross connection between microcontrollers.
 */ #define DBAL_CROSS_CONNECTION

/** Compiler switch to activate logical description of communication partner and the connection to it.
 *  It is set by the generic modules, which need it. NOT TO BE DEFINED by application.
 */ #define DBAL_USE_INSTANCE
#endif

#define DBAL_PROTOCOL_VERSION               4U //!< Has to be identical for communication partner.

#define DBAL_UNIQUE_HEADER_OFFSET           0x02U //!< Sender + ProtocolType (0x03) in generic frame; See DBal spec.
#define DBAL_HEADER_OFFSET                  0x06U //!< Type + Size + ServiceId_High + ServiceId_Low + CommandId_High + CommandId_Low; See DBal spec.

#define DBAL_MESSAGE_APP_LAYER_CONNECTION   0xB980U //!< Dbus2 MsgId: Value defined in general DBal specification.
#define DBAL_MESSAGE_APP_LAYER_REQUEST      0xB981U //!< Dbus2 MsgId: Value defined in general DBal specification.
#define DBAL_MESSAGE_APP_LAYER_RESPONSE     0xB982U //!< Dbus2 MsgId: Value defined in general DBal specification.

#define DBAL_MSG_INDEX_APP_LAYER_CONNECTION 0U //!< Bal TxIndex used to transmit connection frames to SMM
#define DBAL_MSG_INDEX_APP_LAYER_REQUEST    1U //!< Bal TxIndex used to transmit DBal request Dbus2 frames to SMM
#define DBAL_MSG_INDEX_APP_LAYER_RESPONSE   2U //!< Bal TxIndex used to transmit DBal response Dbus2 frames to SMM
#define DBAL_MSG_INDEX_CROSS_OFFSET         3U //!< Number of BAL transmit entries for DBal communication with SMM
#define DBAL_MSG_INDEX_APP_LAYER_CROSS_CON  3U //!< Bal TxIndex used to transmit connection frames to other microcontrollers
#define DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ  4U //!< Bal TxIndex used to transmit DBal request Dbus2 frames to other microcontrollers
#define DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP 5U //!< Bal TxIndex used to transmit DBal response Dbus2 frames to other microcontrollers
#ifdef DBAL_CROSS_CONNECTION
#define DBAL_MSG_INDEX_COUNT                6U //!< Overall number of entries in transmit table of DBal
#else/*no DBAL_CROSS_CONNECTION*/
#define DBAL_MSG_INDEX_COUNT                3U //!< Overall number of entries in transmit table of DBal
#endif/*DBAL_CROSS_CONNECTION*/

#define DBAL_APPLIANCE_LAYER                0x03U //!< Value o DBal protocol type inside generic frame.

#define DBAL_CON_MSG_SENDER                 0U //!< Generic frame sender at start of DBal connection frame. See DBal spec.
#define DBAL_CON_MSG_PROTOCOL_TYPE          1U //!< Generic frame protocol type at start of DBal connection frame. See DBal spec.
#define DBAL_CON_MSG_TYPE                   2U //!< Connection frame: See DBal spec.
#define DBAL_CON_MSG_PROTOCOL_VERSION       3U //!< Connection frame: See DBal spec.
#define DBAL_CON_MSG_LEN                    4U //!< Connection frame: See DBal spec.

#define DBAL_MSG_SENDER                     0U //!< Generic frame sender at start of DBal request/response Dbus2 frame. See DBal spec.
#define DBAL_MSG_PROTOCOL_TYPE              1U //!< Generic frame protocol type at start of DBal request/response Dbus2 frame. See DBal spec.
#define DBAL_MSG_SEQID                      2U //!< SeqenceId: Has to increase with each request/response Dbus2 frame in DBal. Otherwise the entire Dbus2 frame is ignored.
#define DBAL_MSG_FRAME_OFFSET               3U //!< Offset, at which first DBal frame starts in Dbus2 frame. See DBal spec.

#define DBAL_FRAME_DBAL_TYPE                0U //!< DBal frame: See DBal spec.
#define DBAL_FRAME_PAYLOADLEN               1U //!< DBal frame: See DBal spec.
#define DBAL_FRAME_SERVICE_ID_HI            2U //!< DBal frame: See DBal spec.
#define DBAL_FRAME_SERVICE_ID_LO            3U //!< DBal frame: See DBal spec.
#define DBAL_FRAME_COMMAND_ID_HI            4U //!< DBal frame: See DBal spec.
#define DBAL_FRAME_COMMAND_ID_LO            5U //!< DBal frame: See DBal spec.
#define DBAL_FRAME_DATA_OFFSET              DBAL_HEADER_OFFSET //!< DBal frame: See DBal spec.

#define DBAL_OWN_NODE_ADDRESS               (((uint8_t)__DBUS_NODE_ADDRESS << NIBBLE_SIZE) | (uint8_t)DBAL_DBUS_HANDLER_SUBSYSTEM) //!< Main Node plus DBal Subsystem
#define DBAL_PRELIMINARY_PARTNER_ADDR       0x00U //!< Init DBal with this value, if communication partner address is unknown at that time. See DBal spec.

#define DBAL_MIN_PING_TIME_MS               5000U //!< Ping: See DBal spec.
#define DBAL_PING_REPEAT_MS                 1000U //!< Ping: See DBal spec.
#define DBAL_PING_MAX_REPEAT                2U //!< Ping: See DBal spec.

#define DBAL_DISABLE_TIME_MS                1000U //!< Disable Request: Maximum time, after which it is expected to be responded. See DBal spec.

#ifndef DBAL_IO_MAX_CALLBACK_BUFFER_SIZE
#define DBAL_IO_MAX_CALLBACK_BUFFER_SIZE    1U //!< Maximum number of connection status callbacks per communication partner. Can be re-adjusted in build process.
#endif

#ifndef DBAL_BUFFER_SIZE
#define DBAL_BUFFER_SIZE                    ((uint8_t)DLL_TRANSMIT_BUFFER_DATA_LENGTH - BUS_MESSAGE_MIN_FRAME_LENGTH)//!< Size of DBal tx buffer. Calculated from size of Dbus2 tx buffer.
#endif

#define DBAL_LAST_MSG2REPEAT                ((uint8_t)DBAL_MAX_MSGS2REPEAT - 1U) //!< Highest possible index in queue with messages to repeat.
#define DBAL_LAST_QUEUE_ENTRY               ((uint8_t)DBAL_CROSS_CONNECT_COUNT - 1U) //!< Highest possible index in queue for cross connections.

/*Critical code sections bit masks*/
#define DBAL_CODE_SEC_REP_TIMER             ((uint8_t)0x01U) //!< Repetition timer section bit
#define DBAL_CODE_SEC_REQ_RESP_POST         ((uint8_t)0x02U) //!< DBal request/response sending post section bit
#ifdef DBAL_CROSS_CONNECTION
#define DBAL_CODE_SEC_CR_REQ_RESP_POST      ((uint8_t)0x04U) //!< DBal request/response sending post section (cross connection) bit
#endif
#define DBAL_CODE_SEC_TASK                  ((uint8_t)0x08U) // !< Task section bit
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \enum   DBAL_Dbus2FrameType
 *
 * \brief   Value that represents the type of Dbus2 frame, in which a DBal message is to be (preferably) sent.
 *
 * \details See DBal specification for more details.
**/
enum DBAL_Dbus2FrameType
{
    DBAL_DBUS2_FRAME_TYPE_CON = 0, //!< Connection (0xB980)
    DBAL_DBUS2_FRAME_TYPE_REQ, //!< Request (0xB981), used to send DBal Command and Query
    DBAL_DBUS2_FRAME_TYPE_RESP, //!< Response (0xB982), used to send DBal Event, Event Ack, Command Ack and Query Ack
    DBAL_DBUS2_FRAME_TYPE_UNKNOWN //!< Invalid value
};

/** \enum   DBAL_Msgs2RepeatStatus
 *
 * \brief   Status, whether we have any message in our queue, which need to be repeated, if no reponse.
 *
 * \details Message Types: DBAL_TYPE_CMD, DBAL_TYPE_QUERY, DBAL_TYPE_EVENT
**/
enum DBAL_Msgs2RepeatStatus
{
    DBAL_MSG2REPEAT_NONE = 0, //!< We have no undelivered messages
    DBAL_MSG2REPEAT_EXIST //!< We have undelivered messages
};

/** \enum DBAL_ConnectionState
 *
 * \brief Value that represents the status of the internal connection state machine in DBal
**/
enum DBAL_ConnectionState
{
    DBAL_CONNECTIONSTATE_DISCONNECTED = 0, //!< Fully disconnected
    DBAL_CONNECTIONSTATE_CONNECTING, //!< One of the communication partners has requested a connection. The other partner has not confirmed it.
    DBAL_CONNECTIONSTATE_CONNECTED, //!< Fully connected
    DBAL_CONNECTIONSTATE_DISCONNECTING //!< One of the communication partners has requested to disable the connection. The other partner has not confirmed.
};

/** \enum DBAL_ConnectionSmEvent
 *
 * \brief Value that represents the actions inside the internal connection state machine in DBal
**/
enum DBAL_ConnectionSmEvent
{
    DBAL_CON_SM_EVENT_ENABLE = 0, //!< User has asked to enable the connection.
    DBAL_CON_SM_EVENT_ENABLE_REQUEST, //!< Communication partner has asked to enable the connection.
    DBAL_CON_SM_EVENT_DISABLE, //!< User has asked to disable the connection.
    DBAL_CON_SM_EVENT_DISABLE_SILENT, //!< An error has occurred, when trying to reach communication partner. -> Connection is disabled, without telling partner.
    DBAL_CON_SM_EVENT_DISABLE_REQUEST, //!< Communication partner has asked to disable the connection.
    DBAL_CON_SM_EVENT_ACCEPT, //!< Our request has been accepted by communication partner.
    DBAL_CON_SM_EVENT_REJECT, //!< Our request has been rejected by communication partner.
    DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST, //!< Enabling of temporary connection has been requested.
    DBAL_CON_SM_EVENT_TEMP_DISABLE_REQUEST //!< Disabling of temporary connection has been requested.
};

#ifdef DBAL_USE_INSTANCE
/** \struct DBAL_Msg2Repeat
 *
 * \brief   Structure describing DBal frames to be repeated, because they were not responded.
 *
 * \details For internal usage only. NOT TO BE USED BY USER!
**/
struct DBAL_Msg2Repeat
{
    uint16_t ServiceId; //!< See DBal spec
    uint16_t CommandId; //!< See DBal spec
    enum DBAL_MessageType DbalType; //!< See DBal spec
    uint8_t Datalen; //!< Length of data for DBal frame
    bool IsSlotOccupied; //!< Info, whether the given entry for repeating a frame is occupied
    uint8_t MsgRetryCounter; //!< Info, how many times a Command/Query/Event has been repeated so far, because no answer has been received.
    uint8_t Data[DBAL_MAX_DATALEN_2REPEAT]; //!< Array with data for DBal frame, length set in DBal_cfg.h
};

/** \struct DBAL_TxRepeat
 *
 * \brief   Representation of Data to be repeated on Dbus2 level.
 *
 * \details For internal usage only. NOT TO BE USED BY USER!
 * \details For connection to SMM only.
 */
struct DBAL_TxRepeat
{
    uint8_t TxRepeatBuffer[DBAL_BUFFER_SIZE]; //!< Tx buffer duplicate for DBal request/response repetitions on Dbus2 level
    uint8_t TxRepeatLen; //!< Length of data to transmit - duplcate for repetitions on Dbus2 level
};

#ifdef DBAL_CROSS_CONNECTION
/** \struct DBALCR_InstanceExtension
 *
 * \brief   Extends DBAL_Instance for cross communication.
 *
 * \details For internal usage only. NOT TO BE USED BY USER!
**/
struct DBALCR_InstanceExtension
{
    struct STIM_Timer PingReqTimer; //!< Timer for pinging, if we are client.
    struct STDCB_Callback PingReqCbInst; //!< Callback of Ping Timer
    uint16_t PingTimeMs; //!< Time interval for pinging
    uint8_t PingRepetition; //!< Ping Repetitions already performed
    enum DBALCR_ParticipantType OwnPartType; //!< Our own participant type toward the communication partner.
};
#endif/*DBAL_CROSS_CONNECTION*/

/** \struct DBAL_Instance
 *
 * \brief   Structure logically describing a DBAL connection with another controller/Dbus2Node.
 *
 * \details For internal usage only. NOT TO BE USED BY USER!
**/
struct DBAL_Instance
{
    uint32_t InitPattern; //!< Makes sure, instance can be initialized exactly once during runtime.
    uint8_t CodeSectionBitMask; //!< Indicating, which of the critical code sections want to access DBAL_Instance for writing.
    uint8_t DBUS_ComPartner; //!< Dbus2 Node Address of communication partner
    uint8_t DBUS_ComBackup; //!< Backup of original Address of communication partner, in case of temporary connection
    bool DisableReqReceived; //!< Disable request received; Disable Pending; messages still to be delivered
    uint8_t ConnectTransmitBuffer[DBAL_CON_MSG_LEN]; //!< Tx buffer used for connection handling
    uint8_t ConnectDataLen; //!< Length of data to transmit in current Connection Dbus2 message
    uint8_t ConRepeatCnt; //!< Repetition counter for Enable/Disable Request
    uint8_t TransmitBuffer[DBAL_BUFFER_SIZE]; //!< Tx buffer for DBal request/response messages
    uint8_t TransmitDataLen; //!< Length of data to transmit in current Dbus2 message
    uint8_t DbalFrames2TransmitCnt; //!< Number of DBal frames contained in current Dbus2 message to transmit
    uint8_t LastSendingStatus[DBAL_MSG_INDEX_COUNT]; //!< Statuses from dbusdll, telling whether Dbus2 message was delivered.
    uint8_t SendRetryCounter[DBAL_MSG_INDEX_COUNT]; //!< Information, how many times it has been attempted to send a Dbus2 message.
    uint8_t SeqId2Send; //!< SequenceId, with which the next Dbus2 request/response message will be sent by DBal
    uint8_t LastSeqIdReceived; //!< SequenceId of the last request/response Dbus2 message, that has been received.
    struct DBAL_Msg2Repeat Msgs2Repeat[DBAL_MAX_MSGS2REPEAT]; //!< Info about Command/Query/Event to repeat, if no answer received. Length of array set in DBal_cfg.h.
    struct DBAL_Msg2Repeat* MsgRptPtrs[DBAL_MAX_MSGS2REPEAT]; //!< Pointers to Msgs2Repeat, to sort them as a FIFO, without copy operations.
    struct STIM_Timer ConMsgTimer; //!< Timer to repeat connection frame, if not delivered (no_ack)/responded (ack_ok).
    struct STDCB_Callback ConMsgCbInst; //!< Callback of ConMsgTimer
    struct STIM_Timer MsgTimer; //!< Timer to repeat Command/Query/Event, if no answer received.
    struct STDCB_Callback MsgCbInst; //!< Callback of MsgTimer
    enum DBAL_ConnectionState IoCurrentConnectionState; //!< State of connection state machine.
    enum DBAL_CommState IoCurrentCommState; //!< DBAL_COMMSTATE_READY or DBAL_COMMSTATE_NOT_READY
    uint8_t IoCommStateCbCounter; //!< Count of user callbacks, to notify about CommState change.
    uint8_t IoMsgLostCbCounter; //!< Count of user callbacks, to notify, that a message could not be delivered.
    DBAL_ioCommStateCb IoCommStateCbArray[DBAL_IO_MAX_CALLBACK_BUFFER_SIZE]; //!< Function pointers to user callbacks for Commstate change.
    DBAL_ioMsgLostCb IoMsgLostCbArray[DBAL_IO_MAX_CALLBACK_BUFFER_SIZE];//!< Function pointers to user callback to receive notification, that a message could not be delivered.
    struct DBAL_TxRepeat* TxRepeat; //!< Extension for connection to SMM to repeat transmission  of data on Dbus2 level.
#ifdef DBAL_CROSS_CONNECTION
    struct DBALCR_InstanceExtension* CrExtension; //!< Cross connection specific extension of instance
#endif/*DBAL_CROSS_CONNECTION*/
};

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

// Io Connection Handling
/** \brief     Reads the communication state, for a communication partner defined by a DBal instance.
 *
 * \param Inst DBal communication instance
 *
 * \return     DBAL_COMMSTATE_NOT_READY, DBAL_COMMSTATE_READY
*/
enum DBAL_CommState DBAL_getCommStateByInstance(const struct DBAL_Instance* const Inst);

/** \brief     Reads the connection state, for a communication partner defined by a DBal instance.
 *
 * \param Inst DBal communication instance
 *
 * \return     DBAL_CONNECTIONSTATE_DISCONNECTED, DBAL_CONNECTIONSTATE_CONNECTING,
 *             DBAL_CONNECTIONSTATE_CONNECTED, DBAL_CONNECTIONSTATE_DISCONNECTING
*/
enum DBAL_ConnectionState DBAL_getConnectionStateByInstance(const struct DBAL_Instance* const Inst);

/** \brief       Triggers the state machine for the status of the connection,
 *               for a communication partner defined by a DBal instance.
 *
 * \param Inst   DBal communication instance
 * \param Event  DBAL_CONNECTION_SM_EVENT_ENABLE, DBAL_CONNECTION_SM_EVENT_ENABLE_REQUEST,
 *               DBAL_CONNECTION_SM_EVENT_DISABLE, DBAL_CONNECTION_SM_EVENT_DISABLE_SILENT,
 *               DBAL_CONNECTION_SM_EVENT_DISABLE_REQUEST, DBAL_CONNECTION_SM_EVENT_ACCEPT,
 *               DBAL_CONNECTION_SM_EVENT_REJECT
*/
void DBAL_connectionSm(struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Event);

#ifdef DBAL_CROSS_CONNECTION
/** \brief     Notifies ping request timer to start from the beginning,
 *             because communication with partner has taken place.
 *
 * \param Inst DBal communication instance
 */
void DBALCR_triggerPingReqTimerWithStdTime(const struct DBAL_Instance* const Inst);
#endif /*DBAL_CROSS_CONNECTION*/

// functions for Instance handling
/** \brief                  Initializes a DBal communication instance.
 * \details                 Sets communication partner addresses.
 * \details                 Sets an "init pattern" to make sure, initialization can be done exactly once during runtime.
 * \details                 Remaining parameters are set to standard values.
 *
 * \param ComPartnerAddress Communication partners Dbus2 addresses
 * \param InstancePtr       Pointer to DBal communication instance
*/
void DBALIN_init(uint8_t ComPartnerAddress, struct DBAL_Instance* const InstancePtr);

/** \brief Returns pointer to main DBal communication instance (SMM) to use for initialization only.
 *
 * \return Returns pointer to main DBal communication instance (SMM) to use for initialization only.
*/
struct DBAL_Instance* DBALIN_getMainInstanceForInit(void);

/** \brief Returns pointer to user DBal communication instance (ecu to ecu cross communication)
 *         to use for initialization only.
 *
 * \return Returns pointer to user DBal communication instance (ecu to ecu cross communication)
 *         to use for initialization only.
*/
struct DBAL_Instance* DBALIN_getUserInstancesForInit(void);

/** \brief Returns pointer to main DBal communication instance, if initialized; otherwise NULL.
 *
 * \return Returns pointer to main DBal communication instance, if initialized; otherwise NULL.
*/
struct DBAL_Instance* DBALIN_getMainInstance(void);

/** \brief               Searches both main (SMM) and user (ecu to ecu) DBal communication instances,
 *                       for one, that has the given Dbus2 address.
 *
 * \param PartnerAddress Dbus2 address of communication partner.
 *
 * \return               Returns a valid pointer, if such an instance is found; otherwise NULL.
*/
struct DBAL_Instance* DBALIN_getInstanceByAddress(uint8_t PartnerAddress);

/** \brief       Returns pointer to a DBal user (ecu to ecu) communication instance.
 *
 * \param Index  Position of user (ecu to ecu) DBal communication instance inside the array, which contains them.
 *
 * \return       Returns a valid pointer, if an instance with the given index exists and if it is initialized;
 *               otherwise false.
*/
struct DBAL_Instance* DBALIN_getUserInstanceByIndex(uint8_t Index);

/** \brief      Returns index of DBal user (ecu to ecu) DBal communication instance inside the array with user instances.
 *
 * \param  Inst Pointer to DBal user (ecu to ecu) communication instance.
 *
 * \return      If provided pointer is invalid, function returns UINT8_MAX,
 *              otherwise a valid index between zero and DBAL_CROSS_CONNECT_COUNT.
*/
uint8_t DBALIN_getIndexOfUserInstance(const struct DBAL_Instance* const Inst);

#ifdef DBAL_CROSS_CONNECTION
/** \brief                Init cross connection specific part of DBal instance and decide,
 *                        whether it will be server or client on the given cross connection.
 *
 * \details               This is possible exactly once during runtime; For each value of parameter "Index".
 *
 * \param Index           Position of user (ecu to ecu) DBal communication instance inside the array,
 *                        which contains them.
 * \param ParticipantType DBALCR_PART_TYPE_NONE, DBALCR_PART_TYPE_CLIENT, DBALCR_PART_TYPE_SERVER
 */
void DBALIN_initCrExtensionByIndex(uint8_t Index, enum DBALCR_ParticipantType ParticipantType);

/** \brief      See, whether we are server or client or nothing on the given cross connection.
 *
 * \param Index Position of user (ecu to ecu) DBal communication instance inside the array, which contains them.
 *
 * \return      DBALCR_PART_TYPE_NONE, DBALCR_PART_TYPE_CLIENT, DBALCR_PART_TYPE_SERVER
 */
enum DBALCR_ParticipantType DBALIN_getCrParticipantTypeByIndex(uint8_t Index);

// functions for cross connection queue
/** \brief Initialization of internal variables in cross connection queue.
 */
void DBALCQ_init(void);
/** \brief          Add connection message to cross connection queue.
 *
 * \param Inst[in]  Pointer to DBal user (ecu to ecu) communication instance.
 */
void DBALCQ_queueConnectMsg(const struct DBAL_Instance* Inst);
/** \brief          Read connection message from cross connection queue
 *
 * \param Inst[out] Pointer to pointer to DBal user (ecu to ecu) communication instance.
 *
 * \return          True, if there was at least one entry in the queue, otherwise false.
 */
bool DBALCQ_getConnectMsg(const struct DBAL_Instance** const Inst);
/** \brief            Add request/response message to cross communication queue.
 *
 * \param Inst[in]    Pointer to DBal user (ecu to ecu) communication instance.
 * \param TxIndex[in] Index of message in DBAL_DBUS_Handler_TxObject.
 */
void DBALCQ_queueReqRespMsg(const struct DBAL_Instance* const Inst, uint8_t TxIndex);
/** \brief             Read request/response message from cross communication queue.
 *
 * \param Inst[out]    Pointer to pointer to DBal user (ecu to ecu) communication instance.
 * \param TxIndex[out] Pointer to index of message in DBAL_DBUS_Handler_TxObject.
 *
 * \return             True, if there was at least one entry in the queue, otherwise false.
 */
bool DBALCQ_getReqRespMsg(const struct DBAL_Instance** const Inst, uint8_t* const TxIndex);
#endif/*DBAL_CROSS_CONNECTION*/

// functions used by internal App Layer stuff
/** \brief     Initializes the timers for repeating messages, for a communication partner defined by a DBal instance.
 *
 * \param Inst DBal communication instance
*/
void DBAL_appLayerDBus2Init(struct DBAL_Instance* const Inst);

/** \brief     Sends a connection message to communication partner to request an enabling of the connection.
 *
 * \param Inst DBal communication instance
*/
void DBAL_sendCommEnableRequest(struct DBAL_Instance* const Inst);

/** \brief     Sends a connection message to communication partner to confirm that connection was enabled.
 *
 * \param Inst DBal communication instance
*/
void DBAL_sendCommEnableResponse(struct DBAL_Instance* const Inst);

/** \brief     Sends a connection message to communication partner to request a disabling of the connection.
 *
 * \param Inst DBal communication instance
*/
void DBAL_sendCommDisableRequest(struct DBAL_Instance* const Inst);

/** \brief     Sends a connection message to communication partner to confirm that connection was disabled.
 *
 * \param Inst DBal communication instance
*/
void DBAL_sendCommDisableResponse(struct DBAL_Instance* const Inst);

/** \brief     Sends a ping request
 *
 * \param Inst DBal communication instance
*/
void DBAL_sendCommPingRequest(struct DBAL_Instance* const Inst);

/** \brief     Sends a ping response
 *
 * \param Inst DBal communication instance
*/
void DBAL_sendCommPingResponse(struct DBAL_Instance* const Inst);

/** \brief     Sends a connection message to communication partner to confirm that temporary connection was enabled.
 *
 * \param Inst DBal communication instance
*/
void DBAL_sendCommTempEnableResponse(struct DBAL_Instance* const Inst);

/** \brief     Sends a connection message to communication partner to confirm that temporary connection was disabled.
 *
 * \param Inst DBal communication instance
*/
void DBAL_sendCommTempDisableResponse(struct DBAL_Instance* const Inst);

#endif /* DBAL_USE_INSTANCE*/

/** \brief             Notification from Dbus2 Bal, that message could not be delivered to recipient.
 *
 * \param MessageIndex Index of message in DBAL_DBUS_Handler_TxObject.
*/
void DBAL_nonDeliverableMessage(uchar MessageIndex);

/** \brief              Sets dbusdll sending status from DLL_vMessageTransmitted_UserCallback.
 * \details             Used by DBal to evaluate further actions, see DBal spec.
 * \details             DLL_ACK_NOT_RECEIVED: Disable connection and try to wake up communication partner,
 *                      before sending connection frame.
 * \details             Other error: Just send message again.
 * \details             DLL_ACK_OK: Correctly delivered. No need for further action.
 *
 * \param TargetAddress Dbus2 address of recipient.
 * \param Status        DLL_ACK_NOT_RECEIVED, DLL_ACK_BUSY, DLL_ACK_WRONG, DLL_ACK_OK, DLL_ACK_TRANSMISSION_ABORTED
 * \param MsgIndex      Index of message in DBAL_DBUS_Handler_TxObject.
*/
void DBAL_setLastSendingStatus(uint8_t TargetAddress, uint8_t Status, uint8_t MsgIndex);

/** \brief            This function is a mapping to DBPL_bIsNodeToBeWokenUp.
*
*
*\param TargetAddress Address, for which status is requested.\n

*\return              Status, whether to wake up node or not.\n
*/
bool DBAL_isNodeToBeWokenUp(uint8_t TargetAddress);

//shared by all
/** \brief        Checks, if value is a valid Dbus2 address for directed communication.
 *
 * \details       Values below 0x10 are wrong, because those are for broadcast.
 *
 * \param Address Dbus2 node address to be verified.
 *
 * \return        Returns true, if address valid according to named conditions, otherwise false.
 */
static inline bool DBAL_isValidDbus2Address(uint8_t Address)
{
    return (Address >= 0x10U) ? true : false;
}

/** \brief        Checks correctness of protocol version.
 *
 * \details       Current or older tolerated Version is correct
 *
 * \param Address Protocol version to be verified.
 *
 * \return        Returns true, if version valid according to named conditions, otherwise false.
 */
static inline bool DBAL_isCorrectProtocolVersion(uint8_t ProtocolVersion)
{
    return (ProtocolVersion == DBAL_PROTOCOL_VERSION) ? true : false;
}

#ifdef __cplusplus
}
#endif

#endif
