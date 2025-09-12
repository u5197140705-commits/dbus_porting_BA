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
 *  \brief    Describing sending/receiving of messages in DBAL in a multi-instanceable way.
 *
 *  \details  DBAL is the communication protocol which is used by symana. For
 *            detailed information, please read the md file which is included in the
 *            doc folder.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#define DBAL_USE_INSTANCE
#include "BshDBus2AppLayer_internal.h"
#include "BshDBus2AppLayer_types.h"
#include "dbusdll.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifdef DOXY_DEVELOPERS_DOC
/** Compiler switch to activate cross connection between microcontrollers.
 */ #define DBAL_CROSS_CONNECTION

/** Compiler switch to define number of cross connections between microcontrollers.
 */ #define DBAL_CROSS_CONNECT_COUNT
#endif

// https://www.wolframalpha.com/input/?i=convert+pi+to+base16
#define DBALIN_INIT_PATTERN       0x3243f6A8U //!< Init pattern, to make sure that communication instance is initialized exactly once during runtime.

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

static bool DBALIN_isInstanceInitialized(const struct DBAL_Instance* const Inst);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static struct DBAL_Instance DBAL_MainInstance; //!< Connection to SMM.
#ifdef DBAL_CROSS_CONNECTION
static struct DBAL_Instance DBALCR_UserInstances[DBAL_CROSS_CONNECT_COUNT]; //!< Cross connection to microcontrollers. Length set by user in build process.
#endif/*DBAL_CROSS_CONNECTION*/
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBALIN_init(uint8_t ComPartnerAddress, struct DBAL_Instance* const InstancePtr)
{
    static struct DBAL_TxRepeat TxRepeatInst;

    if(InstancePtr != NULL)
    {
        if(InstancePtr->InitPattern != DBALIN_INIT_PATTERN)
        {
            InstancePtr->InitPattern = DBALIN_INIT_PATTERN;
            InstancePtr->CodeSectionBitMask = 0;
            InstancePtr->DBUS_ComPartner = ComPartnerAddress;
            InstancePtr->DBUS_ComBackup = ComPartnerAddress;
            InstancePtr->DisableReqReceived = false;
            (void)memset(InstancePtr->ConnectTransmitBuffer, 0, sizeof(InstancePtr->ConnectTransmitBuffer));/*lint !e920 cast ok in this case we do not need pointer*/
            InstancePtr->ConnectDataLen = 0;
            InstancePtr->ConRepeatCnt = 0;
            (void)memset(InstancePtr->TransmitBuffer, 0, sizeof(InstancePtr->TransmitBuffer));/*lint !e920 cast ok in this case we do not need pointer*/
            InstancePtr->TransmitDataLen = 0;
            InstancePtr->DbalFrames2TransmitCnt = 0;
            (void)memset(InstancePtr->LastSendingStatus, (int16_t)DLL_ACK_OK, sizeof(InstancePtr->LastSendingStatus));/*lint !e920 cast ok in this case we do not need pointer*/
            (void)memset(InstancePtr->SendRetryCounter, 0, sizeof(InstancePtr->SendRetryCounter));/*lint !e920 cast ok in this case we do not need pointer*/
            InstancePtr->SeqId2Send = UINT8_MAX; /*First ID to send is zero.*/
            InstancePtr->LastSeqIdReceived = UINT8_MAX; /*We expect, that first ID to receive is zero.*/
            (void)memset(InstancePtr->Msgs2Repeat, 0, sizeof(InstancePtr->Msgs2Repeat));/*lint !e920 cast ok in this case we do not need pointer*/
            for(uint8_t i = 0; i < (uint8_t)DBAL_MAX_MSGS2REPEAT; i++)
            {
                InstancePtr->MsgRptPtrs[i] = &InstancePtr->Msgs2Repeat[i];
            }
            InstancePtr->IoCurrentConnectionState = DBAL_CONNECTIONSTATE_DISCONNECTED;
            InstancePtr->IoCurrentCommState = DBAL_COMMSTATE_NOT_READY;
            InstancePtr->IoCommStateCbCounter = 0;
            InstancePtr->IoMsgLostCbCounter = 0;
            (void)memset(InstancePtr->IoCommStateCbArray, 0, sizeof(InstancePtr->IoCommStateCbArray));/*lint !e920 cast ok in this case we do not need pointer*/
            (void)memset(InstancePtr->IoMsgLostCbArray, 0, sizeof(InstancePtr->IoMsgLostCbArray));/*lint !e920 cast ok in this case we do not need pointer*/

            if(InstancePtr == &DBAL_MainInstance)
            {
                InstancePtr->TxRepeat = &TxRepeatInst;
                (void)memset(InstancePtr->TxRepeat->TxRepeatBuffer, 0, sizeof(InstancePtr->TxRepeat->TxRepeatBuffer));/*lint !e920 cast ok in this case we do not need pointer*/
                InstancePtr->TxRepeat->TxRepeatLen = 0;
            }
            else
            {
                InstancePtr->TxRepeat = NULL;
            }
#ifdef DBAL_CROSS_CONNECTION
            InstancePtr->CrExtension = NULL;
#endif
        }
    }
}

