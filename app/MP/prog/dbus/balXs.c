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
 *     |  -uchar     Subsystem                    |  -  -  -  -  - >  |  -uint32 Busidentifier              |
 *     |  -reference to ReceiveObjectSubsystem    |  -                |  -uint8 DataLength                  |
 *     |  -reference to TransmitObjectSubsystem   |     -             |  -reference to void ServiceFunction |
 *     |  -reference to NumberOfElementsInTxTable |        -          --------------------------------------
 *     |  -reference to MessageToTransmitBits     |           -       ______________________________________
 *     -------------------------------------------               - >  |TransmitObjectSubsystem Table        |
 *                                                                    |  -uint32 Busidentifier              |
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
#include "libtypes.h"

#include "bustypes.h"
#include "bal.h"
#include "utility.h"
#include "dbuspresentation.h"



#ifdef TENG_INCLUDED 
#include "testengine/teng_msgsrv.h"
#include "testengine/teng_msgsrv_xs.h"
#endif
#include "hsup.h"

#ifdef CCSS
#include "CocoDbus2PedFwAdapter.h"
#endif
#ifdef RTOS
#include "rtos_api.h"
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
RTOS_MUTEX mutex_BAL =
{
    .priority_inheritance = true,
};
#endif


static uchar BAL_ucEmptyTxTable = 0;                 //!< Constant 0, used in BAL_tBusObject

const TbusObjectTable BAL_tBusObject[] = {
 {BAL_SERVICE_SUBSYSTEM_ID, REC_SERVICE_MSG_TABLE, NO_TX_TABLE,  &BAL_ucEmptyTxTable,   &BAL_ucEmptyTxTable} /*lint !e929 Cast tested to work correctly */
/* Example: {0x1, (const TbusReceiveObject *) BAL_tReceiveObject_Subsystem_A, (const TbusTransmitObject *) BAL_tTransmitObject_Subsystem_X, &BAL_ucNumberOfElementsInSubsystem_X, &BAL_ucTransmitFlagSubsystem_X} */
#ifdef CCSS
 ,{DBUS2_COCO_SUBSYS, (const void *) COCO_Dbus2PedFWAdapter_rxObject, (const void *) COCO_Dbus2PedFWAdapter_txObject, &COCO_Dbus2PedFWAdapter_numberOfTxObjects, COCO_Dbus2PedFWAdapter_txFlags}
#endif
#ifdef TENG_INCLUDED
   TENG_BAL_BUSOBJ_ENTRY
#endif
};

const uchar BAL_ucNumberOfSubsystems = (uchar)UTI_NELEMENTS(BAL_tBusObject); //!< A constant telling how many subsystems are available in the system (needed e.g. for transmission of messages).
const uchar BAL_ucMaxCollisionRetries = BAL_MAX_TRANSMISSION_RETRIES_AFTER_COLLISION; //!<A constant telling how many transmission attempts (retries) are made before giving up transmission of a message.
const uchar BAL_ucMaxMissingAckRetries = BAL_MAX_TRANSMISSION_RETRIES_AFTER_MISSING_ACKNOWLEDGE; //!< A constant telling how many transmission attempts (retries) are made before giving up transmission of a message.
const uchar BAL_ucMaxNackRetries = BAL_MAX_TRANSMISSION_RETRIES_AFTER_NEGATIVE_ACKNOWLEDGE; //!<A constant telling how many transmission attempts (retries) are made before giving up transmission of a message.
/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/

void BAL_vNotifyNonDeliverableMessage(uchar ucSubsystem, uchar ucMessageIndex)
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
#ifdef CCSS
      case DBUS2_COCO_SUBSYS:
          switch (ucMessageIndex)
          {
              case 0U:
                  COCO_Dbus2PedFWAdapter_AfterTxCallback0(CC_DBUS2_PED_FW_ADAPTER_ERROR);
                  break;
              case 1U:
                  COCO_Dbus2PedFWAdapter_AfterTxCallback1(CC_DBUS2_PED_FW_ADAPTER_ERROR);
                  break;
              default:
                  break;
          }
          break;
#endif
   }
}

#define BAL_ERROR_TYPES 4U

void BAL_vNotifyError(uchar ucError)
{
   static uchar BAL_aucErrorCounter[BAL_ERROR_TYPES];

   if (ucError < BAL_ERROR_TYPES)
   {
      (void)BAL_aucErrorCounter[ucError]; /*just access variable*/
      BAL_aucErrorCounter[ucError]++;

   }
}

void BAL_vNotifyUnknownMessage(uchar ucReceivedTargetAddress, TbusMessageIdentifier tReceivedMessageId)
{
    (void)ucReceivedTargetAddress;
    (void)tReceivedMessageId;
}

BOOL BAL_bIsIdentifierInRangeOfReceiveTable(TbusMessageIdentifier tMessageIdentifier, uchar ucSubsystemIndex)
{
    BOOL ret = FALSE;
    (void)tMessageIdentifier;
    (void)ucSubsystemIndex;
#ifdef CCSS
    uchar mySubsystemIndex = BAL_ucGetSubsystemIndex(DBUS2_COCO_SUBSYS);
    uint16 lowestMsgIdInMyLib = (uint16_t)CM_MESSAGES_RANGE_START;
    uint16 highestMsgIdInMyLib = (uint16_t)CM_MESSAGES_RANGE_END;
    if(mySubsystemIndex == ucSubsystemIndex)
    {
         if((tMessageIdentifier >= lowestMsgIdInMyLib) && (tMessageIdentifier <= highestMsgIdInMyLib))
         {
             ret=TRUE;
         }
    }
#endif
    return ret;
}


void BAL_vEnterCriticalSectionForSending(void)
{
#ifdef RTOS
    (void)RTOS_EnterCriticalSection(&mutex_BAL);
#endif
}


void BAL_vExitCriticalSectionforSending(void)
{
#ifdef RTOS
    (void)RTOS_ExitCriticalSection(&mutex_BAL);
#endif
}
