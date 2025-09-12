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
 *  \brief    Implementation of sending and repeating messages in DBAL.
 *
 *  \details  DBAL is the communication protocol which is used by symana. For
 *            detailed information, please read the md file which is included in the
 *            doc folder.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#define DBAL_USE_INSTANCE
#include "BshDBus2AppLayer_internal.h"
#include "BshDBus2AppLayer.h"
#include "BshDBus2AppLayerCross.h"
#include "BshDBus2AppLayer_debug.h"
#include "DBal_cfg.h"
#include "bal.h"
#include "dbuspresentation.h"
#include "dbusdll.h"
#include "bustypes.h"
#include "dbusmapping.h"
#include "timer/system_timer.h"
#include "LibDefines.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/* protocol definition:
* connection frame:
* Bytes:    1       1               1                 1
* Name:     sender  protocol type   connection type   protocol-version
*           ---------------------
*           generic frame header
*
* connection types:     enableRequest       - 0
*                       enableResponse      - 1
*                       disableRequest      - 2
*                       disableResponse     - 3
*                       pingRequest         - 4
*                       pingResponse        - 5
*                       tempEnableRequest   - 6
*                       tempEnableResponse  - 7
*                       tempDisableRequest  - 8
*                       tempDisableResponse - 9

* request/response frame (multiple dbal frames can be included one after the other):
* Bytes:    1       1                1            |    1           1                   2           2           n
* Name:     sender  protocol type    sequenceId   |    dbal-type   size(of payload)    serviceId   commandId   payload
*           ---------------------                      -----------------------------------------------------
*           generic frame header                       dbal header
*                                                      ----------------------------------------------------------------
*                                                      dbal frame
*                                    ----------------------------------------------------------------------------------
*                                    payload of generic frame
*/
#ifndef __cplusplus
#define DBAL_CAST_STRING const void* //!< Cast of sending Buffer to string for strlen. Different needs for C and C++.
#else
#define DBAL_CAST_STRING const char* //!< Cast of sending Buffer to string for strlen. Different needs for C and C++.
#endif /*__cplusplus*/

#ifdef DBAL_CROSS_CONNECTION
#ifndef __cplusplus
#define DBALCR_CAST_INSTANCE_TO_QUEUE void* //!< Cast of DBal communication instance when using queue. Different needs for C and C++.
#else
#define DBALCR_CAST_INSTANCE_TO_QUEUE const struct DBAL_Instance** const //!< Cast of DBal communication instance when using queue. Different needs for C and C++.
#endif /*__cplusplus*/
#endif /*DBAL_CROSS_CONNECTION*/

#ifdef DBAL_CROSS_CONNECTION
#define DBALCR_NO_CONST //!< For elements, that need to be const only, if no cross connection
#else
#define DBALCR_NO_CONST const //!< For elements, that need to be const only, if no cross connection
#endif
/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \struct DBAL_Event2Ack
 *
 * \brief   Describes the identifier for received events to acknowledge.
 *
 * \details If an event has been received twice in one Dbus2 message, only one ack has to be sent.
*/
struct DBAL_Event2Ack
{
    uint16_t ServiceId;//!< See DBal spec.
    uint16_t CommandId;//!< See DBal spec.
};

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

static bool DBAL_isConTransmitIndex(uint8_t MessageIndex);
static bool DBAL_isTempConMessage(uint8_t ConMsgType);
static enum DBAL_Dbus2FrameType DBAL_getDbus2FrameTypeFromDBalType(enum DBAL_MessageType DBalType);
static enum DBAL_MessageType DBAL_convertUint8ToDBalType(uint8_t Value);
static enum DBAL_MessageType DBAL_getResponseDBalType(enum DBAL_MessageType ReqDBalType);
static bool DBAL_isMsgOfDBalType2Repeat(enum DBAL_MessageType DBalType);
static bool DBAL_isMsgOfAckDBalType(enum DBAL_MessageType DBalType);
static void DBAL_resetEvents2Ack(void);
static bool DBAL_isEventAlreadyAcknowledged(uint16_t ServiceId, uint16_t CommandId);

static void DBAL_setCodeSectionFlag(struct DBAL_Instance* const Inst, uint8_t Mask);
static bool DBAL_checkTaskCodeSectionFlag(const struct DBAL_Instance* const Inst);
static void DBAL_executeSetNonTaskCodeSections(struct DBAL_Instance* const Inst);

static void DBAL_writeReqRespInDbus2Buffer(struct DBAL_Instance* const Inst, uint8_t DataLen, uint8_t* const Bytes);
static bool DBAL_check4NextMsgs2SendAndTrigger(const struct DBAL_Instance* const Inst, uint8_t TxIndex);
#ifdef DBAL_CROSS_CONNECTION
static void DBALCR_triggerNextConnectMsgInstanceTx(const struct DBAL_Instance* const Inst);
static void DBALCR_triggerNextReqRespMsgInstanceTx(const struct DBAL_Instance* const Inst);
static bool DBALCR_isCmdOrQuery2BeRepeated(const struct DBAL_Instance* const Inst);
#endif/*DBAL_CROSS_CONNECTION*/
static bool DBAL_isTempConActive(const struct DBAL_Instance* const Inst, uint8_t TempCommPartner);
static bool DBAL_isDisablePending(const struct DBAL_Instance* const Inst);
static void DBAL_clearRetryCounters(struct DBAL_Instance* const Inst);
static void DBAL_clearMsgs2Repeat(struct DBAL_Instance* const Inst);
static void DBAL_checkAndFireMsgs2RepeatWhenClearing(const struct DBAL_Instance* const Inst);
static void DBAL_closeGapBetweenMsgs2Repeat(struct DBAL_Instance* const Inst, uint8_t StartIndex);
static enum DBAL_Msgs2RepeatStatus DBAL_organizeMsgs2Repeat(struct DBAL_Instance* const Inst);
static void DBAL_handleConMsgTxFail(struct DBAL_Instance* const Inst);
static void DBAL_handleReqRespMsgTxFail(struct DBAL_Instance* const Inst);

static void DBAL_msgTimerAction(struct DBAL_Instance* const Inst);
/*Callback function definition*/
static int32_t DBAL_conMsgTimerCallback(void *Obj, uint32_t Flags, int32_t Data);
static int32_t DBAL_msgTimerCallback(void *Obj, uint32_t Flags, int32_t Data);

static void DBAL_handleConMsg(struct DBAL_Instance* const Inst, uint8_t ConMsgType);
static void DBAL_handleTempConMsg(struct DBAL_Instance* const Inst, uint8_t TempCommPartner, uint8_t ConMsgType);
static bool DBAL_callServiceCallback
(
    const struct DBAL_Instance* const Inst,
    enum DBAL_MessageType DBalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DBalPayloadLen
);

static void DBAL_look4MsgReception(struct DBAL_Instance* const Inst, const uint8_t* const Bytes, uint8_t DataLen);
static void DBAL_look4AckMsgReception(struct DBAL_Instance* const Inst, const uint8_t* const Bytes, uint8_t DataLen);

static bool DBAL_isReceivedConMsgCorrupt
(
    const struct DBAL_Instance* const Inst,
    const uint8_t* const Bytes,
    uint8_t DataLen
);
static bool DBAL_isReceivedConMsgTemp
(
    const struct DBAL_Instance* const Inst,
    const uint8_t* const Bytes,
    uint8_t DataLen
);
static bool DBAL_doesReceivedConMsgInitCommPartner
(
    const struct DBAL_Instance* const Inst,
    const uint8_t* const Bytes,
    uint8_t DataLen
);
static bool DBAL_isReceivedReqRespMsgCorrupt
(
    const struct DBAL_Instance* const Inst,
    const uint8_t* const Bytes,
    uint8_t DataLen
);
static bool DBAL_isReceivedReqRespMsg2BeIgnored(const struct DBAL_Instance* const Inst, const uint8_t* const Bytes);

static void DBAL_conMsgTxPostRepeatTrigger(struct DBAL_Instance* const Inst);
static void DBAL_reqRespTxPostAction(const struct DBAL_Instance* const Inst);
#ifdef DBAL_CROSS_CONNECTION
static void DBALCR_reqRespTxPostAction(const struct DBAL_Instance* const Inst);
#endif

static void DbalTransmit_appLayerConnection(uint8_t DataLen, uint8_t* Bytes);
static void DbalTransmitPost_appLayerConnection(void);
#ifdef DBAL_CROSS_CONNECTION
static void DbalTransmitCr_appLayerConnection(uint8_t DataLen, uint8_t* Bytes);
static void DbalTransmitCrPost_appLayerConnection(void);
#endif/*DBAL_CROSS_CONNECTION*/
static void DbalReceive_appLayerConnection(uint8_t DataLen, uint8_t* Bytes);

static void DbalTransmit_appLayerRequest(uint8_t DataLen, uint8_t* Bytes);
static void DbalTransmitPost_appLayerRequest(void);
#ifdef DBAL_CROSS_CONNECTION
static void DbalTransmitCr_appLayerRequest(uint8_t DataLen, uint8_t* Bytes);
static void DbalTransmitCrPost_appLayerRequest(void);
#endif/*DBAL_CROSS_CONNECTION*/
static void DbalReceive_appLayerRequest(uint8_t DataLen, uint8_t* Bytes);

static void DbalTransmit_appLayerResponse(uint8_t DataLen, uint8_t* Bytes);
static void DbalTransmitPost_appLayerResponse(void);
#ifdef DBAL_CROSS_CONNECTION
static void DbalTransmitCr_appLayerResponse(uint8_t DataLen, uint8_t* Bytes);
static void DbalTransmitCrPost_appLayerResponse(void);
#endif/*DBAL_CROSS_CONNECTION*/
static void DbalReceive_appLayerResponse(uint8_t DataLen, uint8_t* Bytes);

static void DBAL_prepareGenericFrameHeader(uint8_t* const Buffer);
static void DBAL_storePayloadToTransmitBuffer
(
    uint8_t* const Buffer,
    uint8_t From,
    const uint8_t* const Bytes,
    uint8_t DataLen
);
static void DBAL_clearIoTransmitBuffer(struct DBAL_Instance* const Inst);
static void DBAL_clearIoTxRepeatBuffer(const struct DBAL_Instance* const Inst);
static void DBAL_clearBothIoTxBuffers(struct DBAL_Instance* const Inst);
static bool DBAL_isIoTransmitBufferEmpty(const struct DBAL_Instance* const Inst);
static bool DBAL_isIoTxRepeatBufferEmpty(const struct DBAL_Instance* const Inst);
static bool DBAL_setCrMsgPtrs(DBALCR_NO_CONST struct DBAL_Instance* const Inst, uint8_t TxIndex);
static bool DBAL_isReqQueued4Sending(const struct DBAL_Instance* const Inst);
#ifdef DBAL_CROSS_CONNECTION
static bool DBAL_areConTxFlagsSet(const struct DBAL_Instance* const Inst);
#endif
static bool DBAL_areReqRespTxFlagsSet(const struct DBAL_Instance* const Inst);
static void DBAL_prepareTxEntry(const struct DBAL_Instance* const Inst, uint8_t TxIndex, uint8_t DataLen);
static uint8_t DBAL_getTxIndex(const struct DBAL_Instance* const Inst, enum DBAL_Dbus2FrameType FrameType);
static void DBAL_sendConnectionMessage(struct DBAL_Instance* const Inst, enum DBAL_ConnectionMessageType ConMessageType);
static bool DBAL_isMsg2RepeatSaved
(
    const struct DBAL_Instance* const Inst,
    enum DBAL_MessageType DBalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DataLen,
    uint8_t* const FreeIndex
);
static bool DBAL_saveMsg2Repeat
(
    struct DBAL_Instance* const Inst,
    enum DBAL_MessageType DBalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DataLen,
    uint8_t FreeIndex
);
static void DBAL_prepareHeaderAndSeqId(struct DBAL_Instance* const Inst, uint8_t Repetition);
static bool DBAL_appendDBalFrame2ReqRespFrame
(
    struct DBAL_Instance* const Inst,
    uint8_t From,
    enum DBAL_MessageType DBalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DataLen,
    uint8_t Repetition
);
static bool DBAL_ioDbusHandler_send
(
    struct DBAL_Instance* const Inst,
    enum DBAL_MessageType DbalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DataLen,
    uint8_t Repetition
);
static uint8_t DBAL_getLastSendingStatus(const struct DBAL_Instance* const Inst, uint8_t MsgIndex);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
const TbusReceiveObject DBAL_DBUS_Handler_RxObject[] =
{
    {{0x00U, 0, DBAL_MESSAGE_APP_LAYER_CONNECTION}, DbalReceive_appLayerConnection},
    {{0x00U, 0, DBAL_MESSAGE_APP_LAYER_REQUEST}, DbalReceive_appLayerRequest},
    {{0xFF,  0, DBAL_MESSAGE_APP_LAYER_RESPONSE}, DbalReceive_appLayerResponse}
    /* 0xFF marks final entry */
};

TbusTransmitObject DBAL_DBUS_Handler_TxObject[] =
{
    /*messageID length = 2, message length => payload + 2*/
     {{0x05, 0, DBAL_MESSAGE_APP_LAYER_CONNECTION}, 0x03, DbalTransmit_appLayerConnection, DbalTransmitPost_appLayerConnection}
    ,{{0x05, 0, DBAL_MESSAGE_APP_LAYER_REQUEST},    0x03, DbalTransmit_appLayerRequest, DbalTransmitPost_appLayerRequest}
    ,{{0x05, 0, DBAL_MESSAGE_APP_LAYER_RESPONSE},   0x03, DbalTransmit_appLayerResponse, DbalTransmitPost_appLayerResponse}
#ifdef DBAL_CROSS_CONNECTION
    ,{{0x05, 0, DBAL_MESSAGE_APP_LAYER_CONNECTION}, 0x03, DbalTransmitCr_appLayerConnection, DbalTransmitCrPost_appLayerConnection}
    ,{{0x05, 0, DBAL_MESSAGE_APP_LAYER_REQUEST},    0x03, DbalTransmitCr_appLayerRequest, DbalTransmitCrPost_appLayerRequest}
    ,{{0x05, 0, DBAL_MESSAGE_APP_LAYER_RESPONSE},   0x03, DbalTransmitCr_appLayerResponse, DbalTransmitCrPost_appLayerResponse}
#endif/*DBAL_CROSS_CONNECTION*/
};

