/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          IC Dbus2
 *  AUTHOR           Rune Holen
 *  COMP_ABBREV      DBM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *     Mapping library for D-Bus-2 data link layer using one specific UART channel
 *
 *     The mapping layer needs about 40 byte extra ROM (RAM unchanged) for implementation of the following mapping
 *     functions (compared to using macros, hence no mapping layer):
 *     \li DBM_UART_vInit(uint8_t ucIndex)
 *     \li DBM_UART_vEnable(void)
 *     \li DBM_UART_vDisableTxEvent(void)
 *     \li DBM_UART_vEnableTxEvent(void)
 *     \li DBM_UART_vDisableRxEvent(void)
 *     \li DBM_UART_vEnableRxEvent(void)
 *     \li DBM_UART_bIsRxError(void)
 *     \li DBM_UART_ucGetStatus(void)
 *     \li DBM_UART_ucGetRxData(void)
 *     \li DBM_UART_vSetTxData(uint8_t Data)
 *     \li DBM_UART_bIsRxOnGoing(void)\n\n
 *     And in the \b opposite direction, mapping of:
 *     \li DLL_vHandleRxEvent(void)
 *     \li DLL_vHandleTxEvent(void)\n
 *     through HUART<CHANNEL>_vHandleRxEvent() and HUART<CHANNEL>_vHandleTxEvent()
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"


/*   Available compiler switches (to be handled by the make process)
__DBM_DBUS_MAPPING_INCLUDE_FUNCSAFE
__DBM_USE_IRQ_FOR_IDLE_DETECTION
__DBM_INITIAL_MANUAL_TX_INT_CALL_NEEDED
__DBM_DBUS_MAPPING_LOCK_TX_DISABLE
__DBM_DBUS_MAPPING_LOCK_RX_DISABLE
DBUS_UART_CHANNEL
__DBM_EXCLUDE_ERROR_COUNTERS
*/
#ifdef DOXY_DEVELOPERS_DOC
/* Doxygen documentation of compiler switches */

/** Compiler switch. When defined functional safety is regarded.
*/ #define __DBM_DBUS_MAPPING_INCLUDE_FUNCSAFE

/** Compiler switch. When defined an external interrupt is used for
    determining IDLE state. This is usedful for controller, which don't have
    ReceiveOngoing or IDLE flags available.
*/ #define __DBM_USE_IRQ_FOR_IDLE_DETECTION

/** Compiler switch handled by the make process. When defined initally the
    transmit interrupt handler will be called manually. This is due to the fact
    that some controllers do not set the interrupt pending flag after enabling
    the transmit interrupt, when the transmit buffer is empty.
*/ #define __DBM_INITIAL_MANUAL_TX_INT_CALL_NEEDED

/** Compiler switch. When defined RxInterrupt is not disabled, when requested, but locked.
*/ #define __DBM_DBUS_MAPPING_LOCK_RX_DISABLE

/** Compiler switch. When defined TxInterrupt is not disabled, when requested, but locked.
*/ #define __DBM_DBUS_MAPPING_LOCK_TX_DISABLE

/** Compiler switch. Definition needed for mapping D-Bus-2 mapping layer to a specific DBUS_UART_CHANNEL. This defined D-Bus-2 for usage of the first UART channel.
*/ #define DBUS_UART_CHANNEL 0

/** Compiler switch for removing the defined UART error counters. Usage introduced to the D-Bus-2 Light master software - please keep in mind that this compiler switch currently (as of November 2009) will only support D-Bus-2 Light master (D-Bus-2 Light slave can also be considered, but this is not yet implemented). Error counters include errors reported by the UART, e.g. framing error, parity error, overrun error and noise error (noise error is not available for all platforms).
*/ #define __DBM_EXCLUDE_ERROR_COUNTERS
#endif

#include "libtypes.h"
#ifdef PSOC
#include "mapping.h"
#endif
#include "hsup.h"
#include "huart.h"
#include "timerlib.h" //Circulating timer.
#ifdef LSW_DBUS_MAPPING_INCLUDE_FUNCSAFE
/* FSF_vIsrDoEntry() and FSF_vIsrDoExit() */
#include "fsafe.h"
#endif

// switch off lint advisories for this file: #/## usage, function like macro
/*lint -e9023 -e9024 */
/*lint -e9026 Function-like macro... [MISRA 2012 Directive 4.9, advisory] */
/*lint -e9032 [MISRA 2012 Rule 10.7, required], behaviour required here for EFR32 error macros definitions*/
/*lint -e774  -e731 -e835 -e587, Infos occuring for queries for RL78, but not critical.*/

