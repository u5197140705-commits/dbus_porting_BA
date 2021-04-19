/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Framework
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *   \brief    Implementation of micro-controller/platform initialisation
 *             and configuration for STM32L4.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "basic.h"
#include "processor.h"
#include "hwdt.h"
#ifdef APP_VARIANT
#include "hadc.h"
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
#include "fSafe.h"
#endif
/* Several prescalers can be used to configure the AHB frequency, the high speed APB
(APB2) and the low speed APB (APB1) domains. The maximum frequency of the AHB, the
APB1 and the APB2 domains is 80 MHz.*/

/*****************************************************************************
*-----------------------------------------------------------------------------
*        System Clock source                    | PLL (HSE-HSI16-MSI)
*-----------------------------------------------------------------------------
*        SYSCLK(MHz)                            | 80 MHz max
*-----------------------------------------------------------------------------
*        HCLK(MHz)                              | 80 MHz max
*-----------------------------------------------------------------------------
*        AHB Prescaler                          | range 2^0 - 2^9
*-----------------------------------------------------------------------------
*        APB1 Prescaler                         | range 2^0 - 2^4
*-----------------------------------------------------------------------------
*        APB2 Prescaler                         | range 2^0 - 2^4
*-----------------------------------------------------------------------------
*        PLL_M                                  | auto
*-----------------------------------------------------------------------------
*        PLL_N                                  | auto
*-----------------------------------------------------------------------------
*        PLL_P                                  | NA
*-----------------------------------------------------------------------------
*        PLL_Q                                  | auto
*-----------------------------------------------------------------------------
*        PLL_R                                  | auto
*-----------------------------------------------------------------------------
*        Flash Latency(WS)                      | auto
*-----------------------------------------------------------------------------
*        Prefetch Buffer                        | ON
*****************************************************************************/
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
/** \brief shifts a bitfield (value) to the desired position (shift) */
#define SHFT_BIT(value,shift) ((uint32_t)(((uint32_t)(value) << ((uint32_t)(shift))))) //lint !e9026 macro needed to be resolved at compile time
//lint -esym( 750, BSC_HCLK_DIV*) "message: local macro not referenced..."
#define  BSC_HCLK_DIV1  0x0UL    ///< HCLK not divided
#define  BSC_HCLK_DIV2  0x4UL    ///< HCLK divided by 2
#define  BSC_HCLK_DIV4  0x5UL    ///< HCLK divided by 4
#define  BSC_HCLK_DIV8  0x6UL    ///< HCLK divided by 8
#define  BSC_HCLK_DIV16 0x7UL    ///< HCLK divided by 16

//lint -esym( 750, BSC_CLK_DIV*) "message: local macro not referenced..."
#define  BSC_CLK_DIV1  1U      ///< clock divided by 1 (not divided)
#define  BSC_CLK_DIV2  2U      ///< clock divided by 2
#define  BSC_CLK_DIV3  3U      ///< clock divided by 3
#define  BSC_CLK_DIV4  4U      ///< clock divided by 4
#define  BSC_CLK_DIV5  5U      ///< clock divided by 5
#define  BSC_CLK_DIV6  6U      ///< clock divided by 6
#define  BSC_CLK_DIV7  7U      ///< clock divided by 7
#define  BSC_CLK_DIV8  8U      ///< clock divided by 8

//lint -esym( 750, BSC_SYSCLK_DIV*) "message: local macro not referenced..."
#define  BSC_SYSCLK_DIV1    0x0UL   ///< SYSCLK not divided
#define  BSC_SYSCLK_DIV2    0x8UL   ///< SYSCLK divided by 2
#define  BSC_SYSCLK_DIV4    0x9UL   ///< SYSCLK divided by 4
#define  BSC_SYSCLK_DIV8    0xAUL   ///< SYSCLK divided by 8
#define  BSC_SYSCLK_DIV16   0xBUL   ///< SYSCLK divided by 16
#define  BSC_SYSCLK_DIV64   0xCUL   ///< SYSCLK divided by 64
#define  BSC_SYSCLK_DIV128  0xDUL   ///< SYSCLK divided by 128
#define  BSC_SYSCLK_DIV256  0xEUL   ///< SYSCLK divided by 256
#define  BSC_SYSCLK_DIV512  0xFUL   ///< SYSCLK divided by 512

