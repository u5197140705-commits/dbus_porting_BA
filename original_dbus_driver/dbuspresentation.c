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
*   COMP_ABBREV      DBPL
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief Presentation layer for the D-Bus-2
 *     This module handles the predefined messages in the dbuspresentation.c + .h and
 *     includes definitions for software modules, as well as the forwarding mechanism
 *     of not-predefined-messages (e.g. nmt message handler), in the excess template: dbuspresentationXS.c
 *
 *     An incoming message will be picked up by the presentation layer. If it is a predefined message,
 *     the presentation layer will handle this message. Else the message will be distributed to the
 *     message handler, which is defined in the excess template.
 *     
 *****************************************************************************************************************/
/*lint -save -e9059 "C comment contains C++ comment" */
#ifdef DOXY_DEVELOPERS_DOC
//Doxygen documentation of compiler switches
/**
Compiler switch, which is defined in the make file for controllers, which need their LittleEndian representation of integers to be adapted to the D-Bus-2 used format: BigEndian.

This switch should be set for all LittleEndian controllers (e.g. Renesas) in order for the message identifier to be transmitted in BigEndian format on D-Bus-2. Likewise it is important for LittleEndian controllers to adapt the received BigEndian message identifier to LittleEndian format for correct internal representation.
*/
#define __LITTLE_ENDIAN_ADAPTATION
/**
Compiler switch, which is defined for Update Service Messages without HSI.
*/
#define DBUS2_UPDATE
/**
Compiler switch, which is defined for Update Service Messages with HSI.
*/
#define DBUS2_UPDATE_HSI
/**
Compiler switch, which means, that production messages are not only in the programmer.
 */
#define DBUS2_PROD_MSG_IN_APP
#endif

#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "LibDefines.h"
#include "bustypes.h"
#include "dbusdll.h"
#include "dbuspresentation.h"       
#if defined(MCAL_MSUP_INCLUDED)
#include "mcal/msup.h" //Needed for reset execution
#else
#include "hsup.h" //Needed for reset execution
#endif //MCAL_MSUP_INCLUDED
#include "utility.h"    //Needed for UTI_NELEMENTS()
#include "bal.h"        //Needed for distribution of received messages
#include "dbusmapping.h" //Needed for DBM_UART_vDisableRxEvent()/DBM_UART_vEnableRxEvent()
#include "dbuspresentation_update.h"
#include "dbuspresentation_production.h"
#include "dbus_lock.h"
#include "dbus2_hsi_adaptor.h"
#include "timer/system_timer.h"
#include "dbus_rtos_interface.h"
#ifdef RTOS_DBUS_EVENTDRIVEN
#include "dbuscan_dbus.h"
#endif

/*lint -e818 Pointer parameter ... could be declared: not possible for service function */
#ifndef VARIANT_PROGRAMMER
//lint -esym( 766,dbuspresentation_subsys0rxXS.h) "header not used..., used actually in some cases"
#include "dbuspresentation_subsys0rxXS.h"
#endif

#define DBPL_SIZE_OF_DATA_BUFFER_IN_T_BUS_MESSAGE  (DLL_TRANSMIT_BUFFER_DATA_LENGTH) //!< Length of DBPL transmit buffer

/* Message definitions */
#define MSG_READ_REQUEST                            0xF000U //!< Read Request (mandatory msg).
#define MSG_READ_RESPONSE                           0xF100U //!< Response to Read Request (mandatory msg).
#define MSG_WRITE_REQUEST                           0xF200U //!< Write Request (optional msg).
#define MSG_READ_REQUEST32                          0xF001U //!< 32 bit Read Request (optional msg).
#define MSG_READ_RESPONSE32                         0xF101U //!< 32 bit Response to Read Request (optional msg).
#define MSG_WRITE_REQUEST32                         0xF201U //!< 32 bit Write Request (optional msg).
#define MSG_IDENTITY_REQUEST                        0xFF00U //!< Identity Request (mandatory msg).
#define MSG_IDENTITY_RESPONSE                       0xFE00U //!< Response to Identity Request: Address of the first element in the corresponding identity string (mandatory msg).
#define MSG_GO_OFFLINE                              0xFDFFU //!< Request to go offline, for all but one node (optional msg).
#define MSG_RESET_EXECUTE                           0xFD00U //!< MSG_RESET_EXECUTE; Request to execute reset (optional msg).
#define MSG_SET_MEMORY_MODULE                       0xF300U //!< Set memory module (optional msg - default = 0). If a requested memory module does not exist, the current memory module will be set to 0. This can be tested by requesting the ID (MSG_IDENTITY_REQUEST) - instead of the non-existing memory module 0 will be returned.
#define MSG_WAKEUP_SENT_REQUEST                     0xF306U
#define MSG_WAKEUP_SENT_RESPONSE                    0xF307U
#define MSG_ECU_UNIQUE_ID_READ_REQ                  0xF308U
#define MSG_ECU_UNIQUE_ID_READ_RES                  0xF309U
#define MSG_POWER_FAIL                              0xF310U
#define MSG_POWER_RESURGE                           0xF311U
#if ((defined DBUS2_UPDATE_HSI) || (defined DBUS2_PROD_MSG_IN_APP))
    #define MSG_WRITE_ECU_CONFIG_HW_REQUEST         0xF312U
    #define MSG_WRITE_ECU_CONFIG_HW_RESPONSE        0xF313U
    #define MSG_WRITE_TRACING_ID_REQUEST            0xF314U
    #define MSG_WRITE_TRACING_ID_RESPONSE           0xF315U
    #define MSG_WRITE_PROD_TIME_REQUEST             0xF316U
    #if defined (DBUS2_APPLIANCE_MSG_IN_APP)
        #define MSG_WRITE_APPLIANCE_DATA_REQUEST    0xF31CU
        #define MSG_WRITE_APPLIANCE_DATA_RESPONSE   0xF31DU
        #define MSG_READ_APPLIANCE_DATA_REQUEST     0xF31EU
        #define MSG_READ_APPLIANCE_DATA_RESPONSE    0xF31FU
    #endif
#endif /* ((defined DBUS2_UPDATE_HSI) || (defined DBUS2_PROD_MSG_IN_APP)) */
#define MSG_PROD_TIME_RESPONSE                      0xF317U
#define MSG_READ_PROD_TIME_REQUEST                  0xF318U
#define MSG_WRITE_TESTST_REQUEST                    0xF319U
#define MSG_TESTST_REPAIRCNT_RESPONSE               0xF31AU
#define MSG_READ_TESTST_REPAIRCNT_REQUEST           0xF31BU
#define MSG_POWER_MGMT_WAKEUP                       0xF320U
#define MSG_POWER_MGMT_WAKEUP_COMPLETE              0xF322U
#define MSG_POWER_MGMT_NODE_RESET                   0xF324U
#define MSG_FACTORY_RESET                           0xF326U
#define MSG_SILENT_MODE_TRANSITION_REQUEST          0xF501U
#define MSG_SILENT_MODE_TRANSITION_REQUEST_BROADCAST    0xF502U
#define MSG_SILENT_MODE_TRANSITION_RESPONSE         0xF503U
#define MSG_UPDATE_MODE_VERIFY_REQUEST              0xF504U
#define MSG_UPDATE_MODE_VERIFY_RESPONSE             0xF505U
#define MSG_UPDATE_MODE_TRANSITION_REQUEST          0xF506U
#define MSG_BAUDRATE_VERIFY_REQUEST                 0xF507U
#define MSG_BAUDRATE_VERIFY_RESPONSE                0xF508U
#define MSG_BAUDRATE_TRANSITION_REQUEST             0xF509U
#define MSG_BAUDRATE_TRIGGER_REQUEST                0xF50AU
#define MSG_RESET_TRIGGER_REQUEST                   0xF50BU
#ifdef DBUS2_UPDATE_HSI
    #define MSG_HSI_PROTOCOL_REQUEST                0xF50CU
    #define MSG_HSI_PROTOCOL_RESPONSE               0xF50DU
#endif
#define MSG_RETURN_FROM_SILENT_MODE_REQUEST         0xF50EU
#define MSG_RETURN_FROM_SILENT_MODE_RESPONSE        0xF50FU
#define MSG_ECU_CONFIG_READ_REQUEST                 0xF510U
#define MSG_ECU_CONFIG_READ_RESPONSE                0xF511U
#define MSG_ECU_SW_SUBMODULE_READ_REQUEST           0xF512U
#define MSG_ECU_SW_SUBMODULE_READ_RESPONSE          0xF513U

#define DBPL_POWER_MSG_SEND_RETRIES       5U
#define DBPL_UPDATE_MSG_SEND_RETRIES      3U
#define DBPL_PRODUCTION_MSG_SEND_RETRIES  3U
#if defined (DBUS2_APPLIANCE_MSG_IN_APP)
    #define DBPL_APPLIANCE_DATA_MSG_SEND_RETRIES  3U
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