const uint8_t DBAL_DBUS_Handler_NumberOfTxObjects = (uint8_t)(sizeof(DBAL_DBUS_Handler_TxObject) / sizeof(TbusTransmitObject));
uint8_t DBAL_DBUS_Handler_TxFlags[1];
/*lint -e9003 Block scope only possible, if no cross connection*/
static DBALCR_NO_CONST struct DBAL_Instance* DBALCR_ConInst = NULL;//!< Cross connection (user) instance currently sending connection message.
static DBALCR_NO_CONST struct DBAL_Instance* DBALCR_ReqRespInst = NULL;//!< Cross connection (user) instance currently sending request/response message.
/*lint -save -restore*/
static uint8_t DBAL_CountOfEvents2Ack = 0U;//!< Bookkeeping of different events, for which to send an ack. Always for current communication instance.
/*Axivion Next Line MisraC2012-9.3 : Implicit initialization needed for array of flexible length.*/
static struct DBAL_Event2Ack DBAL_Events2Ack[DBAL_MAX_MSGS2REPEAT] = {{0U}};//!< Bookkeeping of different events, for which to send an ack. Always for current communication instance.
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*
 * function implementations of BshDbus2AppLayer_internal.h
 */
void DBAL_appLayerDBus2Init(struct DBAL_Instance* const Inst)
{
    (void)STIM_InitCallback(&Inst->ConMsgCbInst, DBAL_conMsgTimerCallback, Inst, STIM_STATUS_TRIGGERED);
    (void)STIM_InitTimer(&Inst->ConMsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, (bool)false, &Inst->ConMsgCbInst);
    (void)STIM_InitCallback(&Inst->MsgCbInst, DBAL_msgTimerCallback, Inst, STIM_STATUS_TRIGGERED);
    (void)STIM_InitTimer(&Inst->MsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_RESPONSE_TIME_MS, STIM_MODE_SINGLE, (bool)false, &Inst->MsgCbInst);
}


/*functions used for statemachine events*/
void DBAL_sendCommEnableRequest(struct DBAL_Instance* const Inst)
{
    DBAL_sendConnectionMessage(Inst, DBAL_CON_ENABLE_REQUEST);
}

void DBAL_sendCommEnableResponse(struct DBAL_Instance* const Inst)
{
    DBAL_sendConnectionMessage(Inst, DBAL_CON_ENABLE_RESPONSE);
}

void DBAL_sendCommDisableRequest(struct DBAL_Instance* const Inst)
{
    DBAL_sendConnectionMessage(Inst, DBAL_CON_DISABLE_REQUEST);
}

void DBAL_sendCommDisableResponse(struct DBAL_Instance* const Inst)
{
    DBAL_sendConnectionMessage(Inst, DBAL_CON_DISABLE_RESPONSE);
}

void DBAL_sendCommPingRequest(struct DBAL_Instance* const Inst)
{
    DBAL_sendConnectionMessage(Inst, DBAL_CON_PING_REQUEST);/*IoConnectionHandling making sure, this is only called in the right situation.*/
}

void DBAL_sendCommPingResponse(struct DBAL_Instance* const Inst)
{
    if
    (
        (DBAL_getCommStateByInstance(Inst) == DBAL_COMMSTATE_READY) &&
        (DBAL_getConnectionStateByInstance(Inst) == DBAL_CONNECTIONSTATE_CONNECTED)
    )
    {
        DBAL_sendConnectionMessage(Inst, DBAL_CON_PING_RESPONSE);/*Only respond, if connection is opened.*/
    }
}

void DBAL_sendCommTempEnableResponse(struct DBAL_Instance* const Inst)
{
    DBAL_sendConnectionMessage(Inst, DBAL_CON_TEMP_ENABLE_RESPONSE);
}

void DBAL_sendCommTempDisableResponse(struct DBAL_Instance* const Inst)
{
    DBAL_sendConnectionMessage(Inst, DBAL_CON_TEMP_DISABLE_RESPONSE);
}

void DBAL_nonDeliverableMessage(uchar MessageIndex)
{
    if(MessageIndex < DBAL_MSG_INDEX_COUNT)
    {
        DBM_DISABLE_INT();
        uint8_t TargetAddress = DBAL_DBUS_Handler_TxObject[MessageIndex].tBusIdentifier.ucTargetAddress;
        struct DBAL_Instance* const Inst = DBALIN_getInstanceByAddress(TargetAddress);

        if(Inst != NULL)
        {
            if((Inst->SendRetryCounter[MessageIndex] < (uint8_t)DBAL_DBUS_RETRY_MAX))
            {
                Inst->SendRetryCounter[MessageIndex]++;

                if(DBAL_getLastSendingStatus(Inst, MessageIndex) == DLL_ACK_NOT_RECEIVED)
                {
                    if(DBAL_isConTransmitIndex(MessageIndex) == true)
                    {
                        if(Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE] == (uint8_t)DBAL_CON_ENABLE_REQUEST)
                        {/*Send break, if repeating connection enable request and no ack was received.*/
                            BAL_vSendWakeupBreak();
                            STIM_ReloadTimer(&Inst->ConMsgTimer, (STIM_Time_t)DBAL_DBUS_RECOVERY_TIME_MS);
                            STIM_EnableTimer(&Inst->ConMsgTimer);
                            DBAL_INFO("Resent Break, Addr %x", Inst->DBUS_ComPartner);
                        }
                        else
                        {/*If other connection message, end connection, if no ack.*/
                            DBAL_WARNING("No Ack to connection message of type %d. Addr %x", Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE], Inst->DBUS_ComPartner);
                            DBAL_handleConMsgTxFail(Inst);
                        }
                    }
                    else
                    {/*No connection message: End connection, if no ack.*/
                        DBAL_handleReqRespMsgTxFail(Inst);
                        DBAL_INFO("No Ack, TxIndex %u, Addr %x", MessageIndex, Inst->DBUS_ComPartner);
                    }
                }
                else
                {/*Just repeat, if other error, than missing ack.*/
                    BAL_vTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, MessageIndex);
                    DBAL_INFO("Set Tx Flag again. TxIndex %u, Addr %x", MessageIndex, Inst->DBUS_ComPartner);
                }
            }
            else
            {/*Give up, if all repetitions used.*/
                if(DBAL_isConTransmitIndex(MessageIndex) == true)
                {
                    DBAL_WARNING("Connection handling failed. MsgType %d, Addr %x", Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE], Inst->DBUS_ComPartner);
                    DBAL_handleConMsgTxFail(Inst);
                }
                else
                {
                    DBAL_handleReqRespMsgTxFail(Inst);
                    DBAL_WARNING("Sending message failed. TxIndex %u, Addr %x", MessageIndex, Inst->DBUS_ComPartner);
                }
            }
        }
        DBM_ENABLE_INT();
    }
}

void DBAL_setLastSendingStatus(uint8_t TargetAddress, uint8_t Status, uint8_t MsgIndex)
{
    struct DBAL_Instance* const Inst = DBALIN_getInstanceByAddress(TargetAddress);

    if((Inst != NULL) && (MsgIndex < DBAL_DBUS_Handler_NumberOfTxObjects))
    {
        Inst->LastSendingStatus[MsgIndex] = Status;
    }
}

bool DBAL_isNodeToBeWokenUp(uint8_t TargetAddress)
{
    struct DBAL_Instance* const Inst = DBALIN_getInstanceByAddress(TargetAddress);

    if(Inst != NULL)
    {
        DBM_DISABLE_INT();
        if
        (
            (Inst->ConMsgTimer.enabled == true) &&
            (Inst->ConMsgTimer.targetTime_ms == (STIM_Time_t)DBAL_DBUS_RECOVERY_TIME_MS)
        ) // Timer running after first Enable Request message was sent with no ack and then break was sent...
        {
            STIM_DisableTimer(&Inst->ConMsgTimer);
            DBAL_sendCommEnableRequest(Inst);
        }
        DBM_ENABLE_INT();
    }

    return false;
}

/*
 * Provided functions for generic io interface from BshDBus2AppLayer.h
 */
bool DBAL_sendCmdResponse(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        if(DBAL_isDisablePending(Inst) == false)
        {
            DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
            RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_CMD_ACK, ServiceId, CommandId, Bytes, DataLen, 0U);
            DBAL_executeSetNonTaskCodeSections(Inst);
        }
        else
        {
            DBAL_INFO("Disable Pending; ServiceId %d, CommandId %d", ServiceId, CommandId);
        }
    }
    else
    {
        DBAL_ERROR("Fail. Uninitialized?");/*Should not ever be called.*/
    }

    return RetVal;
}

bool DBAL_sendQueryResponse(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        if(DBAL_isDisablePending(Inst) == false)
        {
            DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
            RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_QUERY_ACK, ServiceId, CommandId, Bytes, DataLen, 0U);
            DBAL_executeSetNonTaskCodeSections(Inst);
        }
        else
        {
            DBAL_INFO("Disable Pending; ServiceId %d, CommandId %d", ServiceId, CommandId);
        }
    }
    else
    {
        DBAL_ERROR("Fail. Uninitialized?");/*Should not ever be called.*/
    }

    return RetVal;
}

bool DBAL_sendEvent(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        if(DBAL_isDisablePending(Inst) == false)
        {
            DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
            RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_EVENT, ServiceId, CommandId, Bytes, DataLen, 0U);
            DBAL_executeSetNonTaskCodeSections(Inst);
        }
        else
        {
            DBAL_INFO("Disable Pending; ServiceId %d, CommandId %d", ServiceId, CommandId);
        }
    }
    else
    {
        DBAL_ERROR("Fail. Uninitialized?");/*Should not ever be called.*/
    }

    return RetVal;
}

#ifdef DBAL_CROSS_CONNECTION
/*
 * Provided functions for generic io interface from BshDBus2AppLayerCross.h
 */
/*client*/
bool DBALCR_sendCmd(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if
    (
        (Inst != NULL) &&
        (DBALCR_isCmdOrQuery2BeRepeated(Inst) == false) &&
        (DBALIN_getCrParticipantTypeByIndex(Index) == DBALCR_PART_TYPE_CLIENT) &&
        (DBAL_isDisablePending(Inst) == false) /*This condition and the one two lines above must have same result.*/
    )
    {
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
        RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_CMD, ServiceId, CommandId, Bytes, DataLen, 0U);
        DBAL_executeSetNonTaskCodeSections(Inst);
    }
    else if
    (
        (Inst == NULL) ||
        (DBALIN_getCrParticipantTypeByIndex(Index) != DBALCR_PART_TYPE_CLIENT)
    )
    {
        DBAL_WARNING("Fail. Inst %p. Index %d. ServiceId 0x%x. CommandId 0x%x", Inst, Index, ServiceId, CommandId);/*Should not ever be called.*/
    }
    else
    {
        /*Failed normally. Probably Cmd or Query repeated.*/
    }

    return RetVal;
}

bool DBALCR_sendQuery(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const  Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if
    (
        (Inst != NULL) &&
        (DBALCR_isCmdOrQuery2BeRepeated(Inst) == false) &&
        (DBALIN_getCrParticipantTypeByIndex(Index) == DBALCR_PART_TYPE_CLIENT) &&
        (DBAL_isDisablePending(Inst) == false) /*This condition and the one two lines above must have same result.*/
    )
    {
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
        RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_QUERY, ServiceId, CommandId, Bytes, DataLen, 0U);
        DBAL_executeSetNonTaskCodeSections(Inst);
    }
    else if
    (
        (Inst == NULL) ||
        (DBALIN_getCrParticipantTypeByIndex(Index) != DBALCR_PART_TYPE_CLIENT)
    )
    {
        DBAL_WARNING("Fail. Inst %p. Index %d. ServiceId 0x%x. CommandId 0x%x", Inst, Index, ServiceId, CommandId);/*Should not ever be called.*/
    }
    else
    {
        /*Failed normally. Probably Cmd or Query repeated.*/
    }

    return RetVal;
}
/*server*/
bool DBALCR_sendCmdResponse(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if
    (
        (Inst != NULL) &&
        (DBALCR_isCmdOrQuery2BeRepeated(Inst) == false) &&
        (DBALIN_getCrParticipantTypeByIndex(Index) == DBALCR_PART_TYPE_SERVER)
    )
    {
        if(DBAL_isDisablePending(Inst) == false)
        {
            DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
            RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_CMD_ACK, ServiceId, CommandId, Bytes, DataLen, 0U);
            DBAL_executeSetNonTaskCodeSections(Inst);
        }
        else
        {
            DBAL_INFO("Disable Pending; ServiceId %d, CommandId %d", ServiceId, CommandId);
        }
    }
    else
    {
        DBAL_WARNING("Fail. Inst %p. Index %d. ServiceId 0x%x. CommandId 0x%x", Inst, Index, ServiceId, CommandId);/*Should not ever be called.*/
    }

    return RetVal;
}

bool DBALCR_sendQueryResponse(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const  Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if
    (
        (Inst != NULL) &&
        (DBALCR_isCmdOrQuery2BeRepeated(Inst) == false) &&
        (DBALIN_getCrParticipantTypeByIndex(Index)  == DBALCR_PART_TYPE_SERVER)
    )
    {
        if(DBAL_isDisablePending(Inst) == false)
        {
            DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
            RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_QUERY_ACK, ServiceId, CommandId, Bytes, DataLen, 0U);
            DBAL_executeSetNonTaskCodeSections(Inst);
        }
        else
        {
            DBAL_INFO("Disable Pending; ServiceId %d, CommandId %d", ServiceId, CommandId);
        }
    }
    else
    {
        DBAL_WARNING("Fail. Inst %p. Index %d. ServiceId 0x%x. CommandId 0x%x", Inst, Index, ServiceId, CommandId);/*Should not ever be called.*/
    }

    return RetVal;
}

