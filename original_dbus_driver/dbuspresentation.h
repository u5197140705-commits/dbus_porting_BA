/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          IC DBUS
*   COMP_ABBREV      DBPL
*******************************************************************************/

/*****************************************************************************************************************
 *   DESCRIPTION
 *     Presentation layer for the D-Bus-2
 *     This module handles the predefined messages in the dbuspresentation.c + .h and
 *     includes definitions for software modules, as well as the forwarding mechanism
 *     of not-predefined-messages (e.g. nmt message handler), in the excess template: dbuspresentationXS.c
 *     
 *     An incoming message will be picked up by the presentation layer. If it is a predefined message,
 *     the presentation layer will handle this message. Else the message will be distributed to the
 *     message handler, which is defined in the excess template.
 *     
 *****************************************************************************************************************/

/**
\if DBus2Presentation
\file 
     See Mainpage for a description of Presentation layer
\endif
*/

/**
\if DBus2Presentation
\mainpage PresentationLayer
\else
\file
 Presentation layer for the D-Bus-2
\endif

\section introDBPL Presentation Layer for D-Bus-2

     This module handles the predefined messages in the dbuspresentation.c + .h and
     includes definitions for memory modules, as well as the forwarding mechanism
     of not-predefined-messages (e.g. nmt message handler), in the excess template: dbuspresentationXS.c
     
     An incoming message will be picked up by the presentation layer. If it is a predefined message,
     the presentation layer will handle this message. Else the message will be distributed to the
     message handler, which is defined in the excess template.

    \subsection subsection1 Service Messages
    The Presentation Layer handles service messages. \b Service \b messages constitute the \b Diagnostic-II part of \b D-Bus-2. Diagnostic-II is the subset used for communication with external systems (e.g. PC used by after sales services).

*/
#ifndef DBUSPRESENTATION_H__
#define DBUSPRESENTATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsh_stdinc.h"
#include "LibDefines.h"
#include "LibTypes.h"
#include "bustypes.h"

/*Forwand declaration of needed struct.*/
struct DBPL_EcuUniqueIdReadResult;

#define DEPLIB_DBPL MOD_DLL, MOD_TIM, ///< dependencies of library module




/* Tables (access needed by the application layer for transmission/reception of messages) */
/*Axivion Disable MisraC2012-1.1 : Number of significant initial characters for external identifier not a problem with our tools */
extern const TbusReceiveObject DBPL_tReceiveMandatoryServiceMessages[];
extern const TbusReceiveObject DBPL_tReceiveAll16BitServiceMessages[];
extern const TbusReceiveObject DBPL_tReceiveAllServiceMessages[];
extern const TbusReceiveObject DBPL_tReceiveAllServiceMessagesUpdate[];
#ifdef DBUS2_UPDATE_HSI
extern const TbusReceiveObject DBPL_tReceiveAllServiceMessagesUpdateHsi[];
#endif
/*Axivion Enable MisraC2012-1.1 */
extern const TbusModuleTable DBPL_tModuleDict[];
extern const uint8_t DBPL_ucNumberOfElements;
/* variables defined in the excess template */
extern uint16_t DBPL_uiPage;
extern bool DBPL_bInvalidReadProcess;  //!< Variable used for discarding the due read response in case read memory cannot be performed.
extern bool DBPL_bInvalidWriteProcess; //!< Variable used for discarding the due write process in case it cannot be performed.

/*
Also DBPL_tMessageBuffer is defined in the excess template. This variables is, however, not declared in this header, due to problem of redefinition (if e.g. a different size is wished in the project compared to the used size at compilation time of library).
*/
/* Declaration of private functions and variables */
/* This section should better be defined in a private header, but as several existing projects operate with dbuspresentationXS.c files, which only include this header, these declarations are made global. */
uint8_t DBPL_ucGetModuleIndex(uint8_t ucMemoryModule);
uint8_t DBPL_ucPossiblyChangeTimerBase(uint8_t ucBase);

#ifdef RTOS_DBUS_EVENTDRIVEN
/** @name DBPL Interface Functions
 This function must be called at initialization of the system.

 Function is abstracted from DBPL_HandleTask() and handles this functionality instead, if RTOS eventdriven DBUS implementation is used.

\param none

\return none
*
*/
extern void DBPL_ModuleInit( void );

