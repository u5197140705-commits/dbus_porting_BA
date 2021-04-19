/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  AUTHOR           LehmannJe/code generator
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
   This file defines the interrupt table that contains the ISR addresses.
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
    |         ITBL_fixedVectors[4]       | \
    |                                    |  >  Fixed Vectors (16 Bytes)
    |       0x8000000 - 0x800000f        | /   in file IntTblArmCM.c
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
    |  (0x8000040 - 0x80000xx) + Offset  |
    | ---------------------------------- |
*/


/* Interrupt Vector table for STM32L4x1. */
SDEF_SetSegmentConst(vectors)
const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
// ||Handler function name||       ||POSITION||           ||Priority||        ||Address offset||
   WWDG_IRQHandler                  ,     //  0               7                 0x0040
   PVD_PVM_IRQHandler               ,     //  1               8                 0x0044
   RTC_TAMP_STAMP_IRQHandler        ,     //  2               9                 0x0048
   RTC_WKUP_IRQHandler              ,     //  3              10                 0x004C
   FLASH_IRQHandler                 ,     //  4              11                 0x0050
   RCC_IRQHandler                   ,     //  5              12                 0x0054
   HINT0_vIsrExtiIrqHandler         ,     //  6              13                 0x0058
   HINT1_vIsrExtiIrqHandler         ,     //  7              14                 0x005C
   HINT2_vIsrExtiIrqHandler         ,     //  8              15                 0x0060
   HINT3_vIsrExtiIrqHandler         ,     //  9              16                 0x0064
   HINT4_vIsrExtiIrqHandler         ,     // 10              17                 0x0068
   DMA1_CH1_IRQHandler              ,     // 11              18                 0x006C
   DMA1_CH2_IRQHandler              ,     // 12              19                 0x0070
   DMA1_CH3_IRQHandler              ,     // 13              20                 0x0074
   DMA1_CH4_IRQHandler              ,     // 14              21                 0x0078
   DMA1_CH5_IRQHandler              ,     // 15              22                 0x007C
   DMA1_CH6_IRQHandler              ,     // 16              23                 0x0080
   DMA1_CH7_IRQHandler              ,     // 17              24                 0x0084
   ADC1_IRQHandler                  ,     // 18              25                 0x0088
   CAN1_TX_IRQHandler               ,     // 19              26                 0x008C
   CAN1_RX0_IRQHandler              ,     // 20              27                 0x0090
   CAN1_RX1_IRQHandler              ,     // 21              28                 0x0094
   CAN1_SCE_IRQHandler              ,     // 22              29                 0x0098
   HINT95_vIsrExtiIrqHandler        ,     // 23              30                 0x009C
   HTIM15_vIsrHandleTimerEvent      ,     // 24              31                 0x00A0
   HTIM116_vIsrHandleTimerEvent     ,     // 25              32                 0x00A4
   HTIM17_vIsrHandleTimerEvent      ,     // 26              33                 0x00A8
   HTIM1_vIsrHandleEvent            ,     // 27              34                 0x00AC
   HTIM2_vIsrHandleTimerEvent       ,     // 28              35                 0x00B0
   HTIM3_vIsrHandleTimerEvent       ,     // 29              36                 0x00B4
   HTIM4_vIsrHandleTimerEvent       ,     // 30              37                 0x00B8
   I2C1_EV_IRQHandler               ,     // 31              38                 0x00BC
   I2C1_ER_IRQHandler               ,     // 32              39                 0x00C0
   I2C2_EV_IRQHandler               ,     // 33              40                 0x00C4
   I2C2_ER_IRQHandler               ,     // 34              41                 0x00C8
   HSPI0_vIsrHandleTxRxEvent        ,     // 35              42                 0x00CC
   HSPI1_vIsrHandleTxRxEvent        ,     // 36              43                 0x00D0
   HUART0_vIsrHandleRxTxEvent       ,     // 37              44                 0x00D4
   HUART1_vIsrHandleRxTxEvent       ,     // 38              45                 0x00D8
   HUART2_vIsrHandleRxTxEvent       ,     // 39              46                 0x00DC
   HINT1510_vIsrExtiIrqHandler      ,     // 40              47                 0x00E0
   RTC_ALARM_IRQHandler             ,     // 41              48                 0x00E4
   DFSDM1_FLT3_IRQHandler           ,     // 42              49                 0x00E8
   ITBL_vSpuriousIsr                ,     // 43              50                 0x00EC
   HTIM8_vIsrHandleTimerEvent       ,     // 44              51                 0x00F0
   ITBL_vSpuriousIsr                ,     // 45              52                 0x00F4
   HTIM8_vIsrHandleEvent            ,     // 46              53                 0x00F8