bool DBALCR_sendEvent(uint8_t Index, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if
    (
        (Inst != NULL) &&
        (DBALCR_isCmdOrQuery2BeRepeated(Inst) == false) &&
        (DBALIN_getCrParticipantTypeByIndex(Index) == DBALCR_PART_TYPE_SERVER)
    )
    {
        if(DBAL_isDisablePending(Inst) == false)
        {
            DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
            RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_EVENT, ServiceId, CommandId, Bytes, DataLen, 0U);
            DBAL_executeSetNonTaskCodeSections(Inst);
        }
        else
        {
            DBAL_INFO("Disable Pending; ServiceId %d, CommandId %d", ServiceId, CommandId);
        }
    }
    else
    {
        DBAL_WARNING("Fail. Inst %p. Index %d. ServiceId 0x%x. CommandId 0x%x", Inst, Index, ServiceId, CommandId);/*Should not ever be called.*/
    }

    return RetVal;
}
#endif/*DBAL_CROSS_CONNECTION*/

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/** \brief             Gives the info, whether it is a connection message, that is being sent.
 *
 * \param MessageIndex Index of transmitted message in DBAL_DBUS_Handler_TxObject.
 *
 * \return             True, if it is a connection message, else false.
*/
static bool DBAL_isConTransmitIndex(uint8_t MessageIndex)
{
    return  (
                (MessageIndex == DBAL_MSG_INDEX_APP_LAYER_CONNECTION)
#ifdef DBAL_CROSS_CONNECTION
                || (MessageIndex == DBAL_MSG_INDEX_APP_LAYER_CROSS_CON)
#endif/*DBAL_CROSS_CONNECTION*/
            ) ? true : false;
}

/** \brief           Give info, whether it is a connection message for handling of temporary connections.
 *
 * \param ConMsgType Type of connection message; see DBal spec.
 *
 * \return           True, if temporary connection messages, else false.
 */
static bool DBAL_isTempConMessage(uint8_t ConMsgType)
{
    return ((ConMsgType >= (uint8_t)DBAL_CON_TEMP_ENABLE_REQUEST) && (ConMsgType < (uint8_t)DBAL_CON_MSG_TYPE_COUNT)) ? true : false;
}

/** \brief             Gives the info, inside which Dbus2 frame a frame of the given DBal type should be sent.
 *
 * \param DBalType     Type of DBal frame; see DBal spec.
 *
 * \return             DBAL_DBUS2_FRAME_TYPE_REQ, DBAL_DBUS2_FRAME_TYPE_RESP, DBAL_DBUS2_FRAME_TYPE_UNKNOWN
*/
static enum DBAL_Dbus2FrameType DBAL_getDbus2FrameTypeFromDBalType(enum DBAL_MessageType DBalType)
{
    enum DBAL_Dbus2FrameType RetVal = DBAL_DBUS2_FRAME_TYPE_UNKNOWN;

    switch(DBalType)
    {
        case DBAL_TYPE_CMD:
        case DBAL_TYPE_QUERY:
            RetVal = DBAL_DBUS2_FRAME_TYPE_REQ;
            break;
        case DBAL_TYPE_CMD_ACK:
        case DBAL_TYPE_QUERY_ACK:
        case DBAL_TYPE_EVENT:
        case DBAL_TYPE_EVENT_ACK:
            RetVal = DBAL_DBUS2_FRAME_TYPE_RESP;
            break;
        case DBAL_TYPE_UNKNOWN:
        default:
            /*Stay with result unknown.*/
            break;
    }

    return RetVal;
}

/** \brief       Translation between uint8_t received on Dbus2 and according DBal type.
 *
 * \param Value  Unsigned Integer received.
 *
 * \return       DBAL_TYPE_UNKNOWN if received value is not valid,
 *               otherwise DBAL_TYPE_CMD, DBAL_TYPE_CMD_ACK, DBAL_TYPE_QUERY,
 *                         DBAL_TYPE_QUERY_ACK, DBAL_TYPE_EVENT, DBAL_TYPE_EVENT_ACK
*/
static enum DBAL_MessageType DBAL_convertUint8ToDBalType(uint8_t Value)
{
    enum DBAL_MessageType RetVal;

    switch (Value)
    {
        case (uint8_t)DBAL_TYPE_CMD:
                RetVal = DBAL_TYPE_CMD;
            break;
        case (uint8_t)DBAL_TYPE_CMD_ACK:
                RetVal = DBAL_TYPE_CMD_ACK;
            break;
        case (uint8_t)DBAL_TYPE_QUERY:
                RetVal = DBAL_TYPE_QUERY;
            break;
        case (uint8_t)DBAL_TYPE_QUERY_ACK:
                RetVal = DBAL_TYPE_QUERY_ACK;
            break;
        case (uint8_t)DBAL_TYPE_EVENT:
                RetVal = DBAL_TYPE_EVENT;
            break;
        case (uint8_t)DBAL_TYPE_EVENT_ACK:
                RetVal = DBAL_TYPE_EVENT_ACK;
            break;
        case (uint8_t)DBAL_TYPE_UNKNOWN:
        default:
            RetVal = DBAL_TYPE_UNKNOWN;
            break;
    }

    return RetVal;
}

/** \brief             Translation between "request" DBal type Command/Query/Event and the corresponding response.
 *
 * \param ReqDBalType  DBal type of message, that will be repeated, if no response.
 *
 * \return             DBAL_TYPE_UNKNOWN if value is "response" already or invalid,
 *                     otherwise DBAL_TYPE_CMD_ACK, DBAL_TYPE_QUERY_ACK, DBAL_TYPE_EVENT_ACK
*/
static enum DBAL_MessageType DBAL_getResponseDBalType(enum DBAL_MessageType ReqDBalType)
{
    enum DBAL_MessageType RetVal;

    switch(ReqDBalType)
    {
        case DBAL_TYPE_CMD:
            RetVal = DBAL_TYPE_CMD_ACK;
            break;
        case DBAL_TYPE_QUERY:
            RetVal = DBAL_TYPE_QUERY_ACK;
            break;
        case DBAL_TYPE_EVENT:
            RetVal = DBAL_TYPE_EVENT_ACK;
            break;
        case DBAL_TYPE_CMD_ACK:
        case DBAL_TYPE_QUERY_ACK:
        case DBAL_TYPE_EVENT_ACK:
        case DBAL_TYPE_UNKNOWN:
        default:
            RetVal = DBAL_TYPE_UNKNOWN;
            break;
    }

    return RetVal;
}

/** \brief         Gives info, whether DBal frame is of type for which a response is expected.
 *
 * \param DBalType Type of DBal frame.
 *
 * \return         True, if a response is expected to the DBal frame.
*/
static bool DBAL_isMsgOfDBalType2Repeat(enum DBAL_MessageType DBalType)
{
    return ((DBalType == DBAL_TYPE_CMD) || (DBalType == DBAL_TYPE_QUERY) || (DBalType == DBAL_TYPE_EVENT)) ? true : false;
}

/** \brief         Gives info, whether DBal frame is of type which is a response to a commad/query/event.
 *
 * \param DBalType Type of DBal frame.
 *
 * \return         True, if DBal frame is a response.
*/
static bool DBAL_isMsgOfAckDBalType(enum DBAL_MessageType DBalType)
{
    return ((DBalType == DBAL_TYPE_CMD_ACK) || (DBalType == DBAL_TYPE_QUERY_ACK) || (DBalType == DBAL_TYPE_EVENT_ACK)) ? true : false;
}

/** \brief Clears list of events, that must be acknowledged.
*/
static void DBAL_resetEvents2Ack(void)
{
    DBAL_CountOfEvents2Ack = 0;
    (void)memset(DBAL_Events2Ack, 0 , sizeof(DBAL_Events2Ack));/*lint !e920 cast ok in this case we do not need pointer*/
}

/** \brief           Checks, whether an event has already been acknowledged in the frame of a Dbus2 message,
 *                   to avoid double acknowledges.
 *
 * \param ServiceId  See DBal spec for meaning.
 * \param CommandId  See DBal spec for meaning.
 *
 * \return           Returns true, if there has been an event with the same serviceId and commandId
 *                   in the Dbus2 message, that is currently processed.
*/
static bool DBAL_isEventAlreadyAcknowledged(uint16_t ServiceId, uint16_t CommandId)
{
    bool RetVal = false;

    for(uint8_t i = 0; i< DBAL_CountOfEvents2Ack; i++)
    {
        if
        (
            (DBAL_Events2Ack[i].ServiceId == ServiceId) &&
            (DBAL_Events2Ack[i].CommandId == CommandId)
        )
        {
            RetVal = true;
            break;
        }
    }

    if((DBAL_CountOfEvents2Ack < (uint8_t)DBAL_MAX_MSGS2REPEAT) && (RetVal == false))
    {
        DBAL_Events2Ack[DBAL_CountOfEvents2Ack].ServiceId = ServiceId;
        DBAL_Events2Ack[DBAL_CountOfEvents2Ack].CommandId = CommandId;
        DBAL_CountOfEvents2Ack++;
    }

    return RetVal;
}

/** \brief     Sets flag indicating execution of critical code section.
 *
 * \param Inst DBal communication instance
 * \param Mask Bit mask
 */
static void DBAL_setCodeSectionFlag(struct DBAL_Instance* const Inst, uint8_t Mask)
{
    Inst->CodeSectionBitMask |= Mask;
}

/** \brief Checks, whether task part of critical sections is being executed.
 *
 * \return True, if task section is being run, otherwise false.
 */
static bool DBAL_checkTaskCodeSectionFlag(const struct DBAL_Instance* const Inst)
{
    return ((Inst->CodeSectionBitMask & DBAL_CODE_SEC_TASK) != 0U) ? true : false;
}

/** \brief     Executes critical interrupt code sections and resets all flags at the end.
 *
 * \param Inst DBal communication instance
 */
static void DBAL_executeSetNonTaskCodeSections(struct DBAL_Instance* const Inst)
{
    DBM_DISABLE_INT();
    if((Inst->CodeSectionBitMask & DBAL_CODE_SEC_REQ_RESP_POST) != 0U)
    {
        DBAL_reqRespTxPostAction(Inst);
    }

#ifdef DBAL_CROSS_CONNECTION
    if((Inst->CodeSectionBitMask & DBAL_CODE_SEC_CR_REQ_RESP_POST) != 0U)
    {
        DBALCR_reqRespTxPostAction(Inst);
    }
#endif

    if((Inst->CodeSectionBitMask & DBAL_CODE_SEC_REP_TIMER) != 0U)
    {
        DBAL_msgTimerAction(Inst);
    }

    Inst->CodeSectionBitMask = 0;
    DBM_ENABLE_INT();
}

/** \brief        Writes a DBal request/response into the Dbus2 Tx buffer before transmission.
 *
 * \details       Checks, whether it is the first attempt and if so, also fills DBal buffer for Dbus2 repetitions.
 *
 * \param Inst    DBal communication instance
 * \param DataLen Length of Data
 * \param Bytes   Pointer to Dbus2Buffer
 */
static void DBAL_writeReqRespInDbus2Buffer(struct DBAL_Instance* const Inst, uint8_t DataLen, uint8_t* const Bytes)
{
    if((DataLen != Inst->TransmitDataLen) && (DataLen != Inst->TxRepeat->TxRepeatLen))
    {
        DBAL_ERROR("Saved TX lengths in Dbus2 and Tx Buffer unaligned!!!");/*Must not be ever called!*/
    }
    else
    {
        if(DBAL_isIoTxRepeatBufferEmpty(Inst) == true)
        {
            memcpy(Inst->TxRepeat->TxRepeatBuffer, Inst->TransmitBuffer, Inst->TransmitDataLen);
            Inst->TxRepeat->TxRepeatLen = Inst->TransmitDataLen;
            DBAL_clearIoTransmitBuffer(Inst);
        }
        memcpy(Bytes, Inst->TxRepeat->TxRepeatBuffer, Inst->TxRepeat->TxRepeatLen);
    }
}

/** \brief        From Dbus2 confirmation function sees, if there is anything more to send.
 *
 * \details       Triggers sending of next messages.
 *
 * \param Inst    DBal communication instance.
 * \param TxIndex Index of Dbus2 message in DBAL_DBUS_Handler_TxObject
 * \return        True, if sending of more messages was triggered, else false.
 */
static bool DBAL_check4NextMsgs2SendAndTrigger(const struct DBAL_Instance* const Inst, uint8_t TxIndex)
{
    bool RetVal = false;

    if(DBAL_isIoTransmitBufferEmpty(Inst) == false)
    {
        DBAL_prepareTxEntry(Inst, TxIndex, Inst->TransmitDataLen);
        BAL_vTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, TxIndex);
        RetVal = true;
    }

    return RetVal;
}

#ifdef DBAL_CROSS_CONNECTION
/** \brief     Triggers transmission of DBal Dbus2 connection frames for next cross connection.
 *
 * \details    Checks, whether it is a DBal communication instance that has just finished transmission,
 *             and if so, starts it for the next one.
 *
 * \param Inst DBal communication instance
 */
