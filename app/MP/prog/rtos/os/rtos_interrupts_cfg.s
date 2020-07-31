;/*******************************************************************************
; *  Copyright (c) 2017 BSH Hausgeraete GmbH,
; *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
; *
; *  All rights reserved. This program and the accompanying materials
; *  are protected by international copyright laws.
; *  Please contact copyright holder for licensing information.
; *
; *******************************************************************************
; *   PROJECT          RTOS Generic SW
; ******************************************************************************/

;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */ 
;/**    RTOS Component                                                     */ 
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/


;/**************************************************************************/
;/**    IMPORT SECTION                                                     */
;/**************************************************************************/

    IMPORT  _tx_thread_context_save
    IMPORT  _tx_thread_context_restore


    AREA ||.text||, CODE, READONLY


;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */ 
;/**   Interrupt Management                                                */
;/*                                                                        */
;/*	   When using RTOS, Interrupts can be handled in two different ways:   */
;/*    Non-Managed and Managed.                                            */
;/*                                                                        */
;/*    A Non-Managed Interrupt is a regular Interrupt and requires no      */ 
;/*    special handling. The limitation is that the user should not use    */ 
;/*    any RTOS services from it. For example, sending a message to a      */ 
;/*    queue, setting an Event Flag, entering a CCS with MUTEX etc.        */ 
;/*                                                                        */ 
;/*    Therefore, Managed Interrupts are needed if RTOS Services are to be */ 
;/*    used from an Interrupt Handler. Runtime of Managed Interrupts can   */ 
;/*    also be monitored using RTOS Time Monitor.                          */ 
;/*                                                                        */ 
;/*    Please use following Template for Managed Interrupt Configuration.  */ 
;/*                                                                        */
;/**************************************************************************/
;/**************************************************************************/
;/**
; *     \brief      Configuration of Managed Interrupts
; *
; *     \details    Step 1: Define a C-function anywhere in Application code,
; *                         which would handle the Interrupt Handler work.
; *                 Step 2: Import this C-function above in IMPORT SECTION.
; *                         Example:- IMPORT user_interrupt_function
; *                 Step 3: Copy-Paste the following Template and remove all commas.
; *                 Step 4: Replace the '_Interrupt_Handler' at both the places with
; *                         the Interrupt Handler function which needs to be managed.
; *                 Step 5: Replace the 'user_interrupt_function' with the imported
; *                         C-Function.
; *
; *                 The Interrupt is now configured as a Managed Interrupt.
; *                 The Interrupt Handler work should be done inside the
; *                 User C-function.
; */

; /* TEMPLATE */
;       EXPORT  _Interrupt_Handler             ;//< Interrupt Handler
;_Interrupt_Handler                             ;//< Interrupt Handler
;       PUSH    {lr}
;       BL      _tx_thread_context_save
;       /* Do interrupt handler work here */
;       BL      user_interrupt_function         ;//< User Interrupt C-Function
;       B       _tx_thread_context_restore


;/* Place your Managed Interrupts here */




    ALIGN
    LTORG
    END
