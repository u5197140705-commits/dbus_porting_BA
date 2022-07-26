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

/*lint -esym(754,INT_StackFrame*)
        "local struct member 'INT_StackFrame::XX' not referenced" */
/*lint -esym(9045,INT_StackFrame)
        "complete definition of 'INT_StackFrame' is unnecessary
        in this translation unit [MISRA 2012 Directive 4.8, advisory] */
struct INT_StackFrame
{
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R12;
    uint32_t LR;
    uint32_t PC;
    uint32_t xPSR;
};


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static struct INT_StackFrame* INT_getStackFrame(void)
{
    /*lint -e{923, 9033} Conversion between pointer type and integer type [MISRA 2012 Rule 11.4, required] Tested on CortexMx */
    /* Axivion Next Line MisraC2012-11.4 */
    return (struct INT_StackFrame *)__get_MSP();
}


/******************************************************************************/
/* INTERRUPT FUNCTIONS FOR FAULT HANDLING                                     */
/******************************************************************************/

void SpuriousIsr_Handler(uint32_t SpuriousInt)
{
   /* in bits [9..0] of the SpuriousInt is spurious interrupt number
    * e.g. 0x13 is for Stm32f10XXX vector table RTC_IRQHandler() 
    * i.e. IRQ3 (0x13 - 0x10 = 0x03)
    * where 0x10 in subtraction represents IRQs offset
    */
    (void) SpuriousInt;
    
    /* Endless loop - no return from spurious interrupt */
    for(;;) {}
}


SYMBOL_IRQ void HardFault_Handler(void)
{
    static volatile struct INT_StackFrame *stackFrame;
    
    /* Read stack frame */
    stackFrame = INT_getStackFrame();
    
    /* Use stackFrame to suppress compiler warning */
    stackFrame = stackFrame;
    
    /* Endless loop - no return from fault */
    for(;;) {}
}


#if !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS)

SYMBOL_IRQ void MemManage_Handler(void)
{
    static volatile struct INT_StackFrame *stackFrame;
    
    /* Read stack frame */
    stackFrame = INT_getStackFrame();
    
    /* Use stackFrame to suppress compiler warning */
    stackFrame = stackFrame;
    
    /* Endless loop - no return from fault */
    for(;;) {}
}


SYMBOL_IRQ void BusFault_Handler(void)
{
    static volatile struct INT_StackFrame *stackFrame;
    
    /* Read stack frame */
    stackFrame = INT_getStackFrame();
    
    /* Use stackFrame to suppress compiler warning */
    stackFrame = stackFrame;
    
    /* Endless loop - no return from fault */
    for(;;) {}
}


SYMBOL_IRQ void UsageFault_Handler(void)
{
    static volatile struct INT_StackFrame *stackFrame;
    
    /* Read stack frame */
    stackFrame = INT_getStackFrame();
    
    /* Use stackFrame to suppress compiler warning */
    stackFrame = stackFrame;
    
    /* Endless loop - no return from fault */
    for(;;) {}
}

#endif /* !defined(CORTEX_M0) && !defined(CORTEX_M0_PLUS) */