static void DBALCR_triggerNextConnectMsgInstanceTx(const struct DBAL_Instance* const Inst)
{
    if(Inst == DBALCR_ConInst)
    {
        DBALCR_ConInst = NULL;/*Axivion Next Line MisraC2012-11.5 : No other choice than to do this conversion. Conversion struct->void->struct is safe.*/
        if(DBALCQ_getConnectMsg((DBALCR_CAST_INSTANCE_TO_QUEUE)&DBALCR_ConInst) == true)
        {
            DBAL_prepareTxEntry(DBALCR_ConInst, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON, DBALCR_ConInst->ConnectDataLen);
            BAL_vTransmitMessage((uint8_t)DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
        }
    }
}

/** \brief     Triggers transmission of DBal Dbus2 Request/Response frames for next cross connection.
 *
 * \details    Checks, whether it is a DBal cross communication instance that has just finished transmission,
 *             and if so, starts it for the next one.
 * \details    Concerns DBal frames: Command, Query, Event, Command Ack, Query Ack, Event Ack.
 *
 * \param Inst DBal communication instance
 */
static void DBALCR_triggerNextReqRespMsgInstanceTx(const struct DBAL_Instance* const Inst)
{
    if(Inst == DBALCR_ReqRespInst)
    {
        uint8_t TxIndex = DBAL_MSG_INDEX_COUNT;
        DBALCR_ReqRespInst = NULL;/*Axivion Next Line MisraC2012-11.5 : No other choice than to do this conversion. Conversion struct->void->struct is safe.*/
        if(DBALCQ_getReqRespMsg((DBALCR_CAST_INSTANCE_TO_QUEUE)&DBALCR_ReqRespInst, &TxIndex) == true)/*lint !e934 Taking address of TxIndex safe: Used before function returns.*/
        {
            DBAL_prepareTxEntry(DBALCR_ReqRespInst, TxIndex, DBALCR_ReqRespInst->TransmitDataLen);
            BAL_vTransmitMessage((uint8_t)DBAL_DBUS_HANDLER_SUBSYSTEM, TxIndex);
        }
    }
}

/** \brief     Returns info, whether it is allowed to send another command or query.
 *
 * \details    It is prohibited to send the next command or query, if the last one has not yet been responded.
 *
 * \param Inst DBal communication instance
 *
 * \return     True, if a command or query may be sent, otherwise false.
*/
static bool DBALCR_isCmdOrQuery2BeRepeated(const struct DBAL_Instance* const Inst)
{
    bool RetVal = false;

    for(uint8_t i = 0; i < (uint8_t)DBAL_MAX_MSGS2REPEAT; i++)
    {
        if
        (
             (Inst->MsgRptPtrs[i]->IsSlotOccupied == true) &&
             ((Inst->MsgRptPtrs[i]->DbalType == DBAL_TYPE_CMD) ||
             (Inst->MsgRptPtrs[i]->DbalType == DBAL_TYPE_QUERY))
        )
        {
            RetVal = true;
            break;
        }
    }

    return RetVal;
}
#endif/*DBAL_CROSS_CONNECTION*/

/** \brief                Checks, whether temporary connection is active.
 *
 * \param Inst            DBal communication instance
 * \param TempCommPartner Address of temporary communication partner
 *
 * \return                True, if temporary connection is active, otherwise false.
 */
static bool DBAL_isTempConActive(const struct DBAL_Instance* const Inst, uint8_t TempCommPartner)
{
    return
    (
        (Inst->DBUS_ComPartner != Inst->DBUS_ComBackup) &&
        (Inst->DBUS_ComPartner == TempCommPartner)
    ) ? true : false;
}

/** \brief     Checks, whether disable handshake is being performed.
 *
 * \param Inst DBal communication instance
 *
 * \return     True, if disable handshake is being performed (unfinished), otherwise false.
 */
static bool DBAL_isDisablePending(const struct DBAL_Instance* const Inst)
{
    return
    (
        (Inst->DisableReqReceived == true) ||
        (Inst->IoCurrentConnectionState == DBAL_CONNECTIONSTATE_DISCONNECTING)
    );
}

/** \brief     Resets counter for resending messages, that have not been delivered.
 *
 * \param Inst DBal communication instance
*/
static void DBAL_clearRetryCounters(struct DBAL_Instance* const Inst)
{
    (void)memset(Inst->SendRetryCounter, 0, sizeof(Inst->SendRetryCounter));/*lint !e920 cast ok in this case we do not need pointer*/
}

/** \brief     Clears queue of messages, for which a response is expected.
 *
 * \param Inst DBal communication instance
*/
static void DBAL_clearMsgs2Repeat(struct DBAL_Instance* const Inst)
{
    DBAL_checkAndFireMsgs2RepeatWhenClearing(Inst);
    (void)memset(Inst->Msgs2Repeat, 0 , sizeof(Inst->Msgs2Repeat));/*lint !e920 cast ok in this case we do not need pointer*/
}

/** \brief     Notifies user about non delivered messages, before setting CommState to NOT_READY, if callback is registered.
 *
 * \param Inst DBal communication instance
 */
static void DBAL_checkAndFireMsgs2RepeatWhenClearing(const struct DBAL_Instance* const Inst)
{
    for(uint8_t j = 0; j < Inst->IoMsgLostCbCounter; j++)
    {
        for(uint8_t i = 0; i < (uint8_t)DBAL_MAX_MSGS2REPEAT; i++)
        {
            if(Inst->MsgRptPtrs[i]->IsSlotOccupied == true)
            {
                Inst->IoMsgLostCbArray[j](Inst->MsgRptPtrs[i]->ServiceId, Inst->MsgRptPtrs[i]->CommandId, Inst->MsgRptPtrs[i]->Data, Inst->MsgRptPtrs[i]->Datalen);
            }
        }
    }
}

/** \brief           Closes gap in queue of messages, for which a response is expected.
 *
 * \details          Important, if the message in the middle has been confirmed first, to keep FIFO behavior.
 *
 * \param Inst       DBal communication instance
 * \param StartIndex Index, at which we start looking for gaps is queue.
*/
static void DBAL_closeGapBetweenMsgs2Repeat(struct DBAL_Instance* const Inst, uint8_t StartIndex)
{
    struct DBAL_Msg2Repeat* PutToEnd = Inst->MsgRptPtrs[StartIndex];

    for(uint8_t i = StartIndex; i < (uint8_t)DBAL_LAST_MSG2REPEAT; i++)
    {
        Inst->MsgRptPtrs[i] = Inst->MsgRptPtrs[i + 1U];
    }
    Inst->MsgRptPtrs[DBAL_LAST_MSG2REPEAT] = PutToEnd;

    (void)memset(Inst->MsgRptPtrs[DBAL_LAST_MSG2REPEAT], 0 , sizeof(struct DBAL_Msg2Repeat));/*lint !e920 cast ok in this case we do not need pointer*/
}

/** \brief     Calls closing of gaps; looks if anything is left in queue of messages to repeat.
 *
 * \details    If queue empty, timer is stopped.
 *
 * \param Inst DBal communication instance.
 *
 * \return     DBAL_MSG2REPEAT_NONE, DBAL_MSG2REPEAT_EXIST
*/
static enum DBAL_Msgs2RepeatStatus DBAL_organizeMsgs2Repeat(struct DBAL_Instance* const Inst)
{
    enum DBAL_Msgs2RepeatStatus RetVal = DBAL_MSG2REPEAT_NONE;

    for(int8_t i = (int8_t)DBAL_LAST_MSG2REPEAT; i >= 0; i--)
    {
        if(Inst->MsgRptPtrs[i]->IsSlotOccupied == true)
        {
            RetVal = DBAL_MSG2REPEAT_EXIST;
        }
        else
        {
            DBAL_closeGapBetweenMsgs2Repeat(Inst, (uint8_t)i);
        }
    }

    if(RetVal == DBAL_MSG2REPEAT_NONE)
    {
        STIM_DisableTimer(&Inst->MsgTimer);
    }

    return RetVal;
}

/** \brief     Resets internal status and parameters for sending of DBal connection messages.
 *
 * \details    Called, whenever transmission has failed completely.
 *
 * \param Inst DBal communication instance.
*/
static void DBAL_handleConMsgTxFail(struct DBAL_Instance* const Inst)
{
#ifdef DBAL_CROSS_CONNECTION
    DBALCR_triggerNextConnectMsgInstanceTx(Inst);
#endif/*DBAL_CROSS_CONNECTION*/
    enum DBAL_ConnectionSmEvent Event = ((Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE] == (uint8_t)DBAL_CON_PING_REQUEST) ||
        (Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE] == (uint8_t)DBAL_CON_PING_RESPONSE)) ?
        DBAL_CON_SM_EVENT_DISABLE_SILENT : DBAL_CON_SM_EVENT_REJECT;/*Ping message->silent, else->reject.*/
    DBAL_clearRetryCounters(Inst);
    DBAL_connectionSm(Inst, Event);
}

/** \brief     Resets internal status and parameters for sending of DBal Dbus request/response messages.
 *
 * \details    Called, whenever transmission has failed completely.
 *
 * \param Inst DBal communication instance.
*/
static void DBAL_handleReqRespMsgTxFail(struct DBAL_Instance* const Inst)
{
#ifdef DBAL_CROSS_CONNECTION
    DBALCR_triggerNextReqRespMsgInstanceTx(Inst);
#endif/*DBAL_CROSS_CONNECTION*/
    DBAL_clearMsgs2Repeat(Inst);
    DBAL_clearRetryCounters(Inst);
    DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_DISABLE_SILENT);
}

/** \brief  Action to be performed by DBAL_msgTimerCallback...
 * \details ...or afterwards by task interrupted by this interrupt handler.
 *
 * \param Inst DBal communication instance.
 */
static void DBAL_msgTimerAction(struct DBAL_Instance* const Inst)
{
    for(uint8_t i = 0; i < (uint8_t)DBAL_MAX_MSGS2REPEAT; i++)
    {
        if
        (
            (Inst->MsgRptPtrs[i]->MsgRetryCounter < (uint8_t)DBAL_DBUS_RETRY_MAX) &&
            (Inst->MsgRptPtrs[i]->IsSlotOccupied == true)
        )
        {
            Inst->MsgRptPtrs[i]->MsgRetryCounter++;
            STIM_ResetTimer(&Inst->MsgTimer);
            STIM_EnableTimer(&Inst->MsgTimer);

            if(DBAL_isMsgOfDBalType2Repeat(Inst->MsgRptPtrs[i]->DbalType) == true)
            {
                if(DBAL_ioDbusHandler_send(Inst, Inst->MsgRptPtrs[i]->DbalType, Inst->MsgRptPtrs[i]->ServiceId, Inst->MsgRptPtrs[i]->CommandId, Inst->MsgRptPtrs[i]->Data, Inst->MsgRptPtrs[i]->Datalen, Inst->MsgRptPtrs[i]->MsgRetryCounter) == false)
                {
                    DBAL_WARNING("Failed 2 resent Addr %x, ServiceId 0x%x, CommandId 0x%x", Inst->DBUS_ComPartner, Inst->MsgRptPtrs[i]->ServiceId, Inst->MsgRptPtrs[i]->CommandId);
                    DBAL_clearMsgs2Repeat(Inst);
                    DBAL_clearRetryCounters(Inst);
                    DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_DISABLE_SILENT);
                }
            }
        }
        else if
        (
            (Inst->MsgRptPtrs[i]->MsgRetryCounter >= (uint8_t)DBAL_DBUS_RETRY_MAX) &&
            (Inst->MsgRptPtrs[i]->IsSlotOccupied == true)
        )
        {
            DBAL_WARNING("All repetitions failed, Addr %x, ServiceId 0x%x, CommandId 0x%x", Inst->DBUS_ComPartner, Inst->MsgRptPtrs[i]->ServiceId, Inst->MsgRptPtrs[i]->CommandId);
            DBAL_clearMsgs2Repeat(Inst);
            DBAL_clearRetryCounters(Inst);
            DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_DISABLE_SILENT);
        }
        else
        {
            /*Do nothing, everything fine.*/
        }
    }

    (void)DBAL_organizeMsgs2Repeat(Inst);
}

/** \brief  Timer to repeat connection messages.
 *
 * \details For meaning of params, look at STIM module description.
*/
static int32_t DBAL_conMsgTimerCallback(void *Obj, uint32_t Flags, int32_t Data)
{
    (void)Flags;
    (void)Data;
    /*Axivion Next Line MisraC2012-11.5 : No other choice than to do this conversion. Works fine.*/
    struct DBAL_Instance* const Inst = (struct DBAL_Instance*)Obj;
    DBAL_sendConnectionMessage(Inst, (enum DBAL_ConnectionMessageType)Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE]);/*lint !e9030 No other choice than that cast.*/
    return 0;
} /*lint !e818 could be pointing to const but is a predefined interface*/

/** \brief  Timer to repeat request/response DBal messages.
 *
 * \details For meaning of params, look at STIM module description.
*/
static int32_t DBAL_msgTimerCallback(void *Obj, uint32_t Flags, int32_t Data)
{
    (void)Flags;
    (void)Data;

    /*Axivion Next Line MisraC2012-11.5 : No other choice than to do this conversion. Works fine.*/
    struct DBAL_Instance* const Inst = (struct DBAL_Instance*)Obj;

    if(DBAL_checkTaskCodeSectionFlag(Inst) == false)
    {
        DBAL_msgTimerAction(Inst);
    }
    else
    {
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_REP_TIMER);
    }
    return 0;
}

/** \brief           Decides what to do with connection message, depending on its' type.
 *
 * \param Inst       DBal communication instance
 * \param ConMsgType Type of connection message; See DBal spec for meaning.
 */
static void DBAL_handleConMsg(struct DBAL_Instance* const Inst, uint8_t ConMsgType)
{
    DBM_DISABLE_INT();
    switch(ConMsgType)
    {
        case (uint8_t)DBAL_CON_ENABLE_REQUEST:
            DBAL_clearBothIoTxBuffers(Inst);
            DBAL_clearRetryCounters(Inst);
            DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
            break;
        case (uint8_t)DBAL_CON_ENABLE_RESPONSE:
            DBAL_clearBothIoTxBuffers(Inst);
            DBAL_clearRetryCounters(Inst);
            DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_ACCEPT);
            break;
        case (uint8_t)DBAL_CON_DISABLE_REQUEST:
            if(DBAL_organizeMsgs2Repeat(Inst) == DBAL_MSG2REPEAT_NONE)
            {
                DBAL_clearMsgs2Repeat(Inst);
                DBAL_clearRetryCounters(Inst);
                DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
            }
            else
            {
                Inst->DisableReqReceived = true;
            }
            break;
        case (uint8_t)DBAL_CON_DISABLE_RESPONSE:
            DBAL_clearMsgs2Repeat(Inst);
            DBAL_clearRetryCounters(Inst);
            DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_ACCEPT);
            break;
        case (uint8_t)DBAL_CON_PING_REQUEST:
            DBAL_sendCommPingResponse(Inst);
            break;
        case (uint8_t)DBAL_CON_PING_RESPONSE:
#ifdef DBAL_CROSS_CONNECTION
            DBALCR_triggerPingReqTimerWithStdTime(Inst);
#else/*DBAL_CROSS_CONNECTION*/
            DBAL_WARNING("No ping response processed because no cross connection. Addr %x", Inst->DBUS_ComPartner);/*Only for cross connection.*/
#endif/*DBAL_CROSS_CONNECTION*/
            break;
        default:
            DBAL_WARNING("Connection frame of invalid type %d, Addr %x", ConMsgType, Inst->DBUS_ComPartner);/*No valid connection frame*/
            break;
    }
    DBM_ENABLE_INT();
}

/** \brief                Decides what to do with temporary connection message, depending on its' type.
 *
 * \param Inst            DBal communication instance
 * \param TempCommPartner Communication partner for temporary connection.
 * \param ConMsgType      Type of connection message; See DBal spec for meaning.
 */
