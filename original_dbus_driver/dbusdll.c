/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          IC Dbus
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
 *             - wrapping the message data into a frame as described below, which includes message legth,
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
Local compiler switch.
When defined several variables are defined for statistical purposes.
*/
#define LSW_DLL_INCLUDE_STATISTICS

/**
Compiler switch, which is defined in the make file for controllers, which need their LittleEndian representation of integers to be adapted to the D-Bus-2 used format: BigEndian.

This switch should be set for all LittleEndian controllers (e.g. Renesas) in order for the message identifier to be transmitted in BigEndian format on D-Bus-2. Likewise it is important for LittleEndian controllers to adapt the received BigEndian message identifier to LittleEndian format for correct internal representation.
*/
#define __LITTLE_ENDIAN_ADAPTATION

/**
Local compiler switch.
When defined an array, DLL_ulMonitorDistributionOfCollisions, DLL_LENGTH_OF_COLLISION_DISTRIBUTION_ARRAY uint32s are used for monitoring the distribution of collisions - each time an idle state is detected (after every frame), the current value of DLL_ucSucceedingCollisions is noted by incrementing the correpsonding element of this array.
*/
#define LSW_DLL_MONITOR_DISTRIBUTION_OF_COLLISIONS
/**
Compiler switch, which is defined for Update Service Messages without HSI.
*/
#define DBUS2_UPDATE
/**
Compiler switch, which is defined for Update Service Messages with HSI.
*/
#define DBUS2_UPDATE_HSI
#endif

#include "bsh_stdinc.h"
#include "LibDefines.h"
#include "LibTypes.h"
#include "bustypes.h"
#include "dbusmapping.h"
#include "hsup.h"  //Needed for HSUP_HIGH_BYTE, HSUP_LOW_BYTE and HSUP_uiProcToBigEndian
#include "dbusdll.h"
#include "dbuspresentation.h"
#include "dbuspresentation_update.h"
#include "bal.h"


/* LOCAL DEFINITIONS */

/* Timing */
#define DLL_STD_BIT_TIME               __BIT_TIME  //!<Duration of one bit in micro seconds.

#define DLL_BIT_TIME_CAPPED            26U         //!<Fixed bit time for 38400 baud, needed for ACK timeout cap in higher baudrates

#define DLL_IDLE_STATE_DETECTION_TIME  ((uint8_t)10U)      //!<Time for detecting idle state on the bus - value in bit times.
#define DLL_INTERBYTE_TIME_OUT         ((uint8_t)(10U+9U)) //!<Time for detecting interbyte time-out on the bus - value in bit times. Time between end of one received byte and beginning of the next received byte must be smaller than transmission time of one byte, hence the time from start of one byte till start of next byte is maximum 10 bit times for transmission of the byte + 9 bit times latency time.
#define DLL_ACK_GENERATION_TIMEOUT     ((uint8_t)(9U))     //!< Timeout definition between activation of Tx-interrupt for generation of Ack and the occurrence of this (hence the difference of 10 bit times as compared to the timeout definition used for RX-direction, which includes the duration of one character.

/*Status*/
#define DLL_STATUS_BROADCAST_BIT                      ((uint8_t)0x01U)//!< Bit used to signalise Broadcast message.
#define DLL_STATUS_OVERFLOW_BIT                       ((uint8_t)0x02U)//!< Bit used to signalise buffer overflow upon receiving a message. No message is received until at least one message has been read.
#define DLL_STATUS_WRONG_CRC_BIT                      ((uint8_t)0x04U)//!< Bit used to signalise that the calculated crc does not correspond to the received crc.
#define DLL_STATUS_MESSAGE_FOR_THIS_NODE_BIT          ((uint8_t)0x08U)//!< Bit used to signalise that the message was addressed to this node.
#define DLL_STATUS_READY_TO_INITIATE_TRANSMISSION_BIT ((uint8_t)0x10U)//!< Bit used to signalise that the random waiting time is finished, hence transmission may be initiated.
#define DLL_STATUS_MESSAGE_TRANSMISSION_BIT           ((uint8_t)0x20U)//!< Bit used to signalise that the present message is being sent, not received.
#define DLL_STATUS_D_BUS_3_TRANSMISSION_BIT           ((uint8_t)0x40U)//!< Bit used to signalise that a D-Bus-3 message is being sent and shall be ignored.

/**
Definition for dummy message. The data length 255 is invalid, because it is defined to be too long, but this specific value is used to signalise the end of the ring buffer. NB! This length (255) cannot be covered by representing the message length as only one byte (as according to the specification), as the effectual frame length is 2 byte longer than the message length signalised by this value: ucMessageLength (1 byte) + TargetAddress (1 byte) + Message (ucMessageLength).
*/
#define DLL_DUMMY_MESSAGE_MESG_LEN     0xFFU //!< Message Lenght used as marker for end of input buffer (no valid message behind this byte -> wrap-around)
#define DLL_BROADCAST                  0U    //!< Broadcast addressing.
#define DLL_MINIMUM_MESSAGE_LENGTH     2U    //!< A message with message length less than 2 does not contain a valid identifier. This message will be discarded.

#define DLL_MAX_SUCCEEDING_COLLISIONS    ((uint8_t)0xFFU)     //!< The maximum value, which can be kept in the byte counting the succeeding collisions.
#define DLL_ALIGNMENT_BASE               2U    //!< The base for aligning 16 bit systems is 2.


// Axivion Disable Style MisraC2012-11.5

/* Violation of naming convention due to historic code*/
struct TBusMessageXS
{
   TbusIdentifier tBusIdentifier;
   uint8_t aucData[DLL_TRANSMIT_BUFFER_DATA_LENGTH];
};


union TByteCounter /*lint !e9018 "union should not be used" MisraC2012 19.2 advisory*/
{
  uint8_t ucCharactersToWrite;
  uint8_t ucRxDataLen;
  uint8_t ucAcknowledge;
};     //!< Union used to hold either CharactersToWrite during transmission, RxDataLen during reception or the value of the acknowledgement before transmitting this.


/** Possible states for data link layer task handler.
*/
enum DLL_States
{
    DLL_INIT_STATE,
    DLL_IDLE_STATE,
    DLL_MESSAGE_TO_RECEIVE_GET_RECIPIENT,
    DLL_MESSAGE_TO_RECEIVE,
    DLL_MESSAGE_TO_SEND,
    DLL_SEND_ACK,
    DLL_WAIT_FOR_ACK,
    DLL_OFFLINE
};

enum DLL_MessageBytes
{
    DLL_MSG_BYTE_LENGTH,
    DLL_MSG_BYTE_ADDRESS,
    DLL_MSG_BYTE_ID_HI,
    DLL_MSG_BYTE_ID_LO,
    DLL_MSG_DATA_OFFSET
};


static uint8_t DLL_aucMessageInputBuffer[DLL_MESSAGE_INPUT_BUFFER_SIZE]; //!< The input receive buffer.

static uint16_t DLL_uiBitTime  =  DLL_STD_BIT_TIME;  //!< BIT_TIME. This is just a numerical value, representing the bit time in micro seconds for 9600 BAUD - must be adjusted for other baud-rates than 9600 - get the value from HAL.
static uint16_t DLL_uiIdleStateDetectionTime = (uint16_t)((uint16_t)DLL_IDLE_STATE_DETECTION_TIME * (uint16_t)DLL_STD_BIT_TIME);  //!< DLL_IDLE_STATE_DETECTION_TIME (10*BIT_TIME) 10 bit times must be added as the duration of the received byte must also be included (as the time-out is calculated between end of presently received byte, and end of previously received byte.

/* For reception and transmission different timeout values should be used. Allow messages with 9.5 bit times interbyte time to be received. To make sure no message is acknowledged after 9 bit times, use 8.5 bit times as limit in transmit event handler. Please note that the correct values for the following definitions is 19.5 bit times for RX (taking into consideration that the transmission of one character takes 10 bit times), but 8.5 bit times for TX check - considering only the timeout value (i.e. the latency time) is to be checked. */
static uint16_t DLL_uiInterbyteTimeoutRx   = (uint16_t)((uint16_t)((uint16_t)DLL_INTERBYTE_TIME_OUT * (uint16_t)DLL_STD_BIT_TIME) + (uint16_t)((uint16_t)DLL_STD_BIT_TIME>>(uint16_t)1U));   //9.5 bit times interbyte time allowed to limit the effect of jitter between t0 and t caused by interrupt latency - adding up the transmission time of one character, this gives us the value 19.5 bit times for this definition.

