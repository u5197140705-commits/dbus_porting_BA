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
 *  \brief    Implementation of connection status and initialization in DBAL.
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
#include "BshDBus2AppLayer.h"
#include "BshDBus2AppLayerCross.h"
#include "BshDBus2AppLayer_debug.h"
#include "timer/system_timer.h"
#include "dbusmapping.h"
#include "dbuspresentation.h"
#include "bustypes.h"
#include "bal.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifdef DOXY_DEVELOPERS_DOC
/** Compiler switch to activate cross connection between microcontrollers.
 */ #define DBAL_CROSS_CONNECTION

/** Compiler switch to define number of cross connections between microcontrollers.
 */ #define DBAL_CROSS_CONNECT_COUNT
#endif
/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static int32_t DBAL_breakProcessTimerCallback(void *Obj, uint32_t Flags, int32_t Data);
static void DBAL_initTimer4BreakProcessing(void);
#ifdef DBAL_CROSS_CONNECTION
static int32_t DBALCR_pingReqTimerCallback(void *Obj, uint32_t Flags, int32_t Data);
#endif/*DBAL_CROSS_CONNECTION*/
static void DBAL_printStatemachineInfo(const struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Event);
static void DBAL_printUnexpectedEvent(const struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Event);
#ifdef DBAL_CROSS_CONNECTION
static void DBALCR_initPingReqTimer(struct DBAL_Instance* const Inst);
static void DBALCR_setPingTime(const struct DBAL_Instance* const Inst, uint16_t TimeMs);
static void DBALCR_triggerPingReqTimer(const struct DBAL_Instance* const Inst, uint16_t TimeMs);
static void DBALCR_stopPingReqTimer(const struct DBAL_Instance* const Inst);
#endif/*DBAL_CROSS_CONNECTION*/
static bool DBAL_enableByInstance(struct DBAL_Instance* const Inst);
static bool DBAL_disableByInstance(struct DBAL_Instance* const Inst);
static bool DBAL_registerCommStateByInstance(struct DBAL_Instance* const Inst, DBAL_ioCommStateCb Cb);
static bool DBAL_registerMsgLostByInstance(struct DBAL_Instance* const Inst, DBAL_ioMsgLostCb Cb);
static void DBAL_fireCommState(const struct DBAL_Instance* const Inst, enum DBAL_CommState CommState);
static void DBAL_actionToConnected(struct DBAL_Instance* const Inst);
static void DBAL_actionToDisconnected(struct DBAL_Instance* const Inst);

/* Warning 522: highest operation, function 'DBAL_printUnexpectedEvent', lacks side effects [MISRA 2012 Rule 2.2, advisory] */
/*lint -esym(522,DBAL_printUnexpectedEvent)  : ok, intention */
/*lint -esym(522,DBAL_printStatemachineInfo) : ok, intention */
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static struct STIM_Timer BreakProcessTimer; //!< Timer to wait for Enable Request afer Dbus2 break
static struct STDCB_Callback BreakCbInst;   //!< Callback of BreakProcessTimer

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void DBAL_init(uint8_t TargetAddress)
{
    uint8_t TargetAddr = TargetAddress;

    if
    (
        (DBAL_isValidDbus2Address(TargetAddress) == true) ||
        (TargetAddress == DBAL_PRELIMINARY_PARTNER_ADDR)
    )
    {
        if(TargetAddress == DBAL_PRELIMINARY_PARTNER_ADDR)
        {/*Address from param not valid. -> See for value in memory.*/
            TargetAddr = DBAL_getTargetAddress();
            if(DBAL_isValidDbus2Address(TargetAddr) == false)
            {
                DBAL_INFO("SMM Read Target Address %x not valid", TargetAddr);
                TargetAddr = DBAL_PRELIMINARY_PARTNER_ADDR;
            }
        }
        DBALIN_init(TargetAddr, DBALIN_getMainInstanceForInit());
        DBAL_appLayerDBus2Init(DBALIN_getMainInstance());
        DBAL_initTimer4BreakProcessing();
        DBAL_INFO("SMM Inst initialized, Addr %x", TargetAddr);
    }
    else
    {
        DBAL_ERROR("Tried to set invalid node address %x of partner.", TargetAddress);/*Wrong config*/
    }
}