static void DBAL_handleTempConMsg(struct DBAL_Instance* const Inst, uint8_t TempCommPartner, uint8_t ConMsgType)
{
    DBM_DISABLE_INT();
    switch(ConMsgType)
    {
        case (uint8_t)DBAL_CON_TEMP_ENABLE_REQUEST:
            if((Inst->DBUS_ComPartner != TempCommPartner) && (Inst->DBUS_ComPartner == Inst->DBUS_ComBackup))
            {
                Inst->DBUS_ComPartner = TempCommPartner;
                DBAL_clearBothIoTxBuffers(Inst);
                DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST);
                DBAL_INFO("Established temporary connection with addr %x", Inst->DBUS_ComPartner);
            }
            else if(DBAL_isTempConActive(Inst, TempCommPartner) == true)
            {
                DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST);
                DBAL_INFO("Confirmed temporary connection with addr %x", Inst->DBUS_ComPartner);
            }
            else
            {
                DBAL_WARNING("Cannot enable temporary connection with addr %x. Already enabled?", TempCommPartner);
            }
            break;
        case (uint8_t)DBAL_CON_TEMP_ENABLE_RESPONSE:
            /*We are passive here.*/
            DBAL_ERROR("DBAL_CON_TEMP_ENABLE_RESPONSE: Processing not supported!");
            break;
        case (uint8_t)DBAL_CON_TEMP_DISABLE_REQUEST:
            if(DBAL_isTempConActive(Inst, TempCommPartner) == true)
            {
                DBAL_clearMsgs2Repeat(Inst);
                DBAL_clearRetryCounters(Inst);
                DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_TEMP_DISABLE_REQUEST);
                DBAL_INFO("Cut temporary connection with addr %x", TempCommPartner);
            }
            else
            {
                DBAL_WARNING("Cannot disable temporary connection with addr %x. Already disabled?", Inst->DBUS_ComPartner);
            }
            break;
        case (uint8_t)DBAL_CON_TEMP_DISABLE_RESPONSE:
            /*We are passive here.*/
            DBAL_ERROR("DBAL_CON_TEMP_DISABLE_RESPONSE: Processing not supported!");
            break;
        default:
            DBAL_WARNING("Temporary Connection frame of invalid type %d, Addr %x", ConMsgType, Inst->DBUS_ComPartner);/*No valid connection frame*/
            break;
    }
    DBM_ENABLE_INT();
}

/** \brief               Searches for user function to process DBal frame, and if existing, calls it.
 *
 * \param Inst           DBal communication instance
 * \param DBalType       See DBal spec for meaning.
 * \param ServiceId      See DBal spec for meaning.
 * \param CommandId      See DBal spec for meaning.
 * \param Bytes          Payload
 * \param DBalPayloadLen Length of payload
 *
 * \return               Returns true, if callback exists and was called; else false.
*/
static bool DBAL_callServiceCallback
(
    const struct DBAL_Instance* const Inst,
    enum DBAL_MessageType DBalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DBalPayloadLen
)
{
#ifdef DBAL_CROSS_CONNECTION
    if(Inst != DBALIN_getMainInstance())
    {
        DBALCR_triggerPingReqTimerWithStdTime(Inst);
        for(uint8_t ReceiveObjIndex = 0; ReceiveObjIndex < DBALCR_ObjectTableSize; ReceiveObjIndex++)
        {
            if(ServiceId == DBALCR_ObjectTable[ReceiveObjIndex].ServiceId)
            {
                const struct DBALCR_ReceiveObject* Ptr = DBALCR_ObjectTable[ReceiveObjIndex].ReceiveObject;
                const uint8_t ObjectCount = DBALCR_ObjectTable[ReceiveObjIndex].ReceiveObjectCount;

                for(uint8_t CmdIndex = 0; CmdIndex < ObjectCount; CmdIndex++)
                {
                    if((Ptr->Identifier.CommandId == CommandId) && (Ptr->Identifier.Type == DBalType))
                    {
                        Ptr->Service(DBALIN_getIndexOfUserInstance(Inst), Bytes, DBalPayloadLen);
                        return true;
                    }
                    Ptr++;
                }
            }
        }
    }
    else
#else/*DBAL_CROSS_CONNECTION*/
    (void)Inst;/*lint !e920 cast ok in this case we do not need pointer*/
#endif/*not DBAL_CROSS_CONNECTION*/
    {
        for(uint8_t ReceiveObjIndex = 0; ReceiveObjIndex < DBAL_ObjectTableSize; ReceiveObjIndex++)
        {
            if(ServiceId == DBAL_ObjectTable[ReceiveObjIndex].ServiceId)
            {
                const struct DBAL_ReceiveObject* Ptr = DBAL_ObjectTable[ReceiveObjIndex].ReceiveObject;
                const uint8_t ObjectCount = DBAL_ObjectTable[ReceiveObjIndex].ReceiveObjectCount;

                for(uint8_t CmdIndex = 0; CmdIndex < ObjectCount; CmdIndex++)
                {
                    if((Ptr->Identifier.CommandId == CommandId) && (Ptr->Identifier.Type == DBalType))
                    {
                        Ptr->Service(Bytes, DBalPayloadLen);
                        return true;
                    }
                    Ptr++;
                }
            }
        }
    }

    return false;
}

/** \brief        Scans Dbus2 DBal request/response frame, for command/query/event.
 *
 * \details       Also check, whether Dbus2 frame is coherent,
 *                i.e. declared lengths of DBal frames and overall length fit.
 * \details       Also check, whether all DBal frames have valid DBal types.
 * \details       Report incoherences and unknown DBal frames to application.
 *
 * \param Inst    DBal communication instance
 * \param Bytes   Payload of Dbus2 frame
 * \param DataLen Length of payload of Dbus2 frame
 *
*/
static void DBAL_look4MsgReception(struct DBAL_Instance* const Inst, const uint8_t* const Bytes, uint8_t DataLen)
{
    uint8_t Offset = DBAL_MSG_FRAME_OFFSET;
    DBAL_resetEvents2Ack();
    while((Offset + DBAL_FRAME_DATA_OFFSET) <= DataLen)
    {
        enum DBAL_MessageType DBalType = DBAL_convertUint8ToDBalType(Bytes[Offset]);
        uint8_t PayloadLen = Bytes[Offset + DBAL_FRAME_PAYLOADLEN];
        uint16_t ServiceId = (uint16_t)(((uint16_t)Bytes[Offset + DBAL_FRAME_SERVICE_ID_HI] << BYTE_SIZE) | Bytes[Offset + DBAL_FRAME_SERVICE_ID_LO]);
        uint16_t CommandId = (uint16_t)(((uint16_t)Bytes[Offset + DBAL_FRAME_COMMAND_ID_HI] << BYTE_SIZE) | Bytes[Offset + DBAL_FRAME_COMMAND_ID_LO]);

        if((Offset + DBAL_FRAME_DATA_OFFSET + PayloadLen) <= DataLen)
        {
            if(DBAL_isMsgOfDBalType2Repeat(DBalType) == true)
            {
                if(DBAL_callServiceCallback(Inst, DBalType, ServiceId, CommandId, &Bytes[Offset + DBAL_FRAME_DATA_OFFSET], PayloadLen) == false)
                {
                    DBAL_ntfUnknownDBalFrameReceived(Inst->DBUS_ComPartner, DBalType, ServiceId, CommandId, &Bytes[Offset + DBAL_FRAME_DATA_OFFSET], PayloadLen);
                }

                if(DBalType == DBAL_TYPE_EVENT)
                {
                    if(DBAL_isEventAlreadyAcknowledged(ServiceId, CommandId) == false)
                    {
                        if(DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_EVENT_ACK, ServiceId, CommandId, &Bytes[Offset + DBAL_FRAME_DATA_OFFSET], 0U, 0U) == false)
                        {
                            /*Ignore. Other side will send the event again*/
                        }
                    }
                }
            }
            else
            {
                if(DBalType == DBAL_TYPE_UNKNOWN)
                {
                    DBAL_ntfUnknownDBalFrameReceived(Inst->DBUS_ComPartner, DBalType, ServiceId, CommandId, &Bytes[Offset + DBAL_FRAME_DATA_OFFSET], PayloadLen);
                }
            }
        }

        Offset += DBAL_FRAME_DATA_OFFSET + PayloadLen;
    }

    if((Offset != DataLen) || (Offset == DBAL_MSG_FRAME_OFFSET))
    {
        DBAL_ntfCorruptReqRespDbus2FrameReceived(Bytes, DataLen);
    }
}

/** \brief        Scans Dbus2 DBal response frame, for command_ack/query_ack/event_ack.
 *
 * \details       Also check, whether any unexpected ack has been received.
 * \details       Report incoherences and unknown DBal frames to application.
 *
 * \param Inst    DBal communication instance
 * \param Bytes   Payload of Dbus2 frame
 * \param DataLen Length of payload of Dbus2 frame
 *
*/
static void DBAL_look4AckMsgReception(struct DBAL_Instance* const Inst, const uint8_t* const Bytes, uint8_t DataLen)
{
    uint8_t Offset = DBAL_MSG_FRAME_OFFSET;
    while((Offset + DBAL_FRAME_DATA_OFFSET) <= DataLen)
    {
        bool MessageIsUnexpected = true;
        enum DBAL_MessageType DBalType = DBAL_convertUint8ToDBalType(Bytes[Offset]);
        uint8_t PayloadLen = Bytes[Offset + DBAL_FRAME_PAYLOADLEN];
        uint16_t ServiceId = (uint16_t)(((uint16_t)Bytes[Offset + DBAL_FRAME_SERVICE_ID_HI] << BYTE_SIZE) | Bytes[Offset + DBAL_FRAME_SERVICE_ID_LO]);
        uint16_t CommandId = (uint16_t)(((uint16_t)Bytes[Offset + DBAL_FRAME_COMMAND_ID_HI] << BYTE_SIZE) | Bytes[Offset + DBAL_FRAME_COMMAND_ID_LO]);

        for(uint8_t i = 0; i < (uint8_t)DBAL_MAX_MSGS2REPEAT; i++)
        {
            if
            (
                (DBAL_isMsgOfAckDBalType(DBalType) == true) &&
                ((Offset + DBAL_FRAME_DATA_OFFSET + PayloadLen) <= DataLen) &&
                (DBalType == DBAL_getResponseDBalType(Inst->MsgRptPtrs[i]->DbalType)) &&
                (Inst->MsgRptPtrs[i]->IsSlotOccupied == true) &&
                (ServiceId == Inst->MsgRptPtrs[i]->ServiceId) &&
                (CommandId == Inst->MsgRptPtrs[i]->CommandId) &&
                ((PayloadLen == 0U) || (DBalType != DBAL_TYPE_EVENT_ACK))
            )
            {
                if(DBalType != DBAL_TYPE_EVENT_ACK)
                {
                    if(DBAL_callServiceCallback(Inst, DBalType, ServiceId, CommandId, &Bytes[Offset + DBAL_FRAME_DATA_OFFSET], PayloadLen) == false)
                    {
                        DBAL_ntfUnknownDBalFrameReceived(Inst->DBUS_ComPartner, DBalType, ServiceId, CommandId, &Bytes[Offset + DBAL_FRAME_DATA_OFFSET], PayloadLen);
                    }
                }

                Inst->MsgRptPtrs[i]->IsSlotOccupied = false;
                MessageIsUnexpected = false;
            }
            else
            {
                if(DBAL_isMsgOfDBalType2Repeat(DBalType) == true)
                {
                    MessageIsUnexpected = false;
                }
            }
        }

        if(MessageIsUnexpected == true)
        {
            DBAL_ntfUnexpectedDBalFrameReceived(Inst->DBUS_ComPartner, DBalType, ServiceId, CommandId, &Bytes[Offset + DBAL_FRAME_DATA_OFFSET], PayloadLen);
        }
        Offset += DBAL_FRAME_DATA_OFFSET + PayloadLen;
    }
}

/** \brief        Checks whether DBal connection frame is corrupt.
 *
 * \param Inst    DBal communication instance
 * \param Bytes   Payload of Dbus2 frame
 * \param DataLen Length of payload of Dbus2 frame
 *
 * \return        true, if check is positive, otherwise false
*/
static bool DBAL_isReceivedConMsgCorrupt
(
    const struct DBAL_Instance* const Inst,
    const uint8_t* const Bytes,
    uint8_t DataLen
)
{
    return
    (
        (Inst == NULL) ||
        (DataLen != DBAL_CON_MSG_LEN) ||
        (Bytes[DBAL_CON_MSG_SENDER] != Inst->DBUS_ComPartner) ||
        (Bytes[DBAL_CON_MSG_PROTOCOL_TYPE] != DBAL_APPLIANCE_LAYER) ||
        (Bytes[DBAL_CON_MSG_TYPE] >= (uint8_t)DBAL_CON_MSG_TYPE_COUNT) ||
        (DBAL_isCorrectProtocolVersion(Bytes[DBAL_CON_MSG_PROTOCOL_VERSION]) != true)
    ) ? true : false;
}

/** \brief        Checks whether DBal connection frame is for handling a temporary connection.
 *
 * \param Inst    DBal communication instance
 * \param Bytes   Payload of Dbus2 frame
 * \param DataLen Length of payload of Dbus2 frame
 *
 * \return        true, if check is positive, otherwise false
*/
static bool DBAL_isReceivedConMsgTemp
(
    const struct DBAL_Instance* const Inst,
    const uint8_t* const Bytes,
    uint8_t DataLen
)
{
    return
    (
        ((Inst == NULL) ||
        (Inst == DBALIN_getMainInstance())) &&
        (DataLen == DBAL_CON_MSG_LEN) &&
        (DBAL_isValidDbus2Address(Bytes[DBAL_CON_MSG_SENDER]) == true) &&
        (Bytes[DBAL_CON_MSG_PROTOCOL_TYPE] == DBAL_APPLIANCE_LAYER) &&
        (DBAL_isTempConMessage(Bytes[DBAL_CON_MSG_TYPE]) == true) &&
        (DBAL_isCorrectProtocolVersion(Bytes[DBAL_CON_MSG_PROTOCOL_VERSION]) == true)
    ) ? true : false;
}