/* Function prototypes needed for transmit and receive tables */
static void DBPL_vReadRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vWriteRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vReadRequest32(uint8_t ucDataLen, uint8_t *pucData);      //Also to be used for 16-bit addressing, when 32-bit is needed
static void DBPL_vWriteRequest32(uint8_t ucDataLen, uint8_t *pucData);     //Also to be used for 16-bit addressing, when 32-bit is needed
static void DBPL_vIdentityRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vIdentityRequest32(uint8_t ucDataLen, uint8_t *pucData); //This function is to be used if 32 bit ID-String address is (potentially) needed, otherwise the "normal" function handles 16 bit addresses in a cheaper way (DBPL_uiPage variable is not needed, as well as a few lines of ROM).
static void DBPL_vGoOfflineService(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vResetExecute(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vSetMemoryModule(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vWakeupSentRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vWakeupSentResponse(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vPowerResurgeMsg(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vEcuUniqueIdReadRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vFactoryReset(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vSilentModeTransitionRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vSilentModeTransitionRequestBroadcast(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vUpdateModeVerifyRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vUpdateModeTransitionRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vBaudrateVerifyRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vBaudrateTransitionRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vBaudrateTriggerRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vResetTriggerRequest(uint8_t ucDataLen, uint8_t *pucData);
#ifdef DBUS2_UPDATE_HSI
static void DBPL_vHsiProtocolRequest(uint8_t ucDataLen, uint8_t *pucData);
#endif
static void DBPL_vReturnFromSilentModeRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vEcuConfigReadRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vEcuSwSubmoduleReadRequest(uint8_t ucDataLen, uint8_t *pucData);
#if ((defined DBUS2_UPDATE_HSI) || (defined DBUS2_PROD_MSG_IN_APP))
static void DBPL_vWriteEcuConfigHwRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vWriteTracingIdRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vWriteProductionTimeRequest(uint8_t ucDataLen, uint8_t *pucData);
#if defined (DBUS2_APPLIANCE_MSG_IN_APP)
static void DBPL_vWriteApplianceDataRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vReadApplianceDataRequest(uint8_t ucDataLen, uint8_t *pucData);
#endif
#endif /* ((defined DBUS2_UPDATE_HSI) || (defined DBUS2_PROD_MSG_IN_APP)) */
static void DBPL_vReadProductionTimeRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vWriteTestStateRequest(uint8_t ucDataLen, uint8_t *pucData);
static void DBPL_vReadTestStateRepairCntRequest(uint8_t ucDataLen, uint8_t *pucData);

static void DBPL_vSendResponse(uint8_t ucDataLen, uint8_t *pucData);
static bool DBPL_bIsSendingPowerMgmtMsg(void);

static void DBPL_initTimers(void);
static void DBPL_setResetTimer(uint16_t resetDelay);
static void DBPL_setOfflineTimer(uint16_t offlineTime);
static void DBPL_setBaudRateTimer(uint32_t baudRateTime);
#ifdef DBUS2_UPDATE_HSI
static uint16_t DBPL_getBaudRateTimer(void);
#endif
static void DBPL_setResetTriggerTimer(uint16_t resetTriggerTime);

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
// callback functions attached to SW timers 
static int32_t DBPL_baudRateTimerElapsed(void *obj, uint32_t flags, int32_t eventData);
static int32_t DBPL_resetTriggerTimerElapsed(void *obj, uint32_t flags, int32_t eventData);
#endif /* DBUS2_UPDATE || DBUS2_UPDATE_HSI */
static int32_t DBPL_offlineTimerElapsed(void *obj, uint32_t flags, int32_t eventData);

/*! \page SubsysMan Mandatory service messages
* 
* \section secMan Mandatory service messages
* 
* Mandatory service messages for \b reception:
* - Read Request
* - Identity Request
*
* Mandatory service messages for \b transmission:
* - Read Response
* - Identity Response
*/

/*! Subsystem: Mandatory service messages (to be received).
*
* This table contains mandatory message definitions for service messages, which may be received (messages known to the presentation layer). Please note that the \b last entry in this table \b MUST have \b MSB set in the message length element (this marks the end of the table).
*
* Specification changed Spring 2005: Write Request removed from mandatory service messages, as there is no obligation to use write request!
*/
const TbusReceiveObject DBPL_tReceiveMandatoryServiceMessages[] = {
#ifdef DBUS2_SUBSYSTEM_ZERO_RECEIVE
        DBUS2_SUBSYSTEM_ZERO_RECEIVE,
#endif

       {{0x00U,0x00U,MSG_READ_REQUEST}, DBPL_vReadRequest},
       {{MSB_BYTE,0x00U,MSG_IDENTITY_REQUEST}, DBPL_vIdentityRequest} //Last element (marked with MSB in MesgLen set).
};
/*! \page SubsysOpt Mandatory and optional service messages
* 
* \section secManOpt Mandatory and optional service messages
* 
* Mandatory and optional service messages for \b reception:
* - Read Request
* - Write Request
* - Identity Request
* - Reset Execute
* - Go Offline
* - Set Memory Module
* - Read Request 32 bit addressing
* - Write Request 32 bit addressing
* - Silent Mode Transition Request
* - Update Mode Verify Request
* - Update Mode Transition Request
* - Baudrate Verify Request
* - Baudrate Transition Request
* - Baudrate Trigger
* - Reset Trigger Request
* - Wakeup Sent Request
* - Ecu Config Read Request
* - Ecu Unique Id Request
*
*
* Mandatory and optional service messages for \b transmission:
* - Read Response
* - Identity Response
* - Read Response 32 bit addressing
* - Update Mode Verify Response
* - Baudrate Verify Response
* - Wakeup Sent Response
* - Ecu Config Read Response
* - Ecu Unique Id Response
*
* \note Please note that 32 bit memory access is still using a 16 bit interface for reading and writing. The upper 16 bits of the address are transferred via the variable DBPL_uiPage.
*/
/*!  Subsystem: Mandatory and optional service messages (to be received)
*
* Table containing mandatory and optional message definitions for service messages, which may be received (messages known to the presentation layer). Please note that the last entry in this table MUST have MSB set in the message length element (this marks the end of the table).
*
* This table does not include DBPL_vReadRequest32 and DBPL_vWriteRequest32 (which are not implemented). If these functions are needed they must be implemented in the excess template and DBPL_tReceiveObjectServiceMessagesIncludingAllOptionalMessages must be used instead of this table.
*
*/
const TbusReceiveObject DBPL_tReceiveAll16BitServiceMessages[] = {
#ifdef DBUS2_SUBSYSTEM_ZERO_RECEIVE
        DBUS2_SUBSYSTEM_ZERO_RECEIVE,
#endif

       {{0x00U,0x00U,MSG_READ_REQUEST},                       DBPL_vReadRequest},
       {{0x00U,0x00U,MSG_WRITE_REQUEST},                      DBPL_vWriteRequest},      
       {{0x00U,0x00U,MSG_IDENTITY_REQUEST},                   DBPL_vIdentityRequest},
       {{0x00U,0x00U,MSG_RESET_EXECUTE},                      DBPL_vResetExecute},
       {{0x00U,0x00U,MSG_GO_OFFLINE},                         DBPL_vGoOfflineService},
       {{0x00U,0x00U,MSG_SET_MEMORY_MODULE},                  DBPL_vSetMemoryModule},
       {{0x00U,0x00U,MSG_WAKEUP_SENT_REQUEST},                DBPL_vWakeupSentRequest},
       {{0x00U,0x00U,MSG_WAKEUP_SENT_RESPONSE},               DBPL_vWakeupSentResponse},
       {{0x00U,0x00U,MSG_POWER_RESURGE},                      DBPL_vPowerResurgeMsg},
       {{0x00U,0x00U,MSG_WRITE_TESTST_REQUEST},               DBPL_vWriteTestStateRequest},
       {{MSB_BYTE,0x00U,MSG_READ_TESTST_REPAIRCNT_REQUEST},   DBPL_vReadTestStateRepairCntRequest} //Last element (marked with MSB in MesgLen set).
};
/*!  Subsystem: Mandatory and optional service messages (to be received)
*
* Table containing mandatory and optional message definitions for service messages, which may be received (messages known to the presentation layer). Please note that the last entry in this table MUST have MSB set in the message length element (this marks the end of the table).
*
* This table does include DBPL_vReadRequest32 and DBPL_vWriteRequest32. If these functions are not needed then DBPL_tReceiveAll16BitServiceMessages should be used instead of this table (this will save a few bytes of ROM).
*
*/

const TbusReceiveObject DBPL_tReceiveAllServiceMessages[] = {
#ifdef DBUS2_SUBSYSTEM_ZERO_RECEIVE
        DBUS2_SUBSYSTEM_ZERO_RECEIVE,
#endif
       {{0x00U,0x00U,MSG_READ_REQUEST},                       DBPL_vReadRequest},
       {{0x00U,0x00U,MSG_WRITE_REQUEST},                      DBPL_vWriteRequest},
       {{0x00U,0x00U,MSG_READ_REQUEST32},                     DBPL_vReadRequest32},
       {{0x00U,0x00U,MSG_WRITE_REQUEST32},                    DBPL_vWriteRequest32},
       {{0x00U,0x00U,MSG_IDENTITY_REQUEST},                   DBPL_vIdentityRequest32},
       {{0x00U,0x00U,MSG_RESET_EXECUTE},                      DBPL_vResetExecute},
       {{0x00U,0x00U,MSG_GO_OFFLINE},                         DBPL_vGoOfflineService},
       {{0x00U,0x00U,MSG_SET_MEMORY_MODULE},                  DBPL_vSetMemoryModule},
       {{0x00U,0x00U,MSG_WAKEUP_SENT_REQUEST},                DBPL_vWakeupSentRequest},
       {{0x00U,0x00U,MSG_WAKEUP_SENT_RESPONSE},               DBPL_vWakeupSentResponse},
       {{0x00U,0x00U,MSG_POWER_RESURGE},                      DBPL_vPowerResurgeMsg},
       {{0x00U,0x00U,MSG_FACTORY_RESET},                      DBPL_vFactoryReset},
       {{0x00U,0x00U,MSG_WRITE_TESTST_REQUEST},               DBPL_vWriteTestStateRequest},
       {{MSB_BYTE,0x00U,MSG_READ_TESTST_REPAIRCNT_REQUEST},   DBPL_vReadTestStateRepairCntRequest} //Last element (marked with MSB in MesgLen set).
};

/*Axivion Next Line MisraC2012-1.1 : Number of significant initial characters for external identifier not a problem with our tools */
const TbusReceiveObject DBPL_tReceiveAllServiceMessagesUpdate[] = {
#ifdef DBUS2_SUBSYSTEM_ZERO_RECEIVE
        DBUS2_SUBSYSTEM_ZERO_RECEIVE,
#endif

       {{0x00U,0x00U,MSG_READ_REQUEST},           DBPL_vReadRequest},
       {{0x00U,0x00U,MSG_WRITE_REQUEST},          DBPL_vWriteRequest},
       {{0x00U,0x00U,MSG_READ_REQUEST32},         DBPL_vReadRequest32},
       {{0x00U,0x00U,MSG_WRITE_REQUEST32},        DBPL_vWriteRequest32},
       {{0x00U,0x00U,MSG_IDENTITY_REQUEST},       DBPL_vIdentityRequest32},
       {{0x00U,0x00U,MSG_RESET_EXECUTE},          DBPL_vResetExecute},
       {{0x00U,0x00U,MSG_GO_OFFLINE},             DBPL_vGoOfflineService},
       {{0x00U,0x00U,MSG_SET_MEMORY_MODULE},      DBPL_vSetMemoryModule},
       {{0x00U,0x00U,MSG_WAKEUP_SENT_REQUEST},    DBPL_vWakeupSentRequest},
       {{0x00U,0x00U,MSG_WAKEUP_SENT_RESPONSE},   DBPL_vWakeupSentResponse},
       {{0x00U,0x00U,MSG_POWER_RESURGE},          DBPL_vPowerResurgeMsg},
       {{0x00U,0x00U,MSG_ECU_UNIQUE_ID_READ_REQ}, DBPL_vEcuUniqueIdReadRequest},
       {{0x00U,0x00U,MSG_FACTORY_RESET},          DBPL_vFactoryReset},
       {{0x00U,0x00U,MSG_SILENT_MODE_TRANSITION_REQUEST},           DBPL_vSilentModeTransitionRequest},
       {{0x00U,0x00U,MSG_SILENT_MODE_TRANSITION_REQUEST_BROADCAST}, DBPL_vSilentModeTransitionRequestBroadcast},
       {{0x00U,0x00U,MSG_UPDATE_MODE_VERIFY_REQUEST},               DBPL_vUpdateModeVerifyRequest},
       {{0x00U,0x00U,MSG_UPDATE_MODE_TRANSITION_REQUEST},           DBPL_vUpdateModeTransitionRequest},
       {{0x00U,0x00U,MSG_BAUDRATE_VERIFY_REQUEST},                  DBPL_vBaudrateVerifyRequest},
       {{0x00U,0x00U,MSG_BAUDRATE_TRANSITION_REQUEST},              DBPL_vBaudrateTransitionRequest},
       {{0x00U,0x00U,MSG_BAUDRATE_TRIGGER_REQUEST},                 DBPL_vBaudrateTriggerRequest},
       {{0x00U,0x00U,MSG_RESET_TRIGGER_REQUEST},                    DBPL_vResetTriggerRequest},
       {{0x00U,0x00U,MSG_RETURN_FROM_SILENT_MODE_REQUEST},          DBPL_vReturnFromSilentModeRequest},
       {{0x00U,0x00U,MSG_ECU_CONFIG_READ_REQUEST},                  DBPL_vEcuConfigReadRequest},
       {{0x00U,0x00U,MSG_ECU_SW_SUBMODULE_READ_REQUEST},            DBPL_vEcuSwSubmoduleReadRequest},
       {{0x00U,0x00U,MSG_READ_PROD_TIME_REQUEST},                   DBPL_vReadProductionTimeRequest},
#ifdef DBUS2_PROD_MSG_IN_APP
       {{0x00U,0x00U,MSG_WRITE_ECU_CONFIG_HW_REQUEST},        DBPL_vWriteEcuConfigHwRequest},
       {{0x00U,0x00U,MSG_WRITE_TRACING_ID_REQUEST},           DBPL_vWriteTracingIdRequest},
       {{0x00U,0x00U,MSG_WRITE_PROD_TIME_REQUEST},            DBPL_vWriteProductionTimeRequest},
#if defined (DBUS2_APPLIANCE_MSG_IN_APP)
       {{0x00U,0x00U,MSG_WRITE_APPLIANCE_DATA_REQUEST},       DBPL_vWriteApplianceDataRequest},
       {{0x00U,0x00U,MSG_READ_APPLIANCE_DATA_REQUEST},        DBPL_vReadApplianceDataRequest},
#endif
#endif /* DBUS2_PROD_MSG_IN_APP */
       {{0x00U,0x00U,MSG_WRITE_TESTST_REQUEST},                     DBPL_vWriteTestStateRequest},
       {{MSB_BYTE,0x00U,MSG_READ_TESTST_REPAIRCNT_REQUEST},         DBPL_vReadTestStateRepairCntRequest} //Last element (marked with MSB in MesgLen set).
};

#ifdef DBUS2_UPDATE_HSI
/*Axivion Next Line MisraC2012-1.1 : Number of significant initial characters for external identifier not a problem with our tools */
const TbusReceiveObject DBPL_tReceiveAllServiceMessagesUpdateHsi[] = {
       {{0x00U,0x00U,MSG_IDENTITY_REQUEST},       DBPL_vIdentityRequest32},
       {{0x00U,0x00U,MSG_READ_REQUEST32},         DBPL_vReadRequest32},
       {{0x00U,0x00U,MSG_GO_OFFLINE},             DBPL_vGoOfflineService},
       {{0x00U,0x00U,MSG_ECU_UNIQUE_ID_READ_REQ}, DBPL_vEcuUniqueIdReadRequest},
       {{0x00U,0x00U,MSG_SILENT_MODE_TRANSITION_REQUEST},           DBPL_vSilentModeTransitionRequest},
       {{0x00U,0x00U,MSG_SILENT_MODE_TRANSITION_REQUEST_BROADCAST}, DBPL_vSilentModeTransitionRequestBroadcast},
       {{0x00U,0x00U,MSG_UPDATE_MODE_VERIFY_REQUEST},               DBPL_vUpdateModeVerifyRequest},
       {{0x00U,0x00U,MSG_UPDATE_MODE_TRANSITION_REQUEST},           DBPL_vUpdateModeTransitionRequest},
       {{0x00U,0x00U,MSG_BAUDRATE_VERIFY_REQUEST},                  DBPL_vBaudrateVerifyRequest},
       {{0x00U,0x00U,MSG_BAUDRATE_TRANSITION_REQUEST},              DBPL_vBaudrateTransitionRequest},
       {{0x00U,0x00U,MSG_BAUDRATE_TRIGGER_REQUEST},                 DBPL_vBaudrateTriggerRequest},
       {{0x00U,0x00U,MSG_RESET_TRIGGER_REQUEST},                    DBPL_vResetTriggerRequest},
       {{0x00U,0x00U,MSG_HSI_PROTOCOL_REQUEST},                     DBPL_vHsiProtocolRequest},
       {{0x00U,0x00U,MSG_ECU_CONFIG_READ_REQUEST},                  DBPL_vEcuConfigReadRequest},
       {{0x00U,0x00U,MSG_ECU_SW_SUBMODULE_READ_REQUEST},            DBPL_vEcuSwSubmoduleReadRequest},
       {{0x00U,0x00U,MSG_WRITE_ECU_CONFIG_HW_REQUEST},              DBPL_vWriteEcuConfigHwRequest},
       {{0x00U,0x00U,MSG_WRITE_TRACING_ID_REQUEST},                 DBPL_vWriteTracingIdRequest},
       {{MSB_BYTE,0x00U,MSG_WRITE_PROD_TIME_REQUEST},               DBPL_vWriteProductionTimeRequest} //Last element (marked with MSB in MesgLen set).
};
#endif

/** Values used by DBPL_TaskState
*/
enum DBPL_States
{
    DBPL_INIT,                         //0 Init timers
    DBPL_ONLINE,                       //1 Normal state when D-Bus-2 is online
    DBPL_OFFLINE_WAIT_FOR_RESET,       //2 Offline state has been requested for a certain time
    DBPL_STAY_OFFLINE                  //3 Offline state has been requested for ever (until power reset)
};

/* Violation of naming convention due to historic code*/
struct TmessageXS
{
    TbusIdentifier tBusIdentifier;
    uint8_t aucData[DBPL_SIZE_OF_DATA_BUFFER_IN_T_BUS_MESSAGE];
};

//lint -esym(9003,DBPL_ucSizeOfDataBufferIn_tBusMessage) block scope not possible
static const uint8_t DBPL_ucSizeOfDataBufferIn_tBusMessage = DBPL_SIZE_OF_DATA_BUFFER_IN_T_BUS_MESSAGE;// PRQA S 6008

static struct TmessageXS  DBPL_tMessageBuffer;              //!<Buffer used for transmission (response) of an incoming request

// following variable used ony in DBPL_vRead/vWrite functions
static uint16_t   DBPL_uiAddress;                             //!< In case a read/write request cannot be handled immediately (read/write function returns ONGOING) the address of the data to be read/written must be buffered.

static enum DBPL_States DBPL_TaskState = DBPL_INIT;      //!< Positive values, when offline state is active, or about to be entered.

static uint8_t    DBPL_ucNumberOfBytesInMessage;              //!< In case a read/write request cannot be handled immediately (read/write function returns ONGOING) the length of the data to be read/written must be buffered. NB! This "data length" is not the same as the data length referred to as first byte in the message frame, this length refers to how many data bytes are present in the message.
static uint8_t    DBPL_ucMemoryModule;                        //!< Memory module refers to a system dependent allocation of the memory within one communication partner. The default memory module is always 0, hence if the message (command) set memory module, MSG_SET_MEMORY_MODULE, is not received, the module is always 0.
static bool       DBPL_bSendMsg = false;                      //!< Variable used to signalize, that a message is to be sent.
static bool       DBPL_bWakeupBreakSent = false;              //!< Variable to see, whether 15 ms break for wakeup has been sent.
static uint8_t    DBPL_ucRepetitions = 0U;                    //!< Variable to see, whether to repeat message.
static volatile   uint16_t DBPL_uiSendPowerMsgUserTrigger = 0U; //!< Trigger WakeUpSentRequest, PowerFail, PowerResurge, PowerMgmtWakeup, PowerMgmtWakeupComplete or PowerMgmtNodeReset message
static uint8_t    DBPL_ucPowerMgmtMsgTargetAddr = 0U;         //!< Variable for setting target DBus address to which power management message (PowerMgmtWakeup, PowerMgmtWakeupComplete or PowerMgmtNodeReset) will be sent
static uint8_t    DBPL_ucNodeAddrToRespondFrom = 0U;          //!< Variable, keeping the dbus address, with which we were addressed by the currently processed message. If it is a service message, we respond to exactly that address.

// Default time is a multiple of 10 ms after which DBus baudrate is set to default value
#define DBPL_STANDARD_BAUD_RESET_TIME_10MS 3500U

// Macros to convert time
#define CONVERT_10MS_TO_1MS(time)   ((time)*10u)
#define CONVERT_2S_TO_1S(time)      ((time)*2u)

static volatile uint16_t DBPL_uBaudRate;
/**
 STIM_Timer instances used by presentation layer.
 The timer concept uses software timers from the STIM library. Once the timer instance has elapsed, the corresponding callback function is executed.
*/
static struct STIM_Timer DBPL_tOfflineDelayTimer;   //!< Timer used by Offline mode and delayed jump to update mode (wired firmware update)
static struct STIM_Timer DBPL_baudRateTimer;       //!< Timer instance responsible for switching to default baudrate
static struct STIM_Timer DBPL_resetTriggerTimer;   //!< Timer instance responsible for reset to user application

#ifdef DBUS2_UPDATE_HSI
static uint8_t  DBPL_ucHsiSenderAddress = 0U;
static uint32_t DBPL_hsiBaudRateTime = 0U; /*Remember value of timer, when processing of HSI request starts, to set value back, once we sent the HSI response*/
#endif

/* Local variables, which may be needed also by the memory access function for 32 bit addressing (the interface of the memory access function only has 16 bit addressing for the parameter */
/**
High integer (most significant integer) of 32 bit address.

The high integer is kept in a separate variable, as the interface function for memory access only foresees 16 bit addressing for the parameter.

*/
uint16_t DBPL_uiPage;
/**
Variable used for signalising dbuspresentation that the current read process cannot be performed, e.g. due to failure of EEPROM. This variable should then be set to a positive value, which will discard the current request (no response message will follow), successively this variable will be reset by dbuspresentation again.
*/
bool DBPL_bInvalidReadProcess = false;
/**
Variable used for signalising dbuspresentation that the current write process cannot be performed, e.g. due to failure of EEPROM. This variable should then be set to a positive value, which will discard the current request (no response message will follow), successively this variable will be reset by dbuspresentation again.
*/
bool DBPL_bInvalidWriteProcess = false;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/**
* Finds the memory module index corresponding to the memory module given by the overlaying layer.
*
* If the given memory module is not found, the index 0 is returned (hence the first defined module will be used).
* This function may be replaced by an empty function in the excess template to save ROM code - if this function is not needed.

\param ucMemoryModule
The memory module, which index is sought.

\return The index corresponding to the module\n
 \b type: uint8_t\n
 \b range: 0..(number of elements in DBPL_tModuleDict) - 1
*/
uint8_t DBPL_ucGetModuleIndex(uint8_t ucMemoryModule)
{
   for (uint8_t ucModuleIndex = 0; ucModuleIndex < DBPL_ucNumberOfElements; ucModuleIndex++)
   {
      if (DBPL_tModuleDict[ucModuleIndex].ucModule == ucMemoryModule)
      {
         return ucModuleIndex;
         //[MISRA 2004 Rule 14.7]: This function contains a second return path. However, this implementation is more effective.
      }
      else
      {
         //Index not found yet, continue searching.
      }
   }
   //Module not found - 'cause it isn't defined
   return 0U;      //If the requested module is not found in the table, the first index (0) is returned.
}


#ifdef RTOS_DBUS_EVENTDRIVEN
void DBPL_ModuleInit( void )
{
    DBPL_initTimers(); //Instances for Baudrate timeout, reset delay & offline delay
    DBPL_TaskState = DBPL_ONLINE;
}


bool DBPL_ReceiveMessage( void )
{
    bool HandlingDone = false;
    const TbusMessage *DBPL_ptReceivedMessage; //!< Pointer to the message presently being processed.

    if( DBPL_TaskState == DBPL_ONLINE )
    {
        if( (DBPL_bSendMsg == false) && (DBPL_bIsSendingPowerMsg() == false) )
        {
            DBPL_ptReceivedMessage = DLL_ptReceiveMessage();
            if (DBPL_ptReceivedMessage != NULL) //Check whether there is a new message for distribution
            {
                DBPL_ucNodeAddrToRespondFrom = DBPL_ptReceivedMessage->tBusIdentifier.ucTargetAddress;
                uint16_t rxMsgId = DBPL_ptReceivedMessage->tBusIdentifier.tMessageIdentifier;
                bool isServiceMsg          = (rxMsgId >= BAL_FIRST_SERVICE_MSGID) ? true : false;
                bool isResetTriggerRequest = (rxMsgId == MSG_RESET_TRIGGER_REQUEST) ? true : false;
                bool isSilentModeRequest   = (rxMsgId == MSG_SILENT_MODE_TRANSITION_REQUEST) ? true : false;
                if (!DLL_isSilentMode() || (isResetTriggerRequest && (0u == DBPL_ucNodeAddrToRespondFrom))) // In Silent mode the only message handled is broadcast ResetTriggerRequest
                {
                    (void)BAL_bDispatchRcvdDbus2Msg(DBPL_ptReceivedMessage); //Nothing to do for Dbus CAN chip variant
                }
                if (DLL_isSilentMode() && (0u != DBPL_ucNodeAddrToRespondFrom) && isServiceMsg && !isSilentModeRequest)
                {   //leave Silent mode only upon addressed (not broadcast) service message received (excluding SilentModeRequest)
                    DLL_vUnsetSilentMode();
                }
            }

            //Check if some more messages received or regular repatition to be done
            if( (DBCDBUS_getNewMsgCount() == 0u) && (DBCDBUS_getRxFramePtr() == NULL) )
            {
                HandlingDone = true;
            }
        }
        else
        {
            //wait for message sent
            HandlingDone = false;
        }
    }
    else
    {
        //nothing to do, if node is offline
        HandlingDone = true;
    }

    return HandlingDone;
}


bool DBPL_SendServiceMessage( void )
{
    bool HandlingDone = false;

    if( (DBPL_TaskState == DBPL_ONLINE) && (DBPL_bSendMsg != false)  )
    {
        if( DBLK_IsDbusLockActive() == false )
        {
            /* Check whether a response to request should be sent */
            if (DLL_bTransmitMessage(DBPL_tMessageBuffer.tBusIdentifier, DBPL_vSendResponse, DBPL_ucNumberOfBytesInMessage, DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier) != false)
            {
                //Message processing will be finished with this step
                HandlingDone = true;
            }
            else
            {
                HandlingDone = false;
            }
        }
        else
        {
            //Repeat, if DBUS locked
            HandlingDone = false;
        }
    }
    else
    {
        //nothing to do, if node is offline or nothing to send
        HandlingDone = true;
    }

    return HandlingDone;
}


bool DBPL_SendPowerMessage( void )
{
    bool HandlingDone = false;

    if( (DBPL_TaskState == DBPL_ONLINE) && (DBPL_bIsSendingPowerMsg() != false) )
    {
        if( (DBPL_bSendMsg == false) && (DBLK_IsDbusLockActive() == false) )
        {
            DBPL_ucNumberOfBytesInMessage = 0U;
            DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = 0U;
            DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = DBPL_uiSendPowerMsgUserTrigger;
            if ((DBPL_bIsSendingPowerResurge() == true) || (DBPL_bIsSendingWakeupSentRequest() == true))
            {
                DBPL_ucNumberOfBytesInMessage = (uint8_t)sizeof(uint8_t);/*Only our own node address*/
                DBPL_tMessageBuffer.aucData[0] = DBPL_ucGetOwnAddressToInformOtherNodes();
            }
            else if (DBPL_bIsSendingPowerMgmtMsg() == true)
            {
                DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_ucPowerMgmtMsgTargetAddr;
            }
            else {}
            DBPL_ucRepetitions = DBPL_POWER_MSG_SEND_RETRIES;
            DBPL_bSendMsg = true;
            DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
            HandlingDone = true;
        }
        else
        {
            //wait for service message sent or unlocked
            HandlingDone = false;
        }
    }
    else
    {
        //nothing to do, if node is offline or no message triggered
        HandlingDone = true;
    }

    return HandlingDone;
}


#else
uint8_t DBPL_HandleTask(void)
{
    const TbusMessage *DBPL_ptReceivedMessage; //!< Pointer to the message presently being processed. 

    if (DBPL_TaskState == DBPL_INIT)
    {
        DBPL_initTimers();              //Instances for Baudrate timeout, reset delay & offline delay
        DBPL_TaskState = DBPL_ONLINE;
    }
    else if (DBPL_TaskState == DBPL_ONLINE)
    {
        /* Check whether a response to request should be sent */
        DBM_UART_vDisableRxEvent();
        if(DBLK_IsDbusLockActive() == false)
        {
            /* Check for received messages or send PowerFail, only if previous was answered properly, or all attempts have been used. -> No race condition! */
            if(DBPL_bSendMsg == false)
            {
                if(DBPL_bIsSendingPowerMsg() == false)
                {
                    DBPL_ptReceivedMessage = DLL_ptReceiveMessage();
                    if (DBPL_ptReceivedMessage != NULL) //Check whether there is a new message for distribution
                    {
                        DBPL_ucNodeAddrToRespondFrom = DBPL_ptReceivedMessage->tBusIdentifier.ucTargetAddress;
                        uint16_t rxMsgId = DBPL_ptReceivedMessage->tBusIdentifier.tMessageIdentifier;
                        bool isServiceMsg = (rxMsgId >= BAL_FIRST_SERVICE_MSGID) ? true : false;
                        #ifdef DBM_DBUSCAN
                        bool isResetTriggerRequest = (rxMsgId == MSG_RESET_TRIGGER_REQUEST) ? true : false;
                        if ((!DLL_isSilentMode() || (isResetTriggerRequest && (0u == DBPL_ucNodeAddrToRespondFrom))) && // In Silent mode the only message handled is broadcast ResetTriggerRequest
                            (BAL_bDispatchRcvdDbus2Msg(DBPL_ptReceivedMessage) != false))
                        #else //!DBM_DBUSCAN
                        if (BAL_bDispatchRcvdDbus2Msg(DBPL_ptReceivedMessage) != false)
                        #endif
                        {
                            if (!isServiceMsg) //Check if message lies within the range 0xF000-0xFFFF, which is the region for service messages
                            {
                                DLL_vReleaseDataFromReceivedMessage(); //The data has been read (possibly saved), message may be released from the input buffer.
                            }
                            else
                            {
                                //Service messages
                                //For DBPL_bSendResponse: The message will be released when the response has been transmitted.
                            }
                        }
                        else
                        {
                            DLL_vReleaseDataFromReceivedMessage(); //Unknown message.
                        }
                        #ifdef DBM_DBUSCAN
                        bool isSilentModeRequest = (rxMsgId == MSG_SILENT_MODE_TRANSITION_REQUEST) ? true : false;
                        if (DLL_isSilentMode() && (0u != DBPL_ucNodeAddrToRespondFrom) && isServiceMsg && !isSilentModeRequest)
                        {   //leave Silent mode only upon addressed (not broadcast) service message received (excluding SilentModeRequest)
                            DLL_vUnsetSilentMode();
                        }
                        #endif //DBM_DBUSCAN
                    }
                }
                else
                {
                    DBPL_ucNumberOfBytesInMessage = 0U;
                    DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = 0U;
                    DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = DBPL_uiSendPowerMsgUserTrigger;
                    if ((DBPL_bIsSendingPowerResurge() == true) || (DBPL_bIsSendingWakeupSentRequest() == true))
                    {
                        DBPL_ucNumberOfBytesInMessage = (uint8_t)sizeof(uint8_t);/*Only our own node address*/
                        DBPL_tMessageBuffer.aucData[0] = DBPL_ucGetOwnAddressToInformOtherNodes();
                    }
                    else if (DBPL_bIsSendingPowerMgmtMsg() == true)
                    {
                        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_ucPowerMgmtMsgTargetAddr;
                    }
                    else {/* No payload for transmit */}
                    DBPL_ucRepetitions = DBPL_POWER_MSG_SEND_RETRIES;
                    DBPL_bSendMsg = true;
                }
            }

            if (DBPL_bSendMsg != false)
            {
                if (DLL_bTransmitMessage(DBPL_tMessageBuffer.tBusIdentifier, DBPL_vSendResponse, DBPL_ucNumberOfBytesInMessage, DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier) != false)
                {
                    //Message processing will be finished with this step, once positive ack received or number of repetitions went to zero.
                }
                else
                {
                    //Response will be tried again next time. No further received message will be processed until message transmission is possible.
                    DBM_UART_vEnableRxEvent();
                    return TASK_INITIALISED;
                }
            }
        }
        DBM_UART_vEnableRxEvent();
    }
    else
    {
        //Stay offline
    }
    return TASK_INITIALISED;
}
#endif //RTOS_DBUS_EVENTDRIVEN

void DBPL_vDecrementRepeatServiceMsg(void)
{
    if(DBPL_ucRepetitions > 0U)
    {
        DBPL_ucRepetitions--;
    }
    else
    {
        if(DBPL_bIsSendingPowerMsg() == true)
        {
            DBPL_vNotifyNonDeliverablePowerMessage(DBPL_uiSendPowerMsgUserTrigger);
        }
        DBPL_vStopRepeatServiceMsg();
    }
}

void DBPL_vStopRepeatServiceMsg(void)
{
    if(DBPL_bIsSendingPowerMsg() == true)
    {
        DBPL_uiSendPowerMsgUserTrigger = 0U;
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
    DBPL_ucRepetitions = 0U;
    DBPL_bSendMsg = false;            //Reset flag
}

bool DBPL_bDoesAppSpecificRepetitionTimeoutApply(uint16_t uiUserCode)
{
    return (uiUserCode >= (uint16_t)BAL_FIRST_SERVICE_MSGID) ? false : true;
}

bool DBPL_bIsOfflineMode(void)
{
    return (DBPL_TaskState >= DBPL_OFFLINE_WAIT_FOR_RESET) ? true : false;
}

uint8_t DBPL_ucGetCurrentMemoryModule(void)
{
    return DBPL_ucMemoryModule;
}

bool DBPL_bSendWakeupSentRequest(void)
{
    if(DBPL_uiSendPowerMsgUserTrigger == 0U)
    {
        DBPL_uiSendPowerMsgUserTrigger = MSG_WAKEUP_SENT_REQUEST;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_PWR );
        return true;
    }
    return false;
}

bool DBPL_bSendPowerFailMsg(void)
{
    if(DBPL_uiSendPowerMsgUserTrigger == 0U)
    {
        DBPL_uiSendPowerMsgUserTrigger = MSG_POWER_FAIL;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_PWR );
        return true;
    }
    return false;
}

bool DBPL_bSendPowerResurgeMsg(void)
{
    if(DBPL_uiSendPowerMsgUserTrigger == 0U)
    {
        DBPL_uiSendPowerMsgUserTrigger = MSG_POWER_RESURGE;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_PWR );
        return true;
    }
    return false;
}

bool DBPL_bSendPowerMgmtWakeup(uint8_t ucTargetAddr)
{
    if(DBPL_uiSendPowerMsgUserTrigger == 0U)
    {
        DBPL_uiSendPowerMsgUserTrigger = MSG_POWER_MGMT_WAKEUP;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_PWR );
        DBPL_ucPowerMgmtMsgTargetAddr  = ucTargetAddr;
        return true;
    }
    return false;
}

bool DBPL_bSendPowerMgmtWakeupComplete(uint8_t ucTargetAddr)
{
    if(DBPL_uiSendPowerMsgUserTrigger == 0U)
    {
        DBPL_uiSendPowerMsgUserTrigger = MSG_POWER_MGMT_WAKEUP_COMPLETE;
        DBPL_ucPowerMgmtMsgTargetAddr  = ucTargetAddr;
        return true;
    }
    return false;
}

bool DBPL_bSendPowerMgmtNodeReset(uint8_t ucTargetAddr)
{
    if(DBPL_uiSendPowerMsgUserTrigger == 0U)
    {
        DBPL_uiSendPowerMsgUserTrigger = MSG_POWER_MGMT_NODE_RESET;
        DBPL_ucPowerMgmtMsgTargetAddr  = ucTargetAddr;
        return true;
    }
    return false;
}

bool DBPL_bIsSendingWakeupSentRequest(void)
{
    return (DBPL_uiSendPowerMsgUserTrigger == MSG_WAKEUP_SENT_REQUEST) ? true : false;
}

bool DBPL_bIsSendingPowerFail(void)
{
    return (DBPL_uiSendPowerMsgUserTrigger == MSG_POWER_FAIL) ? true : false;
}

bool DBPL_bIsSendingPowerResurge(void)
{
    return (DBPL_uiSendPowerMsgUserTrigger == MSG_POWER_RESURGE) ? true : false;
}

bool DBPL_bIsSendingPowerMgmtWakeup(void)
{
    return (DBPL_uiSendPowerMsgUserTrigger == MSG_POWER_MGMT_WAKEUP) ? true : false;
}

bool DBPL_bIsSendingPowerMgmtWakeupComplete(void)
{
    return (DBPL_uiSendPowerMsgUserTrigger == MSG_POWER_MGMT_WAKEUP_COMPLETE) ? true : false;
}

bool DBPL_bIsSendingPowerMgmtNodeReset(void)
{
    return (DBPL_uiSendPowerMsgUserTrigger == MSG_POWER_MGMT_NODE_RESET) ? true : false;
}

bool DBPL_bIsSendingPowerMsg(void)
{
    return ((DBPL_bIsSendingWakeupSentRequest() == true) || (DBPL_bIsSendingPowerFail() == true) || (DBPL_bIsSendingPowerResurge() == true) || (DBPL_bIsSendingPowerMgmtMsg() == true)) ? true : false;
}

bool DBPL_bIsReceivedMessageIdPowerFail(uint16_t msgId)
{
    return (msgId == MSG_POWER_FAIL) ? true : false;
}

bool DBPL_bReadMemory(uint8_t ucMemoryModule, uint16_t uiAddress, uint8_t ucDataLength, uint8_t *pucMessageData)
{
   uint8_t ucModuleIndex;             //The index of the module in the list of modules
   ucModuleIndex = DBPL_ucGetModuleIndex(ucMemoryModule);
   return (DBPL_tModuleDict[ucModuleIndex].tComReadFunction(ucMemoryModule, uiAddress, ucDataLength, pucMessageData));
}

bool DBPL_bWriteMemory(uint8_t ucMemoryModule, uint16_t uiAddress, uint8_t ucDataLength, uint8_t *pucMessageData)
{
   uint8_t ucModuleIndex;             //The index of the module in the list of modules
   ucModuleIndex = DBPL_ucGetModuleIndex(ucMemoryModule);
   return (DBPL_tModuleDict[ucModuleIndex].tComWriteFunction(ucMemoryModule, uiAddress, ucDataLength, pucMessageData));
}

bool DBPL_bGetIdAddress(uint8_t ucMemoryModule, uint16_t *puiIdAddress)
{
   uint8_t ucModuleIndex;             //The index of the module in the list of modules
   ucModuleIndex = DBPL_ucGetModuleIndex(ucMemoryModule);
   return (DBPL_tModuleDict[ucModuleIndex].tIdFunction(ucMemoryModule, puiIdAddress));
}

void DBPL_vNotifyWakeupBreakSent(void)
{
    DBPL_bWakeupBreakSent = true;
}

void DBPL_vUnNotifyWakeupBreakSent(void)
{
    DBPL_bWakeupBreakSent = false;
}

uint16_t DBPL_uiGetStandardBaudRate(void)
{
    return (uint16_t)DBPL_uDefaultBaudRate;
}

#ifdef DBM_DBUSCAN
uint8_t DBPL_ucGetMsgRepetitions(void)
{
    return DBPL_ucRepetitions;
}

uint16_t DBPL_uiGetPowerMsgUserTrigger(void)
{
    return DBPL_uiSendPowerMsgUserTrigger;
}
#endif // DBM_DBUSCAN


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/**Mandatory service function for read requests*/
static void DBPL_vReadRequest(uint8_t ucDataLen, uint8_t *pucData)
{
   enum{
       DBPL_READ_REQ_SA,              //0 Source Address
       DBPL_READ_REQ_NBR_BYTES,       //1 Number of bytes to read
       DBPL_READ_REQ_ADRH,            //2 Address to read (high byte)
       DBPL_READ_REQ_ADRL,            //3 Address to read (low byte)
       DBPL_READ_REQ_MODULE_NUM,      //4 Memory Module (optional parameter)
       DBPL_READ_REQ_MAX_LEN
   };

   enum{
       DBPL_READ_RESP_SA,
       DBPL_READ_RESP_NBR_BYTES,
       DBPL_READ_RESP_DATA_OFFSET
   };

   uint8_t ucDataLength;  //Data length to be read.
   uint8_t ucMemoryModule; //Memory module to be read

   if((ucDataLen == (uint8_t)DBPL_READ_REQ_MODULE_NUM) || (ucDataLen == (uint8_t)DBPL_READ_REQ_MAX_LEN))
   {
       ucDataLength = pucData[DBPL_READ_REQ_NBR_BYTES];
       DBPL_ucNumberOfBytesInMessage = pucData[DBPL_READ_REQ_NBR_BYTES] + (uint8_t)DBPL_READ_RESP_DATA_OFFSET; //Length of data part of message

       if (ucDataLen == (uint8_t)DBPL_READ_REQ_MAX_LEN)
       {
           ucMemoryModule = pucData[DBPL_READ_REQ_MODULE_NUM];
       }
       else
       {
           ucMemoryModule = DBPL_ucMemoryModule;
       }

       if ((DBPL_ucNumberOfBytesInMessage) > DBPL_ucSizeOfDataBufferIn_tBusMessage)
       {
           //Cannot read more bytes than can be saved in the tMessage struct
           DBPL_ucNumberOfBytesInMessage = DBPL_ucSizeOfDataBufferIn_tBusMessage;
           ucDataLength = DBPL_ucNumberOfBytesInMessage - (uint8_t)DBPL_READ_RESP_DATA_OFFSET;
       }
       DBPL_uiAddress = ((uint16_t)pucData[DBPL_READ_REQ_ADRH]<<8)|((uint16_t)pucData[DBPL_READ_REQ_ADRL]);
       if (DBPL_bReadMemory(ucMemoryModule, DBPL_uiAddress, ucDataLength, &DBPL_tMessageBuffer.aucData[DBPL_READ_RESP_DATA_OFFSET]) == DBPL_DONE)
       {
           //ok, message processing can be finished in one step.
           DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_READ_REQ_SA];
           DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_READ_RESPONSE;
           DBPL_tMessageBuffer.aucData[DBPL_READ_RESP_SA] = DBPL_ucNodeAddrToRespondFrom;
           DBPL_tMessageBuffer.aucData[DBPL_READ_RESP_NBR_BYTES] = ucDataLength;
           DBPL_bSendMsg = true;
           DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
       }
       else
       {
           //Finishing this message must be done later...
           if (DBPL_bInvalidReadProcess != false)
           {
               //Read process cannot be finished, discard request.
               DBPL_bInvalidReadProcess = false;
               DBPL_bSendMsg = false;

               DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
           }
       }
   }
   else
   {
       DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
   }
}

static void DBPL_vIdentityRequest(uint8_t ucDataLen, uint8_t *pucData)
{
   enum{
       DBPL_ID_REQ_SOURCE,
       DBPL_ID_REQ_MEM_MODULE,
       DBPL_ID_REQ_MAX_LEN
   };

   enum{
       DBPL_ID_RESP_ADRS,     //0 Source address
       DBPL_ID_RESP_MODL,     //1 Memory module
       DBPL_ID_RESP_ADRH,     //2 Address high of ID-String
       DBPL_ID_RESP_ADRL,     //3 Address low of ID-String
       DBPL_ID_RESP_LEN
   };

   uint8_t ucMemoryModule;
   uint16_t addressOfString;

   if((ucDataLen == (uint8_t)DBPL_ID_REQ_MEM_MODULE) || (ucDataLen == (uint8_t)DBPL_ID_REQ_MAX_LEN))
   {
       if(ucDataLen == (uint8_t)DBPL_ID_REQ_MAX_LEN)
       {
           ucMemoryModule = pucData[DBPL_ID_REQ_MEM_MODULE];
       }
       else
       {
           ucMemoryModule = DBPL_ucMemoryModule;
       }
       DBPL_ucNumberOfBytesInMessage = DBPL_ID_RESP_LEN;
       if (DBPL_bGetIdAddress(ucMemoryModule, &addressOfString) == DBPL_DONE)/*lint !e934 : Taking address of near auto variable 'addressOfString'. -> This is intended, and the value copied is elsewhere, before function returns.*/
       {
           DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_ID_REQ_SOURCE]; //Save the target address of the requesting communication partner.
           DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_IDENTITY_RESPONSE;
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_ADRS] = (uint8_t) (DBPL_ucNodeAddrToRespondFrom);
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_MODL] = ucMemoryModule;
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_ADRH] = (uint8_t)(addressOfString>>8);
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_ADRL] = (uint8_t)addressOfString;
           DBPL_bSendMsg = true;
           DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
       }
   }
   else
   {
       DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
   }
}