//lint -esym( 750, BSC_RCC_CFGR_*) "message: local macro not referenced..."
/** These defines determines divider of the SYSCLK and output clock is called
 *  the HCLK frequency that supplies APB1/2 (PCLK1/2). 
 *  The default macro is BSC_RCC_CFGR_HPRE_SYSCLK_DIV1 and now there is no
 *  need to control this prescaler from the build process.
 */
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV1      (BSC_SYSCLK_DIV1) // shift not done left argument is zero   << RCC_CFGR_HPRE_POS)
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV2      (BSC_SYSCLK_DIV2   << RCC_CFGR_HPRE_POS)
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV4      (BSC_SYSCLK_DIV4   << RCC_CFGR_HPRE_POS)
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV8      (BSC_SYSCLK_DIV8   << RCC_CFGR_HPRE_POS)
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV16     (BSC_SYSCLK_DIV16  << RCC_CFGR_HPRE_POS)
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV64     (BSC_SYSCLK_DIV64  << RCC_CFGR_HPRE_POS)
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV128    (BSC_SYSCLK_DIV128 << RCC_CFGR_HPRE_POS)
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV256    (BSC_SYSCLK_DIV256 << RCC_CFGR_HPRE_POS)
#define BSC_RCC_CFGR_HPRE_SYSCLK_DIV512    (BSC_SYSCLK_DIV512 << RCC_CFGR_HPRE_POS)

/** These defines determines divider of the HCLK for the low speed bus APB1 (frequency PCLK1).
 *  The Maximum frequency of the bus is 80 MHZ.
 *  The coresponding macro is chosen according to user defined define PCLOCK in the build process.
 */
#define BSC_RCC_CFGR_PPRE1_DIV1     (BSC_HCLK_DIV1) // shift not done left argument is zero
#define BSC_RCC_CFGR_PPRE1_DIV2     (BSC_HCLK_DIV2  << RCC_CFGR_PPRE1_POS)
#define BSC_RCC_CFGR_PPRE1_DIV4     (BSC_HCLK_DIV4  << RCC_CFGR_PPRE1_POS)
#define BSC_RCC_CFGR_PPRE1_DIV8     (BSC_HCLK_DIV8  << RCC_CFGR_PPRE1_POS)
#define BSC_RCC_CFGR_PPRE1_DIV16    (BSC_HCLK_DIV16 << RCC_CFGR_PPRE1_POS)

/** These defines determines divider of the HCLK for the high speed bus APB2 (frequency PCLK2).
 *  The Maximum frequency of the bus is 80 MHZ.
 *  The coresponding macro is chosen according to user defined define PCLOCK2 in the build process.
 */
#define BSC_RCC_CFGR_PPRE2_DIV1     (BSC_HCLK_DIV1) // shift not done left argument is zero
#define BSC_RCC_CFGR_PPRE2_DIV2     (BSC_HCLK_DIV2  << RCC_CFGR_PPRE2_POS)
#define BSC_RCC_CFGR_PPRE2_DIV4     (BSC_HCLK_DIV4  << RCC_CFGR_PPRE2_POS)
#define BSC_RCC_CFGR_PPRE2_DIV8     (BSC_HCLK_DIV8  << RCC_CFGR_PPRE2_POS)
#define BSC_RCC_CFGR_PPRE2_DIV16    (BSC_HCLK_DIV16 << RCC_CFGR_PPRE2_POS)

#define BSC_RCC_CFGR_SW_PLL            0x00000003UL        ///< PLL selected as system clock
#define BSC_RCC_CFGR_SW_HSI            0x00000001UL        ///< HSI16 selected as system clock

#define BSC_RCC_CFGR_SWS_HSI           0x00000004UL        ///< HSI16 used as system clock
#define BSC_RCC_CFGR_SWS_PLL           0x0000000CUL        ///< PLL used as system clock

