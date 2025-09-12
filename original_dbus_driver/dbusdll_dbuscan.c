/*******************************************************************************
 *   Copyright (c) 2022 BSH Hausgeraete GmbH,
 *   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *   All rights reserved. This program and the accompanying materials
 *   are protected by international copyright laws.
 *   Please contact copyright holder for licensing information.
 *
 ********************************************************************************
 *   PROJECT          DBUS
 *   COMP_ABBREV      DLL
 *******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief   Implementation of Data link layer for D-Bus-2
 *
 *     This layer handles the message reception and transmission close to the bus-hardware,
 *     handling the following:
 *             - wrapping the message data into a frame as described below, which includes message length,
 *               TargetAddress, MessageIdentifier, the data of the message and the attached CRC (16 bit),
 *               all followed by an acknowledgement from the receiving node (or sending node by broadcasts)
 *
 *     Frame: \image html dbusdll.gif
 */

/*
 *     ___________________________________________________________________________
 *     |Msg.Len|   TA  |     MsgID     |    DATA ...             |      CRC       |
 *     ___________________________________________________________________________
 *NoOfBits  8       8         16           (Msg.Len-2)*8                16
 *
 *     |------------------CRC-Calculation------------------------|
 *****************************************************************************************************************
 *   CHANGES
 *
 * %PL%
 *
 **************************************************************************************************************/
#ifdef DOXY_DEVELOPERS_DOC
/**
Compiler switch, which is defined for Update Service Messages without HSI.
*/
#define DBUS2_UPDATE
/**
Compiler switch, which is defined for Update Service Messages with HSI.
*/
#define DBUS2_UPDATE_HSI
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibDefines.h"
#include "LibTypes.h"
#include "bustypes.h"
#include "hsup.h"
#include "dbusdll.h"
#include "dbuspresentation.h"
#include "bal.h"
#include "dbuscan_drv.h"
#include "dbuscan_dbus.h"
#include "dbus_rtos_interface.h"
#include "system_timer.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define DLL_GET_IRQ_TIMEOUT               (uint32_t)(STIM_TIME_SEC(1u))  //!< Timeout for readout interrupt events from DBusCAN chip.

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/
/* Note 9046: 'DLL_States::DLL_INIT' is typographically ambiguous with respect to 'DLL_init' when ignoring case [MISRA 2012 Directive 4.5, advisory] */
/*lint -esym(9046,DLL_init) */

// Axivion Disable Style MisraC2012-11.5

/** Possible states for data link layer task handler.
 */
enum DLL_States
{
    DLL_INIT,
    DLL_IDLE,
    DLL_MSG_TO_SEND,
    DLL_WAIT_FOR_MSG_DELIVERY,
    DLL_OFFLINE
};

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
static void DLL_msgTransmitted(uint8_t acknowledge, uint16_t userCode);
static int32_t DLL_getIrqTimeoutCallback(void *obj, uint32_t flags, int32_t data);
static bool DLL_init(void);
static uint16_t DLL_calculateCrc(const uint8_t *data, uint16_t size);
static uint16_t DLL_addByteToCrcCalculation(uint16_t crc, uint8_t value);
static bool DLL_isCorrectCrc(const TbusMessage *rxMsg);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static enum DLL_States DLL_taskState = DLL_INIT;       //!< Task state of module DLL, Data Link Layer.
static struct STIM_Timer DLL_getIrqTimeout;            //!< Timer for regular readout of interrupt events from DBusCAN chip
static uint8_t DLL_receivingNodeAddress;               //!< Variable containing the address of the receiving address, this is needed to be able to supply the overlaying layer with this information in the transmission complete function (instead of returning only DLL_ACK_NOT_RECEIVED (without any node information).
static uint16_t DLL_userCode;                          //!< DLL_userCode is any code/number/index, which the overlaying layer associates with this specific message (returned by the message completion function).
static bool DLL_isSilent = false;                      //!< Indicates, whether dbus is in Silent Mode.
#ifndef RTOS_DBUS_EVENTDRIVEN
static volatile bool DLL_isDbuscanIrqPending = false;  //!< Indicates a pending interrupt from DBusCAN chip.
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
#ifdef RTOS_DBUS_EVENTDRIVEN