#if DLL_STD_BIT_TIME < DLL_BIT_TIME_CAPPED //if the bit time is lower than 26 ms (e.g. baudrate is higher than 38400 baud) then the ACK timeout is capped at 8.5 bit times @ 38400 baud
    static uint16_t DLL_uiAckGenerationTimeout = (uint16_t)(((uint16_t)DLL_ACK_GENERATION_TIMEOUT * (uint16_t)DLL_BIT_TIME_CAPPED) - ((uint16_t)DLL_BIT_TIME_CAPPED>>(uint16_t)1U));   //8.5 bit times interbyte time allowed before ACK generation to limit the effect of jitter between t0 and t caused by interrupt latency - and to assure that all other nodes are the same opinion concerning ACK timeout.
#else
    static uint16_t DLL_uiAckGenerationTimeout = (uint16_t)((uint16_t)((uint16_t)DLL_ACK_GENERATION_TIMEOUT * (uint16_t)DLL_STD_BIT_TIME) - (uint16_t)((uint16_t)DLL_STD_BIT_TIME>>(uint16_t)1U));   //8.5 bit times interbyte time allowed before ACK generation to limit the effect of jitter between t0 and t caused by interrupt latency - and to assure that all other nodes are the same opinion concerning ACK timeout.
#endif

/* Prototypes for static functions */
static bool DLL_bInit(void);
static void DLL_vEnterIdleState(void);
static void DLL_vCrcInit(void);
static void DLL_vAddByteToCrcCalculation(uint8_t ucByte);
static uint16_t DLL_uiGetCalculatedCrc(void);
static void DLL_vAddCollision(void);
static bool DLL_bIsBusIdle(void);

/* LOCAL VARIABLES and VARIABLES DEFINED IN THE EXCESS TEMPLATE */

/*lint -save -e9018 -e708 union used to prevent suspicious casts */
static union TByteCounter DLL_tUnionCounter = {0U}; //!< Union having the meaning of: How many bytes to write (for transmission), how many bytes to receive for reception and acknowledgement for acknowledgement generation.
/*lint -restore -e9018 -e708 */

static uint8_t DLL_ucTaskState = 0U; //!< Task state of module DLL, Data Link Layer.
static uint8_t DLL_ucStatus = 0U; //!< Status of the present message reception/transmission.

/* For reception */
/* DLL_aucMessageInputBuffer (message based) */
static uint8_t DLL_ucCharactersInRxMessageBuffer = 0U; //!< How many characters are in the input ring buffer
static uint8_t DLL_ucDataLengthOfReceivedMessage = 0U;  //!< Needed by DLL_psReceiveMessage() and DLL_vReleaseDataFromReceivedMessage(). Please note that the data length does not contain the target address and the data length of the message, as this would exceed 255 bytes for the dummy message.
static uint8_t DLL_ucRxNextCharacterToRead = 0U; //!< Offset index where to get next message in the input ring buffer.
//Variables defined directly in DLL_vHandleRxEvent(): DLL_ucRxNextCharacterToWrite and DLL_ucReceivingOffset,
/* For transmission */
static uint8_t *DLL_pucNextCharToWrite; //!< Next byte, which will be sent
/*lint -e{844} cannot be declared to const without creating other SCA messages */
static uint8_t *DLL_pucNextCharToCheck; //!< Next byte, which will be checked. This byte must correspond to the byte pointed to by DLL_pucNextCharToWrite, otherwise a collision has occured.
static uint8_t DLL_ucCharactersToCheck; //!< This is a counter to keep track of how many bytes are left to be checked (i.e. how many bytes are left to be sent and checked).
#ifdef __LITTLE_ENDIAN_ADAPTATION
static bool DLL_bLittleEndianIdAdaptation = false;
#endif

/** Succeeding collisions, limited at 255, this variable may not be removed, even if no statistics are needed, as the variable is needed by the idle-time recognition.
If during a transmission a negative acknowledgement is reported, or the acknowledgement is missing, the collision counter is not affected - as this is not a problem of colliding transmissions (the problem could be e.g. EMC or input buffer of receiver is full).
*/
static uint8_t DLL_ucSucceedingCollisions = 0U;
#ifdef LSW_DLL_INCLUDE_STATISTICS
/* Bus statistics (Error Counters) */
/* Implementing these error counters costs about 80 bytes of ROM */
static uint8_t DLL_ucMaxSucceedingCollisions = 0U;     //!< Counted maximum succeeding collisions
static uint32_t DLL_ulAccumulatedCollisions = 0U;      //!< Counted accumulated succeeding collisions
static uint32_t DLL_ulCrcErrorCounter = 0U;            //!< Counted CRC errors
static uint32_t DLL_ulAckTimeoutCounter = 0U;          //!< Counted ack time-outs
static uint32_t DLL_ulOwnAckTimeoutCounter = 0U;       //!< Counted time-outs in own ack
static uint32_t DLL_ulNackCounter = 0U;                //!< Counted negative acknowledgements
static uint32_t DLL_ulInterbyteTimeoutCounter = 0U;    //!< counted interbyte time-outs
#endif
static uint8_t DLL_ucReceivingNodeAddress;                //!< Variable containing the address of the receiving address, this is needed to be able to supply the overlaying layer with this information in the transmission complete function (instead of returning only DLL_ACK_NOT_RECEIVED (without any node information).
static uint8_t DLL_ucLastAddressWithWhichWeWereAddressed; //!< Variable used (mainly by dbuspresentation) to declare in response last address, with which we were addressed, as sender address

static uint32_t DLL_ulTimeStamp; //!< Variable used to save the time0 (this is set every time a byte has been received) before testing whether an interbyte time-out has occured (this is done continuously). The acknowledgement time-out is smaller than the idle time recognition - this is the same as the interbyte time-out, and can be tested by using the same variable, as there is no interbyte time-out surveillance after all bytes have been transmitted/received.
static uint16_t DLL_uiCollisionRandomValue; //!< DLL_uiCollisionRandomValue; value used for calculating bus-idle before start of retransmission -different value for each node (communication partner) - value dependent on how many collisions are perceived.
static uint16_t DLL_uiUserCode;  //!< DLL_uiUserCode is any code/number/index, which the overlaying layer associates with this specific message (returned by the message completion function).
static uint32_t DLL_ulCollisionWaitingTime; //!< A temporary placeholder for the pseudorandom value, which has to be used, once called.

static bool DLL_bSilent = false; //!< Indicates, whether dbus is in Silent Mode

#define DLL_RANDOM_X   0x1798U    //!< Random value x.

/**
  Initialisation of the module.

\return 
 \b type: bool\n
 \b range \li 0 = not initialised
          \li all other values = initialised.
*/
static bool DLL_bInit(void)
{
   DLL_vInitDllConfig();
   DLL_uiCollisionRandomValue = DLL_RANDOM_X + (uint16_t)DLL_ConfigurationItem.ucNodeAddress;
   /* Uart initialisation */
   DBM_UART_vInit(DLL_ConfigurationItem.ucConfigIndex);
   DBM_UART_vEnable();
   DLL_vSetupSavedBaudRate();
   DBM_UART_vEnableRxEvent();
   DBM_UART_vDisableTxEvent();
   return true;
}

/**
 Calculates the bit timings for the requested baud rate, if DBUS2_UPDATE is activated

 \param baud Baud Rate in 100 bits per second
 \return
 \b type: void
*/
void DLL_vSetupTimings(uint16_t baud)
{
#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
    uint16_t new_bit_time = DBPL_uGetUsBitTimeForBaudRate(baud);
    if(new_bit_time > 0U)
    {
        DLL_uiBitTime = new_bit_time;
        DLL_uiIdleStateDetectionTime = (uint16_t)((uint16_t)DLL_IDLE_STATE_DETECTION_TIME * DLL_uiBitTime);

        if(DLL_uiBitTime < DLL_BIT_TIME_CAPPED)
        {
            DLL_uiAckGenerationTimeout = (uint16_t)((uint16_t)((uint16_t)DLL_ACK_GENERATION_TIMEOUT * (uint16_t)DLL_BIT_TIME_CAPPED) - (uint16_t)((uint16_t)DLL_BIT_TIME_CAPPED>>1U));
            DLL_uiInterbyteTimeoutRx = (uint16_t)((uint16_t)((uint16_t)DLL_INTERBYTE_TIME_OUT * (uint16_t)DLL_BIT_TIME_CAPPED) + (uint16_t)((uint16_t)DLL_BIT_TIME_CAPPED>>1U));
        }
        else
        {
            DLL_uiAckGenerationTimeout = (uint16_t)(((uint16_t)DLL_ACK_GENERATION_TIMEOUT * DLL_uiBitTime) - (DLL_uiBitTime>>1U));
            DLL_uiInterbyteTimeoutRx = (uint16_t)((uint16_t)((uint16_t)DLL_INTERBYTE_TIME_OUT * DLL_uiBitTime) + (DLL_uiBitTime>>1U));
        }
    }
#else
    (void)baud;
    (void)DLL_uiIdleStateDetectionTime;
    (void)DLL_uiAckGenerationTimeout;
#endif
}


