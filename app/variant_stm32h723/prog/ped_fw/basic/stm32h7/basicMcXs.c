/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          MCU Framework
 *   MODULE-PREFIX    BASIC
 *   AUTHOR           Jens Lehmann
 *   CREATED          20.10.2022
 *******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief  implementation of micro controller initialisation and configuration.
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "basic.h"
#include "processor.h"
#include "hwdt.h"
#include "hsup.h"
#ifdef APP_VARIANT
//#include "hadc.h" // for Stm32h7 not implemented
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
#include "fSafe.h"
#endif


/*****************************************************************************
*-----------------------------------------------------------------------------
*        System Clock source                    | HSI 64 or HSE 4-48
*-----------------------------------------------------------------------------
*        CPU Clock (MHz)                        | max 550
*-----------------------------------------------------------------------------
*        SYSCLK (MHz)                           | max 550
*-----------------------------------------------------------------------------
*        Systick clock (MHz)                    | max 68,75
*-----------------------------------------------------------------------------
*        AXI Peripheral Clock (MHz)             | max 275
*-----------------------------------------------------------------------------
*        APB1 (MHz) - PCLOCK1                   | max 137,5
*-----------------------------------------------------------------------------
*        APB1 Timer Clock (MHz)                 | max 275
*-----------------------------------------------------------------------------
*        APB2 (MHz) - PCLOCK2                   | max 137,5
*-----------------------------------------------------------------------------
*        APB2 Timer Clock (MHz)                 | max 275
*-----------------------------------------------------------------------------
*        APB3 (MHz) - PCLOCK3                   | max 137,5
*-----------------------------------------------------------------------------
*        APB4 (MHz) - PCLOCK4                   | max 137,5
*-----------------------------------------------------------------------------
*        AHB1,2 (MHz) - PCLOCK                  | max 275
*-----------------------------------------------------------------------------
*        HCLK3 (MHz)                            | max 275
*-----------------------------------------------------------------------------
*        D1CPRE Prescaler                       | 1
*-----------------------------------------------------------------------------
*        PLL1                                   | ON - Source for SYSCLK
*-----------------------------------------------------------------------------
*        PLL1_P                                 | 1
*-----------------------------------------------------------------------------
*        PLL1_Q                                 | 4
*-----------------------------------------------------------------------------
*        PLL1_R                                 | OFF
*-----------------------------------------------------------------------------
*        PLL2                                   | OFF
*-----------------------------------------------------------------------------
*        PLL3                                   | ON - Source for ADC
*-----------------------------------------------------------------------------
*        PLL3_P                                 | OFF
*-----------------------------------------------------------------------------
*        PLL3_Q                                 | OFF
*-----------------------------------------------------------------------------
*        PLL3_R                                 | ON
*-----------------------------------------------------------------------------
*****************************************************************************/
/*lint -save -e923 -e750 -e845*/
//lint -esym( 750, HSI64*) "message: symbol not used..."
#define  HSI64      64U    // 64MHz High Speed Internal Oscilator

#define BSC_Voltage_scale_VOS0     (0x0000u)
#define BSC_Voltage_scale_VOS1     (0xC000u)
#define BSC_Voltage_scale_VOS2     (0x8000u)
#define BSC_Voltage_scale_VOS3     (0x4000u)

#if   (CORE_CLOCK <= 170u)
    #define    BSC_VOLTAGE_SCALE   BSC_Voltage_scale_VOS3
    #define    BSC_AHB_MAX         (85u)
    #define    BSC_APB_MAX         (43u)
#elif (CORE_CLOCK <= 300u)
    #define    BSC_VOLTAGE_SCALE   BSC_Voltage_scale_VOS2
    #define    BSC_AHB_MAX         (150u)
    #define    BSC_APB_MAX         (75u)
#elif (CORE_CLOCK <= 400u)
    #define    BSC_VOLTAGE_SCALE   BSC_Voltage_scale_VOS1
    #define    BSC_AHB_MAX         (200u)
    #define    BSC_APB_MAX         (100u)
#elif (CORE_CLOCK <= 550u)
    #define    BSC_VOLTAGE_SCALE   BSC_Voltage_scale_VOS0
    #define    BSC_AHB_MAX         (275u)
    #define    BSC_APB_MAX         (138u) // the max frequency for APB is 137,5MHz
#else
    #error "No valid CORE_CLOCK. Maximum frequency is 550MHz."
#endif

#define BSC_LATENCY_0WS         (0u)
#define BSC_LATENCY_1WS         (1u)
#define BSC_LATENCY_2WS         (2u)
#define BSC_LATENCY_3WS         (3u)
#define BSC_LATENCY_MAX         BSC_LATENCY_3WS

    // VOS3 range 0.95 V - 1.05 V
#if   ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS3) && (BSC_AHB_MAX <= 35u))
    #define BSC_LATENCY    BSC_LATENCY_0WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS3) && (BSC_AHB_MAX <= 70u))
    #define BSC_LATENCY    BSC_LATENCY_1WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS3) && (BSC_AHB_MAX <= 85u))
    #define BSC_LATENCY    BSC_LATENCY_2WS
    // VOS2 range 1.05 V - 1.15 V
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS2) && (BSC_AHB_MAX <= 50u))
    #define BSC_LATENCY    BSC_LATENCY_0WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS2) && (BSC_AHB_MAX <= 100u))
    #define BSC_LATENCY    BSC_LATENCY_1WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS2) && (BSC_AHB_MAX <= 150u))
    #define BSC_LATENCY    BSC_LATENCY_2WS
    // VOS1 range 1.15 V - 1.26 V
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS1) && (BSC_AHB_MAX <= 67u))
    #define BSC_LATENCY    BSC_LATENCY_0WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS1) && (BSC_AHB_MAX <= 133u))
    #define BSC_LATENCY    BSC_LATENCY_1WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS1) && (BSC_AHB_MAX <= 200u))
    #define BSC_LATENCY    BSC_LATENCY_2WS
    // VOS0 range 1.26 V - 1.40 V
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS0) && (BSC_AHB_MAX <= 70u))
    #define BSC_LATENCY    BSC_LATENCY_0WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS0) && (BSC_AHB_MAX <= 140u))
    #define BSC_LATENCY    BSC_LATENCY_1WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS0) && (BSC_AHB_MAX <= 210u))
    #define BSC_LATENCY    BSC_LATENCY_2WS
#elif ((BSC_VOLTAGE_SCALE == BSC_Voltage_scale_VOS0) && (BSC_AHB_MAX <= 275u))
    #define BSC_LATENCY    BSC_LATENCY_3WS
#else
    #error "The LATENCY can not be set."
#endif

//lint -esym( 750, D1PPRE_DIV*) "message: symbol not used..."
#define  D1PPRE_DIV1  0x0U
#define  D1PPRE_DIV2  0x4U
#define  D1PPRE_DIV4  0x5U
#define  D1PPRE_DIV8  0x6U
#define  D1PPRE_DIV16 0x7U

//lint -esym( 750, HPRE_DIV*) "message: symbol not used..."
#define  HPRE_DIV1    0x0U
#define  HPRE_DIV2    0x8U
#define  HPRE_DIV4    0x9U
#define  HPRE_DIV8    0xAU
#define  HPRE_DIV16   0xBU
#define  HPRE_DIV64   0xCU
#define  HPRE_DIV128  0xDU
#define  HPRE_DIV256  0xEU
#define  HPRE_DIV512  0xFU

