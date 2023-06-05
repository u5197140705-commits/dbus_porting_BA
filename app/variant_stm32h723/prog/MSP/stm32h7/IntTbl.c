/*******************************************************************************
*   Copyright (c) 2022 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        STM32H7
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
    |  (0x8000040 - 0x80000xx) + Offset  |
    | ---------------------------------- |
*/

/* Info 714: external symbol 'ITBL_peripheralVectors' was defined but not referenced
   Info 765: external symbol 'ITBL_peripheralVectors' could be made static [MISRA 2012 Rule 8.7, advisory] */
/*lint -e714 -e765 */


/* Interrupt Vector table for STM32H7. */
SDEF_SetSegmentConst(vectors)
const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
// ||Handler function name||             ||POSITION||    ||Priority||     ||Address offset||
   // Cortex external interrupts
   WWDG1_IRQHandler                 ,     //  0               7                 0x0040
   PVD_PVM_IRQHandler               ,     //  1               8                 0x0044
   RTC_TAMP_STAMP_CSS_LSE_IRQHandler,     //  2               9                 0x0048
   RTC_WKUP_IRQHandler              ,     //  3              10                 0x004C
   FLASH_IRQHandler                 ,     //  4              11                 0x0050
   RCC_IRQHandler                   ,     //  5              12                 0x0054
   EXTI0_IRQHandler                 ,     //  6              13                 0x0058
   EXTI1_IRQHandler                 ,     //  7              14                 0x005C
   EXTI2_IRQHandler                 ,     //  8              15                 0x0060
   EXTI3_IRQHandler                 ,     //  9              16                 0x0064
   EXTI4_IRQHandler                 ,     // 10              17                 0x0068
   DMA_STR0_IRQHandler              ,     // 11              18                 0x006C
   DMA_STR1_IRQHandler              ,     // 12              19                 0x0070
   DMA_STR2_IRQHandler              ,     // 13              20                 0x0074
   DMA_STR3_IRQHandler              ,     // 14              21                 0x0078
   DMA_STR4_IRQHandler              ,     // 15              22                 0x007C
   DMA_STR5_IRQHandler              ,     // 16              23                 0x0080
   DMA_STR6_IRQHandler              ,     // 17              24                 0x0084
   ADC1_2_IRQHandler                ,     // 18              25                 0x0088
   FDCAN1_IT0_IRQHandler            ,     // 19              26                 0x008C
   FDCAN2_IT0_IRQHandler            ,     // 20              27                 0x0090
   FDCAN1_IT1_IRQHandler            ,     // 21              28                 0x0094
   FDCAN2_IT1_IRQHandler            ,     // 22              29                 0x0098
   EXTI9_5_IRQHandler               ,     // 23              30                 0x009C
   TIM1_BRK_IRQHandler              ,     // 24              31                 0x00A0
   TIM1_UP_IRQHandler               ,     // 25              32                 0x00A4
   TIM1_TRG_COM_IRQHandler          ,     // 26              33                 0x00A8
   TIM1_CC_IRQHandler               ,     // 27              34                 0x00AC
   TIM2_IRQHandler                  ,     // 28              35                 0x00B0
   TIM3_IRQHandler                  ,     // 29              36                 0x00B4
   TIM4_IRQHandler                  ,     // 30              37                 0x00B8
   I2C1_EV_IRQHandler               ,     // 31              38                 0x00BC
   I2C1_ER_IRQHandler               ,     // 32              39                 0x00C0
   I2C2_EV_IRQHandler               ,     // 33              40                 0x00C4
   I2C2_ER_IRQHandler               ,     // 34              41                 0x00C8
   SPI1_IRQHandler                  ,     // 35              42                 0x00CC
   SPI2_IRQHandler                  ,     // 36              43                 0x00D0
   USART1_IRQHandler                ,     // 37              44                 0x00D4
   USART2_IRQHandler                ,     // 38              45                 0x00D8
   USART3_IRQHandler                ,     // 39              46                 0x00DC
   EXTI15_10_IRQHandler             ,     // 40              47                 0x00E0
   RTC_ALARM_IRQHandler             ,     // 41              48                 0x00E4
   ITBL_vSpuriousIsr                ,     // 42              49                 0x00E8
   TIM8_BRK_TIM12_IRQHandler        ,     // 43              50                 0x00EC
   TIM8_UP_TIM13_IRQHandler         ,     // 44              51                 0x00F0
   TIM8_TRG_COM_TIM14_IRQHandler    ,     // 45              52                 0x00F4
   TIM8_CC_IRQHandler               ,     // 46              53                 0x00F8
   DMA1_STR7_IRQHandler             ,     // 47              54                 0x00FC
   FMC_IRQHandler                   ,     // 48              55                 0x0100
   SDMMC1_IRQHandler                ,     // 49              56                 0x0104
   TIM5_IRQHandler                  ,     // 50              57                 0x0108
   SPI3_IRQHandler                  ,     // 51              58                 0x010C
   UART4_IRQHandler                 ,     // 52              59                 0x0110
   UART5_IRQHandler                 ,     // 53              60                 0x0114
   TIM6_DAC_IRQHandler              ,     // 54              61                 0x0118
   TIM7_IRQHandler                  ,     // 55              62                 0x011C
   DMA2_STR0_IRQHandler             ,     // 56              63                 0x0120
   DMA2_STR1_IRQHandler             ,     // 57              64                 0x0124
   DMA2_STR2_IRQHandler             ,     // 58              65                 0x0128
   DMA2_STR3_IRQHandler             ,     // 59              66                 0x012C
   DMA2_STR4_IRQHandler             ,     // 60              67                 0x0130
   ETH_IRQHandler                   ,     // 61              68                 0x0134
   ETH_WKUP_IRQHandler              ,     // 62              69                 0x0138
   FDCAN_CAL_IRQHandler             ,     // 63              70                 0x013C
   ITBL_vSpuriousIsr                ,     // 64              71                 0x0140
   ITBL_vSpuriousIsr                ,     // 65              72                 0x0144
   ITBL_vSpuriousIsr                ,     // 66              73                 0x0148
   ITBL_vSpuriousIsr                ,     // 67              74                 0x014C
   DMA2_STR5_IRQHandler             ,     // 68              75                 0x0150
   DMA2_STR6_IRQHandler             ,     // 69              76                 0x0154
   DMA2_STR7_IRQHandler             ,     // 70              77                 0x0158
   USART6_IRQHandler                ,     // 71              78                 0x015C
   I2C3_EV_IRQHandler               ,     // 72              79                 0x0160
   I2C3_ER_IRQHandler               ,     // 73              80                 0x0164
   ITBL_vSpuriousIsr                ,     // 74              81                 0x0168
   ITBL_vSpuriousIsr                ,     // 75              82                 0x016C
   ITBL_vSpuriousIsr                ,     // 76              83                 0x0170
   ITBL_vSpuriousIsr                ,     // 77              84                 0x0174
   DCMI_IRQHandler                  ,     // 78              85                 0x0178
   ITBL_vSpuriousIsr                ,     // 79              86                 0x017C
   ITBL_vSpuriousIsr                ,     // 80              87                 0x0180
   FPU_IRQHandler                   ,     // 81              88                 0x0184
   UART7_IRQHandler                 ,     // 82              89                 0x0188
   UART8_IRQHandler                 ,     // 83              90                 0x018C
   SPI4_IRQHandler                  ,     // 84              91                 0x0190
   SPI5_IRQHandler                  ,     // 85              92                 0x0194
   SPI6_IRQHandler                  ,     // 86              93                 0x0198
   SAI1_IRQHandler                  ,     // 87              94                 0x019C
   LTDC_IRQHandler                  ,     // 88              95                 0x01A0
   LTDC_ER_IRQHandler               ,     // 89              96                 0x01A4
   DMA2D_IRQHandler                 ,     // 90              97                 0x01A8
   ITBL_vSpuriousIsr                ,     // 91              98                 0x01AC
   OCTOSPI1_IRQHandler              ,     // 92              99                 0x01B0
   LPTIM1_IRQHandler                ,     // 93             100                 0x01B4
   CEC_IRQHandler                   ,     // 94             101                 0x01B8
   I2C4_EV_IRQHandler               ,     // 95             102                 0x01BC
   I2C4_ER_IRQHandler               ,     // 96             103                 0x01C0
   SPDIF_IRQHandler                 ,     // 97             104                 0x01C4
   ITBL_vSpuriousIsr                ,     // 98             105                 0x01C8
   ITBL_vSpuriousIsr                ,     // 99             106                 0x01CC
   ITBL_vSpuriousIsr                ,     //100             107                 0x01D0
   ITBL_vSpuriousIsr                ,     //101             108                 0x01D4
   DMAMUX1_OV_IRQHandler            ,     //102             109                 0x01D8
   ITBL_vSpuriousIsr                ,     //103             110                 0x01DC
   ITBL_vSpuriousIsr                ,     //104             111                 0x01E0
   ITBL_vSpuriousIsr                ,     //105             112                 0x01E4
   ITBL_vSpuriousIsr                ,     //106             113                 0x01E8
   ITBL_vSpuriousIsr                ,     //107             114                 0x01EC
   ITBL_vSpuriousIsr                ,     //108             115                 0x01F0
   ITBL_vSpuriousIsr                ,     //109             116                 0x01F4
   DFSDM1_FLT0_IRQHandler           ,     //110             117                 0x01F8
   DFSDM1_FLT1_IRQHandler           ,     //111             118                 0x01FC
   DFSDM1_FLT2_IRQHandler           ,     //112             119                 0x0200
   DFSDM1_FLT3_IRQHandler           ,     //113             120                 0x0204
   ITBL_vSpuriousIsr                ,     //114             121                 0x0208
   SWPMI1_IRQHandler                ,     //115             122                 0x020C
   TIM15_IRQHandler                 ,     //116             123                 0x0210
   TIM16_IRQHandler                 ,     //117             124                 0x0214
   TIM17_IRQHandler                 ,     //118             125                 0x0218
   MDIOS_WKUP_IRQHandler            ,     //119             126                 0x021C
   MDIOS_IRQHandler                 ,     //120             127                 0x0220
   ITBL_vSpuriousIsr                ,     //121             128                 0x0224
   MDMA_IRQHandler                  ,     //122             129                 0x0228
   ITBL_vSpuriousIsr                ,     //123             130                 0x022C
   SDMMC2_IRQHandler                ,     //124             131                 0x0230
   HSEM0_IRQHandler                 ,     //125             132                 0x0234
   ITBL_vSpuriousIsr                ,     //126             133                 0x0238
   ADC3_IRQHandler                  ,     //127             134                 0x023C
   DMAMUX2_OVR_IRQHandler           ,     //128             135                 0x0240
   ITBL_vSpuriousIsr                ,     //129             136                 0x0244
   ITBL_vSpuriousIsr                ,     //130             137                 0x0248
   ITBL_vSpuriousIsr                ,     //131             138                 0x024C
   ITBL_vSpuriousIsr                ,     //132             139                 0x0250
   ITBL_vSpuriousIsr                ,     //133             140                 0x0254
   ITBL_vSpuriousIsr                ,     //134             141                 0x0258
   ITBL_vSpuriousIsr                ,     //135             142                 0x025C
   ITBL_vSpuriousIsr                ,     //136             143                 0x0260
   COMP_IRQHandler                  ,     //137             144                 0x0264
   LPTIM2_IRQHandler                ,     //138             145                 0x0268
   LPTIM3_IRQHandler                ,     //139             146                 0x026C
   LPTIM4_IRQHandler                ,     //140             147                 0x0270
   LPTIM5_IRQHandler                ,     //141             148                 0x0274
   LPUART_IRQHandler                ,     //142             149                 0x0278
   ITBL_vSpuriousIsr                ,     //143             150                 0x027C
   CRS_IRQHandler                   ,     //144             151                 0x0280
   ITBL_vSpuriousIsr                ,     //145             152                 0x0284
   SAI4_IRQHandler                  ,     //146             153                 0x0288
   ITBL_vSpuriousIsr                ,     //147             154                 0x028C
   ITBL_vSpuriousIsr                ,     //148             155                 0x0290
   WKUP_IRQHandler                  ,     //149             156                 0x0294
   OCTOSPI2_IRQHandler              ,     //150             157                 0x0298
   ITBL_vSpuriousIsr                ,     //151             158                 0x029C
   ITBL_vSpuriousIsr                ,     //152             159                 0x02A0
   FMAC_IRQHandler                  ,     //153             160                 0x02A4
   CORDIC_IT_IRQHandler             ,     //154             161                 0x02A8
   ITBL_vSpuriousIsr                ,     //155             162                 0x02AC
   USART10_IRQHandler               ,     //156             163                 0x02B0
   I2C5_EV_IRQHandler               ,     //157             164                 0x02B4
   I2C5_ER_IRQHandler               ,     //158             165                 0x02B8
   FDCAN3_IT0_IRQHandler            ,     //159             166                 0x02BC
   FDCAN3_IT1_IRQHandler            ,     //160             167                 0x02C0
   TIM23_IRQHandler                 ,     //161             168                 0x02C4
   TIM24_IRQHandler                 ,     //162             169                 0x02C8
};
SDEF_SetSegmentConst_Default()