/**
 Calculates a waiting time before initiation of a new transmission is allowed,
 including a random value, so that each node waits a different time.

\return The random waiting time\n
 \b type: uint16_t\n
 \b range: 0x0000-0xFFFF
*/
uint32_t DLL_uiCalcCollisionWaitingTime(void)
{
   #define DLL_RANDOM_Y   0x2B1BU   //!< Random value y.
   #define DLL_RANDOM_Z   0x3039U   //!< Random value z.

   #define DLL_COLL_FACT_0  ((uint8_t)0x07U)     //!< Mask after 0 succeeding collisions
   #define DLL_COLL_FACT_1  ((uint8_t)0x0FU)     //!< Mask after 1 succeeding collision
   #define DLL_COLL_FACT_2  ((uint8_t)0x1FU)     //!< Mask after 2 succeeding collisions
   #define DLL_COLL_FACT_3  ((uint8_t)0x3FU)     //!< Mask after 3 succeeding collisions
   #define DLL_COLL_FACT_4_MORE  ((uint8_t)0x7FU)     //!< Mask after 4 and more succeeding collisions

   enum
   {
       DLL_SC_0,
       DLL_SC_1,
       DLL_SC_2,
       DLL_SC_3
   };
   uint8_t ucCollisionFactor;
   DLL_uiCollisionRandomValue = (DLL_uiCollisionRandomValue ^ DLL_RANDOM_Y) + DLL_RANDOM_Z;
   ucCollisionFactor = (uint8_t)(DLL_uiCollisionRandomValue >> BYTE_SIZE);
   switch (DLL_ucSucceedingCollisions)
   {
      case DLL_SC_0:
      {
         ucCollisionFactor &= DLL_COLL_FACT_0;
         break;
      }
      case DLL_SC_1:
      {
         ucCollisionFactor &= DLL_COLL_FACT_1;
         break;
      }
      case DLL_SC_2:
      {
         ucCollisionFactor &= DLL_COLL_FACT_2;
         break;
      }
      case DLL_SC_3:
      {
         ucCollisionFactor &= DLL_COLL_FACT_3;
         break;
      }
      default:
      {
         ucCollisionFactor &= DLL_COLL_FACT_4_MORE;
         break;
      }
   }
   return (uint32_t)(((uint32_t)ucCollisionFactor * (uint32_t)DLL_uiBitTime) +
           (((DBPL_bDoesAppSpecificRepetitionTimeoutApply(DLL_uiUserCode) == true) && (BAL_bIsSendingRetry() == true)) ? DLL_ConfigurationItem.ulWaitingTimeOffsetUs : 0U));
   /*Above: If repeating message, after unsuccessful attempt(s), add additional time specified by user to waiting.*/
}


static uint16_t DLL_uiCrc;       //!< Calculated CRC value
/**
 Initialisation of the crc calculation (resets the present value of the crc). The initialisation
 must be called before starting a new crc calculation, hence every time a new message is received
 (start of the message reception).


\return 
 \b type: void
*/
static void DLL_vCrcInit(void)
{
   DLL_uiCrc = DLL_CRC_INIT;
}
/**
 FAST_CODE_EFFICIENT_XMODEM_CRC
 This function calculates (continously) a new crc based on the influence by the present byte.
 The calculation is based on the xmodem crc with the polynom 0x1021
 (please refer to the literature for a discussion of the meaning of this polynomial...).

\param Byte: The present byte of the stream of data covered by the crc calculation.

\return 
 \b type: void
*/
static void DLL_vAddByteToCrcCalculation(uint8_t ucByte)
{
   /* Values specific for the CRC algorithm (XMODEM) */
   #define DLL_CRC_3  3
   #define DLL_CRC_5  5

   uint8_t ucLocrc; //Low CRC
   uint8_t ucHicrc; //High CRC
   ucHicrc = (uint8_t) DLL_uiCrc;
   ucLocrc = (uint8_t) (DLL_uiCrc >> BYTE_SIZE) ^ucByte;
   ucLocrc ^= (uint8_t)(ucLocrc >> NIBBLE_SIZE);
   ucHicrc ^= (uint8_t)(ucLocrc << NIBBLE_SIZE);
   ucHicrc ^= (uint8_t)(ucLocrc >> DLL_CRC_3);
   ucLocrc ^= (uint8_t)(ucLocrc << DLL_CRC_5);
   DLL_uiCrc = (((uint16_t) ucHicrc) << BYTE_SIZE) | ucLocrc;
   return;
}


/**

 \return The calculated crc\n
 \b type: uint16_t
*/
static uint16_t DLL_uiGetCalculatedCrc(void)
{
   return DLL_uiCrc;
}

#ifdef LSW_DLL_MONITOR_DISTRIBUTION_OF_COLLISIONS
#define DLL_LENGTH_OF_COLLISION_DISTRIBUTION_ARRAY      5 //!< Length in 4 byte (uint32_t)
static uint32_t DLL_ulMonitorDistributionOfCollisions[DLL_LENGTH_OF_COLLISION_DISTRIBUTION_ARRAY];  //!< Helpful for analysing the distribution of collisions in the system.
#endif
/**
 Returns to idle state after aborting a transmission or reception. The idle state timer is reloaded.
 This function should only be called from the task handler and the interrupt handler,
 as the task state is manipulated.

\return 
 \b type: void
*/
static void DLL_vEnterIdleState(void)
{
   DLL_ulTimeStamp = DBM_ulGetCircleMicroSeconds();
   DLL_ulCollisionWaitingTime = DLL_uiCalcCollisionWaitingTime();
#ifdef LSW_DLL_MONITOR_DISTRIBUTION_OF_COLLISIONS
   if (DLL_ucSucceedingCollisions < DLL_LENGTH_OF_COLLISION_DISTRIBUTION_ARRAY-1)
   {
      DLL_ulMonitorDistributionOfCollisions[DLL_ucSucceedingCollisions]++;
   }
   else
   {
      DLL_ulMonitorDistributionOfCollisions[DLL_LENGTH_OF_COLLISION_DISTRIBUTION_ARRAY-1]++;
   }
#endif
   DLL_ucTaskState = (uint8_t)DLL_IDLE_STATE;
   DLL_ucStatus = 0U;       //Reset Status, so that a new received message is not getting status from previous transmissions/receptions.
}

/**
Increments the collision counters: DLL_ucSucceedingCollisions, DLL_ulAccumulatedCollisions and DLL_ucMaxSucceedingCollisions.

\return 
 \b type: void
*/
static void DLL_vAddCollision(void)
{
   if (DLL_ucSucceedingCollisions < DLL_MAX_SUCCEEDING_COLLISIONS)
   {
      DLL_ucSucceedingCollisions++;
#ifdef LSW_DLL_INCLUDE_STATISTICS
      DLL_ulAccumulatedCollisions++;
      if (DLL_ucSucceedingCollisions > DLL_ucMaxSucceedingCollisions)
      {
         DLL_ucMaxSucceedingCollisions = DLL_ucSucceedingCollisions;
      }
#endif
   }
}

/**
Function is called from function DLL_bIsBusReadyForTransmission, so that the idle state can be recognised before the data link layer task handler was able to observe this (i.e. if idle state occured after last call of the data link layer task handler).

\return 
 \b type: bool\n
 \b range \li 0 = false
          \li all other values = true
*/
static bool DLL_bIsBusIdle(void)
{
   if ((DLL_ucStatus & DLL_STATUS_READY_TO_INITIATE_TRANSMISSION_BIT) == 0u)
   {
      if ((DLL_ucTaskState < (uint8_t)DLL_MESSAGE_TO_SEND)) //NB! This assumes that all receive states are lower valued, if the data link layer has been transmitting a message, it is important to let the task handler the possibility to call the message confirmation function.
      {
         if ((uint32_t)(DBM_ulGetCircleMicroSeconds() - DLL_ulTimeStamp) > (uint32_t)(DLL_ulCollisionWaitingTime + DLL_uiIdleStateDetectionTime))
         {
            //[MISRA 2004 Rule 14.7]: This function contains more than one return path. However, for such a compact function this is not likely to used falsely.
            return true;
            //It is important that this branch is not reachable after DLL_STATUS_READY_TO_INITIATE_TRANSMISSION_BIT has been set by the task handler, as this branch may return wrong value after ~1hour (32 bit overflow in DBM_ulGetCircleMicroSeconds()).
         }
      }
      else
      {
         //It is important that idle is not recognised before the DLL-task handler recognises this - when waiting for acknowledgement. Otherwise the MessageComplete-function will not be called, and the repetition (done by the bus application layer task handler, due to missing acknowledgement) of the message due to missing acknowledgement may follow arbitrarily many times.
      }
      return false;
   }
   else
   {
      return true;
   }
}