//lint -esym( 750, SYSTEM_RCC_CFGR_*) "message: symbol not used..."
/** These defines determines divider of the SYSCLK and output clock is called the HCLK frequency that
supplies AHB1..4 (pclock) and APB1..4 (pclock1..4/2).
Maximum frequency of the HCLK frequency can be 275MHZ. The default macro is SYSTEM_RCC_CFGR_HPRE_DIV1 and now there is no
need to control this prescaler from the build process. */
#define BSC_HPRE_DIV1           (HPRE_DIV1) // shift not done left argument is zero   << RCC_D1CFGR_HPRE_POS)
#define BSC_HPRE_DIV2           ((uint32_t)HPRE_DIV2   << (uint32_t)RCC_D1CFGR_HPRE_POS)
#define BSC_HPRE_DIV4           ((uint32_t)HPRE_DIV4   << (uint32_t)RCC_D1CFGR_HPRE_POS)
#define BSC_HPRE_DIV8           ((uint32_t)HPRE_DIV8   << (uint32_t)RCC_D1CFGR_HPRE_POS)
#define BSC_HPRE_DIV16          ((uint32_t)HPRE_DIV16  << (uint32_t)RCC_D1CFGR_HPRE_POS)
#define BSC_HPRE_DIV64          ((uint32_t)HPRE_DIV64  << (uint32_t)RCC_D1CFGR_HPRE_POS)
#define BSC_HPRE_DIV128         ((uint32_t)HPRE_DIV128 << (uint32_t)RCC_D1CFGR_HPRE_POS)
#define BSC_HPRE_DIV256         ((uint32_t)HPRE_DIV256 << (uint32_t)RCC_D1CFGR_HPRE_POS)
#define BSC_HPRE_DIV512         ((uint32_t)HPRE_DIV512 << (uint32_t)RCC_D1CFGR_HPRE_POS)


/** These defines determines divider of the HCLK for the bus APB1 (frequency pclock1). Maximum frequency
of the bus can be 137,5MHZ. The coresponding macro is chosen according to user defined define PCLOCK1 in the build process. */
#define BSC_D2PPRE1_DIV1        (D1PPRE_DIV1) // shift not done left argument is zero
#define BSC_D2PPRE1_DIV2        ((uint32_t)D1PPRE_DIV2  << (uint32_t)RCC_D2CFGR_D2PPRE1_POS)
#define BSC_D2PPRE1_DIV4        ((uint32_t)D1PPRE_DIV4  << (uint32_t)RCC_D2CFGR_D2PPRE1_POS)
#define BSC_D2PPRE1_DIV8        ((uint32_t)D1PPRE_DIV8  << (uint32_t)RCC_D2CFGR_D2PPRE1_POS)
#define BSC_D2PPRE1_DIV16       ((uint32_t)D1PPRE_DIV16 << (uint32_t)RCC_D2CFGR_D2PPRE1_POS)

/** These defines determines divider of the HCLK for the bus APB2 (frequency pclock2). Maximum frequency
of the bus can be 137,5MHZ. The coresponding macro is chosen according to user defined define PCLOCK2 in the build process. */
#define BSC_D2PPRE2_DIV1        (D1PPRE_DIV1) // shift not done left argument is zero
#define BSC_D2PPRE2_DIV2        ((uint32_t)D1PPRE_DIV2  << (uint32_t)RCC_D2CFGR_D2PPRE2_POS)
#define BSC_D2PPRE2_DIV4        ((uint32_t)D1PPRE_DIV4  << (uint32_t)RCC_D2CFGR_D2PPRE2_POS)
#define BSC_D2PPRE2_DIV8        ((uint32_t)D1PPRE_DIV8  << (uint32_t)RCC_D2CFGR_D2PPRE2_POS)
#define BSC_D2PPRE2_DIV16       ((uint32_t)D1PPRE_DIV16 << (uint32_t)RCC_D2CFGR_D2PPRE2_POS)

/** These defines determines divider of the HCLK for the bus APB3 (frequency pclock3). Maximum frequency
of the bus can be 137,5MHZ. The coresponding macro is chosen according to user defined define PCLOCK3 in the build process. */
#define BSC_D1PPRE_DIV1         (D1PPRE_DIV1) // shift not done left argument is zero
#define BSC_D1PPRE_DIV2         ((uint32_t)D1PPRE_DIV2  << (uint32_t)RCC_D1CFGR_D1PPRE_POS)
#define BSC_D1PPRE_DIV4         ((uint32_t)D1PPRE_DIV4  << (uint32_t)RCC_D1CFGR_D1PPRE_POS)
#define BSC_D1PPRE_DIV8         ((uint32_t)D1PPRE_DIV8  << (uint32_t)RCC_D1CFGR_D1PPRE_POS)
#define BSC_D1PPRE_DIV16        ((uint32_t)D1PPRE_DIV16 << (uint32_t)RCC_D1CFGR_D1PPRE_POS)

/** These defines determines divider of the HCLK for the bus APB4 (frequency pclock4). Maximum frequency
of the bus can be 137,5MHZ. The coresponding macro is chosen according to user defined define PCLOCK4 in the build process. */
#define BSC_D3PPRE_DIV1         (D1PPRE_DIV1) // shift not done left argument is zero
#define BSC_D3PPRE_DIV2         ((uint32_t)D1PPRE_DIV2  << (uint32_t)RCC_D3CFGR_D3PPRE_POS)
#define BSC_D3PPRE_DIV4         ((uint32_t)D1PPRE_DIV4  << (uint32_t)RCC_D3CFGR_D3PPRE_POS)
#define BSC_D3PPRE_DIV8         ((uint32_t)D1PPRE_DIV8  << (uint32_t)RCC_D3CFGR_D3PPRE_POS)
#define BSC_D3PPRE_DIV16        ((uint32_t)D1PPRE_DIV16 << (uint32_t)RCC_D3CFGR_D3PPRE_POS)

#define  MIN_SYSCLK   8U
#define  MAX_SYSCLK   170U
#define  MAX_PCLOCK   170U

/* PLL_M calculation */
#ifdef EXT_CRYSTAL_OSC
    #define SRC_CLK     EXT_CRYSTAL_OSC
    #define BSC_PLL_SRC (2u)    // 10: HSE selected as PLL clock (hse_ck)
    #if ((EXT_CRYSTAL_OSC < 4U) || (48U < EXT_CRYSTAL_OSC))
        #error "EXT_CRYSTAL_OSC out of boundaries (4-48MHz)."
    #endif
#else
    #define SRC_CLK     HSI64
    #define BSC_PLL_SRC (0u)    // 00: HSI selected as PLL clock (hsi_ck)
#endif

#define PLLM_MAX    16U
#define PLLM_MIN     1U
// output from PLLM must be 1-2MHz (VCOL) or 2-16MHz (VCOH)
    /* 4MHz PLL input preferred, so it's checked first */
#if    (((SRC_CLK % 4U) == 0U) && ((SRC_CLK / (SRC_CLK / 4U)) <= PLLM_MAX) && ((SRC_CLK / (SRC_CLK / 4U)) >= PLLM_MIN))
    #define BSC_PLLM (4U)
#elif  (((SRC_CLK % 10U)== 0U) && ((SRC_CLK / (SRC_CLK / 10U))<= PLLM_MAX) && ((SRC_CLK / (SRC_CLK / 10U))>= PLLM_MIN))
    #define BSC_PLLM (10U)
#elif  (((SRC_CLK % 7U) == 0U) && ((SRC_CLK / (SRC_CLK / 7U)) <= PLLM_MAX) && ((SRC_CLK / (SRC_CLK / 7U)) >= PLLM_MIN))
    #define BSC_PLLM (7U)
#elif  (((SRC_CLK % 6U) == 0U) && ((SRC_CLK / (SRC_CLK / 6U)) <= PLLM_MAX) && ((SRC_CLK / (SRC_CLK / 6U)) >= PLLM_MIN))
    #define BSC_PLLM (6U)