//lint -esym( 750, BSC_RCC_PLLCFGR_*) "message: local macro not referenced..."
#define BSC_RCC_PLLCFGR_PLLREN_ENABLE   1U  ///< PLLCLK output enable
#define BSC_RCC_PLLCFGR_PLLPEN_ENABLE   1U  ///< PLLSAI2CLK output enable
#define BSC_RCC_PLLCFGR_PLLQEN_ENABLE   1U  ///< PLL48M1CLK output enable

#define BSC_RCC_PLLCFGR_PLLPDIV        0U  ///< PLLSAI2CLK is controlled by the bit PLLP

//lint -esym( 750, RCC_CR_*) "message: local macro not referenced..."
#ifndef RCC_CR_PLLSAI2RDY_MASK
    #define  RCC_CR_PLLSAI2RDY_MASK         0x20000000UL
#endif

//lint -esym( 750, BSC_FLASH_ACR_LATENCY_*) "message: local macro not referenced..."
#define BSC_FLASH_ACR_LATENCY_0WS              0x0U   ///< Zero wait state
#define BSC_FLASH_ACR_LATENCY_1WS              0x1U   ///< One wait state
#define BSC_FLASH_ACR_LATENCY_2WS              0x2U   ///< Two wait states
#define BSC_FLASH_ACR_LATENCY_3WS              0x3U   ///< Three wait states
#define BSC_FLASH_ACR_LATENCY_4WS              0x4U   ///< Four wait states

#define MIN_PLL_SRC_CLK      4U
#define MAX_PLL_SRC_CLK     48U
#define MIN_PLLM_OUT_CLK     4U
#define MAX_PLLM_OUT_CLK    16U
#define MIN_PLL_VCO_CLK     64U
#define MAX_PLL_VCO_CLK    344U
#define MIN_SYSCLK          32U
#define MAX_SYSCLK          80U
#define MAX_PCLOCK          80U
#define MAX_PCLOCK1         80U
#define MIN_PLLQ_CLK         8U
#define MAX_PLLQ_CLK        80U

#ifndef  HSI16
//lint -esym( 750, HSI16) "message: local macro not referenced..."
    #define HSI16   16U      ///< define internal oscilator HSI to 16MHz if no oscilator is defined
#endif

/* PLL_SRC_CLK - PLL Source clock definition */
#if defined(EXT_CRYSTAL_OSC)
    #define PLL_SRC_CLK     EXT_CRYSTAL_OSC
    #define BSC_RCC_PLLCFGR_PLLSRC  3U      ///< HSE clock selected as PLL and PLLSAI1 clock entry
#elif defined(MSI)
    #define PLL_SRC_CLK     MSI
    #define BSC_RCC_PLLCFGR_PLLSRC  1U      ///< MSI clock selected as PLL and PLLSAI1 clock entry
#elif defined(HSI16)
    #define PLL_SRC_CLK     HSI16
    #define BSC_RCC_PLLCFGR_PLLSRC  2U      ///< HSI16 clock selected as PLL and PLLSAI1 clock entry
//        #define  HSI16      16U     // 16MHz High Speed Internal Oscilator
#else
    #pragma error "No EXT_CRYSTAL_OSC defined, nor internal MSI and nor HSI16 defined."
#endif

