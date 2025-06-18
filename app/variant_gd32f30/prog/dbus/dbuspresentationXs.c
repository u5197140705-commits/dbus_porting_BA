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
#include "build_date.h"

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
#include "LibTypes.h"
#include "LibDefines.h"
#include "bustypes.h"
#include "dbuspresentation.h"
#include "dbuspresentation_ecu_types.h"
#include "dbuspresentation_production_types.h"
#include "bal.h"
#include "dbusdll.h"

#if defined (REMOTE_FIRMWARE_UPDATE) || defined (WIRED_FIRMWARE_UPDATE)
#include "firmware_update/BootManager/BootManagerSharedData.h"
#endif
#if defined (BTM_FIRMWARE_UPDATE)
#include "firmware_update/gecko_bl/gecko_bl_api.h"
#endif

#include "utility.h"    //UTI_NELEMENTS()

#ifdef DBUS2_UPDATE_HSI
#include "hsi_basic.h"
#endif

#ifdef DBAL_INCLUDED
#include "BshDBus2AppLayer_internal.h"
#endif

/* The functions, which are prototyped here, may be defined/prototyped in an external file, which is included in this excess template */
/*lint -esym(818,pucBuffer) parameter cannot by declared constant */
static bool MOD_bRead(uint8_t ucMemoryModule, uint16_t uiAddress, uint8_t ucDataLen, uint8_t *pucBuffer);
static bool MOD_bWrite(uint8_t ucMemoryModule, uint16_t uiAddress, uint8_t ucDataLen, uint8_t *pucBuffer);
static bool MOD_bGetId(uint8_t ucMemoryModule, uint16_t *puiIdAddress);
#ifndef LSW_DBPL_MINIMAL_IMPLEMENTATION
/**
 ModuleDict (data type: TbusModuleTable) is a table where the different read/write/id-functions are found, depending on the current memory module.
    NOTE: DO NOT REMOVE default memory module 0, it is used in cases when invalid memory module or no memory module is provided by user. 
*/
const TbusModuleTable DBPL_tModuleDict[] = {
/*  Module, ReadFunction, WriteFunction, IdFunction, Blocksize */
       {0, MOD_bRead, MOD_bWrite, MOD_bGetId, 1}      //!<Access to memory module 0 (mandatory, do not remove, access functions can be adapted if needed)
      ,{1, MOD_bRead, MOD_bWrite, MOD_bGetId, 16}     //!<Access to memory module 1
//      ,{2, MOD_bRead, MOD_bWrite, MOD_bGetId, 1}    //!<Access to memory module 2
//      ,{9, MOD_bRead, MOD_bWrite, MOD_bGetId, 32}   //!<Access to memory module 9
};
#endif
const uint8_t DBPL_ucNumberOfElements = (uint8_t)UTI_NELEMENTS(DBPL_tModuleDict); //!< A constant containing the number of elements in ModuleDict, i.e. how many memory modules are defined.

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


/*
Local functions
*/
#ifdef LSW_DBPL_MINIMAL_IMPLEMENTATION
/**This function DBPL_ucGetModuleIndex(uint8_t ucMemoryModule) replaces the existing by an empty function for the minimal implementation.*/
uint8_t DBPL_ucGetModuleIndex(uint8_t ucMemoryModule)
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
\b type    :   uint8_t\n
\b range   :   0..255\n

\param uiAddress
The address, corresponding to the wanted memory access\n
\b type   :    uint16_t

\param ucDataLen
How many bytes should be read.\n
\b type : uint8_t\n
\b range : 0..MaxDataLength (depending on the size of the smallest buffer, which must be regarded)

\param pucBuffer
 The data to return.\n
\b type   :   uint8_t *

\return
 state of request\n
\b type    :   bool\n
\b range   :   discrete values: DBPL_DONE (true), DBPL_ONGOING (false)
*/

