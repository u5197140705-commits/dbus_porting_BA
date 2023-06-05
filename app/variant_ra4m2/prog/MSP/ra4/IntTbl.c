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
    PROCESSOR        RA4
*******************************************************************************/


/****************************************************************************************************************/
/* DOCUMENTATION                                                                                                */
/****************************************************************************************************************/

/** \file
 *
 *   \brief    This file defines the interrupt table.
 *
 *   \details  This file was created automatically, no manual changes should be
 *             done in this file.
 *
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
    |       0x0000000 - 0x000000F        | /   in file IntTblArmCM.c
    | ---------------------------------- |/
    |             Reserved               |
    |         for ROM Bootloader         |
    |        0x0000008 - 0x00007FF       |
    | ---------------------------------- |

    | ---------------------------------- |
    |        ITBL_coreVectors[16]        |
    |         moved Vector Table         |
    |  Offset + (0x0000010 - 0x000003F)  |
    | ---------------------------------- |

    | ---------------------------------- |
    |     ITBL_peripheralVectors[n]      |
    |         moved Vector Table         |
    |  Offset + (0x0000040 - 0x00000BC)  |
    | ---------------------------------- |
*/

/*lint -esym(528,ITBL_peripheralVectors) "Warning 528: Symbol ... not referenced" */

/* Interrupt Vector table for Renesas RA4
 * This array was manually predefined. RA4 has no static interrupt vector table.
 */
SDEF_SetSegmentConst(vectors)
static const ITBL_fptr SYMBOL_USED ITBL_peripheralVectors[]   =
{
// ||Handler function name||         ||POSITION||   ||Priority||   ||Address offset||
   /*lint -e{923, 9029}  cast tested to work correctly for ARM CortexMx */
    USART0_RX_IRQHandler            ,  //    0            7           0x0040
    USART0_TX_IRQHandler            ,  //    1            8           0x0044
    USART0_TX_IRQHandler            ,  //    2            9           0x0048
    USART0_RX_IRQHandler            ,  //    3           10           0x004C
    USART1_RX_IRQHandler            ,  //    4           11           0x0050
    USART1_TX_IRQHandler            ,  //    5           12           0x0054
    USART1_TX_IRQHandler            ,  //    6           13           0x0058
    USART1_RX_IRQHandler            ,  //    7           14           0x005C
    USART2_RX_IRQHandler            ,  //    8           15           0x0060
    USART2_TX_IRQHandler            ,  //    9           16           0x0064
    USART2_TX_IRQHandler            ,  //   10           17           0x0068
    USART2_RX_IRQHandler            ,  //   11           18           0x006C
    USART3_RX_IRQHandler            ,  //   12           19           0x0070
    USART3_TX_IRQHandler            ,  //   13           20           0x0074
    USART3_TX_IRQHandler            ,  //   14           21           0x0078
    USART3_RX_IRQHandler            ,  //   15           22           0x007C
    USART9_RX_IRQHandler            ,  //   16           23           0x0080
    USART9_TX_IRQHandler            ,  //   17           24           0x0084
    USART9_TX_IRQHandler            ,  //   18           25           0x0088
    USART9_RX_IRQHandler            ,  //   19           26           0x008C
    ADC0_IRQHandler                 ,  //   20           27           0x0090
    EXTI00_IRQHandler               ,  //   21           28           0x0094
    EXTI01_IRQHandler               ,  //   22           29           0x0098
    EXTI02_IRQHandler               ,  //   23           30           0x009C
    EXTI03_IRQHandler               ,  //   24           31           0x00A0
    EXTI04_IRQHandler               ,  //   25           32           0x00A4
    EXTI05_IRQHandler               ,  //   26           33           0x00A8
    EXTI06_IRQHandler               ,  //   27           34           0x00AC
    EXTI07_IRQHandler               ,  //   28           35           0x00B0
    EXTI08_IRQHandler               ,  //   29           36           0x00B4
    EXTI09_IRQHandler               ,  //   30           37           0x00B8
    EXTI10_IRQHandler               ,  //   31           38           0x00BC
    EXTI11_IRQHandler               ,  //   32           39           0x00C0
    EXTI12_IRQHandler               ,  //   33           40           0x00C4
    EXTI13_IRQHandler               ,  //   34           41           0x00C8
    EXTI14_IRQHandler               ,  //   35           42           0x00CC
    EXTI15_IRQHandler               ,  //   36           43           0x00D0
    GPT0_CCMPA_IRQHandler           ,  //   37           44           0x00D4
    GPT0_CCMPB_IRQHandler           ,  //   38           45           0x00D8
    GPT0_CMPC_IRQHandler            ,  //   39           46           0x00DC
    GPT0_CMPD_IRQHandler            ,  //   40           47           0x00E0
    GPT0_CMPE_IRQHandler            ,  //   41           48           0x00E4
    GPT0_CMPF_IRQHandler            ,  //   42           49           0x00E8
    GPT0_OVF_IRQHandler             ,  //   43           50           0x00EC
    GPT0_UDF_IRQHandler             ,  //   44           51           0x00F0
    GPT0_PC_IRQHandler              ,  //   45           52           0x00F4
    GPT1_CCMPA_IRQHandler           ,  //   46           53           0x00F8
    GPT1_CCMPB_IRQHandler           ,  //   47           54           0x00FC
    GPT1_CMPC_IRQHandler            ,  //   48           55           0x0100
    GPT1_CMPD_IRQHandler            ,  //   49           56           0x0104
    GPT1_CMPE_IRQHandler            ,  //   50           57           0x0108
    GPT1_CMPF_IRQHandler            ,  //   51           58           0x010C
    GPT1_OVF_IRQHandler             ,  //   52           59           0x0110
    GPT1_UDF_IRQHandler             ,  //   53           60           0x0114
    GPT1_PC_IRQHandler              ,  //   54           61           0x0118
    GPT2_CCMPA_IRQHandler           ,  //   55           62           0x011C
    GPT2_CCMPB_IRQHandler           ,  //   56           63           0x0120
    GPT2_CMPC_IRQHandler            ,  //   57           64           0x0124
    GPT2_CMPD_IRQHandler            ,  //   58           65           0x0128
    GPT2_CMPE_IRQHandler            ,  //   59           66           0x012C
    GPT2_CMPF_IRQHandler            ,  //   60           67           0x0130
    GPT2_OVF_IRQHandler             ,  //   61           68           0x0134
    GPT2_UDF_IRQHandler             ,  //   62           69           0x0138
    GPT3_CCMPA_IRQHandler           ,  //   63           70           0x013C
    GPT3_CCMPB_IRQHandler           ,  //   64           71           0x0140
    GPT3_CMPC_IRQHandler            ,  //   65           72           0x0144
    GPT3_CMPD_IRQHandler            ,  //   66           73           0x0148
    GPT3_CMPE_IRQHandler            ,  //   67           74           0x014C
    GPT3_CMPF_IRQHandler            ,  //   68           75           0x0150
    GPT3_OVF_IRQHandler             ,  //   69           76           0x0154
    GPT3_UDF_IRQHandler             ,  //   70           77           0x0158
    ITBL_vSpuriousIsr               ,  //   71           78           0x015C
    ITBL_vSpuriousIsr               ,  //   72           79           0x0160
    ITBL_vSpuriousIsr               ,  //   73           80           0x0164
    ITBL_vSpuriousIsr               ,  //   74           81           0x0168
    ITBL_vSpuriousIsr               ,  //   75           82           0x016C
    ITBL_vSpuriousIsr               ,  //   76           83           0x0170
    ITBL_vSpuriousIsr               ,  //   77           84           0x0174
    ITBL_vSpuriousIsr               ,  //   78           85           0x0178
    ITBL_vSpuriousIsr               ,  //   79           86           0x017C
    ITBL_vSpuriousIsr               ,  //   80           87           0x0180
    ITBL_vSpuriousIsr               ,  //   81           88           0x0184
    ITBL_vSpuriousIsr               ,  //   82           89           0x0188
    ITBL_vSpuriousIsr               ,  //   83           90           0x018C
    ITBL_vSpuriousIsr               ,  //   84           91           0x0190
    ITBL_vSpuriousIsr               ,  //   85           92           0x0194
    ITBL_vSpuriousIsr               ,  //   86           93           0x0198
    ITBL_vSpuriousIsr               ,  //   87           94           0x019C
    ITBL_vSpuriousIsr               ,  //   88           95           0x01A0
    ITBL_vSpuriousIsr               ,  //   89           96           0x01A4
    ITBL_vSpuriousIsr               ,  //   90           97           0x01A8
    ITBL_vSpuriousIsr               ,  //   91           98           0x01AC
    ITBL_vSpuriousIsr               ,  //   92           99           0x01B0
    ITBL_vSpuriousIsr               ,  //   93          100           0x01B4
    ITBL_vSpuriousIsr               ,  //   94          101           0x01B8
    ITBL_vSpuriousIsr               ,  //   95          102           0x01BC
};
SDEF_SetSegmentConst_Default()

