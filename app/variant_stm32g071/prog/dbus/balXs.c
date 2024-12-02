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
*   COMP_ABBREV      BAL
*******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/

 /** \file 
 *     Application layer configuration template for the D-Bus-2 and CAN Bus
 *     
 *     This layer handles the message distribution of the system messages. Received messages, which belong to
 *     system messages (differentiation according to the identifier) will be distributed to the
 *     subsystem corresponding to either the subsystem addressed (when addressed subsystem differs from 0),
 *     or to the subsystem, which is indirectly addressed via a service function in the corresponding ReceiveObject
 *     table.
 *       \image html bal.gif
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
 */     

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stddef.h>
#include "LibTypes.h"

#include "bustypes.h"
#include "bal.h"
#include "utility.h"
#include "dbuspresentation.h"

#if defined(SSBD_INCLUDED) && defined !(SSB_USE_CPP_INSTEAD_OF_C_API)
#include "dbus/msg_lists.h"
#endif

#ifdef DBGX_INCLUDED
#include "debug_extended/dbus/dbgx_msg_lists.h"
#endif

#ifdef TENG_INCLUDED 
#include "testengine/teng_msgsrv.h"
#include "testengine/teng_msgsrv_xs.h"
#endif
#include "hsup.h"

#ifdef CCSS/*SystemStates service.*/
#include "CCCM_Dbus2PedFwMapping.h"
#endif
#ifdef RTOS
#include "rtos_api.h"
#endif

#ifdef DBAL_INCLUDED
#include "dbus/DBal/BshDBus2AppLayer.h"
#include "dbus/DBal/BshDBus2AppLayer_internal.h"
#include "dbus/DBal/DBal_cfg.h"
#endif



/**
 BAL_MAX_TRANSMISSION_RETRIES_AFTER_COLLISION: A message, which is aborted, due to a collision,
 must be repeated in order for any recipient to be able to receive the message correctly.
 In case a node has a hardware defect in the bus interface, it can be advantageous to define a
 definite amount of retries for a message after an occurred collision. This value should be
 at least as large as the expected maximum amount of succeeding collisions in the system (e.g. 16).
*/
#define BAL_MAX_TRANSMISSION_RETRIES_AFTER_COLLISION    16

/**
 BAL_MAX_TRANSMISSION_RETRIES_AFTER_MISSING_ACKNOWLEDGE: A message, which cannot be received on first try
 (e.g. while the addressed node is not present, or broken), will be retried this many times,
 before the corresponding transmit flag is reset.
*/
#define BAL_MAX_TRANSMISSION_RETRIES_AFTER_MISSING_ACKNOWLEDGE    4

/**
 BAL_MAX_TRANSMISSION_RETRIES_AFTER_NEGATIVE_ACKNOWLEDGE: A message, which is not correctly received
 by the receiving node, will be retried this many times, before the corresponding transmit flag is reset.
*/
#define BAL_MAX_TRANSMISSION_RETRIES_AFTER_NEGATIVE_ACKNOWLEDGE    2

#define NO_TX_TABLE  ((const TbusTransmitObject *)(NULL))            //!<zero pointer with type TbusTransmitObject*
#ifdef GSW_INCLUDE_ONLY_MANDATORY_MESSAGES
        #define REC_SERVICE_MSG_TABLE ((const TbusReceiveObject *) DBPL_tReceiveMandatoryServiceMessages)  //!<Only mandatory service messages
#else
     #ifdef GSW_INCLUDE_16_BIT_OPTIONAL_MESSAGES
        #define REC_SERVICE_MSG_TABLE ((const TbusReceiveObject *) DBPL_tReceiveAll16BitServiceMessages) //!<All service messages except for 32-bit memory access
     #else
        #if defined (DBUS2_UPDATE)
            #define REC_SERVICE_MSG_TABLE ((const TbusReceiveObject *) DBPL_tReceiveAllServiceMessagesUpdate) //!<All service messages including 32-bit memory access and update
        #elif defined (DBUS2_UPDATE_HSI)
            #define REC_SERVICE_MSG_TABLE ((const TbusReceiveObject *) DBPL_tReceiveAllServiceMessagesUpdateHsi) //!<All service messages including 32-bit memory access and update
        #else
            #define REC_SERVICE_MSG_TABLE ((const TbusReceiveObject *) DBPL_tReceiveAllServiceMessages) //!<All service messages including 32-bit memory access
        #endif
     #endif
#endif

#ifdef RTOS
    static char BAL_cMutexName[4] = "BAL";
    RTOS_MUTEX mutex_BAL =
    {
        .mutex_name = BAL_cMutexName,
        .priority_inheritance = true,
    };
#endif


