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
 *  COMP_ABBREV      MPX
 ******************************************************************************/

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
#include <stdint.h>
#include "basic.h"
#include "processor.h"
#ifdef APP_VARIANT
#include "hadc.h"
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fs_psm.h"
#endif

#if defined(MCAL_INCLUDED)
#include "mcal/mcal_includes.h"
#include "mcal/mcal_assert.h"
#endif

/*****************************************************************************
*-----------------------------------------------------------------------------
*        System Clock source                    | PLL (HSI-HSE)
*-----------------------------------------------------------------------------
*        SYSCLK(MHz)                            | 64 MHz max
*-----------------------------------------------------------------------------
*        HCLK(MHz)                              | 64 MHz max
*-----------------------------------------------------------------------------
*        AHB Prescaler                          | range 2^0 - 2^8
*-----------------------------------------------------------------------------
*        APB Prescaler                          | range 2^0 - 2^4
*-----------------------------------------------------------------------------
*        HSI Frequency(MHz)                     | 16
*-----------------------------------------------------------------------------
*        PLLMUL                                 | auto
*-----------------------------------------------------------------------------
*        PREDIV                                 | auto
*-----------------------------------------------------------------------------
*        Flash Latency(WS)                      | auto
*-----------------------------------------------------------------------------
*        Prefetch Buffer                        | ON
*-----------------------------------------------------------------------------
*****************************************************************************/
/* Switch off: info 845: the right operand to << always evaluates to 0 */
/*lint -save -e845 */

/** \brief shifts a bitfield (value) to the desired position (shift) */
#define SHFT_BIT(value,shift) ((uint32_t)(((uint32_t)(value) << ((uint32_t)(shift))))) //lint !e9026 macro needed to be resolved at compile time

//lint -esym( 750, HSE_SRC) "message: symbol mot used..."
#define HSI16_SRC         2u   ///< HSI16 used as clock source (needed for different registers)
#define HSE_SRC           3u   ///< HSE used as clock source


#if !defined(MCAL_MPCM_INCLUDED)
#define RCC_CFGR_SW_PLL   2u   ///< PLL used as system clock

#define  HSI16      16U    // 16MHz High Speed Internal Oscillator

#define  MAX_SYSCLK   64U    // max PLL output frequency
#define  MIN_SYSCLK    8U    // min PLL output frequency
#define  MAX_PCLOCK   64U

//lint -esym( 750, SYSTEM_RCC_CFGR_*) "message: symbol mot used..."
/** These defines determines divider of the SYSCLK and output clock is called the HCLK frequency that
supplies APB1/2 (PCLK1/2). The default macro is SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV1 and now there is no
need to control this prescaler from the build process. */
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV1      0x0U
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV2      0x8U
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV4      0x9U
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV8      0xAU
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV16     0xBU
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV64     0xCU
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV128    0xDU
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV256    0xEU
#define SYSTEM_RCC_CFGR_HPRE_SYSCLK_DIV512    0xFU

/** These defines determines divider of the HCLK for the low speed bus APB1 (frequency PCLK1). Maximum frequency
of the bus can be 64 MHZ. The corresponding macro is chosen according to user defined define PCLOCK in the build process. */
#define SYSTEM_RCC_CFGR_PPRE_HCLK_DIV1        0x0U
#define SYSTEM_RCC_CFGR_PPRE_HCLK_DIV2        0x4U
#define SYSTEM_RCC_CFGR_PPRE_HCLK_DIV4        0x5U
#define SYSTEM_RCC_CFGR_PPRE_HCLK_DIV8        0x6U
#define SYSTEM_RCC_CFGR_PPRE_HCLK_DIV16       0x7U

/* PLL_M calculation */
#ifdef EXT_CRYSTAL_OSC
    #define SRC_CLK   EXT_CRYSTAL_OSC
#else
    #define SRC_CLK   HSI16
#endif

#if ((4U <= SRC_CLK) && (SRC_CLK <= 48U))
    /* 4 MHz PLL input preferred, so it's checked first */
    #if (((SRC_CLK % 4U) == 0U) && (((SRC_CLK / 4U) - 1U) <= 7U))
        #define PLLM   ((SRC_CLK / 4U) - 1U)    // -1U -> PLLM_1 = 0U
    #elif  (((SRC_CLK % 7U) == 0U) && (((SRC_CLK / 7U) - 1U) <= 7U))
        #define PLLM   ((SRC_CLK / 7U) - 1U)
    #elif  (((SRC_CLK % 6U) == 0U) && (((SRC_CLK / 6U) - 1U) <= 7U))
        #define PLLM   ((SRC_CLK / 6U) - 1U)
    #elif  (((SRC_CLK % 5U) == 0U) && (((SRC_CLK / 5U) - 1U) <= 7U))
        #define PLLM   ((SRC_CLK / 5U) - 1U)
    #elif  (((SRC_CLK % 3U) == 0U) && (((SRC_CLK / 3U) - 1U) <= 7U))
        #define PLLM   ((SRC_CLK / 3U) - 1U)
    #elif  (((SRC_CLK % 2U) == 0U) && (((SRC_CLK / 2U) - 1U) <= 7U))
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
    #error "CORE_CLOCK out of boundaries."
