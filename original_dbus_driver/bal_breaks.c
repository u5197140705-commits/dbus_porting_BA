/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
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
/** \file
 *
 * \brief Implementation of generation of breaks to address SystemMaster.
 */

#ifdef DOXY_DEVELOPERS_DOC
/** Compiler switch to enable system timer.
 */#define SYSTEM_TIMER_ENABLED
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "bal.h"
#include "dbusdll.h"
#include "dbuspresentation.h"
#include "dbuspresentation_update.h"
#include "dbusmapping.h"
#include <stddef.h>
#include "dbus_lock.h"
#include "system_timer.h"
#include "watchdogtimer/watchdogtimer.h"

#ifdef SYSTEM_TIMER_ENABLED
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/**
 * This define specifies time (in ms), after which another attempt to generate
 * wake up or reset pulse is made, if it is not successful (e.g. because DBus
 * was busy).
 */
#define BAL_BREAK_REPEAT_TIME_MS          (1u)


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static int32_t BAL_BreakCallbackFunction(void *obj, uint32_t flags, int32_t data);
static void BAL_GenerateBreakSignal(struct BAL_BreakHandler *breakHandler, const struct BAL_BreakRecipe breakRecipe[], void (*notifyFunction)(void));
#endif /*SYSTEM_TIMER_ENABLED*/

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void BAL_vSendWakeupBreak(void)
{
#if defined(SYSTEM_TIMER_ENABLED)
    static struct BAL_BreakHandler BAL_WakeupBreakHandler;
    /*start generating of wakeup pulse*/
    BAL_GenerateBreakSignal(&BAL_WakeupBreakHandler, BAL_WakeupBreakRecipe, DBPL_vNotifyWakeupBreakSent);

#else /*RX platforms are not yet supported by new timer library. Therefore old solution (0x00 over UART) is still used for these platforms.*/
    while(DLL_bIsDbusCommunicating() == true){};
    DBM_PERIPH_vSetBaudRate(DLL_ucGetStandardUartConfigIndex(),6);/*600 baud for reaching 15ms*/
    /** \todo Check if baud rate of 600 works for your controller, or if you need another solution.
     *        Alternatively, you can try baud rate 900 to reach at least 10ms in length.
     *        If that is not working as well, toggle a pin.
     */

    DBM_UART_vSetTxData(0x00U);
    while(DBM_UART_bIsRxOnGoing())
    {
        WDT_trigger();
    }
    DBPL_vNotifyWakeupBreakSent();

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
    /*Assumption, that during update no wake up break will be sent, so standard baud rate*/
    DBPL_vConfigureBaudrate(DBPL_uiGetStandardBaudRate());
#else
    DBM_PERIPH_vSetBaudRate(DLL_ucGetStandardUartConfigIndex(), (uint16_t)DBUS_DEFAULT_BAUDRATE);
#endif /*DBUS2_UPDATE*/
#endif /*!SYSTEM_TIMER_ENABLED*/
}

void BAL_vSendResetBreak(void)
{
#ifdef SYSTEM_TIMER_ENABLED
    static struct BAL_BreakHandler BAL_ResetBreakHandler;
    /*start generating of reset pulse*/
    BAL_GenerateBreakSignal(&BAL_ResetBreakHandler, BAL_ResetBreakRecipe, NULL);
#endif /*SYSTEM_TIMER_ENABLED*/
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

#ifdef SYSTEM_TIMER_ENABLED
/** \brief               Initiate break generation.
 *
 * \param breakHandler   Logical description of the current stage of the break generation
 * \param breakRecipe    List of stages of the break generation
 * \param notifyFunction Notifies dbuspresentation layer about a successful transmission of the break.
 */
static void BAL_GenerateBreakSignal(struct BAL_BreakHandler *breakHandler, const struct BAL_BreakRecipe breakRecipe[], void (*notifyFunction)(void))
{
    if(breakHandler->stage == 0u)
    {
        breakHandler->recipe = breakRecipe;
        (void)STDCB_InitCallback(&breakHandler->callback, BAL_BreakCallbackFunction, breakHandler, STIM_STATUS_TRIGGERED);
        (void)STIM_InitTimer(&breakHandler->timer, STIM_PROCESSING_SCHEDULER, 0u, STIM_MODE_SINGLE, (bool)false, &breakHandler->callback);
        (void)STDCB_ExecuteCallbacks(&breakHandler->callback, STIM_STATUS_TRIGGERED, 0);
        breakHandler->notifyFunction = notifyFunction;
    }
}

/** \brief  Switch between the stages of a break generation.
 *
 * \details See STIM module for the concrete meaning of all parameters.
 */
/* Note: This function is called automatically by the timer library and manually from BAL_vSendWakeupBreak/BAL_vSendResetBreak function. */
static int32_t BAL_BreakCallbackFunction(void *obj, uint32_t flags, int32_t data)
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
        case BAL_BREAK_START:  /*was pulse generation started?*/
            if(false == DBLK_ApplyDbusLock()) /*lock DBus (if it is not already busy)*/
            {
                /*DBus is busy, try again after defined time*/
                reloadTime = BAL_BREAK_REPEAT_TIME_MS;
                break;
            }
            /*USART Tx pin set to GPIO output mode with log.0*/
            DBM_GPIO_SetBreakPinModeOutput();
            DBM_GPIO_SetBreakPinState(false);
            breakHandler->stage++;
            break;
        case BAL_BREAK_LOW:
            /*set USART Tx pin to log.0*/
            DBM_GPIO_SetBreakPinState(false);
            breakHandler->stage++;
            break;
        case BAL_BREAK_HIGH:
            /*set USART Tx pin to log.1*/
            DBM_GPIO_SetBreakPinState(true);
            breakHandler->stage++;
            break;
        case BAL_BREAK_END:
            /*reset pulses are completed. Switch pin back to USART Tx and unlock dBus*/
            DBM_GPIO_SetBreakPinModeAlternateFunction();
            if(breakHandler->notifyFunction != NULL)
            {
                breakHandler->notifyFunction();
            }
            DBLK_ReleaseDbusLock();
            breakHandler->stage = 0u;
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

    return 0;
}
#endif /*SYSTEM_TIMER_ENABLED*/