/**
 This function must be called, if a dbus message is received.

 Function is abstracted from DBPL_HandleTask() and handles this functionality instead, if RTOS eventdriven DBUS implementation is used.
 It's required, to call (with procssing time of it's dependent functions), as long as "true" was not returned.

\param none

\return
state, if handling is done\n
\b type       : bool\n
\b range      : handling done: true, else false
*
*/
extern bool DBPL_ReceiveMessage( void );

/**
 This function must be called, if a dbus service message must be sent.

 Function is abstracted from DBPL_HandleTask() and handles this functionality instead, if RTOS eventdriven DBUS implementation is used.
 It's required, to call (with procssing time of it's dependent functions), as long as "true" was not returned.

\param none

\return
state, if handling is done\n
\b type       : bool\n
\b range      : handling done: true, else false
*
*/
extern bool DBPL_SendServiceMessage( void );

/**
 This function must be called, if a powermessage (via dbus) must be sent.

 Function is abstracted from DBPL_HandleTask() and handles this functionality instead, if RTOS eventdriven DBUS implementation is used.
 It's required, to call (with procssing time of it's dependent functions), as long as "true" was not returned.

\param none

\return
state, if handling is done\n
\b type       : bool\n
\b range      : handling done: true, else false
*
*/
extern bool DBPL_SendPowerMessage( void );

#else
/** @name DBPL Interface Functions
Presentation layer interface functions. DBPL: D-Bus-2 Presentation layer.

*/
/**
 Presentation layer task handler.

\return
\b type       : uint8_t\n
\b range      : returns TASK_INITIALISED when initialised, which is always the case.
*/
uint8_t DBPL_HandleTask(void);
#endif //#ifdef RTOS_DBUS_EVENTDRIVEN

uint8_t DBPL_ucGetCurrentMemoryModule(void); //!<Returns the currently set MemoryModule (DBPL_ucMemoryModule).

/** @name DBPL Memory Access Services

Memory access services.

Presentation layer memory access services.
*/
#define DBPL_DONE       (true)    //!< Definition for successful call.
#define DBPL_ONGOING    (false)   //!< Definition for ongoing call (call must be repeated).
/**
This function is called, when a read memory request is received on the bus.

 This function will call another read function, which must be supplied by the excess template, depending on the active memory module.
 The function, which is called in the excess template gives access to the corresponding memory cell.

\param ucMemoryModule
MemoryModule as referred to by the message, received on the bus.\n
\b type       : uint8_t\n
\b range      : 0..255, according to definitions in dbuspresentationXS.c

\param uiAddress
Address: relative to a base - possibly with offset (depending on the Memory module definition), referred to by the message, received on the bus.\n
\b type       : uint16_t\n
\b range      : 0x0000..0xFFFF

\param ucDataLength
Length of the message to read (how many byte to read from the address referred to).\n
\b type       : uint8_t\n
\b range      : 0..255

\param pucMessageData
The requested message data\n
\b type:      : uint8_t *

\return
 state of request\n
\b type       : bool\n
\b range      : discrete values: DBPL_DONE (true), DBPL_ONGOING (false)

\note Please note that this interface is used also for reading 32 bit addresses - then in addition the variable DBPL_uiPage is used for the high integer of the address.
*/
bool DBPL_bReadMemory(uint8_t ucMemoryModule, uint16_t uiAddress, uint8_t ucDataLength, uint8_t *pucMessageData);