#endif

/* PLLR calculation */
#if ((64U <= (SYSCLK * 2U)) && ((SYSCLK * 2U) <= 344U))
    #define PLLR    1U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 2  
#elif ((64U <= (SYSCLK * 3U)) && ((SYSCLK * 3U) <= 344U))
    #define PLLR    2U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 3
#elif ((64U <= (SYSCLK * 4U)) && ((SYSCLK * 4U) <= 344U))
    #define PLLR    3U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 4
#elif ((64U <= (SYSCLK * 5U)) && ((SYSCLK * 5U) <= 344U))
    #define PLLR    4U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 5
#elif ((64U <= (SYSCLK * 6U)) && ((SYSCLK * 6U) <= 344U))
    #define PLLR    5U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 6
#elif ((64U <= (SYSCLK * 7U)) && ((SYSCLK * 7U) <= 344U))
    #define PLLR    6U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 7
#elif ((64U <= (SYSCLK * 8U)) && ((SYSCLK * 8U) <= 344U))
    #define PLLR    7U                                      ///< PLL "R" output clock frequency = VCO frequency / PLLR with PLLR = 8
#else
    #error "Not able to calculate PLL_R value."
#endif

#define PLLN   ((SYSCLK * (PLLR+1U)) / (SRC_CLK / (PLLM+1U)))

/* FLASH_ACR_LATENCY calculation */
#if(CORE_CLOCK <= 24U)
    #define FLASH_ACR_LATENCY_VALUE   0u    // 0 wait states
#elif(CORE_CLOCK <= 48U)
    #define FLASH_ACR_LATENCY_VALUE   1u    // 1 wait states
#else
    #define FLASH_ACR_LATENCY_VALUE   2u    // 2 wait states
#endif

/* PCLOCK calculation */
#ifndef PCLOCK
    #define PCLOCK    CORE_CLOCK
#endif

#define DIV    (CORE_CLOCK / PCLOCK)
#if ( (CORE_CLOCK % PCLOCK) == 0U ) && ( PCLOCK <= MAX_PCLOCK )
    /* BSC_SYSTEM_RCC_CFGR_PRE1_HCLK define*/
    #if DIV == 1U
        #define SYSTEM_RCC_CFGR_PPRE_HCLK       SYSTEM_RCC_CFGR_PPRE_HCLK_DIV1
    #elif DIV == 2U
        #define SYSTEM_RCC_CFGR_PPRE_HCLK       SYSTEM_RCC_CFGR_PPRE_HCLK_DIV2
    #elif DIV == 4U
        #define SYSTEM_RCC_CFGR_PPRE_HCLK       SYSTEM_RCC_CFGR_PPRE_HCLK_DIV4
    #elif DIV == 8U
        #define SYSTEM_RCC_CFGR_PPRE_HCLK       SYSTEM_RCC_CFGR_PPRE_HCLK_DIV8
    #elif DIV == 16U
        #define SYSTEM_RCC_CFGR_PPRE_HCLK       SYSTEM_RCC_CFGR_PPRE_HCLK_DIV16
    #else
        #error "No valid peripheral clock defined. PCLOCK must be correctly defined."
    #endif
#else
    #error "No valid peripheral clock defined. PCLOCK must be correctly defined."
#endif



/** \brief Sets the PLL */
static void BASIC_vSetPLL(uint32_t divFactorPLLM, uint32_t mulFactorPLLN, uint32_t divFactorPLLR);

/** \brief Initializes clock based on make configuration */
static void BASIC_vInitClock(void);

#endif /* !defined(MCAL_MPCM_INCLUDED) */

/*
const THWDT_Config HWDT_atConfig[1] = {
    {HWDT_COUNTER_CLOCK_DIV_8,HWDT_WINDOW_MAX_VALUE,0x7E}
};
*/