static void DBPL_vIdentityRequest32(uint8_t ucDataLen, uint8_t *pucData)
{
   enum{
       DBPL_ID_REQ_SOURCE,
       DBPL_ID_REQ_MEM_MODULE,
       DBPL_ID_REQ_MAX_LEN
   };

   enum{
       DBPL_ID_RESP_ADRS,      //0 Source Address
       DBPL_ID_RESP_MODL,      //1 Memory module
       DBPL_ID_RESP_ADRHH,     //2 Address High (byte 3)
       DBPL_ID_RESP_ADRHL,     //3 Address byte 2
       DBPL_ID_RESP_ADRLH,     //4 Address byte 1
       DBPL_ID_RESP_ADRLL,     //5 Address Low (byte 0)
       DBPL_ID_RESP_LEN
   };

   uint16_t uiPageNumberToBeRestoredAfterCommandExecution;
   uint16_t lowerTwoBytesOfAddrOfString;
   uint8_t ucMemoryModule;

   if((ucDataLen == (uint8_t)DBPL_ID_REQ_MEM_MODULE) || (ucDataLen == (uint8_t)DBPL_ID_REQ_MAX_LEN))
   {
       if(ucDataLen == (uint8_t)DBPL_ID_REQ_MAX_LEN)
       {
           ucMemoryModule = pucData[DBPL_ID_REQ_MEM_MODULE];
       }
       else
       {
           ucMemoryModule = DBPL_ucMemoryModule;
       }
       uiPageNumberToBeRestoredAfterCommandExecution = DBPL_uiPage;
       DBPL_uiPage = 0U; //Important, in case uiPage is not used by ID-Response
       DBPL_ucNumberOfBytesInMessage = DBPL_ID_RESP_LEN;
       if (DBPL_bGetIdAddress(ucMemoryModule, &lowerTwoBytesOfAddrOfString) == DBPL_DONE)/*lint !e934 : Taking address of near auto variable 'lowerTwoBytesOfAddrOfString'. -> This is intended, and the value is copied elsewhere, before function returns.*/
       {
           DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_ID_REQ_SOURCE]; //Save the target address of the requesting communication partner.
           DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_IDENTITY_RESPONSE;
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_ADRS] = DBPL_ucNodeAddrToRespondFrom;
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_MODL] = ucMemoryModule;
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_ADRHH] = (uint8_t)(DBPL_uiPage>>8);
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_ADRHL] = (uint8_t)DBPL_uiPage;
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_ADRLH] = (uint8_t)(lowerTwoBytesOfAddrOfString>>8);
           DBPL_tMessageBuffer.aucData[DBPL_ID_RESP_ADRLL] = (uint8_t)lowerTwoBytesOfAddrOfString;
           DBPL_bSendMsg = true;
           DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
       }
       DBPL_uiPage = uiPageNumberToBeRestoredAfterCommandExecution;
   }
   else
   {
       DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
   }
}