/* AHB Prescaler calculation. It will be used divide SYSCLK */
#if (CORE_CLOCK < MIN_SYSCLK)
    #if (MIN_SYSCLK <= (CORE_CLOCK * 2U)) && ((CORE_CLOCK * 2U) <= MAX_SYSCLK)
        #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV2   ///< SYSCLK divided by 2
        #define BSC_SYSCLK            (CORE_CLOCK * 2U)
    #elif (MIN_SYSCLK <= (CORE_CLOCK * 4U)) && ((CORE_CLOCK * 4U) <= MAX_SYSCLK)
        #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV4   ///< SYSCLK divided by 4
        #define BSC_SYSCLK            (CORE_CLOCK * 4U)
    #elif (MIN_SYSCLK <= (CORE_CLOCK * 8U)) && ((CORE_CLOCK * 8U) <= MAX_SYSCLK)
        #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV8   ///< SYSCLK divided by 8
        #define BSC_SYSCLK            (CORE_CLOCK * 8U)
    #elif (MIN_SYSCLK <= (CORE_CLOCK * 16U)) && ((CORE_CLOCK * 16U) <= MAX_SYSCLK)
        #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV16   ///< SYSCLK divided by 16
        #define BSC_SYSCLK            (CORE_CLOCK * 16U)
    #elif (MIN_SYSCLK <= (CORE_CLOCK * 64U)) && ((CORE_CLOCK * 64U) <= MAX_SYSCLK)
        #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV64   ///< SYSCLK divided by 64
        #define BSC_SYSCLK            (CORE_CLOCK * 64U)
    #elif (MIN_SYSCLK <= (CORE_CLOCK * 128U)) && ((CORE_CLOCK * 128U) <= MAX_SYSCLK)
        #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV128  ///< SYSCLK divided by 128
        #define BSC_SYSCLK            (CORE_CLOCK * 128U)
    #elif (MIN_SYSCLK <= (CORE_CLOCK * 256U)) && ((CORE_CLOCK * 256U) <= MAX_SYSCLK)
        #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV256  ///< SYSCLK divided by 256
        #define BSC_SYSCLK            (CORE_CLOCK * 256U)
    #elif (MIN_SYSCLK <= (CORE_CLOCK * 512U)) && ((CORE_CLOCK * 512U) <= MAX_SYSCLK)
        #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV512  ///< SYSCLK divided by 512
        #define BSC_SYSCLK            (CORE_CLOCK * 512U)
    #else
        #pragma error "No valid system clock defined. CORE_CLOCK must be correctly defined."
    #endif
#elif (MIN_SYSCLK <= CORE_CLOCK) && (CORE_CLOCK <= MAX_SYSCLK)
    #define BSC_RCC_CFGR_HPRE_SYSCLK_DIV    BSC_RCC_CFGR_HPRE_SYSCLK_DIV1   ///< SYSCLK not divided
    #define BSC_SYSCLK       (CORE_CLOCK * 1U)
#else
    #pragma error "No valid core clock defined. CORE_CLOCK must be correctly defined."
#endif

#if ( MIN_PLL_SRC_CLK <= PLL_SRC_CLK ) && ( PLL_SRC_CLK <= MAX_PLL_SRC_CLK )
/* PLLR calculation,
   PLLCLK (System Clock) = VCO frequency / PLLPDIV*/
    #if (MIN_PLL_VCO_CLK <= (BSC_SYSCLK * BSC_CLK_DIV2)) && ((BSC_SYSCLK * BSC_CLK_DIV2) <= MAX_PLL_VCO_CLK)
        #define PLL_R       0U      ///< PLLCLK divided by 2
        #define PLLR_DIV    BSC_CLK_DIV2
    #elif (MIN_PLL_VCO_CLK <= (BSC_SYSCLK * BSC_CLK_DIV4)) && ((BSC_SYSCLK * BSC_CLK_DIV4) <= MAX_PLL_VCO_CLK)
        #define PLL_R       1U      ///< PLLCLK divided by 4
        #define PLLR_DIV    BSC_CLK_DIV4
    #elif (MIN_PLL_VCO_CLK <= (BSC_SYSCLK * BSC_CLK_DIV6)) && ((BSC_SYSCLK * BSC_CLK_DIV6) <= MAX_PLL_VCO_CLK)
        #define PLL_R       2U      ///< PLLCLK divided by 6
        #define PLLR_DIV    BSC_CLK_DIV6
    #elif (MIN_PLL_VCO_CLK <= (BSC_SYSCLK * BSC_CLK_DIV8)) && ((BSC_SYSCLK * BSC_CLK_DIV8) <= MAX_PLL_VCO_CLK)
        #define PLL_R       3U      ///< PLLCLK divided by 8
        #define PLLR_DIV    BSC_CLK_DIV8
    #else
        #pragma error "No valid core clock defined. BSC_SYSCLK must be correctly defined."
    
    #endif