/* Function's task is to set the interrupt priorities according to the user's needs, different from the default settings.
 * If necessary, uncomment the relevant lines and set the required interrupt priorities
 */
void ITBL_setCustomInterruptPriority(void)
{
    /* Cortex-M23 settable interrupts */
    //NVIC_SetPriority(MemoryManagement_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(BusFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(UsageFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SecureFault_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SVCall_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(DebugMonitor_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(PendSV_IRQn, PRIORITY_HIGH);
    //NVIC_SetPriority(SysTick_IRQn, PRIORITY_HIGH);
    /* Peripheral interrupts */
    //NVIC_SetPriority(Position_0_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_1_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_2_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_3_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_4_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_5_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_6_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_7_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_8_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_9_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_10_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_11_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_12_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_13_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_14_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_15_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_16_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_17_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_18_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_19_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_20_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_21_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_22_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_23_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_24_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_25_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_26_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_27_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_28_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_29_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_30_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_31_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_32_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_33_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_34_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_35_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_36_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_37_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_38_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_39_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_40_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_41_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_42_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_43_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_44_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_45_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_46_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_47_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_48_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_49_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_50_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_51_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_52_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_53_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_54_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_55_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_56_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_57_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_58_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_59_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_60_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_61_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_62_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_63_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_64_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_65_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_66_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_67_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_68_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_69_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_70_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_71_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_72_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_73_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_74_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_75_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_76_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_77_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_78_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_79_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_80_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_81_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_82_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_83_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_84_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_85_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_86_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_87_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_88_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_89_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_90_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_91_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_92_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_93_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_94_IRQn, PRIORITY_MEDIUM);
    //NVIC_SetPriority(Position_95_IRQn, PRIORITY_LOW);
}