static uint8_t BAL_ucEmptyTxTable = 0u;                 //!< Constant 0, used in BAL_tBusObject
/* Conversion between object pointer and void pointer [const void*->const _TBusReceiveObject*] MisraC2012 11.5 */
/*lint -save -e929 : The cast of object pointer and void pointer is no dangerous here and tested to work correctly */
const TbusObjectTable BAL_tBusObject[] = {
#ifndef VARIANT_PROGRAMMER /*If you want to send messages from subsystem 0 by BAL_vTransmitMessage, modify the entry below.*/
                                                   /*Include your own tx table for subsystem 0 exactly here.*/
 {BAL_SERVICE_SUBSYSTEM_ID, REC_SERVICE_MSG_TABLE, NO_TX_TABLE,  &BAL_ucEmptyTxTable,   &BAL_ucEmptyTxTable}
#else /*Do NOT modify entry below this comment, but entry ABOVE, if you want to send messages from subsystem 0 by BAL_vTransmitMessage.*/
 {BAL_SERVICE_SUBSYSTEM_ID, REC_SERVICE_MSG_TABLE, NO_TX_TABLE,  &BAL_ucEmptyTxTable,   &BAL_ucEmptyTxTable}
#endif
 /* Example: {0x1, (const TbusReceiveObject *) BAL_tReceiveObject_Subsystem_A, (const TbusTransmitObject *) BAL_tTransmitObject_Subsystem_X, &BAL_ucNumberOfElementsInSubsystem_X, &BAL_ucTransmitFlagSubsystem_X} */
#ifdef CCSS
 ,{CCCMTD_SUBSYS, (const void *)CCCMTD_RxObject, (const void *)CCCMTD_TxObject, &CCCMTD_NumberOfTxObjects, CCCMTD_TxFlags}
#endif
#if defined(SSBD_INCLUDED) && defined !(SSB_USE_CPP_INSTEAD_OF_C_API)
 /* SSBD DBus message subsystem */
 ,{SSBD_SUBSYSTEM_ID, (const void *)SSBD_tReceiveObject, (const void *)SSBD_tTransmitObject, &SSBD_numberOfElementsInSubsystem, SSBD_transmitFlags}
#endif
#ifdef DBGX_INCLUDED
 /* debug_extended DBus message subsystem */
 ,{DBGX_SUBSYSTEM_ID, DBGX_tReceiveObject, DBGX_tTransmitObject, &DBGX_numberOfElementsInSubsystem, DBGX_transmitFlags}
#endif
#ifdef TENG_INCLUDED
   TENG_BAL_BUSOBJ_ENTRY
#endif
#ifdef DBAL_INCLUDED
,{DBAL_DBUS_HANDLER_SUBSYSTEM, (const void *) DBAL_DBUS_Handler_RxObject, (const void *) DBAL_DBUS_Handler_TxObject, &DBAL_DBUS_Handler_NumberOfTxObjects, DBAL_DBUS_Handler_TxFlags}
#endif
};
/*lint -restore -e929 */

const uint8_t BAL_ucNumberOfSubsystems = (uint8_t)UTI_NELEMENTS(BAL_tBusObject); //!< A constant telling how many subsystems are available in the system (needed e.g. for transmission of messages).
const uint8_t BAL_ucMaxCollisionRetries = BAL_MAX_TRANSMISSION_RETRIES_AFTER_COLLISION; //!<A constant telling how many transmission attempts (retries) are made before giving up transmission of a message.
const uint8_t BAL_ucMaxMissingAckRetries = BAL_MAX_TRANSMISSION_RETRIES_AFTER_MISSING_ACKNOWLEDGE; //!< A constant telling how many transmission attempts (retries) are made before giving up transmission of a message.
const uint8_t BAL_ucMaxNackRetries = BAL_MAX_TRANSMISSION_RETRIES_AFTER_NEGATIVE_ACKNOWLEDGE; //!<A constant telling how many transmission attempts (retries) are made before giving up transmission of a message.
/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/

void BAL_vNotifyNonDeliverableMessage(uint8_t ucSubsystem, uint8_t ucMessageIndex)
{
   /* Application specific part: 
   -take any necessary action.
   E.g. for remote control, triggering of WrongData should be done here, as the received object does not exist */
   
  (void)ucMessageIndex; // unused param
   switch( ucSubsystem )
   {
      default:
      case 0x0U:
      {
         break;
      }
      case 0x1U:
      {
         break;
      }
#ifdef DBAL_INCLUDED      
      case DBAL_DBUS_HANDLER_SUBSYSTEM:
      {
          DBAL_nonDeliverableMessage(ucMessageIndex);
          break;
      }
#endif
      
#ifdef CCSS
      case CCCMTD_SUBSYS:
      {
          CCCMTD_dbus2PedFwNotifyNondeliverableMessage(ucMessageIndex);
          break;
      }
#endif
   }
}

#define BAL_ERROR_TYPES 4U

void BAL_vNotifyError(uint8_t ucError)
{
   static uint8_t BAL_aucErrorCounter[BAL_ERROR_TYPES] = {0};

   if (ucError < BAL_ERROR_TYPES)
   {
      (void)BAL_aucErrorCounter[ucError]; /*just access variable*/
      BAL_aucErrorCounter[ucError]++;

   }
}

void BAL_vNotifyUnknownMessage(uint8_t ucReceivedTargetAddress, TbusMessageIdentifier tReceivedMessageId)
{
    (void)ucReceivedTargetAddress;
    (void)tReceivedMessageId;
}

bool BAL_bIsIdentifierInRangeOfReceiveTable(TbusMessageIdentifier tMessageIdentifier, uint8_t ucSubsystemIndex)
{
    bool ret = false;
    (void)tMessageIdentifier;
    (void)ucSubsystemIndex;
#ifdef CCSS
    uint8_t subsystemIndexCCSS = BAL_ucGetSubsystemIndex(CCCMTD_SUBSYS);
    uint16_t lowestMsgIdInCCSSLib = (uint16_t)CCCMTD_MSG_RANGE_START;
    uint16_t highestMsgIdInCCSSLib = (uint16_t)CCCMTD_MSG_RANGE_END;
    if(subsystemIndexCCSS == ucSubsystemIndex)
    {
         if((tMessageIdentifier >= lowestMsgIdInCCSSLib) && (tMessageIdentifier <= highestMsgIdInCCSSLib))
         {
             ret=true;
         }
    }
#endif
    return ret;
}


void BAL_vEnterCriticalSectionForSending(void)
{
#ifdef RTOS
    (void)RTOS_enterCriticalSection(&mutex_BAL);
#endif
}


void BAL_vExitCriticalSectionforSending(void)
{
#ifdef RTOS
    (void)RTOS_exitCriticalSection(&mutex_BAL);
#endif
}