bool DLL_moduleInit(void)
{
    bool isInit;
    static struct STDCB_Callback cbGetIrqTimeout;

    if(DLL_init() != false)
    {
        (void)STIM_InitCallback(&cbGetIrqTimeout, DLL_getIrqTimeoutCallback, NULL, STIM_STATUS_TRIGGERED);
        (void)STIM_InitTimer(&DLL_getIrqTimeout, STIM_PROCESSING_SCHEDULER, DLL_GET_IRQ_TIMEOUT, STIM_MODE_PERIODIC, false, &cbGetIrqTimeout);
        STIM_EnableTimer(&DLL_getIrqTimeout);
        DLL_taskState = DLL_IDLE;
        isInit = true;
    }
    else
    {
        isInit = false;
    }

    return isInit;
}

bool DLL_sendingInit(void)
{
    bool retState = false;

    if(true == DBCDBUS_sendFrame())
    {
        DLL_taskState = DLL_WAIT_FOR_MSG_DELIVERY;
        retState = true;
    }
    return retState;
}

void DLL_sendingDone(void)
{
    uint8_t ack = DBCDBUS_getReceivedAck();
    DLL_vMessageTransmitted_UserCallback(DLL_receivingNodeAddress, ack, DLL_userCode);
    DLL_msgTransmitted(ack, DLL_userCode);
    DLL_taskState = DLL_IDLE;
}

bool DLL_handleInterrupt(void)
{
    const bool retState = DBCDBUS_handleIrq();
    if(true == retState)
    {
        STIM_ResetTimer(&DLL_getIrqTimeout);
        if(DBCDRV_isPowerOnReset())
        {
            // Re-initialization of DBusCAN is necessary
            DBCDRV_disableIrq();
            STIM_DisableTimer(&DLL_getIrqTimeout);
            DBR_RunEventdrivenDbusTask(DBR_ED_TASK_DLL);
            DLL_taskState = DLL_INIT; // Re-initialization secured by changing the state
        }
    }
    return retState;
}

#else //!RTOS_DBUS_EVENTDRIVEN

uint8_t DLL_HandleTask(void)
{
    static struct STDCB_Callback cbGetIrqTimeout;

    switch (DLL_taskState)
    {
        default:
        case DLL_IDLE:
        {
            break;
        }
        case DLL_INIT:
        {
            if (true != DLL_init())
            {
                return TASK_NOT_INITIALISED;
            }
            #ifdef VARIANT_PROGRAMMER
            // Silent mode as default in programmer
            DLL_vSetSilentMode();
            DBPL_vSilentModeHasBeenEntered();
            #endif
            (void)STIM_InitCallback(&cbGetIrqTimeout, DLL_getIrqTimeoutCallback, NULL, STIM_STATUS_TRIGGERED);
            (void)STIM_InitTimer(&DLL_getIrqTimeout, STIM_PROCESSING_SCHEDULER, DLL_GET_IRQ_TIMEOUT, STIM_MODE_PERIODIC, false, &cbGetIrqTimeout);
            STIM_EnableTimer(&DLL_getIrqTimeout);
            DLL_taskState = DLL_IDLE;
            break;
        }
        case DLL_MSG_TO_SEND:
        {
            if (true == DBCDBUS_sendFrame())
            {
                DLL_taskState = DLL_WAIT_FOR_MSG_DELIVERY;
            }
            break;
        }
        case DLL_WAIT_FOR_MSG_DELIVERY:
        {
            if (true == DBCDBUS_isMsgTransmitted())
            {
                uint8_t ack = DBCDBUS_getReceivedAck();
                DLL_vMessageTransmitted_UserCallback(DLL_receivingNodeAddress, ack, DLL_userCode);
                DLL_msgTransmitted(ack, DLL_userCode);
                DLL_taskState = DLL_IDLE;
            }
            break;
        }
        case DLL_OFFLINE:
        {
            // enable event again to leave option for other protocols/goOnline to be received
            break;
        }
    }

    if (DBCDRV_isIrqEvent())
    {
        DLL_isDbuscanIrqPending = true;
    }
    if (DLL_isDbuscanIrqPending)
    {
        if (true == DBCDBUS_handleIrq())
        {
            DLL_isDbuscanIrqPending = false;
            STIM_ResetTimer(&DLL_getIrqTimeout);
            if (DBCDRV_isPowerOnReset())
            {
                #ifdef APP_VARIANT
                DBCDRV_disableIrq();
                #endif
                STIM_DisableTimer(&DLL_getIrqTimeout);
                DLL_taskState = DLL_INIT;
            }
        }
    }

    return TASK_INITIALISED; // i.e. Init finished
}
#endif //!RTOS_DBUS_EVENTDRIVEN

