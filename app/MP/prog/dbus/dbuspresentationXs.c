/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   COMP_ABBREV      DBPL
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     Presentation layer for the D-Bus-2 - excess template
 *     
 *     This is the excess template, which may be modified in any project using D-Bus-2 Presentation layer to make room
 *     for system dependent implementations/adaptions
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"

#ifdef DOXY_DEVELOPERS_DOC
//Doxygen documentation of compiler switch
/**
Local compiler switch.

If set: Minimal implementation.
*/
#define LSW_DBPL_MINIMAL_IMPLEMENTATION

//Doxygen documentation of compiler switch
/**
Local compiler switch.

If set: Memory module 1 will be defined as a array, which can be addressed starting with address 0. This is relative addressing (offset corresponding to the "global" address of this array) compared to memory module 0. Memory module 1 is an array in RAM, which can be overwritten, however, care should be taken, not to write beyond the end of this array.
*/
#endif

#ifndef DOXY_DEVELOPERS_DOC
//        #define LSW_DBPL_MINIMAL_IMPLEMENTATION
#endif


#include <string.h>    //memcpy()
#include "libtypes.h"
#include "libdefines.h"
#include "bustypes.h"
#include "SegmentDef.h"
#include "timerlib.h"
#include "dbuspresentation.h"
#include "dbuspresentation_production_types.h"
#include "bal.h"
#include "dbusdll.h"

#if defined (LRO_BOOTLOADER_MERGE) || defined (__BOOTLOADER_INCLUDED)
#include "firmware_update/BootManager/BootManagerSharedData.h"
#endif

#include "utility.h"    //UTI_NELEMENTS()

#ifdef DBUS2_UPDATE_HSI
#include "hsi_basic.h"
#endif

/* The functions, which are prototyped here, may be defined/prototyped in an external file, which is included in this excess template */

static BOOL MOD_bRead(uchar ucMemoryModule, uint16 uiAddress, uchar ucDataLen, uchar *pucBuffer);
static BOOL MOD_bWrite(uchar ucMemoryModule, uint16 uiAddress, uchar ucDataLen, uchar *pucBuffer);
static BOOL MOD_bGetId(uchar ucMemoryModule, uint16 *puiIdAddress);
#ifndef LSW_DBPL_MINIMAL_IMPLEMENTATION
/**
 ModuleDict (data type: TbusModuleTable) is a table where the different read/write/id-functions are found, depending on the current memory module.
*/
const TbusModuleTable DBPL_tModuleDict[] = {
/*  Module, ReadFunction, WriteFunction, IdFunction, Blocksize */
       {0, MOD_bRead, MOD_bWrite, MOD_bGetId, 1}      //!<Access to memory module 0
      ,{1, MOD_bRead, MOD_bWrite, MOD_bGetId, 16}     //!<Access to memory module 1
//      ,{2, MOD_bRead, MOD_bWrite, MOD_bGetId, 1}    //!<Access to memory module 2
//      ,{9, MOD_bRead, MOD_bWrite, MOD_bGetId, 32}   //!<Access to memory module 9
};
#endif
const uchar DBPL_ucNumberOfElements = (uchar)UTI_NELEMENTS(DBPL_tModuleDict); //!< A constant containing the number of elements in ModuleDict, i.e. how many memory modules are defined.

/** \defgroup DBPL_Timers Timers used by presentation layer.

The timer concept presently used for HC08 uses variables defined in special segments, which are decremented by the timer-library. If this does not work, an alternative solution must be worked out to supply the following timers. The way to put the variables into segments is done via <i>\#pragma</i>, which may not work for all compilers/processors.
*/

SDEF_SetSegmentRW(TIMER8_10MS)
static Ttimer8 DBPL_tResetTimer;    //!<This is a timer, for keeping track of when to execute a reset (after the corresponding delay [10 ms]).

SDEF_SetSegmentRW(TIMER8_2S)
static Ttimer8 DBPL_tOfflineDelayTimer; //!< Timer, which keeps track of how long to stay offline [2s] - 0 has the meaning of: "until reset".
SDEF_SetSegmentRW_Default()

