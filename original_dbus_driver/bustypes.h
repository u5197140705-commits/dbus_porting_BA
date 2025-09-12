/**************************************************************************************************************
* Copyright by BSH Bosch und Siemens Hausgeraete GmbH - EDS                                             
*                                                                                                       
*-------------------------------------------------------------------------------------------------------
*                                                                                                       
* Use, reproduction and dissemination of this software or parts of it, is not permitted without express 
* written authority of BSH EDS. The user is allowed to use this software exclusively for the development
* of electronic boards for BSH. Usage for other purposes is strictly prohibited (e.g. development of    
* electronic boards for third parties). All rights, including copyright, rights created by patent grant 
* or registration, and rights by protection of utility patents, are reserved. Violations will be        
* prosecuted by civil and criminal law.                                                                 
*                                                                                                       
* The software was created and approved by acknowledged rules of technology. Because of the complexity  
* of embedded controller software the user of this software has to perform his own tests to ensure      
* proper functionality in his environment. The software was developed for usage as a software library.  
* The user is sole responsible for every other usage.  The user may modify the software for adaptations 
* needed for other microcontrollers, as well as to another compiler at own risk. BSH will not           
* support any adaptations or changes. BSH is authorised to use all adaptations for own purposes free of 
* charge.                                                                                               
*                                                                                                       
* BSH assumes no liability for the functionality or reliability of the software even in concrete        
* applications. BSH assumes no liability for consequential damages, except in case of intention or gross
* negligence. In any case the liability is limited to the typical and predictable damage.               
*                                                                                                       
* Technical changes are reserved.                                                                       
*                                                                                                       
********************************************************************************************************
 *   PROJECT          SW_LIB_D_BUS_II
 *   MODULE-PREFIX    ---
 *   FILE             %PM%
 *   ARCHIVE          %PP%:%PI%
 *   PROCESSOR        independent of processor - tested on Freescale 68HC08, Renesas R8C and M16C
 *   COMPILER         HiWare-C
 *****************************************************************************************************************
 *   \author          Rune Holen
 *   \date            11.09.2003
 *
 *   LAST CHANGE      %PRT%
 *   \version         %PR%
 *   STATUS           %PS%
 *****************************************************************************************************************
 *   DESCRIPTION
 */
 /** \file 
 *     Genaral types used by the bus extension (tyically D-Bus-2)
 */
 /*****************************************************************************************************************
 *   CHANGES
 *
 * %PL%
 *
 **************************************************************************************************************/

#ifndef BUSTYPES_H__
#define BUSTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "LibTypes.h"

#define BUS_MESSAGE_OVERHEAD                         ((uint8_t)2U) //!<A message to save is ucMessageLength + 2 byte big (the 2 bytes not regarded by the message length: ucMessageLength and Target Address (CRC will not be saved, only evaluated).
#define BUS_MESSAGE_OVERHEAD_INCLUDING_IDENTIFIER    ((uint8_t)4U) //!< The tMessageIdentifier is 2 byte long, hence a bus message including the message identifier and the DLL_MESSAGE_OVERHEAD is 2+2 bytes long. This definition is used by the data link layer.
#define BUS_MESSAGE_ID_LEN                           ((uint8_t)2U) //!< The length of the message ID in bytes.
#define BUS_MESSAGE_MIN_FRAME_LENGTH                 ((uint8_t)6U) //!< The length of a frame, which contains only a Message ID, no additional data.

// define as empty if not set platform specific
#ifndef PACKED
#define PACKED
#endif
#ifndef POSTPACKED
#define POSTPACKED
#endif



/**
 Typedefinition used for data exchanging services. When data is received or transmitted,
 the data belonging to the message will be exchanged from the bus to the application (receive) or from the application to the bus (transmission) via this service.

\param ucDataLen
DataLength of the service\n
\b type        : uint8_t\n
\b range       : 0..249\n
\b scale       : []

\param pucData
Data of the service\n
\b type        : * uint8_t\n
\b range       : []

*/
typedef void (*TbusService)(uint8_t ucDataLen, uint8_t *pucData);


/**
Typedefinition for service, which may be called when a message has been sent on the bus.
*/
typedef void (*TbusConfirmationService)(void);


typedef uint16_t TbusMessageIdentifier;     //!< Typedefinition for the 16 bit message identifier.


