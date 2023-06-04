/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
*   PROJECT          PP PED Framework
*   MODULE-PREFIX    BASIC
*   AUTHOR           Jens Lehmann
*   CREATED          31.08.2017
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
#include "SegmentDef.h"
#include "hsup.h"
#ifdef APP_VARIANT
//#include "hadc.h" // for Stm32g4 not implemented yet
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
#include "fSafe.h"
#endif

#if defined(MCAL_MPCM_INCLUDED)
#include "mcal/mcal_includes.h"
#include "mcal/mcal_assert.h"
#endif

/*****************************************************************************
*-----------------------------------------------------------------------------
*        System Clock source                    | HSI
*-----------------------------------------------------------------------------
*        SYSCLK(MHz)                            | max 170
*-----------------------------------------------------------------------------
*        HCLK(MHz)                              | max 170
*-----------------------------------------------------------------------------
*        AHB Prescaler                          | 2^0 - 2^8
*-----------------------------------------------------------------------------
*        APB1 Prescaler                         | 2^0 - 2^4
*-----------------------------------------------------------------------------
*        APB2 Prescaler                         | 2^0 - 2^4
*-----------------------------------------------------------------------------
*        PLL_M                                  | auto
*-----------------------------------------------------------------------------
*        PLL_N                                  | auto
*-----------------------------------------------------------------------------
*        PLL_P                                  | 2
*-----------------------------------------------------------------------------
*        PLL_Q                                  | 2
*-----------------------------------------------------------------------------
*        PLL_R                                  | auto
*-----------------------------------------------------------------------------
*        PLLSAI1_P                              | NA
*-----------------------------------------------------------------------------
*        PLLSAI1_Q                              | NA
*-----------------------------------------------------------------------------
*        PLLSAI1_R                              | NA
*-----------------------------------------------------------------------------
*        PLLSAI2_P                              | NA
*-----------------------------------------------------------------------------
*        PLLSAI2_Q                              | NA
*-----------------------------------------------------------------------------
*        PLLSAI2_R                              | NA
*-----------------------------------------------------------------------------
*        Require 48MHz for USB OTG FS,          | Disabled
*        SDIO and RNG clock                     |
*-----------------------------------------------------------------------------
*****************************************************************************/

#if !defined(MCAL_MPCM_INCLUDED)
//lint -esym( 750, HSI16*) "message: symbol mot used..."
#define  HSI16      16U    // 16MHz High Speed Internal Oscilator  
  
//lint -esym( 750, HCLK_DIV*) "message: symbol mot used..."
#define  HCLK_DIV1  0x0U
#define  HCLK_DIV2  0x4U
#define  HCLK_DIV4  0x5U
#define  HCLK_DIV8  0x6U
#define  HCLK_DIV16 0x7U

//lint -esym( 750, SYSCLK_DIV*) "message: symbol mot used..."
#define  SYSCLK_DIV1    0x0U
#define  SYSCLK_DIV2    0x8U
#define  SYSCLK_DIV4    0x9U
#define  SYSCLK_DIV8    0xAU
#define  SYSCLK_DIV16   0xBU
#define  SYSCLK_DIV64   0xCU
#define  SYSCLK_DIV128  0xDU
#define  SYSCLK_DIV256  0xEU
#define  SYSCLK_DIV512  0xFU

//lint -esym( 750, FLASH_*) "message: symbol mot used..."
#define  FLASH_ACR_LATENCY_8WS              ((uint32_t)0x00000008)