/**
 This function is called, when a write memory request is received on the bus.

 This function will call another write function, which must be supplied by the excess template, depending on the active memory module.
 The function, which is called in the excess template gives access to the corresponding memory cell.

\param ucMemoryModule
MemoryModule as referred to by the message, received on the bus.\n
\b type       : uint8_t\n
\b range      : 0..255, according to definitions in dbuspresentationXS.c

\param uiAddress
Address: relative to a base - possibly with offset
 (depending on the memory module definition),
 referred to by the message, received on the bus.\n
\b type       : uint16_t\n
\b range      : 0x0000..0xFFFF

\param ucDataLength
Length of the message to write (how many byte to write from the address referred to).\n
\b type       : uint8_t\n
\b range      : 00..255

\param pucMessageData
The requested message data\n
\b type:      : uint8_t *

\return
state of request\n
\b type       : bool\n
\b range      : discrete values: DBPL_DONE (true), DBPL_ONGOING (false)

\note Please note that this interface is used also for writing 32 bit addresses - then in addition the variable DBPL_uiPage is used for the high integer of the address.
*/
bool DBPL_bWriteMemory(uint8_t ucMemoryModule, uint16_t uiAddress, uint8_t ucDataLength, uint8_t *pucMessageData);


/**
 This function gives the address of the first character in a BSH identification string.

 Please note that the address is not returned via return value, but over the pointer parameter.
 The identification string is terminated by a double 0x00 (not ASCII 0)
 This function is called, when an identity request is received on the bus.
 This function will call another id-function, which must be supplied by the excess template, depending on the active memory module.
 The function, which is called in the excess template gives access to the corresponding memory cell.

\param ucMemoryModule
Which memory module to identify\n
\b type       : uint8_t\n
\b range      : 0..255

\param puiIdAddress
Pointer to where the address of the start of the string of ASCII characters is stored, used for identification purposes.\n
\b type       : uint16_t *

\return
 state of request\n
\b type       : bool\n
\b range      : discrete values: DBPL_DONE (true), DBPL_ONGOING (false)

\note Please note that this interface is used also for "reading" 32 bit addresses. If the ID-string is placed in a memory area where 32 bit access is needed, the resulting address, which is returned in the ID-response message, will be appended by the upper integer, which is kept in variable DBPL_uiPage in the service: DBPL_vIdentityRequest1632().
*/
bool DBPL_bGetIdAddress(uint8_t ucMemoryModule, uint16_t *puiIdAddress);

/**
*This function notifies dbuspresentation, when  a 4ms wakeup break has been sent by the node.
*
*
\return\n
\b type       : void\n
\b param      : void\n
*
*/
void DBPL_vNotifyWakeupBreakSent(void);

/**
*This function notifies dbuspresentation, that the sending of 4ms break should not be confirmed any more.
*
*
\return\n
\b type       : void\n
\b param      : void\n
*
*/
void DBPL_vUnNotifyWakeupBreakSent(void);

/**
*This function is for application to tell DBPL, whether a node is to be woken up, i.e. if WakeupSentResponse is to be sent. In XS-template.
*
*
\return       status, whether to wake up node\n
\b type       : bool\n
\b param      : uint8_t\n
*
*/
bool DBPL_bIsNodeToBeWokenUp(uint8_t nodeAddress);
/**
*This function is DBPL to tell the application, that a WakeupSentResponse from the given node has come has been received. In dbuspresentationXS-template.
*
*
\b type       : void\n
\b param      : uint8_t\n
*
*/
void DBPL_vWakeupSentResponseReceived(uint8_t nodeAddress);
/**
*This function is for Application to tell DBPL the node address, that is to be put into the WakeUpSentRequest/Response and PowerResurgeMsg. In dbuspresentationXS-template.
*
*
\return       Own node address, containing main node and subsystem chosen by user. The user has to choose, which of the subsystems supported by his app is to be returned here.
\b type       : uint8_t\n
*
*/
uint8_t DBPL_ucGetOwnAddressToInformOtherNodes(void);

/**
*This function writes the read result for the unique Id. In dbuspresentation_productionXs-template.
*
\param maxReadableUniqueIdLen
Maximum length of Unique Id fitting into Dbus sending buffer.\n
\b type       : uint8_t\n
\b range      : 00..248
\param result
Struct containing pointer, where Unique Id string is to be written, as well as status code.\n
\b type       : struct DBPL_EcuUniqueIdReadResult*\n
\b range      : 00..248
\return\n
\b type       : void
*
*/
void DBPL_tGetUniqueIdReadResult(uint8_t maxReadableUniqueIdLen, struct DBPL_EcuUniqueIdReadResult* const result);