/*
DLL_vHandleRxEvent(void); This function, which handles the receive interrupts, is called from HAL (hardware abstraction layer) each time a receive interrupt is released. The function defined in HAL will be replaced by this one.
*/
void DLL_vHandleRxEvent(void)
{
   static uint8_t DLL_ucRxMessageToWrite = 0U; //!< Offset index where to save next message in the input ring buffer.
   static uint8_t DLL_ucRxCharToWrite = 0U; //!< Offset index where to save the present character, before it is certain that this message will be kept (e.g. CRC must be checked at the end of the transmission of this message).
   static uint16_t DLL_uiReceivedCrc;  //!< CRC value received from the bus.

   uint32_t uiPreviousTimestamp; //!<Copy of previous timestamp
   uint8_t ucReceivedCharacter; //Used for keeping the character received by the UART
   uint8_t ucDataLength; //Used for an intermediate calculation in DLL_MESSAGE_TO_RECEIVE_GET_RECIPIENT
   uint16_t uiRegisteredMsgId; //Find out, whether it is Power Fail message
   uiPreviousTimestamp = DLL_ulTimeStamp;
   DLL_ulTimeStamp = DBM_ulGetCircleMicroSeconds(); //Get current timestamp as soon as possible within Rx-Interrupt handler
   /* Check for erroneous receive interrupt */
   if (DBM_UART_bIsRxError() == false) //Noise errors are ignored
   {
      ucReceivedCharacter = DBM_UART_ucGetRxData();  //Includes reset interrupt flag
      if (DLL_ucTaskState == (uint8_t)DLL_OFFLINE)
      {
         return; //Stay offline, no normal handling.
      }
      if (DLL_ucTaskState != (uint8_t)DLL_IDLE_STATE)
      {
         /*Check for interbyte time-out*/
         if ((uint16_t)(DLL_ulTimeStamp - uiPreviousTimestamp) > DLL_uiInterbyteTimeoutRx)
         {
            //Interbyte timeout:
#ifdef LSW_DLL_INCLUDE_STATISTICS
            DLL_ulInterbyteTimeoutCounter++;
#endif
            if ((DLL_ucStatus & DLL_STATUS_MESSAGE_TRANSMISSION_BIT) != 0u)
            {
               DLL_vMessageTransmitted_UserCallback(DLL_ucReceivingNodeAddress, (uint8_t)(DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress) | DLL_ACK_TRANSMISSION_ABORTED), DLL_uiUserCode);
               DLL_vMessageTransmitted((uint8_t)(DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress) | DLL_ACK_TRANSMISSION_ABORTED), DLL_uiUserCode);
               DLL_vEnterIdleState(); //Received byte is the first of a new reception, due to the perceived time-out.
            }
            else
            {
               if (DLL_ucTaskState != (uint8_t)DLL_SEND_ACK)
               {
                  DLL_vEnterIdleState(); //Received byte is the first of a new reception, due to the perceived time-out.
               }
               else
               {
                  /* Timeout recognised - however, during reception this message should be evaluated as if no timeout occurred when ack was already sent. In case the timeout is falsely perceived, due to a too late time stamp (due to too high interrupt burden), it is important that the the message is accepted if the generated acknowledgement is positive. */
#ifdef LSW_DLL_INCLUDE_STATISTICS
                  DLL_ulOwnAckTimeoutCounter++;
#endif
               }
            }
         }
      }
      switch (DLL_ucTaskState)
      {
         default:
         case (uint8_t) DLL_IDLE_STATE:
         {
            if (ucReceivedCharacter >= DLL_MINIMUM_MESSAGE_LENGTH)
            {
               DLL_tUnionCounter.ucRxDataLen = ucReceivedCharacter + BUS_MESSAGE_OVERHEAD;    //The first byte of the bus identifier contains the data length of the message (not the frame), which is of course not allowed to exceed 253 bytes (253 bytes + BUS_MESSAGE_OVERHEAD = 255)!
               if (DLL_tUnionCounter.ucRxDataLen > DLL_ConfigurationItem.ucInputBufferLength) //Reset DLL_ucStatus:
               {
                  DLL_ucStatus = DLL_STATUS_OVERFLOW_BIT;      //This is the start of a new reception, delete the status from the previous reception/transmission, and mark this as too long for the input buffer.
                  //NB! This flag must be reset, if the message is not addressed to this node!
               }
               else
               {
                  DLL_ucStatus = ((uint8_t)0x00U);      //This is the start of a new reception, delete the status from the previous reception/transmission.
               }
               DLL_vCrcInit(); //reset the crc calculation.
               DLL_vAddByteToCrcCalculation(ucReceivedCharacter); //Update calculated check sum
               DLL_ucTaskState = (uint8_t)DLL_MESSAGE_TO_RECEIVE_GET_RECIPIENT;
            }
            else //D-Bus-3 Frame (first byte is 0x00 or 0x01)
            {
                DLL_ucTaskState = (uint8_t)DLL_MESSAGE_TO_RECEIVE;
                DLL_ucStatus = DLL_STATUS_D_BUS_3_TRANSMISSION_BIT; //Reset DLL_STATUS_OVERFLOW_BIT - and set D-Bus-3 Transmission flag only.
                            
            }
            break;
         }
         case (uint8_t) DLL_MESSAGE_TO_RECEIVE_GET_RECIPIENT:
         {
            if (DLL_GET_COMMUNICATION_PARTNER(ucReceivedCharacter) == DLL_BROADCAST)
            {
               DLL_ucStatus |= DLL_STATUS_BROADCAST_BIT; //Set flag telling that message is broadcasted, no acknowledgement generation from the receiver.
            }
            else
            {
               DLL_ucLastAddressWithWhichWeWereAddressed = ucReceivedCharacter; //Possibility to send response with param "SenderAddress" being the address, with which our app has been addressed last.

               if (DLL_bIsCurrentNode(ucReceivedCharacter)==false)
               {
                    DLL_tUnionCounter.ucRxDataLen++; //Read also Acknowledgement, which will be generated by the addressed node.
                    DLL_ucTaskState = (uint8_t)DLL_MESSAGE_TO_RECEIVE;
                    DLL_ucStatus = 0U; //Reset DLL_STATUS_OVERFLOW_BIT - and as no other flags should be set at this point, all bits may be reset.
                    break;
               }
            }

            DLL_ucStatus |= DLL_STATUS_MESSAGE_FOR_THIS_NODE_BIT; //Set flag telling that message was addressed to this node
            DLL_ucReceivingNodeAddress = ucReceivedCharacter; //Save the used address - this is needed, for the acknowledgement generation if an alias address is used.

            if ((DLL_ucStatus & DLL_STATUS_OVERFLOW_BIT) == 0U)
            {
               //Save the two bytes already received:
               if (DLL_tUnionCounter.ucRxDataLen > (DLL_ConfigurationItem.ucInputBufferLength - DLL_ucRxMessageToWrite))
               {
                  /*
                      Wrap needed in the ring buffer:
                      If any characters are left before end of the buffer: Fill the buffer with a dummy message (marked with ucMessageLength of 0xFF)
                  */
                  if (((int16)DLL_ConfigurationItem.ucInputBufferLength - (int16)DLL_ucRxMessageToWrite) > (int16)0)
                  {
                     DLL_aucMessageInputBuffer[DLL_ucRxMessageToWrite] = (uint8_t) DLL_DUMMY_MESSAGE_MESG_LEN;
                     DLL_ucCharactersInRxMessageBuffer += (uint8_t)(DLL_ConfigurationItem.ucInputBufferLength - DLL_ucRxMessageToWrite);
                  }
                  DLL_ucRxMessageToWrite = 0U;
               }
               else
               {
                  //No wrap needed in the ring buffer
               }
               DLL_ucRxCharToWrite = 0U;
               if (DLL_ucCharactersInRxMessageBuffer > (DLL_ConfigurationItem.ucInputBufferLength - BUS_MESSAGE_OVERHEAD) )
               {
                  //Overflow, no space for the two received bytes in the RxBuffer
                  DLL_ucStatus |= DLL_STATUS_OVERFLOW_BIT;
               }
               else
               {
                  //OK (no overflow)
                  ucDataLength = (uint8_t)(DLL_tUnionCounter.ucRxDataLen - BUS_MESSAGE_OVERHEAD);
                  if (ucDataLength <= (DLL_ConfigurationItem.ucInputBufferLength - DLL_ucRxMessageToWrite)) //Check whether the data length is out of range
                  {
                     DLL_ucRxCharToWrite = 0U;
                     //lint -save -e9016  advisory  "Pointer arithmetic not allowed MisraC2012 18.4 advisory" tested to work correctly
                     *(&DLL_aucMessageInputBuffer[0] + DLL_ucRxMessageToWrite + DLL_ucRxCharToWrite) = (uint8_t)(DLL_tUnionCounter.ucRxDataLen - BUS_MESSAGE_OVERHEAD);
                     DLL_ucRxCharToWrite++;
                     *(&DLL_aucMessageInputBuffer[0] + DLL_ucRxMessageToWrite + DLL_ucRxCharToWrite) = ucReceivedCharacter;
                     DLL_ucRxCharToWrite++;
                     //lint -restore -e9016
                  }
                  else
                  {
                     //Data out of range (too long message)
                     DLL_ucStatus |= DLL_STATUS_OVERFLOW_BIT;
                  }
               }
            }
            DLL_ucTaskState = (uint8_t)DLL_MESSAGE_TO_RECEIVE;
            DLL_vAddByteToCrcCalculation(ucReceivedCharacter); //Update calculated check sum
            break;
         }
         case (uint8_t) DLL_MESSAGE_TO_RECEIVE:
         {
            if (((DLL_ucStatus & DLL_STATUS_MESSAGE_FOR_THIS_NODE_BIT) != 0U) &&
                ((DLL_ucStatus & DLL_STATUS_OVERFLOW_BIT) == 0U))
            {
               //Message for this node
               DLL_tUnionCounter.ucRxDataLen--;
               if (DLL_tUnionCounter.ucRxDataLen > 1U)
               {
                  //Save data part of the message
                  if ((DLL_ucCharactersInRxMessageBuffer + DLL_ucRxCharToWrite) >= DLL_ConfigurationItem.ucInputBufferLength)
                  {
                     //Overflow, no space for the bus identifier in the RxBuffer
                     DLL_ucStatus |= DLL_STATUS_OVERFLOW_BIT;
                     //message discarded, due to overflow
                  }
                  else
                  {
                     //lint -e{9016}  advisory  "Pointer arithmetic not allowed MisraC2012 18.4 advisory" tested to work correctly
                     *(&DLL_aucMessageInputBuffer[0]+DLL_ucRxMessageToWrite + DLL_ucRxCharToWrite) = ucReceivedCharacter;
                     DLL_ucRxCharToWrite++;
                     DLL_vAddByteToCrcCalculation(ucReceivedCharacter); //Update calculated check sum
                  }
               }
               else
               {
                  //End of data part of message reached: Received bytes = expected bytes
                  //Receive Check sum
                  if (DLL_tUnionCounter.ucRxDataLen == 0U) //Possible cases: DLL_tUnionCounter.ucRxDataLen = 1: CRC-high or DLL_tUnionCounter.ucRxDataLen = 0: crc-low
                  {
                     HSUP_LOW_BYTE(DLL_uiReceivedCrc) = ucReceivedCharacter;
                     if (DLL_uiGetCalculatedCrc() == DLL_uiReceivedCrc) //Compare received check sum with calculated check sum.
                     {
                        //CRC ok
                        if ((DLL_ucStatus & DLL_STATUS_BROADCAST_BIT) == 0U)
                        {
                           DLL_ucSucceedingCollisions = 0U;  //Successfully received a message, reset the collision counter
                        }
                     }
                     else
                     {
                        //CRC error
                        DLL_ucStatus |= DLL_STATUS_WRONG_CRC_BIT;
#ifdef LSW_DLL_INCLUDE_STATISTICS
                        DLL_ulCrcErrorCounter++;
#endif
                     }
                     if ((DLL_ucStatus & DLL_STATUS_BROADCAST_BIT) != 0U)
                     {
                        //Broadcast, acknowledgement generated from sender.
                        DLL_ucTaskState = (uint8_t) DLL_WAIT_FOR_ACK;
                     }
                     else
                     {
                        DLL_ucTaskState = (uint8_t) DLL_SEND_ACK;
                        DBM_UART_vEnableTxEvent();
                     }
                  }
                  else
                  {
                     HSUP_HIGH_BYTE(DLL_uiReceivedCrc) = ucReceivedCharacter;
                  }
               }
            }
            else
            {
               //Message for someone else, read until end, then return to idle state, or message overflow (handle as if the message was for someone else, until the Acknowledge shall be sent).
               DLL_tUnionCounter.ucRxDataLen--;
               if (DLL_tUnionCounter.ucRxDataLen == 0U)
               {
                  if ((DLL_ucStatus & DLL_STATUS_OVERFLOW_BIT) != 0U)
                  {
                     if ((DLL_ucStatus & DLL_STATUS_BROADCAST_BIT) != 0U)
                     {
                        //Broadcast, acknowledgement generated from sender.
                        DLL_ucTaskState = (uint8_t) DLL_WAIT_FOR_ACK;
                     }
                     else
                     {
                        if ((DLL_ucStatus & DLL_STATUS_MESSAGE_FOR_THIS_NODE_BIT) != 0U)
                        {
                           DLL_ucTaskState = (uint8_t) DLL_SEND_ACK;
                           DBM_UART_vEnableTxEvent();
                        }
                     }
                  }
                  else
                  {
                     if ((DLL_ucStatus & DLL_STATUS_D_BUS_3_TRANSMISSION_BIT) != 0U)
                     {
                         //Byte will not be saved, as the D-Bus-3 message is being ignored. This state is left via timeout.
                     }
                     else
                     {
                         DLL_vEnterIdleState();
                     }
                  }
               }
               else
               {
                  //Byte will not be saved, as the message is not for this node.
               }
            }
            break;
         }
         case (uint8_t) DLL_MESSAGE_TO_SEND:
         {
            //Check if received Data is equal to sent data.
            if (DLL_ucCharactersToCheck == 1U)
            {
               DLL_pucNextCharToCheck = &HSUP_HIGH_BYTE(DLL_uiCrc); //Point to high byte of the calculated CRC.
            }
            else
            {
#ifdef __LITTLE_ENDIAN_ADAPTATION
               if (DLL_ucCharactersToCheck==0u)
               {
                  DLL_pucNextCharToCheck = &HSUP_LOW_BYTE(DLL_uiCrc); //Point to low byte of the calculated CRC.
               }
#endif
            }
            if (ucReceivedCharacter == *DLL_pucNextCharToCheck)
            {
               if (DLL_ucCharactersToCheck==0u)
               {
                  if ((DLL_ucStatus & DLL_STATUS_BROADCAST_BIT) != 0U)
                  {
                     DLL_ucTaskState = (uint8_t) DLL_SEND_ACK;
                     DLL_ucReceivingNodeAddress = (uint8_t)(DLL_ConfigurationItem.ucNodeAddress<<NIBBLE_SIZE); //Needed for acknowledgement generation.
                     DBM_UART_vEnableTxEvent();
                  }
                  else
                  {
                     DLL_ucTaskState = (uint8_t) DLL_WAIT_FOR_ACK;
                  }
               }
               else
               {
                  DLL_ucCharactersToCheck--;
               }
            }
            else
            {
               DBM_UART_vDisableTxEvent();
               DLL_vAddCollision();
               DLL_vMessageTransmitted_UserCallback(DLL_ucReceivingNodeAddress, (uint8_t)(DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress) | DLL_ACK_TRANSMISSION_ABORTED), DLL_uiUserCode); //The character read back from the bus does not correspond to the sent character, notify overlying layer that the transmission will be aborted.
               DLL_vMessageTransmitted((uint8_t)(DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress) | DLL_ACK_TRANSMISSION_ABORTED), DLL_uiUserCode); //The character read back from the bus does not correspond to the sent character, notify overlying layer that the transmission will be aborted.
               DLL_vEnterIdleState();
            }
            DLL_pucNextCharToCheck++;
            break;
         }
         case (uint8_t) DLL_SEND_ACK:
         {
            if ((ucReceivedCharacter == DLL_tUnionCounter.ucAcknowledge) || (DLL_tUnionCounter.ucAcknowledge == (uint8_t) DLL_ACK_OK))
            {
               DLL_ucSucceedingCollisions = 0U;  //Successfully transmitted(broadcast)/received a message reset the collision counter
               if ((DLL_ucStatus & DLL_STATUS_BROADCAST_BIT) != 0U)
               {
                  //Broadcast, i.e. message transmission
                  DLL_vMessageTransmitted_UserCallback(DLL_ucReceivingNodeAddress, DLL_tUnionCounter.ucAcknowledge, DLL_uiUserCode); //Msg completion function called with parameter: Acknowledge
                  DLL_vMessageTransmitted(DLL_tUnionCounter.ucAcknowledge, DLL_uiUserCode); //Msg completion function called with parameter: Acknowledge
               }
               else
               {
                  if ((DLL_ucStatus & DLL_STATUS_WRONG_CRC_BIT) == 0U)
                  {
                     if ((DLL_ucStatus & DLL_STATUS_OVERFLOW_BIT) == 0U)
                     {

                        if ((DLL_ucRxCharToWrite % DLL_ALIGNMENT_BASE) != 0U)
                        {
                           DLL_ucRxCharToWrite++; //align 16 bit access to even address offset.
                        }

                        DLL_ucCharactersInRxMessageBuffer += DLL_ucRxCharToWrite;
                        DLL_ucRxMessageToWrite += DLL_ucRxCharToWrite;
                     }
                  }
                  else
                  {
                     //Wrong CRC, this message must be ignored, i.e. not forwarded to the overlaying layer.
                  }
               }
            }
            else
            {
               DLL_vAddCollision();
               if ((DLL_ucStatus & DLL_STATUS_BROADCAST_BIT) != 0U)
               {
                  //Broadcast, i.e. message transmission
                  DLL_vMessageTransmitted_UserCallback(DLL_ucReceivingNodeAddress, (uint8_t)(DLL_ConfigurationItem.ucNodeAddress << NIBBLE_SIZE), DLL_uiUserCode); //The character read back from the bus does not correspond to the sent acknowledgement.
                  DLL_vMessageTransmitted((uint8_t)(DLL_ConfigurationItem.ucNodeAddress << NIBBLE_SIZE), DLL_uiUserCode); //The character read back from the bus does not correspond to the sent acknowledgement.
               }
            }
            DLL_vEnterIdleState();
            break;
         }
         case (uint8_t) DLL_WAIT_FOR_ACK:
         {
            DLL_tUnionCounter.ucAcknowledge = ucReceivedCharacter;
            if (DLL_GET_ACKNOWLEDGE(ucReceivedCharacter) == (uint8_t) DLL_ACK_OK)
            {
               //Ack OK
               if ((DLL_ucStatus & DLL_STATUS_MESSAGE_TRANSMISSION_BIT) == 0U)
               {
                  //Message reception
                  if ((DLL_ucStatus & DLL_STATUS_WRONG_CRC_BIT) == 0U)
                  {
                     if (((DLL_ucStatus & DLL_STATUS_MESSAGE_FOR_THIS_NODE_BIT) != 0U) &&
                         ((DLL_ucStatus & DLL_STATUS_OVERFLOW_BIT) == 0U))
                     {
                        /*
                        Broadcast Message Completed - update DLL_ucRxNextCharacterToWrite
                        */
                        if ((DLL_ucRxCharToWrite % DLL_ALIGNMENT_BASE) != 0U)
                        {
                           DLL_ucRxCharToWrite++; //align 16 bit access to even address offset.
                        }

                        uiRegisteredMsgId = ((((uint16_t)DLL_aucMessageInputBuffer[DLL_ucRxMessageToWrite + ((uint8_t)DLL_MSG_BYTE_ID_HI)] << 8)) | ((uint16_t)DLL_aucMessageInputBuffer[DLL_ucRxMessageToWrite + ((uint8_t)DLL_MSG_BYTE_ID_LO)]));

                        if(DBPL_bIsReceivedMessageIdPowerFail(uiRegisteredMsgId) == true)
                        {
                            DLL_ucRxCharToWrite = 0U;
                            DBPL_vPowerFailNotificationHasBeenReceived();
                        }
                        else
                        {
                            DLL_ucCharactersInRxMessageBuffer += DLL_ucRxCharToWrite;
                            DLL_ucRxMessageToWrite += DLL_ucRxCharToWrite;
                        }
                     }
                     else
                     {
                        //Message for other node or overflow.
                     }
                  }
                  else
                  {
                     //Wrong CRC, this message must be ignored, i.e. not forwarded to the overlaying layer.
                  }
               }
               else
               {
                  //Message transmission
                  uint8_t nodeInReceivedByte = DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(ucReceivedCharacter);
                  uint8_t correctNodeAddress = DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress);
                  if (nodeInReceivedByte != correctNodeAddress)
                  {
                     DLL_tUnionCounter.ucAcknowledge = DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress); //corresponds to: DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress) | DLL_ACK_NOT_RECEIVED;
                  }
                  else
                  {
                     //OK - Message was received.
                  }
               }
               DLL_ucSucceedingCollisions = 0U;  //Successfully transmitted/received a message, reset the collision counter
            }
            else
            {
               //Ack error
#ifdef LSW_DLL_INCLUDE_STATISTICS
               DLL_ulNackCounter++;
#endif
            }
            if ((DLL_ucStatus & DLL_STATUS_MESSAGE_TRANSMISSION_BIT) != 0U)
            {
               DLL_vMessageTransmitted_UserCallback(DLL_ucReceivingNodeAddress, DLL_tUnionCounter.ucAcknowledge, DLL_uiUserCode); //Msg completion function called with parameter: Acknowledge
               DLL_vMessageTransmitted(DLL_tUnionCounter.ucAcknowledge, DLL_uiUserCode); //Msg completion function called with parameter: Acknowledge
            }
            DLL_vEnterIdleState();
            break;
         }
      }
   }
   else
   {
      /* An error has occured within the last character. This character will be discarded. */
#ifndef __HC08__
      //Compiler error in Metrowerks Hicross 6 (version 5.0.15)
      (void)
#endif
      DBM_UART_ucGetRxData(); //Reset interrupt flags
   }
   return;
}
/*
DLL_vHandleTxEvent(void); This function, which handles the transmit interrupts, is called from HAL (hardware abstraction layer) each time a transmit interrupt is released. The function defined in HAL will be replaced by this one.
*/
void DLL_vHandleTxEvent(void)
{
   if (DLL_ucTaskState == (uint8_t) DLL_MESSAGE_TO_SEND)
   {
      if (DLL_tUnionCounter.ucCharactersToWrite > 1U)
      {
         DBM_UART_vSetTxData(*DLL_pucNextCharToWrite);
         DLL_vAddByteToCrcCalculation(*DLL_pucNextCharToWrite);
         //Update calculated check sum
         //Send next character
         DLL_pucNextCharToWrite++;
         DLL_tUnionCounter.ucCharactersToWrite--;
         return;
      }
      else
      {
         /* Send CRC */
         if (DLL_tUnionCounter.ucCharactersToWrite == 1U)
         {
            DLL_pucNextCharToWrite = &HSUP_HIGH_BYTE(DLL_uiCrc); //Point to high byte of the calculated CRC.
            DBM_UART_vSetTxData(*DLL_pucNextCharToWrite); //Send calculated CRC - high byte
            DLL_tUnionCounter.ucCharactersToWrite--;
            return;
         }
         else
         {
            /*Point to low byte of the calculated CRC. - For LittleEndian systems this must be done, as the low byte does not follow the high byte (it's the opposite).*/
            DLL_pucNextCharToWrite = &HSUP_LOW_BYTE(DLL_uiCrc);
            DBM_UART_vSetTxData(*DLL_pucNextCharToWrite); //Send calculated CRC - low byte
         }
      }
   }
   else
   {
      if (DLL_ucTaskState == (uint8_t) DLL_SEND_ACK)
      {
         while ((uint32_t)(DBM_ulGetCircleMicroSeconds() - DLL_ulTimeStamp) < (uint32_t) ((uint32_t)DLL_uiBitTime>>1) )
         {
            /*Wait until at least 1/2 T_Bit has passed since RxInterrupt was left, as the RxInterrupt typically is generated after the last sample of the Stop bit has been performed (by single sampling this is around the middle of the bit). Runtime of RxInterupt is not considered
            */
            HSUP_vNop();
         }
         if ((DLL_ucStatus & DLL_STATUS_OVERFLOW_BIT) != 0U)
         {
            DLL_tUnionCounter.ucAcknowledge = DLL_ACK_BUSY;
         }
         else
         {
            if ((DLL_ucStatus & DLL_STATUS_WRONG_CRC_BIT) != 0U)
            {
               DLL_tUnionCounter.ucAcknowledge = DLL_ACK_WRONG;
            }
            else
            {
               DLL_tUnionCounter.ucAcknowledge = (uint8_t) DLL_ACK_OK;
            }
         }
         DLL_tUnionCounter.ucAcknowledge |= DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress); //return addressed address.
         /*Please note, that when alias address has been used this is not the own address (DLL_ConfigurationItem.ucNodeAddress << 4)... */
         if ((uint32_t)(DBM_ulGetCircleMicroSeconds() - DLL_ulTimeStamp) < DLL_uiAckGenerationTimeout)
         {
            if((DLL_isSilentMode() == false) || ((DLL_tUnionCounter.ucAcknowledge & 0x0Fu) == (uint8_t) DLL_ACK_OK))
            {
                DBM_UART_vSetTxData(DLL_tUnionCounter.ucAcknowledge);
            }
         }
         else
         {
            /*
            Timeout occurred since end of last CRC Byte. Don't send Ack anymore.
            
            This case will typically not be reached (only by problems of large interrupt latencys, where the timestamp is not correctly set). Should this branch, however, be reached, the task handler will recognise a timeout after some time, due to the missing ACK, and then reset the state machine (rather than adding state machine logic into this branch).
            */
#ifdef LSW_DLL_INCLUDE_STATISTICS
            DLL_ulOwnAckTimeoutCounter++;
#endif
         }
      }
   }
   DBM_UART_vDisableTxEvent();   //This must be enabled again outside of the transmit interrupt (typically in the receive interrupt)
   return;
}