/** \brief        Checks whether DBal connection frame is correct and CommPartner (Main Instance)
 *                address still needs initialization.
 *
 * \param Inst    DBal communication instance
 * \param Bytes   Payload of Dbus2 frame
 * \param DataLen Length of payload of Dbus2 frame
 *
 * \return        true, if check is positive, otherwise false
*/
static bool DBAL_doesReceivedConMsgInitCommPartner
(
    const struct DBAL_Instance* const Inst,
    const uint8_t* const Bytes,
    uint8_t DataLen
)
{
    const struct DBAL_Instance* const MainInst = DBALIN_getMainInstance();

    return
    (
        (Inst == NULL) &&
        (DataLen == DBAL_CON_MSG_LEN) &&
        (Bytes[DBAL_CON_MSG_SENDER] != MainInst->DBUS_ComPartner) &&
        (MainInst->DBUS_ComPartner == DBAL_PRELIMINARY_PARTNER_ADDR) &&
        (DBAL_isTempConActive(MainInst, MainInst->DBUS_ComPartner) == false) &&
        (DBAL_isValidDbus2Address(Bytes[DBAL_CON_MSG_SENDER]) == true) &&
        (Bytes[DBAL_CON_MSG_PROTOCOL_TYPE] == DBAL_APPLIANCE_LAYER) &&
        (Bytes[DBAL_CON_MSG_TYPE] == (uint8_t)DBAL_CON_ENABLE_REQUEST) &&
        (DBAL_isCorrectProtocolVersion(Bytes[DBAL_CON_MSG_PROTOCOL_VERSION]) == true)
    ) ? true : false;
}

/** \brief        Checks whether DBal request/response frame is corrupt.
 *
 * \param Inst    DBal communication instance
 * \param Bytes   Payload of Dbus2 frame
 * \param DataLen Length of payload of Dbus2 frame
 *
 * \return        true, if check is positive, otherwise false
*/
static bool DBAL_isReceivedReqRespMsgCorrupt
(
    const struct DBAL_Instance* const Inst,
    const uint8_t* const Bytes,
    uint8_t DataLen
)
{
    return
    (
        (Inst == NULL) ||
        (DataLen < (uint8_t)DBAL_MSG_FRAME_OFFSET) ||
        (Bytes[DBAL_MSG_SENDER] != Inst->DBUS_ComPartner) ||
        (Bytes[DBAL_MSG_PROTOCOL_TYPE] != (uint8_t)DBAL_APPLIANCE_LAYER)
    ) ? true : false;
}

/** \brief      Checks whether DBal request/response frame is to be ignored.
 *
 * \details     Validity of SequenceCounter, Dbus2 address of CommPartner, CommState
 *
 * \param Inst  DBal communication instance
 * \param Bytes Payload of Dbus2 frame
 *
 * \return      true, if check is positive, otherwise false
*/
static bool DBAL_isReceivedReqRespMsg2BeIgnored(const struct DBAL_Instance* const Inst, const uint8_t* const Bytes)
{
    return
    (
        (DBAL_getCommStateByInstance(Inst) == DBAL_COMMSTATE_NOT_READY)||
        (DBAL_isValidDbus2Address(Inst->DBUS_ComPartner) == false) ||
        ((uint8_t)(Bytes[DBAL_MSG_SEQID] - Inst->LastSeqIdReceived) == (uint8_t)0U) ||/*New SeqId not equal to old.*/
        ((uint8_t)(Bytes[DBAL_MSG_SEQID] - Inst->LastSeqIdReceived) > (uint8_t)INT8_MAX)/*New SeqId not smaller than old.*/
    ) ? true : false;
}

/** \brief  Start timer for connection enable/disable request for repetition after sending
 *
 * \details ...with ack_ok received.
 *
 * \param Inst  DBal communication instance
 */
static void DBAL_conMsgTxPostRepeatTrigger(struct DBAL_Instance* const Inst)
{
    if
    (
        (Inst != NULL) &&
        ((Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE] == (uint8_t)DBAL_CON_ENABLE_REQUEST) ||
        (Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE] == (uint8_t)DBAL_CON_DISABLE_REQUEST))
    )
    {
        if(Inst->ConRepeatCnt < (uint8_t)DBAL_DBUS_RETRY_MAX)
        {
            STIM_ReloadTimer(&Inst->ConMsgTimer, (STIM_Time_t)DBAL_RESPONSE_TIME_MS);
            STIM_EnableTimer(&Inst->ConMsgTimer);
            Inst->ConRepeatCnt++;
        }
        else
        {
            DBAL_handleConMsgTxFail(Inst);
            DBAL_WARNING("Used all repetitions on Instance with Addr 0x%x. Connection Message %d not responded!.",
                Inst->DBUS_ComPartner, Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE]);
        }
    }
}

/** \brief  Action to be performed by DbalTransmitPost_appLayerRequest(Response)...
 *
 * \details ...or afterwards by task interrupted by this interrupt handler.
 *
 * \param Inst  DBal communication instance
 */
static void DBAL_reqRespTxPostAction(const struct DBAL_Instance* const Inst)
{
    if(Inst != NULL)
    {
        DBAL_clearIoTxRepeatBuffer(Inst);
        (void)DBAL_check4NextMsgs2SendAndTrigger(Inst, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    }
}

#ifdef DBAL_CROSS_CONNECTION
/** \brief  Action to be performed by DbalTransmitCrPost_appLayerRequest(Response)...
 *
 * \details ...or afterwards by task interrupted by this interrupt handler.
 *
 * \param Inst  DBal communication instance
 */
static void DBALCR_reqRespTxPostAction(const struct DBAL_Instance* const Inst)
{
    if
    (
        (DBALCR_ReqRespInst != NULL) &&
        (DBALCR_ReqRespInst == Inst) &&
        (DBAL_areReqRespTxFlagsSet(DBALCR_ReqRespInst) == false)
    )
    {
        DBAL_clearIoTransmitBuffer(DBALCR_ReqRespInst);
        DBALCR_triggerPingReqTimerWithStdTime(DBALCR_ReqRespInst);
        DBALCR_triggerNextReqRespMsgInstanceTx(DBALCR_ReqRespInst);
    }
}
#endif

/** \brief  Transmit handler for connection message to SMM.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalTransmit_appLayerConnection(uint8_t DataLen, uint8_t* Bytes)
{
    const struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        memcpy(Bytes, Inst->ConnectTransmitBuffer, DataLen);
    }
}

/** \brief  Transmit confirmation function for connection message to SMM.
*/
static void DbalTransmitPost_appLayerConnection(void)
{
    DBAL_conMsgTxPostRepeatTrigger(DBALIN_getMainInstance());
}

#ifdef DBAL_CROSS_CONNECTION
/** \brief  Transmit handler for connection message in ecu to ecu communication.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalTransmitCr_appLayerConnection(uint8_t DataLen, uint8_t* Bytes)
{
    if(DBALCR_ConInst != NULL)
    {
        memcpy(Bytes, DBALCR_ConInst->ConnectTransmitBuffer, DataLen);
    }
    else
    {
        DBAL_ERROR("Trying to send con msg with no Cr instance set.");/*Must never be called.*/
    }
}

/** \brief  Transmit confirmation function for connection message in ecu to ecu communication.
*/
static void DbalTransmitCrPost_appLayerConnection(void)
{
    if
    (
        (DBALCR_ConInst != NULL) &&
        (DBAL_areConTxFlagsSet(DBALCR_ConInst) == false)
    )
    {
        DBAL_conMsgTxPostRepeatTrigger(DBALCR_ConInst);
        DBALCR_triggerNextConnectMsgInstanceTx(DBALCR_ConInst);
    }
}
#endif/*DBAL_CROSS_CONNECTION*/

/* Info 818: parameter 'Bytes' of function 'DbalReceive_appLayer*(uint8_t, uint8_t *)' could be pointer to const [MISRA 2012 Rule 8.13, advisory]
 * Cannot be const, as interfaces are predefined in bustypes.h! */
/*lint -esym(818,DbalReceive_appLayer*) */

/** \brief  Receive handler for connection message.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalReceive_appLayerConnection(uint8_t DataLen, uint8_t* Bytes)
{
    struct DBAL_Instance* const Inst = DBALIN_getInstanceByAddress(Bytes[DBAL_CON_MSG_SENDER]);
    struct DBAL_Instance* const MainInst = DBALIN_getMainInstance();

    if(DBAL_doesReceivedConMsgInitCommPartner(Inst, Bytes, DataLen) == true)
    {
        MainInst->DBUS_ComPartner = Bytes[DBAL_CON_MSG_SENDER];
        MainInst->DBUS_ComBackup = Bytes[DBAL_CON_MSG_SENDER];
        DBAL_storeTargetAddress(MainInst->DBUS_ComPartner);
        DBAL_handleConMsg(MainInst, Bytes[DBAL_CON_MSG_TYPE]);
        DBAL_INFO("SMM Remembered target address %x", MainInst->DBUS_ComPartner);
    }
    else if(DBAL_isReceivedConMsgTemp(Inst, Bytes, DataLen) == true)
    {
        DBAL_handleTempConMsg(MainInst, Bytes[DBAL_CON_MSG_SENDER], Bytes[DBAL_CON_MSG_TYPE]);
    }
    else if
    (
        (DBAL_isTempConActive(MainInst, MainInst->DBUS_ComPartner) == true) &&
        (MainInst->DBUS_ComBackup == Bytes[DBAL_CON_MSG_SENDER])
    )
    {
        /*Cannot process normal connection, if temporary active.*/
        DBAL_WARNING("Cannot open connection for addr %x, because temporary connection for addr %x active!", MainInst->DBUS_ComBackup, MainInst->DBUS_ComPartner);
    }
    else if(DBAL_isReceivedConMsgCorrupt(Inst, Bytes, DataLen) == true)
    {
        DBAL_ntfCorruptConDbus2FrameReceived(Bytes, DataLen);
    }
    else
    {
        DBAL_handleConMsg(Inst, Bytes[DBAL_CON_MSG_TYPE]);
    }
}

/** \brief  Transmit handler for Dbus2 DBal request message to SMM.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalTransmit_appLayerRequest(uint8_t DataLen, uint8_t* Bytes)
{
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
        DBAL_writeReqRespInDbus2Buffer(Inst, DataLen, Bytes);
        DBAL_executeSetNonTaskCodeSections(Inst);
    }
}

/** \brief  Transmit confirmation function for Dbus2 DBal request message to SMM.
*/
static void DbalTransmitPost_appLayerRequest(void)
{
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(DBAL_checkTaskCodeSectionFlag(Inst) == false)
    {
        DBAL_reqRespTxPostAction(Inst);
    }
    else
    {
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_REQ_RESP_POST);
    }
}


#ifdef DBAL_CROSS_CONNECTION
/** \brief  Transmit handler for Dbus2 DBal request message in ecu to ecu communication.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalTransmitCr_appLayerRequest(uint8_t DataLen, uint8_t* Bytes)
{
    if(DBALCR_ReqRespInst != NULL)
    {
        DBAL_setCodeSectionFlag(DBALCR_ReqRespInst, DBAL_CODE_SEC_TASK);
        memcpy(Bytes, DBALCR_ReqRespInst->TransmitBuffer, DataLen);
        DBAL_executeSetNonTaskCodeSections(DBALCR_ReqRespInst);
    }
    else
    {
        DBAL_ERROR("Trying to send request with no Cr instance set.");/*Must never be called.*/
    }
}

/** \brief  Transmit confirmation function for Dbus2 DBal request message in ecu to ecu communication.
*/
static void DbalTransmitCrPost_appLayerRequest(void)
{
    if
    (
        (DBALCR_ReqRespInst != NULL) &&
        (DBAL_checkTaskCodeSectionFlag(DBALCR_ReqRespInst) == false)
    )
    {
        DBALCR_reqRespTxPostAction(DBALCR_ReqRespInst);
    }
    else
    {
        DBAL_setCodeSectionFlag(DBALCR_ReqRespInst, DBAL_CODE_SEC_CR_REQ_RESP_POST);
    }
}
#endif/*DBAL_CROSS_CONNECTION*/

/** \brief  Receive handler for Dbus2 DBal request message.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalReceive_appLayerRequest(uint8_t DataLen, uint8_t* Bytes)
{
    struct DBAL_Instance* const Inst = DBALIN_getInstanceByAddress(Bytes[DBAL_MSG_SENDER]);

    if(DBAL_isReceivedReqRespMsgCorrupt(Inst, Bytes, DataLen) == true)
    {
        DBAL_ntfCorruptReqRespDbus2FrameReceived(Bytes, DataLen);/*Notify user.*/
    }
    else if(DBAL_isReceivedReqRespMsg2BeIgnored(Inst, Bytes) == true)
    {
        /*Ignore*/
        DBAL_INFO("Ignore request, SeqId %u, Last %u, Commstate %u, Addr %x", Bytes[DBAL_MSG_SEQID], Inst->LastSeqIdReceived, DBAL_getCommStateByInstance(Inst), Inst->DBUS_ComPartner);
    }
    else
    {
        Inst->LastSeqIdReceived = Bytes[DBAL_MSG_SEQID];
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
        DBAL_look4MsgReception(Inst, Bytes, DataLen);
        DBAL_executeSetNonTaskCodeSections(Inst);
    }
}

/** \brief  Transmit handler for Dbus2 DBal response message to SMM.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalTransmit_appLayerResponse(uint8_t DataLen, uint8_t* Bytes)
{
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
        DBAL_writeReqRespInDbus2Buffer(Inst, DataLen, Bytes);
        DBAL_executeSetNonTaskCodeSections(Inst);
    }
}

/** \brief  Transmit confirmation function for Dbus2 DBal response message to SMM.
*/
static void DbalTransmitPost_appLayerResponse(void)
{
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(DBAL_checkTaskCodeSectionFlag(Inst) == false)
    {
        DBAL_reqRespTxPostAction(Inst);
    }
    else
    {
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_REQ_RESP_POST);
    }
}

#ifdef DBAL_CROSS_CONNECTION
/** \brief  Transmit handler for Dbus2 DBal response message in ecu to ecu communication.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalTransmitCr_appLayerResponse(uint8_t DataLen, uint8_t* Bytes)
{
    if(DBALCR_ReqRespInst != NULL)
    {
        DBAL_setCodeSectionFlag(DBALCR_ReqRespInst, DBAL_CODE_SEC_TASK);
        memcpy(Bytes, DBALCR_ReqRespInst->TransmitBuffer, DataLen);
        DBAL_executeSetNonTaskCodeSections(DBALCR_ReqRespInst);
    }
    else
    {
        DBAL_ERROR("Trying to send response with no Cr instance set.");/*Must never be called.*/
    }
}

