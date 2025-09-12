/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          DBus Application Layer
 *  COMP_ABBREV      DBAL
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Queue for transmitting messages in ecu to ecu communication.
 *
 *  \details  Separate queues for connection messages and other messages.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
/*lint -e9045 non-hidden definition of type 'struct DBAL_Instance', not dangerous*/
#define DBAL_USE_INSTANCE
#include "BshDBus2AppLayer_internal.h"
/*lint -save -restore*/
#include "BshDBus2AppLayer_debug.h"
#ifdef DBAL_CROSS_CONNECTION
#if DBAL_CROSS_CONNECT_COUNT > 1

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define DBALQR_INVALID_TXINDEX   UINT8_MAX//!< Non existent index in DBAL_DBUS_Handler_TxObject

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \struct DBALCQ_ReqResp
 *
 * \brief   Description of messages to send on a given cross connection, which is in DBAL_COMMSTATE_READY.
 *
 * \details No connection messages.
 * \details Command, Query, Event, Command Ack, Query Ack, Event Ack.
 */
struct DBALCQ_ReqResp
{
    const struct DBAL_Instance* Inst;//!< DBal communication instance
    uint8_t TxIndex;//!< Index in DBAL_DBUS_Handler_TxObject
};

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static bool DBALCQ_Initialized = false;//!< Initialization status of cross connection queue.
static const struct DBAL_Instance* DBALCQ_ConnectMsgs[DBAL_CROSS_CONNECT_COUNT];//!< Cross connection queue for connection messages.
static struct DBALCQ_ReqResp DBALCQ_ReqRespMsgs[DBAL_CROSS_CONNECT_COUNT];//!< Cross connection queue for other than connection messages.

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBALCQ_init(void)
{
    if(DBALCQ_Initialized == false)
    {
        for(uint8_t i = 0; i < (uint8_t)DBAL_CROSS_CONNECT_COUNT; i++)
        {
            DBALCQ_ConnectMsgs[i] = NULL;
            DBALCQ_ReqRespMsgs[i].Inst = NULL;
            DBALCQ_ReqRespMsgs[i].TxIndex = DBALQR_INVALID_TXINDEX;
        }

        DBALCQ_Initialized = true;
        DBAL_INFO("Init of CrossQueue");
    }
}

void DBALCQ_queueConnectMsg(const struct DBAL_Instance* Inst)
{
    if(DBALCQ_Initialized == true)
    {
        for(uint8_t i = 0; i < (uint8_t)DBAL_CROSS_CONNECT_COUNT; i++)
        {
            if(DBALCQ_ConnectMsgs[i] == Inst)
            {
                break;
            }
            else if(DBALCQ_ConnectMsgs[i] == NULL)
            {
                DBALCQ_ConnectMsgs[i] = Inst;
                break;
            }
            else
            {
                /*Slot occupied; look into next one.*/
            }
        }
    }
}

bool DBALCQ_getConnectMsg(const struct DBAL_Instance** const Inst)
{
    bool RetVal = false;

    if(DBALCQ_Initialized == true)
    {
        if(DBALCQ_ConnectMsgs[0] != NULL)
        {
            *Inst = DBALCQ_ConnectMsgs[0];
            RetVal = true;

            for(uint8_t i = 0; i < DBAL_LAST_QUEUE_ENTRY; i++)
            {
                DBALCQ_ConnectMsgs[i] = DBALCQ_ConnectMsgs[i + 1U];
            }
            DBALCQ_ConnectMsgs[DBAL_LAST_QUEUE_ENTRY] = NULL;
        }
        else
        {
            *Inst = NULL;
        }
    }

    return RetVal;
}