#elif  (((SRC_CLK % 5U) == 0U) && ((SRC_CLK / (SRC_CLK / 5U)) <= PLLM_MAX) && ((SRC_CLK / (SRC_CLK / 5U)) >= PLLM_MIN))
    #define BSC_PLLM (5U)
#elif  (((SRC_CLK % 3U) == 0U) && ((SRC_CLK / (SRC_CLK / 3U)) <= PLLM_MAX) && ((SRC_CLK / (SRC_CLK / 3U)) >= PLLM_MIN))
    #define BSC_PLLM (3U)
#elif  (((SRC_CLK % 2U) == 0U) && ((SRC_CLK / (SRC_CLK / 2U)) <= PLLM_MAX) && ((SRC_CLK / (SRC_CLK / 2U)) >= PLLM_MIN))
    #define BSC_PLLM (2U)
#else
    #error "No valid EXT_CRYSTAL_OSC value defined."
#endif
#define BSC_PLLM_OUT_MHZ    (SRC_CLK / BSC_PLLM)

#if   ((BSC_PLLM_OUT_MHZ >= 2u) && (BSC_PLLM_OUT_MHZ <= PLLM_MAX))
    #define BSC_VCOSEL          (0u)                             // 0: Wide VCO range: 192 to 836MHz (default after reset) - VCOH
    #define BSC_DIVN_OUT_MAX    (836u)
    #define BSC_DIVN_OUT_MIN    (192u)
#elif ((BSC_PLLM_OUT_MHZ == 1u) || (BSC_PLLM_OUT_MHZ == 2u))
    #define BSC_VCOSEL          (1u)                             // 1: Medium VCO range: 150 to 420MHz - VCOL
    #define BSC_DIVN_OUT_MAX    (420u)
    #define BSC_DIVN_OUT_MIN    (150u)
#else
    #error "No valid input for PLLM. Valid values 1..16"
#endif

// setting for PLLxRGE bits in RCC_PLLCFGR register
#if   (BSC_PLLM_OUT_MHZ <= 2u)
    #define   BSC_PLL_INPUT_FREQ_RANGE    (0U)
#elif (BSC_PLLM_OUT_MHZ <= 4u)
    #define    BSC_PLL_INPUT_FREQ_RANGE   (1U)
#elif (BSC_PLLM_OUT_MHZ <= 8u)
    #define    BSC_PLL_INPUT_FREQ_RANGE   (2U)
#elif (BSC_PLLM_OUT_MHZ <= 16u)
    #define    BSC_PLL_INPUT_FREQ_RANGE   (3U)
#else
    #error "No valid clock for PLL input."
#endif

// output from DIVN must be 150-420MHz (VCOL) or 192-836MHz (VCOH)
#define BSC_PLLN_OUT_DIV    (0xFFFFu & (CORE_CLOCK / BSC_PLLM_OUT_MHZ))        // 0xFFFFu & - cast to uint16_t
#define BSC_PLLN_OUT_MHZ    (0xFFFFu & (BSC_PLLN_OUT_DIV * BSC_PLLM_OUT_MHZ))  // 0xFFFFu & - cast to uint16_t
#if  ((BSC_PLLN_OUT_MHZ > BSC_DIVN_OUT_MAX) || (BSC_PLLN_OUT_MHZ < BSC_DIVN_OUT_MIN))
    #error "Output from PLLN is out of range."
#endif

#if ((CORE_CLOCK - BSC_PLLN_OUT_MHZ) == 0U)
    #define BSC_PLL1_FRACT   0U
#else
    #define BSC_PLL1_FRACT   (uint32_t)(((((CORE_CLOCK * 1000000u) / BSC_PLLM_OUT_MHZ) - (BSC_PLLN_OUT_DIV * 1000000u)) * 8192u) / 1000000u)
#endif


#if ((BSC_PLLN_OUT_MHZ % CLOCK_ADC) > 0u)
    #define BSC_PLL3R_DIV               ((uint16_t)((BSC_PLLN_OUT_MHZ / CLOCK_ADC) + 1u))
    // Fraction calculation
    #define PLL3_FRACT_STEP_BASE0       ((long double)(BSC_PLLM_OUT_MHZ * ((uint8_t)BSC_PLLN_OUT_DIV + ((long double)0u / 8192u))))
    #define PLL3_FRACT_STEP_BASE1       ((long double)(BSC_PLLM_OUT_MHZ * ((uint8_t)BSC_PLLN_OUT_DIV + ((long double)1u / 8192u))))
    #define PLL3_FRACT_STEP_BASE0_DIVR  ((long double)(PLL3_FRACT_STEP_BASE0 / BSC_PLL3R_DIV))
    #define PLL3_FRACT_STEP_BASE1_DIVR  ((long double)(PLL3_FRACT_STEP_BASE1 / BSC_PLL3R_DIV))
    #define PLL3_FRACT_STEP_DIVR_DIFF   ((long double)(PLL3_FRACT_STEP_BASE1_DIVR - PLL3_FRACT_STEP_BASE0_DIVR))
    #define PLL3_FRACT_CALC             ((long double)(CLOCK_ADC - PLL3_FRACT_STEP_BASE0_DIVR))
    #define BSC_PLL3_FRACT              ((uint32_t)((PLL3_FRACT_CALC) / (PLL3_FRACT_STEP_DIVR_DIFF)))
#else
    #define BSC_PLL3R_DIV               ((uint16_t)(BSC_PLLN_OUT_MHZ / CLOCK_ADC))
    #define BSC_PLL3_FRACT              (0u)
#endif


#if    ((((CORE_CLOCK / PCLOCK) % 512u) == 0U) && ((CORE_CLOCK / 512u) <= BSC_AHB_MAX))
    #define BSC_HPRE_DIV    BSC_HPRE_DIV512
    #define BSC_HPRE_MHZ    (CORE_CLOCK / 512u)
#elif  ((((CORE_CLOCK / PCLOCK) % 256u) == 0U) && ((CORE_CLOCK / 256u) <= BSC_AHB_MAX))
    #define BSC_HPRE_DIV    BSC_HPRE_DIV256
    #define BSC_HPRE_MHZ    (CORE_CLOCK / 256u)
#elif  ((((CORE_CLOCK / PCLOCK) % 128u) == 0U) && ((CORE_CLOCK / 128u) <= BSC_AHB_MAX))
    #define BSC_HPRE_DIV    BSC_HPRE_DIV128
    #define BSC_HPRE_MHZ    (CORE_CLOCK / 128u)
#elif  ((((CORE_CLOCK / PCLOCK) % 64u) == 0U)  && ((CORE_CLOCK /  64u) <= BSC_AHB_MAX))
    #define BSC_HPRE_DIV    BSC_HPRE_DIV64
    #define BSC_HPRE_MHZ    (CORE_CLOCK / 64u)
#elif  ((((CORE_CLOCK / PCLOCK) % 16u) == 0U)  && ((CORE_CLOCK /  16u) <= BSC_AHB_MAX))
    #define BSC_HPRE_DIV    BSC_HPRE_DIV16
    #define BSC_HPRE_MHZ    (CORE_CLOCK / 16u)
#elif  ((((CORE_CLOCK / PCLOCK) % 8u ) == 0U)  && ((CORE_CLOCK /   8u) <= BSC_AHB_MAX))
    #define BSC_HPRE_DIV    BSC_HPRE_DIV8
    #define BSC_HPRE_MHZ    (CORE_CLOCK / 8u)
