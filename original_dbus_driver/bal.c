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
*   COMP_ABBREV      BAL
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief Application layer for the D-Bus-2 and CAN Bus
 *     
 *     This layer handles the message distribution of the system messages. Received messages, which belong to
 *     system messages (differentiation according to the identifier) will be distributed to the
 *     subsystem corresponding to either the subsystem addressed (when addressed subsystem differs from 0),
 *     or to the subsystem, which is indirectly addressed via a service function in the corresponding ReceiveObject
 *     table.
 *     \image html bal.gif
 *
 */
 
/*     ___________________________________________                    ______________________________________
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
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "LibDefines.h"
#include "bustypes.h"
#include "bal.h"
#include "dbusdll.h"
#include "utility.h"
#include "dbusmapping.h" //DBM_UART_vDisableRxEvent() + DBM_UART_vEnableRxEvent()
#include "dbuspresentation.h"   //DBPL_bIsOfflineMode()
#include "dbus_lock.h"
#include "dbus_rtos_interface.h"

#ifdef DOXY_DEVELOPERS_DOC
//Doxygen documentation of compiler switches
/** SystemStates
 */#define CCSS
#endif
/* DEFINITIONS */
#define BAL_REC_TAB_END_BIT   MSB_BYTE //!<The most significant bit of the MessageLength of the last entry of the receive table is used for signalising the end of the table.
#define BAL_MAX_SUBSYS_NO     INT_SIZE //!<It is only possible to have 16 subsystems, even if more are possible according to addressing (ref. alias addressing) - please note that it is assumes (ref. LibTypes.h) that an integer is 16 bit. The limitation of 16 subsystems is due to the usage of a 16 bit variable keeping track of which subsystem has scheduled a message for transmission.

/* PROTOTYPES *****************************************************************************************/
static void BAL_vResetTransmissionBit(uint8_t ucSubsystemIndex, uint8_t ucMessageIndex); //Non-replaceable function

/* Local VARIABLES ******************************************************************************************/
static uint8_t   BAL_ucSubsystemIndex; //!< This is a marker of the subsystem of the presently sent message, in case the message transmission must be repeated certain times.
static uint8_t   BAL_ucMessageIndex = 0U; //!< This is a marker of the message index of the presently sent message, in case the message transmission must be repeated certain times.
static uint8_t BAL_ucTransmissionAttempts; //!< A message sent to a faulty or not present node should not be retried indefinitely. Check against BAL_ucMaxTransmissionRetries.
/** Bit x represents subsystem index x. If there are two subsystems (e.g. 0 and 5) then bit0 represents the first subsystem (0) and bit1 represents the second subsystem (5). If this variable is 0, then there are no messages in any subsystem scheduled for transmission. Please note that only 16 subsystems are allowed, even if numerical value of subsystems may be higher than 15 (only possible when alias-addressing is used). */
static uint16_t BAL_uiTransmitFlagSubsystemIndex = 0U; //!< Variable used to keep track of whether minimum one message is scheduled for transmission in a subsystem. Please note that only 16 subsystems may be defined, as this variable is 16 bits long.
static TbusMessageIdentifier BAL_tReceivedMessageId; //!< The currently received message ID is saved in case this is needed in e.g. function BAL_vNotifyError().
static bool BAL_bNonOperatingMode = false; //!< Flag checking, if non operating mode active

/* Global VARIABLES ******************************************************************************************/
const struct BAL_BreakRecipe BAL_WakeupBreakRecipe[] =
{
    {BAL_BREAK_START, 15u},
    {BAL_BREAK_END,   0u}
};

const struct BAL_BreakRecipe BAL_ResetBreakRecipe[] =
{
    {BAL_BREAK_START, 7000u},
    {BAL_BREAK_HIGH,  750u},
    {BAL_BREAK_LOW,   2000u},
    {BAL_BREAK_END,   0u}
};