uint8_t DLL_HandleTask(void)
{
   if (DLL_ucTaskState != (uint8_t) DLL_INIT_STATE)
   {  // do not access UART registers before its initialization
      DBM_UART_vDisableRxEvent();
   }
   switch (DLL_ucTaskState)
   {
      default:
      case (uint8_t) DLL_IDLE_STATE:
      {
         if ((DLL_bIsBusIdle()) != false)
         {
            DLL_ucStatus |= DLL_STATUS_READY_TO_INITIATE_TRANSMISSION_BIT;
         }
         else
         {
            //Wait corresponding to DLL_uiCollisionRandomValue
         }
         break;
      }
      case (uint8_t) DLL_INIT_STATE:
      {
         if ((DLL_bInit()) != false)
         {
            DLL_vEnterIdleState();
         }
         else
         {
            return TASK_NOT_INITIALISED;
         }
         break;
      }
      case (uint8_t) DLL_MESSAGE_TO_RECEIVE:
      case (uint8_t) DLL_MESSAGE_TO_RECEIVE_GET_RECIPIENT:
      {
         if ((uint32_t)(DBM_ulGetCircleMicroSeconds() - DLL_ulTimeStamp) > DLL_uiInterbyteTimeoutRx)
         {
            //Time-out, return to idle state.
            DLL_vEnterIdleState();
#ifdef LSW_DLL_INCLUDE_STATISTICS
            DLL_ulInterbyteTimeoutCounter++;
#endif
         }
         else
         {
            //Wait till character has been read back
         }
         break;
      }
      case (uint8_t) DLL_MESSAGE_TO_SEND:
      {
         if ((uint32_t)(DBM_ulGetCircleMicroSeconds() - DLL_ulTimeStamp) > DLL_uiInterbyteTimeoutRx)
         {
            //Time-out, return to idle state, via collision.
            DBM_UART_vDisableTxEvent();
            DLL_vMessageTransmitted_UserCallback(DLL_ucReceivingNodeAddress, DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress), DLL_uiUserCode);
            DLL_vMessageTransmitted(DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress), DLL_uiUserCode);
            DLL_vEnterIdleState();
#ifdef LSW_DLL_INCLUDE_STATISTICS
            DLL_ulInterbyteTimeoutCounter++;
#endif
         }
         else
         {
            //Wait till character has been read back
         }
         break;
      }
      case (uint8_t) DLL_SEND_ACK:
      case (uint8_t) DLL_WAIT_FOR_ACK:
      {
         if ((uint32_t)(DBM_ulGetCircleMicroSeconds() - DLL_ulTimeStamp) > DLL_uiInterbyteTimeoutRx)
         {
            //AckTimeout
#ifdef LSW_DLL_INCLUDE_STATISTICS
            DLL_ulAckTimeoutCounter++;
#endif
            if ((DLL_ucStatus & DLL_STATUS_MESSAGE_TRANSMISSION_BIT) != 0U)
            {
               DLL_vMessageTransmitted_UserCallback(DLL_ucReceivingNodeAddress, DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress), DLL_uiUserCode);
               DLL_vMessageTransmitted(DLL_IGNORE_SUBSYSTEM_IN_TARGET_ADDRESS(DLL_ucReceivingNodeAddress), DLL_uiUserCode);
               DBM_UART_vDisableTxEvent();
            }
            DLL_vEnterIdleState();
         }
         else
         {
            //No AckTimeout
            //wait for acknowledgement
            break;
         }
         break;
      }
      case (uint8_t) DLL_OFFLINE:
      {
          //enable event again to leave option for other protocols/goOnline to be received
          break;
      }
   }
   DBM_UART_vEnableRxEvent();
   return TASK_INITIALISED; //i.e. Init finished
}