#elif  ((((CORE_CLOCK / PCLOCK) % 4u ) == 0U)  && ((CORE_CLOCK /   4u) <= BSC_AHB_MAX))
    #define BSC_HPRE_DIV    BSC_HPRE_DIV4
    #define BSC_HPRE_MHZ    (CORE_CLOCK / 4u)
#elif  ((((CORE_CLOCK / PCLOCK) % 2u ) == 0U)  && ((CORE_CLOCK /   2u) <= BSC_AHB_MAX))
    #define BSC_HPRE_DIV    BSC_HPRE_DIV2
    #define BSC_HPRE_MHZ    (CORE_CLOCK / 2u)
#else
    #error "PCLOCK (AHB) must be at least 2 (may be 4, 8, 16, 64, 128, 256 and 512) times lower as CORE_CLOCK."
#endif

/* PCLOCK1 (framework naming) - APB1 (STm naming) - D2PPRE1 (bits) */
#if    ((((BSC_HPRE_MHZ / PCLOCK1) % 16u) == 0U)&& ((BSC_HPRE_MHZ / 16u)<= BSC_APB_MAX))
    #define BSC_D2PPRE1_DIV    BSC_D2PPRE1_DIV16
#elif  ((((BSC_HPRE_MHZ / PCLOCK1) % 8u) == 0U) && ((BSC_HPRE_MHZ / 8u) <= BSC_APB_MAX))
    #define BSC_D2PPRE1_DIV    BSC_D2PPRE1_DIV8
#elif  ((((BSC_HPRE_MHZ / PCLOCK1) % 4u) == 0U) && ((BSC_HPRE_MHZ / 4u) <= BSC_APB_MAX))
    #define BSC_D2PPRE1_DIV    BSC_D2PPRE1_DIV4
#elif  ((((BSC_HPRE_MHZ / PCLOCK1) % 2u) <= 1U) && ((BSC_HPRE_MHZ / 2u) <= BSC_APB_MAX))
    // The value 138 sets PCLOCK1 to 137.5MHz (It is max clock for APB1 for 550MHz CORE_CLOCK)
    #define BSC_D2PPRE1_DIV    BSC_D2PPRE1_DIV2
#else
    #error "PCLOCK1 (APB1) must be at least 2 (may be 4, 8 and 16) times lower as PCLOCK."
#endif

/* PCLOCK2 (framework naming) - APB2 (STm naming) - D2PPRE2 (bits) */
#if    ((((BSC_HPRE_MHZ / PCLOCK2) % 16u) == 0U)&& ((BSC_HPRE_MHZ / 16u)<= BSC_APB_MAX))
    #define BSC_D2PPRE2_DIV    BSC_D2PPRE2_DIV16
#elif  ((((BSC_HPRE_MHZ / PCLOCK2) % 8u) == 0U) && ((BSC_HPRE_MHZ / 8u) <= BSC_APB_MAX))
    #define BSC_D2PPRE2_DIV    BSC_D2PPRE2_DIV8
#elif  ((((BSC_HPRE_MHZ / PCLOCK2) % 4u) == 0U) && ((BSC_HPRE_MHZ / 4u) <= BSC_APB_MAX))
    #define BSC_D2PPRE2_DIV    BSC_D2PPRE2_DIV4
#elif  ((((BSC_HPRE_MHZ / PCLOCK2) % 2u) <= 1U) && ((BSC_HPRE_MHZ / 2u) <= BSC_APB_MAX))
    // The value 138 sets PCLOCK2 to 137.5 MHz (It is max clock for APB2 for 550MHz CORE_CLOCK)
    #define BSC_D2PPRE2_DIV    BSC_D2PPRE2_DIV2
#else
    #error "PCLOCK2 (APB2) must be at least 2 (may be 4, 8 and 16) times lower as PCLOCK."
#endif

/* PCLOCK3 (framework naming) - APB3 (STm naming) - D1PPRE (bits) */
#if    ((((BSC_HPRE_MHZ / PCLOCK3) % 16u) == 0U)&& ((BSC_HPRE_MHZ / 16u)<= BSC_APB_MAX))
    #define BSC_D1PPRE_DIV    BSC_D1PPRE_DIV16
#elif  ((((BSC_HPRE_MHZ / PCLOCK3) % 8u) == 0U) && ((BSC_HPRE_MHZ / 8u) <= BSC_APB_MAX))
    #define BSC_D1PPRE_DIV    BSC_D1PPRE_DIV8
#elif  ((((BSC_HPRE_MHZ / PCLOCK3) % 4u) == 0U) && ((BSC_HPRE_MHZ / 4u) <= BSC_APB_MAX))
    #define BSC_D1PPRE_DIV    BSC_D1PPRE_DIV4
#elif  ((((BSC_HPRE_MHZ / PCLOCK3) % 2u) <= 1U) && ((BSC_HPRE_MHZ / 2u) <= BSC_APB_MAX))
    // The value 138 sets PCLOCK3 to 137.5MHz (It is max clock for APB3 for 550MHz CORE_CLOCK)
    #define BSC_D1PPRE_DIV    BSC_D1PPRE_DIV2
#else
    #error "PCLOCK3 (APB3) must be at least 2 (may be 4, 8 and 16) times lower as PCLOCK."
#endif

/* PCLOCK4 (framework naming) - APB4 (STm naming) - D3PPRE (bits) */
#if    ((((BSC_HPRE_MHZ / PCLOCK4) % 16u) == 0U)&& ((BSC_HPRE_MHZ / 16u)<= BSC_APB_MAX))
    #define BSC_D3PPRE_DIV    BSC_D3PPRE_DIV16
#elif  ((((BSC_HPRE_MHZ / PCLOCK4) % 8u) == 0U) && ((BSC_HPRE_MHZ / 8u) <= BSC_APB_MAX))
    #define BSC_D3PPRE_DIV    BSC_D3PPRE_DIV8
#elif  ((((BSC_HPRE_MHZ / PCLOCK4) % 4u) == 0U) && ((BSC_HPRE_MHZ / 4u) <= BSC_APB_MAX))
    #define BSC_D3PPRE_DIV    BSC_D3PPRE_DIV4
#elif  ((((BSC_HPRE_MHZ / PCLOCK4) % 2u) <= 1U) && ((BSC_HPRE_MHZ / 2u) <= BSC_APB_MAX))
    // The value 138 sets PCLOCK4 to 137.5MHz (It is max clock for APB4 for 550MHz CORE_CLOCK)
    #define BSC_D3PPRE_DIV    BSC_D3PPRE_DIV2
#else
    #error "PCLOCK4 (APB4) must be at least 2 (may be 4, 8 and 16) times lower as PCLOCK."
#endif

/** \brief shifts a bitfield (value) to the desired position (shift) */
#define SHIFT_BIT(value,shift) ((uint32_t)(((uint32_t)(value) << ((uint32_t)(shift))))) //lint !e9026 macro needed to be resolved at compile time

#define BSC_RCC_CFGR_SWS_HSI    (0u)        ///< HSI64 used as system clock
#define BSC_RCC_CFGR_SW_PLL     (3uL)       ///< PLL as system clock
#define BSC_RCC_CFGR_SWS_PLL    SHIFT_BIT(BSC_RCC_CFGR_SW_PLL, RCC_CFGR_SWS_POS)   ///< PLL used as system clock