/**
 Resets the transmission bit, which belongs to the message referred to.

\param SubsystemIndex
The index corresponding to the subsystem in the bus table.\n
\b type       : uint8_t\n
\b range      : 0...(number of elements in bus table - 1)


\param MessageIndex
The message index corresponding to the message in the transmission/receive table\n
\b type       : uint8_t\n
\b range      : 0...(number of elements (=messages) in transmission/receive table - 1)


\return
\b type       : void
*/
static void BAL_vResetTransmissionBit(uint8_t ucSubsystemIndex, uint8_t ucMessageIndex)
{
   UTI_vSetBit(BAL_tBusObject[ucSubsystemIndex].pucMessageToTransmitBits, ucMessageIndex, false); //Remove current message from transmission list.
}

/**
 * \brief See, whether the given Subsystem is one, which can send messages, in non operating mode
 */
static bool BAL_bDoesOperatingModeBlockApply(uint8_t ucSubsystem)
{
    (void)ucSubsystem;
#ifdef CCSS /*If SystemStates is implemented, it has the right to send/receive messages, when application has not.*/
    if(BAL_bIsNonOperatingMode() == true)
    {
        return (ucSubsystem != (uint8_t)CCCMTD_SUBSYS) ? true : false;
    }
#endif
    return BAL_bIsNonOperatingMode();
}

void BAL_vEnterNonOperatingMode(void)
{
    BAL_bNonOperatingMode = true;
}

void BAL_vLeaveNonOperatingMode(void)
{
    BAL_bNonOperatingMode = false;
}

bool BAL_bIsNonOperatingMode(void)
{
    return BAL_bNonOperatingMode;
}

uint8_t BAL_ucGetSubsystemIndex(uint8_t ucNodeAddress)
{
#ifndef VARIANT_PROGRAMMER
   uint8_t ucSubsystemIndex;             /*The index of the subsystem in the list of modules*/
   uint8_t ucSubsystem = DLL_GET_SUBSYSTEM(ucNodeAddress); /*Mask base address (base address and broadcast addressing result in the same subsystem).*/
   for (ucSubsystemIndex = 0; ucSubsystemIndex < BAL_ucNumberOfSubsystems; ucSubsystemIndex++)
   {
      if (BAL_tBusObject[ucSubsystemIndex].ucSubsystem == ucSubsystem)
      {
         return ucSubsystemIndex;
      }
      else
      {
         /*Index not found yet, continue searching.*/
      }
   }
   /*Subsystem not found - 'cause it isn't defined*/
#else
   (void)ucNodeAddress;/*Here, only subsystem zero is present.*/
#endif
   return 0U; /*If the requested Subsystem is not found in the table, the first index (0) is returned, as this is always present.*/
}

uint8_t BAL_ucGetMessageIndex(TbusMessageIdentifier tMessageIdentifier, uint8_t ucSubsystemIndex)
{
   const TbusObjectTable *ptBusObject;
   typedef union _TReceiveObj {//lint !e9018  "union used.." MisraC2012 19.2
    const TbusReceiveObject *ptReceiveObject;
    const TbusIdentifier *ptBusId;
   } TReceiveObj;
   
   TReceiveObj receiveObj;/*lint !e9018 union used to prevent suspicious casts */
   uint8_t ucMessageIndex;             /*The index of the message in the list of messages*/

   ptBusObject = &BAL_tBusObject[ucSubsystemIndex];
   /*lint -e{850} Infinite loop must be of form 'for ( ; ; )'	MisraC2012	14.2 */
   for (ucMessageIndex = 0; ; ucMessageIndex++) /*NB! If a message is sought, and the last entry is not marked with MSB set, this search might never end...*/
   {
      receiveObj.ptReceiveObject = &(ptBusObject->ptReceiveTable[ucMessageIndex]);
      receiveObj.ptBusId = &receiveObj.ptReceiveObject->tBusIdentifier;
      if ((receiveObj.ptBusId->tMessageIdentifier != tMessageIdentifier) && (BAL_bIsIdentifierInRangeOfReceiveTable(tMessageIdentifier, ucSubsystemIndex) == false))
      {
         /*Index not found yet, continue searching.*/
         if ((receiveObj.ptBusId->ucMessageLength & BAL_REC_TAB_END_BIT) != 0U) /*MSB Set: Identifies that this is the last message in the list.*/
         {
            /* End of message list.
               Message not found - 'cause it isn't defined */
            return BAL_UNKNOWN_MESSAGE;
         }
         else
         {
            /*Due to the missing condition in the for statement no return is reached through this branch.*/
         }
      }
      else
      {
         return ucMessageIndex;
      }
   }
}