//lint -esym( 750, SYSTEM_RCC_CFGR_*) "message: symbol mot used..."
/** These defines determines divider of the SYSCLK and output clock is called the HCLK frequency that
supplies APB1/2 (PCLK1/2). The default macro is SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV1 and now there is no
need to control this prescaler from the build process. */
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV1      (SYSCLK_DIV1) // shift not done left argument is zero   << RCC_CFGR_HPRE_POS)
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV2      ((uint32_t)SYSCLK_DIV2   << (uint32_t)RCC_CFGR_HPRE_POS)
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV4      ((uint32_t)SYSCLK_DIV4   << (uint32_t)RCC_CFGR_HPRE_POS)
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV8      ((uint32_t)SYSCLK_DIV8   << (uint32_t)RCC_CFGR_HPRE_POS)
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV16     ((uint32_t)SYSCLK_DIV16  << (uint32_t)RCC_CFGR_HPRE_POS)
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV64     ((uint32_t)SYSCLK_DIV64  << (uint32_t)RCC_CFGR_HPRE_POS)
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV128    ((uint32_t)SYSCLK_DIV128 << (uint32_t)RCC_CFGR_HPRE_POS)
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV256    ((uint32_t)SYSCLK_DIV256 << (uint32_t)RCC_CFGR_HPRE_POS)
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV512    ((uint32_t)SYSCLK_DIV512 << (uint32_t)RCC_CFGR_HPRE_POS)

/** These defines determines divider of the HCLK for the low speed bus APB1 (frequency PCLK1). Maximum frequency
of the bus can be 36 MHZ. The coresponding macro is chosen according to user defined define PCLOCK in the build process. */
#define SYSTEM_RCC_CFGR_PRE1_HCLK_DIV1        (HCLK_DIV1) // shift not done left argument is zero
#define SYSTEM_RCC_CFGR_PRE1_HCLK_DIV2        ((uint32_t)HCLK_DIV2  << (uint32_t)RCC_CFGR_PPRE1_POS)
#define SYSTEM_RCC_CFGR_PRE1_HCLK_DIV4        ((uint32_t)HCLK_DIV4  << (uint32_t)RCC_CFGR_PPRE1_POS)
#define SYSTEM_RCC_CFGR_PRE1_HCLK_DIV8        ((uint32_t)HCLK_DIV8  << (uint32_t)RCC_CFGR_PPRE1_POS)
#define SYSTEM_RCC_CFGR_PRE1_HCLK_DIV16       ((uint32_t)HCLK_DIV16 << (uint32_t)RCC_CFGR_PPRE1_POS)

/** These defines determines divider of the HCLK for the high speed bus APB2 (frequency PCLK1)
The coresponding macro is chosen according to user defined define PCLOCK1 in the build process. */
#define SYSTEM_RCC_CFGR_PRE2_HCLK_DIV1        (HCLK_DIV1) // shift not done left argument is zero
#define SYSTEM_RCC_CFGR_PRE2_HCLK_DIV2        ((uint32_t)HCLK_DIV2  << (uint32_t)RCC_CFGR_PPRE2_POS)
#define SYSTEM_RCC_CFGR_PRE2_HCLK_DIV4        ((uint32_t)HCLK_DIV4  << (uint32_t)RCC_CFGR_PPRE2_POS)
#define SYSTEM_RCC_CFGR_PRE2_HCLK_DIV8        ((uint32_t)HCLK_DIV8  << (uint32_t)RCC_CFGR_PPRE2_POS)
#define SYSTEM_RCC_CFGR_PRE2_HCLK_DIV16       ((uint32_t)HCLK_DIV16 << (uint32_t)RCC_CFGR_PPRE2_POS)

#define  MIN_SYSCLK   8U
#define  MAX_SYSCLK   170U
#define  MAX_PCLOCK   170U

/* PLL_M calculation */
#ifdef EXT_CRYSTAL_OSC
    #define SRC_CLK   EXT_CRYSTAL_OSC
#else
    #define SRC_CLK   HSI16
#endif

