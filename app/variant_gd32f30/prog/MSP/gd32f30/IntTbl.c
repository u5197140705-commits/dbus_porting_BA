/*******************************************************************************
*   Copyright (c) 2023 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        GD32F303
*******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file    IntTbl.c
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

/*
Memory mapping of the interrupt vector table that is partioned
    | ---------------------------------- |\
    |         __VectorsFixed[4]          | \
    |                                    |  >  Fixed Vectors (16 Bytes)
    |       0x8000000 - 0x800000F        | /   in file IntTblArmCM.c
    | ---------------------------------- |/
    |             Reserved               |
    |         for ROM Bootloader         |
    |        0x8000008 - 0x80007FF       |
    | ---------------------------------- |

    | ---------------------------------- |
    |        ITBL_coreVectors[16]        |
    |         moved Vector Table         |
    |  (0x8000000 - 0x800003F) + Offset  |
    | ---------------------------------- |

    | ---------------------------------- |
    |     ITBL_peripheralVectors[n]      |
    |         moved Vector Table         |
    |  (0x8000040 - 0x800014C) + Offset  |
    | ---------------------------------- |
*/

/* Interrupt Vector table for GD32F303 */
SDEF_SetSegmentConst(vectors)
// Axivion Next Line MisraC2012-8.4: Missing declaration for object definition [ITBL_peripheralVectors[]]
const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
// ||Handler function name||           ||POSITION||     ||Priority||     ||Address offset||
   /*lint -e{923}  cast tested to work correctly for ARM CortexMx */
#if defined(GD32F30)
   WWDG_IRQHandler                  ,     //  0               7                 0x0040
   EXTI_LVD_IRQHandler              ,     //  1               8                 0x0044
   TMPR_IRQHandler                  ,     //  2               9                 0x0048
   RTC_IRQHandler                   ,     //  3              10                 0x004C
   FMC_IRQHandler                   ,     //  4              11                 0x0050
   RCU_CTC_IRQHandler               ,     //  5              12                 0x0054
   EXTI0_IRQHandler                 ,     //  6              13                 0x0058
   EXTI1_IRQHandler                 ,     //  7              14                 0x005C
   EXTI2_IRQHandler                 ,     //  8              15                 0x0060
   EXTI3_IRQHandler                 ,     //  9              16                 0x0064
   EXTI4_IRQHandler                 ,     // 10              17                 0x0068
   DMA0_CH0_IRQHandler              ,     // 11              18                 0x006C
   DMA0_CH1_IRQHandler              ,     // 12              19                 0x0070
   DMA0_CH2_IRQHandler              ,     // 13              20                 0x0074
   DMA0_CH3_IRQHandler              ,     // 14              21                 0x0078
   DMA0_CH4_IRQHandler              ,     // 15              22                 0x007C
   DMA0_CH5_IRQHandler              ,     // 16              23                 0x0080
   DMA0_CH6_IRQHandler              ,     // 17              24                 0x0084
   ADC0_ADC1_IRQHandler             ,     // 18              25                 0x0088
   USBD_HP_OR_CAN0_TX_IRQHandler    ,     // 19              26                 0x008C
   USBD_LP_OR_CAN0_RX0_IRQHandler   ,     // 20              27                 0x0090
   CAN0_RX1_IRQHandler              ,     // 21              28                 0x0094
   CAN0_EWMC_IRQHandler             ,     // 22              29                 0x0098
   EXTI5_9_IRQHandler               ,     // 23              30                 0x009C
   TIM0_BRK_TIM8_IRQHandler         ,     // 24              31                 0x00A0
   TIM0_UP_TIM9_IRQHandler          ,     // 25              32                 0x00A4
   TIM0_TRG_CMT_TIM10_IRQHandler    ,     // 26              33                 0x00A8
   TIM0_CHN_IRQHandler              ,     // 27              34                 0x00AC
   TIM1_IRQHandler                  ,     // 28              35                 0x00B0
   TIM2_IRQHandler                  ,     // 29              36                 0x00B4
   TIM3_IRQHandler                  ,     // 30              37                 0x00B8
   I2C0_EV_IRQHandler               ,     // 31              38                 0x00BC
   I2C0_ER_IRQHandler               ,     // 32              39                 0x00C0
   I2C1_EV_IRQHandler               ,     // 33              40                 0x00C4
   I2C1_ER_IRQHandler               ,     // 34              41                 0x00C8
   SPI0_IRQHandler                  ,     // 35              42                 0x00CC
   SPI1_IRQHandler                  ,     // 36              43                 0x00D0
   USART0_IRQHandler                ,     // 37              44                 0x00D4
   USART1_IRQHandler                ,     // 38              45                 0x00D8
   USART2_IRQHandler                ,     // 39              46                 0x00DC
   EXTI10_15_IRQHandler             ,     // 40              47                 0x00E0
   EXTI_RTC_ALM_IRQHandler          ,     // 41              48                 0x00E4
   EXTI_USBD_WKUP_IRQHandler        ,     // 42              49                 0x00E8
   TIM7_BRK_TIM11_IRQHandler        ,     // 43              50                 0x00EC
   TIM7_UP_TIM12_IRQHandler         ,     // 44              51                 0x00F0
   TIM7_TRG_CMT_TIM13_IRQHandler    ,     // 45              52                 0x00F4
   TIM7_CHN_IRQHandler              ,     // 46              53                 0x00F8
   ADC2_IRQHandler                  ,     // 47              54                 0x00FC
   EXMC_IRQHandler                  ,     // 48              55                 0x0100
   SDIO_IRQHandler                  ,     // 49              56                 0x0104
   TIM4_IRQHandler                  ,     // 50              57                 0x0108
   SPI2_IRQHandler                  ,     // 51              58                 0x010C
   UART3_IRQHandler                 ,     // 52              59                 0x0110
   UART4_IRQHandler                 ,     // 53              60                 0x0114
   TIM5_IRQHandler                  ,     // 54              61                 0x0118
   TIM6_IRQHandler                  ,     // 55              62                 0x011C
   DMA1_CH0_IRQHandler              ,     // 56              63                 0x0120
   DMA1_CH1_IRQHandler              ,     // 57              64                 0x0124
   DMA1_CH2_IRQHandler              ,     // 58              65                 0x0128
   DMA1_CH3_4_IRQHandler            ,     // 59              66                 0x012C
   ITBL_vSpuriousIsr                ,     // 60              67                 0x0130
   ITBL_vSpuriousIsr                ,     // 61              68                 0x0134
   ITBL_vSpuriousIsr                ,     // 62              69                 0x0138
   ITBL_vSpuriousIsr                ,     // 63              70                 0x013C
   ITBL_vSpuriousIsr                ,     // 64              71                 0x0140
   ITBL_vSpuriousIsr                ,     // 65              72                 0x0144
   ITBL_vSpuriousIsr                ,     // 66              73                 0x0148
   ITBL_vSpuriousIsr                ,     // 67              74                 0x014C
#endif
};
SDEF_SetSegmentConst_Default()