/*lint -save -e9016 "Pointer arithmetic MisraC2012 18.4 advisory" tested to work correctly */
uint8_t BAL_ucGetMessageTransmitIndex(TbusMessageIdentifier tMessageIdentifier, uint8_t ucSubsystemIndex)
{
   const TbusObjectTable *ptBusObject;

   typedef union _TTransmitObj//lint !e9018  "union used.." MisraC2012 19.2
   {
      const TbusTransmitObject *ptTransmitObject;
      const TbusIdentifier *ptBusId;
   } TTransmitObj;

   TTransmitObj transmitObj;/*lint !e9018 union used to prevent suspicious casts */
   uint8_t ucMessageIndex;             //The index of the message in the list of messages
   ptBusObject = BAL_tBusObject + ucSubsystemIndex;// Pointer to the bus object referred to by BAL_ucSubsystemIndex. 
   for (ucMessageIndex = 0; ucMessageIndex < *ptBusObject->pucNumberOfElementsInTransmitTable; ucMessageIndex++)
   {
      transmitObj.ptTransmitObject = &(ptBusObject->ptBusTransmitTable[ucMessageIndex]);
      transmitObj.ptBusId = &transmitObj.ptTransmitObject->tBusIdentifier;
      if (transmitObj.ptBusId->tMessageIdentifier == tMessageIdentifier)
      {
         return ucMessageIndex;
      }
      else
      {
         //Index not found yet, continue searching.
      }
   }
   //End of message list.
   //Message not found - 'cause it isn't defined
   return BAL_UNKNOWN_MESSAGE;
}