#if ((4U <= SRC_CLK) && (SRC_CLK <= 48U))
    /* 4 MHz PLL input preferred, so it's checked first */
    #if (((SRC_CLK % 4U) == 0U) && ((SRC_CLK / (SRC_CLK / 4U)) <= 8U) && ((SRC_CLK / (SRC_CLK / 4U)) >= 3U))
        #define PLLM   ((SRC_CLK / 4U) - 1U)    // -1U -> PLLM_1 = 0U
    #elif  (((SRC_CLK % 7U) == 0U) && ((SRC_CLK / (SRC_CLK / 7U)) <= 8U) && ((SRC_CLK / (SRC_CLK / 7U)) >= 3U))
        #define PLLM   ((SRC_CLK / 7U) - 1U)
    #elif  (((SRC_CLK % 6U) == 0U) && ((SRC_CLK / (SRC_CLK / 6U)) <= 8U) && ((SRC_CLK / (SRC_CLK / 6U)) >= 3U))
        #define PLLM   ((SRC_CLK / 6U) - 1U)
    #elif  (((SRC_CLK % 5U) == 0U) && ((SRC_CLK / (SRC_CLK / 5U)) <= 8U) && ((SRC_CLK / (SRC_CLK / 5U)) >= 3U))
        #define PLLM   ((SRC_CLK / 5U) - 1U)
    #elif  (((SRC_CLK % 3U) == 0U) && ((SRC_CLK / (SRC_CLK / 3U)) <= 8U) && ((SRC_CLK / (SRC_CLK / 3U)) >= 3U))
        #define PLLM   ((SRC_CLK / 3U) - 1U)
    #elif  (((SRC_CLK % 2U) == 0U) && ((SRC_CLK / (SRC_CLK / 2U)) <= 8U) && ((SRC_CLK / (SRC_CLK / 2U)) >= 3U))
        #define PLLM   ((SRC_CLK / 2U) - 1U)
    #else
        #error "No valid EXT_CRYSTAL_OSC value defined."
    #endif
#else
    #error "EXT_CRYSTAL_OSC out of boundaries (4-48MHz)."
#endif

/* HPRE calculation */
#if ((CORE_CLOCK < MIN_SYSCLK) && (CORE_CLOCK <= MAX_SYSCLK))
    #if ((MIN_SYSCLK <= (CORE_CLOCK * 2U)) && ((CORE_CLOCK * 2U) <= MAX_SYSCLK))
        #define SYSTEM_RCC_CFGR_HPRE_SYSCLK    SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV2
        #define SYSCLK                     (CORE_CLOCK * 2U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 4U)) && ((CORE_CLOCK * 4U) <= MAX_SYSCLK))
        #define SYSTEM_RCC_CFGR_HPRE_SYSCLK   SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV4
        #define SYSCLK   (CORE_CLOCK * 4U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 8U)) && ((CORE_CLOCK * 8U) <= MAX_SYSCLK))
        #define SYSTEM_RCC_CFGR_HPRE_SYSCLK   SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV8
        #define SYSCLK   (CORE_CLOCK * 8U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 16U)) && ((CORE_CLOCK * 16U) <= MAX_SYSCLK))
        #define SYSTEM_RCC_CFGR_HPRE_SYSCLK   SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV16
        #define SYSCLK   (CORE_CLOCK * 16U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 64U)) && ((CORE_CLOCK * 64U) <= MAX_SYSCLK))
        #define SYSTEM_RCC_CFGR_HPRE_SYSCLK   SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV64
        #define SYSCLK   (CORE_CLOCK * 64U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 128U)) && ((CORE_CLOCK * 128U) <= MAX_SYSCLK))
        #define SYSTEM_RCC_CFGR_HPRE_SYSCLK   SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV128
        #define SYSCLK   (CORE_CLOCK * 128U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 256U)) && ((CORE_CLOCK * 256U) <= MAX_SYSCLK))
        #define SYSTEM_RCC_CFGR_HPRE_SYSCLK   SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV256
        #define SYSCLK   (CORE_CLOCK * 256U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 512U)) && ((CORE_CLOCK * 512U) <= MAX_SYSCLK))
        #define SYSTEM_RCC_CFGR_HPRE_SYSCLK   SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV512
        #define SYSCLK   (CORE_CLOCK * 512U)
    #else
        #error "No valid CORE_CLOCK defined."
    #endif
#elif ((MIN_SYSCLK <= CORE_CLOCK)  && (CORE_CLOCK <= MAX_SYSCLK))
    #define SYSTEM_RCC_CFGR_HPRE_SYSCLK   SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV1
    #define SYSCLK                        CORE_CLOCK 