/* MPU Region Number */
#define MPU_RGN_NUMBER0     (0x00u)
#define MPU_RGN_NUMBER1     (0x01u)
#define MPU_RGN_NUMBER2     (0x02u)
#define MPU_RGN_NUMBER3     (0x03u)
#define MPU_RGN_NUMBER4     (0x04u)
#define MPU_RGN_NUMBER5     (0x05u)
#define MPU_RGN_NUMBER6     (0x06u)
#define MPU_RGN_NUMBER7     (0x07u)
#define MPU_RGN_NUMBER8     (0x08u)
#define MPU_RGN_NUMBER9     (0x09u)
#define MPU_RGN_NUMBER10    (0x0Au)
#define MPU_RGN_NUMBER11    (0x0Bu)
#define MPU_RGN_NUMBER12    (0x0Cu)
#define MPU_RGN_NUMBER13    (0x0Du)
#define MPU_RGN_NUMBER14    (0x0Eu)
#define MPU_RGN_NUMBER15    (0x0Fu)

/* MPU Region Enable */
#define MPU_RGN_ENABLE      (0x01u)
#define MPU_RGN_DISABLE     (0x00u)

/* MPU Region Size */
#define MPU_RGN_SIZE_32B      (0x04u)
#define MPU_RGN_SIZE_64B      (0x05u)
#define MPU_RGN_SIZE_128B     (0x06u)
#define MPU_RGN_SIZE_256B     (0x07u)
#define MPU_RGN_SIZE_512B     (0x08u)
#define MPU_RGN_SIZE_1KB      (0x09u)
#define MPU_RGN_SIZE_2KB      (0x0Au)
#define MPU_RGN_SIZE_4KB      (0x0Bu)
#define MPU_RGN_SIZE_8KB      (0x0Cu)
#define MPU_RGN_SIZE_16KB     (0x0Du)
#define MPU_RGN_SIZE_32KB     (0x0Eu)
#define MPU_RGN_SIZE_64KB     (0x0Fu)
#define MPU_RGN_SIZE_128KB    (0x10u)
#define MPU_RGN_SIZE_256KB    (0x11u)
#define MPU_RGN_SIZE_512KB    (0x12u)
#define MPU_RGN_SIZE_1MB      (0x13u)
#define MPU_RGN_SIZE_2MB      (0x14u)
#define MPU_RGN_SIZE_4MB      (0x15u)
#define MPU_RGN_SIZE_8MB      (0x16u)
#define MPU_RGN_SIZE_16MB     (0x17u)
#define MPU_RGN_SIZE_32MB     (0x18u)
#define MPU_RGN_SIZE_64MB     (0x19u)
#define MPU_RGN_SIZE_128MB    (0x1Au)
#define MPU_RGN_SIZE_256MB    (0x1Bu)
#define MPU_RGN_SIZE_512MB    (0x1Cu)
#define MPU_RGN_SIZE_1GB      (0x1Du)
#define MPU_RGN_SIZE_2GB      (0x1Eu)
#define MPU_RGN_SIZE_4GB      (0x1Fu)

/* MPU Execute Never encoding */
#define MPU_INSTRUCTION_EXEC_PERMITTED        (0x00u)
#define MPU_INSTRUCTION_EXEC_NOT_PERMITED     (0x01u)

/* MPU Access Permissions Field encoding*/
#define MPU_AP_NO_ACCESS                        (0x00u)
#define MPU_AP_PRIVILIGED_RW                    (0x01u)
#define MPU_AP_PRIVILIGED_RW_UNPRIVILIGED_RO    (0x02u)
#define MPU_AP_FULL_ACCESS                      (0x03u)
#define MPU_AP_PRIVILIGED_RO                    (0x05u)
#define MPU_AP_READ_ONLY                        (0x06u)


/* MPU Bufferable Bit encoding*/
#define MPU_RGN_BUFFERABLE         (0x01u)
#define MPU_RGN_NOT_BUFFERABLE     (0x00u)

/* MPU Cacheable Bit encoding*/
#define MPU_RGN_CACHEABLE         (0x01u)
#define MPU_RGN_NOT_CACHEABLE     (0x00u)

/* MPU Shareable Bit encoding*/
#define MPU_RGN_SHAREABLE         (0x01u)
#define MPU_RGN_NOT_SHAREABLE     (0x00u)

/* MPU Type Extension Field encoding */
#define MPU_RGN_TEX0     (0x00u)
#define MPU_RGN_TEX1     (0x01u)
#define MPU_RGN_TEX2     (0x02u)

/* MPU HFNMI and PRIVDEF Bits encoding */
#define  MPU_HFNMI_PRIVDEF_DISABLE        (0x00u)
#define  MPU_HFNMI_ENABLE                 (0x02u)
#define  MPU_PRIVDEF_ENABLE               (0x04u)
#define  MPU_HFNMI_PRIVDEF_ENABLE         (0x06u)

struct MPU_RegionConfig 
{
    uint8_t  rgnNumber;
    uint32_t rgnBaseAddress;
    uint8_t  rgnEnable;
    uint8_t  rgnSize;
    uint8_t  subRgnDisable;
    uint8_t  executeNever;
    uint8_t  accPermission;
    uint8_t  typeExtension;
    uint8_t  isBufferable;
    uint8_t  isCacheable;
    uint8_t  isShareable;
};


/** \brief Sets the PLL */
static void BASIC_vSetPLL(void);
/** \brief Sets the Voltage scaling to power V_CORE */
static void BASIC_vSetVoltageScaling(const uint32_t voltageScale);
/** \brief Sets HPRE and APB buses */
static void BASIC_vSetDividers(void);
/** \brief Sets the number of wait states (LATENCY) for FLASH memory */
static void BASIC_vSetLatency(uint8_t latency);

/** \brief Enables MPU */
static void BASIC_vEnableMpu(uint32_t mpuCtrl);
/** \brief Disables MPU */
static void BASIC_vDisableMpu(void);
/** \brief Configures MPU */
static void BASIC_vCofigureMpuRegion(struct MPU_RegionConfig const* config);

#if defined(APP_VARIANT) && defined(ENABLE_OPTION_BYTES_CONFIG)
#define  FLASH_KEYR1_KEY_1           (0x45670123U)
#define  FLASH_KEYR1_KEY_2           (0xCDEF89ABU)
#define  FLASH_OPTKEYR_KEY_1         (0x08192A3BU)
#define  FLASH_OPTKEYR_KEY_2         (0x4C5D6E7FU)

/* Flash register masks for option bytes configuration */
#define  OB_CHANGE_FLASH_OPTSR_CUR_MASK     (FLASH_OPTSR_PRG_IO_HSLV_MASK | FLASH_OPTSR_PRG_SECURITY_MASK | FLASH_OPTSR_PRG_ST_RAM_SIZE_MASK | FLASH_OPTSR_PRG_FZ_IWDG_SDBY_MASK |\
                                             FLASH_OPTSR_PRG_FZ_IWDG_STOP_MASK | FLASH_OPTSR_PRG_RDP_MASK | FLASH_OPTSR_PRG_NRST_STBY_D1_MASK |\
                                             FLASH_OPTSR_PRG_NRST_STOP_D1_MASK | FLASH_OPTSR_PRG_IWDG1_HW_MASK | FLASH_OPTSR_PRG_BOR_LEV_MASK)
#define  OB_CHANGE_FLASH_OPTSR2_CUR_MASK    (FLASH_OPTSR2_PRG_CPUFREQ_BOOST_MASK | FLASH_OPTSR2_PRG_TCM_AXI_SHARED_MASK)
#define  OB_CHANGE_FLASH_BOOT_CURR_MASK     (FLASH_BOOT_PRGR_BOOT_ADD1_MASK | FLASH_BOOT_PRGR_BOOT_ADD0_MASK)
#define  OB_CHANGE_FLASH_PRAR_CUR1_MASK     (FLASH_PRAR_PRG1_DMEP1_MASK | FLASH_PRAR_PRG1_PROT_AREA_END1_MASK | FLASH_PRAR_PRG1_PROT_AREA_START1_MASK)
#define  OB_CHANGE_FLASH_SCAR_CUR1_MASK     (FLASH_SCAR_PRG1_DMES1_MASK | FLASH_SCAR_PRG1_SEC_AREA_END1_MASK | FLASH_SCAR_PRG1_SEC_AREA_START1_MASK)
#define  OB_CHANGE_FLASH_WPSN_CUR1R_MASK    (FLASH_WPSN_PRG1R_WRPSN1_MASK)