/* Optional messages for reception */
/**Service function for write requests*/ //This service is no longer mandatory (since spring 2005)
static void DBPL_vWriteRequest(uint8_t ucDataLen, uint8_t *pucData)
{
   enum{
       DBPL_WRITE_NBR_BYTES,       //0 Number of bytes to read
       DBPL_WRITE_ADRH,            //1 Address to write (high byte)
       DBPL_WRITE_ADRL,            //2 Address to write (low byte)
       DBPL_BUFFER_START           //3 Start of the buffer with data to be written
   };

   uint8_t ucMemoryModule;
   
   if(ucDataLen >= (uint8_t)DBPL_BUFFER_START)
   {
       DBPL_ucNumberOfBytesInMessage = pucData[DBPL_WRITE_NBR_BYTES];

       if((ucDataLen == ((uint8_t)DBPL_BUFFER_START + DBPL_ucNumberOfBytesInMessage + 1U)) || (ucDataLen == ((uint8_t)DBPL_BUFFER_START + DBPL_ucNumberOfBytesInMessage)))
       {
           if(ucDataLen == ((uint8_t)DBPL_BUFFER_START + DBPL_ucNumberOfBytesInMessage + 1U))
           {
               ucMemoryModule = pucData[((uint8_t)DBPL_BUFFER_START) + DBPL_ucNumberOfBytesInMessage];
           }
           else
           {
               ucMemoryModule = DBPL_ucMemoryModule;
           }
           DBPL_uiAddress = ((uint16_t)pucData[DBPL_WRITE_ADRH]<<8)|((uint16_t)pucData[DBPL_WRITE_ADRL]);
           //Care must be taken, that the data length used in the call below is correct. Typically, if DBPL_ucNumberOfBytesInMessage is more than the actual length to be written  (compare DBPL_vReadRequest() above).
           if (DBPL_bWriteMemory(ucMemoryModule, DBPL_uiAddress, DBPL_ucNumberOfBytesInMessage, &pucData[DBPL_BUFFER_START]) == DBPL_DONE)
           {
               //ok, message processing can be finished in one step.
               DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
           }
           else
           {
               //Finishing this message must be done later...
               if(DBPL_bInvalidWriteProcess != false)
               {
                   //Write process cannot be finished, discard request.
                   DBPL_bInvalidWriteProcess = false;
                   DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
               }
           }
       }
       else
       {
           DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
       }
   }
   else
   {
       DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
   }
}

