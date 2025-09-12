/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          IC Dbus 2
*   COMP_ABBREV      BAL
*******************************************************************************/

/*****************************************************************************************************************
   DESCRIPTION
 *     
 *     Application layer for the D-Bus-2 and CAN Bus
 *     
 *     This layer handles the message distribution of the system messages. Received messages, which belong to
 *     system messages (differentiation according to the identifier) will be distributed to the
 *     subsystem corresponding to either the subsystem addressed (when addressed subsystem differs from 0),
 *     or to the subsystem, which is indirectly addressed via a service function in the corresponding ReceiveObject
 *     table.
 *     
 *     ___________________________________________                    ______________________________________
 *     |BusObject Table                           |                   |ReceiveObjectSubsystem Table         |
 *     |  -uint8_t     Subsystem                  |  -  -  -  -  - >  |  -uint32_t Busidentifier            |
 *     |  -reference to ReceiveObjectSubsystem    |  -                |  -uint8 DataLength                  |
 *     |  -reference to TransmitObjectSubsystem   |     -             |  -reference to void ServiceFunction |
 *     |  -reference to NumberOfElementsInTxTable |        -          --------------------------------------
 *     |  -reference to MessageToTransmitBits     |           -       ______________________________________
 *     -------------------------------------------               - >  |TransmitObjectSubsystem Table        |
 *                                                                    |  -uint32_t Busidentifier            |
 *                                                                    |  -uint8 DataLength                  |
 *                                                                    |  -reference to void ServiceFunction |
 *                                                                    |  -reference to void CompleteFunction|
 *                                                                    --------------------------------------
 *                                                                                                           
 *****************************************************************************************************************/

/**
\if BAL
\file 
     \brief See Mainpage for a description of Bus application layer
\endif
*/

/**
\if BAL
\mainpage BAL
\else
\file
    \brief Application layer for the D-Bus-2 and CAN bus
\endif

\section introBAL Bus application layer


 *     Application layer for the D-Bus-2 and CAN Bus
 *     
 *     This layer handles the message distribution of the system messages. Received messages, which belong to
 *     system messages (differentiation according to the identifier) will be distributed to the
 *     subsystem corresponding to either the subsystem addressed (when addressed subsystem differs from 0),
 *     or to the subsystem, which is indirectly addressed via a service function in the corresponding ReceiveObject
 *     table.

     \image html bal.gif

*/
/**
\if BAL_RCtrl
\mainpage BAL_RCtrl
\else
\file
    \brief Application layer for the D-Bus-2 and CAN bus
\endif

\section introBAL Bus application layer


 *     Application layer for the D-Bus-2 and CAN Bus
 *     
 *     This layer handles the message distribution of the system messages. Received messages, which belong to
 *     system messages (differentiation according to the identifier) will be distributed to the
 *     subsystem corresponding to either the subsystem addressed (when addressed subsystem differs from 0),
 *     or to the subsystem, which is indirectly addressed via a service function in the corresponding ReceiveObject
 *     table.

     \image html bal.gif

*/
/**
\if BAL_IgnSubSys
\mainpage BAL_IgnSubSys
\else
\file
    \brief Application layer for the D-Bus-2 and CAN bus
\endif

\section introBAL Bus application layer


 *     Application layer for the D-Bus-2 and CAN Bus
 *     
 *     This layer handles the message distribution of the system messages. Received messages, which belong to
 *     system messages (differentiation according to the identifier) will be distributed to the
 *     subsystem corresponding to either the subsystem addressed (when addressed subsystem differs from 0),
 *     or to the subsystem, which is indirectly addressed via a service function in the corresponding ReceiveObject
 *     table.

     \image html bal.gif

*/
/**
\if BAL_SysTest
\mainpage BAL_SysTest
\else
\file
    \brief Application layer for the D-Bus-2 and CAN bus
\endif

\section introBAL Bus application layer


 *     Application layer for the D-Bus-2 and CAN Bus
 *     
 *     This layer handles the message distribution of the system messages. Received messages, which belong to
 *     system messages (differentiation according to the identifier) will be distributed to the
 *     subsystem corresponding to either the subsystem addressed (when addressed subsystem differs from 0),
 *     or to the subsystem, which is indirectly addressed via a service function in the corresponding ReceiveObject
 *     table.

     \image html bal.gif

*/

#ifndef BAL_H__
#define BAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "bustypes.h"
#include "system_timer.h"


