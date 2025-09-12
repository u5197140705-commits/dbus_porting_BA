/*******************************************************************************
*   Copyright (c) 2024 BSH Hausgeraete GmbH,
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
/** \file bal_breaks_dbuscan.c
 *
 * \brief Implementation of generation of breaks to address SystemMaster.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stddef.h>
#include "bal.h"
#include "dbuspresentation.h"
#include "dbus_lock.h"
#include "dbusdll.h"
#include "system_timer.h"
#include "dbuscan_dbus.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/**
 * This define specifies time (in ms), after which another attempt to generate
 * wake up or reset pulse is made, if it is not successful (e.g. because DBus
 * was busy).
 */
#define BAL_BREAK_REPEAT_TIME_MS          (1u)

/**
 * Specifies maximum number of attempts to send out a break signal over DBusCAN chip.
 */
#define BAL_BREAK_ATTEMPTS_MAX           (10u)


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void BAL_GenerateBreakSignal(struct BAL_BreakHandler *breakHandler, const struct BAL_BreakRecipe breakRecipe[], STDCB_CallbackFunction_t breakCbFunction, void (*notifyFunction)(void));
static int32_t BAL_WakeUpBreakCallbackFunction(void *obj, uint32_t flags, int32_t data);
static int32_t BAL_ResetBreakCallbackFunction(void *obj, uint32_t flags, int32_t data);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void BAL_vSendWakeupBreak(void)
{
    static struct BAL_BreakHandler BAL_WakeUpBreakHandler;
    /*start generating WakeUp break signal*/
    BAL_GenerateBreakSignal(&BAL_WakeUpBreakHandler, BAL_WakeupBreakRecipe, &BAL_WakeUpBreakCallbackFunction, DBPL_vNotifyWakeupBreakSent); //lint !e546 taking address of function
}

