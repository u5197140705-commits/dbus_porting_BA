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
*   PROCESSOR        STM32G4
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

/*lint -esym(528,ITBL_peripheralVectors) "Warning 528: Symbol not explicitly referenced, but will be called by interrupts." */


/* Interrupt Vector table for STM32G4. */
SDEF_SetSegmentConst(vectors)
static const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
// ||Handler function name||            ||POSITION||        ||Priority||        ||Address offset||
   // Cortex external interrupts
    WWDG_IRQHandler                 ,//      0                   7                    0x0040
    PVD_PVM_IRQHandler              ,//      1                   8                    0x0044
    RTC_TAMP_LSECSS_IRQHandler      ,//      2                   9                    0x0048
    RTC_WKUP_IRQHandler             ,//      3                  10                    0x004C
    FLASH_IRQHandler                ,//      4                  11                    0x0050
    RCC_IRQHandler                  ,//      5                  12                    0x0054
    EXTI0_IRQHandler                ,//      6                  13                    0x0058
    EXTI1_IRQHandler                ,//      7                  14                    0x005C
    EXTI2_IRQHandler                ,//      8                  15                    0x0060
    EXTI3_IRQHandler                ,//      9                  16                    0x0064
    EXTI4_IRQHandler                ,//     10                  17                    0x0068
    DMA1_Channel1_IRQHandler        ,//     11                  18                    0x006C
    DMA1_Channel2_IRQHandler        ,//     12                  19                    0x0070
    DMA1_Channel3_IRQHandler        ,//     13                  20                    0x0074
    DMA1_Channel4_IRQHandler        ,//     14                  21                    0x0078
    DMA1_Channel5_IRQHandler        ,//     15                  22                    0x007C
    DMA1_Channel6_IRQHandler        ,//     16                  23                    0x0080
    DMA1_Channel7_IRQHandler        ,//     17                  24                    0x0084
    ADC1_2_IRQHandler               ,//     18                  25                    0x0088
    USB_HP_IRQHandler               ,//     19                  26                    0x008C
    USB_LP_IRQHandler               ,//     20                  27                    0x0090
    FDCAN1_IT0_IRQHandler           ,//     21                  28                    0x0094
    FDCAN1_IT1_IRQHandler           ,//     22                  29                    0x0098
    EXTI9_5_IRQHandler              ,//     23                  30                    0x009C
    TIM1_BRK_TIM15_IRQHandler       ,//     24                  31                    0x00A0
    TIM1_UP_TIM16_IRQHandler        ,//     25                  32                    0x00A4
    TIM1_TRG_COM_TIM17_IRQHandler   ,//     26                  33                    0x00A8
    TIM1_CC_IRQHandler              ,//     27                  34                    0x00AC
    TIM2_IRQHandler                 ,//     28                  35                    0x00B0
    TIM3_IRQHandler                 ,//     29                  36                    0x00B4
    TIM4_IRQHandler                 ,//     30                  37                    0x00B8
    I2C1_EV_IRQHandler              ,//     31                  38                    0x00BC
    I2C1_ER_IRQHandler              ,//     32                  39                    0x00C0
    I2C2_EV_IRQHandler              ,//     33                  40                    0x00C4
    I2C2_ER_IRQHandler              ,//     34                  41                    0x00C8
    SPI1_IRQHandler                 ,//     35                  42                    0x00CC
    SPI2_IRQHandler                 ,//     36                  43                    0x00D0
    USART1_IRQHandler               ,//     37                  44                    0x00D4
    USART2_IRQHandler               ,//     38                  45                    0x00D8
    USART3_IRQHandler               ,//     39                  46                    0x00DC
    EXTI15_10_IRQHandler            ,//     40                  47                    0x00E0
    RTC_Alarm_IRQHandler            ,//     41                  48                    0x00E4
    USBWakeUp_IRQHandler            ,//     42                  49                    0x00E8
    TIM8_BRK_IRQHandler             ,//     43                  50                    0x00EC
    TIM8_UP_IRQHandler              ,//     44                  51                    0x00F0
    TIM8_TRG_COM_IRQHandler         ,//     45                  52                    0x00F4
    TIM8_CC_IRQHandler              ,//     46                  53                    0x00F8
    ADC3_IRQHandler                 ,//     47                  54                    0x00FC
    FMC_IRQHandler                  ,//     48                  55                    0x0100                                    
    LPTIM1_IRQHandler               ,//     49                  56                    0x0104
    TIM5_IRQHandler                 ,//     50                  57                    0x0108                              
    SPI3_IRQHandler                 ,//     51                  58                    0x010C
    UART4_IRQHandler                ,//     52                  59                    0x0110
    UART5_IRQHandler                ,//     53                  60                    0x0114
    TIM6_DAC_IRQHandler             ,//     54                  61                    0x0118
    TIM7_DAC_IRQHandler             ,//     55                  62                    0x0118
    DMA2_Channel1_IRQHandler        ,//     56                  63                    0x0120
    DMA2_Channel2_IRQHandler        ,//     57                  64                    0x0124
    DMA2_Channel3_IRQHandler        ,//     58                  65                    0x0128
    DMA2_Channel4_IRQHandler        ,//     59                  66                    0x012C
    DMA2_Channel5_IRQHandler        ,//     60                  67                    0x0130
    ADC4_IRQHandler                 ,//     61                  68                    0x0134
    ADC5_IRQHandler                 ,//     62                  69                    0x0138                                                                         
    UCPD1_IRQHandler                ,//     63                  70                    0x013C
    COMP1_2_3_IRQHandler            ,//     64                  71                    0x0140
    COMP4_5_6_IRQHandler            ,//     65                  72                    0x0144
    COMP7_IRQHandler                ,//     66                  73                    0x0148   
    HRTIM1_Master_IRQHandler        ,//     67                  74                    0x014C
    HRTIM1_TIMA_IRQHandler          ,//     68                  75                    0x0150
    HRTIM1_TIMB_IRQHandler          ,//     69                  76                    0x0154
    HRTIM1_TIMC_IRQHandler          ,//     70                  77                    0x0158
    HRTIM1_TIMD_IRQHandler          ,//     71                  78                    0x015C
    HRTIM1_TIME_IRQHandler          ,//     72                  79                    0x0160
    HRTIM1_FLT_IRQHandler           ,//     73                  80                    0x0164
    HRTIM1_TIMF_IRQHandler          ,//     74                  81                    0x0168
    CRS_IRQHandler                  ,//     75                  82                    0x016C
    SAI1_IRQHandler                 ,//     76                  83                    0x0170
    TIM20_BRK_IRQHandler            ,//     77                  84                    0x0174
    TIM20_UP_IRQHandler             ,//     78                  85                    0x0178
    TIM20_TRG_COM_IRQHandler        ,//     79                  86                    0x017C
    TIM20_CC_IRQHandler             ,//     80                  87                    0x0180
    FPU_IRQHandler                  ,//     81                  88                    0x0184
    I2C4_EV_IRQHandler              ,//     82                  89                    0x0188
    I2C4_ER_IRQHandler              ,//     83                  90                    0x018C
    SPI4_IRQHandler                 ,//     84                  91                    0x0190
    AES_IRQHandler                  ,//     85                  92                    0x0194
    FDCAN2_IT0_IRQHandler           ,//     86                  93                    0x0198
    FDCAN2_IT1_IRQHandler           ,//     87                  94                    0x019C
    FDCAN3_IT0_IRQHandler           ,//     88                  95                    0x01A0
    FDCAN3_IT1_IRQHandler           ,//     89                  96                    0x01A4                                
    RNG_IRQHandler                  ,//     90                  97                    0x01A8
    LPUART1_IRQHandler              ,//     91                  98                    0x01AC
    I2C3_EV_IRQHandler              ,//     92                  99                    0x01B0
    I2C3_ER_IRQHandler              ,//     93                 100                    0x01B4
    DMAMUX_OVR_IRQHandler           ,//     94                 101                    0x01B8
    QUADSPI_IRQHandler              ,//     95                 102                    0x01BC
    DMA1_Channel8_IRQHandler        ,//     96                 103                    0x01C0
    DMA2_Channel6_IRQHandler        ,//     97                 104                    0x01C4
    DMA2_Channel7_IRQHandler        ,//     98                 105                    0x01C8
    DMA2_Channel8_IRQHandler        ,//     99                 106                    0x01CC
    CORDIC_IRQHandler               ,//    100                 107                    0x01D0
    FMAC_IRQHandler                 ,//    101                 108                    0x01D4
};
SDEF_SetSegmentConst_Default()