bool DBAL_enable(void)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        if(DBAL_isValidDbus2Address(Inst->DBUS_ComPartner) == true)
        {
            RetVal = DBAL_enableByInstance(Inst);
        }
        else
        {
            DBAL_WARNING("SMM tried to enable with invalid node address %x.", Inst->DBUS_ComPartner);
        }
    }
    DBAL_INFO("SMM enable %u, Inst %p", RetVal, Inst);

    return RetVal;
}

bool DBAL_disable(void)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        if(DBAL_isValidDbus2Address(Inst->DBUS_ComPartner) == true)
        {
            RetVal = DBAL_disableByInstance(Inst);
        }
        else
        {
            DBAL_WARNING("SMM tried to disable with invalid node address %x.", Inst->DBUS_ComPartner);
        }
    }
    DBAL_INFO("SMM disable %u, Inst %p", RetVal, Inst);

    return RetVal;
}

enum DBAL_CommState DBAL_getCommState(void)
{
    enum DBAL_CommState RetVal = DBAL_COMMSTATE_NOT_READY;
    const struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        RetVal = DBAL_getCommStateByInstance(Inst);
    }

    return RetVal;
}

bool DBAL_registerCommState(DBAL_ioCommStateCb Cb)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        RetVal = DBAL_registerCommStateByInstance(Inst, Cb);
    }
    DBAL_INFO("SMM register CommState %u, Inst %p", RetVal, Inst);

    return RetVal;
}

bool DBAL_registerLostMsg(DBAL_ioMsgLostCb Cb)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        RetVal = DBAL_registerMsgLostByInstance(Inst, Cb);
    }
    DBAL_INFO("SMM register LostMsg %u, Inst %p", RetVal, Inst);

    return RetVal;
}

void DBAL_breakSignalDetected(void)
{
    STIM_ResetTimer(&BreakProcessTimer);
    STIM_EnableTimer(&BreakProcessTimer);
    if(DBPL_bSendWakeupSentRequest() == false)
    {
        DBAL_WARNING("Sending Wakeup Break did not work!!!");
    }
}

#ifdef DBAL_CROSS_CONNECTION
bool DBALCR_init(void)
{
    bool RetVal = true;
    for(uint8_t i = 0; i < (uint8_t)DBAL_CROSS_CONNECT_COUNT; i++)
    {
        if
        (    /*Check coherence of config with addresses of comm partners:*/
            (DBAL_isValidDbus2Address(DBALCR_ComPartners[i]) == false) &&
            (DBALCR_RolesTowardPartners[i] != DBALCR_PART_TYPE_NONE)
        )
        {
            RetVal = false;
            DBAL_ERROR("Wrong cross config, CommPartner %x, Type %u", DBALCR_ComPartners[i], DBALCR_RolesTowardPartners[i]);
            break;
        }
        else if
        (
            (DBAL_isValidDbus2Address(DBALCR_ComPartners[i]) == true) &&
            (DBALCR_RolesTowardPartners[i] != DBALCR_PART_TYPE_NONE)
        )
        {
            DBALIN_init(DBALCR_ComPartners[i], (&DBALIN_getUserInstancesForInit()[i]));
            struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(i);
            if(Inst != NULL)
            {
                DBAL_appLayerDBus2Init(Inst);
                DBALIN_initCrExtensionByIndex(i, DBALCR_RolesTowardPartners[i]);
                DBALCR_setPingTime(Inst, DBALCR_PingTimeMs);/*lint !e522 False positive. Side effects occur.*/
                DBALCR_initPingReqTimer(Inst);
            }
            else
            {
                RetVal = false;
                break;
            }
        }
        else
        {
            DBAL_INFO("Not all Cross Inst initialized!");
            break;/*Both params invalid: Return true, and assume that rest of config arrays is invalid from here.*/
        }
    }

    if(RetVal == true)
    {
        DBALCQ_init();
    }
    DBAL_INFO("CR init %u", RetVal);

    return RetVal;
}