void BAL_vSendResetBreak(void)
{
    static struct BAL_BreakHandler BAL_ResetBreakHandler;
    /*start generating reset break signal*/
    BAL_GenerateBreakSignal(&BAL_ResetBreakHandler, BAL_ResetBreakRecipe, &BAL_ResetBreakCallbackFunction, NULL); //lint !e546 taking address of function
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief Initiate break generation.
 *
 * \param breakHandler    Logical description of the current stage of the break generation
 * \param breakRecipe     List of stages of the break generation
 * \param breakCbFunction Callback function to be used for generating break signal
 * \param notifyFunction  Notifies dbuspresentation layer about a successful transmission of the break.
 */
static void BAL_GenerateBreakSignal(struct BAL_BreakHandler *breakHandler, const struct BAL_BreakRecipe breakRecipe[], STDCB_CallbackFunction_t breakCbFunction, void (*notifyFunction)(void))
{
    if(breakHandler->stage == 0u)
    {
        breakHandler->recipe = breakRecipe;
        (void)STDCB_InitCallback(&breakHandler->callback, breakCbFunction, breakHandler, STIM_STATUS_TRIGGERED);
        (void)STIM_InitTimer(&breakHandler->timer, STIM_PROCESSING_SCHEDULER, 0u, STIM_MODE_SINGLE, (bool)false, &breakHandler->callback);
        (void)STDCB_ExecuteCallbacks(&breakHandler->callback, STIM_STATUS_TRIGGERED, 0);
        breakHandler->notifyFunction = notifyFunction;
    }
}


/** \brief  Switch between the stages of a WakeUp break generation.
 *
 * \details See STIM module for the concrete meaning of all parameters.
 */
/* Note: This function is called automatically by the timer library and manually from BAL_vSendWakeupBreak() function. */
static int32_t BAL_WakeUpBreakCallbackFunction(void *obj, uint32_t flags, int32_t data)
{
    struct BAL_BreakHandler *breakHandler = (struct BAL_BreakHandler *)obj;/*lint !e926 cast tested to work correctly */
    const struct BAL_BreakRecipe *recipeItem;
    STIM_Time_t reloadTime;

    (void)flags;
    (void)data;

    recipeItem = &breakHandler->recipe[breakHandler->stage];
    reloadTime = recipeItem->delayMs;

    switch(recipeItem->state)
    {
        case BAL_BREAK_START:
            if(false == DBLK_ApplyDbusLock()) /*lock DBus (if it is not already busy)*/
            {
                /*DBus is busy, try again after defined time*/
                reloadTime = BAL_BREAK_REPEAT_TIME_MS;
                break;
            }
            if (DBC_OK != DBCDBUS_sendWakeUpPulse())
            {
                /*DBusCAN is busy, try again after defined time (unless max. number of retries is achieved)*/
                breakHandler->attempts++;
                reloadTime = (breakHandler->attempts >= BAL_BREAK_ATTEMPTS_MAX) ? 0u : BAL_BREAK_REPEAT_TIME_MS;
                DBLK_ReleaseDbusLock(); // relase dbus lock for the next try of the case BAL_BREAK_START
                break;
            }
            breakHandler->attempts = 0u;
            breakHandler->stage++;
            break;
        case BAL_BREAK_END:
            if (true != DBCDBUS_isWakeUpPulseSent())
            {
                /*Transmission of WakeUp Break ongoing, try again after defined time (unless max. number of retries is achieved)*/
                breakHandler->attempts++;
                reloadTime = (breakHandler->attempts >= BAL_BREAK_ATTEMPTS_MAX) ? 0u : BAL_BREAK_REPEAT_TIME_MS;
                break;
            }
            if(breakHandler->notifyFunction != NULL)
            {
                breakHandler->notifyFunction();     // notify upper layer >> WakeUp sent 
            }
            DBLK_ReleaseDbusLock();
            breakHandler->stage = 0u;
            breakHandler->attempts = 0u;
            break;
         default:
            /* no valid state */
            break;
    }

    if(reloadTime > 0u)
    {
        STIM_ReloadTimer(&breakHandler->timer, reloadTime);
        STIM_EnableTimer(&breakHandler->timer);
    }
    else
    {
        if (breakHandler->attempts >= BAL_BREAK_ATTEMPTS_MAX)
        {
            DLL_handleBreakTxFailure(breakHandler);
            DBLK_ReleaseDbusLock();
            breakHandler->stage = 0u;
            breakHandler->attempts = 0u;
        }
    }

    return 0;
}


/** \brief  Switch between the stages of a reset break generation.
 *
 * \details See STIM module for the concrete meaning of all parameters.
 */
/* Note: This function is called automatically by the timer library and manually from BAL_vSendResetBreak() function. */
static int32_t BAL_ResetBreakCallbackFunction(void *obj, uint32_t flags, int32_t data)
{
    struct BAL_BreakHandler *breakHandler = (struct BAL_BreakHandler *)obj;/*lint !e926 cast tested to work correctly */
    const struct BAL_BreakRecipe *recipeItem;
    STIM_Time_t reloadTime;

    (void)flags;
    (void)data;

    recipeItem = &breakHandler->recipe[breakHandler->stage];
    reloadTime = recipeItem->delayMs;

    switch(recipeItem->state)
    {
        case BAL_BREAK_START:
            if(false == DBLK_ApplyDbusLock()) /*lock DBus (if it is not already busy)*/
            {
                /*DBus is busy, try again after defined time*/
                reloadTime = BAL_BREAK_REPEAT_TIME_MS;
                break;
            }
            if (DBC_OK != DBCDBUS_unlockDbusPin(false))
            {
                /*DBusCAN is busy, try again after defined time (unless max. number of retries is achieved)*/
                breakHandler->attempts++;
                reloadTime = (breakHandler->attempts >= BAL_BREAK_ATTEMPTS_MAX) ? 0u : BAL_BREAK_REPEAT_TIME_MS;
                DBLK_ReleaseDbusLock(); // relase dbus lock for the next try of the case BAL_BREAK_START
                break;
            }
            breakHandler->attempts = 0u;
            breakHandler->stage++;
            break;
        case BAL_BREAK_LOW:
            if (DBC_OK != DBCDBUS_setDbusPin(false))
            {
                /*DBusCAN is busy, try again after defined time (unless max. number of retries is achieved)*/
                breakHandler->attempts++;
                reloadTime = (breakHandler->attempts >= BAL_BREAK_ATTEMPTS_MAX) ? 0u : BAL_BREAK_REPEAT_TIME_MS;
                break;
            }
            breakHandler->attempts = 0u;
            breakHandler->stage++;
            break;
        case BAL_BREAK_HIGH:
            if (DBC_OK != DBCDBUS_setDbusPin(true))
            {
                /*DBusCAN is busy, try again after defined time (unless max. number of retries is achieved)*/
                breakHandler->attempts++;
                reloadTime = (breakHandler->attempts >= BAL_BREAK_ATTEMPTS_MAX) ? 0u : BAL_BREAK_REPEAT_TIME_MS;
                break;
            }
            breakHandler->attempts = 0u;
            breakHandler->stage++;
            break;
        case BAL_BREAK_END:
            if (DBC_OK != DBCDBUS_lockDbusPin())
            {
                /*DBusCAN is busy, try again after defined time (unless max. number of retries is achieved)*/
                breakHandler->attempts++;
                reloadTime = (breakHandler->attempts >= BAL_BREAK_ATTEMPTS_MAX) ? 0u : BAL_BREAK_REPEAT_TIME_MS;
                break;
            }
            if(breakHandler->notifyFunction != NULL)
            {
                breakHandler->notifyFunction();
            }
            DBLK_ReleaseDbusLock();
            breakHandler->stage = 0u;
            breakHandler->attempts = 0u;
            break;
         default:
            /* no valid state */
            break;
    }

    if(reloadTime > 0u)
    {
        STIM_ReloadTimer(&breakHandler->timer, reloadTime);
        STIM_EnableTimer(&breakHandler->timer);
    }
    else
    {
        if (breakHandler->attempts >= BAL_BREAK_ATTEMPTS_MAX)
        {
            DLL_handleBreakTxFailure(breakHandler);
            DBLK_ReleaseDbusLock();
            breakHandler->stage = 0u;
            breakHandler->attempts = 0u;
        }
    }

    return 0;
}