/* VCO input frequency = PLL_SRC_CLK / PLLM with 1 <= PLLM <= 8 
   4MHz <= VCO input frequency <=16MHz*/
    #if (((PLL_SRC_CLK % BSC_CLK_DIV1) == 0U) && (MIN_PLLM_OUT_CLK <= (PLL_SRC_CLK / BSC_CLK_DIV1)) && (MAX_PLLM_OUT_CLK >= (PLL_SRC_CLK / BSC_CLK_DIV1)))
        #define PLL_M   (BSC_CLK_DIV1 - 1U)         ///< PLL source will be div by 1
    #elif (((PLL_SRC_CLK % BSC_CLK_DIV2) == 0U) && (MIN_PLLM_OUT_CLK <= (PLL_SRC_CLK / BSC_CLK_DIV2)) && (MAX_PLLM_OUT_CLK >= (PLL_SRC_CLK / BSC_CLK_DIV2)))
        #define PLL_M   (BSC_CLK_DIV2 - 1U)         ///< PLL source will be div by 2
    #elif (((PLL_SRC_CLK % BSC_CLK_DIV3) == 0U) && (MIN_PLLM_OUT_CLK <= (PLL_SRC_CLK / BSC_CLK_DIV3)) && (MAX_PLLM_OUT_CLK >= (PLL_SRC_CLK / BSC_CLK_DIV3)))
        #define PLL_M   (BSC_CLK_DIV3 - 1U)         ///< PLL source will be div by 3
    #elif (((PLL_SRC_CLK % BSC_CLK_DIV4) == 0U) && (MIN_PLLM_OUT_CLK <= (PLL_SRC_CLK / BSC_CLK_DIV4)) && (MAX_PLLM_OUT_CLK >= (PLL_SRC_CLK / BSC_CLK_DIV4)))
        #define PLL_M   (BSC_CLK_DIV4 - 1U)         ///< PLL source will be div by 4
    #elif (((PLL_SRC_CLK % BSC_CLK_DIV5) == 0U) && (MIN_PLLM_OUT_CLK <= (PLL_SRC_CLK / BSC_CLK_DIV5)) && (MAX_PLLM_OUT_CLK >= (PLL_SRC_CLK / BSC_CLK_DIV5)))
        #define PLL_M   (BSC_CLK_DIV5 - 1U)         ///< PLL source will be div by 5
    #elif (((PLL_SRC_CLK % BSC_CLK_DIV6) == 0U) && (MIN_PLLM_OUT_CLK <= (PLL_SRC_CLK / BSC_CLK_DIV6)) && (MAX_PLLM_OUT_CLK >= (PLL_SRC_CLK / BSC_CLK_DIV6)))
        #define PLL_M   (BSC_CLK_DIV6 - 1U)         ///< PLL source will be div by 6
    #elif (((PLL_SRC_CLK % BSC_CLK_DIV7) == 0U) && (MIN_PLLM_OUT_CLK <= (PLL_SRC_CLK / BSC_CLK_DIV7)) && (MAX_PLLM_OUT_CLK >= (PLL_SRC_CLK / BSC_CLK_DIV7)))
        #define PLL_M   (BSC_CLK_DIV7 - 1U)         ///< PLL source will be div by 7
    #elif (((PLL_SRC_CLK % BSC_CLK_DIV8) == 0U) && (MIN_PLLM_OUT_CLK <= (PLL_SRC_CLK / BSC_CLK_DIV8)) && (MAX_PLLM_OUT_CLK >= (PLL_SRC_CLK / BSC_CLK_DIV8)))
        #define PLL_M   (BSC_CLK_DIV8 - 1U)         ///< PLL source will be div by 8
    #else
        #pragma error "No valid PLL_SRC_CLK. It must be correctly defined."
    #endif
/* PLLM calculation */
    #define PLL_N  ((BSC_SYSCLK * PLLR_DIV) / (PLL_SRC_CLK / (PLL_M + 1U)))
    #if (PLL_N <= 7U) || (PLL_N >= 86U)
        #pragma error "No valid peripheral clock defined. EXT_CRYSTAL_OSC must be correctly defined."
    #endif
