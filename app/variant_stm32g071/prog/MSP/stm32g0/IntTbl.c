/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        STM32G0
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
    |  (0x8000040 - 0x80000CF) + Offset  |
    | ---------------------------------- |
*/

/*lint -esym(528,ITBL_peripheralVectors) "Warning 528: Symbol ... not referenced" */

/* Interrupt Vector table for STM32G0_v0r0. */
SDEF_SetSegmentConst(vectors)
static const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
// ||Handler function name||       ||POSITION||        ||Priority||        ||Address offset||
   /*lint -e{923, 9029}  cast tested to work correctly for ARM CortexMx */
#if defined(STM32G030)
   WWDG_IRQHandler                  ,     //  0               7                 0x0040
   ITBL_vSpuriousIsr                ,     //  1               8                 0x0044
   RTC_TAMP_IRQHandler              ,     //  2               9                 0x0048
   FLASH_IRQHandler                 ,     //  3              10                 0x004C
   RCC_IRQHandler                   ,     //  4              11                 0x0050
   EXTI0_1_IRQHandler               ,     //  5              12                 0x0054
   EXTI2_3_IRQHandler               ,     //  6              13                 0x0058
   EXTI4_15_IRQHandler              ,     //  7              14                 0x005C
   ITBL_vSpuriousIsr                ,     //  8              15                 0x0060
   DMA_Channel1_IRQHandler          ,     //  9              16                 0x0064
   DMA_Channel2_3_IRQHandler        ,     // 10              17                 0x0068
   DMA_Channel4_5_6_7_IRQHandler    ,     // 11              18                 0x006C
   ADC_COMP_IRQHandler              ,     // 12              19                 0x0070
   TIM1_BRK_UP_TRG_COMP_IRQHandler  ,     // 13              20                 0x0074
   TIM1_CC_IRQHandler               ,     // 14              21                 0x0078
   ITBL_vSpuriousIsr                ,     // 15              22                 0x007C
   TIM3_IRQHandler                  ,     // 16              23                 0x0080
   ITBL_vSpuriousIsr                ,     // 17              24                 0x0084
   ITBL_vSpuriousIsr                ,     // 18              25                 0x0088
   TIM14_IRQHandler                 ,     // 19              26                 0x008C
   ITBL_vSpuriousIsr                ,     // 20              27                 0x0090
   TIM16_IRQHandler                 ,     // 21              28                 0x0094
   TIM17_IRQHandler                 ,     // 22              29                 0x0098
   I2C1_IRQHandler                  ,     // 23              30                 0x009C
   I2C2_IRQHandler                  ,     // 24              31                 0x00A0
   SPI1_IRQHandler                  ,     // 25              32                 0x00A4
   SPI2_IRQHandler                  ,     // 26              33                 0x00A8
   USART1_IRQHandler                ,     // 27              34                 0x00AC
   USART2_IRQHandler                ,     // 28              35                 0x00B0

#elif defined(STM32G0B0)
   WWDG_IRQHandler                        ,     //  0               7                 0x0040
   ITBL_vSpuriousIsr                      ,     //  1               8                 0x0044
   RTC_STAMP_IRQHandler                   ,     //  2               9                 0x0048
   FLASH_IRQHandler                       ,     //  3              10                 0x004C
   RCC_IRQHandler                         ,     //  4              11                 0x0050
   EXTI0_1_IRQHandler                     ,     //  5              12                 0x0054
   EXTI2_3_IRQHandler                     ,     //  6              13                 0x0058
   EXTI4_15_IRQHandler                    ,     //  7              14                 0x005C
   USB_IRQHandler                         ,     //  8              15                 0x0060
   DMA_Channel1_IRQHandler                ,     //  9              16                 0x0064
   DMA_Channel2_3_IRQHandler              ,     // 10              17                 0x0068
   DMA_Channel4_5_6_7_IRQHandler          ,     // 11              18                 0x006C
   ADC_IRQHandler                         ,     // 12              19                 0x0070
   TIM1_BRK_UP_TRG_COMP_IRQHandler        ,     // 13              20                 0x0074
   TIM1_CC_IRQHandler                     ,     // 14              21                 0x0078
   ITBL_vSpuriousIsr                      ,     // 15              22                 0x007C
   TIM3_TIM4_IRQHandler                   ,     // 16              23                 0x0080
   TIM6_IRQHandler                        ,     // 17              24                 0x0084
   TIM7_IRQHandler                        ,     // 18              25                 0x0088
   TIM14_IRQHandler                       ,     // 19              26                 0x008C
   TIM15_IRQHandler                       ,     // 20              27                 0x0090
   TIM16_IRQHandler                       ,     // 21              28                 0x0094
   TIM17_IRQHandler                       ,     // 22              29                 0x0098
   I2C1_IRQHandler                        ,     // 23              30                 0x009C
   I2C2_I2C3_IRQHandler                   ,     // 24              31                 0x00A0
   SPI1_IRQHandler                        ,     // 25              32                 0x00A4
   SPI2_SPI3_IRQHandler                   ,     // 26              33                 0x00A8
   USART1_IRQHandler                      ,     // 27              34                 0x00AC
   USART2_IRQHandler                      ,     // 28              35                 0x00B0
   USART3_USART4_USART5_USART6_IRQHandler ,     // 29              36                 0x00B4