#include "dbusmapping.h"
#include "dbuspresentation.h"

/*Macro definitions for building names of functions to map according to used channel (DBM_UART_CHANNEL)*/
#define MAP_UART(channel, function) HUART ## channel ##function
#define DBM_MAP_UART(channel, function) MAP_UART(channel, function)
/*******************************************************************************************************/
#ifdef __DBM_INITIAL_MANUAL_TX_INT_CALL_NEEDED
#if (DBUS_UART_CHANNEL == 0)
#ifdef DBUS2_CONTROLLER_SPECIFIC_MAPPING
/* Section moved to dbusmappingMc.h */
#else
#error Please implement Initial Manual Tx Interrupt Call in the corresponding dbusmappingMc file
#endif

#elif (DBUS_UART_CHANNEL == 1)
#ifdef DBUS2_CONTROLLER_SPECIFIC_MAPPING
/* Section moved to dbusmappingMc.h */
#else
#error Please implement Initial Manual Tx Interrupt Call in the corresponding dbusmappingMc file
#endif

#elif (DBUS_UART_CHANNEL == 2)
#ifdef DBUS2_CONTROLLER_SPECIFIC_MAPPING
/* Section moved to dbusmappingMc.h */
#elif defined(__PSoC5LP)
#define DBM_UART_SET_TX_INTERRUPT() do {CYMAP_CY_INT_SET_PEND_REG |= CYMAP_ISR_UART_2_Tx__INTC_MASK;} while(0)
#else
#error Please implement Initial Manual Tx Interrupt Call in the corresponding dbusmappingMc file
#endif
#endif
#endif // __DBM_INITIAL_MANUAL_TX_INT_CALL_NEEDED


/**************************************************************************************************/
/* Definition of derivative specific issues. */
/* Please make sure the error definition <Terror> is defined for all derivatives, which do not use uint8_t for this purpose */
/* The following definition will be redefined, when needed - otherwise the use of these definitions will have no effect... */
#define DBM_UART_ENABLE_OTHER_INTERRUPTS()   do {} while(0)


/**************************************************************************************************/

#if defined(DBUS2_CONTROLLER_SPECIFIC_MAPPING)
/*Section moved to dbusmappingMc.h*/
#else
#error Definitions for derivative needed
#endif


#ifndef DBM_DERIVATIVE_SPECIFIC_ERROR_TYPE
/** General typedef for Error definitions */
typedef uint8_t Terror;
#else
/** Derivative specific error definition (different from uint8_t) */
typedef DBM_DERIVATIVE_SPECIFIC_ERROR_TYPE Terror;
#endif
#ifdef __DBM_DBUS_MAPPING_LOCK_TX_DISABLE
/*
The following section is used for Renesas adaptation:
Tx interrupt is not disabled, it is just locked, so that to
the overlaying layer the tx interrupt seems to be disabled..*/
/** Value used for signalising that the tx event is NOT locked (the overlaying
   layer believes that the tx interrupt is re-enabled, because DBM_UART_vEnableTxEvent()
   has been called). */
#define DBM_UART_TX_EVENT_ENABLED            0
/** Value used for signalising that the tx event is locked
   (the overlaying layer believes that the tx interrupt is disabled,
    because DBM_UART_vDisableTxEvent() has been called). */
#define DBM_UART_DISABLE_TX_EVENT            1
/** Value used for signalising that a tx event has occurred during the
   time that it has been locked (the overlaying layer believes that the
   tx interrupt is currently disabled, because DBM_UART_vDisableTxEvent()
   has been called). */
#define DBM_UART_TX_EVENT_DURING_LOCK_STATE  2
static uint8_t DBM_UART_ucTxDisabled = 0U;
static BOOL DBM_UART_bTxEventAlreadyEnabled = 0U;
#endif
#ifdef __DBM_DBUS_MAPPING_LOCK_RX_DISABLE
/*
The following section is used for necessary Renesas adaptation:
Rx interrupt is not disabled, it is just locked,
so that to the overlaying layer the rx interrupt seems to be disabled.
*/
/** Value used for signalising that the rx event is NOT locked
   (the overlaying layer believes that the rx interrupt is re-enabled,
    because DBM_UART_vEnableRxEvent() has been called). */
#define DBM_UART_RX_EVENT_ENABLED            0
/** Value used for signalising that the rx event is locked
   (the overlaying layer believes that the rx interrupt is disabled,
    because DBM_UART_vDisableRxEvent() has been called). */