#ifdef STM32L431
   ADC3_IRQHandler                  ,     // 47              54                 0x00FC
#else
   ITBL_vSpuriousIsr                ,     // 47              54                 0x00FC
#endif
   ITBL_vSpuriousIsr                ,     // 48              55                 0x0100
   SDMMC1_IRQHandler                ,     // 49              56                 0x0104
   HTIM5_vIsrHandleTimerEvent       ,     // 50              57                 0x0108
   HSPI2_vIsrHandleTxRxEvent        ,     // 51              58                 0x010C
   HUART3_vIsrHandleRxTxEvent       ,     // 52              59                 0x0110
   ITBL_vSpuriousIsr                ,     // 53              60                 0x0114
   TIM6_DACUNDER_IRQHandler         ,     // 54              61                 0x0118
   TIM7_IRQHandler                  ,     // 55              62                 0x011C
   DMA2_CH1_IRQHandler              ,     // 56              63                 0x0120
   DMA2_CH2_IRQHandler              ,     // 57              64                 0x0124
   DMA2_CH3_IRQHandler              ,     // 58              65                 0x0128
   DMA2_CH4_IRQHandler              ,     // 59              66                 0x012C
   DMA2_CH5_IRQHandler              ,     // 60              67                 0x0130
   DFSDM1_IRQHandler                ,     // 61              68                 0x0134
   DFSDM2_IRQHandler                ,     // 62              69                 0x0138
   DFSDM1_FLT2_IRQHandler           ,     // 63              70                 0x013C
   COMP_IRQHandler                  ,     // 64              71                 0x0140
   LPTIM1_IRQHandler                ,     // 65              72                 0x0144
   LPTIM2_IRQHandler                ,     // 66              73                 0x0148
   USB_FS_IRQHandler                ,     // 67              74                 0x014C
   DMA2_CH6_IRQHandler              ,     // 68              75                 0x0150
   DMA2_CH7_IRQHandler              ,     // 69              76                 0x0154
   HUART10_vIsrHandleRxTxEvent      ,     // 70              77                 0x0158
   QUADSPI_IRQHandler               ,     // 71              78                 0x015C
   I2C3_EV_IRQHandler               ,     // 72              79                 0x0160
   I2C3_ER_IRQHandler               ,     // 73              80                 0x0164
   SAI1_IRQHandler                  ,     // 74              81                 0x0168
   ITBL_vSpuriousIsr                ,     // 75              82                 0x016C
   SWPMI1_IRQHandler                ,     // 76              83                 0x0170
   TSC_IRQHandler                   ,     // 77              84                 0x0174
   LCD_IRQHandler                   ,     // 78              85                 0x0178
   AES_IRQHandler                   ,     // 79              86                 0x017C
   RNG_IRQHandler                   ,     // 80              87                 0x0180
   FPU_IRQHandler                   ,     // 81              88                 0x0184
   CRS_IRQHandler                   ,     // 82              89                 0x0188
   I2C4_EV_IRQHandler               ,     // 83              90                 0x018C
   I2C4_ER_IRQHandler                     // 84              91                 0x0190

};
SDEF_SetSegmentConst_Default()