#else
    #error "CORE_CLOCK out of boudaries."
#endif

/* PLLR calculation */
#if ((64U <= (SYSCLK * 2U)) && ((SYSCLK * 2U) <= 344U))
    #define PLLR    0U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 2
    #define PLL_R   2U                                      // needed for calculation
#elif ((64U <= (SYSCLK * 4U)) && ((SYSCLK * 4U) <= 344U))
    #define PLLR    1U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 4
    #define PLL_R   4U                                      // needed for calculation
#elif ((64U <= (SYSCLK * 6U)) && ((SYSCLK * 6U) <= 344U))
    #define PLLR    2U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 6
    #define PLL_R   6U                                      // needed for calculation
#elif ((64U <= (SYSCLK * 8U)) && ((SYSCLK * 8U) <= 344U))
    #define PLLR    3U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 8
    #define PLL_R   8U                                      // needed for calculation
#else
    #error "Not able to calculate PLL_R value."
#endif

#define PLLN   ((SYSCLK * PLL_R) / (SRC_CLK / (PLLM+1U)))

/* FLASH_ACR_LATENCY calculation */
#if(CORE_CLOCK <= 150u)
    #if((CORE_CLOCK % 30u) == 0u)
        #define FLASH_ACR_LATENCY_VALUE ((uint8_t) (CORE_CLOCK / 30u)) - 1u
    #else
        #define FLASH_ACR_LATENCY_VALUE ((uint8_t) (CORE_CLOCK / 30u))
    #endif
#else
    #define FLASH_ACR_LATENCY_VALUE     (4u)
#endif


//lint -esym( 750, BSC_SYSTEM_RCC_CFGR_PRE1_HCLK) "message: symbol mot used..."
#define DIV1    (CORE_CLOCK / PCLOCK)
#if ( (CORE_CLOCK % PCLOCK) == 0U ) && ( PCLOCK <= MAX_PCLOCK )
    /* BSC_SYSTEM_RCC_CFGR_PRE1_HCLK define*/
    #if DIV1 == 1U
        #define SYSTEM_RCC_CFGR_PRE1_HCLK       SYSTEM_RCC_CFGR_PRE1_HCLK_DIV1
    #elif DIV1 == 2U
        #define SYSTEM_RCC_CFGR_PRE1_HCLK       SYSTEM_RCC_CFGR_PRE1_HCLK_DIV2
    #elif DIV1 == 4U
        #define SYSTEM_RCC_CFGR_PRE1_HCLK       SYSTEM_RCC_CFGR_PRE1_HCLK_DIV4
    #elif DIV1 == 8U
        #define SYSTEM_RCC_CFGR_PRE1_HCLK       SYSTEM_RCC_CFGR_PRE1_HCLK_DIV8
    #elif DIV1 == 16U
        #define SYSTEM_RCC_CFGR_PRE1_HCLK       SYSTEM_RCC_CFGR_PRE1_HCLK_DIV16
    #else
        #error "No valid peripheral clock defined. PCLOCK must be correctly defined."
    #endif
#else
    #error "No valid peripheral clock defined. PCLOCK must be correctly defined."
#endif


//lint -esym( 750, BSC_SYSTEM_RCC_CFGR_PRE2_HCLK) "message: symbol mot used..."
#define DIV2    (CORE_CLOCK / PCLOCK1)
#if ( (CORE_CLOCK % PCLOCK1) == 0U ) && ( PCLOCK1 <= MAX_PCLOCK )
    /* BSC_SYSTEM_RCC_CFGR_PRE2_HCLK define*/
    #if DIV2 == 1U
        #define BSC_SYSTEM_RCC_CFGR_PRE2_HCLK       SYSTEM_RCC_CFGR_PRE2_HCLK_DIV1
    #elif DIV2 == 2U
        #define BSC_SYSTEM_RCC_CFGR_PRE2_HCLK       SYSTEM_RCC_CFGR_PRE2_HCLK_DIV2
    #elif DIV2 == 4U
        #define BSC_SYSTEM_RCC_CFGR_PRE2_HCLK       SYSTEM_RCC_CFGR_PRE2_HCLK_DIV4
    #elif DIV2 == 8U
        #define BSC_SYSTEM_RCC_CFGR_PRE2_HCLK       SYSTEM_RCC_CFGR_PRE2_HCLK_DIV8
    #elif DIV2 == 16U
        #define BSC_SYSTEM_RCC_CFGR_PRE2_HCLK       SYSTEM_RCC_CFGR_PRE2_HCLK_DIV16
    #else
        #error "No valid peripheral clock defined. PCLOCK1 must be correctly defined."
    #endif