bool DBALCR_enable(uint8_t Index)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if(Inst != NULL)
    {
        RetVal = DBAL_enableByInstance(Inst);
    }
    DBAL_INFO("CR enable %u, Index %u, Inst %p", RetVal, Index, Inst);

    return RetVal;
}

bool DBALCR_disable(uint8_t Index)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if(Inst != NULL)
    {
        RetVal = DBAL_disableByInstance(Inst);
    }
    DBAL_INFO("CR disable %u, Index %u, Inst %p", RetVal, Index, Inst);

    return RetVal;
}

enum DBAL_CommState DBALCR_getCommState(uint8_t Index)
{
    enum DBAL_CommState RetVal = DBAL_COMMSTATE_NOT_READY;
    const struct DBAL_Instance* const Inst =  DBALIN_getUserInstanceByIndex(Index);

    if(Inst != NULL)
    {
        RetVal = DBAL_getCommStateByInstance(Inst);
    }

    return RetVal;
}

bool DBALCR_registerCommState(uint8_t Index, DBAL_ioCommStateCb Cb)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if(Inst != NULL)
    {
        RetVal = DBAL_registerCommStateByInstance(Inst, Cb);
    }
    DBAL_INFO("CR register CommState %u, Index %u, Inst %p", RetVal, Index, Inst);

    return RetVal;
}

bool DBALCR_registerLostMsg(uint8_t Index, DBAL_ioMsgLostCb Cb)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getUserInstanceByIndex(Index);

    if(Inst != NULL)
    {
        RetVal = DBAL_registerMsgLostByInstance(Inst, Cb);
    }
    DBAL_INFO("CR register LostMsg %u, Index %u, Inst %p", RetVal, Index, Inst);

    return RetVal;
}
#endif/*DBAL_CROSS_CONNECTION*/

/*internal stuff*/
enum DBAL_CommState DBAL_getCommStateByInstance(const struct DBAL_Instance* const Inst)
{
    return Inst->IoCurrentCommState;
}

enum DBAL_ConnectionState DBAL_getConnectionStateByInstance(const struct DBAL_Instance* const Inst)
{
    return Inst->IoCurrentConnectionState;
}

/*connection statemachine*/
void DBAL_connectionSm(struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Event)
{
    switch (Inst->IoCurrentConnectionState)
    {
        case DBAL_CONNECTIONSTATE_DISCONNECTED:
        {
            switch (Event)
            {
                case DBAL_CON_SM_EVENT_ENABLE:
                {
                    Inst->IoCurrentConnectionState = DBAL_CONNECTIONSTATE_CONNECTING;
                    DBAL_sendCommEnableRequest(Inst);
                    break;
                }
                case DBAL_CON_SM_EVENT_ENABLE_REQUEST:
                {
                    DBAL_sendCommEnableResponse(Inst);
                    DBAL_actionToConnected(Inst);
                    STIM_DisableTimer(&BreakProcessTimer);
                    break;
                }
                case DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST:
                {
                    DBAL_sendCommTempEnableResponse(Inst);
                    DBAL_actionToConnected(Inst);
                    break;
                }
                default:
                {
                    DBAL_printUnexpectedEvent(Inst, Event);
                    break;
                }
            }/*lint !e788 other cases are here invalid */
            break;
        }
        case DBAL_CONNECTIONSTATE_CONNECTING:
        {
            switch (Event)
            {
                case DBAL_CON_SM_EVENT_ENABLE_REQUEST:
                {
                    DBAL_sendCommEnableResponse(Inst);
                    DBAL_actionToConnected(Inst);
                    break;
                }
                case DBAL_CON_SM_EVENT_ACCEPT:
                {
                    DBAL_actionToConnected(Inst);
                    break;
                }
                case DBAL_CON_SM_EVENT_REJECT:
                {
                    Inst->IoCurrentConnectionState = DBAL_CONNECTIONSTATE_DISCONNECTED;
                    break;
                }
                case DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST:
                {
                    DBAL_sendCommTempEnableResponse(Inst);
                    DBAL_actionToConnected(Inst);
                    break;
                }
                default:
                {
                    DBAL_printUnexpectedEvent(Inst, Event);
                    break;
                }
            }/*lint !e788 other cases are here invalid */
            break;
        }
        case DBAL_CONNECTIONSTATE_CONNECTED:
        {
            switch (Event)
            {
                case DBAL_CON_SM_EVENT_ENABLE_REQUEST:
                {
                    DBAL_sendCommEnableResponse(Inst);
                    DBAL_actionToDisconnected(Inst);
                    DBAL_actionToConnected(Inst);
                    break;
                }
                case DBAL_CON_SM_EVENT_DISABLE:
                {
                    Inst->IoCurrentConnectionState = DBAL_CONNECTIONSTATE_DISCONNECTING;
                    DBAL_sendCommDisableRequest(Inst);
                    break;
                }
                case DBAL_CON_SM_EVENT_DISABLE_SILENT:
                {
                    DBAL_actionToDisconnected(Inst);
                    break;
                }
                case DBAL_CON_SM_EVENT_DISABLE_REQUEST:
                {
                    DBAL_sendCommDisableResponse(Inst);
                    DBAL_actionToDisconnected(Inst);
                    break;
                }
                case DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST:
                {
                    DBAL_sendCommTempEnableResponse(Inst);
                    DBAL_actionToConnected(Inst);
                    break;
                }
                case DBAL_CON_SM_EVENT_TEMP_DISABLE_REQUEST:
                {
                    DBAL_sendCommTempDisableResponse(Inst);
                    DBAL_actionToDisconnected(Inst);
                    break;
                }
                default:
                {
                    DBAL_printUnexpectedEvent(Inst, Event);
                    break;
                }
            }/*lint !e788 other cases are here invalid */
            break;
        }
        case DBAL_CONNECTIONSTATE_DISCONNECTING:
        {
            if((Event == DBAL_CON_SM_EVENT_ACCEPT) || (Event == DBAL_CON_SM_EVENT_REJECT)) // both here is valid?
            {
                DBAL_actionToDisconnected(Inst);
            }
            else
            {
                DBAL_printUnexpectedEvent(Inst, Event);
            }
            break;
        }
        default:
            /*No valid state here.*/
            break;
    }
    DBAL_printStatemachineInfo(Inst, Event);
}

