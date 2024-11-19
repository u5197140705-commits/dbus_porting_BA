/*******************************************************************************
*   Copyright (c) 2020 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        ARM CortexMx
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Configuration file for customize exception Interrupt handlers
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/


/* AXIVION Disable Style MisraC2012-5.7 : A tag name shall be a unique identifier. */
/* AXIVION Disable Style MisraC2012-5.8 : Identifiers that define objects or functions with external linkage shall be unique. */

#include "bsh_stdinc.h"
#include <stdint.h>
#include "processor.h"
#include "IntTblArmCM.h"
#include "std_lib/std_symbol.h"

/******************************************************************************/
/* LOCAL DATA DEFINITIONS                                                     */
/******************************************************************************/

/* Retrieving context switch information which was saved before */
/* entering ISR (Unstacking)                                    */
/* Stacked registers:  R0, R1, R2, R3, R12, LR, PC, PSR         */
/*                                                              */
/*            / +------+                                        */
/*           /  | xPSR |                                        */
/*          /   +------+      Stacked PC                        */
/*         /    | PC   | <--- at SP+0x18                        */
/*        /     +------+                                        */
/*       /      | LR   |                                        */
/*      /       +------+                                        */
/*     /        | R12  |                                        */
/* Stack Frame  +------+                                        */
/*     \        | R3   |                                        */
/*      \       +------+                                        */
/*       \      | R2   |                                        */
/*        \     +------+                                        */
/*         \    | R1   |                                        */
/*          \   +------+                                        */
/*           \  | R0   |                                        */
/*            \ +------+                                        */

/*lint -esym(551,INT_StackFrame)  Symbol not accessed, for debugging only! */
/*lint -esym(754,INT_StackFrame*)
        "local struct member 'INT_StackFrame::XX' not referenced" */
/*lint -esym(9045,INT_StackFrame)
        "complete definition of 'INT_StackFrame' is unnecessary
        in this translation unit [MISRA 2012 Directive 4.8, advisory] */
volatile struct INT_StackFrame_s
{
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R12;
    uint32_t LR;
    uint32_t PC;
    uint32_t xPSR;
} const *INT_StackFrame;


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static struct INT_StackFrame_s* INT_getStackFrame(void)
{
    /*lint -e{923, 9033} Conversion between pointer type and integer type [MISRA 2012 Rule 11.4, required] Tested on CortexMx */
    /* Axivion Next Line MisraC2012-11.4 */
    return (struct INT_StackFrame_s *)__get_MSP();
}


/******************************************************************************/
/* INTERRUPT FUNCTIONS FOR FAULT HANDLING                                     */
/******************************************************************************/

SYMBOL_IRQ SYMBOL_NO_RETURN void SpuriousIsr_Handler(uint32_t SpuriousInt)
{
   /* in bits [9..0] of the SpuriousInt is spurious interrupt number
    * e.g. 0x13 corresponds to an interrupt handler in the vector table
    * i.e. IRQ3 (0x13 - 0x10 = 0x03)
    * where 0x10 in subtraction represents IRQs offset
    */
    (void) SpuriousInt;

    /* Endless loop - no return from spurious interrupt */
    for(;;) {}
}


SYMBOL_IRQ SYMBOL_NO_RETURN void HardFault_Handler(void)
{
    /* Read stack frame */
    INT_StackFrame = INT_getStackFrame();
    (void) INT_StackFrame;
#if defined (LOG_EXCEPTION_ENABLED)
    INT_logException(INT_StackFrame->LR, INT_HANDLER_TYPE_HARDFAULT);
#endif
    /* Endless loop - no return from fault */
    for(;;) {}
}

#if !defined(NO_ISR_SUPPORT) || defined(need_ModuleHeader)
SYMBOL_IRQ SYMBOL_NO_RETURN void NMI_Handler(void)
{
    /* Read stack frame */
    INT_StackFrame = INT_getStackFrame();
#if defined (LOG_EXCEPTION_ENABLED)
    INT_logException(INT_StackFrame->LR, INT_HANDLER_TYPE_NMI);
#endif
    /* Endless loop - no return from fault */
    for(;;) {}
}
#endif /* #if !defined(NO_ISR_SUPPORT) || defined(need_ModuleHeader) */

#if !defined(NO_ISR_SUPPORT)
#if !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS) && !defined(CORTEX_M23)

SYMBOL_IRQ SYMBOL_NO_RETURN void MemManage_Handler(void)
{
    /* Read stack frame */
    INT_StackFrame = INT_getStackFrame();
#if defined (LOG_EXCEPTION_ENABLED)
    INT_logException(INT_StackFrame->LR, INT_HANDLER_TYPE_MEMMANAGE);
#endif
    /* Endless loop - no return from fault */
    for(;;) {}
}


SYMBOL_IRQ SYMBOL_NO_RETURN void BusFault_Handler(void)
{
    /* Read stack frame */
    INT_StackFrame = INT_getStackFrame();
#if defined (LOG_EXCEPTION_ENABLED)
    INT_logException(INT_StackFrame->LR, INT_HANDLER_TYPE_BUSFAULT);
#endif
    /* Endless loop - no return from fault */
    for(;;) {}
}


SYMBOL_IRQ SYMBOL_NO_RETURN void UsageFault_Handler(void)
{
    /* Read stack frame */
    INT_StackFrame = INT_getStackFrame();
#if defined (LOG_EXCEPTION_ENABLED)
    INT_logException(INT_StackFrame->LR, INT_HANDLER_TYPE_USAGEFAULT);
#endif
    /* Endless loop - no return from fault */
    for(;;) {}
}

#endif /* !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS) && !defined(CORTEX_M23)*/

#endif /* !defined(NO_ISR_SUPPORT) */

#if defined (LOG_EXCEPTION_ENABLED)
SYMBOL_WEAK void INT_logException(uint32_t stackFrameRegister, enum INT_ExceptionHandlerType handlerType)
{
    (void) stackFrameRegister;
    (void) handlerType;
}
#endif //defined (LOG_EXCEPTION_ENABLED)
/* AXIVION Enable Style MisraC2012-5.7 */
/* AXIVION Enable Style MisraC2012-5.8 */

