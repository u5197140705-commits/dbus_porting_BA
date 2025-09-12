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

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "GPIO_control\LED_control.h"
#include "DBus_user_interface\DBus_user_interface.h"
#include "bal.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                              */
/******************************************************************************/
/* Receive functions */
void DBEx_vToggleLed1Request(uint8_t ucDataLen, const uint8_t* pucData);
void DBEx_vToggleLed2Request(uint8_t ucDataLen, const uint8_t* pucData);

/* Transmit functions */
static void DBEx_vToggleLED1Resp(uint8_t dataLen, uint8_t *p_Data);
static void DBEx_vToggleLED2Resp(uint8_t dataLen, uint8_t *p_Data);

/* Confirmation functions */
static void DBEx_vToggleLED1RespConfirm(void);
static void DBEx_vToggleLED2RespConfirm(void);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

/* Global variables to store required LED states*/
bool DBEx_LED1_requested_state;
bool DBEx_LED2_requested_state;

/*--------------------------------------------------------------------------*/
/*          tables for reception (accessed from balXs.c                     */
/*--------------------------------------------------------------------------*/

// see TbusReceiveObject in bustypes.h for more info about receive table parameters
const TbusReceiveObject ptDBExReceiveTable[] =
{  /* ucMsg Length,     ucTargetAddr,   tMessageIdentifier,                      tServiceFunction  */
    {{0x00U,            0x00U,          DBEX_MSGID_TOGGLE_LED1_REQUEST},         (TbusService)DBEx_vToggleLed1Request},
    {{MSB_BYTE,         0x00U,          DBEX_MSGID_TOGGLE_LED2_REQUEST},         (TbusService)DBEx_vToggleLed2Request}
}; /* last element (marked with MSB in MsgLen set) */


/*--------------------------------------------------------------------------*/
/*          tables for transmission (accessed from balXs.c                  */
/*--------------------------------------------------------------------------*/

// see TbusTransmitObject in bustypes.h for more info about transmit table parameters
const TbusTransmitObject ptDBExTransmitTable[] =
{/* Length,     TargetAddr, tMessageIdentifier,             ucDataLen,              tServiceFunction,       tConfirmationFunction */
    {{0x01,     0xC0,       DBEX_MSGID_TOGGLE_LED1_RESP},   DBEX_LEN_LED1_RESP,     DBEx_vToggleLED1Resp,   DBEx_vToggleLED1RespConfirm},
    {{0x01,     0xC0,       DBEX_MSGID_TOGGLE_LED2_RESP},   DBEX_LEN_LED2_RESP,     DBEx_vToggleLED2Resp,   DBEx_vToggleLED2RespConfirm}
 };


/* number of messages defined in the corresponding transmit table */
const uint8_t DBEx_NumberOfElementsInSubsystem = DBEX_TXID_COUNT;

/* The transmit flags consist of one bit for each message,
   keeping information about which messages are due for transmission
   (bit set: corresponding message is due for transmission).
   A subsystem with up to 8 messages needs one byte for the transmit flags.
   Subsystems with up to 16 messages needs two bytes, etc.
   Bit number 0 represents the first message in a subsystem
   (index 0 in the transmission table).
*/
uint8_t DBEx_TransmitFlagSubsystem[((DBEX_TXID_COUNT - 1)/8)+1];/*lint !e778 suppress err just here */


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/* Receive functions - called when DBus message is received */
void DBEx_vToggleLed1Request(uint8_t ucDataLen, const uint8_t* pucData)
{
    //check message length 
    if(ucDataLen == (uint8_t)DBEX_LED_REQ_MAX_LEN)
    {
        // set state of LED
        (void)DBEx_SetLED(RECEIVE_LED1, pucData[DBEX_LED_REQ_STATE]);
        
        DBEx_LED1_requested_state = pucData[DBEX_LED_REQ_STATE];

        /*Send DBus response to currently handled LED toggle request
            - first parameter - Subsystem ID - present in balXs.c in BAL_tBusObject[] - this defines which transmit table from BAL_tBusObject should be used
            - second parameter - Message ID - present in DBus_user_interface.h as Message Indexes - this defines which message should be transmitted from transmit table */
        BAL_vTransmitMessage(LED_CONTROL_SUBSYSTEM_ID, DBEX_TXID_MSG_SETLED1_RESP);
    }
}


void DBEx_vToggleLed2Request(uint8_t ucDataLen, const uint8_t* pucData)
{
    //check message length 
    if(ucDataLen == (uint8_t)DBEX_LED_REQ_MAX_LEN)
    {
        // set state of LED
        (void)DBEx_SetLED(RECEIVE_LED2, pucData[DBEX_LED_REQ_STATE]);
        
        DBEx_LED2_requested_state = pucData[DBEX_LED_REQ_STATE];

        /*Send DBus response to currently handled LED toggle request
            - first parameter - Subsystem ID - present in balXs.c in BAL_tBusObject[] - this defines which transmit table from BAL_tBusObject should be used
            - second parameter - Message ID - present in DBus_user_interface.h as Message Indexes - this defines which message should be transmitted from transmit table */
        BAL_vTransmitMessage(LED_CONTROL_SUBSYSTEM_ID, DBEX_TXID_MSG_SETLED2_RESP);
    }
}


/* Transmit functions - called when user transmit data to DBus via function BAL_vTransmitMessage() */
static void DBEx_vToggleLED1Resp(uint8_t dataLen, uint8_t *p_Data)
{
	(void)(dataLen);        // not used here, but it tells us how many user bytes can be sent in this DBus message (parameter DBEX_LEN_LED1_RESP from transmit table)
    bool LED_actual_state;
    
    LED_actual_state = DBEx_GetLED(RECEIVE_LED1);
    if (LED_actual_state == DBEx_LED1_requested_state)
    {
        //LED setting successful
        p_Data[0] = (uint8_t)TRUE;          // store required data into DBus buffer to be transmitted
    }
    else 
    {
        //LED setting failed
        p_Data[0] = (uint8_t)FALSE;
    }

	// report state of LED1
    p_Data[1] = (uint8_t)LED_actual_state;
}

static void DBEx_vToggleLED2Resp(uint8_t dataLen, uint8_t *p_Data)
{
	(void)(dataLen);        // not used here, but it tells us how many user bytes can be sent in this DBus message (parameter DBEX_LEN_LED2_RESP from transmit table)
    bool LED_actual_state;
    
    LED_actual_state = DBEx_GetLED(RECEIVE_LED2);
    if (LED_actual_state == DBEx_LED2_requested_state)
    {
        //LED setting successful
        p_Data[0] = (uint8_t)TRUE;
    }
    else 
    {
        //LED setting failed
        p_Data[0] = (uint8_t)FALSE;
    }

	// report state of LED1
    p_Data[1] = (uint8_t)LED_actual_state;
}


/* Confirmation functions - called once transmission of user message is finished */
static void DBEx_vToggleLED1RespConfirm(void)
{
    //toggle led3 - confirmation LED
    if (DBEx_GetLED(CONFIRM_LED3) == TRUE)
    {
        (void)DBEx_SetLED(CONFIRM_LED3, FALSE);
    }
    else 
    {
        (void)DBEx_SetLED(CONFIRM_LED3, TRUE);
    }
}


static void DBEx_vToggleLED2RespConfirm(void)
{
    //toggle led4 - confirmation LED
    if (DBEx_GetLED(CONFIRM_LED4) == TRUE)
    {
        (void)DBEx_SetLED(CONFIRM_LED4, FALSE);
    }
    else 
    {
        (void)DBEx_SetLED(CONFIRM_LED4, TRUE);
    }
}