struct OB_Config
{
    uint32_t optsrPrgReg;
    uint32_t optsr2PrgReg;
    uint32_t bootPrgReg;
    uint32_t prarPrgReg;
    uint32_t scarPrgReg;
    uint32_t wpsnPrgReg;
};

/** \brief Unlocks flash register configuration */
static void BASIC_vUnlockFlashConfig(void);
/** \brief Locks flash register configuration */
static void BASIC_vLockFlashConfig(void);
/** \brief Unlocks option bytes configuration */
static void BASIC_vUnlockObConfig(void);
/** \brief Locks option bytes configuration */
static void BASIC_vLockObConfig(void);
/** \brief Configures option bytes */
static void BASIC_vConfigureOb(struct OB_Config const* obConfig);
/** \brief Ensures only option bytes are changed and reserved bits are preserved */
static void BASIC_vPrepareObValues(struct OB_Config *obConfig);
/** \brief Checks if there is difference in option bytes configuration */
static bool BASIC_bIsObConfigChange(struct OB_Config *obConfig);
#endif// APP_VARIANT && ENABLE_OPTION_BYTES_CONFIG


void BASIC_vInitPlatform(void)
{
    /* Set the clocks to default state as like after the hardware reset */

    BASIC_vSetLatency(BSC_LATENCY_MAX);
    // HSI64 clock on
    RCC_CR |= RCC_CR_HSION_MASK;
    /* Wait for HSIRDY = 1 (HSI64 is ready) */
    while ((RCC_CR & RCC_CR_HSIRDY_MASK) == 0u) { }

    // HSI64 selected as system clock
    RCC_CFGR_SW &= ~RCC_CFGR_SW_MASK;
    // Wait till HSI64 is used as system clock source
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != BSC_RCC_CFGR_SWS_HSI) { }

    // RCC_CR register set to the reset state (clear all bits except for HSION bit)
    RCC_CR &= RCC_CR_HSION_MASK;
    // Wait till HSE and PLL are disabled
    while ((RCC_CR & (RCC_CR_HSERDY_MASK  | \
                      RCC_CR_PLL1RDY_MASK | \
                      RCC_CR_PLL2RDY_MASK | \
                      RCC_CR_PLL3RDY_MASK)) != 0u) { }

    /* CFGR and D1CFGR registers to the reset state (prescalers to default state and system clock HSI) */
    RCC_CFGR = 0u;
    RCC_D1CFGR = 0u;

    /* Set dynamic Voltage scaling management */
    BASIC_vSetVoltageScaling(BSC_VOLTAGE_SCALE);

#ifdef EXT_CRYSTAL_OSC
    /* HSE clock on */
    RCC_CR |= RCC_CR_HSEON_MASK;

    /* Wait for HSERDY = 1 (HSE is ready) */
    while ((RCC_CR & RCC_CR_HSERDY_MASK) == 0U)
    {

    }
#endif  //if not EXT_CRYSTAL_OSC clock continue from HSI

    /* Setup latency for flash and wait to apply new value */
    BASIC_vSetLatency(BSC_LATENCY);

    // Setup HPRE and APB buses
    BASIC_vSetDividers();

    /* Setup PLL */
    BASIC_vSetPLL();

    /* enable clock for ports */
    RCC_AHB4ENR |= (uint32_t)(
        RCC_AHB4ENR_GPIOAEN_MASK | RCC_AHB4ENR_GPIOBEN_MASK | RCC_AHB4ENR_GPIOCEN_MASK |
        RCC_AHB4ENR_GPIODEN_MASK | RCC_AHB4ENR_GPIOEEN_MASK | RCC_AHB4ENR_GPIOFEN_MASK |
        RCC_AHB4ENR_GPIOGEN_MASK | RCC_AHB4ENR_GPIOHEN_MASK | RCC_AHB4ENR_GPIOIEN_MASK |
        RCC_AHB4ENR_GPIOJEN_MASK | RCC_AHB4ENR_GPIOKEN_MASK);

/* Setting for measurement on MCO output, if you want to measure comment #if 0 and #endif to make source code active */
#if 0
    RCC_CFGR &= ((~RCC_CFGR_MCO1SEL_MASK) & (~RCC_CFGR_MCO1PRE_MASK) & (~RCC_CFGR_MCO2SEL_MASK) & (~RCC_CFGR_MCO2PRE_MASK));
    RCC_CFGR |= (uint32_t)(
        SHIFT_BIT(10u, RCC_CFGR_MCO1PRE_POS) | // MCO1 prescaler set to 10
        SHIFT_BIT(10u, RCC_CFGR_MCO2PRE_POS) | // MCO2 prescaler set to 10
        SHIFT_BIT( 3u, RCC_CFGR_MCO1SEL_POS) | // MCO1 set to PLL - 011: PLL1 clock selected (pll1_q_ck)
        SHIFT_BIT( 0u, RCC_CFGR_MCO2SEL_POS)); // MCO2 set to PLL - 000: System clock selected (sys_ck)

    RCC_AHB4ENR |= RCC_AHB4ENR_GPIOAEN_MASK | RCC_AHB4ENR_GPIOCEN_MASK;
    GPIOA_OSPEEDR_OSPEED8 = 3u;  // Very High Speed
    GPIOC_OSPEEDR_OSPEED9 = 3u;  // Very High Speed
    GPIOA_AFRH_AFSEL8     = 0u;  // AF0 - MCO1
    GPIOC_AFRH_AFSEL9     = 0u;  // AF0 - MCO2
    GPIOA_MODER_MODE8     = 2u;  // ALTFUNC mode
    GPIOC_MODER_MODE9     = 2u;  // ALTFUNC mode
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    FSF_vBoot();
#endif
}

void BASIC_vConfigPlatform(void)
{
    /* Clock Configuration*/
    BASIC_vInitPlatform();

    struct MPU_RegionConfig mpuInitConfig = {0};
    
    /* Disables the MPU */
    BASIC_vDisableMpu();

    mpuInitConfig.rgnNumber      = MPU_RGN_NUMBER0;
    mpuInitConfig.rgnBaseAddress = 0x00u;
    mpuInitConfig.rgnEnable      = MPU_RGN_ENABLE;
    mpuInitConfig.rgnSize        = MPU_RGN_SIZE_4GB;  
    mpuInitConfig.subRgnDisable  = 0x87u;
    mpuInitConfig.executeNever   = MPU_INSTRUCTION_EXEC_NOT_PERMITED;
    mpuInitConfig.accPermission  = MPU_AP_NO_ACCESS;
    mpuInitConfig.typeExtension  = MPU_RGN_TEX0;
    mpuInitConfig.isBufferable   = MPU_RGN_NOT_BUFFERABLE;
    mpuInitConfig.isCacheable    = MPU_RGN_NOT_CACHEABLE;
    mpuInitConfig.isShareable    = MPU_RGN_SHAREABLE;  

    BASIC_vCofigureMpuRegion(&mpuInitConfig);
    /* Enables the MPU */
    BASIC_vEnableMpu(MPU_PRIVDEF_ENABLE);

#ifdef APP_VARIANT
#ifdef ENABLE_OPTION_BYTES_CONFIG
    /* Option Bytes Configuration*/
    struct OB_Config obConfig;
    
    if(BASIC_bIsObConfigChange(&obConfig))
    {
        /* Ensure no OPTCHANGEERR flag is set */
        if((FLASH_OPTSR_CUR & FLASH_OPTSR_CUR_OPTCHANGEERR_MASK) == 0U)
        {
            BASIC_vConfigureOb(&obConfig);
        }
    } 
#endif// ENABLE_OPTION_BYTES_CONFIG

    /* Here should be placed all application specific initialization */
    RCC_D2CCIP1R_SPI123SRC = 0u; // 000: pll1_q_ck clock selected as SPI/I2S1,2 and 3 kernel clock
    RCC_D3CCIPR_ADCSRC = 1u;     //  01: pll3_r_ck clock selected for ADC

    /* Enable ICACHE - inline function from CMSIS */
    #if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT >= 1u)
    SCB_EnableICache();         //Invalidate and then enable the instruction cache
    #endif
    /* Enable DCACHE - inline function from CMSIS */
    #if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT >= 1u)
    SCB_EnableDCache();         //Invalidate and then enable the data cache
    #endif

    /* Set Priority of configurable interrupts */
    // setDefaultInterruptPriority();

    // HADC0_vInit(0); // not implemented yet for this platform

    /* all other initializations */
    // ...
#endif
}