#elif defined(STM32G031)
   WWDG_IRQHandler                  ,     //  0               7                 0x0040
   PVD_IRQHandler                   ,     //  1               8                 0x0044
   RTC_TAMP_IRQHandler              ,     //  2               9                 0x0048
   FLASH_IRQHandler                 ,     //  3              10                 0x004C
   RCC_IRQHandler                   ,     //  4              11                 0x0050
   EXTI0_1_IRQHandler               ,     //  5              12                 0x0054
   EXTI2_3_IRQHandler               ,     //  6              13                 0x0058
   EXTI4_15_IRQHandler              ,     //  7              14                 0x005C
   ITBL_vSpuriousIsr                ,     //  8              15                 0x0060
   DMA_Channel1_IRQHandler          ,     //  9              16                 0x0064
   DMA_Channel2_3_IRQHandler        ,     // 10              17                 0x0068
   DMA_Channel4_5_6_7_IRQHandler    ,     // 11              18                 0x006C
   ADC_COMP_IRQHandler              ,     // 12              19                 0x0070
   TIM1_BRK_UP_TRG_COMP_IRQHandler  ,     // 13              20                 0x0074
   TIM1_CC_IRQHandler               ,     // 14              21                 0x0078
   TIM2_IRQHandler                  ,     // 15              22                 0x007C
   TIM3_IRQHandler                  ,     // 16              23                 0x0080
   ITBL_vSpuriousIsr                ,     // 17              24                 0x0084
   ITBL_vSpuriousIsr                ,     // 18              25                 0x0088
   TIM14_IRQHandler                 ,     // 19              26                 0x008C
   ITBL_vSpuriousIsr                ,     // 20              27                 0x0090
   TIM16_IRQHandler                 ,     // 21              28                 0x0094
   TIM17_IRQHandler                 ,     // 22              29                 0x0098
   I2C1_IRQHandler                  ,     // 23              30                 0x009C
   I2C2_IRQHandler                  ,     // 24              31                 0x00A0
   SPI1_IRQHandler                  ,     // 25              32                 0x00A4
   SPI2_IRQHandler                  ,     // 26              33                 0x00A8
   USART1_IRQHandler                ,     // 27              34                 0x00AC
   USART2_IRQHandler                ,     // 28              35                 0x00B0
   USART3_USART4_LPUART1_IRQHandler ,     // 29              36                 0x00B4
   CEC_IRQHandler                   ,     // 30              37                 0x00B8
   AES_RNG_IRQHandler               ,     // 31              38                 0x00BC

#elif defined(STM32G051)
   WWDG_IRQHandler                  ,     //  0               7                 0x0040
   PVD_IRQHandler                   ,     //  1               8                 0x0044
   RTC_STAMP_IRQHandler             ,     //  2               9                 0x0048
   FLASH_IRQHandler                 ,     //  3              10                 0x004C
   RCC_IRQHandler                   ,     //  4              11                 0x0050
   EXTI0_1_IRQHandler               ,     //  5              12                 0x0054
   EXTI2_3_IRQHandler               ,     //  6              13                 0x0058
   EXTI4_15_IRQHandler              ,     //  7              14                 0x005C
   ITBL_vSpuriousIsr                ,     //  8              15                 0x0060
   DMA_Channel1_IRQHandler          ,     //  9              16                 0x0064
   DMA_Channel2_3_IRQHandler        ,     // 10              17                 0x0068
   DMA_Channel4_5_6_7_IRQHandler    ,     // 11              18                 0x006C
   ADC_COMP_IRQHandler              ,     // 12              19                 0x0070
   TIM1_BRK_UP_TRG_COMP_IRQHandler  ,     // 13              20                 0x0074
   TIM1_CC_IRQHandler               ,     // 14              21                 0x0078
   TIM2_IRQHandler                  ,     // 15              22                 0x007C
   TIM3_IRQHandler                  ,     // 16              23                 0x0080
   TIM6_DAC_IRQHandler              ,     // 17              24                 0x0084
   TIM7_IRQHandler                  ,     // 18              25                 0x0088
   TIM14_IRQHandler                 ,     // 19              26                 0x008C
   TIM15_IRQHandler                 ,     // 20              27                 0x0090
   TIM16_IRQHandler                 ,     // 21              28                 0x0094
   TIM17_IRQHandler                 ,     // 22              29                 0x0098
   I2C1_IRQHandler                  ,     // 23              30                 0x009C
   I2C2_IRQHandler                  ,     // 24              31                 0x00A0
   SPI1_IRQHandler                  ,     // 25              32                 0x00A4
   SPI2_IRQHandler                  ,     // 26              33                 0x00A8
   USART1_IRQHandler                ,     // 27              34                 0x00AC
   USART2_IRQHandler                ,     // 28              35                 0x00B0