struct DBAL_Instance* DBALIN_getMainInstanceForInit(void)
{
    return &DBAL_MainInstance;
}

struct DBAL_Instance* DBALIN_getUserInstancesForInit(void)
{
#ifdef DBAL_CROSS_CONNECTION
    return DBALCR_UserInstances;
#else/*DBAL_CROSS_CONNECTION*/
    return NULL;
#endif/*not DBAL_CROSS_CONNECTION*/
}

struct DBAL_Instance* DBALIN_getMainInstance(void)
{
    struct DBAL_Instance* RetVal = NULL;

    if(DBALIN_isInstanceInitialized(&DBAL_MainInstance))
    {
        RetVal = &DBAL_MainInstance;
    }

    return RetVal;
}

struct DBAL_Instance* DBALIN_getInstanceByAddress(uint8_t PartnerAddress)
{
    struct DBAL_Instance* RetVal = NULL;

    if
    (
        (PartnerAddress == DBAL_MainInstance.DBUS_ComPartner)&&
        (DBALIN_isInstanceInitialized(&DBAL_MainInstance))
    )
    {
        RetVal = &DBAL_MainInstance;
    }
#ifdef DBAL_CROSS_CONNECTION
    else
    {
        for(uint8_t i = 0; i < (uint8_t)DBAL_CROSS_CONNECT_COUNT; i++)
        {
            if
            (
                (PartnerAddress == DBALCR_UserInstances[i].DBUS_ComPartner) &&
                (DBALIN_isInstanceInitialized(&DBALCR_UserInstances[i]) == true)
            )
            {
                RetVal = &DBALCR_UserInstances[i];
                break;
            }
        }
    }
#endif/*DBAL_CROSS_CONNECTION*/

    return RetVal;
}

struct DBAL_Instance* DBALIN_getUserInstanceByIndex(uint8_t Index)
{
    struct DBAL_Instance* RetVal = NULL;

#ifdef DBAL_CROSS_CONNECTION
    if
    (
        (Index < (uint8_t)DBAL_CROSS_CONNECT_COUNT) &&
        (DBALIN_isInstanceInitialized(&DBALCR_UserInstances[Index]) == true)
    )
    {
        RetVal = &DBALCR_UserInstances[Index];
    }
#else/*DBAL_CROSS_CONNECTION*/
    (void)Index;
#endif/*not DBAL_CROSS_CONNECTION*/

    return RetVal;
}

uint8_t DBALIN_getIndexOfUserInstance(const struct DBAL_Instance* const Inst)
{
    uint8_t RetVal = UINT8_MAX;
#ifdef DBAL_CROSS_CONNECTION
    for(uint8_t i = 0; i < (uint8_t)DBAL_CROSS_CONNECT_COUNT; i++)
    {
        if(Inst == &DBALCR_UserInstances[i])
        {
            RetVal = i;
            break;
        }
    }
#else/*DBAL_CROSS_CONNECTION*/
    (void)Inst;/*lint !e920 cast ok in this case we do not need pointer*/
#endif/*not DBAL_CROSS_CONNECTION*/

    return RetVal;
}

#ifdef DBAL_CROSS_CONNECTION
void DBALIN_initCrExtensionByIndex(uint8_t Index, enum DBALCR_ParticipantType ParticipantType)
{
    static struct DBALCR_InstanceExtension DBALCR_Extensions[DBAL_CROSS_CONNECT_COUNT];

    if(Index < (uint8_t)DBAL_CROSS_CONNECT_COUNT)
    {
        if(DBALCR_UserInstances[Index].CrExtension == NULL)
        {
            DBALCR_UserInstances[Index].CrExtension = &DBALCR_Extensions[Index];
            DBALCR_UserInstances[Index].CrExtension->PingTimeMs = 0;
            DBALCR_UserInstances[Index].CrExtension->OwnPartType = ParticipantType;
            DBALCR_UserInstances[Index].CrExtension->PingRepetition = 0U;
        }
    }
}

enum DBALCR_ParticipantType DBALIN_getCrParticipantTypeByIndex(uint8_t Index)
{
    enum DBALCR_ParticipantType RetVal = DBALCR_PART_TYPE_NONE;

    if(Index < (uint8_t)DBAL_CROSS_CONNECT_COUNT)
    {
        if(DBALCR_UserInstances[Index].CrExtension != NULL)
        {
            RetVal = DBALCR_UserInstances[Index].CrExtension->OwnPartType;
        }
    }

    return RetVal;
}
#endif/*DBAL_CROSS_CONNECTION*/
/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief     Checks initialization of DBal communication instance.
 *
 * \details    Checks validity of init pattern and communication partner Dbus2 address.
 * \details    Only addresses with values >= 0x10 are valid.
 *
 * \param Inst DBal communication instance
 *
 * \return     True, if correctly initialized, else false.
 */
static bool DBALIN_isInstanceInitialized(const struct DBAL_Instance* const Inst)
{
    return (Inst->InitPattern == DBALIN_INIT_PATTERN) ? true : false;
}