/**
 Typedefinition for the bus identifier, containing the message length, the target address
 and the 16 bit message identifier.

 Background:
 BusIdentifier for CAN is 29 bit long (4 byte), for this reason, 4 byte length makes D-Bus-2
 being easier compatible.
 To make the exchange of bus system from D-Bus-2 to CAN is therefore easy (minor adaptions,
 or no adaptions necessary). The D-Bus-2 uses 16 bit identifier, which together with
 the target address (1 byte) and ucMessageLength make 4 byte

\param ucMessageLength
ucMessageLength contains the message identifier: DataLength + 2 (length of MsgID) = ucMessageLength\n
 ucMessageLength + 4 (MsgLen + TA + 16bit CRC) = FrameLength\n
\b type        : uint8_t\n
\b range       : 0..251

\param ucTargetAddress
ucTargetAddress contains the address of the corresponding communication partner in upper nibble, and the addressed subsystem in the lower nibble.\n
\b type        : uint8_t\n
\b range       : 0x00..0xFF

\param tMessageIdentifier
Identifier of the message. The identifier includes information, which categorise
 the message as belonging to e.g. network management, presentation layer,
 application layer or remote control.\n
\b type        : TbusMessageIdentifier

*/
typedef PACKED struct POSTPACKED _TBusIdentifier
{
       uint8_t                  ucMessageLength;
       uint8_t                  ucTargetAddress;
       TbusMessageIdentifier  tMessageIdentifier;
}TbusIdentifier;


/**
 A message consists of the bus identifier and the corresponding message data.

Important: The size of the data-element may differ from these predefined 4 bytes, as this definition
 may be changed by redefining the message in the excess template. According to this fact (variable length of data)
 the maximum length of the message content is application dependent.

\param tBusIdentifier
Typedefinition for the bus identifier, containing: Message length,
target address and the 16 bit message identifier.\n
\b type        : TbusIdentifier\n
\b range       : 0x00000000..0xFFFFFFFF

\param aucData
Data\n
\b type        : uint8_t[]

\note: The value used for aucData (0) is the minimum length. The size of this default type must be redefined in the excess template of data link layer and presentation layer. The defined value of DLL_TRANSMIT_BUFFER_DATA_LENGTH (and possibly DBPL_SIZE_OF_DATA_BUFFER_IN_T_BUS_MESSAGE) allows the usage of larger messages.
\note: For compilers not supporting C99 standard, the old solution with standard length 6 for aucData is used.
*/
typedef PACKED struct POSTPACKED _TBusMessage
{
       TbusIdentifier  tBusIdentifier;
#if __STDC_VERSION__ >= 199901L /*C99 or higher*/
/* Note 9038: Flexible array members shall not be declared. MisraC2012 18.7
   Tested to work correctly */
       uint8_t           aucData[];/*lint !e9038 */
#else
       uint8_t           aucData[6];
#endif
}TbusMessage;/*lint !e9038 Arrays with flexible length not liked by Lint, but they reflect reality. This structure is always overridden during linkage, so we can be sure the real size of the array is clearly defined.*/



/**
 A message consists of the bus identifier and the corresponding message data.

Important: The size of the data-element may differ from these predefined 4 bytes, as this definition
 may be changed by redefining the message in the excess template. According to this fact (variable length of data)
 the maximum length of the message content is application dependent.

\param tBusIdentifier
Typedefinition for the bus identifier, containing the message length,
 the target address and the 16 bit message identifier.\n
\b type        : TbusIdentifier\n
\b range       : 0x00000000..0xFFFFFFFF

\param aucData
Data\n
\b type        : uint8_t[]

*/
typedef PACKED struct POSTPACKED _TCanMessage
{
       TbusIdentifier  tBusIdentifier;
       uint8_t           aucData[6];
}TcanMessage;   //NB! Please adapt to "CAN philosophy" - if needed (e.g. tBusIdentifier)


/**
 BusReceiveObject
 consists of bus identifier and a reference to the service function,
 where the corresponding data is delivered to the application.

\param tBusIdentifier
The bus identifier identifies the transferred data according to different categories
 (predefined messages, system messages, network management messages and remote control messages).\n
\b type        : TbusIdentifier

\param tServiceFunction
Reference to ServiceFunction, where the data is being transferred from the bus to the application.\n
\b type        : TbusService

*/
typedef PACKED struct POSTPACKED _TBusReceiveObject
{
       TbusIdentifier  tBusIdentifier;
       TbusService     tServiceFunction;
} TbusReceiveObject;