#define DBM_UART_DISABLE_RX_EVENT            1
/** Value used for signalising that an rx event has occurred during the time
   that it has been locked (the overlaying layer believes that the rx interrupt
   is currently disabled, because DBM_UART_vDisableRxEvent() has been called). */
#define DBM_UART_RX_EVENT_DURING_LOCK_STATE  2
/** Status byte used for not calling the rx interrupt handler if the
    rx interrupt has been disabled (the UART interrupt is in fact not disabled,
    but the overlaying layer believes this...)
*/
static uint8_t DBM_UART_ucRxDisabled = 0U;


/** Variable used for keeping a byte, which is received during the time that
    the rx interrupt is locked, according to the state of DBM_UART_ucRxDisabled.
*/
static uint8_t DBM_UART_ucRxData;

/** Variable used for keeping possible errors, which occur during the time that
    the rx interrupt is locked, according to the state of DBM_UART_ucRxDisabled.
*/
static uint8_t DBM_UART_ucRxError;
#endif

#ifndef __DBM_EXCLUDE_ERROR_COUNTERS
/** Please note that the following error counters are incremented when the UART reports an error. After 256 times the value will be 0 again, unless e.g. EDITH or application resets the values */
static uint32_t DBM_UART_ulNoiseCounter = 0U; //!< Counter, which is incremented when corresponding error flag is set by the UART.
static uint32_t DBM_UART_ulFramingErrorCounter = 0U; //!< Counter, which is incremented when corresponding error flag is set by the UART.
static uint8_t DBM_UART_ucOverrunErrorCounter = 0U; //!< Counter, which is incremented when corresponding error flag is set by the UART.
static uint32_t DBM_UART_ulBreakCounter = 0U; //!< Counter, which is incremented when corresponding error flag is set by the UART.
#endif