/* Funtion sets the RCC_PLLCFGR */
static void BASIC_vSetPLL()
{
    /* Disable the main PLL to set registers */
    RCC_CR &= ~((uint32_t)RCC_CR_PLL1ON_MASK | RCC_CR_PLL3ON_MASK);
    /* wait until PLL is off */
    while((RCC_CR & (RCC_CR_PLL1RDY_MASK | RCC_CR_PLL3RDY_MASK)) != 0u) { }

    // Sets fraction for PLL1 and PLL3
    RCC_PLLCFGR = 0u;   // bits PLLxVCOSEL and PLLxFRACEN have to be 0 before PLLx setting
    RCC_PLL1FRACR = SHIFT_BIT(BSC_PLL1_FRACT, RCC_PLL1FRACR_FRACN1_POS);
    RCC_PLL3FRACR = SHIFT_BIT(BSC_PLL3_FRACT, RCC_PLL3FRACR_FRACN3_POS);
    RCC_PLLCFGR |= (uint32_t)RCC_PLLCFGR_PLL1FRACEN_MASK | RCC_PLLCFGR_PLL3FRACEN_MASK;
    // Sets ranges for PLL1
    RCC_PLLCFGR |= (uint32_t)(
        SHIFT_BIT(              BSC_VCOSEL, RCC_PLLCFGR_PLL1VCOSEL_POS) |
        SHIFT_BIT(              BSC_VCOSEL, RCC_PLLCFGR_PLL3VCOSEL_POS) |
        SHIFT_BIT(BSC_PLL_INPUT_FREQ_RANGE, RCC_PLLCFGR_PLL1RGE_POS)    |
        SHIFT_BIT(BSC_PLL_INPUT_FREQ_RANGE, RCC_PLLCFGR_PLL3RGE_POS)    |
        SHIFT_BIT(                      1u, RCC_PLLCFGR_DIVP1EN_POS)    |   // SYSCLK
        SHIFT_BIT(                      1u, RCC_PLLCFGR_DIVQ1EN_POS)    |   // SPI123
        SHIFT_BIT(                      1u, RCC_PLLCFGR_DIVR3EN_POS));      // ADC clock

    // Sets source for all PLL and input DIVM1 divider for PLL1 and DIVM3 divider for PLL3
    RCC_PLLCKSELR = (uint32_t)(
        SHIFT_BIT(BSC_PLL_SRC, RCC_PLLCKSELR_PLLSRC_POS) |
        SHIFT_BIT(BSC_PLLM   , RCC_PLLCKSELR_DIVM1_POS)  |
        SHIFT_BIT(BSC_PLLM   , RCC_PLLCKSELR_DIVM3_POS));

    // Sets Multiplication factor (DIVN) and division factors (DIVP, DIVQ and DIVR) for PLL1
    RCC_PLL1DIVR = (uint32_t)(
        SHIFT_BIT((BSC_PLLN_OUT_DIV - 1u), RCC_PLL1DIVR_DIVN1_POS)   |
        SHIFT_BIT(                    0u , RCC_PLL1DIVR_DIVP1_POS)   |   // 0000000: pll1_p_ck = vco1_ck
        SHIFT_BIT(                    3u , RCC_PLL1DIVR_DIVQ1_POS)   |   // 0000011: pll1_q_ck = vco1_ck / 4
        SHIFT_BIT(                    3u , RCC_PLL1DIVR_DIVR1_POS));     // 0000011: pll1_r_ck = vco1_ck / 4
    RCC_PLL3DIVR = (uint32_t)(
        SHIFT_BIT((BSC_PLLN_OUT_DIV - 1u), RCC_PLL3DIVR_DIVN3_POS)   |
        SHIFT_BIT((BSC_PLL3R_DIV    - 1u), RCC_PLL3DIVR_DIVR3_POS));

    // PLL turn on
    RCC_CR |= (uint32_t)RCC_CR_PLL1ON_MASK | RCC_CR_PLL3ON_MASK;
    while(0u == (RCC_CR & ((uint32_t)RCC_CR_PLL1RDY_MASK | RCC_CR_PLL3RDY_MASK))) { }

    // Set PLL as clock source
    RCC_CFGR |= BSC_RCC_CFGR_SW_PLL;
    while(BSC_RCC_CFGR_SWS_PLL != (RCC_CFGR & BSC_RCC_CFGR_SWS_PLL)) { }
}

static void BASIC_vSetVoltageScaling(const uint32_t voltageScale)
{
    PWR_CR3 &= ~PWR_CR3_SDEN_MASK;  // 0: SMPS step-down converter disabled
    PWR_D3CR = voltageScale;        // Set Voltage scaling selection
    // wait to Voltage is ready
    while(0u == (PWR_D3CR & PWR_D3CR_VOSRDY_MASK)) { }
    while((PWR_CSR1 & PWR_CSR1_ACTVOS_MASK) != voltageScale) { }
    while(0u == (PWR_CSR1 & PWR_CSR1_ACTVOSRDY_MASK)) { }
}

static void BASIC_vSetDividers(void)
{
    // Sets HPRE, D1CPRE and APB3 dividers
    RCC_D1CFGR = (uint32_t)(
        BSC_HPRE_DIV   |
        BSC_D1PPRE_DIV |
        SHIFT_BIT(0u, RCC_D1CFGR_D1CPRE_POS)); // 0xxx: sys_ck not divided
    while(RCC_D1CFGR != (BSC_HPRE_DIV   |      // checking that the new prescaler value has been taken into account
                         BSC_D1PPRE_DIV |
                         SHIFT_BIT(0u, RCC_D1CFGR_D1CPRE_POS))) { }

    // Sets APB1 and APB2 dividers
    RCC_D2CFGR = (uint32_t)(BSC_D2PPRE1_DIV | BSC_D2PPRE2_DIV); // set PCLOCK1 and PCLOCK2 (APB1 and APB2)
    while(RCC_D2CFGR != (BSC_D2PPRE1_DIV | BSC_D2PPRE2_DIV)) { }// checking that the new prescaler value has been taken into account

    // Sets APB4 divider
    RCC_D3CFGR = (uint32_t)(BSC_D3PPRE_DIV);
    while(RCC_D3CFGR != BSC_D3PPRE_DIV) { }     // checking that the new prescaler value has been taken into account
}