#ifdef DBAL_CROSS_CONNECTION
void DBALCR_triggerPingReqTimerWithStdTime(const struct DBAL_Instance* const Inst)
{
    if
    (
        (Inst->CrExtension != NULL) &&
        (Inst->CrExtension->OwnPartType == DBALCR_PART_TYPE_CLIENT) &&
        (Inst->CrExtension->PingTimeMs >= DBAL_MIN_PING_TIME_MS) &&
        (Inst->IoCurrentCommState == DBAL_COMMSTATE_READY) &&
        (Inst->IoCurrentConnectionState == DBAL_CONNECTIONSTATE_CONNECTED)
    )
    {
        DBALCR_triggerPingReqTimer(Inst, Inst->CrExtension->PingTimeMs);
        Inst->CrExtension->PingRepetition = 0U;
    }
}
#endif/*DBAL_CROSS_CONNECTION*/

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
/* Info 818: parameter 'Obj' of function 'DBAL_breakProcessTimerCallback(void *, uint32_t, int32_t)' could be pointer to const [MISRA 2012 Rule 8.13, advisory]
 * Cannot be const, as interfaces are predefined! */
/*lint -esym(818,DBAL_breakProcessTimerCallback*) */
static int32_t DBAL_breakProcessTimerCallback(void *Obj, uint32_t Flags, int32_t Data)
{
    (void)Flags;
    (void)Data;

    if(Obj != NULL)
    {
        DBAL_WARNING("Break process timer with initialized object");
    }

    struct DBAL_Instance* Inst = DBALIN_getMainInstance();

    if
    (
        (Inst != NULL) &&
        ((Inst->IoCurrentCommState == DBAL_COMMSTATE_NOT_READY) ||
        (Inst->IoCurrentConnectionState == DBAL_CONNECTIONSTATE_DISCONNECTING))
    )
    {
        DBAL_actionToDisconnected(Inst);
    }
#ifdef DBAL_CROSS_CONNECTION
    for(uint8_t i = 0; i < (uint8_t)DBAL_CROSS_CONNECT_COUNT; i++)
    {
        Inst = DBALIN_getUserInstanceByIndex(i);
        if
        (
            (Inst != NULL) &&
            ((Inst->IoCurrentCommState == DBAL_COMMSTATE_NOT_READY) ||
            (Inst->IoCurrentConnectionState == DBAL_CONNECTIONSTATE_DISCONNECTING))
        )
        {
            DBAL_actionToDisconnected(Inst);
        }
    }
#endif
    return 0;
}