#elif defined(STM32G071)
   WWDG_IRQHandler                  ,     //  0               7                 0x0040
   PVD_IRQHandler                   ,     //  1               8                 0x0044
   RTC_STAMP_IRQHandler             ,     //  2               9                 0x0048
   FLASH_IRQHandler                 ,     //  3              10                 0x004C
   RCC_IRQHandler                   ,     //  4              11                 0x0050
   EXTI0_1_IRQHandler               ,     //  5              12                 0x0054
   EXTI2_3_IRQHandler               ,     //  6              13                 0x0058
   EXTI4_15_IRQHandler              ,     //  7              14                 0x005C
   UCPD1_UCPD2_IRQHandler           ,     //  8              15                 0x0060
   DMA_Channel1_IRQHandler          ,     //  9              16                 0x0064
   DMA_Channel2_3_IRQHandler        ,     // 10              17                 0x0068
   DMA_Channel4_5_6_7_IRQHandler    ,     // 11              18                 0x006C
   ADC_COMP_IRQHandler              ,     // 12              19                 0x0070
   TIM1_BRK_UP_TRG_COMP_IRQHandler  ,     // 13              20                 0x0074
   TIM1_CC_IRQHandler               ,     // 14              21                 0x0078
   TIM2_IRQHandler                  ,     // 15              22                 0x007C
   TIM3_IRQHandler                  ,     // 16              23                 0x0080
   TIM6_DAC_LPTIM1_IRQHandler       ,     // 17              24                 0x0084
   TIM7_LPTIM2_IRQHandler           ,     // 18              25                 0x0088
   TIM14_IRQHandler                 ,     // 19              26                 0x008C
   TIM15_IRQHandler                 ,     // 20              27                 0x0090
   TIM16_IRQHandler                 ,     // 21              28                 0x0094
   TIM17_IRQHandler                 ,     // 22              29                 0x0098
   I2C1_IRQHandler                  ,     // 23              30                 0x009C
   I2C2_IRQHandler                  ,     // 24              31                 0x00A0
   SPI1_IRQHandler                  ,     // 25              32                 0x00A4
   SPI2_IRQHandler                  ,     // 26              33                 0x00A8
   USART1_IRQHandler                ,     // 27              34                 0x00AC
   USART2_IRQHandler                ,     // 28              35                 0x00B0
   USART3_USART4_LPUART1_IRQHandler ,     // 29              36                 0x00B4
   CEC_IRQHandler                   ,     // 30              37                 0x00B8
   AES_RNG_IRQHandler               ,     // 31              38                 0x00BC