void DBALCQ_queueReqRespMsg(const struct DBAL_Instance* const Inst, uint8_t TxIndex)
{
    if(DBALCQ_Initialized == true)
    {
        for(uint8_t i = 0; i < (uint8_t)DBAL_CROSS_CONNECT_COUNT; i++)
        {
            if(DBALCQ_ReqRespMsgs[i].Inst == Inst)
            {
                DBALCQ_ReqRespMsgs[i].TxIndex = TxIndex;
                break;
            }
            else if
            (
                (DBALCQ_ReqRespMsgs[i].Inst == NULL) &&
                (DBALCQ_ReqRespMsgs[i].TxIndex == DBALQR_INVALID_TXINDEX)
            )
            {
                DBALCQ_ReqRespMsgs[i].Inst = Inst;
                DBALCQ_ReqRespMsgs[i].TxIndex = TxIndex;
                break;
            }
            else
            {
                /*Slot occupied; look into next one.*/
            }
        }
    }
}

bool DBALCQ_getReqRespMsg(const struct DBAL_Instance** const Inst, uint8_t* const TxIndex)
{
    bool RetVal = false;

    if(DBALCQ_Initialized == true)
    {
        while
        (
            (DBALCQ_ReqRespMsgs[0].Inst != NULL) &&
            (DBALCQ_ReqRespMsgs[0].TxIndex != DBALQR_INVALID_TXINDEX) &&
            (RetVal == false)
        )
        {
            if
            (
                (DBAL_getCommStateByInstance(DBALCQ_ReqRespMsgs[0].Inst) == DBAL_COMMSTATE_READY) &&
                (DBAL_getConnectionStateByInstance(DBALCQ_ReqRespMsgs[0].Inst) == DBAL_CONNECTIONSTATE_CONNECTED)
            )
            {
                *Inst = DBALCQ_ReqRespMsgs[0].Inst;
                *TxIndex = DBALCQ_ReqRespMsgs[0].TxIndex;
                RetVal = true;
            }

            for(uint8_t i = 0; i < DBAL_LAST_QUEUE_ENTRY; i++)
            {
                DBALCQ_ReqRespMsgs[i].Inst = DBALCQ_ReqRespMsgs[i + 1U].Inst;
                DBALCQ_ReqRespMsgs[i].TxIndex = DBALCQ_ReqRespMsgs[i + 1U].TxIndex;
            }
            DBALCQ_ReqRespMsgs[DBAL_LAST_QUEUE_ENTRY].Inst = NULL;
            DBALCQ_ReqRespMsgs[DBAL_LAST_QUEUE_ENTRY].TxIndex = DBALQR_INVALID_TXINDEX;
        }
    }

    if(RetVal == false)
    {
        *Inst = NULL;
        *TxIndex = DBALQR_INVALID_TXINDEX;
    }

    return RetVal;
}
#else /*DBAL_CROSS_CONNECT_COUNT > 1*/
/*Dummys in case there is only one cross connection*/
/*lint -esym(818,DBALCQ_*) Param could be const only in dummy version of function here!*/
void DBALCQ_init(void)
{

}

void DBALCQ_queueConnectMsg(const struct DBAL_Instance* Inst)
{
    (void)*Inst; /*lint !e920 Ok here. Pointer not needed.*/
}

bool DBALCQ_getConnectMsg(const struct DBAL_Instance** const Inst)
{
    (void)*Inst; /*lint !e920 Ok here. Pointer not needed.*/

    return false;
}

void DBALCQ_queueReqRespMsg(const struct DBAL_Instance* const Inst, uint8_t TxIndex)
{
    (void)*Inst; /*lint !e920 Ok here. Pointer not needed.*/
    (void)TxIndex;
}

bool DBALCQ_getReqRespMsg(const struct DBAL_Instance** const Inst, uint8_t* const TxIndex)
{
    (void)*Inst; /*lint !e920 Ok here. Pointer not needed.*/
    (void)*TxIndex; /*lint !e920 Ok here. Pointer not needed.*/

    return false;
}
#endif /*NOT DBAL_CROSS_CONNECT_COUNT > 1*/

#endif /*DBAL_CROSS_CONNECTION*/