static void BASIC_vSetLatency(uint8_t latency)
{
    uint8_t programmingDelay;
    latency &= FLASH_ACR_LATENCY_MASK;
    programmingDelay = (latency > 3u) ? (uint8_t)FLASH_ACR_WRHIGHFREQ_MASK : (uint8_t)((SHIFT_BIT(latency, FLASH_ACR_WRHIGHFREQ_POS)) & FLASH_ACR_WRHIGHFREQ_MASK);
    FLASH_ACR = (uint32_t)latency | programmingDelay;
    while ((FLASH_ACR & (FLASH_ACR_LATENCY_MASK | FLASH_ACR_WRHIGHFREQ_MASK)) != ((uint32_t)latency | programmingDelay)) { }
}

static void BASIC_vEnableMpu(uint32_t mpuCtrl)
{
    /* Enable the MPU */
    MPU_CTRL = mpuCtrl | MPU_CTRL_ENABLE_MASK;

    /* Enable fault exceptions */
    SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA_MASK;

    /* Ensure MPU setting take effects */
    __DSB();
    __ISB();
}

static void BASIC_vDisableMpu(void)
{
    /* Make sure outstanding transfers are done */
    __DMB();

    /* Disable fault exceptions */
    SCB_SHCSR &= ~SCB_SHCSR_MEMFAULTENA_MASK;

    /* Disable the MPU and clear the control register*/
    MPU_CTRL = 0u;
}

static void BASIC_vCofigureMpuRegion(struct MPU_RegionConfig const* config)
{
    MPU_RNR = config->rgnNumber;

    if ((config->rgnEnable) != 0UL)
    {
        MPU_RBAR = config->rgnBaseAddress;
        MPU_RASR =  ((uint32_t)config->executeNever     << MPU_RASR_XN_POS)   |
                    ((uint32_t)config->accPermission    << MPU_RASR_AP_POS)   |
                    ((uint32_t)config->typeExtension    << MPU_RASR_TEX_POS)  |
                    ((uint32_t)config->isShareable      << MPU_RASR_S_POS)    |
                    ((uint32_t)config->isCacheable      << MPU_RASR_C_POS)    |
                    ((uint32_t)config->isBufferable     << MPU_RASR_B_POS)    |
                    ((uint32_t)config->subRgnDisable    << MPU_RASR_SRD_POS)  |
                    ((uint32_t)config->rgnSize          << MPU_RASR_SIZE_POS) |
                    ((uint32_t)config->rgnEnable        << MPU_RASR_ENABLE_POS);
    }
    else
    {
        MPU_RBAR = 0u;
        MPU_RASR = 0u;
    }
}

#if defined(APP_VARIANT) && defined(ENABLE_OPTION_BYTES_CONFIG)
static void BASIC_vUnlockFlashConfig(void)
{
    if((FLASH_CR1 & FLASH_CR1_LOCK1_MASK) != 0U)
    {
        FLASH_KEYR1 = FLASH_KEYR1_KEY_1;
        FLASH_KEYR1 = FLASH_KEYR1_KEY_2;
    }
}

static void BASIC_vLockFlashConfig(void)
{
    FLASH_CR1 |= FLASH_CR1_LOCK1_MASK;
}

static void BASIC_vUnlockObConfig(void)
{
    if((FLASH_OPTCR & FLASH_OPTCR_OPTLOCK_MASK) != 0U)
    {
        FLASH_OPTKEYR = FLASH_OPTKEYR_KEY_1;
        FLASH_OPTKEYR = FLASH_OPTKEYR_KEY_2;
    }
}

static void BASIC_vLockObConfig(void)
{
    FLASH_OPTCR |= FLASH_OPTCR_OPTLOCK_MASK;
}

static void BASIC_vConfigureOb(struct OB_Config const* obConfig)
{
    BASIC_vUnlockFlashConfig();
    BASIC_vUnlockObConfig();

    FLASH_OPTSR_PRG  = obConfig->optsrPrgReg;
    FLASH_OPTSR2_PRG = obConfig->optsr2PrgReg;
    FLASH_BOOT_PRGR  = obConfig->bootPrgReg;
    FLASH_PRAR_PRG1  = obConfig->prarPrgReg;
    FLASH_SCAR_PRG1  = obConfig->scarPrgReg;
    FLASH_WPSN_PRG1R = obConfig->wpsnPrgReg;

    /* Launch Option Bytes configuration */
    FLASH_OPTCR |= FLASH_OPTCR_OPTSTART_MASK;

    /* Wait for Option Bytes change completion */
    while((FLASH_OPTSR_CUR & FLASH_OPTSR_CUR_OPT_BUSY_MASK) != 0U){}

    BASIC_vLockObConfig();
    BASIC_vLockFlashConfig();
}

static void BASIC_vPrepareObValues(struct OB_Config *obConfig)
{
    /* Ensuring only option bytes change */
    obConfig->optsrPrgReg = FLASH_OPTSR_PRG_VALUE & OB_CHANGE_FLASH_OPTSR_CUR_MASK;
    obConfig->optsr2PrgReg = FLASH_OPTSR2_PRG_VALUE & OB_CHANGE_FLASH_OPTSR2_CUR_MASK;
    obConfig->bootPrgReg = FLASH_BOOT_PRGR_VALUE & OB_CHANGE_FLASH_BOOT_CURR_MASK;
    obConfig->prarPrgReg = FLASH_PRAR_PRG1_VALUE & OB_CHANGE_FLASH_PRAR_CUR1_MASK;
    obConfig->scarPrgReg = FLASH_SCAR_PRG1_VALUE & OB_CHANGE_FLASH_SCAR_CUR1_MASK;
    obConfig->wpsnPrgReg = FLASH_WPSN_PRG1R_VALUE & OB_CHANGE_FLASH_WPSN_CUR1R_MASK;

    /* Preserving reserved bits values */
    obConfig->optsrPrgReg |= (FLASH_OPTSR_CUR & (~OB_CHANGE_FLASH_OPTSR_CUR_MASK));
    obConfig->optsr2PrgReg |= (FLASH_OPTSR2_CUR & (~OB_CHANGE_FLASH_OPTSR2_CUR_MASK));
    obConfig->bootPrgReg |=  (FLASH_BOOT_CURR & (~OB_CHANGE_FLASH_BOOT_CURR_MASK));
    obConfig->prarPrgReg |= (FLASH_PRAR_CUR1 & (~OB_CHANGE_FLASH_PRAR_CUR1_MASK));
    obConfig->scarPrgReg |= (FLASH_SCAR_CUR1 & (~OB_CHANGE_FLASH_SCAR_CUR1_MASK));
    obConfig->wpsnPrgReg |= (FLASH_WPSN_CUR1R & (~OB_CHANGE_FLASH_WPSN_CUR1R_MASK));

}

static bool BASIC_bIsObConfigChange(struct OB_Config *obConfig)
{
    BASIC_vPrepareObValues(obConfig);
    if(FLASH_OPTSR_CUR  != obConfig->optsrPrgReg){return true;};
    if(FLASH_OPTSR2_CUR != obConfig->optsr2PrgReg){return true;};
    if(FLASH_BOOT_CURR  != obConfig->bootPrgReg){return true;};
    if(FLASH_PRAR_CUR1  != obConfig->prarPrgReg){return true;};
    if(FLASH_SCAR_CUR1  != obConfig->scarPrgReg){return true;};
    if(FLASH_WPSN_CUR1R != obConfig->wpsnPrgReg){return true;};
    return false;
}
#endif// APP_VARIANT && ENABLE_OPTION_BYTES_CONFIG
//lint -restore -e923 -e750 -e845