BOOL DBPL_bEraseBlockSuccess = 0U;      //Can be const, if erase block is not needed by any memory module.

#ifndef ID
#define DBUS2_ID "MyDummyId\0"
#else
#define DBUS2_ID LD_STRINGIZE(ID) "\0"
#endif

#ifndef DBUS2_NODE_DATE
#define DBUS2_NODE_DATE 1111111111 //Dummy date. The date should be defined by the build process.
#endif
#ifndef DBUS2_NODE_ID
//No ID is defined by build chain
#define NODE_ID "This is a dummy ID of this node. The ID should be defined by the build process."
#else
//ID is defined by build chain
#define NODE_ID LD_STRINGIZE(DBUS2_NODE_ID)
#endif

#ifndef PROD_DATA_TRID_ADDR
//Dummy definition to avoid problems with FWU1 build. For FWU3 address is defined by the build process.
#define PROD_DATA_TRID_ADDR (UL_INVALID)
#endif

/*
Local functions
*/
#ifdef LSW_DBPL_MINIMAL_IMPLEMENTATION
/**This function DBPL_ucGetModuleIndex(uchar ucMemoryModule) replaces the existing by an empty function for the minimal implementation.*/
uchar DBPL_ucGetModuleIndex(uchar ucMemoryModule)
{
   return 0U;
}
#endif

/**
 Example of a function for reading e.g. memory module 0.

This function should be adapted, depending on the application.
By data length of e.g. 2 consideration must be taken, whether data consistentcy is guaranteed;
interrupts may have to be disabled for the time it takes to read the two bytes.

\param ucMemoryModule
 Possibly redundant, depending on whether other modules are indirectly addressable over this module.\n
\b type    :   uchar\n
\b range   :   0..255\n

\param uiAddress
The address, corresponding to the wanted memory access\n
\b type   :    uint16

\param ucDataLen
How many bytes should be read.\n
\b type : uchar\n
\b range : 0..MaxDataLength (depending on the size of the smallest buffer, which must be regarded)

\param pucBuffer
 The data to return.\n
\b type   :   uchar *

\return
 state of request\n
\b type    :   BOOL\n
\b range   :   discrete values: DBPL_DONE (TRUE), DBPL_ONGOING (FALSE)
*/

static BOOL MOD_bRead(uchar ucMemoryModule, uint16 uiAddress, uchar ucDataLen, uchar *pucBuffer)
{
    /*
    * If the read process cannot be performed because of invalid parameters (e.g. memory address) or failure of hardware, set return value to DBPL_ONGOING and
    * DBPL_bInvalidReadProcess = TRUE;
    */
#ifdef _DATAMODEL_FAR    //Definition set by make process when addressing beyond 16 bit is used.
    uint32    ulAddress32;
    ulAddress32 = ((uint32)DBPL_uiPage<<INT_SIZE) + uiAddress;
    (void)memcpy((void *)pucBuffer, (void *)ulAddress32, ucDataLen);//lint !e923 !e920  both casts are ok
#else
    (void)memcpy((void *)pucBuffer, (void *)uiAddress, ucDataLen);//lint !e923 !e920  both casts are ok
#endif
    (void)ucMemoryModule;
    return DBPL_DONE;
}
/**
 Example of a function for writing e.g. memory module 0.

This function should be adapted, depending on the application.
By data length of e.g. 2 consideration must be taken, whether data consistentcy is guaranteed;
interrupts may have to be disabled for the time it takes to write the two bytes.

\param ucMemoryModule
Possibly redundant, depending if other modules are indirectly addressable over this module.\n
\b type       : uchar\n
\b range      : 0..255

\param uiAddress
 description: The address, corresponding to the wanted memory access\n
\b type       : uint16

\param ucDataLen
How many bytes should be written.\n
\b type       : uchar\n
\b range      : 0..MaxDataLength (depending on the size of the smallest buffer, which must be regarded)

\param pucBuffer
\b The data to write.\n
\b type       : uchar *

\return
 state of request\n
\b type       : BOOL\n
\b range      : discrete values: DBPL_DONE (TRUE), DBPL_ONGOING (FALSE)
*/