/** \brief  Transmit confirmation function for Dbus2 DBal response message in ecu to ecu communication.
*/
static void DbalTransmitCrPost_appLayerResponse(void)
{
    if
    (
        (DBALCR_ReqRespInst != NULL) &&
        (DBAL_checkTaskCodeSectionFlag(DBALCR_ReqRespInst) == false)
    )
    {
        DBALCR_reqRespTxPostAction(DBALCR_ReqRespInst);
    }
    else
    {
        DBAL_setCodeSectionFlag(DBALCR_ReqRespInst, DBAL_CODE_SEC_CR_REQ_RESP_POST);
    }
}
#endif/*DBAL_CROSS_CONNECTION*/

/** \brief  Receive handler for Dbus2 DBal response message.
 *
 * \details Interface predefined in bustypes.h, so no description of parameters here.
*/
static void DbalReceive_appLayerResponse(uint8_t DataLen, uint8_t* Bytes)
{
    struct DBAL_Instance* const Inst = DBALIN_getInstanceByAddress(Bytes[DBAL_MSG_SENDER]);

    if(DBAL_isReceivedReqRespMsgCorrupt(Inst, Bytes, DataLen) == true)
    {
        DBAL_ntfCorruptReqRespDbus2FrameReceived(Bytes, DataLen);/*Notify user.*/
    }
    else if(DBAL_isReceivedReqRespMsg2BeIgnored(Inst, Bytes) == true)
    {
        /*Ignore*/
        DBAL_INFO("Ignore response, SeqId %u, Last %u, Commstate %u, Addr %x", Bytes[DBAL_MSG_SEQID], Inst->LastSeqIdReceived, DBAL_getCommStateByInstance(Inst), Inst->DBUS_ComPartner);
    }
    else
    {
        Inst->LastSeqIdReceived = Bytes[DBAL_MSG_SEQID];
        DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
        DBAL_look4AckMsgReception(Inst, Bytes, DataLen);
        if
        (
            (DBAL_organizeMsgs2Repeat(Inst) == DBAL_MSG2REPEAT_NONE) &&
            (Inst->DisableReqReceived == true)
        )
        {
            DBAL_clearMsgs2Repeat(Inst);
            DBAL_clearRetryCounters(Inst);
            DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
        }
        DBAL_look4MsgReception(Inst, Bytes, DataLen);
        DBAL_executeSetNonTaskCodeSections(Inst);
    }
}

/** \brief       Assigns values to first two bytes of generic frame, before making DBal specific stuff follow.
 *
 * \param Buffer Pointer to buffer used for transmission.
*/
static void DBAL_prepareGenericFrameHeader(uint8_t* const Buffer)
{
    Buffer[DBAL_MSG_SENDER] = DBAL_OWN_NODE_ADDRESS;
    Buffer[DBAL_MSG_PROTOCOL_TYPE] = DBAL_APPLIANCE_LAYER;
}

/** \brief        Stores payload of a DBal frame to the according position in the transmit buffer used.
 *
 * \param Buffer  Pointer to buffer used for transmission.
 * \param From    Index of start point, from which data shall be written.
 * \param Bytes   Payload to write.
 * \param DataLen Size of payload to write.
*/
static void DBAL_storePayloadToTransmitBuffer
(
    uint8_t* const Buffer,
    uint8_t From,
    const uint8_t* const Bytes,
    uint8_t DataLen
)
{
    if(DataLen < ((uint8_t)DBAL_BUFFER_SIZE - From))
    {
        memcpy(&Buffer[From], Bytes, DataLen);
    }
    else
    {
        DBAL_ERROR("Not enough space in buffer %u, needed %u", (uint8_t)DBAL_BUFFER_SIZE-From, DataLen);/*Must not happen. Checked elsewhere.*/
    }
}

/** \brief      Clears content and indexes of buffer used to send Dbus DBal request/response messages.
 *
 * \param Inst  DBal communication instance.
*/
static void DBAL_clearIoTransmitBuffer(struct DBAL_Instance* const Inst)
{
    Inst->DbalFrames2TransmitCnt = 0;
    Inst->TransmitDataLen = 0;
    (void)memset(Inst->TransmitBuffer, 0, sizeof(Inst->TransmitBuffer)); /*lint !e920 cast ok in this case we do not need pointer*/
}

/** \brief      Clears content and indexes of buffer used to repeat DBal request/response messages on Dbus2 level.
 *
 * \param Inst  DBal communication instance.
*/
static void DBAL_clearIoTxRepeatBuffer(const struct DBAL_Instance* const Inst)
{
    if(Inst->TxRepeat != NULL)
    {
        Inst->TxRepeat->TxRepeatLen = 0;
        (void)memset(Inst->TxRepeat->TxRepeatBuffer, 0, sizeof(Inst->TxRepeat->TxRepeatBuffer)); /*lint !e920 cast ok in this case we do not need pointer*/
    }
}

/** \brief      Calls both DBAL_clearIoTransmitBuffer and DBAL_clearIoTxRepeatBuffer.
 *
 * \param Inst  DBal communication instance.
*/
static void DBAL_clearBothIoTxBuffers(struct DBAL_Instance* const Inst)
{
    DBAL_clearIoTransmitBuffer(Inst);
    DBAL_clearIoTxRepeatBuffer(Inst);
}

/** \brief      Checks, whether buffer used to send DBal request/response messages is empty.
 *
 * \param Inst  DBal communication instance.
*/
static bool DBAL_isIoTransmitBufferEmpty(const struct DBAL_Instance* const Inst)
{
    return ((Inst->TransmitDataLen == 0U) && (strlen((DBAL_CAST_STRING)Inst->TransmitBuffer) == 0U));/*lint !e929 "Conversion between object pointer and void pointer" MisraC2012 11.5" : no dangerous here */
}

/** \brief      Checks, whether buffer used to repeat DBal request/response messages on Dbus2 level is empty.
 *
 * \param Inst  DBal communication instance.
*/
static bool DBAL_isIoTxRepeatBufferEmpty(const struct DBAL_Instance* const Inst)
{
    if((Inst->TxRepeat == NULL))
    {
        return false;
    }
    return ((Inst->TxRepeat->TxRepeatLen == 0U) && (strlen((DBAL_CAST_STRING)Inst->TxRepeat->TxRepeatBuffer) == 0U));/*lint !e929 "Conversion between object pointer and void pointer" MisraC2012 11.5" : no dangerous here */
}

/** \brief        Sets values of pointers indicating,
 *                which DBal communication instances are currently sending messages in ecu to ecu communication.
 *
 * \param Inst    DBal communication instance.
 * \param TxIndex Index of message in DBAL_DBUS_Handler_TxObject.
 *
 * \return        Returns true, if assigning pointer values has worked, otherwise false.
*/
static bool DBAL_setCrMsgPtrs(DBALCR_NO_CONST struct DBAL_Instance* const Inst, uint8_t TxIndex)
{
    bool RetVal = false;

    switch(TxIndex)
    {
        case DBAL_MSG_INDEX_APP_LAYER_CONNECTION:
        case DBAL_MSG_INDEX_APP_LAYER_REQUEST:
        case DBAL_MSG_INDEX_APP_LAYER_RESPONSE:
            RetVal = true;
            break;
        case DBAL_MSG_INDEX_APP_LAYER_CROSS_CON:
            if((DBALCR_ConInst == Inst) || (DBALCR_ConInst == NULL))
            {
                RetVal = true;
                DBALCR_ConInst = Inst;
            }
            break;
        case DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ:
        case DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP:
            if((DBALCR_ReqRespInst == Inst) || (DBALCR_ReqRespInst == NULL))
            {
                RetVal = true;
                DBALCR_ReqRespInst = Inst;
            }
            break;
        default:
            /*Unknown index/message.*/
            break;
    }

    return RetVal;
}

/** \brief   Looks, whether the given DBal instance has queued a DBal Dbus2 request message for sending.
 *
 *  \return  Returns true, if this is the case, otherwise false.
 */
static bool DBAL_isReqQueued4Sending(const struct DBAL_Instance* const Inst)
{
    bool RetVal = false;
    const uint8_t BalMainInstMask = 2U;/*Bit 1*/
#ifdef DBAL_CROSS_CONNECTION
    const uint8_t BalUserInstMask = 16U;/*Bit 4*/
#endif/*DBAL_CROSS_CONNECTION*/

    if
    (
        (Inst == DBALIN_getMainInstance()) &&
        ((DBAL_DBUS_Handler_TxFlags[0] & BalMainInstMask) != 0U) &&
        (DBAL_DBUS_Handler_TxObject[DBAL_MSG_INDEX_APP_LAYER_REQUEST].tBusIdentifier.ucTargetAddress == Inst->DBUS_ComPartner)
    )
    {
        RetVal = true;
    }
#ifdef DBAL_CROSS_CONNECTION
    else if
    (
        (Inst != DBALIN_getMainInstance()) &&
        ((DBAL_DBUS_Handler_TxFlags[0] & BalUserInstMask) != 0U) &&
        (DBAL_DBUS_Handler_TxObject[DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ].tBusIdentifier.ucTargetAddress == Inst->DBUS_ComPartner)
    )
    {
        RetVal = true;
    }
#endif/*DBAL_CROSS_CONNECTION*/
    else
    {
        /*Not set, so return false.*/
    }

    return RetVal;
}
#ifdef DBAL_CROSS_CONNECTION
/** \brief     Looks, whether Bal Transmit flags are set for Dbus2 DBal connection messages.
 *
 * \details    Make sure, we do not switch to next instance, if message was updated, after being picked up.
 *
 * \param Inst DBal communication instance.
 */
static bool DBAL_areConTxFlagsSet(const struct DBAL_Instance* const Inst)
{
    bool RetVal = false;
    const uint8_t BalUserInstMask = 8U;/*Bit 3*/

    if
    (
        (Inst != DBALIN_getMainInstance()) &&
        ((DBAL_DBUS_Handler_TxFlags[0] & BalUserInstMask) != 0U)
    )
    {
        RetVal = true;
    }

    return RetVal;
}
#endif

/** \brief     Looks, whether Bal Transmit flags are set for Dbus2 DBal request/response messages.
 *
 * \details    Make sure, confirmation function of transmission does not delete data,
 *             if message was updated, while already being sent.
 *
 * \param Inst DBal communication instance.
*/
static bool DBAL_areReqRespTxFlagsSet(const struct DBAL_Instance* const Inst)
{
    bool RetVal = false;
    const uint8_t BalMainInstMask = 6U;/*Bit 1 and 2*/
    const uint8_t BalUserInstMask = 48U;/*Bit 4 and 5*/

    if
    (
        (Inst == DBALIN_getMainInstance()) &&
        ((DBAL_DBUS_Handler_TxFlags[0] & BalMainInstMask) != 0U)
    )
    {
        RetVal = true;
    }
    else if
    (
        (Inst != DBALIN_getMainInstance()) &&
        ((DBAL_DBUS_Handler_TxFlags[0] & BalUserInstMask) != 0U)
    )
    {
        RetVal = true;
    }
    else
    {
        /*Not set, so return false.*/
    }

    return RetVal;
}

/** \brief        Prepares entry in DBAL_DBUS_Handler_TxObject before transmission.
 *
 * \details       Sets length of data and Dbus2 address of communication partner.
 *
 * \param Inst    DBal communication instance.
 * \param TxIndex Index of message in DBAL_DBUS_Handler_TxObject.
 * \param DataLen Length of data to send.
*/
static void DBAL_prepareTxEntry(const struct DBAL_Instance* const Inst, uint8_t TxIndex, uint8_t DataLen)
{
    DBAL_DBUS_Handler_TxObject[TxIndex].tBusIdentifier.ucTargetAddress = Inst->DBUS_ComPartner;
    DBAL_DBUS_Handler_TxObject[TxIndex].tBusIdentifier.ucMessageLength = DataLen + BUS_MESSAGE_ID_LEN;
    DBAL_DBUS_Handler_TxObject[TxIndex].ucDataLen = DataLen;
}

/** \brief          Estimates index in DBAL_DBUS_Handler_TxObject for transmission of Dbus2 DBal message.
 *
 * \details         Depends on whether it is a connection message or not.
 * \details         Depends on type and count of DBal frames.
 * \details         See DBal spec.
 *
 * \param Inst      DBal communication instance.
 * \param FrameType Type of Dbus2 frame, that would be used, if we transmitted only the data currently added.
*/
static uint8_t DBAL_getTxIndex(const struct DBAL_Instance* const Inst, enum DBAL_Dbus2FrameType FrameType)
{
    uint8_t RetVal = DBAL_MSG_INDEX_COUNT;

    if(FrameType == DBAL_DBUS2_FRAME_TYPE_CON)
    {
        RetVal = DBAL_MSG_INDEX_APP_LAYER_CONNECTION;
    }
    else if((FrameType == DBAL_DBUS2_FRAME_TYPE_RESP) || (Inst->DbalFrames2TransmitCnt != 0U))
    {
        RetVal = DBAL_MSG_INDEX_APP_LAYER_RESPONSE;
    }
    else if(FrameType == DBAL_DBUS2_FRAME_TYPE_REQ)
    {
        RetVal = DBAL_MSG_INDEX_APP_LAYER_REQUEST;
    }
    else
    {
        DBAL_ERROR("Type not known %u, Addr %x", FrameType, Inst->DBUS_ComPartner);/*Some error has occurred. Unknown message type.*/
    }

    if(Inst != DBALIN_getMainInstance())
    {
        RetVal += DBAL_MSG_INDEX_CROSS_OFFSET;
    }

    return RetVal;
}

