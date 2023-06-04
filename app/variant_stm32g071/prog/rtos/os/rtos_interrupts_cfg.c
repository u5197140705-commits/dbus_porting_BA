/**************************************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 **************************************************************************************************
 *   PROJECT          RTOS (Generic SW)
 *   AUTHOR           Handy
 *   CREATED          02.11.2022
 **************************************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Configurations for module rtos_interrupts_configuration.

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include "os/rtos_interrupts_cfg.h"

#include "tx_user.h"
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
#include "tx_port.h"
#include "tx_execution_profile.h"
#endif

/* place application includes below */



/***************************************************************************************/
/***************************************************************************************/
/*                                                                                     */
/*    Interrupt Management                                                             */
/*                                                                                     */
/*    >> "Kernel-Aware" and "Kernel-Unaware" Interrupts:                               */
/*                                                                                     */
/*    The RTOS ports to ARM disables interrupts inside kernel in order to              */
/*    access critical sections of code and re-enable interrupts when done.             */
/*    Therefore, the interrupts are divided in to                                      */
/*    "kernel-unaware" interrupts, which are never disabled and                        */
/*    "kernel-aware" interrupts, which are disabled in the kernel critical sections.   */
/*                                                                                     */
/*    -> Only "kernel-aware" interrupts are allowed to call RTOS services.             */
/*       For example, sending a message to a queue, setting an Event Flag,             */
/*       entering a CCS with MUTEX etc.                                                */
/*    -> "Kernel-unaware" interrupts are not allowed to call any RTOS services.        */
/*                                                                                     */
/*    Note: RTOS ports to ARM use PRIMASK or BASEPRI register to disable interrupts.   */
/*          Please refer to RTOS port configuration to get the applied configuration.  */
/*                                                                                     */
/*                                                                                     */
/*    >> Managed and Non-Managed interrupts:                                           */
/*                                                                                     */
/*    Some RTOSes also requires "managed" interrupts.                                  */
/*    If this is the case, only "managed" AND "kernel-aware" interrupts are allowed    */
/*    to call RTOS services.                                                           */
/*                                                                                     */
/*    Managed interrupt is an Interrupt Service Routine (ISR) with additional logic.   */
/*    The additional logic is provided by RTOS and is intended to ensure               */
/*    that the data processed in the kernel (e.g. thread context, CPU registers, ...)  */
/*    remains consistent while RTOS services are called form ISR context.              */
/*    E.g:                                                                             */
/*    - ThreadX version 5.x requires "managed" interrupts.                             */
/*      The additional logic is provided inside _tx_thread_context_save and            */
/*      _tx_thread_context_restore APIs.                                               */
/*    - ThreadX version 6.x does not requires "managed" interrupts.                    */
/*                                                                                     */
/*    The Template below is intended for ThreadX version 6.x onwards.                  */
/*                                                                                     */
/*    Conclusion:                                                                      */
/*    Managed interrupts and "kernel-aware" interrupts are two sides of the same coin. */
/*    If required, both properties have to be fulfilled if RTOS services are to be     */
/*    used from an Interrupt Handler.                                                  */
/*                                                                                     */
/*                                                                                     */
/*    >> Further ISR instrumentalization:                                              */
/*                                                                                     */
/*    Runtime of Interrupts can be monitored using RTOS Time Monitor.                  */
/*    (see ISR runtime record below)                                                   */
/*                                                                                     */
/*                                                                                     */
/*    Please use following Template for Interrupt instrumentalization.                 */
/*                                                                                     */
/***************************************************************************************/
/***************************************************************************************/

/**
 *     \brief      Interrupt instrumentalization
 *
 *     \details    Step 1: Define a C-function anywhere in Application code,
 *                         which would handle the Interrupt Handler work.
 *                 Step 2: Import this C-function declaration
 *                         - including the header file above inside INCLUDES section or
 *                         - add extern declaration below, inside EXTERN VARIABLE DEFINTIONS section.
 *                 Step 3: Copy-Paste the provided Template.
 *                         Inside rtos_interrupts_cfg.h to have the function declaration.
 *                         In this file below to have the function definition.
 *                 Step 4: Replace the 'RTOS_vExampleInterruptHandler()' at both the places with
 *                         the Interrupt Handler function which needs to be managed.
 *                 Step 5: Replace the 'EXA_vUserInterruptHandler()' with the imported
 *                         C-Function.
 *
 *                 The Interrupt is now instrumentalized.
 *                 The Interrupt Handler work should be done inside the
 *                 User C-function.
 *
 *                 Note:
 *                 The function pair
 *                     _tx_execution_isr_enter
 *                     _tx_execution_isr_exit
 *                 is used to record the ISR runtime.
 *                 The processor load is calculated out from ISR and Thread runtime.
 *                 > If needed, this function pair can be omitted.
 *                 The consequence is then, the ISR runtime is not explicit taken into processor load calculation.
 *                 The ISR runtime has then following impact:
 *                 ISR while Idle time:
 *                     ISR runtime is omitted and the calculation assumes idle time.
 *                     The calculated processor load is less than it really is.
 *                 ISR while thread is active:
 *                     The ISR runtime is implicit added to the calculated thread runtime.
 *                     In this case the calculated processor load is correct, but not the thread runtime.
 */


/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                                         */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL VARIABLE DEFINTIONS                                                                      */
/**************************************************************************************************/

/**************************************************************************************************/
/* EXTERN VARIABLE DEFINTIONS                                                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL FUNCTION DEFINITIONS                                                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/
//lint -save -e9059 "C comment contains C++ comment"

/* TEMPLATE SECTION */
/*
void RTOS_vExampleInterruptHandler(void)
{
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    _tx_execution_isr_enter();                  // Call the ISR enter function
#endif

    // Do interrupt handler work here
    // EXA_vUserInterruptHandler();

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    _tx_execution_isr_exit();                   // Call the ISR exit function
#endif
}
*/

//lint -restore -e9059

/* place application interrupt handler below */



/*************************************** End of File **********************************************/