/*
NB! It is important that DLL_bIsBusReadyForTransmission() is NOT called when the receive interrupt is enabled.
*/
bool DLL_bIsBusReadyForTransmission(void)
{
   if (DLL_ucTaskState == (uint8_t) DLL_IDLE_STATE)
   {
      if ((DLL_ucStatus & DLL_STATUS_READY_TO_INITIATE_TRANSMISSION_BIT) != 0U)
      {
         if (DBM_UART_bIsRxOnGoing() == false) //Does HAL report Idle state?
         {
            return true;
         }
         else
         {
            /* Set new t0 for idle detection. */
            DLL_ulTimeStamp = DBM_ulGetCircleMicroSeconds();
            //Reset DLL_STATUS_READY_TO_INITIATE_TRANSMISSION_BIT status flag in case it is set.
            DLL_ucStatus &= ~DLL_STATUS_READY_TO_INITIATE_TRANSMISSION_BIT;
         }
      }
      else
      {
         //Wait for Idle state.
      }
   }
   if ((DLL_bIsBusIdle()) != false)
   {
      return true;
   }
   else
   {
      //Wait corresponding to DLL_uiCollisionRandomValue before allowing for transmission
   }
   return false;
}

bool DLL_bSendMessage(TbusMessage *ptMessageToTransmit, uint16_t uiUserCode)
{
   if ((DLL_bIsBusReadyForTransmission() != false) && (DLL_isSilentMode() == false))
   {
      if (DLL_ucTaskState != (uint8_t) DLL_OFFLINE)
      {
         DLL_uiUserCode = uiUserCode;     //Save this parameter, and return when transmission finished
         DLL_vCrcInit(); //reset the crc calculation.
         DLL_pucNextCharToWrite = (uint8_t *) ptMessageToTransmit;/*lint !e928 cast tested to work an all machines */
         DLL_pucNextCharToCheck = (uint8_t *) ptMessageToTransmit;/*lint !e928 cast tested to work an all machines */
         DLL_tUnionCounter.ucCharactersToWrite = ptMessageToTransmit->tBusIdentifier.ucMessageLength + (uint8_t)(BUS_MESSAGE_OVERHEAD_INCLUDING_IDENTIFIER - 1u); //Add msg.len and TA + CRC to the data
         DLL_ucCharactersToCheck = DLL_tUnionCounter.ucCharactersToWrite;
         //Don't adapt for LittleEndian systems, as the message is already expected to be correctly placed in memory (BigEndian format)         ptMessageToTransmit->tBusIdentifier.tMessageIdentifier = HSUP_uiProcToBigEndian(ptMessageToTransmit->tBusIdentifier.tMessageIdentifier); //Adapt Message ID to BigEndian format if current controller is LittleEndian.
         DLL_ucReceivingNodeAddress = ptMessageToTransmit->tBusIdentifier.ucTargetAddress;
         DLL_ucStatus = DLL_STATUS_MESSAGE_TRANSMISSION_BIT;
         //lint -e{9016}  advisory  "Pointer arithmetic not allowed MisraC2012 18.4 advisory" tested to work correctly
         if (DLL_GET_COMMUNICATION_PARTNER(*(DLL_pucNextCharToWrite+1)) == DLL_BROADCAST) //Starting transmission of a new message, delete the status from previous reception/transmission, but note, whether this is a broadcast
         {
                DLL_ucStatus |= DLL_STATUS_BROADCAST_BIT;
         }
         if ((DBM_UART_bIsRxOnGoing())==false)
         {
                DLL_ulTimeStamp = DBM_ulGetCircleMicroSeconds();
                DLL_ucTaskState = (uint8_t) DLL_MESSAGE_TO_SEND;
                DBM_UART_vEnableTxEvent();
                return true;
                //Message transmission initiated (if there is a need to know when the message has been transmitted, this information is returned via the confirmation function after (in)successfull transmission).
         }
         else
         {
            //Receive ongoing, don't interrupt with a transmission, this must be retried later.
            DLL_vEnterIdleState(); //Not allowed to send, because someone else just started transmitting.
         }
      }
   }
   return false; //Not possible to send at the present stage.
}