/** \brief Initializes timer to handle detected DBus2 wake up break
 */
static void DBAL_initTimer4BreakProcessing(void)
{
    (void)STIM_InitCallback(&BreakCbInst, DBAL_breakProcessTimerCallback, NULL, STIM_STATUS_TRIGGERED);
    (void)STIM_InitTimer(&BreakProcessTimer, STIM_PROCESSING_SCHEDULER, ((uint32_t)DBAL_DBUS_RECOVERY_TIME_MS), STIM_MODE_SINGLE, (bool)false, &BreakCbInst);
}

#ifdef DBAL_CROSS_CONNECTION
/** \brief  Timer triggering ping request, if there has been no communication for a longer time.
 *
 * \details For meaning of params, look at STIM module description.
*/
static int32_t DBALCR_pingReqTimerCallback(void *Obj, uint32_t Flags, int32_t Data)
{
    (void)Flags;
    (void)Data;
    /*Axivion Next Line MisraC2012-11.5 : No other choice than to do this conversion. Works fine.*/
    struct DBAL_Instance* const Inst = (struct DBAL_Instance*)Obj;

    if
    (
        (Inst->CrExtension->OwnPartType == DBALCR_PART_TYPE_CLIENT) &&
        (Inst->CrExtension->PingTimeMs >= DBAL_MIN_PING_TIME_MS) &&
        (Inst->IoCurrentCommState == DBAL_COMMSTATE_READY) &&
        (Inst->IoCurrentConnectionState == DBAL_CONNECTIONSTATE_CONNECTED)
    )
    {
        if(Inst->CrExtension->PingRepetition <= DBAL_PING_MAX_REPEAT)
        {
            DBAL_sendCommPingRequest(Inst);
            DBALCR_triggerPingReqTimer(Inst, DBAL_PING_REPEAT_MS);
            DBAL_INFO("Ping Repeat no %d, Addr %x", Inst->CrExtension->PingRepetition, Inst->DBUS_ComPartner);
            Inst->CrExtension->PingRepetition++;
        }
        else
        {
            DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_DISABLE_SILENT);
            DBAL_WARNING("End of connection, because Ping unresponded repeatedly, Addr %x", Inst->DBUS_ComPartner);
        }
    }

    return 0;
}
#endif/*DBAL_CROSS_CONNECTION*/

/** \brief Debug function. To be implemented.
*/
static void DBAL_printStatemachineInfo(const struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Event)
{
    (void)Inst;/*lint !e920 ok here, pointer not needed.*/
    (void)Event;
    DBAL_INFO("StateMachineEvent %u, Addr %x", Event, Inst->DBUS_ComPartner);
}

/** \brief Debug function. To be implemented.
*/
static void DBAL_printUnexpectedEvent(const struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Event)
{
    (void)Inst;/*lint !e920 ok here, pointer not needed.*/
    (void)Event;
    DBAL_INFO("UnexpectedEvent %u, Addr %x", Event, Inst->DBUS_ComPartner);
}

#ifdef DBAL_CROSS_CONNECTION
/** \brief     Measures time, after which ping request is to be sent.
 *
 * \param Inst DBal communication instance
 */
static void DBALCR_initPingReqTimer(struct DBAL_Instance* const Inst)
{
    if
    (
        (Inst->CrExtension != NULL) &&
        (Inst->CrExtension->OwnPartType == DBALCR_PART_TYPE_CLIENT)
    )
    {
        (void)STIM_InitCallback(&Inst->CrExtension->PingReqCbInst, DBALCR_pingReqTimerCallback, Inst, STIM_STATUS_TRIGGERED);
        (void)STIM_InitTimer(&Inst->CrExtension->PingReqTimer, STIM_PROCESSING_SCHEDULER, DBAL_MIN_PING_TIME_MS, STIM_MODE_SINGLE, (bool)false, &Inst->CrExtension->PingReqCbInst);
        DBAL_INFO("Ping Timer Init, Addr %x", Inst->DBUS_ComPartner);
    }
}