/**
*This function is for BAL to know, which baudRate is standard, in Order to return to it, after wake up break is sent.
*
*
\return       baud rate\n
\b type       : uint16_t\n
\b param      : void\n
*
*/
uint16_t DBPL_uiGetStandardBaudRate(void);

#ifdef DBM_DBUSCAN
/**
 * \brief Gets the value of message repetitions (returns DBPL_ucRepetitions variable).
 * \note  This function is called only when DBus is used together with the DBusCAN chip.
 */
uint8_t DBPL_ucGetMsgRepetitions(void);

/**
 * \brief Returns DBPL_uiSendPowerMsgUserTrigger variable.
 * \note  This function is called only when DBus is used together with the DBusCAN chip.
 */
uint16_t DBPL_uiGetPowerMsgUserTrigger(void);
#endif // DBM_DBUSCAN

/**
Trigger sending of WakeupSentRequest.
\return
\b type       : bool\n
\b range      : Returns true, when neither PowerFail, PowerResurge, PowerMgmtWakeup, PowerMgmtWakeupComplete nor PowerMgmtNodeReset message
is being sent at the moment of call, otherwise false. Retry later, if false.
 */
bool DBPL_bSendWakeupSentRequest(void);

/**
Trigger sending of Power Fail Message.
\return
\b type       : bool\n
\b range      : Returns true, when neither WakeupSentRequest, PowerResurge, PowerMgmtWakeup, PowerMgmtWakeupComplete nor PowerMgmtNodeReset message
is being sent at the moment of call, otherwise false. Retry later, if false.
 */
bool DBPL_bSendPowerFailMsg(void);

/**
Trigger sending of Power Resurge Message.
\return
\b type       : bool\n
\b range      : Returns true, when neither WakeupSentRequest, PowerFail, PowerMgmtWakeup, PowerMgmtWakeupComplete nor PowerMgmtNodeReset
message is being sent at the moment of call, otherwise false. Retry later, if false.
 */
bool DBPL_bSendPowerResurgeMsg(void);

/**
Trigger sending of Power Management Wakeup Message.
\param ucTargetAddr
Target DBus address to which message should be sent.\n
\b type       : uint8_t\n
\b range      : 0..255\n
\return
\b type       : bool\n
\b range      : Returns true, when neither WakeupSentRequest, PowerFail, PowerResurge, PowerMgmtWakeupComplete nor PowerMgmtNodeReset
message is being sent at the moment of call, otherwise false. Retry later, if false.
 */
bool DBPL_bSendPowerMgmtWakeup(uint8_t ucTargetAddr);

/**
Trigger sending of Power Management Wakeup Complete Message.
\param ucTargetAddr
Target DBus address to which message should be sent.\n
\b type       : uint8_t\n
\b range      : 0..255\n
\return
\b type       : bool\n
\b range      : Returns true, when neither WakeupSentRequest, PowerFail, PowerResurge, PowerMgmtWakeup nor PowerMgmtNodeReset
message is being sent at the moment of call, otherwise false. Retry later, if false.
 */
bool DBPL_bSendPowerMgmtWakeupComplete(uint8_t ucTargetAddr);

/**
Trigger sending of Power Management Node Reset Message.
\param ucTargetAddr
Target DBus address to which message should be sent.\n
\b type       : uint8_t\n
\b range      : 0..255\n
\return
\b type       : bool\n
\b range      : Returns true, when neither WakeupSentRequest, PowerFail, PowerResurge, PowerMgmtWakeup nor PowerMgmtWakeupComplete
message is being sent at the moment of call, otherwise false. Retry later, if false.
 */
bool DBPL_bSendPowerMgmtNodeReset(uint8_t ucTargetAddr);

/**
 * \brief Information for other Dbus layers, whether WakeupSentRequest is being sent
 */
bool DBPL_bIsSendingWakeupSentRequest(void);

/**
 * \brief Information for other Dbus layers, whether Power Fail message is being sent
 */
bool DBPL_bIsSendingPowerFail(void);

/**
 * \brief Information for other Dbus layers, whether Power Resurge message is being sent
 */
bool DBPL_bIsSendingPowerResurge(void);

/**
 * \brief Information for other Dbus layers, whether Power Management Wakeup message is being sent
 */