#else
    #error "No valid peripheral clock defined. PCLOCK1 must be correctly defined."
#endif


/** \brief shifts a bitfield (value) to the desired position (shift) */
#define SHIFT_BIT(value,shift) ((uint32_t)(((uint32_t)(value) << ((uint32_t)(shift))))) //lint !e9026 macro needed to be resolved at compile time

//lint -esym( 750, HSE_SRC) "message: symbol mot used..."
#define RCC_CFGR_SW_PLL 0x00000003UL        /*!< PLL selected as system clock */
#define HSI16_SRC         2u                ///< HSI16 clock selected as PLL clock entry
#define HSE_SRC RCC_PLLSYSCFGR_PLLSRC_MASK  ///< HSE clock selected as PLL clock entry

#define BSC_RCC_CFGR_SWS_HSI    0x4u        ///< HSI16 used as system clock
#define BSC_RCC_CFGR_SW_HSI     0x1u        ///< HSI16 selected as system clock

/** \brief Sets the PLL */
static void BASIC_vSetPLL(uint32_t divFactorPLLM, uint32_t mulFactorPLLN, uint32_t divFactorPLLR);

#endif /* !defined(MCAL_MPCM_INCLUDED) */

void BASIC_vInitPlatform(void)
{
    /* Disable the internal Pull-Down resistor in Dead Battery pins of UCPD peripheral */
    RCC_APB1LENR |= RCC_APB1LENR_PWREN_MASK;
    PWRCTRL_CR3 |= PWRCTRL_CR3_DBDIS_MASK;

#if defined(MCAL_MPCM_INCLUDED)
    const struct MPCM_PowerModesConfig customPowerModes =
    {
        /* Check available configurations for your platform in mcpm_mc.h */
        &MPCM_CFG_DEFAULT,              // Take configuration from make process
        &MPCM_CFG_CORE_16_PCLK_16,        // Lowered clock speeds for low-power run
        NULL,                           // No special configuration for sleep mode
        NULL                            // No special configuration for stop mode
    };

    if(MPCM_init(&customPowerModes) != MCAL_OK)
    {
        MCAL_error("Invalid MPCM configuration");
    }
    
#else
    /* Set the clocks to default state as like after the hardware reset */
    FLASH_ACR |= FLASH_ACR_LATENCY_8WS;
    // HSI16 clock on
    RCC_CR |= RCC_CR_HSION_MASK;
    /* Wait for HSIRDY = 1 (HSI16 is ready) */
    while ((RCC_CR & RCC_CR_HSIRDY_MASK) == 0u)
    {

    }

    // HSI16 selected as system clock
    RCC_CFGR_SW = BSC_RCC_CFGR_SW_HSI;
    // Wait till HSI16 is used as system clock source
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != BSC_RCC_CFGR_SWS_HSI)
    {

    }

    // RCC_CR register set to the reset state (clear all bits except for HSION bit)
    RCC_CR &= (RCC_CR_HSION_MASK);
    // Wait till HSE and PLL are disabled
    while ((RCC_CR & (RCC_CR_HSERDY_MASK | RCC_CR_PLLSYSRDY_MASK)) != 0u)
    {

    }

    /* CFGR register to the reset state (prescalers to default state) */
    RCC_CFGR &= (BSC_RCC_CFGR_SW_HSI);


    /* Clock Configuration */

    /* AHB prescaler SYSCLK divided by 2. Needed to set dynamic voltage scaling management */
    RCC_CFGR |= SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV2 | \
                SYSTEM_RCC_CFGR_PRE1_HCLK        | \
                BSC_SYSTEM_RCC_CFGR_PRE2_HCLK;