bool DLL_bTransmitMessage(TbusIdentifier tMessageID, TbusService tServiceFunc, uint8_t ucDataLength, uint16_t uiUserCode)
{
   static struct TBusMessageXS DLL_tMessageToSend; //!< The buffer used for sending messages.
   const uint8_t DLL_ucSizeOfDataBufferIn_tBusMessage = DLL_TRANSMIT_BUFFER_DATA_LENGTH;// PRQA S 6008

   if (DLL_bIsBusReadyForTransmission() != false)
   {
      if (DLL_ucTaskState != (uint8_t) DLL_OFFLINE)
      {
         if (ucDataLength > DLL_ucSizeOfDataBufferIn_tBusMessage)
         {
            //The data length is longer than the current transmit buffer, reduce to maximum buffer length.
            ucDataLength = DLL_ucSizeOfDataBufferIn_tBusMessage;
         }
         DLL_uiUserCode = uiUserCode;     //Save this parameter, and return when transmission finished
         DLL_vCrcInit(); //reset the crc calculation.
         DLL_tMessageToSend.tBusIdentifier = tMessageID;
         DLL_tMessageToSend.tBusIdentifier.ucMessageLength = ucDataLength + BUS_MESSAGE_OVERHEAD;
         DLL_tMessageToSend.tBusIdentifier.tMessageIdentifier = HSUP_uiProcToBigEndian(DLL_tMessageToSend.tBusIdentifier.tMessageIdentifier); //Adapt Message ID to BigEndian format if current controller is LittleEndian.
         (*tServiceFunc)(ucDataLength, DLL_tMessageToSend.aucData);
         DLL_pucNextCharToWrite = (uint8_t *) &DLL_tMessageToSend;/*lint !e928 cast tested to work an all machines */
         DLL_pucNextCharToCheck = DLL_pucNextCharToWrite;
         DLL_tUnionCounter.ucCharactersToWrite = DLL_tMessageToSend.tBusIdentifier.ucMessageLength + (uint8_t)(BUS_MESSAGE_OVERHEAD_INCLUDING_IDENTIFIER - 1u); //Add msg.len and TA + CRC to the data
         DLL_ucCharactersToCheck = DLL_tUnionCounter.ucCharactersToWrite;
         DLL_ucReceivingNodeAddress = DLL_tMessageToSend.tBusIdentifier.ucTargetAddress;
         if (DBM_UART_bIsRxOnGoing()==false)
         {
            DLL_ucStatus = DLL_STATUS_MESSAGE_TRANSMISSION_BIT;
            //lint -e{9016}  advisory  "Pointer arithmetic not allowed MisraC2012 18.4 advisory" tested to work correctly
            if (DLL_GET_COMMUNICATION_PARTNER(*(DLL_pucNextCharToWrite+1)) == DLL_BROADCAST) //Starting transmission of a new message, delete the status from previous reception/transmission, but note, whether this is a broadcast
            {
               DLL_ucStatus |= DLL_STATUS_BROADCAST_BIT;
            }
            DLL_ulTimeStamp = DBM_ulGetCircleMicroSeconds();
            DLL_ucTaskState = (uint8_t) DLL_MESSAGE_TO_SEND;
            DBM_UART_vEnableTxEvent();
            return true;
            //Message transmission initiated (if there is a need to know when the message has been transmitted, this information is returned via the confirmation function after (in)successfull transmission).
         }
         else
         {
            //Receive ongoing, don't interrupt with a transmission, this must be retried later.
            DLL_vEnterIdleState(); //Not allowed to send, because someone else just started transmitting.
         }
      }
   }
   return false; //Not possible to send at the present stage.
}