/******************************************************************************/
/* DEPENDENCIES                                                               */
/******************************************************************************/
#define DEPLIB_BAL MOD_DBPL, MOD_DLL, MOD_TIM, ///< dependencies of library module


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define BAL_UNKNOWN_MESSAGE     ((uint8_t)0xFFU)  //!< Value identifying an unknown message. If a message, which is not defined by the receiver, is sent to a subsystem, this needs a way to identify this.
#define BAL_SERVICE_SUBSYSTEM_ID        0x00U	  //!< ID of the service subsystem
#define BAL_FIRST_SYSTEM_MSGID          0x0000U   //!< First message ID in the range of system messages
#define BAL_LAST_SYSTEM_MSGID           0x7FFFU   //!< Last message ID in the range of system messages
#define BAL_FIRST_SERVICE_MSGID         0xF000U   //!< First message ID in the range of service messages
#define BAL_LAST_SERVICE_MSGID          0xFFFFU   //!< Last message ID in the range of service messages

#define BAL_D_BUS_2_MESSAGE_LENGTH_OFFSET     ((uint8_t)0x02U)       //!< The data length of a D-Bus-2 message corresponds to the data lenght of the message + the data length of the identifier (uint16_t), hence the offset to be transferred to the service function is 2.


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** Error definitions (which can be used by an error function in the excess template) */
enum BAL_eErrorDefinitions
{
    BAL_NO_ERROR,
    BAL_ERROR_UNKNOWN_MESSAGE_RECEIVED,
    BAL_ERROR_UNKNOWN_MESSAGE_FOR_TRANSMISSION,
    BAL_ERROR_NOT_STARTET_MESSAGE_REPORTED_FINISHED,
    BAL_ERROR_NOT_ABLE_TO_GAIN_BUS_ACCESS
};

/** \brief Describes, whether the break is being generated right now; as well as the state of the DLine, if the break is being generated.
 */
enum BAL_BreakState
{
    BAL_BREAK_START,
    BAL_BREAK_LOW,
    BAL_BREAK_HIGH,
    BAL_BREAK_END
};

/** \brief Describes duration of a given break state during the break generation.
 *
 * \detail An array of this type is a complete recipe.
 */
struct BAL_BreakRecipe
{
    enum BAL_BreakState state;
    uint16_t            delayMs;
};

/** \brief Describes the current stage of the break generation.
 */