static BOOL MOD_bWrite(uchar ucMemoryModule, uint16 uiAddress, uchar ucDataLen, uchar *pucBuffer)
{
    /*
    * If the write process cannot be performed because of invalid parameters (e.g. memory address) or failure of hardware, set return value to DBPL_ONGOING and
    * DBPL_bInvalidWriteProcess = TRUE;
    */
    (void)ucMemoryModule;
#ifdef _DATAMODEL_FAR    //Definition set by make process when addressing beyond 16 bit is used.
    uint32    ulAddress32;
    ulAddress32 = ((uint32)DBPL_uiPage<<INT_SIZE) + uiAddress;
    (void)memcpy((void *)ulAddress32, (void *)pucBuffer, ucDataLen);//lint !e923 !e920  both casts are ok
#else
    (void)memcpy((void *)uiAddress, (void *)pucBuffer, ucDataLen);//lint !e923 !e920  both casts are ok
#endif
    return DBPL_DONE;
}//lint !e818   pucBuffer cannot be const as service interface is fixed


/** Example of a function for getting the address of the identification string of e.g. memory module 0.

This function should be adapted, depending on the application.

\param ucMemoryModule
Possibly redundant, depending if other modules are indirectly addressable over this module.\n
\b type       : uchar\n
\b range      : 0..255

\param puiIdAddress
The data to write.\n
\b type       : uint16 *

\return
state of request\n
\b type       : BOOL\n
\b range      : discrete values: DBPL_DONE (TRUE), DBPL_ONGOING (FALSE)
*/

static BOOL MOD_bGetId(uchar ucMemoryModule, uint16 *puiIdAddress)
{
#ifdef _DATAMODEL_FAR
   //lint -e{840} Info 840: Use of nul character in a string literal --> is the intention!
   static const uchar aucDummyId32[] =
   NODE_ID
   "\0"
   LD_STRINGIZE(DBUS2_NODE_DATE)
   "\0"
   "32-bit-MemoryModule\0"
   DBUS2_ID;
   
   uint32_t ulIdAddress32 = (uint32)aucDummyId32; //lint !e923 Cast is tested to work correctly
   DBPL_uiPage   = (uint16)(ulIdAddress32>>INT_SIZE); //Save high integer part of address.
   *puiIdAddress = (uint16)ulIdAddress32;// &DummyId32[0];
   
   (void)ucMemoryModule;
   
   /* Msg(2:0506) Dereferencing pointer value that is possibly NULL.
      Code is tested to work correctly
   */
#else
   //lint -e{840} Info 840: Use of nul character in a string literal --> is the intention!
   static const uchar aucDummyId[] =
   NODE_ID
   "\0"
   LD_STRINGIZE(DBUS2_NODE_DATE)
   "\0"
   "16-bit-MemoryModule\0"
   DBUS2_ID;

   *puiIdAddress = (uint16)((uint32_t)aucDummyId);//lint !e923 Cast is tested to work correctly

   (void)ucMemoryModule;

   /* Msg(2:0506) Dereferencing pointer value that is possibly NULL.
      Code is tested to work correctly
   */
#endif
   return DBPL_DONE;
}

//#define DBPL_CHANGE_2_S_TIMER_BASE
/** To be defined to change the 2s timer base. */
#ifdef DBPL_CHANGE_2_S_TIMER_BASE
#define DBPL_MAX_VALUE_FOR_1S   128U    //Maximum representable value, when using 1 s timer instead of 2 is only half of one byte.
uchar DBPL_ucPossiblyChangeTimerBase(uchar ucBase)
{
   /*  Change timer base 2s to 1s (return 0, which represents eternity, if value cannot be represented by 1s-timer)
   */
   if (ucBase < DBPL_MAX_VALUE_FOR_1S)
   {
      return (ucBase*2);
   }
   else
   {
      return 0; //This represents eternity (= until power off).
   }
}
#else
uchar DBPL_ucPossiblyChangeTimerBase(uchar ucBase)
{
   return ucBase;
}
#endif
/*
The following functions are called directly by the library source. These functions constitute an interface to the application.

Please modify (several functions are on default empty), when needed.
*/
void DBPL_vGoOffline(void)
{
   /* Possibly notify the application, before entering offline mode. Possibly shut down actuators ... */
}