/* Function's task is to set the interrupt priorities according to the user's needs, different from the default settings.
 * If necessary, uncomment the relevant lines and set the required interrupt priorities
 */
void ITBL_setCustomInterruptPriority(void)
{
    /* Cortex-M7 settable interrupts */
    //NVIC_SetPriority(MemoryManagement_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(BusFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(UsageFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SVCall_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(DebugMonitor_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(PendSV_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SysTick_IRQn, PRIORITY_HIGH);
    /* Peripheral interrupts */
    //NVIC_SetPriority(WWDG1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(PVD_PVM_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(RTC_TAMP_STAMP_CSS_LSE_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(RTC_WKUP_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FLASH_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(RCC_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(EXTI0_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(EXTI1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(EXTI2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(EXTI3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(EXTI4_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA_STR0_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA_STR1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA_STR2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA_STR3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA_STR4_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA_STR5_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA_STR6_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(ADC1_2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FDCAN1_IT0_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FDCAN2_IT0_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FDCAN1_IT1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FDCAN2_IT1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(EXTI9_5_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM1_BRK_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM1_UP_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM1_TRG_COM_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM1_CC_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM4_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C1_EV_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C1_ER_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C2_EV_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C2_ER_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SPI1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SPI2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(USART1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(USART2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(USART3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(EXTI15_10_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(RTC_ALARM_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM8_UP_TIM13_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM8_CC_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA1_STR7_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FMC_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SDMMC1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM5_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SPI3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(UART4_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(UART5_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM6_DAC_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM7_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2_STR0_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2_STR1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2_STR2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2_STR3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2_STR4_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(ETH_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(ETH_WKUP_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FDCAN_CAL_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2_STR5_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2_STR6_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2_STR7_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(USART6_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C3_EV_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C3_ER_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DCMI_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FPU_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(UART7_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(UART8_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SPI4_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SPI5_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SPI6_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SAI1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(LTDC_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(LTDC_ER_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMA2D_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(OCTOSPI1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(LPTIM1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(CEC_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C4_EV_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C4_ER_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SPDIF_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMAMUX1_OV_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DFSDM1_FLT0_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DFSDM1_FLT1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DFSDM1_FLT2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DFSDM1_FLT3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SWPMI1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM15_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM16_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM17_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(MDIOS_WKUP_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(MDIOS_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(MDMA_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SDMMC2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(HSEM0_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(ADC3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(DMAMUX2_OVR_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(COMP_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(LPTIM2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(LPTIM3_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(LPTIM4_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(LPTIM5_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(LPUART_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(CRS_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(SAI4_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(WKUP_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(OCTOSPI2_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FMAC_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(CORDIC_IT_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(USART10_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C5_EV_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(I2C5_ER_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FDCAN3_IT0_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(FDCAN3_IT1_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM23_IRQn, PRIORITY_LOW);
    //NVIC_SetPriority(TIM24_IRQn, PRIORITY_LOW);
}