/* PLLQ calculation */
    #if (MIN_PLLQ_CLK <= ((BSC_SYSCLK * PLLR_DIV) / BSC_CLK_DIV2)) && (((BSC_SYSCLK * PLLR_DIV) / BSC_CLK_DIV2) <= MAX_PLLQ_CLK)
        #define PLL_Q   0U
    #elif (MIN_PLLQ_CLK <= ((BSC_SYSCLK * PLLR_DIV) / BSC_CLK_DIV4)) && (((BSC_SYSCLK * PLLR_DIV) / BSC_CLK_DIV4) <= MAX_PLLQ_CLK)
        #define PLL_Q   1U
    #elif (MIN_PLLQ_CLK <= ((BSC_SYSCLK * PLLR_DIV) / BSC_CLK_DIV6)) && (((BSC_SYSCLK * PLLR_DIV) / BSC_CLK_DIV6) <= MAX_PLLQ_CLK)
        #define PLL_Q   2U
    #elif (MIN_PLLQ_CLK <= ((BSC_SYSCLK * PLLR_DIV) / BSC_CLK_DIV8)) && (((BSC_SYSCLK * PLLR_DIV) / BSC_CLK_DIV8) <= MAX_PLLQ_CLK)
        #define PLL_Q   3U
    #else
        #pragma error "No valid core clock defined. BSC_SYSCLK must be correctly defined."
    #endif
#else
    #pragma error "No valid peripheral clock defined. EXT_CRYSTAL_OSC must be correctly defined."
#endif

/* FLASH_ACR_LATENCY calculation */
#if(CORE_CLOCK <= 16U)
    #define FLASH_ACR_LATENCY_VALUE   BSC_FLASH_ACR_LATENCY_0WS    // 0 wait states (1 CPU cycles)
#elif(CORE_CLOCK <= 32U)
    #define FLASH_ACR_LATENCY_VALUE   BSC_FLASH_ACR_LATENCY_1WS    // 1 wait states (2 CPU cycles)
#elif(CORE_CLOCK <= 48U)
    #define FLASH_ACR_LATENCY_VALUE   BSC_FLASH_ACR_LATENCY_2WS    // 2 wait states (3 CPU cycles)
#elif(CORE_CLOCK <= 64U)
    #define FLASH_ACR_LATENCY_VALUE   BSC_FLASH_ACR_LATENCY_3WS    // 3 wait states (4 CPU cycles)
#elif(CORE_CLOCK <= 80U)
    #define FLASH_ACR_LATENCY_VALUE   BSC_FLASH_ACR_LATENCY_4WS    // 4 wait states (5 CPU cycles)
#endif

/* PCLOCK calculation */
#ifndef PCLOCK
    #define PCLOCK    CORE_CLOCK
#endif
/* PPRE1 APB low-speed prescaler (APB1) calculation */
#define BSC_PPRE_DIV1 (CORE_CLOCK / PCLOCK)
#if ( (CORE_CLOCK % PCLOCK) == 0U ) && ( PCLOCK <= MAX_PCLOCK )
    #if BSC_PPRE_DIV1 == 1U
        #define BSC_RCC_CFGR_PPRE1       BSC_RCC_CFGR_PPRE1_DIV1
    #elif BSC_PPRE_DIV1 == 2U
        #define BSC_RCC_CFGR_PPRE1       BSC_RCC_CFGR_PPRE1_DIV2
    #elif BSC_PPRE_DIV1 == 4U
        #define BSC_RCC_CFGR_PPRE1       BSC_RCC_CFGR_PPRE1_DIV4
    #elif BSC_PPRE_DIV1 == 8U
        #define BSC_RCC_CFGR_PPRE1       BSC_RCC_CFGR_PPRE1_DIV8
    #else
        #pragma error "No valid value of peripheral clock defined. PCLOCK must be correctly defined."
    #endif
#else
    #pragma error "No valid value of peripheral clock defined. PCLOCK must be correctly defined."