/*
Interface to bootloader. Please inform bootloader that a large timeout (e.g. 5 s) should be used after reset, as a GoOffline message was received, hence a flashing procedure is expected.
*/
void DBPL_vEnterBootloaderMode(void)
{
#ifdef __BOOTLOADER_INCLUDED
    BMDAT_setMagicPattern(BMDAT_startBlAfterReset);
#endif
}
/* Timer functions */
BOOL DBPL_bIsTimerDown(void)
{
   if ( (DBPL_tResetTimer == 0U) && (DBPL_tOfflineDelayTimer == 0U) )
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

void DBPL_vSetResetTimer(uchar ucValue)
{
#if defined (LRO_BOOTLOADER_MERGE)
   BMDAT_setBootModule(MAL_BP2_LOADER_ID);
#endif
   DBPL_tResetTimer = ucValue;
}

void DBPL_vSetOfflineTimer(uchar ucValue)
{
   DBPL_tOfflineDelayTimer = ucValue;
}

/*lint -e{715,818} parameter no used if update disabled */
void DBPL_vHSI_DataIndication(void * data, uint32 data_length)
{
#ifdef DBUS2_UPDATE_HSI
    HSI_DataIndication(data, data_length);
#endif
}

BOOL DBPL_bIsNodeToBeWokenUp(uchar nodeAddress)
{
    BOOL ret = TRUE;
    (void)nodeAddress;
    /*
    Call, if you have woken up all nodes,
    or if your project specific timeout has occured:
    DBPL_vUnNotifyWakeupBreakSent();
    */
    return ret;
}

void DBPL_vWakeupSentResponseReceived(uchar nodeAddress)
{
    (void)nodeAddress;
}

uchar DBPL_ucGetOwnAddressToInformOtherNodes(void)
{
    /*The node address must be correct and should contain a subsystem implemented by the user application.*/
    /*This standard implementation here always returns the lowest subsystem implemented by the user.*/
    for(uchar node = 0; node < UINT8_MAX; node++)
    {
        if((DLL_bIsCurrentNode(node) == TRUE) && (BAL_ucGetSubsystemIndex(node) != 0U))
        {
            return node;
        }
    }

    /*No subsystem apart from zero (Service Messages) implemented. Return subsystem zero.*/
    return ((uchar)(DLL_ucGetMainNodeAddress()<<NIBBLE_SIZE));
}

void DBPL_vSilentModeHasBeenEntered(void)
{

}

void DBPL_vPowerFailNotificationHasBeenReceived(void)
{
    /*Caution. This is called directly from interrupt handler of DBUS! Do not put too much logic here.*/
}

void DBPL_vPowerResurgeNotificationHasBeenReceived(uchar sender)
{
    (void)sender;
}

void DBPL_vNotifyNonDelivarablePowerMessage(uint16 msgId)
{
    (void)msgId;
}

struct DBPL_EcuUniqueIdReadResult DBPL_tGetUniqueIdReadResult(void)
{
    struct DBPL_EcuUniqueIdReadResult result = { .status = DBPL_EcuStatusUpdateModeActive, .uniqueIdLen = 0, .uniqueId = {0}};
#ifndef VARIANT_PROGRAMMER
    const uint8_t SDEF_FAR *pucTrid = (uint8_t SDEF_FAR *)PROD_DATA_TRID_ADDR;    //lint !e923 Cast is tested to work correctly

    result.status = DBPL_EcuStatusOK;
    result.uniqueIdLen = (uchar)sizeof(struct DBPL_TracingId);

    for(uchar i = 0; i < result.uniqueIdLen; i++)
    {
        result.uniqueId[i] = pucTrid[i];
    }
#endif
    return result;
}