#ifdef EXT_CRYSTAL_OSC
    /* HSE clock on */
    RCC_CR |= RCC_CR_HSEON_MASK;

    /* Wait for HSERDY = 1 (HSE is ready) */
    while ((RCC_CR & RCC_CR_HSERDY_MASK) == 0U)
    {

    }
#endif  //if not EXT_CRYSTAL_OSC clock continue from HSI

#if(CORE_CLOCK > 150u)
    RCC_APB1LENR |= RCC_APB1LENR_PWREN_MASK;

    /* System clock test because must be divided by 2 before switching to Range 1 boost mode.
       Not valid for variant romLoader with HSE because memory saving. */
    #if (!defined(BOOTLOADER_VARIANT) && !defined(ROMLOADER_H)) || !defined(EXT_CRYSTAL_OSC)
    while((RCC_CFGR & RCC_CFGR_HPRE_MASK) != SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV2)
    {

    }
    #endif

    /* Set Main regulator in range 1 to boost mode */
    PWRCTRL_CR5  &= ~PWRCTRL_CR5_R1MODE_MASK;
    RCC_APB1LENR &= ~RCC_APB1LENR_PWREN_MASK;
#endif
    
    /* Setup latency for flash and wait to apply new value */
    FLASH_ACR_LATENCY = FLASH_ACR_LATENCY_VALUE;
    while ((FLASH_ACR & FLASH_ACR_LATENCY_MASK) != FLASH_ACR_LATENCY_VALUE)
    {

    }

    /* Setup PLL */
    BASIC_vSetPLL(PLLM, PLLN, PLLR);

    /* Wait for at least 1us and then reconfigure the AHB */
    HSUP_vDelay(2u);
    //lint -e931 "message: both sides have side effects"
    RCC_CFGR = (uint32_t)(RCC_CFGR & (~RCC_CFGR_HPRE_MASK)) | SYSTEM_RCC_CFGR_HPRE_SYSCLK;
    while((RCC_CFGR & RCC_CFGR_HPRE_MASK) != SYSTEM_RCC_CFGR_HPRE_SYSCLK)
    {

    }

    /* enable prefetch buffer instruction cache and debugger */
    FLASH_ACR |= FLASH_ACR_PRFTEN_MASK | FLASH_ACR_ICEN_MASK | FLASH_ACR_DBG_SWEN_MASK;
    
#endif /* defined(MCAL_MPCM_INCLUDED) */

    /* AHB2 peripheral clock enable register */
    /* enable all GPIOs */
    RCC_AHB2ENR |= RCC_AHB2ENR_GPIOAEN_MASK | RCC_AHB2ENR_GPIOBEN_MASK | RCC_AHB2ENR_GPIOCEN_MASK | RCC_AHB2ENR_GPIODEN_MASK | RCC_AHB2ENR_GPIOEEN_MASK;

/* Setting for measurement on MCO output, if you want to measure comment #if 0 and #endif to make source code active */
#if 0
#if CORE_CLOCK==170u
    RCC_CFGR_MCOPRE = 0x1u; // MCO divided by 2 => 170/2=85MHz
#endif
    RCC_CFGR &= ~(RCC_CFGR_MCOSEL_MASK);
    RCC_CFGR |= 0x01000000U; //MCOSEL=SYSCLK
    GPIOA_MODER_MODER8 = 0x2u; //ALTFUNC mode
    GPIOA_OSPEEDR_OSPEEDR8 = 0x03u; //Very High Speed
    GPIOA_AFRH_AFRH8 = 0x0u; //AF0 MCO
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    FSF_vBoot();
#endif
}

void BASIC_vConfigPlatform (void)
{
    /* Clock Configuration*/
    BASIC_vInitPlatform();

#ifdef APP_VARIANT
    /* Here should be placed all application specific initialization */

    /* Set Priority of configurable interrupts */
    // setDefaultInterruptPriority();

    // HADC0_vInit(0); // not implemented yet for this platform

    /* all other initializations */
    // ...
#endif
}