#ifdef RTOS_DBUS_EVENTDRIVEN
bool BAL_SendMessage(void)
{
   uint16_t    uiUserCode;
   uint16_t    uiMask;
   bool    bMessageToTransmit = false;
   TbusIdentifier tBusId;
   TbusService tServiceFunc;
   uint8_t ucDataLength;
   const TbusObjectTable *ptBusObject;
   const TbusTransmitObject *ptTransmitObject;
   bool HandlingDone = false;

   BAL_vEnterCriticalSectionForSending();

   if( BAL_uiTransmitFlagSubsystemIndex != 0U )
   {
      uiMask = 0x0001U;
      for (BAL_ucSubsystemIndex = 0; BAL_ucSubsystemIndex < BAL_MAX_SUBSYS_NO; BAL_ucSubsystemIndex++) //Find the bit number...
      {
         if ((BAL_uiTransmitFlagSubsystemIndex & uiMask) != 0U)
         {
            //Found subsystem with message due for transmission.
            break;
         }
         else
         {
            //Continue searching.
            uiMask <<= 1;
         }
      }

      ptBusObject = BAL_tBusObject + BAL_ucSubsystemIndex;// Pointer to the bus object referred to by BAL_ucSubsystemIndex.
      for (BAL_ucMessageIndex = 0U; BAL_ucMessageIndex < *ptBusObject->pucNumberOfElementsInTransmitTable; BAL_ucMessageIndex++)
      {
         if (UTI_bGetBit(ptBusObject->pucMessageToTransmitBits, BAL_ucMessageIndex))
         {
            //BAL_ucMessageIndex within the corresponding Subsystem has been found
            bMessageToTransmit = true;
            break;
         }
         else
         {
            //Continue searching for Message to send.
         }
      }

      if((DBLK_IsDbusLockActive() == false) && (DBPL_bIsSendingPowerMsg() == false))
      {
         DBM_DISABLE_INT(); //Interrupts are enabled again in both branches below.
         if (bMessageToTransmit != false)
         {
            //Send the message:
            uiUserCode = ((uint16_t)(BAL_ucSubsystemIndex)<<BYTE_SIZE)|BAL_ucMessageIndex;
            DBM_ENABLE_INT();      //Interrupts are disabled before entering this branch...
            ptBusObject = BAL_tBusObject+BAL_ucSubsystemIndex;  // Pointer to the bus object referred to by BAL_ucSubsystemIndex.
            ptTransmitObject = &(ptBusObject->ptBusTransmitTable[BAL_ucMessageIndex]); //Pointer to the transmit table referred to by BAL_ucSubsystemIndex.
            tBusId = ptTransmitObject->tBusIdentifier; //Datatype (bus identifier) contains three elements -> assignment could be problematic for old Kernigan-Ritchie compilers
            tServiceFunc = ptTransmitObject->tServiceFunction;
            ucDataLength = ptTransmitObject->ucDataLen;
            if (DLL_bTransmitMessage(tBusId, tServiceFunc, ucDataLength, uiUserCode) != false)
            {
               DBM_DISABLE_INT();
               BAL_vResetTransmissionBit(BAL_ucSubsystemIndex, BAL_ucMessageIndex);
               DBM_ENABLE_INT();
               //Message successfuly tansmitted for this step
            }
            //Keep calling this handler. If once bMessageToTransmit == false, when no messages are found, handler will be ended
            HandlingDone = false;
         }
         else
         {
            //Remove bit telling that there is a message due for transmission in this subsystem.
            BAL_uiTransmitFlagSubsystemIndex &= ~((uint16_t)((uint16_t)1u<<(uint16_t)BAL_ucSubsystemIndex));
            BAL_ucMessageIndex = 0U; //No message is being handled by the task handler any more...
            HandlingDone = true;
            DBM_ENABLE_INT(); //Interrupts are disabled before entering this branch...
         }
      }
      else
      {
          //Keep callinng this handler, to retry sending at next interation
          HandlingDone = false;
      }
   }
   else
   {
      //No message due to be sent
      HandlingDone = true;
   }
   //Only one message is sent each time the task handler is called...
   BAL_vExitCriticalSectionforSending();

   return HandlingDone;
}
#else
uint8_t BAL_HandleTask(void)
{
   uint16_t    uiUserCode;
   uint16_t    uiMask;
   bool    bMessageToTransmit = false;
   TbusIdentifier tBusId;
   TbusService tServiceFunc;
   uint8_t ucDataLength;
   const TbusObjectTable *ptBusObject;
   const TbusTransmitObject *ptTransmitObject;

   BAL_vEnterCriticalSectionForSending();
   DBM_UART_vDisableRxEvent();
   if (BAL_uiTransmitFlagSubsystemIndex != 0U)
   {
      if (DLL_bIsBusReadyForTransmission() != false)
      {
         if (BAL_bIsSendingRetry() == false)  //lint !e774 the message repetition is handled by DBusCAN chip for DBus with DBusCAN
         {
            uiMask = 0x0001U;
            for (BAL_ucSubsystemIndex = 0; BAL_ucSubsystemIndex < BAL_MAX_SUBSYS_NO; BAL_ucSubsystemIndex++) //Find the bit number...
            {
               if ((BAL_uiTransmitFlagSubsystemIndex & uiMask) != 0U)
               {
                  //Found subsystem with message due for transmission.
                  break;
               }
               else
               {
                  //Continue searching.
                  uiMask <<= 1;
               }
            }

            ptBusObject = BAL_tBusObject + BAL_ucSubsystemIndex;// Pointer to the bus object referred to by BAL_ucSubsystemIndex.
            for (BAL_ucMessageIndex = 0U; BAL_ucMessageIndex < *ptBusObject->pucNumberOfElementsInTransmitTable; BAL_ucMessageIndex++)
            {
               if (UTI_bGetBit(ptBusObject->pucMessageToTransmitBits, BAL_ucMessageIndex))
               {
                  //BAL_ucMessageIndex within the corresponding Subsystem has been found
                  bMessageToTransmit = true;
                  break;
               }
               else
               {
                  //Continue searching for Message to send.
               }
            }
         }
         else
         {
            //Repeat currently transmitted message, as there was some problem (missing or negative acknowledge).
            bMessageToTransmit = true;
         }

         if((DBLK_IsDbusLockActive() == false) && (DBPL_bIsSendingPowerMsg() == false))
         {
            DBM_DISABLE_INT(); //Interrupts are enabled again in both branches below.
            if (bMessageToTransmit != false)
            {
               //Send the message:
               uiUserCode = ((uint16_t)(BAL_ucSubsystemIndex)<<BYTE_SIZE)|BAL_ucMessageIndex;
               DBM_ENABLE_INT();      //Interrupts are disabled before entering this branch...
               ptBusObject = BAL_tBusObject+BAL_ucSubsystemIndex;  // Pointer to the bus object referred to by BAL_ucSubsystemIndex.
               ptTransmitObject = &(ptBusObject->ptBusTransmitTable[BAL_ucMessageIndex]); //Pointer to the transmit table referred to by BAL_ucSubsystemIndex.
               tBusId = ptTransmitObject->tBusIdentifier; //Datatype (bus identifier) contains three elements -> assignment could be problematic for old Kernigan-Ritchie compilers
               tServiceFunc = ptTransmitObject->tServiceFunction;
               ucDataLength = ptTransmitObject->ucDataLen;
               if (DLL_bTransmitMessage(tBusId, tServiceFunc, ucDataLength, uiUserCode) != false)
               {
                  DBM_DISABLE_INT();
                  BAL_vResetTransmissionBit(BAL_ucSubsystemIndex, BAL_ucMessageIndex);
                  DBM_ENABLE_INT();
               }
            }
            else
            {
               //Remove bit telling that there is a message due for transmission in this subsystem.
               BAL_uiTransmitFlagSubsystemIndex &= ~((uint16_t)((uint16_t)1u<<(uint16_t)BAL_ucSubsystemIndex));
               BAL_ucMessageIndex = 0U; //No message is being handled by the task handler any more...
               DBM_ENABLE_INT(); //Interrupts are disabled before entering this branch...
            }
         }
      }
      else
      {
         //The bus is not idle.
      }
   }
   else
   {
      //No message due to be sent.
   }
   //Only one message is sent each time the task handler is called...
   DBM_UART_vEnableRxEvent();
   BAL_vExitCriticalSectionforSending();
   return TASK_INITIALISED;
}
#endif //#ifdef RTOS_DBUS_EVENTDRIVEN