void BASIC_vInitPlatform(void)
{
    /* Disable the internal Pull-Down resistor in Dead Battery pins of UCPD peripheral */
  #if defined(SYSCFG_CFGR1_UCPD1_STROBE_MASK) && defined(SYSCFG_CFGR1_UCPD2_STROBE_MASK)
    SYSCFG_CFGR1 |= SYSCFG_CFGR1_UCPD1_STROBE_MASK | SYSCFG_CFGR1_UCPD2_STROBE_MASK;
  #endif

  #if defined(MCAL_MPCM_INCLUDED)
    const struct MPCM_PowerModesConfig customPowerModes =
    {
        /* Check available configurations for your platform in mcpm_mc.h */
        &MPCM_CFG_DEFAULT,              // Take configuration from make process
        &MPCM_CFG_CORE_8_PCLK_8,        // Lowered clock speeds for low-power run
        NULL,                           // No special configuration for sleep mode
        NULL                            // No special configuration for stop mode
    };
    
    if(MPCM_init(&customPowerModes) != MCAL_OK)
    {
        MCAL_error("Invalid MPCM configuration");
    }
  #else
    BASIC_vInitClock();
  #endif
    
    /* switch on clock in sleep/stop mode for GPIOs */
    RCC_IOPSMENR = RCC_IOPSMENR_GPIOASMEN_MASK | RCC_IOPSMENR_GPIOBSMEN_MASK | RCC_IOPSMENR_GPIOCSMEN_MASK |
                   RCC_IOPSMENR_GPIODSMEN_MASK | RCC_IOPSMENR_GPIOFSMEN_MASK;
    RCC_APBENR2  = RCC_APBENR2_SYSCFGEN_MASK; // enable clock for System configuration controller
    RCC_APBENR1  = RCC_APBENR1_DBGEN_MASK;     // enable clock for debug module

    /* select HSI16 as peripheral clock as for as possible */
    RCC_CCIPR = (uint32_t)(
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_USART1SEL_POS)  |   /* select HSI16 as UART1 clock */ //lint !e835 better readability even when shift is zero
#if defined(RCC_CCIPR_USART2SEL_POS)
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_USART2SEL_POS)  |   /* select HSI16 as UART2 clock */
#endif
#if defined(RCC_CCIPR_LPUART1SEL_POS)
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_LPUART1SEL_POS) |   /* select HSI16 as LPUART1 clock */
#endif
#if defined(RCC_CCIPR_LPUART2SEL_POS)
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_LPUART2SEL_POS) |   /* select HSI16 as LPUART2 clock */
#endif
#if defined(RCC_CCIPR_I2C2SEL_POS)
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_I2C2SEL_POS)    |   /* select HSI16 as I2C2 clock */
#endif
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_I2C1SEL_POS)    |   /* select HSI16 as I2C1 clock */
#if defined(RCC_CCIPR_LPTIM1SEL_POS)
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_LPTIM1SEL_POS)  |   /* select HSI16 as LPTIM1 clock */
#endif
#if defined(RCC_CCIPR_LPTIM2SEL_POS)
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_LPTIM2SEL_POS)  |   /* select HSI16 as LPTIM2 clock */
#endif
#if defined(RCC_CCIPR_RNGSEL_POS)
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_RNGSEL_POS)     |   /* select HSI16 as RNG clock */
#endif
        SHFT_BIT(HSI16_SRC,  RCC_CCIPR_ADCSEL_POS)         /* select HSI16 as ADC clock */
        );

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
    RCC_IOPENR = (uint32_t)(RCC_IOPENR_GPIOAEN_MASK | RCC_IOPENR_GPIOBEN_MASK | RCC_IOPENR_GPIOCEN_MASK | RCC_IOPENR_GPIODEN_MASK | RCC_IOPENR_GPIOFEN_MASK);

#ifdef APP_VARIANT
    /* Here should be placed all application specific initialization */

    /* Set Priority of configurable interrupts */
    // setDefaultInterruptPriority();

    // HADC_vInit(0);

    /* all other initializations */
    // ...
#endif
}