/** \brief       Sets time interval for pinging.
 *
 * \param Inst   DBal communication instance
 * \param TimeMs Time interval in ms
 *
 */
static void DBALCR_setPingTime(const struct DBAL_Instance* const Inst, uint16_t TimeMs)
{
    if
    (
        (Inst->CrExtension != NULL) &&
        (Inst->CrExtension->OwnPartType == DBALCR_PART_TYPE_CLIENT)
    )
    {
        Inst->CrExtension->PingTimeMs = TimeMs;
        DBAL_INFO("Ping Time set %d ms, Addr %x", Inst->CrExtension->PingTimeMs, Inst->DBUS_ComPartner);
    }
}

/** \brief       Starts Ping Request Timer
 *
 * \param Inst   DBal communication instance
 * \param TimeMs Time interval in ms
 *
 */
static void DBALCR_triggerPingReqTimer(const struct DBAL_Instance* const Inst, uint16_t TimeMs)
{
    if(Inst->CrExtension != NULL)
    {
        STIM_ReloadTimer(&Inst->CrExtension->PingReqTimer, TimeMs);
        STIM_EnableTimer(&Inst->CrExtension->PingReqTimer);
    }
}

/** \brief       Stops Ping Request Timer
 *
 * \param Inst   DBal communication instance
 *
 */
static void DBALCR_stopPingReqTimer(const struct DBAL_Instance* const Inst)
{
    if(Inst->CrExtension != NULL)
    {
        STIM_DisableTimer(&Inst->CrExtension->PingReqTimer);
        Inst->CrExtension->PingRepetition = 0U;
    }
}
#endif/*DBAL_CROSS_CONNECTION*/

/** \brief       Enable the communication with the communication partner defined by
 *               the given DBal communication instance.
 *
 * \param Inst   DBal communication instance
 *
 * \return       Returns false, if communication already is (being) enabled, otherwise true.
 *
 * \note         For now, no Ping mechanism is implemented.
*/
static bool DBAL_enableByInstance(struct DBAL_Instance* const Inst)
{
    bool RetVal = false;

    DBM_DISABLE_INT();
    if
    (
        (Inst->IoCurrentCommState == DBAL_COMMSTATE_NOT_READY) &&
        (Inst->IoCurrentConnectionState == DBAL_CONNECTIONSTATE_DISCONNECTED)
    )
    {
        DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_ENABLE);
        DBAL_INFO("Connect enable Addr %x", Inst->DBUS_ComPartner);
        RetVal = true;
    }
    DBM_ENABLE_INT();

    return RetVal;
}

/** \brief       Disable the communication with the communication partner defined by
 *               the given DBal communication instance.
 *
 * \param Inst   DBal communication instance
 *
 * \return       Returns false, if communication already is (being) disabled, otherwise true.
*/
static bool DBAL_disableByInstance(struct DBAL_Instance* const Inst)
{
    bool RetVal = false;

    DBM_DISABLE_INT();
    if
    (
        (Inst->IoCurrentCommState == DBAL_COMMSTATE_READY) &&
        (Inst->IoCurrentConnectionState == DBAL_CONNECTIONSTATE_CONNECTED)
    )
    {
        DBAL_connectionSm(Inst, DBAL_CON_SM_EVENT_DISABLE);
        DBAL_INFO("Connect disable Addr %x", Inst->DBUS_ComPartner);
        RetVal = true;
    }
    DBM_ENABLE_INT();

    return RetVal;
}

/** \brief     Register a callback for changes in communication status with communication partner defined by
 *             the given DBal communication instance.
 *
 * \param Inst DBal communication instance
 * \param Cb   Callback
 *
 * \return     Returns true, if registration worked, otherwise false.
*/
static bool DBAL_registerCommStateByInstance(struct DBAL_Instance* const Inst, DBAL_ioCommStateCb Cb)
{
    bool RetVal = false;

    if((Inst->IoCommStateCbCounter < (uint8_t)DBAL_IO_MAX_CALLBACK_BUFFER_SIZE) && (Cb != NULL))
    {
        Inst->IoCommStateCbArray[Inst->IoCommStateCbCounter] = Cb;
        Inst->IoCommStateCbCounter++;
        RetVal = true;
    }

    return RetVal;
}