bool DBPL_bIsSendingPowerMgmtWakeup(void);

/**
 * \brief Information for other Dbus layers, whether Power Management Wakeup Complete message is being sent
 */
bool DBPL_bIsSendingPowerMgmtWakeupComplete(void);

/**
 * \brief Information for other Dbus layers, whether Power Management Node Reset message is being sent
 */
bool DBPL_bIsSendingPowerMgmtNodeReset(void);

/**
 * \brief Information for other Dbus layers, whether WakeupSentRequest, PowerFail, PowerResurge, PowerMgmtWakeup,
 *        PowerMgmtWakeupComplete or PowerMgmtNodeReset message is being sent
 */
bool DBPL_bIsSendingPowerMsg(void);

/**
 * \brief Information for DLL layer, whether the received message is PowerFail
 */
bool DBPL_bIsReceivedMessageIdPowerFail(uint16_t msgId);

/* Functions, which will normally be implemented/modified in the dbuspresentationXS template */

/**
 This function, defined in the excess template, must be filled with reasonable content,
 depending on the wish of the application.

The application may wish to be notified before 
going offline - possibly to shut down different 
actuators, ignore different sensors...
*/
void DBPL_vGoOffline(void);

/**
* Interface, which is called when message GoOffline is received - and the node to update is this one. This interface can be used to notify the bootloader application, that after performing a reset, the bootloader mode should be entered - and the timeout can be set very high, as a flash programmer is expected (due to the reception of GoOffline message, which normally is received from the flash programmer). Possibly a timeout value of five seconds can be used in this case. A normal start (i.e. when no GoOflline was used prior to power on) can then use a very short timeout, e.g. 20 ms.
* Contained in dbuspresentationXS template.
*
\return \n
\b type       : void\n
*
*/
void DBPL_vEnterBootloaderMode(void);

/**
*This function, must be defined in the excess template, if the message HsiProtocolRequest is used. This interface is needed in the update mode.
*Contained in dbuspresentationXS template.
*
\return\n
\b type       : void\n
\b param      : data data_pointer\n dataLength number_of_bytes\n
*
*/
void DBPL_vHSI_DataIndication(void* const data, uint32_t data_length);

/**
 * \brief Returns the present bus state (Offline, or not).
 */
bool DBPL_bIsOfflineMode(void);

/**
 * \brief In dbuspresentationXS-Template called to inform the application, whenever Silent Mode has just been entered by Dbus2.2. As the Silent Mode is ALWAYS left by a reset on the microcontroller implementation, a callback for leaving Silent Mode is NOT needed there.
 */
void DBPL_vSilentModeHasBeenEntered(void);

/**
 * \brief Implementation in dbuspresentationXS-Template to inform user about received PowerFail Message.
 */
void DBPL_vPowerFailNotificationHasBeenReceived(void);

/**
 * \brief Implementation in dbuspresentationXS-Template to inform user about received PowerResurge Message.
 * \b param  :  sender Node address of sender.
 */
void DBPL_vPowerResurgeNotificationHasBeenReceived(uint8_t sender);

/**
 * \brief Implementation in dbuspresentationXS-Template to inform user, that the sending of a power message triggered by the application failed after all attempts.
 * \b param  :  uiMsgId Message identifier.
 */
void DBPL_vNotifyNonDeliverablePowerMessage(uint16_t msgId);

/**
 * \brief Called by Bal, when sending of service message was not successful. Calls DBPL_vStopRepeatServiceMsg, if all repetitions have been used.
 */
void DBPL_vDecrementRepeatServiceMsg(void);

/**
 * \brief Called by Bal, when sending of service message was successful.
 */
void DBPL_vStopRepeatServiceMsg(void);

/**
 This function gives the information, whether DLL_ConfigurationItem.ulWaitingTimeOffsetUs, which is define by the application, is to be added to the waiting time in case of collision/wrong ack.

\param uiUserCode
User Code of message.\n
\b type       : uint16_t\n
\b range      : 0..65535

\return true or false\n
\b type       : bool\n
*/
bool DBPL_bDoesAppSpecificRepetitionTimeoutApply(uint16_t uiUserCode);
#ifdef __cplusplus
}
#endif

#endif