static void DBPL_vGoOfflineService(uint8_t ucDataLen, uint8_t *pucData)
{
   enum{
       DBPL_GOFFL_TA_UPD,            //0 Address of node to update
       DBPL_GOFFL_RESET_DELAY,       //1 Reset delay of node to update
       DBPL_GOFFL_OFFLINE_DELAY,     //2 Offline delay of node, which will not be updated
       DBPL_GOFFL_LEN
   };

   uint8_t offlineTime_2sec;       // represented in 2 s unit.
   uint8_t ucTgAdr;

   if(ucDataLen == (uint8_t)DBPL_GOFFL_LEN)
   {
       ucTgAdr = pucData[DBPL_GOFFL_TA_UPD];
       if (DLL_bIsCurrentNode(ucTgAdr) != false)
       {
        #if !defined (FWU3_LITE)
           //This node is about to be updated by the bootloader. Execute a reset after ResetDelay * 10 ms
           DBPL_TaskState = DBPL_OFFLINE_WAIT_FOR_RESET;
           DBPL_vEnterBootloaderMode(); //Interface, which can notify bootloader application that the flash programmer is expected, hence a long timeout can be used after reset.
           DLL_vGoOffline();
           DBPL_setResetTimer(CONVERT_10MS_TO_1MS((uint16_t)pucData[DBPL_GOFFL_RESET_DELAY]));
        #endif // !defined (FWU3_LITE)
       }
       else
       {
           //Another node is about to be updated by the bootloader. Stay PASSIVE for OfflineDelay * 2s as the bus is needed exclusively by the bootloader and the node, which will be updated.
           offlineTime_2sec = DBPL_ucPossiblyChangeTimerBase(pucData[DBPL_GOFFL_OFFLINE_DELAY]);
           if (offlineTime_2sec != 0U)
           {
               DBPL_setOfflineTimer(CONVERT_2S_TO_1S((uint16_t)offlineTime_2sec));
               DBPL_TaskState = DBPL_OFFLINE_WAIT_FOR_RESET;
           }
           else
           {
               //Stay offline until reset.
               DBPL_TaskState = DBPL_STAY_OFFLINE;
           }
           DLL_vGoOffline();
           DBPL_vGoOffline(); //Function defined in the excess template, which allows for system dependent actions upon entering offline mode.
       }
   }

   DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Optional service function for reset request*/
static void DBPL_vResetExecute(uint8_t ucDataLen, uint8_t *pucData)
{
    enum{
        DBPL_RESETEXECUTE_TIME,
        DBPL_RESETEXECUTE_LEN
    };

    if(ucDataLen == (uint8_t)DBPL_RESETEXECUTE_LEN)
    {
        DBPL_setResetTimer(CONVERT_10MS_TO_1MS((uint16_t)pucData[DBPL_RESETEXECUTE_TIME]));
        DBPL_TaskState = DBPL_OFFLINE_WAIT_FOR_RESET;
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Optional service function for set memory module request*/
static void DBPL_vSetMemoryModule(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_setModule{
        DBPL_SETMODULE_NUMBER,
        DBPL_SETMODULE_LEN
    };

    if(ucDataLen == (uint8_t)DBPL_SETMODULE_LEN)
    {
        DBPL_ucMemoryModule = pucData[DBPL_SETMODULE_NUMBER];
        if (DBPL_ucGetModuleIndex(DBPL_ucMemoryModule) == 0U)
        {
            //If the requested memory module does not exist (or is 0), then DBPL_ucMemoryModule = 0. This to make sure, that when checking the memory module ID, the used memory module will be returned, not a non-existing one.
            DBPL_ucMemoryModule = 0U;
        }
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Optional service function for read request using 32 bit addressing - this function will also be used for 16 bit addressing, when 32 bit is needed.*/
static void DBPL_vReadRequest32(uint8_t ucDataLen, uint8_t *pucData)
{
   enum{
       DBPL_READ32_REQ_SA,              //0 Source Address
       DBPL_READ32_REQ_NBR_BYTES,       //1 Number of bytes to read
       DBPL_READ32_REQ_ADRHIGH,         //2 Address to read
       DBPL_READ32_REQ_ADR2,            //3 Address to read
       DBPL_READ32_REQ_ADR1,            //4 Address to read
       DBPL_READ32_REQ_ADR0,            //5 Address to read
       DBPL_READ32_REQ_MODULE_NUM,      //6 Memory Module (optional parameter)
       DBPL_READ32_REQ_MAX_LEN
   };

   enum{
       DBPL_READ32_RESP_SA,
       DBPL_READ32_RESP_NBR_BYTES,
       DBPL_READ32_RESP_DATA_OFFSET
   };

   uint8_t ucDataLength;  //Data length to be read.
   uint8_t ucMemoryModule; //Memory module to be read
   uint16_t uiPageNumberToBeRestoredAfterCommandExecution; 
   
   if((ucDataLen == (uint8_t)DBPL_READ32_REQ_MODULE_NUM) || (ucDataLen == (uint8_t)DBPL_READ32_REQ_MAX_LEN))
   {
       ucDataLength = pucData[DBPL_READ32_REQ_NBR_BYTES];
       DBPL_ucNumberOfBytesInMessage = pucData[DBPL_READ32_REQ_NBR_BYTES] + (uint8_t)DBPL_READ32_RESP_DATA_OFFSET;

       if (ucDataLen == (uint8_t)DBPL_READ32_REQ_MAX_LEN)
       {
           ucMemoryModule = pucData[DBPL_READ32_REQ_MODULE_NUM];
       }
       else
       {
           ucMemoryModule = DBPL_ucMemoryModule;
       }

       if ((DBPL_ucNumberOfBytesInMessage) > DBPL_ucSizeOfDataBufferIn_tBusMessage)
       {
           //Cannot read more bytes than can be saved in the tMessage struct
           DBPL_ucNumberOfBytesInMessage = DBPL_ucSizeOfDataBufferIn_tBusMessage;
           ucDataLength = DBPL_ucNumberOfBytesInMessage - (uint8_t)DBPL_READ32_RESP_DATA_OFFSET;
       }

       //This is a 32 bit addressed read request 0xF001U (it is assumed that the data length is correct - this is not explicitly tested, also 0xF001U is not explicitly tested).
       uiPageNumberToBeRestoredAfterCommandExecution = DBPL_uiPage;
       DBPL_uiPage = ((uint16_t)pucData[DBPL_READ32_REQ_ADRHIGH]<<8)|((uint16_t)pucData[DBPL_READ32_REQ_ADR2]);
       DBPL_uiAddress = ((uint16_t)pucData[DBPL_READ32_REQ_ADR1]<<8)|((uint16_t)pucData[DBPL_READ32_REQ_ADR0]);

       if (DBPL_bReadMemory(ucMemoryModule, DBPL_uiAddress, ucDataLength, &DBPL_tMessageBuffer.aucData[DBPL_READ32_RESP_DATA_OFFSET]) == DBPL_DONE)
       {
           //ok, message processing can be finished in one step.
           DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_READ32_REQ_SA]; //Save the target address of the requesting communication partner.
           DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_READ_RESPONSE32;
           DBPL_tMessageBuffer.aucData[DBPL_READ32_RESP_SA] = DBPL_ucNodeAddrToRespondFrom;
           DBPL_tMessageBuffer.aucData[DBPL_READ32_RESP_NBR_BYTES] = ucDataLength;
           DBPL_bSendMsg = true;
           DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
       }
       else
       {
           //Finishing this message must be done later...
           if (DBPL_bInvalidReadProcess != false)
           {
               //Read process cannot be finished, discard request.
               DBPL_bInvalidReadProcess = false;
               DBPL_bSendMsg = false;
               DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
           }
       }
       DBPL_uiPage = uiPageNumberToBeRestoredAfterCommandExecution;
   }
   else
   {
       DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
   }
}

/**Optional service function for write request using 32 bit addressing - this function will also be used for 16 bit addressing, when 32 bit is needed.*/
static void DBPL_vWriteRequest32(uint8_t ucDataLen, uint8_t *pucData)
{
   enum{
       DBPL_WRITE32_NBR_BYTES,       //0 Number of bytes to write
       DBPL_WRITE32_ADRHIGH,         //1 Address to write
       DBPL_WRITE32_ADR2,            //2 Address to write
       DBPL_WRITE32_ADR1,            //3 Address to write
       DBPL_WRITE32_ADR0,            //4 Address to write
       DBPL_WRITE32_DATA_OFFSET
   };

   uint8_t ucMemoryModule;
   uint16_t uiPageNumberToBeRestoredAfterCommandExecution; 
   
   if(ucDataLen >= (uint8_t)DBPL_WRITE32_DATA_OFFSET)
   {
       DBPL_ucNumberOfBytesInMessage = pucData[DBPL_WRITE32_NBR_BYTES];

       if((ucDataLen == ((uint8_t)DBPL_WRITE32_DATA_OFFSET + DBPL_ucNumberOfBytesInMessage + 1U)) || (ucDataLen == ((uint8_t)DBPL_WRITE32_DATA_OFFSET + DBPL_ucNumberOfBytesInMessage)))
       {
           if(ucDataLen == ((uint8_t)DBPL_WRITE32_DATA_OFFSET + DBPL_ucNumberOfBytesInMessage + 1U))
           {
               ucMemoryModule = pucData[((uint8_t)DBPL_WRITE32_DATA_OFFSET) + DBPL_ucNumberOfBytesInMessage];
           }
           else
           {
               ucMemoryModule = DBPL_ucMemoryModule;
           }
           
           uiPageNumberToBeRestoredAfterCommandExecution = DBPL_uiPage;
           DBPL_uiPage = ((uint16_t)pucData[DBPL_WRITE32_ADRHIGH]<<8)|((uint16_t)pucData[DBPL_WRITE32_ADR2]);
           DBPL_uiAddress = ((uint16_t)pucData[DBPL_WRITE32_ADR1]<<8)|((uint16_t)pucData[DBPL_WRITE32_ADR0]);
           //Care must be taken, that the data length used in the call below is correct. Typically, if DBPL_ucNumberOfBytesInMessage is more than the actual length to be written  (compare DBPL_vReadRequest32() above).
           if (DBPL_bWriteMemory(ucMemoryModule, DBPL_uiAddress, DBPL_ucNumberOfBytesInMessage, &pucData[DBPL_WRITE32_DATA_OFFSET]) == DBPL_DONE)
           {
               //ok, message processing can be finished in one step.
               DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
           }
           else
           {
               //Finishing this message must be done later...
               if(DBPL_bInvalidWriteProcess != false)
               {
                   //Write process cannot be finished, discard request.
                   DBPL_bInvalidWriteProcess = false;
                   DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
               }
           }
           DBPL_uiPage = uiPageNumberToBeRestoredAfterCommandExecution;
       }
       else
       {
           DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
       }
   }
   else
   {
       DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
   }
}

/** enumeration for wakeup sent request */
enum DBPL_WakeupSentRequestBytes
{
    DBPL_WAKEUP_SENT_REQUEST_SENDER,
    DBPL_WAKEUP_SENT_REQUEST_LEN
};

/** enumeration for wakeup sent response */
enum DBPL_WakeupSentResponseBytes
{
    DBPL_WAKEUP_SENT_RESPONSE_SENDER,
    DBPL_WAKEUP_SENT_RESPONSE_LEN
};


/**Optional service function for requesting, whether wakeup signal was sent by that node*/
static void DBPL_vWakeupSentRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    if((ucDataLen == (uint8_t)DBPL_WAKEUP_SENT_REQUEST_LEN) && (DBPL_bWakeupBreakSent == true) && (DBPL_bIsNodeToBeWokenUp(pucData[DBPL_WAKEUP_SENT_REQUEST_SENDER]) == true))
    {
        DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_WAKEUP_SENT_RESPONSE_LEN;
        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_WAKEUP_SENT_REQUEST_SENDER]; //Save the target address of the requesting communication partner.
        DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_WAKEUP_SENT_RESPONSE;
        DBPL_tMessageBuffer.aucData[DBPL_WAKEUP_SENT_RESPONSE_SENDER] = DBPL_ucGetOwnAddressToInformOtherNodes();
        DBPL_ucRepetitions = DBPL_POWER_MSG_SEND_RETRIES;
        DBPL_bSendMsg = true;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

static void DBPL_vWakeupSentResponse(uint8_t ucDataLen, uint8_t *pucData)
{

    if(ucDataLen == (uint8_t)DBPL_WAKEUP_SENT_RESPONSE_LEN)
    {
        DBPL_vWakeupSentResponseReceived(pucData[DBPL_WAKEUP_SENT_RESPONSE_SENDER]);
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Optional service function for power resurge.*/
static void DBPL_vPowerResurgeMsg(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_PowerResurgeBytes{
        DBPL_POWER_RESURGE_SENDER,
        DBPL_POWER_RESURGE_LEN
    };

    if(ucDataLen == (uint8_t)DBPL_POWER_RESURGE_LEN)
    {
        DBPL_vPowerResurgeNotificationHasBeenReceived(pucData[DBPL_POWER_RESURGE_SENDER]);
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Optional service function for requesting unique Id.*/
static void DBPL_vEcuUniqueIdReadRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_EcuUniqueIdReadRequestBytes{
        DBPL_ECU_UNIQUE_ID_REQUEST_SENDER,
        DBPL_ECU_UNIQUE_ID_REQUEST_LEN
    };

    enum DBPL_EcuUniqueIdReadResponseBytes{
        DBPL_ECU_UNIQUE_ID_RESPONSE_SENDER,
        DBPL_ECU_UNIQUE_ID_RESPONSE_STATUS,
        DBPL_ECU_UNIQUE_ID_RESPONSE_ID_LEN,
        DBPL_ECU_UNIQUE_ID_RESPONSE_DATAOFFSET
    };

    if(ucDataLen == (uint8_t)DBPL_ECU_UNIQUE_ID_REQUEST_LEN)
    {
        struct DBPL_EcuUniqueIdReadResult result =
        {
            .status = DBPL_EcuStatusUpdateModeActive,
            .uniqueIdLen = 0U,
            .uniqueIdPtr = &DBPL_tMessageBuffer.aucData[((uint8_t)DBPL_ECU_UNIQUE_ID_RESPONSE_DATAOFFSET)]
        };
        uint8_t maxReadableUniqueIdLen = DBPL_ucSizeOfDataBufferIn_tBusMessage - ((uint8_t)DBPL_ECU_UNIQUE_ID_RESPONSE_DATAOFFSET);

        DBPL_tGetUniqueIdReadResult(maxReadableUniqueIdLen, &result);/*lint !e934 Taking address fully intended.*/

        DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_ECU_UNIQUE_ID_RESPONSE_DATAOFFSET + result.uniqueIdLen;
        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_ECU_UNIQUE_ID_REQUEST_SENDER]; //Save the target address of the requesting communication partner.
        DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_ECU_UNIQUE_ID_READ_RES;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_UNIQUE_ID_RESPONSE_SENDER] = DBPL_ucNodeAddrToRespondFrom;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_UNIQUE_ID_RESPONSE_STATUS] = (uint8_t)result.status;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_UNIQUE_ID_RESPONSE_ID_LEN] = result.uniqueIdLen;
        DBPL_ucRepetitions = DBPL_UPDATE_MSG_SEND_RETRIES;
        DBPL_bSendMsg = true;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

/**Production Service Message to reset project specific values written during Factory Test*/
static void DBPL_vFactoryReset(uint8_t ucDataLen, uint8_t *pucData)
{
    if(ucDataLen == 0U)
    {
        DBPL_vDoFactoryReset();
    }
    else
    {
        (void)*pucData;
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/** Service function used for all response service messages (all outgoing presentation layer messages) */
static void DBPL_vSendResponse(uint8_t ucDataLen, uint8_t *pucData)
{
   for (uint8_t i = 0; i < ucDataLen; i++)
   {
      pucData[i] = DBPL_tMessageBuffer.aucData[i];
   }
}

/**Update service function for getting Silent Mode Requests*/
static void DBPL_vSilentModeTransitionRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_SilentModeTransitionRequestBytes{
        DBPL_SILENT_MODE_TRANSITION_REQUEST_SENDER,
        DBPL_SILENT_MODE_TRANSITION_REQUEST_LEN
    };

    if(ucDataLen == (uint8_t)DBPL_SILENT_MODE_TRANSITION_REQUEST_LEN)
    {
        if(DLL_isSilentMode() == false)
        {
            DLL_vSetSilentMode();
            DBPL_vSilentModeHasBeenEntered();
        }
        DBPL_ucNumberOfBytesInMessage = 0U;
        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_SILENT_MODE_TRANSITION_REQUEST_SENDER]; //Save the target address of the requesting communication partner.
        DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_SILENT_MODE_TRANSITION_RESPONSE;
        DBPL_ucRepetitions = DBPL_UPDATE_MSG_SEND_RETRIES;
        DBPL_bSendMsg = true;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

static void DBPL_vSilentModeTransitionRequestBroadcast(uint8_t ucDataLen, uint8_t *pucData)
{
    if(ucDataLen == 0U)
    {
        if(DLL_isSilentMode() == false)
        {
            DLL_vSetSilentMode();
            DBPL_vSilentModeHasBeenEntered();
        }
    }
    else
    {
        (void)*pucData;
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Update service function for getting Update Mode Verify Requests*/
static void DBPL_vUpdateModeVerifyRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_UpdateModeVerifyRequestBytes{
        DBPL_UPDATE_MODE_VERIFY_REQ_SENDER,
        DBPL_UPDATE_MODE_VERIFY_REQ_LEN
    };

    enum DBPL_UpdateModeVerifyResponseBytes{
        DBPL_UPDATE_MODE_VERIFY_RESP_DELAY_MS_HI,
        DBPL_UPDATE_MODE_VERIFY_RESP_DELAY_MS_LO,
        DBPL_UPDATE_MODE_VERIFY_RESP_LEN
    };

    uint16_t delay_10ms;

    if(ucDataLen == (uint8_t)DBPL_UPDATE_MODE_VERIFY_REQ_LEN)
    {
        if(DBPL_bIsUpdateModePossible())
        {
            delay_10ms = DBPL_ulGetUpdateTransitionDelay();
            DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_UPDATE_MODE_VERIFY_RESP_LEN;
            DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_UPDATE_MODE_VERIFY_REQ_SENDER]; //Save the target address of the requesting communication partner.
            DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_UPDATE_MODE_VERIFY_RESPONSE;
            DBPL_tMessageBuffer.aucData[DBPL_UPDATE_MODE_VERIFY_RESP_DELAY_MS_HI] = (uint8_t)(delay_10ms>>8);
            DBPL_tMessageBuffer.aucData[DBPL_UPDATE_MODE_VERIFY_RESP_DELAY_MS_LO] = (uint8_t)delay_10ms;
            DBPL_ucRepetitions = DBPL_UPDATE_MSG_SEND_RETRIES;
            DBPL_bSendMsg = true;
            DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
        }
        else
        {
            DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
        }
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

/**Update service function for getting Update Mode Transition Requests*/
static void DBPL_vUpdateModeTransitionRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    if(ucDataLen == 0U)
    {
        DBPL_vSetUpdateMode();
    }
    else
    {
        (void)*pucData;
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Update service function for getting  Baudrate Verify Requests*/
static void DBPL_vBaudrateVerifyRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_BaurateVerifyRequestBytes{
        DBPL_BAUD_RATE_VERIFY_REQ_SENDER,
        DBPL_BAUD_RATE_VERIFY_REQ_BAUD_HI,
        DBPL_BAUD_RATE_VERIFY_REQ_BAUD_LO,
        DBPL_BAUD_RATE_VERIFY_REQ_LEN
    };
    enum DBPL_BaurateVerifyResponseBytes{
        DBPL_BAUD_RATE_VERIFY_RESP_DELAY_MS_HI,
        DBPL_BAUD_RATE_VERIFY_RESP_DELAY_MS_LO,
        DBPL_BAUD_RATE_VERIFY_RESP_LEN
    };

    uint16_t delay_10ms;
    uint16_t baud;

    if(ucDataLen == (uint8_t)DBPL_BAUD_RATE_VERIFY_REQ_LEN)
    {
        baud = (((uint16_t)pucData[DBPL_BAUD_RATE_VERIFY_REQ_BAUD_HI]<<8)|(uint16_t)pucData[DBPL_BAUD_RATE_VERIFY_REQ_BAUD_LO]);

        if(DBPL_bIsRequestedBaudValid(baud))
        {
            DBPL_uBaudRate = baud;
            delay_10ms = DBPL_ulGetBaudrateTransitionDelay();
            DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_BAUD_RATE_VERIFY_RESP_LEN;
            DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_BAUD_RATE_VERIFY_REQ_SENDER]; //Save the target address of the requesting communication partner.
            DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_BAUDRATE_VERIFY_RESPONSE;
            DBPL_tMessageBuffer.aucData[DBPL_BAUD_RATE_VERIFY_RESP_DELAY_MS_HI] = (uint8_t)(delay_10ms>>8);
            DBPL_tMessageBuffer.aucData[DBPL_BAUD_RATE_VERIFY_RESP_DELAY_MS_LO] = (uint8_t)delay_10ms;
            DBPL_ucRepetitions = DBPL_UPDATE_MSG_SEND_RETRIES;
            DBPL_bSendMsg = true;
            DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
        }
        else
        {
            DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
        }
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

/**Update service function for getting Baudrate Transition Requests*/
static void DBPL_vBaudrateTransitionRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    if(ucDataLen == 0U)
    {
        DBPL_vConfigureBaudrate(DBPL_uBaudRate);
        if(DBPL_uBaudRate != (uint16_t)DBPL_uDefaultBaudRate)
        {
            DBPL_setBaudRateTimer(CONVERT_10MS_TO_1MS(DBPL_STANDARD_BAUD_RESET_TIME_10MS));
        }
    }
    else
    {
        (void)*pucData;
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Update service function for getting Baudrate Trigger Reqests*/
static void DBPL_vBaudrateTriggerRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_BaudRateTriggerBytes{
        DBPL_BAUD_RATE_TRIGGER_TIME_MS_HI,
        DBPL_BAUD_RATE_TRIGGER_MS_LO,
        DBPL_BAUD_RATE_TRIGGER_LEN
    };

    uint16_t time_trigger_10ms;      // represented in 10 ms unit.

    if(ucDataLen == (uint8_t)DBPL_BAUD_RATE_TRIGGER_LEN)
    {
        time_trigger_10ms = (((uint16_t)pucData[DBPL_BAUD_RATE_TRIGGER_TIME_MS_HI]<<8)|((uint16_t)pucData[DBPL_BAUD_RATE_TRIGGER_MS_LO]));

        if(DBPL_uBaudRate != (uint16_t)DBPL_uDefaultBaudRate)
        {
            DBPL_setBaudRateTimer(CONVERT_10MS_TO_1MS((uint32_t)time_trigger_10ms));
        }
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

/**Update service function for getting Reset Trigger Requests*/
static void DBPL_vResetTriggerRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_ResetTriggerRequestBytes{
        DBPL_RESET_TRIGGER_REQUEST_TIME,
        DBPL_RESET_TRIGGER_REQUEST_LEN
    };

    if(ucDataLen == (uint8_t)DBPL_RESET_TRIGGER_REQUEST_LEN)
    {
        DBPL_setResetTriggerTimer(CONVERT_10MS_TO_1MS((uint16_t)pucData[DBPL_RESET_TRIGGER_REQUEST_TIME]));
    }

    DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
}

#ifdef DBUS2_UPDATE_HSI
/**Update service function for getting HSI Protocol Requests*/
static void DBPL_vHsiProtocolRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_HsiProtocolRequestBytes{
        DBPL_HSI_PROTOCOL_REQUEST_SENDER,
        DBPL_HSI_PROTOCOL_REQUEST_OFFSET
    };

    uint32_t DBPL_ulHsiFrameLen = 0U;

    if(ucDataLen >= (uint8_t)DBPL_HSI_PROTOCOL_REQUEST_OFFSET)
    {
        DBPL_hsiBaudRateTime = DBPL_getBaudRateTimer();
        DBPL_ucHsiSenderAddress = pucData[DBPL_HSI_PROTOCOL_REQUEST_SENDER];
        DBPL_ulHsiFrameLen = (uint32_t)ucDataLen - (uint32_t)DBPL_HSI_PROTOCOL_REQUEST_OFFSET;
        DBPL_vHSI_DataIndication((void*)&pucData[DBPL_HSI_PROTOCOL_REQUEST_OFFSET], DBPL_ulHsiFrameLen);
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

/**Service function for sending HSI Response*/
void DBPL_vSendHsiProtocolResponse(const void* const vData, uint8_t ucDataLen)
{
    /*Cannot send answer longer than available buffer*/
    if(ucDataLen <= DBPL_ucSizeOfDataBufferIn_tBusMessage)
    {   /*Axivion Next Line MisraC2012-11.5 : cast tested to work correctly */
        const uint8_t *ucData = vData;
        DBPL_ucNumberOfBytesInMessage = ucDataLen;
        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_ucHsiSenderAddress; //Save the target address of the requesting communication partner.
        DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_HSI_PROTOCOL_RESPONSE;

        for (uint8_t index = 0; index < ucDataLen; index++)
        {
            DBPL_tMessageBuffer.aucData[index] = ucData[index];
        }

        DBPL_ucRepetitions = DBPL_UPDATE_MSG_SEND_RETRIES;
        DBPL_bSendMsg = true;
        DBPL_setBaudRateTimer(DBPL_hsiBaudRateTime);
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
    }
}
#endif

/**Update service function to return from silent mode*/
static void DBPL_vReturnFromSilentModeRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_ReturnFromSilentModeRequestBytes{
        DBPL_RETURN_FROM_SILENT_MODE_REQUEST_SENDER,
        DBPL_RETURN_FROM_SILENT_MODE_REQUEST_LEN
    };

    if(ucDataLen == (uint8_t)DBPL_RETURN_FROM_SILENT_MODE_REQUEST_LEN)
    {
        DLL_vUnsetSilentMode();
        DBPL_ucNumberOfBytesInMessage = 0U;
        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_RETURN_FROM_SILENT_MODE_REQUEST_SENDER]; //Save the target address of the requesting communication partner.
        DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_RETURN_FROM_SILENT_MODE_RESPONSE;
        DBPL_ucRepetitions = DBPL_UPDATE_MSG_SEND_RETRIES;
        DBPL_bSendMsg = true;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

/**Service function to send the identification and version of SW and HW*/
static void DBPL_vEcuConfigReadRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    /*lint -esym(749,DBPL_EcuConfigRead*) "local enumeration constant not referenced [MISRA 2012 Rule 2.5, advisory]" */
    enum DBPL_EcuConfigReadRequestBytes{
        DBPL_ECU_CONFIG_READ_REQUEST_SENDER,
        DBPL_ECU_CONFIG_READ_REQUEST_OBJ_ID,
        DBPL_ECU_CONFIG_READ_REQUEST_LEN
    };

    enum DBPL_EcuConfigReadResponseBytes{
        DBPL_ECU_CONFIG_READ_RESPONSE_SENDER,
        DBPL_ECU_CONFIG_READ_RESPONSE_STATUS,
        DBPL_ECU_CONFIG_READ_RESPONSE_OBJ_ID_NUMBER,
        DBPL_ECU_CONFIG_READ_RESPONSE_OBJ_COUNT,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_1,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_2,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_3,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_4,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_5,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_6,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_7,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_8,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_MAJ_HI,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_MAJ_LO,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_MIN_HI,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_MIN_LO,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_REV_HI,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_REV_LO,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_BUILD_HH,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_BUILD_HL,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_BUILD_LH,
        DBPL_ECU_CONFIG_READ_RESPONSE_HW_V_BUILD_LL,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_1,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_2,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_3,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_4,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_5,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_6,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_7,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_8,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_MAJ_HI,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_MAJ_LO,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_MIN_HI,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_MIN_LO,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_REV_HI,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_REV_LO,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_BUILD_HH,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_BUILD_HL,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_BUILD_LH,
        DBPL_ECU_CONFIG_READ_RESPONSE_SW_V_BUILD_LL,
        DBPL_ECU_CONFIG_READ_RESPONSE_LEN
    };

    struct DBPL_EcuConfigReadResult readResult;

    if(ucDataLen == (uint8_t)DBPL_ECU_CONFIG_READ_REQUEST_LEN)
    {
        DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_ECU_CONFIG_READ_RESPONSE_LEN;
        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_ECU_CONFIG_READ_REQUEST_SENDER]; //Save the target address of the requesting communication partner.
        DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_ECU_CONFIG_READ_RESPONSE;
        readResult = DBPL_tGetEcuConfigReadResult(pucData[DBPL_ECU_CONFIG_READ_REQUEST_OBJ_ID]);

        DBPL_tMessageBuffer.aucData[DBPL_ECU_CONFIG_READ_RESPONSE_SENDER] = DBPL_ucNodeAddrToRespondFrom;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_CONFIG_READ_RESPONSE_STATUS] = (uint8_t)readResult.status;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_CONFIG_READ_RESPONSE_OBJ_ID_NUMBER] = readResult.objIdNumber;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_CONFIG_READ_RESPONSE_OBJ_COUNT] = readResult.objCount;

        for (uint8_t index = 0; index < sizeof(struct STDV_version); index++)
        {   /*lint -save -e928 "cast from pointer to pointer [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]" tested to work correctly */
            DBPL_tMessageBuffer.aucData[((uint8_t)DBPL_ECU_CONFIG_READ_RESPONSE_HW_ID_1) + index] = ((uint8_t*)&(readResult.identificationObject.hw_version))[index];
            DBPL_tMessageBuffer.aucData[((uint8_t)DBPL_ECU_CONFIG_READ_RESPONSE_SW_ID_1) + index] = ((uint8_t*)&(readResult.identificationObject.sw_version))[index];
            /*lint -restore -e928 */
        }
        
        DBPL_ucRepetitions = DBPL_UPDATE_MSG_SEND_RETRIES;
        DBPL_bSendMsg = true;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

/**Service function to send the identification and version of selected SW Submodule */
static void DBPL_vEcuSwSubmoduleReadRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    /*lint -esym(749,DBPL_vEcuSwSubmoduleRead*) "local enumeration constant not referenced [MISRA 2012 Rule 2.5, advisory]" */
    enum DBPL_vEcuSwSubmoduleReadRequestBytes{
        DBPL_ECU_SW_SUBMODULE_READ_REQUEST_SENDER,
        DBPL_ECU_SW_SUBMODULE_READ_REQUEST_SW_ID,
        DBPL_ECU_SW_SUBMODULE_READ_REQUEST_OBJ_NUM = 9,
        DBPL_ECU_SW_SUBMODULE_READ_REQUEST_LEN = 10
    };

    enum DBPL_vEcuSwSubmoduleReadResponseBytes{
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SENDER,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_STATUS,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_OBJ_ID_NUMBER,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_OBJ_COUNT,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_1,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_2,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_3,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_4,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_5,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_6,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_7,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_8,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_MAJ_HI,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_MAJ_LO,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_MIN_HI,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_MIN_LO,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_REV_HI,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_REV_LO,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_BUILD_HH,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_BUILD_HL,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_BUILD_LH,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_V_BUILD_LL,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_1,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_2,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_3,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_4,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_5,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_6,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_7,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_8,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_MAJ_HI,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_MAJ_LO,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_MIN_HI,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_MIN_LO,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_REV_HI,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_REV_LO,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_BUILD_HH,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_BUILD_HL,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_BUILD_LH,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_V_BUILD_LL,
        DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_LEN
    };

    struct DBPL_SwSubmoduleReadResult readResult;

    if(ucDataLen == (uint8_t)DBPL_ECU_SW_SUBMODULE_READ_REQUEST_LEN)
    {
        DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_LEN;
        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_ECU_SW_SUBMODULE_READ_REQUEST_SENDER]; //Save the target address of the requesting communication partner.
        DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_ECU_SW_SUBMODULE_READ_RESPONSE;
        readResult = DBPL_tGetSwSubmoduleReadResult(&pucData[DBPL_ECU_SW_SUBMODULE_READ_REQUEST_SW_ID], pucData[DBPL_ECU_SW_SUBMODULE_READ_REQUEST_OBJ_NUM]);

        DBPL_tMessageBuffer.aucData[DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SENDER] = DBPL_ucNodeAddrToRespondFrom;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_STATUS] = (uint8_t)readResult.status;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_OBJ_ID_NUMBER] = readResult.objIdNumber;
        DBPL_tMessageBuffer.aucData[DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_OBJ_COUNT] = readResult.objCount;

        for (uint8_t index = 0; index < sizeof(struct STDV_version); index++)
        {   /*lint -save -e928 "cast from pointer to pointer [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]" tested to work correctly */
            DBPL_tMessageBuffer.aucData[((uint8_t)DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_ID_1) + index] = ((uint8_t*)&(readResult.identificationObject.sw_version))[index];
            DBPL_tMessageBuffer.aucData[((uint8_t)DBPL_ECU_SW_SUBMODULE_READ_RESPONSE_SW_SUBMODULE_ID_1) + index] = ((uint8_t*)&(readResult.identificationObject.sw_submodule_version))[index];
            /*lint -restore -e928 */
        }
        
        DBPL_ucRepetitions = DBPL_UPDATE_MSG_SEND_RETRIES;
        DBPL_bSendMsg = true;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}

#if ((defined DBUS2_UPDATE_HSI) || (defined DBUS2_PROD_MSG_IN_APP))
/*Production Service Message to write HW part of ECU Config*/
static void DBPL_vWriteEcuConfigHwRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_WriteEcuConfigHardwareResponseBytes{
        DBPL_WRITE_ECU_CONFIG_HW_RESPONSE_STATUS,
        DBPL_WRITE_ECU_CONFIG_HW_RESPONSE_LEN
    };

    DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_WRITE_ECU_CONFIG_HW_RESPONSE_LEN;
    DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_PRODUCTION_RESPONSE_ADDRESS;
    DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_WRITE_ECU_CONFIG_HW_RESPONSE;

    if(ucDataLen == (uint8_t)sizeof(struct STDV_version))
    {   /*Axivion Next Line MisraC2012-11.5 : cast tested to work correctly */
        DBPL_tMessageBuffer.aucData[DBPL_WRITE_ECU_CONFIG_HW_RESPONSE_STATUS] = (uint8_t)DBPL_tWriteEcuConfigHw((const void*)pucData);
    }
    else
    {
        DBPL_tMessageBuffer.aucData[DBPL_WRITE_ECU_CONFIG_HW_RESPONSE_STATUS] = (uint8_t)DBPL_ProductionStatusLengthError;
    }

    DBPL_ucRepetitions = DBPL_PRODUCTION_MSG_SEND_RETRIES;
    DBPL_bSendMsg = true;
    DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
}

/*Production Service Message To write Tracing Id*/
static void DBPL_vWriteTracingIdRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_WriteTracingIdResponseBytes{
        DBPL_WRITE_TRACING_ID_RESPONSE_STATUS,
        DBPL_WRITE_TRACING_ID_RESPONSE_LEN
    };

    DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_WRITE_TRACING_ID_RESPONSE_LEN;
    DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_PRODUCTION_RESPONSE_ADDRESS;
    DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_WRITE_TRACING_ID_RESPONSE;

    if(ucDataLen == (uint8_t)sizeof(struct DBPL_TracingId))
    {   /*Axivion Next Line MisraC2012-11.5 : cast tested to work correctly */
        DBPL_tMessageBuffer.aucData[DBPL_WRITE_TRACING_ID_RESPONSE_STATUS] = (uint8_t)DBPL_tWriteTracingId((const void*)pucData);
    }
    else
    {
        DBPL_tMessageBuffer.aucData[DBPL_WRITE_TRACING_ID_RESPONSE_STATUS] = (uint8_t)DBPL_ProductionStatusLengthError;
    }

    DBPL_ucRepetitions = DBPL_PRODUCTION_MSG_SEND_RETRIES;
    DBPL_bSendMsg = true;
    DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
}

/*Production Service Message To write Production Time*/
static void DBPL_vWriteProductionTimeRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_WriteProductionTimeResponseBytes{
        DBPL_WRITE_PRODUCTION_TIME_RESPONSE_STATUS = 0U,
        DBPL_WRITE_PRODUCTION_TIME_RESPONSE_TIME_OFFSET = 1U,
        DBPL_WRITE_PRODUCTION_TIME_RESPONSE_LEN = 11U
    };

    struct DBPL_ProductionTime writtenTime = {.date = {0}, .clock = {0}};

    DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_WRITE_PRODUCTION_TIME_RESPONSE_LEN;
    DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_PRODUCTION_RESPONSE_ADDRESS;
    DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_PROD_TIME_RESPONSE;

    if(ucDataLen == (uint8_t)sizeof(struct DBPL_ProductionTime))
    {   /*Axivion Next Line MisraC2012-11.5 : cast tested to work correctly */
        DBPL_tMessageBuffer.aucData[DBPL_WRITE_PRODUCTION_TIME_RESPONSE_STATUS] = (uint8_t)DBPL_tWriteProductionTime((const void*)pucData, &writtenTime); /*lint !e934 "Taking address of near auto variable". Here no problem*/
    }
    else
    {
        DBPL_tMessageBuffer.aucData[DBPL_WRITE_PRODUCTION_TIME_RESPONSE_STATUS] = (uint8_t)DBPL_ProductionStatusLengthError;
    }

    for(uint8_t index = 0; index < (uint8_t)sizeof(struct DBPL_ProductionTime); index++)
    {
        DBPL_tMessageBuffer.aucData[((uint8_t)DBPL_WRITE_PRODUCTION_TIME_RESPONSE_TIME_OFFSET) + index] = ((uint8_t*)&(writtenTime))[index]; /*lint !e928 "cast from pointer to pointer". Both types are in fact, byte arrays, so not critical.*/
    }

    DBPL_ucRepetitions = DBPL_PRODUCTION_MSG_SEND_RETRIES;
    DBPL_bSendMsg = true;
    DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
}

#if defined (DBUS2_APPLIANCE_MSG_IN_APP)
/* Service Message to write Appliance Data Item */
static void DBPL_vWriteApplianceDataRequest(uint8_t ucDataLen, uint8_t* pucData)
{
    enum DBPL_WriteApplianceDataRequestBytes{
        DBPL_WRITE_APPLIANCE_DATA_REQUEST_SENDER,
        DBPL_WRITE_APPLIANCE_DATA_REQUEST_PARAMETER_ID,
        DBPL_WRITE_APPLIANCE_DATA_REQUEST_DATA_OFFSET
    };

    enum DBPL_WriteApplianceDataResponseBytes{
        DBPL_WRITE_APPLIANCE_DATA_RESPONSE_PARAMETER_ID,
        DBPL_WRITE_APPLIANCE_DATA_RESPONSE_STATUS,
        DBPL_WRITE_APPLIANCE_DATA_RESPONSE_LEN
    };
    enum DBPL_ApplianceDataStatus retStatus;
    enum DBPL_ApplianceDataItemId parameterID;
    uint8_t ucDataLength;

    parameterID = DBPL_GetParameterID(pucData[DBPL_WRITE_APPLIANCE_DATA_REQUEST_PARAMETER_ID]);
    if (parameterID < DBPL_APPLIANCEDATA_COUNT)
    {
    /* calculation of maximal data length - it depends of parameterID which belongs to particular appliance data item
     * the data length can be: 2 bytes (parameterID and sender) + maximum size of item */
        ucDataLength = (uint8_t)DBPL_WRITE_APPLIANCE_DATA_REQUEST_DATA_OFFSET + DBPL_GetApplianceDataMaxItemSize(parameterID);
        DBPL_tMessageBuffer.aucData[DBPL_WRITE_APPLIANCE_DATA_RESPONSE_PARAMETER_ID] = pucData[DBPL_WRITE_APPLIANCE_DATA_REQUEST_PARAMETER_ID];
        /*test if length of dbus data in message is equal to maximal allowed length - allowed only for 1-byte item type */
        if(ucDataLen <= ucDataLength)
        {
            retStatus = DBPL_tWriteApplianceData(parameterID,
                                                 &pucData[DBPL_WRITE_APPLIANCE_DATA_REQUEST_DATA_OFFSET],
                                                 (ucDataLen - (uint8_t)DBPL_WRITE_APPLIANCE_DATA_REQUEST_DATA_OFFSET));/*lint !e9034 !e9030 Impermissible cast and swap of types needed here and works.*/
        }
        else
        {
            /*length of dbus data in message exceeds maximal allowed length for particular parameterID number */
            retStatus = DBPL_ApplianceDataLengthError;
        }
    }
    else
    {
        retStatus = DBPL_ApplianceDataUnknownParamterIdError;
    }
    DBPL_tMessageBuffer.aucData[DBPL_WRITE_APPLIANCE_DATA_RESPONSE_STATUS] = (uint8_t) retStatus;
    DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_WRITE_APPLIANCE_DATA_RESPONSE_LEN;
    DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_WRITE_APPLIANCE_DATA_REQUEST_SENDER];
    DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_WRITE_APPLIANCE_DATA_RESPONSE;
    DBPL_ucRepetitions = DBPL_APPLIANCE_DATA_MSG_SEND_RETRIES;
    DBPL_bSendMsg = true;
    DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
}


/* Service Message to read Appliance Data Item */
static void DBPL_vReadApplianceDataRequest(uint8_t ucDataLen, uint8_t* pucData)
{
    enum DBPL_ReadApplianceDataRequestBytes{
        DBPL_READ_APPLIANCE_DATA_REQUEST_SENDER,
        DBPL_READ_APPLIANCE_DATA_REQUEST_PARAMETER_ID,
        DBPL_READ_APPLIANCE_DATA_REQUEST_LEN
    };

    enum DBPL_ReadApplianceDataResponseBytes{
        DBPL_READ_APPLIANCE_DATA_RESPONSE_SENDER_ID,
        DBPL_READ_APPLIANCE_DATA_RESPONSE_PARAMETER_ID,
        DBPL_READ_APPLIANCE_DATA_RESPONSE_STATUS,
        DBPL_READ_APPLIANCE_DATA_RESPONSE_DATA_OFFSET
    };
    uint8_t ucDataLength;
    enum DBPL_ApplianceDataItemId parameterID;
    enum DBPL_ApplianceDataStatus retStatus;
    parameterID = DBPL_GetParameterID(pucData[DBPL_READ_APPLIANCE_DATA_REQUEST_PARAMETER_ID]);

    if(ucDataLen == (uint8_t)DBPL_READ_APPLIANCE_DATA_REQUEST_LEN)
    {
        /* checking if parameterID has correct value  */
        if (parameterID < DBPL_APPLIANCEDATA_COUNT)
        {
            /* first parameter is parameterID and it will be checked for correct range in the function itself */
            retStatus = DBPL_tGetApplianceDataReadResult(parameterID,
                        &DBPL_tMessageBuffer.aucData[DBPL_READ_APPLIANCE_DATA_RESPONSE_DATA_OFFSET],
                        &ucDataLength);
            DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_READ_APPLIANCE_DATA_RESPONSE_DATA_OFFSET + ucDataLength;
        }
        else
        {
            retStatus = DBPL_ApplianceDataUnknownParamterIdError;
            DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_READ_APPLIANCE_DATA_RESPONSE_DATA_OFFSET;
            DLL_vReleaseDataFromReceivedMessage();
        }
        DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = pucData[DBPL_READ_APPLIANCE_DATA_REQUEST_SENDER]; //Save the target address of the requesting communication partner.
        DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_READ_APPLIANCE_DATA_RESPONSE;

        DBPL_tMessageBuffer.aucData[DBPL_READ_APPLIANCE_DATA_RESPONSE_SENDER_ID] = DBPL_ucNodeAddrToRespondFrom;
        DBPL_tMessageBuffer.aucData[DBPL_READ_APPLIANCE_DATA_RESPONSE_PARAMETER_ID] = (uint8_t) pucData[DBPL_READ_APPLIANCE_DATA_REQUEST_PARAMETER_ID];
        DBPL_tMessageBuffer.aucData[DBPL_READ_APPLIANCE_DATA_RESPONSE_STATUS] = (uint8_t) retStatus; //lint !e9034 Expression assigned to a narrower or different essential type
        DBPL_ucRepetitions = DBPL_APPLIANCE_DATA_MSG_SEND_RETRIES;
        DBPL_bSendMsg = true;
        DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
    }
    else
    {
        DLL_vReleaseDataFromReceivedMessage(); //Data has been picked up, new incoming data may be saved in the buffer.
    }
}
#endif
#endif /* ((defined DBUS2_UPDATE_HSI) || (defined DBUS2_PROD_MSG_IN_APP)) */

/*Production Service Message To read Production Time*/
static void DBPL_vReadProductionTimeRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_ReadProductionTimeResponseBytes{
        DBPL_READ_PRODUCTION_TIME_RESPONSE_STATUS = 0U,
        DBPL_READ_PRODUCTION_TIME_RESPONSE_TIME_OFFSET = 1U,
        DBPL_READ_PRODUCTION_TIME_RESPONSE_LEN = 11U
    };

    struct DBPL_ProductionTime readTime = {.date = {0}, .clock = {0}};

    DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_READ_PRODUCTION_TIME_RESPONSE_LEN;
    DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_PRODUCTION_RESPONSE_ADDRESS;
    DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_PROD_TIME_RESPONSE;

    if(ucDataLen == 0U)
    {
        DBPL_tMessageBuffer.aucData[DBPL_READ_PRODUCTION_TIME_RESPONSE_STATUS] = (uint8_t)DBPL_tReadProductionTime(&readTime); /*lint !e934 "Taking address of near auto variable". Here no problem*/
    }
    else
    {
        DBPL_tMessageBuffer.aucData[DBPL_READ_PRODUCTION_TIME_RESPONSE_STATUS] = (uint8_t)DBPL_ProductionStatusLengthError;
        (void)*pucData;
    }

    for(uint8_t index = 0; index < (uint8_t)sizeof(struct DBPL_ProductionTime); index++)
    {
        DBPL_tMessageBuffer.aucData[((uint8_t)DBPL_READ_PRODUCTION_TIME_RESPONSE_TIME_OFFSET) + index] = ((uint8_t*)&(readTime))[index]; /*lint !e928 "cast from pointer to pointer". Both types are in fact, byte arrays, so not critical.*/
    }

    DBPL_ucRepetitions = DBPL_PRODUCTION_MSG_SEND_RETRIES;
    DBPL_bSendMsg = true;
    DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
}

static void DBPL_vWriteTestStateRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_WriteTestStateRequestBytes{
        DBPL_WRITE_TEST_STATE_REQUEST_TEST_STATE,
        DBPL_WRITE_TEST_STATE_REQUEST_LEN
    };

    enum DBPL_WriteTestStateRepairCntResponseBytes{
        DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_STATUS,
        DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_TEST_STATE,
        DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_REPAIR_CNT,
        DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_LEN
    };

    DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_LEN;
    DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_PRODUCTION_RESPONSE_ADDRESS;
    DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_TESTST_REPAIRCNT_RESPONSE;

    enum DBPL_TestState readState = DBPL_TestStateFailed;
    uint8_t repairCnt = DBPL_INVALID_REPAIR_CNT;

    if(ucDataLen == (uint8_t)DBPL_WRITE_TEST_STATE_REQUEST_LEN)
    {
        if(pucData[DBPL_WRITE_TEST_STATE_REQUEST_TEST_STATE] < (uint8_t)DBPL_TestStateCnt)
        {
            enum DBPL_TestState receivedState = (enum DBPL_TestState)pucData[DBPL_WRITE_TEST_STATE_REQUEST_TEST_STATE];/*lint !e9030 !e9034 , Value check in line above makes sure this operation is safe.*/
            DBPL_tMessageBuffer.aucData[DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_STATUS] = (uint8_t)DBPL_tWriteTestState(receivedState, &readState, &repairCnt); /*lint !e934 "Taking address of near auto variable". Here no problem*/
        }
        else
        {
            DBPL_tMessageBuffer.aucData[DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_STATUS] = (uint8_t)DBPL_ProductionStatusParam1Error;
        }
    }
    else
    {
        DBPL_tMessageBuffer.aucData[DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_STATUS] = (uint8_t)DBPL_ProductionStatusLengthError;
    }

    DBPL_tMessageBuffer.aucData[DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_TEST_STATE] = (uint8_t)readState;
    DBPL_tMessageBuffer.aucData[DBPL_WRITE_TEST_STATE_REPAIR_CNT_RESPONSE_REPAIR_CNT] = repairCnt;
    DBPL_ucRepetitions = DBPL_PRODUCTION_MSG_SEND_RETRIES;
    DBPL_bSendMsg = true;
    DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
}