#endif
/* PPRE2 APB High-speed prescaler (APB2) calculation */
#define BSC_PPRE_DIV2 (CORE_CLOCK / PCLOCK1)
#if ( (CORE_CLOCK % PCLOCK1) == 0U ) && ( PCLOCK1 <= MAX_PCLOCK1 )
    #if BSC_PPRE_DIV2 == 1U
        #define BSC_RCC_CFGR_PPRE2       BSC_RCC_CFGR_PPRE2_DIV1
    #elif BSC_PPRE_DIV2 == 2U
        #define BSC_RCC_CFGR_PPRE2       BSC_RCC_CFGR_PPRE2_DIV2
    #elif BSC_PPRE_DIV2 == 4U
        #define BSC_RCC_CFGR_PPRE2       BSC_RCC_CFGR_PPRE2_DIV4
    #elif BSC_PPRE_DIV2 == 8U
        #define BSC_RCC_CFGR_PPRE2       BSC_RCC_CFGR_PPRE2_DIV8
    #else
        #pragma error "No valid value of peripheral clock defined. PCLOCK1 must be correctly defined."
    #endif
#else
    #pragma error "No valid value of peripheral clock defined. PCLOCK1 must be correctly defined."
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
/** \brief Sets the PLL */
static void BASIC_vSetPLL(uint32_t divFactorPLLM, uint32_t mulFactorPLLN, uint32_t divFactorPLLR, uint32_t divFactorPLLQ);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void BASIC_vInitPlatform(void)
{
    FLASH_ACR_LATENCY = BSC_FLASH_ACR_LATENCY_4WS; // set max wait states for safety
   /* enable prefetch buffer, instruction and data cache */
    FLASH_ACR |= FLASH_ACR_PRFTEN_MASK | FLASH_ACR_ICEN_MASK | FLASH_ACR_DCEN_MASK;
    // The main regulator setting to Range 1 (High-performance range)
    // See capitol "Dynamic voltage scaling management" in reference manual
    RCC_APB1ENR1_PWREN = 1u;
    PWR_CR1_VOS = 1u;
    while ((PWR_SR2 & PWR_SR2_VOSF_MASK) != 0u)
    {
        
    }
    RCC_APB1ENR1_PWREN = 0u;

    RCC_CR |= RCC_CR_HSION_MASK; // HSI16 clock on
    /* Wait for HSIRDY = 1 (HSI16 is ready) */
    while ((RCC_CR & RCC_CR_HSIRDY_MASK) == 0u)
    {

    }

#ifdef EXT_CRYSTAL_OSC
    /* HSE clock on */
    RCC_CR |= RCC_CR_HSEON_MASK;
    /* Wait for HSERDY = 1 (HSE is ready) */
    while ((RCC_CR & RCC_CR_HSERDY_MASK) == 0U)
    {

    }
#endif

    BASIC_vSetPLL(PLL_M, PLL_N, PLL_R, PLL_Q);    // set PLL
    /* set Flash Latency value */
    FLASH_ACR_LATENCY = FLASH_ACR_LATENCY_VALUE;
    RCC_APB2ENR |= RCC_APB2ENR_SYSCFGEN_MASK; // enable clock for System configuration controller
#ifdef USE_HSI16_CLOCK_AS_PERIPH_SOURCE
    /* select HSI16 as peripheral clock as for as possible */
    RCC_CCIPR = (uint32_t)(
        SHFT_BIT(HSI16_SRC, RCC_CCIPR_USART1SEL_POS) |   /* select HSI16 as UART1 clock */ //lint !e835 better readability even when shift is zero
        SHFT_BIT(HSI16_SRC, RCC_CCIPR_USART2SEL_POS) |   /* select HSI16 as UART2 clock */
        SHFT_BIT(HSI16_SRC, RCC_CCIPR_LPUART1SEL_POS) |   /* select HSI16 as LPUART1 clock */
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_I2C1SEL_POS)    |   /* select HSI16 as I2C1 clock */
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_I2S2SEL_POS)    |   /* select HSI16 as I2C2 clock */
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_LPTIM1SEL_POS)  |   /* select HSI16 as LPTIM1 clock */
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_LPTIM2SEL_POS)  |   /* select HSI16 as LPTIM2 clock */
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_RNGSEL_POS)     |   /* select HSI16 as RNG clock */
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_ADCSEL_POS)         /* select HSI16 as ADC clock */
        );
#endif //USE_HSI16_CLOCK_AS_PERIPH_SOURCE