#if !defined(MCAL_MPCM_INCLUDED)
/* Funtion sets the RCC_PLLCFGR */
static void BASIC_vSetPLL(uint32_t divFactorPLLM, uint32_t mulFactorPLLN, uint32_t divFactorPLLR)
{
    
    /* Disable the main PLL to set registers */
    RCC_CR &= ~(RCC_CR_PLLSYSON_MASK);
    /* wait until PLL ready */
    while( RCC_CR_PLLSYSRDY != 0u )
    {
       
    } 
#ifdef EXT_CRYSTAL_OSC
    RCC_PLLSYSCFGR = (uint32_t)(
        SHIFT_BIT(HSE_SRC,       RCC_PLLSYSCFGR_PLLSRC_POS)   |   /* select external MHz clock             */  //lint !e835 better readability even when shift is zero
        SHIFT_BIT(divFactorPLLM, RCC_PLLSYSCFGR_PLLSYSM_POS)  |   /* select the PLL divider as specified   */
        SHIFT_BIT(mulFactorPLLN, RCC_PLLSYSCFGR_PLLSYSN_POS)  |   /* select the PLL multipier as specified */
        SHIFT_BIT(divFactorPLLR, RCC_PLLSYSCFGR_PLLSYSR_POS)  |   /* select the R-Clock divider            */
        SHIFT_BIT(1u,            RCC_PLLSYSCFGR_PLLSYSREN_POS)     /* enable the RCLOCK to be used as system clock */
    );
#else //no external oscillator used, internall HSI16 will be used
    RCC_PLLSYSCFGR = (uint32_t)(
        SHIFT_BIT(HSI16_SRC,     RCC_PLLSYSCFGR_PLLSRC_POS)   |   /* select internal 16MHz clock           */  //lint !e835 better readability even when shift is zero
        SHIFT_BIT(divFactorPLLM, RCC_PLLSYSCFGR_PLLSYSM_POS)  |   /* select the PLL divider as specified   */
        SHIFT_BIT(mulFactorPLLN, RCC_PLLSYSCFGR_PLLSYSN_POS)  |   /* select the PLL multipier as specified */
        SHIFT_BIT(2u,            RCC_PLLSYSCFGR_PLLSYSP_POS)  |   /* select the P-Clock divider            */
        SHIFT_BIT(2u,            RCC_PLLSYSCFGR_PLLSYSQ_POS)  |   /* select the Q-Clock divider            */
        SHIFT_BIT(divFactorPLLR, RCC_PLLSYSCFGR_PLLSYSR_POS)  |   /* select the R-Clock divider            */
        SHIFT_BIT(1u,            RCC_PLLSYSCFGR_PLLSYSREN_POS)     /* enable the RCLOCK to be used as system clock */
    );
#endif    
    RCC_CR |= RCC_CR_PLLSYSON_MASK; // switch PLL on
    /* wait until PLL ready */
    while( RCC_CR_PLLSYSRDY != 1u )
    {

    }

    /* now set the PLL as system clock, prescalers are 1 */
    RCC_CFGR |= SHIFT_BIT(RCC_CFGR_SW_PLL,  RCC_CFGR_SW_POS);   /* select PLL as system clock */
/*   || SHIFT_BIT(0,  RCC_CFGR_HPRE_POS)  */             /* set APB prescaler for APB peripherals to 1 */
/*   || SHIFT_BIT(0,  RCC_CFGR_PPRE_POS)  */             /* set AHB prescaler for core clock  to 1 */
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_MASK){}  // wait until PLL stabilized as clock source

    RCC_AHB2ENR |= RCC_AHB2ENR_GPIOAEN_MASK | RCC_AHB2ENR_GPIOBEN_MASK | RCC_AHB2ENR_GPIOCEN_MASK | RCC_AHB2ENR_GPIODEN_MASK | RCC_AHB2ENR_GPIOEEN_MASK;
}

#endif /* !defined(MCAL_MPCM_INCLUDED) */