/** \brief     Register a callback to be notified, whenever a message could not be delivered; defined by
 *             the given DBal communication instance.
 *
 * \param Inst DBal communication instance
 * \param Cb   Callback
 *
 * \return     Returns true, if registration worked, otherwise false.
*/
static bool DBAL_registerMsgLostByInstance(struct DBAL_Instance* const Inst, DBAL_ioMsgLostCb Cb)
{
    bool RetVal = false;

    if((Inst->IoMsgLostCbCounter < (uint8_t)DBAL_IO_MAX_CALLBACK_BUFFER_SIZE) && (Cb != NULL))
    {
        Inst->IoMsgLostCbArray[Inst->IoMsgLostCbCounter] = Cb;
        Inst->IoMsgLostCbCounter++;
        RetVal = true;
    }

    return RetVal;
}

/** \brief          Notify user about changes in communication status with communication partner defined by
 *                  the given DBal communication instance.
 * \details         Callback registered by user are called here.
 *
 * \param Inst      DBal communication instance
 * \param CommState DBAL_COMMSTATE_NOT_READY, DBAL_COMMSTATE_READY
 *
 * \return          Returns true, if registration worked, otherwise false.
*/
static void DBAL_fireCommState(const struct DBAL_Instance* const Inst, enum DBAL_CommState CommState)
{
    if(Inst->IoCommStateCbCounter != 0U)
    {
        for(uint8_t i = 0; i < Inst->IoCommStateCbCounter; i++)
        {
            (Inst->IoCommStateCbArray[i])(CommState);
        }
    }
}

/** \brief      Finish opening of connection with communication partner defined by
 *              the given DBal communication instance.
 *
 * \param Inst  DBal communication instance
*/
static void DBAL_actionToConnected(struct DBAL_Instance* const Inst)
{
    Inst->IoCurrentConnectionState = DBAL_CONNECTIONSTATE_CONNECTED;
    Inst->IoCurrentCommState = DBAL_COMMSTATE_READY;
    Inst->SeqId2Send = UINT8_MAX; /*First ID to send is zero.*/
    Inst->LastSeqIdReceived = UINT8_MAX; /*We expect, that first ID to receive is zero.*/
    STIM_DisableTimer(&Inst->ConMsgTimer);
#ifdef DBAL_CROSS_CONNECTION
    DBALCR_triggerPingReqTimerWithStdTime(Inst);
#endif /*DBAL_CROSS_CONNECTION*/
    Inst->ConRepeatCnt = 0;
    DBAL_fireCommState(Inst, DBAL_COMMSTATE_READY);
    DBAL_INFO("CONNECTED: Addr %x", Inst->DBUS_ComPartner);
}

/** \brief      Finish closing of connection with communication partner defined by
 *              the given DBal communication instance.
 *
 * \param Inst  DBal communication instance
*/
static void DBAL_actionToDisconnected(struct DBAL_Instance* const Inst)
{
    Inst->IoCurrentConnectionState = DBAL_CONNECTIONSTATE_DISCONNECTED;
    Inst->IoCurrentCommState = DBAL_COMMSTATE_NOT_READY;
    Inst->DisableReqReceived = false;
    STIM_DisableTimer(&Inst->ConMsgTimer);
#ifdef DBAL_CROSS_CONNECTION
    DBALCR_stopPingReqTimer(Inst);
#endif /*DBAL_CROSS_CONNECTION*/
    DBAL_fireCommState(Inst, DBAL_COMMSTATE_NOT_READY);
    Inst->ConRepeatCnt = 0;
    DBAL_INFO("DISCONNECTED: Addr %x", Inst->DBUS_ComPartner);
    if(Inst->DBUS_ComPartner != Inst->DBUS_ComBackup)
    {/*If temporary connection, go back to original.*/
        Inst->DBUS_ComPartner = Inst->DBUS_ComBackup;
    }
}