/* D-Bus-2 */
bool BAL_bDispatchRcvdDbus2Msg(const TbusMessage *ptReceivedMessage)
{
   /* The parameter of this function could have been of type const, but due to the call of the generic service function, where the parameter is not const, this would need a cast to (uint8_t *). That would then eliminate the advantage of using const parameter in this function... */
   uint8_t ucSubsystemIndex;
   uint8_t ucMessageIndex;

   uint8_t ucReceivedTargetAddress;
   TbusIdentifier tBusId;
   TbusService tServiceFunc;
   uint8_t ucDataLength;
   const TbusObjectTable *ptBusObject;
   const TbusReceiveObject *ptReceiveObject;
   tBusId = ptReceivedMessage->tBusIdentifier;
   ucReceivedTargetAddress = tBusId.ucTargetAddress;
   BAL_tReceivedMessageId = tBusId.tMessageIdentifier;
   ucSubsystemIndex = BAL_ucGetSubsystemIndex(ucReceivedTargetAddress);

   ucMessageIndex = BAL_ucGetMessageIndex(BAL_tReceivedMessageId, ucSubsystemIndex);
   if (ucMessageIndex == BAL_UNKNOWN_MESSAGE)
   {
       /*Message not in given table, look in at least one other Subsystem*/
       uint8_t noOfSubsystemsToBeSearched = 0;
       bool foundOtherSubsystem = false;

       if(ucReceivedTargetAddress != 0x00U)/*No broadcast, or only to one subsystem.*/
       {
           noOfSubsystemsToBeSearched = 1;/*Only look at subsystem Index zero, service messages.*/
       }
       else/*Proper broadcast, look through all subsystems.*/
       {
           noOfSubsystemsToBeSearched = BAL_ucNumberOfSubsystems;
       }

       for(ucSubsystemIndex = 0; ucSubsystemIndex < noOfSubsystemsToBeSearched; ucSubsystemIndex ++)
       {
           ucMessageIndex = BAL_ucGetMessageIndex(BAL_tReceivedMessageId, ucSubsystemIndex);
           if (ucMessageIndex != BAL_UNKNOWN_MESSAGE)
           {
               foundOtherSubsystem = true;
               break;
           }
       }

       if(foundOtherSubsystem == false)
       {
           return false;
       }
   }

   ptBusObject = BAL_tBusObject+ucSubsystemIndex;// Pointer to the bus object referred to by BAL_ucSubsystemIndex.
   ptReceiveObject = &(ptBusObject->ptReceiveTable[ucMessageIndex]);
   tServiceFunc = ptReceiveObject->tServiceFunction;
   ucDataLength = ptReceivedMessage->tBusIdentifier.ucMessageLength - BAL_D_BUS_2_MESSAGE_LENGTH_OFFSET;
   //distribute received message via the corresponding service function.
   //The service function is found in the receive object table, which is accessed via the bus object table (i.e. two tables are accessed via two indexes):
   (tServiceFunc)(ucDataLength, (uint8_t*)ptReceivedMessage->aucData);/*lint !e9005 !e926 "cast form pointer to pointer" , tested to work correctly */
   return true;
}
/*lint -restore -e9016 */