TbusMessage * DLL_ptReceiveMessage(void)
{
   TbusMessage *ptMessageToReturn;
   if ((DLL_ucCharactersInRxMessageBuffer) != 0U)
   {  /*Axivion Next Line MisraC2012-18.1 no out of bound access */
      if (DLL_aucMessageInputBuffer[DLL_ucRxNextCharacterToRead] == (uint8_t) DLL_DUMMY_MESSAGE_MESG_LEN)
      {
         //This is the end of the buffer, a wrap is needed (if the buffer was not completely filled up, the last message is only a dummy data length)
         DLL_ucCharactersInRxMessageBuffer -= (DLL_ConfigurationItem.ucInputBufferLength - DLL_ucRxNextCharacterToRead); //Subtract dummy message, if present
         DLL_ucRxNextCharacterToRead = 0U;
         if ((DLL_ucCharactersInRxMessageBuffer) != 0U)
         {
            ptMessageToReturn = (TbusMessage *)(&DLL_aucMessageInputBuffer[DLL_ucRxNextCharacterToRead]);/*lint !e826 !e927 !e2445 cast tested to work an all machines */
            DLL_ucDataLengthOfReceivedMessage = ptMessageToReturn->tBusIdentifier.ucMessageLength;
         }
         else
         {
            ptMessageToReturn = (TbusMessage *) NULL;
         }
      }
      else
      {  /*Axivion Next Line MisraC2012-18.1 no out of bound access */
         ptMessageToReturn = (TbusMessage *)(&DLL_aucMessageInputBuffer[DLL_ucRxNextCharacterToRead]);/*lint !e826 !e927 !e2445 cast tested to work an all machines */
         DLL_ucDataLengthOfReceivedMessage = ptMessageToReturn->tBusIdentifier.ucMessageLength;
      }
#ifdef __LITTLE_ENDIAN_ADAPTATION
      if (DLL_ucCharactersInRxMessageBuffer != 0U)
      {
         if (DLL_bLittleEndianIdAdaptation==false)
         {
            DLL_bLittleEndianIdAdaptation = true; //LittleEndian adaptation only once for each Message ID
            /*lint -e{613} possible null pointer: tested to work correctly */
            ptMessageToReturn->tBusIdentifier.tMessageIdentifier = HSUP_uiProcToBigEndian(ptMessageToReturn->tBusIdentifier.tMessageIdentifier);
         }
         else
         {
            //MessageID was already adapted for this message.
         }
      }
#endif
   }
   else
   {
      ptMessageToReturn = (TbusMessage *) NULL;
   }
   return ptMessageToReturn;
}

void DLL_vReleaseDataFromReceivedMessage(void)
{
   uint8_t ucDatalength;
   ucDatalength = (uint8_t)(DLL_ucDataLengthOfReceivedMessage + BUS_MESSAGE_OVERHEAD);
#ifdef __LITTLE_ENDIAN_ADAPTATION
   DLL_bLittleEndianIdAdaptation = false; //The next message identifier must be adapted.
#endif
   if ((ucDatalength % DLL_ALIGNMENT_BASE) != 0U)
   {
      ucDatalength++; //align 16 bit access to even address offset.
   }
   if (DLL_ucCharactersInRxMessageBuffer  >= ucDatalength)
   {
      DLL_ucCharactersInRxMessageBuffer -= ucDatalength;
      DLL_ucRxNextCharacterToRead += ucDatalength;
      if (DLL_ucRxNextCharacterToRead >= DLL_ConfigurationItem.ucInputBufferLength)
      {
         DLL_ucRxNextCharacterToRead = 0U;
      }
   }
   else
   {
      //DLL_ucCharactersInRxMessageBuffer should under no circumstanses go below 0!
      //Flush input buffer, as this case (branch) should never be reached...
      DLL_ucRxNextCharacterToRead = 0U;
      DLL_ucCharactersInRxMessageBuffer = 0U;
#ifdef __LITTLE_ENDIAN_ADAPTATION
      DLL_bLittleEndianIdAdaptation = false; //The next message identifier must be adapted.
#endif
   }
   return;
}

uint8_t DLL_ucGetMainNodeAddress(void)
{
   return DLL_ConfigurationItem.ucNodeAddress;
}

uint8_t DLL_ucGetReceivingNodeAddress(void)
{
    return DLL_ucLastAddressWithWhichWeWereAddressed;
}

void DLL_vGoOffline(void)
{
   DLL_ucTaskState = (uint8_t) DLL_OFFLINE;
   DBM_UART_vDisableTxEvent();
}

void DLL_vGoOnline(void)
{
   DLL_vEnterIdleState();
   DBM_UART_vEnableTxEvent();
}

#ifdef LSW_DLL_INCLUDE_STATISTICS
uint8_t DLL_ucGetMaxSucceedingCollisions(void)
{
   return DLL_ucMaxSucceedingCollisions;
}
uint32_t DLL_ulGetAccumulatedCollisions(void)
{
   return DLL_ulAccumulatedCollisions;
}
uint32_t DLL_ulGetCrcErrorCounter(void)
{
   return DLL_ulCrcErrorCounter;
}
uint32_t DLL_ulGetAckTimeoutCounter(void)
{
   return DLL_ulAckTimeoutCounter;
}
uint32_t DLL_ulGetOwnAckTimeoutCounter(void)
{
   return DLL_ulOwnAckTimeoutCounter;
}
uint32_t DLL_ulGetNackCounter(void)
{
   return DLL_ulNackCounter;
}
uint32_t DLL_ulGetInterbyteTimeoutCounter(void)
{
   return DLL_ulInterbyteTimeoutCounter;
}
#endif

/**Function to put silent mode active in Dbus2.2*/
void DLL_vSetSilentMode(void)
{
    DLL_bSilent = true;
}

/**Function to put silent mode out of action in Dbus2.2*/
void DLL_vUnsetSilentMode(void)
{
    DLL_bSilent = false;
}

/**Function to see silent mode status in Dbus2.2*/
bool DLL_isSilentMode(void)
{
    return DLL_bSilent;
}

/* Get Information, whether Dbus is either transmitting or receiving data, to know, if action can be started, that would disturb Dbus, such as disabling interrupts.*/
bool DLL_bIsDbusCommunicating(void)
{
    return (((DLL_ucTaskState > (uint8_t)DLL_IDLE_STATE) && (DLL_ucTaskState < (uint8_t)DLL_OFFLINE)) || (DBM_UART_bIsRxOnGoing() == true)) ? true : false;
}