/* Function's task is to set the interrupt priorities according to the user's needs, different from the default settings.
 * If necessary, uncomment the relevant lines and set the required interrupt priorities
 */
void ITBL_setCustomInterruptPriority(void)
{
    /* Cortex-M4 settable interrupts */
    //NVIC_SetPriority(MemoryManagement_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(BusFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(UsageFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(PendSV_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SysTick_IRQn, PRIORITY_HIGH);
    /* Peripheral interrupts */
    //NVIC_SetPriority(WWDG_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PVD_PVM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(RTC_TAMP_CSS_LSE_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(RTC_WKUP_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FLASH_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(RCC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EXTI0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EXTI1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EXTI2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EXTI3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EXTI4_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_CH1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_CH2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_CH3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_CH4_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_CH5_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_CH6_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_CH7_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ADC1_2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USB_HP_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USB_LP_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(fdcan1_intr1_it_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(fdcan1_intr0_it_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EXTI9_5_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM1_UP_TIM16_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM1_TRG_COM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM1_CC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM4_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C1_EV_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C1_ER_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C2_EV_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C2_ER_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SPI1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SPI2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USART3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(EXTI15_10_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(RTC_ALARM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(USBWakeUP_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM8_BRK_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM8_UP_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM8_TRG_COM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM8_CC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ADC3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FMC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(LPTIM1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM5_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SPI3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART4_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART5_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM6_DACUNDER_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM7_DACUNDER_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_CH1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_CH2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_CH3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_CH4_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_CH5_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ADC4_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ADC5_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UCPD1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(COMP1_2_3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(COMP4_5_6_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(COMP7_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HRTIM_Master_IRQn_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HRTIM_TIMA_IRQn_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HRTIM_TIMB_IRQn_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HRTIM_TIMC_IRQn_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HRTIM_TIMD_IRQn_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HRTIM_TIME_IRQn_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HRTIM_TIM_FLT_IRQn_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HRTIM_TIMF_IRQn_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CRS_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SAI_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM20_BRK_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM20_UP_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM20_TRG_COM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TIM20_CC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FPU_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C4_EV_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C4_ER_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SPI4_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(AES_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FDCAN2_intr0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FDCAN2_intr1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FDCAN3_intr0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FDCAN3_intr1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(RNG_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(LPUART_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C3_EV_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C3_ER_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMAMUX_OVR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(QUADSPI_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_CH8_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_CH6_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_CH7_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_CH8_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Cordic_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FMAC_IRQn, PRIORITY_MEDIUM);
}