bool DLL_bIsBusReadyForTransmission(void)
{
    return (DLL_IDLE == DLL_taskState) ? true : false;
}

bool DLL_bTransmitMessage(TbusIdentifier tMessageID, TbusService tServiceFunc, uint8_t ucDataLength, uint16_t uiUserCode)
{
    const uint8_t dataBufSize = DLL_TRANSMIT_BUFFER_DATA_LENGTH;
    bool isUserMsg   = (tMessageID.tMessageIdentifier < BAL_FIRST_SERVICE_MSGID) ? true : false;
    uint8_t retryNum = (true == isUserMsg) ? BAL_ucMaxTxRetries : DBPL_ucGetMsgRepetitions();

    if (DLL_IDLE == DLL_taskState)
    {
        if (ucDataLength > dataBufSize)
        {
            // The data length is longer than the current transmit buffer, reduce to maximum buffer length.
            ucDataLength = dataBufSize;
        }
        DBCDBUS_txBuf.dbusTxfHdr.MSG_LEN = ucDataLength + BUS_MESSAGE_OVERHEAD;
        if(0u != retryNum)
        {
            retryNum--; // the first transmission is not counted as a retry in DBusCAN chip
        }
        DBCDBUS_txBuf.dbusTxfHdr.NUM_RTRY    = retryNum;
        DBCDBUS_txBuf.dbusTxfHdr.CRC         = DBC_DBUS_TXF_CRC_HW_CALCULATED;
        DBCDBUS_txBuf.dbusTxfHdr.TARGET_ADDR = tMessageID.ucTargetAddress;
        DBCDBUS_txBuf.bytes[DBC_DBUS_TXF_MSG_ID_HIGH] = (uint8_t)(tMessageID.tMessageIdentifier >> BYTE_SIZE);
        DBCDBUS_txBuf.bytes[DBC_DBUS_TXF_MSG_ID_LOW]  = (uint8_t)tMessageID.tMessageIdentifier;
        (*tServiceFunc)(ucDataLength, &DBCDBUS_txBuf.bytes[DBC_DBUS_TXF_DATA_OFFSET]);

        DLL_userCode = uiUserCode; // Save this parameter, and return when transmission finishes
        DLL_receivingNodeAddress = tMessageID.ucTargetAddress;

        DLL_taskState = DLL_MSG_TO_SEND;
        DBR_RunEventdrivenDbusTask(DBR_ED_TASK_DLL_TX_INIT);

        return true;
        // Message transmission initiated (if there is a need to know when the message has been transmitted, this information is returned via the confirmation function after (un)successful transmission).
    }
    return false; // Not possible to send at the present stage.
}

TbusMessage *DLL_ptReceiveMessage(void)
{
    TbusMessage *rxMsg = DBCDBUS_readFrame();
    if (NULL != rxMsg)
    {
        if (DLL_isSilentMode() && !DLL_isCorrectCrc(rxMsg)) //need to check msg CRC as DBusCAN chip is not able to check it in Silent mode
        {
            return NULL;
        }
        // adapt message ID to big endian format
        rxMsg->tBusIdentifier.tMessageIdentifier = HSUP_uiProcToBigEndian(rxMsg->tBusIdentifier.tMessageIdentifier);
    }
    return rxMsg;
}

void DLL_vReleaseDataFromReceivedMessage(void)
{
    HSUP_vNop(); // dummy function definition due to compatibility with upper DBus layers
}