/************************************************************************/
void DBM_UART_vInit(uint8_t ucIndex)
{
   /* HUART0_vInit(ucIndex) when DBUS_UART_CHANNEL is 0 */
   DBM_MAP_UART(DBUS_UART_CHANNEL, _vInit)(ucIndex);
#ifdef __DBM_USE_IRQ_FOR_IDLE_DETECTION
   /* Disable interrupt */
   DBM_INT_DisableIrq();
   DBM_INT_Init(0U);
      /* Interrupt pending only on falling edges */
   DBM_INT_SetIrqModeFalling();
#ifdef __DBM_DBUS_MAPPING_LOCK_TX_DISABLE
   //The following variable must be reset if the initialisation is performed more than once.
   DBM_UART_bTxEventAlreadyEnabled = 0U;
#endif
#endif
}
/************************************************************************/
void DBM_UART_vEnable(void)
{
   /* HUART0_vEnable() when DBUS_UART_CHANNEL is 0 */
   DBM_UART_DISABLE_INT();
   DBM_MAP_UART(DBUS_UART_CHANNEL, _vEnable)();
   DBM_UART_ENABLE_INT();
}
/************************************************************************/
void DBM_UART_vDisableTxEvent(void)
{
#ifndef __DBM_DBUS_MAPPING_LOCK_TX_DISABLE
   /* HUART0_vDisableTxEvent(); when DBUS_UART_CHANNEL is 0 */
   DBM_UART_DISABLE_INT();
   DBM_MAP_UART(DBUS_UART_CHANNEL, _vDisableTxEvent)();
   DBM_UART_ENABLE_INT();
#else
   DBM_UART_ucTxDisabled = DBM_UART_DISABLE_TX_EVENT;
#endif
}
/************************************************************************/
void DBM_UART_vEnableTxEvent(void)
{
#ifndef __DBM_DBUS_MAPPING_LOCK_TX_DISABLE
   /* HUART0_vEnableTxEvent(); when DBUS_UART_CHANNEL is 0 */
   DBM_UART_DISABLE_INT();
   DBM_MAP_UART(DBUS_UART_CHANNEL, _vEnableTxEvent)();
#ifdef PSoC4
   DBM_MAP_UART(DBUS_UART_CHANNEL, _vSetTxEvent)();
#endif
#ifdef __DBM_INITIAL_MANUAL_TX_INT_CALL_NEEDED
   //lint -e{9029} some platforms uses historic BOOL others bool, tested for both
   if (DBM_MAP_UART(DBUS_UART_CHANNEL, _bIsTxEvent)() == 0U) //if HUART0_bIsTxEvent()
   {
        /* Set TX Interrupt pending. - NB! For Stm32 the interrupt pending flag cannot be set, hence the tx event handler is called directly, and this is only allowed when interrupts are disabled. */
        DBM_UART_SET_TX_INTERRUPT();
   }
#endif
   DBM_UART_ENABLE_INT();
#else
   //__DBM_DBUS_MAPPING_LOCK_TX_DISABLE
   if (DBM_UART_bTxEventAlreadyEnabled == 0U)
   {
      DBM_MAP_UART(DBUS_UART_CHANNEL, _vEnableTxEvent)(); //Enable TxEvent
      DBM_UART_bTxEventAlreadyEnabled = 1U;
   }
   DBM_UART_DISABLE_INT();
   DBM_UART_ucTxDisabled = DBM_UART_TX_EVENT_ENABLED;
#ifdef __DBM_INITIAL_MANUAL_TX_INT_CALL_NEEDED
   //lint -e{9029} some platforms uses historic BOOL others bool, tested for both
   if (DBM_MAP_UART(DBUS_UART_CHANNEL, _bIsTxEvent)() == 0U)
   {
      /* Set TX Interrupt pending. */
      DBM_UART_SET_TX_INTERRUPT();
   }
#endif
   DBM_UART_ENABLE_INT();
#endif
}
/************************************************************************/
void DBM_UART_vDisableRxEvent(void)
{
#ifndef __DBM_DBUS_MAPPING_LOCK_RX_DISABLE
   /* HUART0_vDisableRxEvent(); when DBUS_UART_CHANNEL is 0 */
   DBM_UART_DISABLE_INT();
   DBM_MAP_UART(DBUS_UART_CHANNEL, _vDisableRxEvent)();
   DBM_UART_ENABLE_INT();
#else
   //__DBM_DBUS_MAPPING_LOCK_RX_DISABLE
#ifdef __DBM_TUNNEL_MODE_SUPPORT
#error Tunnel mode is not supported when the Rx/Tx interrupts cannot be disabled (__DBM_DBUS_MAPPING_LOCK_RX_DISABLE)
#endif
   DBM_UART_ucRxDisabled = DBM_UART_DISABLE_RX_EVENT;
#endif
}
/************************************************************************/
void DBM_UART_vEnableRxEvent(void)
{
#ifndef __DBM_DBUS_MAPPING_LOCK_RX_DISABLE
   /* HUART0_vEnableRxEvent(); when DBUS_UART_CHANNEL is 0 */
   DBM_UART_DISABLE_INT();
   DBM_MAP_UART(DBUS_UART_CHANNEL, _vEnableRxEvent)();
   DBM_UART_ENABLE_INT();
#else
   /* Enable RxEvent only once, as this will never be disabled again under this compiler switch setting. */
   static uint8_t DBM_UART_bRxEventAlreadyEnabled;
   DBM_UART_DISABLE_INT();
   if (!DBM_UART_bRxEventAlreadyEnabled)
   {
      DBM_MAP_UART(DBUS_UART_CHANNEL, _vEnableRxEvent)();
      DBM_UART_bRxEventAlreadyEnabled = 1U;
   }
   if (DBM_UART_ucRxDisabled == DBM_UART_RX_EVENT_DURING_LOCK_STATE)
   {
      DLL_vHandleRxEvent();
   }
   DBM_UART_ucRxDisabled = DBM_UART_RX_EVENT_ENABLED;
   DBM_UART_ENABLE_INT();
#endif
}
/************************************************************************/
BOOL DBM_UART_bIsRxError(void)
{
   /* return HUART0_ucGetRxError(); when DBUS_UART_CHANNEL is 0 */
      Terror tError;
#ifdef __DBM_DBUS_MAPPING_LOCK_RX_DISABLE
   DBM_UART_DISABLE_INT();
#endif
   tError = (Terror)DBM_MAP_UART(DBUS_UART_CHANNEL, _ucGetRxError)();
#ifdef DBM_UART_REMOVE_OVERRUN_ERROR
   /* This macro is only defined for Stm32F0 and Stm32F3 platforms and empty for others.
                  In case overrun errors must explicitely be removed on other platforms, macro must be implemented accordingly */
   DBM_UART_vRemoveOverrunError();
#endif
#ifdef __DBM_DBUS_MAPPING_LOCK_RX_DISABLE
   if (DBM_UART_ucRxDisabled == DBM_UART_RX_EVENT_DURING_LOCK_STATE)
   {
      tError = DBM_UART_ucRxError;
   }
   DBM_UART_ENABLE_INT();
#endif
#ifndef __DBM_EXCLUDE_ERROR_COUNTERS
   /* Error counters */
   if (tError != 0U)
   {
      /* NB! Break, Overrun and Framing-error variables are not limited at 255, these counters may overflow, and start at 0 again. */
      if ((tError & DBM_UART_NOISE_ERROR) != 0U)
      {
         DBM_UART_ulNoiseCounter++;
      }
      if ((tError & DBM_UART_RX_OVERRUN_ERROR) != 0U)
      {
         DBM_UART_ucOverrunErrorCounter++;
      }
      if ((tError & DBM_UART_FRAMING_ERROR) != 0U)
      {
         DBM_UART_ulFramingErrorCounter++;
         /* A break also forces a framing error, hence it is adequate to check this only when framing error occured. */
         #if defined(HUART_RX_BREAK)
         if ((DBM_UART_ucGetStatus() & DBM_UART_RX_BREAK) != 0U)
         {
            DBM_UART_ulBreakCounter++;
         }
         #endif
      }
   }
#else
   //No Error counters
#endif
   if ((tError & ~(Terror)DBM_UART_NOISE_ERROR) == 0U) //Ignore noise errors
   {
      return FALSE;   //No relevant error
   }
   else
   {
      return TRUE;    //Relevant error(s) occurred
   }
}
/************************************************************************/
uint8_t DBM_UART_ucGetStatus(void)
{
   uint8_t ucStatus;
   DBM_UART_DISABLE_INT();
   ucStatus = (uint8_t)DBM_MAP_UART(DBUS_UART_CHANNEL, _ucGetStatus)();
   DBM_UART_ENABLE_INT();
   return ucStatus;
   /* return HUART0_ucGetStatus(); when DBUS_UART_CHANNEL is 0  */
}
/************************************************************************/
uint8_t DBM_UART_ucGetRxData(void)
{
   /* return HUART0_ucGetRxData(); when DBUS_UART_CHANNEL is 0 */
   uint8_t ucData;
#ifdef __DBM_DBUS_MAPPING_LOCK_RX_DISABLE
   if (DBM_UART_ucRxDisabled != DBM_UART_RX_EVENT_DURING_LOCK_STATE)
   {
      DBM_UART_DISABLE_INT();
      ucData = DBM_MAP_UART(DBUS_UART_CHANNEL, _ucGetRxData)();
      DBM_UART_ENABLE_INT();
   }
   else
   {
      ucData = DBM_UART_ucRxData;
   }
#else
   ucData = DBM_MAP_UART(DBUS_UART_CHANNEL, _ucGetRxData)();
#endif
   return ucData;
}
void DBM_UART_vSetTxData(uint8_t ucData)
{
   /* HUART0_vSetTxData(Data); when DBUS_UART_CHANNEL is 0 */
#ifdef __DBM_DBUS_MAPPING_LOCK_TX_DISABLE
   DBM_UART_DISABLE_INT();
#endif
   DBM_MAP_UART(DBUS_UART_CHANNEL, _vSetTxData)(ucData);
#ifdef __DBM_DBUS_MAPPING_LOCK_TX_DISABLE
   DBM_UART_ENABLE_INT();
#endif
}
/************************************************************************/
BOOL DBM_UART_bIsRxOnGoing(void)
{
#ifdef __DBM_USE_IRQ_FOR_IDLE_DETECTION
   //Check if RX Line is low
   //If RX Line is low a break is detected on DBus
   //Return True to avoid start send message during Break
    if(DBM_INT_bGetPin() == FALSE)
   {
       DBM_INT_ClearIrq();        // Clear interrupt to avoid delay time after break
       return TRUE;
   }

   else
   {
        DBM_UART_DISABLE_INT();
        if (DBM_INT_IsIrqEvent() != FALSE)
        {
               //Clear pending interrupt.
               //This implementation is only to avoid collisions on DBus
               DBM_INT_ClearIrq();

               DBM_UART_ENABLE_INT();
               return TRUE;
        }
        else
        {
               DBM_UART_ENABLE_INT();
               return FALSE;
        }
   }
#else
   return DBM_MAP_UART(DBUS_UART_CHANNEL, _bIsRxOnGoing)() ? TRUE : FALSE;
#endif
}
/************************************************************************/
void DBM_UART_vHandleRxEvent(void)
{
#ifdef DBM_UART_REMOVE_OVERRUN_ERROR
   uint8_t ucError;
#endif
    /* enable higher priority interrupts */
   DBM_UART_ENABLE_OTHER_INTERRUPTS();
#ifdef LSW_DBUS_MAPPING_INCLUDE_FUNCSAFE
   /* Start time measurement */
   FSF_vIsrDoEntry(FS_ISR_DBUS2_RX);
#endif
#ifdef __DBM_USE_IRQ_FOR_IDLE_DETECTION
   /* Clear pending interrupt. */
   DBM_INT_ClearIrq();
#endif
#ifdef __DBM_DBUS_MAPPING_LOCK_RX_DISABLE
   if (DBM_UART_ucRxDisabled != 0U)
   {
      DBM_UART_ucRxDisabled = DBM_UART_RX_EVENT_DURING_LOCK_STATE;
      DBM_UART_ucRxError    = DBM_MAP_UART(DBUS_UART_CHANNEL, _ucGetRxError)();
      DBM_UART_ucRxData     = DBM_MAP_UART(DBUS_UART_CHANNEL, _ucGetRxData)();
   }
   else
   {
      DLL_vHandleRxEvent();
   }
#else
   DLL_vHandleRxEvent();
#endif
#ifdef LSW_DBUS_MAPPING_INCLUDE_FUNCSAFE
   /* Stop time measurement */
   FSF_vIsrDoExit(FS_ISR_DBUS2_RX);
#endif
#ifdef DBM_UART_REMOVE_OVERRUN_ERROR
   //Work-around for Renesas platform in the case a higher priority interrupt interrupts the receive event between reading of DBM_UART_bIsRxError() and DBM_UART_ucGetRxData(). If this interruption lasts long enough to provoke a UART_Overrun error no further receive interrupts would be activated unless this error condition is removed. Unconditional reading of error flags at this point should not have any side effect (i.e. during normal operation the error flags can never be set at this point).
   ucError = (uint8_t)DBM_MAP_UART(DBUS_UART_CHANNEL, _ucGetRxError)(); // dummy read to make sure Rx Interrupts will be generated even if an overrun error occured between reading of U0RBH and U0RBL (which would cause DBus to stop receiving any data otherwise because of missing Rx interrupts)
   (void)ucError;
#endif
}
/************************************************************************/
/* void HUART0_vHandleTxEvent(void) when DBUS_UART_CHANNEL is 0 */
void DBM_UART_vHandleTxEvent(void)
{
   /* Enable higher priority interrupts */
   DBM_UART_ENABLE_OTHER_INTERRUPTS();
   #ifdef LSW_DBUS_MAPPING_INCLUDE_FUNCSAFE
   /* Start time measurement */
   FSF_vIsrDoEntry(FS_ISR_DBUS2_TX);
#endif
   #ifdef __DBM_DBUS_MAPPING_LOCK_TX_DISABLE
   if (!DBM_UART_ucTxDisabled)
   {
      DLL_vHandleTxEvent();
   }
   else
   {
      DBM_UART_ucTxDisabled = DBM_UART_TX_EVENT_DURING_LOCK_STATE;
   }
#else
   DLL_vHandleTxEvent();
#endif
   #ifdef LSW_DBUS_MAPPING_INCLUDE_FUNCSAFE
   /* Stop time measurement */
   FSF_vIsrDoExit(FS_ISR_DBUS2_TX);
#endif
}
/************************************************************************/
void DBM_UART_vSetBaudRate(uint8_t ucIndex, uint16_t uiBaudRate)
{
    (void)DBM_MAP_UART(DBUS_UART_CHANNEL, _bConfigure)(ucIndex,uiBaudRate,HUART_PARITY_NONE,HUART_STOP_BITS_1);
    DLL_vSetupTimings(uiBaudRate);
}
/************************************************************************/

/* Statistics */
#ifndef __DBM_EXCLUDE_ERROR_COUNTERS
uint32_t DBM_UART_ulGetNoiseCounter(void)
{
   return DBM_UART_ulNoiseCounter;
}
uint8_t DBM_UART_ucGetUartOverrunCounter(void)
{
   return DBM_UART_ucOverrunErrorCounter;
}
uint32_t DBM_UART_ulGetUartFramingErrorCounter(void)
{
   return DBM_UART_ulFramingErrorCounter;
}
uint32_t DBM_UART_ulGetUartBreakCounter(void)
{
   return DBM_UART_ulBreakCounter;
}
#endif

/* Timer functions */
uint32_t DBM_ulGetCircleMicroSeconds(void)
{
   return TIM_u32GetCircleMicroSeconds();
}