static void DBPL_vReadTestStateRepairCntRequest(uint8_t ucDataLen, uint8_t *pucData)
{
    enum DBPL_ReadTestStateRepairCntResponseBytes{
        DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_STATUS,
        DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_TEST_STATE,
        DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_REPAIR_CNT,
        DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_LEN
    };

    DBPL_ucNumberOfBytesInMessage = (uint8_t)DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_LEN;
    DBPL_tMessageBuffer.tBusIdentifier.ucTargetAddress = DBPL_PRODUCTION_RESPONSE_ADDRESS;
    DBPL_tMessageBuffer.tBusIdentifier.tMessageIdentifier = MSG_TESTST_REPAIRCNT_RESPONSE;

    enum DBPL_TestState readState = DBPL_TestStateFailed;
    uint8_t repairCnt = DBPL_INVALID_REPAIR_CNT;

    if(ucDataLen == 0U)
    {
        DBPL_tMessageBuffer.aucData[DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_STATUS] = (uint8_t)DBPL_tReadTestStateRepairCnt(&readState, &repairCnt); /*lint !e934 "Taking address of near auto variable". Here no problem*/
    }
    else
    {
        DBPL_tMessageBuffer.aucData[DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_STATUS] = (uint8_t)DBPL_ProductionStatusLengthError;
        (void)*pucData;
    }

    DBPL_tMessageBuffer.aucData[DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_TEST_STATE] = (uint8_t)readState;
    DBPL_tMessageBuffer.aucData[DBPL_READ_TEST_STATE_REPAIR_CNT_RESPONSE_REPAIR_CNT] = repairCnt;
    DBPL_ucRepetitions = DBPL_PRODUCTION_MSG_SEND_RETRIES;
    DBPL_bSendMsg = true;
    DBR_RunEventdrivenDbusTask( DBR_ED_TASK_DBPL_TX );
}