#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    FSF_vBoot();
#endif
}

void BASIC_vConfigPlatform (void)
{
    /* Clock Configuration*/
    BASIC_vInitPlatform();

    /* Enable clock on GPIO */
    RCC_AHB2ENR = (uint32_t)(RCC_AHB2ENR_GPIOAEN_MASK | RCC_AHB2ENR_GPIOBEN_MASK | RCC_AHB2ENR_GPIOCEN_MASK | RCC_AHB2ENR_GPIODEN_MASK | RCC_AHB2ENR_GPIOEEN_MASK | RCC_AHB2ENR_GPIOHEN_MASK);
#ifdef APP_VARIANT
    /* Here should be placed all application specific initialization */

    /* Set Priority of configurable interrupts */
    // setDefaultInterruptPriority();

    // HADC0_vInit(0);

    /* all other initializations */
    // ...
#endif
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
/** \brief    Sets PLL configuration based on input parameters
 *
 *  \details  BASIC_vSetPLL function is used to setup PLL configuration register
 *            based on the imput parameters
 *
 *  \param    divFactorPLLM      PLL multipier is automaticaly set by preprocessor
 *            mulFactorPLLN      PLL divider is automaticaly set by preprocessor
 *            divFactorPLLR      PLLCLK output is automaticaly set by preprocessor
 *            divFactorPLLQ      PLL output is automaticaly set by preprocessor
 */
static void BASIC_vSetPLL(uint32_t divFactorPLLM, uint32_t mulFactorPLLN, uint32_t divFactorPLLR, uint32_t divFactorPLLQ)
{
    RCC_CFGR = BSC_RCC_CFGR_SW_HSI; // set HSI16 
    // wait until HSI16 stabilized as clock source
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != BSC_RCC_CFGR_SWS_HSI)
    {
        
    }
    RCC_CR_PLLON = 0u; // switch PLL off to set registers
    // wait until PLL is turned off
    while( RCC_CR_PLLRDY != 0u )
    {
    }
    RCC_PLLCFGR = (uint32_t)(
        SHFT_BIT(BSC_RCC_PLLCFGR_PLLSRC, RCC_PLLCFGR_PLLSRC_POS)|   /* selection PLL clock source           */  //lint !e835 better readability even when shift is zero
        SHFT_BIT(divFactorPLLM,   RCC_PLLCFGR_PLLM_POS)  |   /* select the PLL divider as specified   */
        SHFT_BIT(mulFactorPLLN,   RCC_PLLCFGR_PLLN_POS)  |   /* select the PLL multipier as specified */
        SHFT_BIT(divFactorPLLQ, RCC_PLLCFGR_PLLQ_POS)  |    /* select the Q-Clock divider            */
        SHFT_BIT(BSC_RCC_PLLCFGR_PLLQEN_ENABLE, RCC_PLLCFGR_PLLQEN_POS)  |  /* PLL48M1CLK output enable */
        SHFT_BIT(divFactorPLLR,   RCC_PLLCFGR_PLLR_POS)  |   /* select the R-Clock divider            */
        SHFT_BIT(BSC_RCC_PLLCFGR_PLLREN_ENABLE, RCC_PLLCFGR_PLLREN_POS)    /* enable the RCLOCK to be used as system clock */
    );
    RCC_CR_PLLON = 1u; // switch PLL on
    // wait until PLL ready
    while( RCC_CR_PLLRDY == 0u )
    {
        
    }
    /* now set the PLL as system clock, prescalers */
    RCC_CFGR =  SHFT_BIT(BSC_RCC_CFGR_SW_PLL, RCC_CFGR_SW_POS) |    /* select PLL as system clock */ //lint !e835 better readability even when shift is zero
                BSC_RCC_CFGR_HPRE_SYSCLK_DIV |                      /* set APB prescaler */
                BSC_RCC_CFGR_PPRE1 |                                /* set AHB1 prescaler */
                BSC_RCC_CFGR_PPRE2 ;                                /* set AHB2 prescaler */
    // wait until PLL stabilized as clock source
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != BSC_RCC_CFGR_SWS_PLL)
    {
        
    }
}