void BAL_vTransmitMessage(uint8_t ucSubsystem, uint8_t ucMessageNumber)
{
   BAL_vEnterCriticalSectionForSending();
   if((DLL_isSilentMode() == false) && (BAL_bDoesOperatingModeBlockApply(ucSubsystem) == false))
   {
      uint8_t ucSubsystemIndex;
      ucSubsystemIndex = BAL_ucGetSubsystemIndex(ucSubsystem);
      if (ucMessageNumber < *BAL_tBusObject[ucSubsystemIndex].pucNumberOfElementsInTransmitTable)
      {
         DBM_DISABLE_INT(); //Encapsulation of bit operations to allow BAL_vTransmitMessage() to be called from interrupt.
         BAL_uiTransmitFlagSubsystemIndex |= (uint16_t)((uint16_t)1u<<(uint16_t)ucSubsystemIndex); //Mark that there is a message due for transmission in this subsystem.
         UTI_vSetBit(BAL_tBusObject[ucSubsystemIndex].pucMessageToTransmitBits, ucMessageNumber, true); //Mark the current message for transmission.
         DBM_ENABLE_INT();
      }
      else
      {
         BAL_vNotifyError((uint8_t)BAL_ERROR_UNKNOWN_MESSAGE_FOR_TRANSMISSION);
      }
   }

   //Set eventflag, to run DBR_Handle_BAL()
   DBR_RunEventdrivenDbusTask( DBR_ED_TASK_BAL );

   BAL_vExitCriticalSectionforSending();
}


void BAL_vCancelTransmitMessage(uint8_t ucSubsystem, uint8_t ucMessageIndex)
{
#define MAX_TRANSMISSION_RETRIES ((uint8_t)0xFFU)
   /*
   This function will be called if an application regrets that a message was flagged for transmission, or if a transmit request is not valid after some time. Flag, which is set in the BAL_vTransmitMessage() function will be reset. This function may not be called from an interrupt (problems with consistency in case BAL_vHandleTask is running at this instance).
   */
   uint8_t ucSubsystemIndex;
   ucSubsystemIndex = BAL_ucGetSubsystemIndex(ucSubsystem);
   DBM_DISABLE_INT();
   if (BAL_ucTransmissionAttempts != 0U)
   {
      if ((ucSubsystemIndex == BAL_ucSubsystemIndex) && (ucMessageIndex == BAL_ucMessageIndex))
      {
         BAL_ucTransmissionAttempts = MAX_TRANSMISSION_RETRIES; //Make sure the max transmission retries is at least reached, hence use the highest available value.
      }
   }
   BAL_vResetTransmissionBit(ucSubsystemIndex, ucMessageIndex);
   DBM_ENABLE_INT();
}