/** \brief               Triggers sending of Dbus2 DBal connection frame.
 *
 * \param Inst           DBal communication instance.
 * \param ConMessageType DBAL_CON_ENABLE_REQUEST, DBAL_CON_ENABLE_RESPONSE,
 *                       DBAL_CON_DISABLE_REQUEST, DBAL_CON_DISABLE_RESPONSE
*/
static void DBAL_sendConnectionMessage(struct DBAL_Instance* const Inst, enum DBAL_ConnectionMessageType ConMessageType)
{
    uint8_t TxIndex = DBAL_getTxIndex(Inst, DBAL_DBUS2_FRAME_TYPE_CON);
    Inst->ConnectDataLen = DBAL_CON_MSG_LEN;

    if(TxIndex < DBAL_MSG_INDEX_COUNT)
    {
        DBAL_prepareGenericFrameHeader(Inst->ConnectTransmitBuffer);
        Inst->ConnectTransmitBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)ConMessageType;
        Inst->ConnectTransmitBuffer[DBAL_CON_MSG_PROTOCOL_VERSION] = (uint8_t)DBAL_PROTOCOL_VERSION;

        if(DBAL_setCrMsgPtrs(Inst, TxIndex) == true)
        {
            DBAL_prepareTxEntry(Inst, TxIndex, Inst->ConnectDataLen);
            BAL_vTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, TxIndex);
        }
#ifdef DBAL_CROSS_CONNECTION
        else
        {
            DBALCQ_queueConnectMsg(Inst);
        }
#endif/*DBAL_CROSS_CONNECTION*/
    }
    else
    {
        DBAL_ERROR("Invalid TxIndex %u, Addr %x", TxIndex, Inst->DBUS_ComPartner);/*Something went completely wrong.*/
    }
}

/** \brief     Checks, whether a message, for which a response would be expected, is already queued.
 *
 * \details    Queue is needed for repetition, in case there is no response.
 * \details    If it is a repetition, the message is already queued, otherwise not.
 *
 * \param      Inst      DBal communication instance.
 * \param      DBalType  See DBal spec.
 * \param      ServiceId See DBal spec.
 * \param      CommandId See DBal spec.
 * \param      Bytes     Payload.
 * \param      DataLen   Length of Payload.
 * \param[out] FreeIndex Index of first free entry.
 *
 * \return     Returns true, if message already queued, otherwise false.
*/
static bool DBAL_isMsg2RepeatSaved
(
    const struct DBAL_Instance* const Inst,
    enum DBAL_MessageType DBalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DataLen,
    uint8_t* const FreeIndex
)
{
    bool RetVal = false;
    *FreeIndex = DBAL_MAX_MSGS2REPEAT;

    for(uint8_t i = 0; i < (uint8_t)DBAL_MAX_MSGS2REPEAT; i++)
    {
        if
        (
            (Inst->MsgRptPtrs[i]->IsSlotOccupied == true) &&
            (Inst->MsgRptPtrs[i]->ServiceId == ServiceId) &&
            (Inst->MsgRptPtrs[i]->CommandId == CommandId) &&
            (Inst->MsgRptPtrs[i]->DbalType == DBalType) &&
            (Inst->MsgRptPtrs[i]->Datalen == DataLen) && /*Axivion Next Line MisraC2012-11.5 : No other choice than to do this conversion. Works fine.*/ /*Axivion Next Line MisraC2012-21.16 : Disallowed type of pointer argument. [void*] */
            (memcmp((const void*)Bytes, (const void*)Inst->MsgRptPtrs[i]->Data, (size_t)DataLen) == 0)
        )
        {
            RetVal = true;
            break;
        }
        else if
        (
            (Inst->MsgRptPtrs[i]->MsgRetryCounter == 0U) &&
            (Inst->MsgRptPtrs[i]->IsSlotOccupied == false)
        )
        {
            *FreeIndex = i;
            break;
        }
        else
        {
            /*Slot occupied, but not by the message to be sent.*/
        }
    }

    return RetVal;
}

/** \brief          Queues a message for repetition, in case there is no response.
 *
 * \param Inst      DBal communication instance.
 * \param DBalType  See DBal spec.
 * \param ServiceId See DBal spec.
 * \param CommandId See DBal spec.
 * \param Bytes     Payload.
 * \param DataLen   Length of Payload.
 * \param FreeIndex Index of first free entry.
 *
 * \return          Returns true, if message was queued successfully or if it is already queued,
 *                  otherwise false.
*/
static bool DBAL_saveMsg2Repeat
(
    struct DBAL_Instance* const Inst,
    enum DBAL_MessageType DBalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DataLen,
    uint8_t FreeIndex
)
{
    bool RetVal = false;

    if
    (
        (DataLen <= (uint8_t)DBAL_MAX_DATALEN_2REPEAT) &&
        (FreeIndex < (uint8_t)DBAL_MAX_MSGS2REPEAT) &&
        (Inst->MsgRptPtrs[FreeIndex]->MsgRetryCounter == 0U) &&
        (Inst->MsgRptPtrs[FreeIndex]->IsSlotOccupied == false)
    )
    {
        Inst->MsgRptPtrs[FreeIndex]->IsSlotOccupied = true;
        Inst->MsgRptPtrs[FreeIndex]->ServiceId = ServiceId;
        Inst->MsgRptPtrs[FreeIndex]->CommandId = CommandId;
        Inst->MsgRptPtrs[FreeIndex]->DbalType = DBalType;
        Inst->MsgRptPtrs[FreeIndex]->Datalen = DataLen;
        (void)memcpy(Inst->MsgRptPtrs[FreeIndex]->Data, Bytes, DataLen);/*lint !e920 cast ok in this case we do not need pointer*/
        STIM_ResetTimer(&Inst->MsgTimer);
        STIM_EnableTimer(&Inst->MsgTimer);
        RetVal = true;
    }

    return RetVal;
}

/** \brief           Writes the generic header and the current sequence id at the start of a
 *                   Dbus2 DBal request/response frame.
 *
 * \param Inst       DBal communication instance.
 * \param Repetition Number of sending repetition - zero is initial attempt.
 *
*/
static void DBAL_prepareHeaderAndSeqId(struct DBAL_Instance* const Inst, uint8_t Repetition)
{
    if(Inst->DbalFrames2TransmitCnt == 0U)
    {
        DBAL_prepareGenericFrameHeader(Inst->TransmitBuffer);
        Inst->TransmitDataLen = DBAL_MSG_FRAME_OFFSET;
    }
    if(Repetition == 0U)
    {
        Inst->SeqId2Send++;
    }
    Inst->TransmitBuffer[DBAL_MSG_SEQID] = Inst->SeqId2Send;
}

/** \brief           Adds a DBal frame to the Dbus2 DBal request/response frame to be sent.
 *
 * \details          If needed, DBal frame is queued for repetition (in case of missing response).
 * \details          If there is no space in queue for repetition left, DBal frame is NOT appended.
 *
 * \param From       Place in transmission buffer, where appended DBal frame is to start.
 * \param DBalType   See DBal spec.
 * \param ServiceId  See DBal spec.
 * \param CommandId  See DBal spec.
 * \param Bytes      Payload.
 * \param DataLen    Length of Payload.
 * \param Repetition Number of repetition. - Zero is initial attempt.
 *
 * \return          Returns true, if there was enough space in tx buffer to append DBal frame
 *                  AND, if needed, a free slot in the queue for repetition; otherwise false.
*/
static bool DBAL_appendDBalFrame2ReqRespFrame
(
    struct DBAL_Instance* const Inst,
    uint8_t From,
    enum DBAL_MessageType DBalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DataLen,
    uint8_t Repetition
)
{
    bool RetVal = true;

    if((uint16_t)((uint16_t)From + (uint16_t)DBAL_FRAME_DATA_OFFSET + (uint16_t)DataLen) >= (uint16_t)DBAL_BUFFER_SIZE)
    {
        RetVal = false;/*No space in sending buffer.*/
        DBAL_WARNING("Buffer too small %u bytes, needed %u, Addr %x", DBAL_BUFFER_SIZE, (From + DBAL_FRAME_DATA_OFFSET + DataLen), Inst->DBUS_ComPartner);
    }
    else
    {
        if(DBAL_isMsgOfDBalType2Repeat(DBalType) == true)
        {
            uint8_t FreeQueueIndex = DBAL_MAX_MSGS2REPEAT;
            if(DBAL_isMsg2RepeatSaved(Inst, DBalType, ServiceId, CommandId, Bytes, DataLen, &FreeQueueIndex) == false)
            {
                RetVal = DBAL_saveMsg2Repeat(Inst, DBalType, ServiceId, CommandId, Bytes, DataLen, FreeQueueIndex);
            }
            else if(Repetition == 0U)
            {
                DBAL_INFO("Already being sent. ServiceId 0x%x, CommandId 0x%x, Addr %x", ServiceId, CommandId, Inst->DBUS_ComPartner);
                RetVal = false;
            }
            else
            {
                /*Nothing to do. Send repetition.*/
            }
        }

        if(RetVal == true)
        {
            Inst->TransmitBuffer[From] = (uint8_t)DBalType;
            Inst->TransmitBuffer[From + DBAL_FRAME_PAYLOADLEN] = DataLen;
            Inst->TransmitBuffer[From + DBAL_FRAME_SERVICE_ID_HI] = (uint8_t)(ServiceId >> BYTE_SIZE);
            Inst->TransmitBuffer[From + DBAL_FRAME_SERVICE_ID_LO] = (uint8_t)ServiceId;
            Inst->TransmitBuffer[From + DBAL_FRAME_COMMAND_ID_HI] = (uint8_t)(CommandId >> BYTE_SIZE);
            Inst->TransmitBuffer[From + DBAL_FRAME_COMMAND_ID_LO] = (uint8_t)CommandId;

            DBAL_storePayloadToTransmitBuffer(Inst->TransmitBuffer, From + DBAL_FRAME_DATA_OFFSET, Bytes, DataLen);
            Inst->TransmitDataLen += DBAL_FRAME_DATA_OFFSET + DataLen;
        }
        else
        {
            /*Queue too small or message already being sent.*/
            DBAL_WARNING("Did not manage to save message for repetition. ServiceId 0x%x, CommandId 0x%x, Addr %x", ServiceId, CommandId, Inst->DBUS_ComPartner);
        }
    }

    return RetVal;
}

/** \brief          Triggers the entire sequence needed to send a DBal frame embedded into a
 *                  Dbus2 DBal request/response frame.
 *
 * \param Inst      DBal communication instance.
 * \param DBalType  See DBal spec.
 * \param ServiceId See DBal spec.
 * \param CommandId See DBal spec.
 * \param Bytes     Payload.
 * \param DataLen   Length of Payload.
 * \param Repetition Number of repetition. - Zero is initial attempt.
 *
 * \return          Returns true, if everything worked, otherwise false.
*/
static bool DBAL_ioDbusHandler_send
(
    struct DBAL_Instance* const Inst,
    enum DBAL_MessageType DbalType,
    uint16_t ServiceId,
    uint16_t CommandId,
    const uint8_t* const Bytes,
    uint8_t DataLen,
    uint8_t Repetition
)
{
    bool RetVal = false;

    if
    (
        (DBAL_getCommStateByInstance(Inst) == DBAL_COMMSTATE_NOT_READY) ||
        ((Bytes == NULL) && (DataLen != 0U))
    )
    {
        DBAL_INFO("Reject send request, CommState %u, Addr %x", DBAL_getCommStateByInstance(Inst), Inst->DBUS_ComPartner);
    }
    else
    {
        uint8_t TxIndex = DBAL_getTxIndex(Inst, DBAL_getDbus2FrameTypeFromDBalType(DbalType));

        if(TxIndex < DBAL_MSG_INDEX_COUNT)
        {
            if
            (
                (Inst->DbalFrames2TransmitCnt == 0U) ||
                ((Inst != DBALIN_getMainInstance()) && (DBAL_areReqRespTxFlagsSet(Inst) == false))
            )
            {
                DBAL_prepareHeaderAndSeqId(Inst, Repetition);
            }

            if(DBAL_isReqQueued4Sending(Inst) == true)
            {/*In case we switch from Request to Response TxIndex, delete Tx flag for request here, so we do not send outdated message.*/
                BAL_vCancelTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
            }

            if(DBAL_appendDBalFrame2ReqRespFrame(Inst, Inst->TransmitDataLen, DbalType, ServiceId, CommandId, Bytes, DataLen, Repetition) == true)
            {
                Inst->DbalFrames2TransmitCnt++; /*Increase message counter - in case further Dbal Frame should be send in same Dbus2 frame.*/

                if(DBAL_setCrMsgPtrs(Inst, TxIndex) == true)
                {
                    if
                    (
                        (((DBAL_areReqRespTxFlagsSet(Inst) == true) || (Inst->DbalFrames2TransmitCnt == 1U)) &&
                        (DBAL_isIoTxRepeatBufferEmpty(Inst) == true)) ||
                        (Inst != DBALIN_getMainInstance())
                    )
                    {
                        Inst->SendRetryCounter[TxIndex] = 0;/*For updated message, full number of attempts.*/
                        DBAL_prepareTxEntry(Inst, TxIndex, Inst->TransmitDataLen);
                        BAL_vTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, TxIndex);
                    }
                }
#ifdef DBAL_CROSS_CONNECTION
                else
                {
                    DBALCQ_queueReqRespMsg(Inst, TxIndex);
                }
#endif/*DBAL_CROSS_CONNECTION*/
                RetVal = true;
            }
        }
        else
        {
            DBAL_ERROR("Invalid TxIndex %u, Addr %x", TxIndex, Inst->DBUS_ComPartner);/*Something went completely wrong*/
        }
    }

    return RetVal;
}

/** \brief         Gets the last dbusdll transmission status of a message.
 *
 * \details        If the delivery of a message did not work,
 *                 further actions depend on the kind of error, see DBal spec.
 * \details        DLL_ACK_NOT_RECEIVED:
 *                 Disable connection and try to wake up communication partner before sending connection frame.
 * \details        Other error: Just send message again.
 * \details        DLL_ACK_OK: Correctly delivered. No need for further action.
 *
 * \param Inst     DBal communication instance.
 * \param MsgIndex Index of message in DBAL_DBUS_Handler_TxObject.
 *
 * \return         Returns value of status: DLL_ACK_NOT_RECEIVED, DLL_ACK_BUSY, DLL_ACK_WRONG,
 *                                          DLL_ACK_OK, DLL_ACK_TRANSMISSION_ABORTED
*/
static uint8_t DBAL_getLastSendingStatus(const struct DBAL_Instance* const Inst, uint8_t MsgIndex)
{
    if(MsgIndex < DBAL_DBUS_Handler_NumberOfTxObjects)
    {
        return Inst->LastSendingStatus[MsgIndex];
    }

    return DLL_ACK_NOT_RECEIVED;
}