#elif defined(STM32G0B1) || defined(STM32G0C1)
   WWDG_IRQHandler                  ,     //  0               7                 0x0040
   PVD_IRQHandler                   ,     //  1               8                 0x0044
   RTC_STAMP_IRQHandler             ,     //  2               9                 0x0048
   FLASH_IRQHandler                 ,     //  3              10                 0x004C
   RCC_IRQHandler                   ,     //  4              11                 0x0050
   EXTI0_1_IRQHandler               ,     //  5              12                 0x0054
   EXTI2_3_IRQHandler               ,     //  6              13                 0x0058
   EXTI4_15_IRQHandler              ,     //  7              14                 0x005C
   UCPD1_UCPD2_IRQHandler           ,     //  8              15                 0x0060
   DMA_Channel1_IRQHandler          ,     //  9              16                 0x0064
   DMA_Channel2_3_IRQHandler        ,     // 10              17                 0x0068
   DMA_Channel4_5_6_7_IRQHandler    ,     // 11              18                 0x006C
   ADC_COMP_IRQHandler              ,     // 12              19                 0x0070
   TIM1_BRK_UP_TRG_COMP_IRQHandler  ,     // 13              20                 0x0074
   TIM1_CC_IRQHandler               ,     // 14              21                 0x0078
   TIM2_IRQHandler                  ,     // 15              22                 0x007C
   TIM3_TIM4_IRQHandler             ,     // 16              23                 0x0080
   TIM6_DAC_LPTIM1_IRQHandler       ,     // 17              24                 0x0084
   TIM7_LPTIM2_IRQHandler           ,     // 18              25                 0x0088
   TIM14_IRQHandler                 ,     // 19              26                 0x008C
   TIM15_IRQHandler                 ,     // 20              27                 0x0090
   TIM16_FDCAN_IT0_IRQHandler       ,     // 21              28                 0x0094
   TIM17_FDCAN_IT1_IRQHandler       ,     // 22              29                 0x0098
   I2C1_IRQHandler                  ,     // 23              30                 0x009C
   I2C2_I2C3_IRQHandler             ,     // 24              31                 0x00A0
   SPI1_IRQHandler                  ,     // 25              32                 0x00A4
   SPI2_SPI3_IRQHandler             ,     // 26              33                 0x00A8
   USART1_IRQHandler                ,     // 27              34                 0x00AC
   USART2_LPUART2_IRQHandler        ,     // 28              35                 0x00B0
   USART3_USART4_USART5_USART6_LPUART1_IRQHandler, // 29     36                 0x00B4
   CEC_IRQHandler                   ,     // 30              37                 0x00B8
   AES_RNG_IRQHandler               ,     // 31              38                 0x00BC

#elif defined(STM32G081)
   WWDG_IRQHandler                  ,     //  0               7                 0x0040
   PVD_IRQHandler                   ,     //  1               8                 0x0044
   RTC_IRQHandler                   ,     //  2               9                 0x0048
   FLASH_IRQHandler                 ,     //  3              10                 0x004C
   RCC_IRQHandler                   ,     //  4              11                 0x0050
   EXTI0_1_IRQHandler               ,     //  5              12                 0x0054
   EXTI2_3_IRQHandler               ,     //  6              13                 0x0058
   EXTI4_15_IRQHandler              ,     //  7              14                 0x005C
   USBPD1_USBPD2_IRQHandler         ,     //  8              15                 0x0060
   DMA_Channel1_IRQHandler          ,     //  9              16                 0x0064
   DMA_Channel2_3_IRQHandler        ,     // 10              17                 0x0068
   DMA_Channel4_5_6_7_IRQHandler    ,     // 11              18                 0x006C
   ADC_COMP_IRQHandler              ,     // 12              19                 0x0070
   TIM1_BRK_UP_TRG_COMP_IRQHandler  ,     // 13              20                 0x0074
   TIM1_CC_IRQHandler               ,     // 14              21                 0x0078
   TIM2_IRQHandler                  ,     // 15              22                 0x007C
   TIM3_IRQHandler                  ,     // 16              23                 0x0080
   TIM6_DAC_LPTIM1_IRQHandler       ,     // 17              24                 0x0084
   TIM7_LPTIM2_IRQHandler           ,     // 18              25                 0x0088
   TIM14_IRQHandler                 ,     // 19              26                 0x008C
   TIM15_IRQHandler                 ,     // 20              27                 0x0090
   TIM16_IRQHandler                 ,     // 21              28                 0x0094
   TIM17_IRQHandler                 ,     // 22              29                 0x0098
   I2C1_IRQHandler                  ,     // 23              30                 0x009C
   I2C2_IRQHandler                  ,     // 24              31                 0x00A0
   SPI1_IRQHandler                  ,     // 25              32                 0x00A4
   SPI2_IRQHandler                  ,     // 26              33                 0x00A8
   USART1_IRQHandler                ,     // 27              34                 0x00AC
   USART2_IRQHandler                ,     // 28              35                 0x00B0
   USART3_USART4_LPUART1_IRQHandler ,     // 29              36                 0x00B4
   CEC_IRQHandler                   ,     // 30              37                 0x00B8
   AES_RNG_IRQHandler               ,     // 31              38                 0x00BC

#else
#endif
};
SDEF_SetSegmentConst_Default()
