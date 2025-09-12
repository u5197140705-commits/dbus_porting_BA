/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          DBusExample
*   COMP_ABBREV      DBEx
*   AUTHOR           Tomas Hrasok
*   CREATED          26.04.2021
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Implementation of DBus_user_interface
*
*   \details  DBus interface based on user defined receive + transmit tables
*
*/

#ifndef DBUS_USER_INTERFACE
#define DBUS_USER_INTERFACE

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "libtypes.h"
#include "processor.h"
#include "bustypes.h"

/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/* DBEx subsystem number - used in balXs.c in BAL_tBusObject[] and during DBus message transmission */
#define LED_CONTROL_SUBSYSTEM_ID  0x06U


/* DBus receive message IDs - unique for every message in communication system */
#define DBEX_MSGID_TOGGLE_LED1_REQUEST      0x6000
#define DBEX_MSGID_TOGGLE_LED2_REQUEST      0x6004


/* DBus transmit message IDs - unique for every message in communication system  */
#define DBEX_MSGID_TOGGLE_LED1_RESP         0x6001
#define DBEX_MSGID_TOGGLE_LED2_RESP         0x6005

/* DBus configuration table declarations */
extern const TbusReceiveObject   ptDBExReceiveTable[];
extern const TbusTransmitObject  ptDBExTransmitTable[];
extern const unsigned char       DBEx_NumberOfElementsInSubsystem;
extern       unsigned char       DBEx_TransmitFlagSubsystem[];

// DBus request types
enum{
    DBEX_LED_REQ_SA,              //0 Source Address
    DBEX_LED_REQ_STATE,           //1 LED State
    DBEX_LED_REQ_MAX_LEN
};

/* Message Indexes used by BAL_vTransmitMessage(), have to be aligned with transmit table */
enum {
	DBEX_TXID_MSG_SETLED1_RESP,
    DBEX_TXID_MSG_SETLED2_RESP,
	DBEX_TXID_COUNT
};

/* Length of response messages - defines how many user bytes (payload) will be transmitted */
#define DBEX_LEN_LED1_RESP 		2U
#define DBEX_LEN_LED2_RESP 		2U

/* Global variables to store required LED states*/
extern bool DBEx_LED1_requested_state;
extern bool DBEx_LED2_requested_state;

/* Receive functions - called when DBus message is received */
void DBEx_vToggleLed1Request(uint8_t ucDataLen, const uint8_t* pucData);
void DBEx_vToggleLed2Request(uint8_t ucDataLen, const uint8_t* pucData);

/* Transmit functions - called when user transmit data to DBus via function BAL_vTransmitMessage() */
static void DBEx_vToggleLED1Resp(uint8_t dataLen, uint8_t *p_Data);
static void DBEx_vToggleLED2Resp(uint8_t dataLen, uint8_t *p_Data);

/* Confirmation functions - called once transmission of user message is finished */
static void DBEx_vToggleLED1RespConfirm(void);
static void DBEx_vToggleLED2RespConfirm(void);


#ifdef __cplusplus
}
#endif

#endif // DBUS_USER_INTERFACE
