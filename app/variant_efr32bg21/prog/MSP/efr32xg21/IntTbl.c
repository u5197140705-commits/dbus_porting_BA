/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW / SiLabs EFR32xG21
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This file defines the interrupt table that contains the ISR addresses.
*
*   \details  This file was created automatically, no manual changes should be done in this file.
*             If required database or code generator must be adapted
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "processor.h"
#include "IntTbl.h"
#include "SegmentDef.h"



/* Interrupt Vector table for EFR32xG21. */

SDEF_SetSegmentConst(vectors)
const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[] =
{ // ||Handler function name||      ||POSITION||       ||Priority||        ||Address offset||
    /*lint -e{923, 9029}  cast tested to work correctly for ARM CortexMx */
    SETAMPERHOST_IRQHandler      ,    //  0                   7                   0x040
    SEMBRX_IRQHandler            ,    //  1                   8                   0x044
    SEMBTX_IRQHandler            ,    //  2                   9                   0x048
    SMU_SECURE_IRQHandler        ,    //  3                   10                  0x04C
    SMU_PRIVILEGED_IRQHandler    ,    //  4                   11                  0x050
    EMU_IRQHandler               ,    //  5                   12                  0x054
    TIMER0_IRQHandler            ,    //  6                   13                  0x058
    TIMER1_IRQHandler            ,    //  7                   14                  0x05C
    TIMER2_IRQHandler            ,    //  8                   15                  0x060
    TIMER3_IRQHandler            ,    //  9                   16                  0x064
    RTCC_IRQHandler              ,    //  10                  17                  0x068
    UART0_ISR_RX_HandleEvent     ,    //  11                  18                  0x06C
    UART0_ISR_TX_HandleEvent     ,    //  12                  19                  0x070
    UART1_ISR_RX_HandleEvent     ,    //  13                  20                  0x074
    UART1_ISR_TX_HandleEvent     ,    //  14                  21                  0x078
    UART2_ISR_RX_HandleEvent     ,    //  15                  22                  0x07C
    UART2_ISR_TX_HandleEvent     ,    //  16                  23                  0x080
    ICACHE0_IRQHandler           ,    //  17                  24                  0x084
    BURTC_IRQHandler             ,    //  18                  25                  0x088
    LETIMER0_IRQHandler          ,    //  19                  26                  0x08C
    SYSCFG_IRQHandler            ,    //  20                  27                  0x090
    LDMA_IRQHandler              ,    //  21                  28                  0x094
    LFXO_IRQHandler              ,    //  22                  29                  0x098
    LFRCO_IRQHandler             ,    //  23                  30                  0x09C
    ULFRCO_IRQHandler            ,    //  24                  31                  0x0A0
    GPIO_ODD_IRQHandler          ,    //  25                  32                  0x0A4
    GPIO_EVEN_IRQHandler         ,    //  26                  33                  0x0A8
    I2C0_IRQHandler              ,    //  27                  34                  0x0AC
    I2C1_IRQHandler              ,    //  28                  35                  0x0B0
    EMUDG_IRQHandler             ,    //  29                  36                  0x0B4
    EMUSE_IRQHandler             ,    //  30                  37                  0x0B8
    AGC_IRQHandler               ,    //  31                  38                  0x0BC
    BUFC_IRQHandler              ,    //  32                  39                  0x0C0
    FRC_PRI_IRQHandler           ,    //  33                  40                  0x0C4
    FRC_IRQHandler               ,    //  34                  41                  0x0C8
    MODEM_IRQHandler             ,    //  35                  42                  0x0CC
    PROTIMER_IRQHandler          ,    //  36                  43                  0x0D0
    RAC_RSM_IRQHandler           ,    //  37                  44                  0x0D4
    RAC_SEQ_IRQHandler           ,    //  38                  45                  0x0D8
    PRORTC_IRQHandler            ,    //  39                  46                  0x0DC
    SYNTH_IRQHandler             ,    //  40                  47                  0x0E0
    ACMP0_IRQHandler             ,    //  41                  48                  0x0E4
    ACMP1_IRQHandler             ,    //  42                  49                  0x0E8
    WDOG0_IRQHandler             ,    //  43                  50                  0x0EC
    WDOG1_IRQHandler             ,    //  44                  51                  0x0F0
    HFXO00_IRQHandler            ,    //  45                  52                  0x0F4
    HFRCO0_IRQHandler            ,    //  46                  53                  0x0F8
    HFRCOEM23_IRQHandler         ,    //  47                  54                  0x0FC
    CMU_IRQHandler               ,    //  48                  55                  0x100
    AES_IRQHandler               ,    //  49                  56                  0x104
    IADC_IRQHandler              ,    //  50                  57                  0x108
    MSC_IRQHandler               ,    //  51                  58                  0x10C
    DPLL0_IRQHandler             ,    //  52                  59                  0x110
    SW0_IRQHandler               ,    //  53                  60                  0x114
    SW1_IRQHandler               ,    //  54                  61                  0x118
    SW2_IRQHandler               ,    //  55                  62                  0x11C
    SW3_IRQHandler               ,    //  56                  63                  0x120
    KERNEL0_IRQHandler           ,    //  57                  64                  0x124
    KERNEL1_IRQHandler           ,    //  58                  65                  0x128
    M33CTI0_IRQHandler           ,    //  59                  66                  0x12C
    M33CTI1_IRQHandler                //  60                  67                  0x130
};
SDEF_SetSegmentConst_Default()

/* Function's task is to set the interrupt priorities according to the user's needs, different from the default settings.
 * If necessary, uncomment the relevant lines and set the required interrupt priorities
 */
void ITBL_setCustomInterruptPriority(void)
{
    /* Cortex-M33 settable interrupts */
    //NVIC_SetPriority(MemoryManagement_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(BusFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(UsageFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(DebugMonitor_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SVCall_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(PendSV_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SysTick_IRQn, PRIORITY_HIGH);
    /* Peripheral interrupts */
    //NVIC_SetPriority(SETAMPERHOST_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SEMBRX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SEMBTX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SMU_SECURE_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SMU_PRIVILEGED_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EMU_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIMER0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIMER1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIMER2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIMER3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(RTCC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART0_RX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART0_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART1_RX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART1_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART2_RX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART2_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ICACHE0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(BURTC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(LETIMER0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SYSCFG_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(LDMA_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(LFXO_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(LFRCO_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ULFRCO_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(GPIO_ODD_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(GPIO_EVEN_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EMUDG_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EMUSE_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(AGC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(BUFC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FRC_PRI_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FRC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(MODEM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PROTIMER_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(RAC_RSM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(RAC_SEQ_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PRORTC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SYNTH_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ACMP0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ACMP1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(WDOG0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(WDOG1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HFXO00_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HFRCO0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HFRCOEM23_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CMU_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(AES_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(IADC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(MSC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DPLL0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SW0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SW1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SW2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SW3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(KERNEL0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(KERNEL1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(M33CTI0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(M33CTI1_IRQn, PRIORITY_LOW);
}