static bool DBPL_bIsSendingPowerMgmtMsg(void)
{
    return ((DBPL_bIsSendingPowerMgmtWakeup() == true) || (DBPL_bIsSendingPowerMgmtWakeupComplete() == true) || (DBPL_bIsSendingPowerMgmtNodeReset() == true)) ? true : false;
}

/** \brief Function responsible for initialization of SW timers used by the DBPL layer.*/
static void DBPL_initTimers(void)
{
    static struct STDCB_Callback offlineTimerCallback;          //!< Callback instance used by GoOffline message handler, resets MCU after defined time

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
    static struct STDCB_Callback baudRateTimerCallback;         //!< Callback instance used by Baudrate transition message handler, resets DBus baudrate after defined time
    static struct STDCB_Callback resetTriggerTimerCallback;     //!< Callback instance used by ResetTrigger message handler, activates firmware update after defined time

    // Timer instance used for tracking Baudrate Timeout event. 
    /*NOTE: Baudrate timer is started during the boot because non-default
        baudrate might be requested from BMDAT_sharedData (DBus runs in High Speed). */
    (void)STIM_InitCallback(&baudRateTimerCallback, DBPL_baudRateTimerElapsed, NULL, STIM_STATUS_TRIGGERED);
    (void)STIM_InitTimer(&DBPL_baudRateTimer, STIM_PROCESSING_SCHEDULER,
            (uint32_t)CONVERT_10MS_TO_1MS(DBPL_STANDARD_BAUD_RESET_TIME_10MS), STIM_MODE_SINGLE, true, &baudRateTimerCallback);

    // Timer instance used for delayed reset from silent mode / update mode (remote firmware update / FWU3).
    (void)STIM_InitCallback(&resetTriggerTimerCallback, DBPL_resetTriggerTimerElapsed, NULL, STIM_STATUS_TRIGGERED);
    (void)STIM_InitTimer(&DBPL_resetTriggerTimer, STIM_PROCESSING_SCHEDULER,
            (uint32_t)0u, STIM_MODE_SINGLE, false, &resetTriggerTimerCallback);
#endif /* DBUS2_UPDATE || DBUS2_UPDATE_HSI */

    // Timer instance used for delayed reset from offline mode / (possible) jump to update mode (wired firmware update / FWU1).
    (void)STIM_InitCallback(&offlineTimerCallback, DBPL_offlineTimerElapsed, NULL, STIM_STATUS_TRIGGERED);
    (void)STIM_InitTimer(&DBPL_tOfflineDelayTimer, STIM_PROCESSING_SCHEDULER, 
            (uint32_t)0u, STIM_MODE_SINGLE, false, &offlineTimerCallback);
}