uint8_t DLL_ucGetMainNodeAddress(void)
{
    return (uint8_t)DBC_DBUS_NODE_ADDRESS;
}

void DLL_vGoOffline(void)
{
    DLL_taskState = DLL_OFFLINE;
    if (DBPL_bIsOfflineMode())
    {   //GoOffline issued by upper DBus layer- enter just standby mode
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_STANDBY);
    }
    else
    {   //GoOffline issued by application- enter sleep mode
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_SLEEP);
    }
}

void DLL_vGoOnline(void)
{
    (void)DBCDRV_setPowerMode(DBC_POWER_MODE_NORMAL);
    DLL_taskState = DLL_IDLE;
}

/**Function to put silent mode active in Dbus2.2*/
void DLL_vSetSilentMode(void)
{
    (void)DBCDBUS_disableAck();
    DLL_isSilent = true;
}

/**Function to put silent mode out of action in Dbus2.2*/
void DLL_vUnsetSilentMode(void)
{
    (void)DBCDBUS_enableAck();
    DLL_isSilent = false;
}

/**Function to see silent mode status in Dbus2.2*/
bool DLL_isSilentMode(void)
{
    return DLL_isSilent;
}

/* Get Information, whether Dbus is either transmitting or receiving data, to know, if action can be started, that would disturb Dbus, such as disabling interrupts.*/
bool DLL_bIsDbusCommunicating(void)
{
    return ((DLL_taskState > DLL_IDLE) && (DLL_taskState < DLL_OFFLINE)) ? true : false;
}

uint8_t DLL_ucGetStandardUartConfigIndex(void)
{
    return 0u;  // dummy function definition due to compatibility with upper DBus layers
}

#ifndef RTOS_DBUS_EVENTDRIVEN
void DLL_setDbuscanIrqPending()
{
    DLL_isDbuscanIrqPending = true;
}
#endif //RTOS_DBUS_EVENTDRIVEN