#if !defined(MCAL_MPCM_INCLUDED)
static void BASIC_vInitClock(void)
{
    FLASH_ACR_LATENCY = 2u; // set max wait states for safety
    RCC_CR = RCC_CR_HSION_MASK; // enable HSI16, divider and external clock off
#if defined(STM32G030) || defined(STM32G050) || defined(STM32G070) || defined(STM32G0B0)
    /* enable prefetch buffer instruction cache */
    FLASH_ACR |= FLASH_ACR_PRFTEN_MASK | FLASH_ACR_ICEN_MASK;
#else
    /* for STM32G0X1 */
    /* enable prefetch buffer instruction cache and debugger */
    FLASH_ACR |= FLASH_ACR_PRFTEN_MASK | FLASH_ACR_ICEN_MASK | FLASH_ACR_DBG_SWEN_MASK;
#endif

#ifdef EXT_CRYSTAL_OSC
    /* HSE clock on */
    RCC_CR |= RCC_CR_HSEON_MASK;

    // Wait for HSERDY = 1 (HSE is ready)
    while ((RCC_CR & RCC_CR_HSERDY_MASK) == 0U)
    {

    }
#endif
    
    BASIC_vSetPLL(PLLM, PLLN, PLLR);    // set PLL
    /* set Flash Latency value */
    FLASH_ACR_LATENCY = FLASH_ACR_LATENCY_VALUE;
}


static void BASIC_vSetPLL(uint32_t divFactorPLLM, uint32_t mulFactorPLLN, uint32_t divFactorPLLR)
{
    RCC_CFGR = 0; // set HSISYS (reset status, but maybe switched on by BootManager)
    // wait until HSISYS stabilized as clock source 
    while (RCC_CFGR_SWS != 0u)
    {
    
    }

    RCC_CR_PLLON = 0u; // switch PLL off to set registers

    // wait until PLL is turned off
    while( RCC_CR_PLLRDY != 0u )
    {

    }
#ifdef EXT_CRYSTAL_OSC
RCC_PLLCFGR = (uint32_t)(
        SHFT_BIT(HSE_SRC,         RCC_PLLCFGR_PLLSRC_POS)|   /* select internal 16MHz clock            */  //lint !e835 better readability even when shift is zero
        SHFT_BIT(divFactorPLLM,   RCC_PLLCFGR_PLLM_POS)  |   /* select the PLL divider as specified    */
        SHFT_BIT(mulFactorPLLN,   RCC_PLLCFGR_PLLN_POS)  |   /* select the PLL multiplier as specified */
        SHFT_BIT(2u,              RCC_PLLCFGR_PLLP_POS)  |   /* select the P-Clock divider             */
        SHFT_BIT(2u,              RCC_PLLCFGR_PLLQ_POS)  |   /* select the Q-Clock divider             */
        SHFT_BIT(divFactorPLLR,   RCC_PLLCFGR_PLLR_POS)  |   /* select the R-Clock divider             */
        SHFT_BIT(1u,              RCC_PLLCFGR_PLLREN_POS)    /* enable the RCLOCK to be used as system clock */
    );
#else
    RCC_PLLCFGR = (uint32_t)(
        SHFT_BIT(HSI16_SRC,       RCC_PLLCFGR_PLLSRC_POS)|   /* select internal 16MHz clock            */  //lint !e835 better readability even when shift is zero
        SHFT_BIT(divFactorPLLM,   RCC_PLLCFGR_PLLM_POS)  |   /* select the PLL divider as specified    */
        SHFT_BIT(mulFactorPLLN,   RCC_PLLCFGR_PLLN_POS)  |   /* select the PLL multiplier as specified */
        SHFT_BIT(2u,              RCC_PLLCFGR_PLLP_POS)  |   /* select the P-Clock divider             */
        SHFT_BIT(2u,              RCC_PLLCFGR_PLLQ_POS)  |   /* select the Q-Clock divider             */
        SHFT_BIT(divFactorPLLR,   RCC_PLLCFGR_PLLR_POS)  |   /* select the R-Clock divider             */
        SHFT_BIT(1u,              RCC_PLLCFGR_PLLREN_POS)    /* enable the RCLOCK to be used as system clock */
    );
#endif
    RCC_CR_PLLON = 1u; // switch PLL on
    while( RCC_CR_PLLRDY == 0u ){}  // wait until PLL ready

    /* now set the PLL as system clock, prescalers are 1 */
    RCC_CFGR =
        SHFT_BIT(RCC_CFGR_SW_PLL,  RCC_CFGR_SW_POS) |                  /* select PLL as system clock */
        SHFT_BIT(SYSTEM_RCC_CFGR_HPRE_SYSCLK,  RCC_CFGR_HPRE_POS) |    /* set APB prescaler for APB peripherals to 1 */
        SHFT_BIT(SYSTEM_RCC_CFGR_PPRE_HCLK,  RCC_CFGR_PPRE_POS);       /* set AHB prescaler for core clock  to 1 */
     while (RCC_CFGR_SWS != RCC_CFGR_SW_PLL){}  // wait until PLL stabilized as clock source

}

#endif /* !defined(MCAL_MPCM_INCLUDED) */


/*lint -restore -e845 */