struct BAL_BreakHandler
{
    struct STDCB_Callback           callback;
    struct STIM_Timer               timer;
    const struct BAL_BreakRecipe    *recipe;
    uint8_t                         stage;
#ifdef DBM_DBUSCAN
    uint8_t                         attempts;
#endif
    void (*notifyFunction)(void);
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

#if !defined(DBM_DBUSCAN)
/* This section should better be defined in a private header, but as several existing projects operate with balXS.c files, which only include this header, these declarations are made global. */
/* The following three maximum repetition values must be defined in the excess template (balXS.c) */
extern const uint8_t BAL_ucMaxCollisionRetries;//   = 16;
extern const uint8_t BAL_ucMaxMissingAckRetries;//  = 4;
extern const uint8_t BAL_ucMaxNackRetries;//        = 2;
#else // DBM_DBUSCAN
extern const uint8_t BAL_ucMaxTxRetries;//          = 16;
#endif

#ifdef __cplusplus
#ifdef DBUS2_NONCONST_BAL_TABLE
extern TbusObjectTable BAL_tBusObject[];
#else /* not DBUS2_NONCONST_BAL_TABLE*/
extern const TbusObjectTable BAL_tBusObject[];
#endif /*DBUS2_NONCONST_BAL_TABLE*/
#else /* not__cplusplus*/
extern const TbusObjectTable BAL_tBusObject[];
#endif /*__cplusplus*/
extern const uint8_t BAL_ucNumberOfSubsystems;
extern const struct BAL_BreakRecipe BAL_WakeupBreakRecipe[];
extern const struct BAL_BreakRecipe BAL_ResetBreakRecipe[];


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/**
 Finds the Subsystem index corresponding to the Subsystem given.

 If the given Subsystem is not found, the index 0 is returned (hence the first defined Subsystem will be used).
 This function may be replaced by an empty function in the excess template to save ROM code,
 if this functionality is not needed - or if possibly a special identifier should be returned, when the subsystem is not know.

\param ucSubsystem
 The subsystem, to which the index will be found\n
\b type       : uint8_t

\return
The index corresponding to the Subsystem\n
\b type       : uint8_t\n
\b Range      : 0...(number of elements in bus table - 1)

*/
uint8_t BAL_ucGetSubsystemIndex(uint8_t ucNodeAddress); //Replaceable function

/**
 Finds the message index corresponding to the received message identifier.

 If the given message is not found, BAL_UNKNOWN_MESSAGE is returned.
 This function may be replaced by an empty function in the excess template to save ROM code,
 if this functionality is not needed.

\param tMessageIdentifier
The message identifier, which is searched.\n
\b type       : TbusMessageIdentifier

\param tSubsystemIndex
The index of the subsystem, where the message will be sought\n
\b type       : uint8_t

\return
The index corresponding to the Subsystem\n
\b type       : uint8_t\n
\b Range      : 0...(number of elements (=messages) in transmission/receive table - 1)


\note This function might never end searching for a received message (if the received message identification is unknown) - if the last entry of the list (table) is not maked with MSB set. In case MISRA rules are applied, this issues should be checked carefully.
*/
uint8_t BAL_ucGetMessageIndex(TbusMessageIdentifier tMessageIdentifier, uint8_t ucSubsystemIndex); //Replaceable function

/**
 Finds the message index corresponding to the transmit message identifier.

 If the given message is not found, BAL_UNKNOWN_MESSAGE is returned.
 This function may be replaced by an empty function in the excess template to save ROM code,
 if this functionality is not needed.

\param MessageIdentifier
The message identifier, which is searched.\n
\b type       : TbusMessageIdentifier

\param SubsystemIndex
The index of the subsystem, where the message will be sought\n
\b type       : uint8_t

\return
The index corresponding to the Subsystem\n
\b type       : uint8_t\n
\b Range      : 0...(number of elements (=messages) in transmission/receive table - 1)

*/
uint8_t BAL_ucGetMessageTransmitIndex(TbusMessageIdentifier tMessageIdentifier, uint8_t ucSubsystemIndex); //Replaceable function

#ifdef RTOS_DBUS_EVENTDRIVEN
/**
 This function must be called, if a dbus message needs to be sent.

 Function is abstracted from BAL_HandleTask() and handles this functionality instead, if RTOS eventdriven DBUS implementation is used.
 It's required, to call (with procssing time of it's dependent functions), as long as "true" was not returned.

\return
\b type       : bool\n
\b range      : true: handling done, false: needs to be called again

*/
bool BAL_SendMessage(void);

#else
/**
 The application layer task handler should be called periodically,
 depending on the load of messages to transmit
 (more frequent messages to transmit: Task handler must be called more frequently).

\return
\b type       : uint8_t\n
\b range      : 0=false (=not initialised), all other= true (initialised)

*/
uint8_t BAL_HandleTask(void);
#endif // #ifdef RTOS_DBUS_EVENTDRIVEN

/**
 This function is called, when a subsystem (1st parameter) wants a message to be transmitted.
 A flag is set in the transmit flag array associated with this subsystem.

\param ucSubsystem
The requesting subsystem\n
\b type       : uint8_t

\param ucMessageIndex
The index in the table of transmit messages\n
\b type       : uint8_t

\return
\b type       : void
 
*/
void BAL_vTransmitMessage(uint8_t ucSubsystem, uint8_t ucMessageNumber);

/**
 This function is called, when a subsystem decides that a previously flagged message, is no longer
 valid for transmission.

\param ucSubsystem
 The requesting subsystem\n
\b type       : uint8_t

\param ucMessageIndex
 The index in the table of transmit messages (in this case: stop transmission)\n
\b type       : uint8_t

\return
\b type       : void
 
*/
void BAL_vCancelTransmitMessage(uint8_t ucSubsystem, uint8_t ucMessageIndex);

/**
 This function is called, when a message is not able to be delivered (the receiver does not acknowledge)
 for a certain amount of times (depending on the BAL_MAX_TRANSMISSION_ATTEMPTS).

\param ucSubsystem
 The corresponding subsystem\n
\b type       : uint8_t

\param ucMessageIndex
 The index in the table of transmit messages.\n
\b type       : uint8_t

\return
\b type       : void
 
*/
void BAL_vNotifyNonDeliverableMessage(uint8_t ucSubsystem, uint8_t ucMessageIndex);

/**
 This function is called, when an error (or unexpected behaviour) has happened.
 The action to take must be decided in the excess template (where this function is also defined).

\param ucError
 An identifier for the error, which has occurred.\n
\b type       : uint8_t

\return
\b type       : void
 
*/
void BAL_vNotifyError(uint8_t ucError);

/**
 This function is called, when an error (or unexpected behaviour) has happened.
 The action to take must be decided in the excess template (where this function is also defined).

\param ucReceivedTargetAddress
\b type       : uint8_t

\param tReceivedMessageId
\b type       : TbusMessageIdentifier

\return
\b type       : void
 
*/
void BAL_vNotifyUnknownMessage(uint8_t ucReceivedTargetAddress, TbusMessageIdentifier tReceivedMessageId);

/**
 This function is called, when a message needs to be changed before sending.
 One may want to do that, if the same messageIds are to be sent to different nodes and/or with different payload lengths.
 For this, you must have a transmit table in RAM (non const), so that you can manipulate entries before calling BAL_vTransmitMessage.
 The function even enables the user, to have less transmit table entries, than different messageIds to be sent.

\param BAL_tTransmitObject_DynLength
\b type       : TbusTransmitObject

\param ucNode
\b type       : uint8_t

\param uiMsgId
\b type       : uint16_t

\param ucDataLength
\b type       : uint8_t

\return
\b type       : void

*/
void BAL_vSetTransmitMsg(TbusTransmitObject* BAL_tTransmitObject, uint8_t ucNode, uint16_t uiMsgId, uint8_t ucDataLength);

/**
 This function is called, when the ID of the last received message is needed.

 The ID of the last received message could be useful e.g. in the function BAL_vNotifyError().

 The ID of the last RemoteControl message/object is needed whenever a received write object request
 is unsuccessful, due to undefined object (this includes objects, which are read-only)
 or the state does not allow the current object to be written.

\return
Last received Message ID\n
\b type       : TbusMessageIdentifier (16 bit)\n
\b range      : 0x0000-0xFFFF\n
*/
TbusMessageIdentifier BAL_tGetReceivedMessageId(void);

/**
 This function is called, when an application layer message has been received (an application layer message
 is identified by checking the high byte of the message identifier).

\param ReceivedMessage
A pointer to the oldest, not yet distributed, message in the input buffer\n
\b type       : TbusMessage *

\return
Positive result when the message is known, hence can be delivered.
 If the message is not defined, this function returns false\n
\b type       : bool

\note This function is used for distributing messages received on D-Bus-2.
*/
bool BAL_bDispatchRcvdDbus2Msg(const TbusMessage *ptReceivedMessage);

/**
 * \brief Enter logical state in which application specific messages cannot be sent.
 * \details Service messages are allowed, as well as generic messages, which supervise the logical state of the system.
 */
void BAL_vEnterNonOperatingMode(void);
/**
 * \brief Leave logical state in which application specific messages cannot be sent.
 * \details Service messages are allowed, as well as generic messages, which supervise the logical state of the system.
 */
void BAL_vLeaveNonOperatingMode(void);
/**
 * \brief Check, whether dbus is in NonOperating Mode
 */
bool BAL_bIsNonOperatingMode(void);

/**
 This function is to sent a 4ms wakeup break.


\return\n
\b type       : void\n
\b param      : void\n

*/
void BAL_vSendWakeupBreak(void);

/**
 This function is to sent a 7s/2s reset break with 750 ms pause between them.


\return\n
\b type       : void\n
\b param      : void\n

*/
void BAL_vSendResetBreak(void);

/**
 This function is to sent a 4ms wakeup break.


\return\n
\b type     : bool\n

\param      : tMessageIdentifier
\b type     : TbusMessageIdentifier

\param      : ucSubsystemIndex
\b type     : uint8_t

*/
bool BAL_bIsIdentifierInRangeOfReceiveTable(TbusMessageIdentifier tMessageIdentifier, uint8_t ucSubsystemIndex);

/**
 * \brief For Real Time Operating Systems
 */
void BAL_vEnterCriticalSectionForSending(void);

/**
 * \brief For Real Time Operating Systems
 */
void BAL_vExitCriticalSectionforSending(void);

/**
Function called by user to clear the flags for messages to send
*/
void BAL_ClearMessagesToSend(void);

/**
 * \brief Gives information, whether a sending retry is going on.
 * \return\n
 * \b type     : bool\n
 */
bool BAL_bIsSendingRetry(void);

#ifdef __cplusplus
}
#endif

#endif
