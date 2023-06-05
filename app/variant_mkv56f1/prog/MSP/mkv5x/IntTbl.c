/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
*******************************************************************************
*   PROJECT          Generic SW
*   PROCESSOR        NXP MKV5X
******************************************************************************/

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


/* Interrupt Vector table for NXP MKV56F24. */
SDEF_SetSegmentConst(vectors)
const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
// ||Handler function name||       ||POSITION||        ||Priority||        ||Address offset||
   /*lint -e{923, 9029}  cast tested to work correctly for ARM CortexMx */

   DMA0_DMA16_IRQHandler            ,     //  0              16                 0x0040
   DMA1_DMA17_IRQHandler            ,     //  1              17                 0x0044
   DMA2_DMA18_IRQHandler            ,     //  2              18                 0x0048
   DMA3_DMA19_IRQHandler            ,     //  3              19                 0x004C
   DMA4_DMA20_IRQHandler            ,     //  4              20                 0x0050
   DMA5_DMA21_IRQHandler            ,     //  5              21                 0x0054
   DMA6_DMA22_IRQHandler            ,     //  6              22                 0x0058
   DMA7_DMA23_IRQHandler            ,     //  7              23                 0x005C
   DMA8_DMA24_IRQHandler            ,     //  8              24                 0x0060
   DMA9_DMA25_IRQHandler            ,     //  9              25                 0x0064
   DMA10_DMA26_IRQHandler           ,     // 10              26                 0x0068
   DMA11_DMA27_IRQHandler           ,     // 11              27                 0x006C
   DMA12_DMA28_IRQHandler           ,     // 12              28                 0x0070
   DMA13_DMA29_IRQHandler           ,     // 13              29                 0x0074
   DMA14_DMA30_IRQHandler           ,     // 14              30                 0x0078
   DMA15_DMA31_IRQHandler           ,     // 15              31                 0x007C
   DMA_Error_IRQHandler             ,     // 16              32                 0x0080
   MCM_IRQHandler                   ,     // 17              33                 0x0084
   FTFE_IRQHandler                  ,     // 18              34                 0x0088
   Read_Collision_IRQHandler        ,     // 19              35                 0x008C
   PMC_IRQHandler                   ,     // 20              36                 0x0090
   LLWU_IRQHandler                  ,     // 21              37                 0x0094
   WDOG_EWM_IRQHandler              ,     // 22              38                 0x0098
   TRNG0_IRQHandler                 ,     // 23              39                 0x009C
   I2C0_IRQHandler                  ,     // 24              40                 0x00A0
   I2C1_IRQHandler                  ,     // 25              41                 0x00A4
   HSPI0_ISR_HandleEvent            ,     // 26              42                 0x00A8
   HSPI1_ISR_HandleEvent            ,     // 27              43                 0x00AC
   UART5_RX_TX_IRQHandler           ,     // 28              44                 0x00B0
   UART5_ERR_IRQHandler             ,     // 29              45                 0x00B4
   ITBL_vSpuriousIsr                ,     // 30              46                 0x00B8
   UART0_RX_TX_IRQHandler           ,     // 31              47                 0x00BC
   UART0_ERR_IRQHandler             ,     // 32              48                 0x00C0
   UART1_RX_TX_IRQHandler           ,     // 33              49                 0x00C4
   UART1_ERR_IRQHandler             ,     // 34              50                 0x00C8
   UART2_RX_TX_IRQHandler           ,     // 35              51                 0x00CC
   UART2_ERR_IRQHandler             ,     // 36              52                 0x00D0
   ADC0_IRQHandler                  ,     // 37              53                 0x00D4
   HSADC_ERR_IRQHandler             ,     // 38              54                 0x00D8
   HSADC0_CCA_IRQHandler            ,     // 39              55                 0x00DC
   CMP0_IRQHandler                  ,     // 40              56                 0x00E0
   CMP1_IRQHandler                  ,     // 41              57                 0x00E4
   FTM0_IRQHandler                  ,     // 42              58                 0x00E8
   FTM1_IRQHandler                  ,     // 43              59                 0x00EC
   UART3_RX_TX_IRQHandler           ,     // 44              60                 0x00F0
   UART3_ERR_IRQHandler             ,     // 45              61                 0x00F4
   UART4_RX_TX_IRQHandler           ,     // 46              62                 0x00F8
   UART4_ERR_IRQHandler             ,     // 47              63                 0x00FC
   PIT0_IRQHandler                  ,     // 48              64                 0x0100
   PIT1_IRQHandler                  ,     // 49              65                 0x0104
   PIT2_IRQHandler                  ,     // 50              66                 0x0108
   PIT3_IRQHandler                  ,     // 51              67                 0x010C
   PDB0_IRQHandler                  ,     // 52              68                 0x0110
   FTM2_IRQHandler                  ,     // 53              69                 0x0114
   XBARA_IRQHandler                 ,     // 54              70                 0x0118
   PDB1_IRQHandler                  ,     // 55              71                 0x011C
   DAC0_IRQHandler                  ,     // 56              72                 0x0120
   MCG_IRQHandler                   ,     // 57              73                 0x0124
   LPTMR0_IRQHandler                ,     // 58              74                 0x0128
   PORTA_IRQHandler                 ,     // 59              75                 0x012C
   PORTB_IRQHandler                 ,     // 60              76                 0x0130
   PORTC_IRQHandler                 ,     // 61              77                 0x0134
   PORTD_IRQHandler                 ,     // 62              78                 0x0138
   PORTE_IRQHandler                 ,     // 63              79                 0x013C
   ITBL_vSpuriousIsr                ,     // 64              80                 0x0140
   HSPI2_ISR_HandleEvent            ,     // 65              81                 0x0144
   ENC_COMPARE_IRQHandler           ,     // 66              82                 0x0148
   ENC_HOME_IRQHandler              ,     // 67              83                 0x014C
   ENC_WDOG_SAB_IRQHandler          ,     // 68              84                 0x0150
   ENC_INDEX_IRQHandler             ,     // 69              85                 0x0154
   CMP2_IRQHandler                  ,     // 70              86                 0x0158
   FTM3_IRQHandler                  ,     // 71              87                 0x015C
   ITBL_vSpuriousIsr                ,     // 72              88                 0x0160
   HSADC0_CCB_IRQHandler            ,     // 73              89                 0x0164
   HSADC1_CCA_IRQHandler            ,     // 74              90                 0x0168
   CAN0_ORed_Message_buffer_IRQHandler,   // 75              91                 0x016C
   CAN0_Bus_Off_IRQHandler          ,     // 76              92                 0x0170
   CAN0_Error_IRQHandler            ,     // 77              93                 0x0174
   CAN0_Tx_Warning_IRQHandler       ,     // 78              94                 0x0178
   CAN0_Rx_Warning_IRQHandler       ,     // 79              95                 0x017C
   CAN0_Wake_Up_IRQHandler          ,     // 80              96                 0x0180
   PWM0_CMP0_IRQHandler             ,     // 81              97                 0x0184
   PWM0_RELOAD0_IRQHandler          ,     // 82              98                 0x0188
   PWM0_CMP1_IRQHandler             ,     // 83              99                 0x018C
   PWM0_RELOAD1_IRQHandler          ,     // 84             100                 0x0190
   PWM0_CMP2_IRQHandler             ,     // 85             101                 0x0194
   PWM0_RELOAD2_IRQHandler          ,     // 86             102                 0x0198
   PWM0_CMP3_IRQHandler             ,     // 87             103                 0x019C
   PWM0_RELOAD3_IRQHandler          ,     // 88             104                 0x01A0
   PWM0_CAP_IRQHandler              ,     // 89             105                 0x01A4
   PWM0_RERR_IRQHandler             ,     // 90             106                 0x01A8
   PWM0_FAULT_IRQHandler            ,     // 91             107                 0x01AC
   CMP3_IRQHandler                  ,     // 92             108                 0x01B0
   HSADC1_CCB_IRQHandler            ,     // 93             109                 0x01B4
   CAN1_ORed_Message_buffer_IRQHandler,   // 94             110                 0x01B8
   CAN1_Bus_Off_IRQHandler          ,     // 95             111                 0x01BC
   CAN1_Error_IRQHandler            ,     // 96             112                 0x01C0
   CAN1_Tx_Warning_IRQHandler       ,     // 97             113                 0x01C4
   CAN1_Rx_Warning_IRQHandler       ,     // 98             114                 0x01C8
   CAN1_Wake_Up_IRQHandler          ,     // 99             115                 0x01CC
   ITBL_vSpuriousIsr                ,     //100             116                 0x01D0
   ITBL_vSpuriousIsr                ,     //101             117                 0x01D4
   ITBL_vSpuriousIsr                ,     //103             118                 0x01DC
   ITBL_vSpuriousIsr                ,     //102             119                 0x01D8
   PWM1_CMP0_IRQHandler             ,     //104             120                 0x01E0
   PWM1_RELOAD0_IRQHandler          ,     //105             121                 0x01E4
   PWM1_CMP1_IRQHandler             ,     //106             122                 0x01E8
   PWM1_RELOAD1_IRQHandler          ,     //107             123                 0x01EC
   PWM1_CMP2_IRQHandler             ,     //108             124                 0x01F0
   PWM1_RELOAD2_IRQHandler          ,     //109             125                 0x01F4
   PWM1_CMP3_IRQHandler             ,     //110             126                 0x01F8
   PWM1_RELOAD3_IRQHandler          ,     //111             127                 0x01FC
   PWM1_CAP_IRQHandler              ,     //112             128                 0x0200
   PWM1_RERR_IRQHandler             ,     //113             129                 0x0204
   PWM1_FAULT_IRQHandler            ,     //114             130                 0x0208
   ITBL_vSpuriousIsr                ,     //115             131                 0x020C
   ITBL_vSpuriousIsr                ,     //116             132                 0x0210
   ITBL_vSpuriousIsr                ,     //117             133                 0x0214
   ITBL_vSpuriousIsr                ,     //118             134                 0x0218
   ITBL_vSpuriousIsr                ,     //119             135                 0x021C
   ITBL_vSpuriousIsr                ,     //120             136                 0x0220
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
    //NVIC_SetPriority(SVCall_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(DebugMonitor_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(PendSV_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SysTick_IRQn, PRIORITY_HIGH);
    /* Peripheral interrupts */
    //NVIC_SetPriority(DMA0_DMA16_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA1_DMA17_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA2_DMA18_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA3_DMA19_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA4_DMA20_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA5_DMA21_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA6_DMA22_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA7_DMA23_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA8_DMA24_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA9_DMA25_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA10_DMA26_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA11_DMA27_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA12_DMA28_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA13_DMA29_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA14_DMA30_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA15_DMA31_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DMA_Error_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(MCM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FTFE_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Read_Collision_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PMC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(LLWU_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(WDOG_EWM_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(TRNG0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(I2C1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SPI0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SPI1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART5_RX_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART5_ERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART0_RX_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART0_ERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART1_RX_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART1_ERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART2_RX_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART2_ERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ADC0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HSADC_ERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HSADC0_CCA_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CMP0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CMP1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FTM0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FTM1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART3_RX_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART3_ERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART4_RX_TX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(UART4_ERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PIT0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PIT1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PIT2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PIT3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PDB0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FTM2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(XBARA_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PDB1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(DAC0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(MCG_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(LPTMR0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PORTA_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PORTB_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PORTC_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PORTD_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PORTE_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(SPI2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ENC_COMPARE_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ENC_HOME_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ENC_WDOG_SAB_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(ENC_INDEX_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CMP2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(FTM3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HSADC0_CCB_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HSADC1_CCA_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN0_ORed_Message_buffer_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN0_Bus_Off_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN0_Error_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN0_Tx_Warning_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN0_Rx_Warning_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN0_Wake_Up_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_CMP0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_RELOAD0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_CMP1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_RELOAD1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_CMP2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_RELOAD2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_CMP3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_RELOAD3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_CAP_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_RERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM0_FAULT_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CMP3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(HSADC1_CCB_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN1_ORed_Message_buffer_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN1_Bus_Off_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN1_Error_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN1_Tx_Warning_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN1_Rx_Warning_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(CAN1_Wake_Up_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_CMP0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_RELOAD0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_CMP1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_RELOAD1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_CMP2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_RELOAD2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_CMP3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_RELOAD3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_CAP_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_RERR_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(PWM1_FAULT_IRQn, PRIORITY_MEDIUM);
}