bool DLL_bIsCurrentNode(uint8_t ucAddress)
{
    uint8_t nodeAddr = ucAddress >> NIBBLE_SIZE;
    uint8_t subnodeAddr = DLL_GET_SUBSYSTEM(ucAddress);
    DBC_Cfg_t cfg = DBCDRV_getConfig();
    if ((nodeAddr == cfg.NODE_ID) && (subnodeAddr == cfg.SUBNODE_ID))
    {
        return true;
    }
    else
    {
        #ifdef APP_VARIANT
        for (uint8_t i = 0u; i < DBC_DBUS_NF_COUNT; i++)
        {
            if ((0u != DBCDBUS_addressList[i].nodeAddress) && (nodeAddr == DBCDBUS_addressList[i].nodeAddress))
            {
                uint16_t subsystMask = (uint16_t)1u << subnodeAddr;
                if (0u != (subsystMask & DBCDBUS_addressList[i].subsystMask))
                {
                    return true;
                }
            }
        }
        #endif // APP_VARIANT
        return false;
    }
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/**
 This function has the opposite message flow of the other functions in DataLinkLayer.
 It is called by data link layer to signalize the layer above that the message has been successfully transmitted
 It is up to the overlaying module, what should be done in this function.

\param acknowledge
Acknowledge from the receiver (or in case of broadcast, the sender)\n
\b type       : uint8_t\n
\b range      : DLL_ACK_NOT_RECEIVED, DLL_ACK_BUSY, DLL_ACK_WRONG and DLL_ACK_OK

\param userCode
The user code is any code/index/numerical value given to DLL from the overlaying layer.
 Hence, this is the confirmation that exactly the message x
 has been transmitted successfully\n
\b type       : uint16_t\n
\b range      : to be defined by the overlaying layer

\return
\b type       : void

*/
static void DLL_msgTransmitted(uint8_t acknowledge, uint16_t userCode)
{
    TbusConfirmationService confirmationFunction = NULL; //Stack variable for optimising the call of the confirmation function (including access via transmit table)

    if (userCode <= (uint16_t)BAL_FIRST_SERVICE_MSGID)/*Sent via BAL*/
    {
        // This is a user message
        if (DLL_GET_ACKNOWLEDGE(acknowledge) == DLL_ACK_OK)
        {
            if(BAL_tBusObject[(userCode>>BYTE_SIZE)].ptBusTransmitTable != NULL)
            {
                confirmationFunction = *BAL_tBusObject[(userCode>>BYTE_SIZE)].ptBusTransmitTable[(uint8_t)userCode].tConfirmationFunction;
                if (confirmationFunction != NULL)//lint !e774 Call confirmation function, if any (i.e. not 0) is defined.
                {
                    confirmationFunction();
                }
            }
        }
        else
        {
            BAL_vNotifyNonDeliverableMessage(BAL_tBusObject[(userCode>>BYTE_SIZE)].ucSubsystem, (uint8_t)userCode);
        }
    }
    else
    {
        // This is a service message
        if (DLL_GET_ACKNOWLEDGE(acknowledge) != DLL_ACK_OK)
        {
            if(DBPL_bIsSendingPowerMsg() == true)
            {
                DBPL_vNotifyNonDeliverablePowerMessage(DBPL_uiGetPowerMsgUserTrigger());
            }
        }
        DBPL_vStopRepeatServiceMsg();
    }
}

/** Callback function for regular readout of interrupt events from DBusCAN chip */
/*lint -esym(818,obj) Pointer parameter 'obj' could be declared as pointing to const [MISRA 2012 Rule 8.13, advisory] */
static int32_t DLL_getIrqTimeoutCallback(void *obj, uint32_t flags, int32_t data)
{
    if (obj == NULL) {}
    (void)flags; (void)data;

    #ifndef RTOS_DBUS_EVENTDRIVEN
    DLL_isDbuscanIrqPending = true;
    #endif
    DBR_RunEventdrivenDbusTask(DBR_ED_TASK_DLL_ISR);

    return 0;
}

/** Initialize DLL layer */
static bool DLL_init(void)
{
#ifdef APP_VARIANT
    if (DBC_OK != DBCDRV_init(DLL_dbuscanIrqCallback))
#else
    if (DBC_OK != DBCDRV_init(NULL))
#endif
    {
        return false;
    }
    DLL_vSetupSavedBaudRate();
    return true;
}

static uint16_t DLL_calculateCrc(const uint8_t *data, uint16_t size)
{
    uint16_t crc = DLL_CRC_INIT;
    while (size-- > 0u)
    {
        crc = DLL_addByteToCrcCalculation(crc, *data);
        data++;
    }
    return crc;
}

static uint16_t DLL_addByteToCrcCalculation(uint16_t crc, uint8_t value)
{
   uint8_t crcLow;
   uint8_t crcHigh;
   /* Values specific for the CRC algorithm (XMODEM) */
   const uint8_t DLL_CRC_3 = 3u;
   const uint8_t DLL_CRC_5 = 5u;

   crcHigh  = (uint8_t)crc;
   crcLow   = (uint8_t)(crc >> BYTE_SIZE) ^ value;
   crcLow  ^= (uint8_t)(crcLow >> NIBBLE_SIZE);
   crcHigh ^= (uint8_t)(crcLow << NIBBLE_SIZE);
   crcHigh ^= (uint8_t)(crcLow >> DLL_CRC_3);
   crcLow  ^= (uint8_t)(crcLow << DLL_CRC_5);
   return (((uint16_t)crcHigh << BYTE_SIZE) | crcLow);
}

static bool DLL_isCorrectCrc(const TbusMessage *rxMsg)
{
    uint16_t rxMsgSize     = (uint16_t)rxMsg->tBusIdentifier.ucMessageLength + BUS_MESSAGE_OVERHEAD;
    uint16_t rxMsgDataSize = (uint16_t)rxMsg->tBusIdentifier.ucMessageLength - BUS_MESSAGE_ID_LEN;
    uint16_t rxMsgCrc      = (((uint16_t)rxMsg->aucData[rxMsgDataSize] << BYTE_SIZE) | rxMsg->aucData[rxMsgDataSize + 1u]);
    uint16_t rxMsgCrcCalc  = DLL_calculateCrc((const uint8_t*)(const void*)rxMsg, rxMsgSize);
    return (rxMsgCrc == rxMsgCrcCalc) ? true : false;
}