/**
 BusTransmitObject
 consists of bus identifier and a pointer to the service function,
 where the corresponding data is picked up from the application.

\param tBusIdentifier
The bus identifier identifies the transferred data according to different categories
 (predefined messages, system messages, network management messages and remote control messages).\n
\b type        : TbusIdentifier

\param ucDataLen
Length of the data\n
\b type        : uint8_t

\param tServiceFunction
Reference to ServiceFunction, where the data is being transferred from the application to the bus.\n
\b type        : TbusService

\param tConfirmationFunction
Pointer to ConfirmationFunction, where the application can be notified about a
 successfull data transmission.\n
\b type        : TbusConfirmationService

*/
typedef PACKED struct POSTPACKED _TBusTransmitObject
{
       TbusIdentifier          tBusIdentifier;
       uint8_t                 ucDataLen;
       TbusService             tServiceFunction;
       TbusConfirmationService tConfirmationFunction;
}TbusTransmitObject;


/**
 BusObjectTable
 consists of tables containing receive and transmit objects.

\param ucSubsystem
This is a numeric value describing, which subsystem the receive and transmit tables belong to.\n
\b type        : uint8_t

\param tReceiveTable
Reference to the ReceiveTable, the table where the reception services are defined\n
\b type        : TbusReceiveObject *

\param tBusTransmitTable
Reference to the BusTransmitTable, the table where the transmission services are defined\n
\b type        : TbusTransmitObject *

\param ucNumberOfElementsInTransmitTable
Reference to the Number of messages, which are defined in the referenced transmit table.\n
\b type        : const uint8_t *

\param MessageToTransmitBits
Reference to the bitfield (with as many bits as there are messages in the present subsystem,
 i.e. 9 defined messages needs 9 bits, hence a uint16_t will be used) where the messages due for transmission
 are marked.\n
\b type        : uint8_t *

*/
typedef PACKED struct POSTPACKED _TBusObjectTable
{
       uint8_t                    ucSubsystem;
       const TbusReceiveObject  *ptReceiveTable;
       const TbusTransmitObject *ptBusTransmitTable;
       const uint8_t              *pucNumberOfElementsInTransmitTable;
       uint8_t                    *pucMessageToTransmitBits;
} TbusObjectTable;


/**
 BusCommunicationFunction is used for read and write access (see definition of ModuleTable)

\param ucModule
The corresponding MemoryModule\n
\b type        : uint8_t

\param uiAddress
The address to read/write\n
\b type        : uint16_t

\param ucDataLen
How many byte to be read/written\n
\b type        : uint8_t

\param ucData
The data of the services\n
\b type        : uint8_t *

*/
typedef bool (*TbusCommunicationFunction)     (uint8_t   ucModule,
                                               uint16_t  uiAddress,
                                               uint8_t   ucDataLen,
                                               uint8_t   *pucData);

/**
 TbusIdFunction is used for accessing the address of the first element of the id-string.

\param ucModule
 The corresponding MemoryModule\n
\b type        : uint8_t

\param ucIdAddress
 Pointer to where the address of the first element of the id-string is saved.\n
\b type        : uint16_t *

\return
 state of request\n
\b type       : bool\n
\b range      : discrete values: DBPL_DONE (true), DBPL_ONGOING (false)

*/
typedef bool (*TbusIdFunction)       (uint8_t ucModule,
                                       uint16_t *puiIdAddress);


/**
 TbusModuleTable is the table where the read and write access functions,
 as well as the identification function and optimal/maximum block size of each module are defined.

\param ucModule
 The Module refferred to by e.g. EDITH
 type        : uint8_t

\param tComReadFunction
 Reference to the service read-function\n
\b type        : TbusCommunicationFunction

\param tComWriteFunction
  Reference to the service write-function\n
\b type        : TbusCommunicationFunction

\param tIdFunction
  Reference to the function needed for getting the ID address\n
\b type        : TbusIdFunction

\param ucBlockSize
 The BlockSize of this MemoryModule\n
\b type        : uint8_t
*/
typedef PACKED struct POSTPACKED _TBusModuleTable
{
   uint8_t                       ucModule;         //The Module refferred to by e.g. GvWin
   TbusCommunicationFunction   tComReadFunction; //Reference to the address of the service read-function
   TbusCommunicationFunction   tComWriteFunction;//Reference to the address of the service write-function
   TbusIdFunction              tIdFunction;      //Function needed for getting the ID address
   uint8_t                       ucBlockSize;      //The BlockSize of this MemoryModule
}TbusModuleTable;


#ifdef __cplusplus
}
#endif

#endif