static bool MOD_bRead(uint8_t ucMemoryModule, uint16_t uiAddress, uint8_t ucDataLen, uint8_t *pucBuffer)
{
    /*
    * If the read process cannot be performed because of invalid parameters (e.g. memory address) or failure of hardware, set return value to DBPL_ONGOING and
    * DBPL_bInvalidReadProcess = true;
    */
    uint32_t    ulAddress32;
    ulAddress32 = ((uint32_t)DBPL_uiPage<<INT_SIZE) + uiAddress;
    (void)memcpy((void *)pucBuffer, (void *)ulAddress32, ucDataLen);//lint !e923 !e920  both casts are ok
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
\b type       : uint8_t\n
\b range      : 0..255

\param uiAddress
 description: The address, corresponding to the wanted memory access\n
\b type       : uint16_t

\param ucDataLen
How many bytes should be written.\n
\b type       : uint8_t\n
\b range      : 0..MaxDataLength (depending on the size of the smallest buffer, which must be regarded)

\param pucBuffer
\b The data to write.\n
\b type       : uint8_t *

\return
 state of request\n
\b type       : bool\n
\b range      : discrete values: DBPL_DONE (true), DBPL_ONGOING (false)
*/

static bool MOD_bWrite(uint8_t ucMemoryModule, uint16_t uiAddress, uint8_t ucDataLen, uint8_t *pucBuffer)
{
    /*
    * If the write process cannot be performed because of invalid parameters (e.g. memory address) or failure of hardware, set return value to DBPL_ONGOING and
    * DBPL_bInvalidWriteProcess = true;
    */
    (void)ucMemoryModule;
    uint32_t    ulAddress32;
    ulAddress32 = ((uint32_t)DBPL_uiPage<<INT_SIZE) + uiAddress;
    (void)memcpy((void *)ulAddress32, (void *)pucBuffer, ucDataLen);//lint !e923 !e920  both casts are ok
    return DBPL_DONE;
}//lint !e818   pucBuffer cannot be const as service interface is fixed


/** Example of a function for getting the address of the identification string of e.g. memory module 0.

This function should be adapted, depending on the application.

\param ucMemoryModule
Possibly redundant, depending if other modules are indirectly addressable over this module.\n
\b type       : uint8_t\n
\b range      : 0..255

\param puiIdAddress
The data to write.\n
\b type       : uint16_t *

\return
state of request\n
\b type       : bool\n
\b range      : discrete values: DBPL_DONE (true), DBPL_ONGOING (false)
*/

static bool MOD_bGetId(uint8_t ucMemoryModule, uint16_t *puiIdAddress)
{
   //lint -e{840} Info 840: Use of nul character in a string literal --> is the intention!
   static const uint8_t aucDummyId32[] =
   NODE_ID
   "\0"
   LD_STRINGIZE(DBUS2_NODE_DATE)
   "\0"
   "32-bit-MemoryModule\0"
   DBUS2_ID;
   
   uint32_t ulIdAddress32 = (uint32_t)aucDummyId32; //lint !e923 Cast is tested to work correctly
   DBPL_uiPage   = (uint16_t)(ulIdAddress32>>INT_SIZE); //Save high integer part of address.
   *puiIdAddress = (uint16_t)ulIdAddress32;// &DummyId32[0];
   
   (void)ucMemoryModule;
   
   /* Msg(2:0506) Dereferencing pointer value that is possibly NULL.
      Code is tested to work correctly
   */
   return DBPL_DONE;
}

//#define DBPL_CHANGE_2_S_TIMER_BASE
/** To be defined to change the 2s timer base. */
#ifdef DBPL_CHANGE_2_S_TIMER_BASE
#define DBPL_MAX_VALUE_FOR_1S   128U    //Maximum representable value, when using 1 s timer instead of 2 is only half of one byte.
uint8_t DBPL_ucPossiblyChangeTimerBase(uint8_t ucBase)
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
uint8_t DBPL_ucPossiblyChangeTimerBase(uint8_t ucBase)
{
   return ucBase;
}
#endif
/*
The following functions are called directly by the library source. These functions constitute an interface to the application.
*/
void DBPL_vGoOffline(void)
{
   /* Possibly notify the application before entering offline mode. Possibly shut down actuators ... */
}

void DBPL_vEnterBootloaderMode(void)
{
#if defined (WIRED_FIRMWARE_UPDATE)
    BMDAT_setMagicPattern(BMDAT_startBlAfterReset);
#elif defined (REMOTE_FIRMWARE_UPDATE) && !defined (FWU3_LITE)
    BMDAT_setBootModule(MAL_BP2_LOADER_ID);
#elif defined (BTM_FIRMWARE_UPDATE)
    GBTL_bootGromLoaderAfterReset();
#endif

    /* Possibly notify the application before entering firmware update mode. Possibly shut down actuators ... */
}

/*lint -e{715,818} parameter no used if update disabled */
void DBPL_vHSI_DataIndication(void* const data, uint32_t data_length)
{
#ifdef DBUS2_UPDATE_HSI
    HSI_DataIndication(data, data_length);
#endif
}

bool DBPL_bIsNodeToBeWokenUp(uint8_t nodeAddress)
{
#ifdef DBAL_INCLUDED
    return DBAL_isNodeToBeWokenUp(nodeAddress);
#else
    bool ret = true;
    (void)nodeAddress;
    /*
    Call, if you have woken up all nodes,
    or if your project specific timeout has occured:
    DBPL_vUnNotifyWakeupBreakSent();
    */
    return ret;
#endif
}

void DBPL_vWakeupSentResponseReceived(uint8_t nodeAddress)
{
    (void)nodeAddress;
}

uint8_t DBPL_ucGetOwnAddressToInformOtherNodes(void)
{
    /*The node address must be correct and should contain a subsystem implemented by the user application.*/
    /*This standard implementation here always returns the lowest subsystem implemented by the user.*/
    for(uint8_t node = 0; node < UINT8_MAX; node++)
    {
        if((DLL_bIsCurrentNode(node) == true) && (BAL_ucGetSubsystemIndex(node) != 0U))/*lint !e931 no relevant side effect 931 already removed form MISRA 13.2 for lintplus 1.4 BETA */
        {
            return node;
        }
    }

    /*No subsystem apart from zero (Service Messages) implemented. Return subsystem zero.*/
    return ((uint8_t)(DLL_ucGetMainNodeAddress()<<NIBBLE_SIZE));
}

void DBPL_vSilentModeHasBeenEntered(void)
{

}

void DBPL_vPowerFailNotificationHasBeenReceived(void)
{
    /*Caution. This is called directly from interrupt handler of DBUS! Do not put too much logic here.*/
}

void DBPL_vPowerResurgeNotificationHasBeenReceived(uint8_t sender)
{
    (void)sender;
}

void DBPL_vNotifyNonDeliverablePowerMessage(uint16_t msgId)
{
    (void)msgId;
}