/** \brief Sets the reset timer instance. Reset to Bootloader (FWU1) is executed after timer overflow.
 *
 * \param resetDelay = delay represented in ms.
*/
static void DBPL_setResetTimer(uint16_t resetDelay)
{
    if(resetDelay != 0u)
    {
        /* OfflineDelay timer can be shared, because GoOffline event and 
            Reset delay event can't occur at the same time, see DBPL_vGoOfflineService() */
        STIM_ReloadTimer(&DBPL_tOfflineDelayTimer, (uint32_t)resetDelay);
        STIM_EnableTimer(&DBPL_tOfflineDelayTimer);
    }
    else
    {   // no delay required, execute action immediately
        STIM_DisableTimer(&DBPL_tOfflineDelayTimer);
        (void)STDCB_ExecuteCallbacks(DBPL_tOfflineDelayTimer.cbList, (uint32_t)STIM_STATUS_TRIGGERED, 0);
    }
}

/** \brief Sets the offline timer instance. Standard reset is executed after timer overflow.
 *
 * \param offlineTime = delay represented in seconds.
*/
static void DBPL_setOfflineTimer(uint16_t offlineTime)
{
    /* OfflineDelay timer can be shared, because GoOffline event and 
        Reset delay event can't occur at the same time, see DBPL_vGoOfflineService() */
    STIM_ReloadTimer(&DBPL_tOfflineDelayTimer, STIM_TIME_SEC((uint32_t)offlineTime));
    STIM_EnableTimer(&DBPL_tOfflineDelayTimer);
}

/** \brief Sets baud rate timer. Switch to default baudrate is executed after timer overflow.
 *
 * \param baudRateTime = delay represented in ms.
*/
static void DBPL_setBaudRateTimer(uint32_t baudRateTime)
{
    if(baudRateTime != 0u)
    {
        STIM_ReloadTimer(&DBPL_baudRateTimer, (uint32_t)baudRateTime);
        STIM_EnableTimer(&DBPL_baudRateTimer);
    }
    else
    {   // no delay required, execute action immediately
        STIM_DisableTimer(&DBPL_baudRateTimer);
        (void)STDCB_ExecuteCallbacks(DBPL_baudRateTimer.cbList, (uint32_t)STIM_STATUS_TRIGGERED, 0);
    }
}

#ifdef DBUS2_UPDATE_HSI
/** \brief Gets the current value of baud rate timer, in units of 1 ms.*/
static uint16_t DBPL_getBaudRateTimer(void)
{
    return (uint16_t)STIM_ReadTimer(&DBPL_baudRateTimer);
}
#endif /* DBUS2_UPDATE_HSI */

/** \brief Sets reset trigger timer. Reset to user application is executed after timer overflow.
 *
 * \param resetTriggerTime = delay represented in ms.
*/
static void DBPL_setResetTriggerTimer(uint16_t resetTriggerTime)
{
    if(resetTriggerTime != 0u)
    {
        STIM_ReloadTimer(&DBPL_resetTriggerTimer, (uint32_t)resetTriggerTime);
        STIM_EnableTimer(&DBPL_resetTriggerTimer);
    }
    else
    {   // no delay required, execute action immediately
        STIM_DisableTimer(&DBPL_resetTriggerTimer);
        (void)STDCB_ExecuteCallbacks(DBPL_resetTriggerTimer.cbList, (uint32_t)STIM_STATUS_TRIGGERED, 0);
    }
}

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
/** \brief Callback function: Switch to default baudrate after timer overflow. */
static int32_t DBPL_baudRateTimerElapsed(void *obj, uint32_t flags, int32_t eventData)
{
    //only to prevent lint warnings
    (void)obj; (void)flags; (void)eventData;
    
    DBPL_uBaudRate = (uint16_t)DBPL_uDefaultBaudRate;
    DBPL_vConfigureBaudrate(DBPL_uBaudRate);
    return 0;
}

/** \brief Callback function: Reset from programmer/silent mode to user app after timer overflow */
static int32_t DBPL_resetTriggerTimerElapsed(void *obj, uint32_t flags, int32_t eventData)
{
    //only to prevent lint warnings
    (void)obj; (void)flags; (void)eventData;
    
    DBM_UART_vEnableRxEvent();
    // reset to user app with default baudrate
    DBPL_vLeaveUpdateMode();
    return 0;
}
#endif /* DBUS2_UPDATE || DBUS2_UPDATE_HSI */

/** \brief Callback function: Switch to default baudrate after timer overflow. */
static int32_t DBPL_offlineTimerElapsed(void *obj, uint32_t flags, int32_t eventData)
{
    //only to prevent lint warnings
    (void)obj; (void)flags; (void)eventData;
    
    #if defined(MCAL_MSUP_INCLUDED)
        MSUP_generateReset();
    #else
        HSUP_vGenerateReset();
    #endif
    
    return 0;
}