#if !defined(DBM_DBUSCAN)
void DLL_vMessageTransmitted(uint8_t ucAcknowledge, uint16_t uiUserCode)
{
   static uint8_t                    BAL_ucMaxRetries = 0U;
   TbusConfirmationService         tConfirmationFunction = NULL; //Stack variable for optimising the call of the confirmation function (including access via transmit table)

   if (uiUserCode <= (uint16_t)BAL_FIRST_SERVICE_MSGID)/*Sent via BAL*/
   {
      if (DLL_GET_ACKNOWLEDGE(ucAcknowledge) == DLL_ACK_OK)
      {
         if(BAL_tBusObject[(uiUserCode>>BYTE_SIZE)].ptBusTransmitTable != NULL)
         {
             tConfirmationFunction = *BAL_tBusObject[(uiUserCode>>BYTE_SIZE)].ptBusTransmitTable[(uint8_t)uiUserCode].tConfirmationFunction;
         }

         if (tConfirmationFunction != NULL)//Call confirmation function, if any (i.e. not 0) is defined.
         {
            tConfirmationFunction();
         }
         else
         {

         }
      }
      else
      {
         if (DLL_GET_ACKNOWLEDGE(ucAcknowledge) == DLL_ACK_TRANSMISSION_ABORTED)
         {
            //Collision appeared
            if (BAL_ucMaxRetries < BAL_ucMaxCollisionRetries)
            {
               BAL_ucMaxRetries = BAL_ucMaxCollisionRetries;
            }
         }
         else
         {
            if (DLL_GET_ACKNOWLEDGE(ucAcknowledge) == DLL_ACK_NOT_RECEIVED)
            {
               //Acknowledgement Time-out
               if (BAL_ucMaxRetries < BAL_ucMaxMissingAckRetries)
               {
                  BAL_ucMaxRetries = BAL_ucMaxMissingAckRetries;
               }
            }
            else
            {
               //Other Acknowledgement problems (wrong CRC or BUSY)
               if (BAL_ucMaxRetries < BAL_ucMaxNackRetries)
               {
                  BAL_ucMaxRetries = BAL_ucMaxNackRetries;
               }
            }
         }
         //The message, which was tried sent last time has not yet been acknowledged. Check whether the message should be sent again, due to problems with the receiver.
         if (BAL_ucTransmissionAttempts >= BAL_ucMaxRetries)
         {
            BAL_vNotifyNonDeliverableMessage(BAL_tBusObject[(uiUserCode>>BYTE_SIZE)].ucSubsystem, (uint8_t)uiUserCode);
         }
         else
         {
            BAL_ucTransmissionAttempts++;
            return;
         }
      }
      BAL_ucTransmissionAttempts = 0U;  //Reset the counter, which counts the retries of last message transmission.
      BAL_ucMaxRetries = 0U;
   }
   else
   {
      //This is a service message.
      if (DLL_GET_ACKNOWLEDGE(ucAcknowledge) == DLL_ACK_OK)
      {
         DBPL_vStopRepeatServiceMsg();
      }
      else
      {
         DBPL_vDecrementRepeatServiceMsg();
      }
   }
   return;
}
#endif //!DBM_DBUSCAN

void BAL_vSetTransmitMsg(TbusTransmitObject* BAL_tTransmitObject, uint8_t ucNode, uint16_t uiMsgId, uint8_t ucDataLength)
{
    if(BAL_tTransmitObject != NULL)
    {
        BAL_tTransmitObject->tBusIdentifier.ucTargetAddress = ucNode;
        BAL_tTransmitObject->tBusIdentifier.tMessageIdentifier = uiMsgId;
        BAL_tTransmitObject->ucDataLen = ucDataLength;
    }
}

TbusMessageIdentifier BAL_tGetReceivedMessageId(void)
{
   return BAL_tReceivedMessageId;
}

void BAL_ClearMessagesToSend(void)
{
   //go through all subsystems, last one has to have the index of 0xF
   for (uint8_t ucSubsystemIndex = 0; BAL_tBusObject[ucSubsystemIndex].ucSubsystem != 0xFu; ucSubsystemIndex++)
   {
      //go through message tables
      for (uint8_t ucMessageNumber = 0; ucMessageNumber < *BAL_tBusObject[ucSubsystemIndex].pucNumberOfElementsInTransmitTable; ucMessageNumber++)
      {
         UTI_vSetBit(BAL_tBusObject[ucSubsystemIndex].pucMessageToTransmitBits, ucMessageNumber, false);
      }
   }
}

bool BAL_bIsSendingRetry(void)
{
#ifdef DBM_DBUSCAN
    return false;    // the message repetition is handled by chip
#else
    return (BAL_ucTransmissionAttempts > 0U) ? true : false;
#endif
}
