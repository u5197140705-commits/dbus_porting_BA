/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
*   PROJECT          MCU Framework
*   MODULE-PREFIX    BASIC
*   AUTHOR           Jing Liu
*   CREATED          10.07.2023
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file   basicMcXs.c
*
*   \brief  implementation of micro-controller initialization and configuration.
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "basic.h"
#include "processor.h"
#ifdef MCAL_MPCM_INCLUDED
    #include "mcal/mpcm.h"
    #include "mcal/mcal_assert.h"
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
#include "fSafe.h"
#endif

/*******************************************************************************
*-------------------------------------------------------------------------------
*        System Clock source                    | PLL (HXTAL/IRC8M/IRC48M)
*-------------------------------------------------------------------------------
*        SYSCLK(MHz)                            | max 120 MHz
*-------------------------------------------------------------------------------
*        AHB(MHz)                               | max 120 MHz
*-------------------------------------------------------------------------------
*        APB1, APB2(MHz)                        | max 60 MHz, max 120 MHz
*-------------------------------------------------------------------------------
*        AHB Prescaler                          | 2^0 - 2^9 range
*-------------------------------------------------------------------------------
*        APB1 Prescaler                         | 2^0 - 2^4 range
*-------------------------------------------------------------------------------
*        APB2 Prescaler                         | 2^0 - 2^4 range
*-------------------------------------------------------------------------------
*        HXTAL Frequency(MHz)                   | 4-32 MHz
*-------------------------------------------------------------------------------
*        PLLMUL                                 | auto
*-------------------------------------------------------------------------------
*        PREDIV                                 | auto
*-------------------------------------------------------------------------------
*        Flash Latency(WS)                      | 0 (zero wait state even with max core clock)
*-------------------------------------------------------------------------------
*        Prefetch Buffer                        | ON
*-------------------------------------------------------------------------------
*******************************************************************************/


/* Minor issues in 3rd party code
 * Info 835: zero given as right argument to << in a constant expression
 * Info 845: the left operand to << always evaluates to 0
 */
/*lint -save -e835 -e845 */
/*lint -emacro(923,RCU_*,GPIO_*,AFIO_*,GPIOx_*) "explicit cast from uint32_t to volatile uint32_t *" tested to work correctly   */

/*
    Keep not referenced macro, same as vendor

    Info 750: local macro 'RCU_CFG0_PREDV0_POS' not referenced [MISRA 2012 Rule 2.5, advisory]
    Info 750: local macro 'RCU_CFG0_AHBPSC_SYSCLK' not referenced [MISRA 2012 Rule 2.5, advisory]
*/
//lint -save -e750

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#define RCU_CFG0_PLLMF_POS     18U     // Position of PLLMF[3:0] multiply factor in CFG0 register
#define RCU_CFG0_PLLMF4_POS    27U     // Position of PLLMF4 multiply factor in CFG0 register
#define RCU_CFG0_PLLMF5_POS    30U     // Position of PLLMF5 multiply factor in CFG0 register
#define RCU_CFG0_PREDV0_POS    17U     // Position of PREDV0 division factor in CFG0 register

#define MAX_SYSCLK     120U    // max PLL output clock frequency
#define MIN_SYSCLK     2U      // min PLL output clock frequency
#define MAX_PCLOCK1    60U     // max APB1 peripheral clock frequency
#define MAX_PCLOCK2    120U    // max APB2 peripheral clock frequency

//lint -esym( 750, HSI8) "message: symbol not used..."
#define HSI8             8U

/* define SRC_CLK based on HSI or HSE */
#ifdef EXT_CRYSTAL_OSC
    #define SRC_CLK   EXT_CRYSTAL_OSC
#else
    #define SRC_CLK   HSI8
#endif

/* PLL_div calculation */
#if ((4U <= SRC_CLK) && (SRC_CLK <= 32U))
    #if ((SRC_CLK % 2U) == 0U)                      //modulo, remainder after division
        #define PLLdiv                      1U      // 1U -> PREDV0 = 2U
    #elif  ((SRC_CLK % 1U) == 0U)
        #define PLLdiv                      0U      // 0U -> PREDV0 = 1U
    #else
        #error "No valid EXT_CRYSTAL_OSC value defined."
    #endif
#else
    #error "EXT_CRYSTAL_OSC out of boundaries (4-32MHz)."
#endif

/* HPRE calculation */
#if ((CORE_CLOCK < MIN_SYSCLK) && (CORE_CLOCK <= MAX_SYSCLK))
    #if ((MIN_SYSCLK <= (CORE_CLOCK * 2U)) && ((CORE_CLOCK * 2U) <= MAX_SYSCLK))
        #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV2    /*!< SYSCLK divided by 2   */
        #define SYSCLK                    (CORE_CLOCK * 2U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 4U)) && ((CORE_CLOCK * 4U) <= MAX_SYSCLK))
        #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV4    /*!< SYSCLK divided by 4   */
        #define SYSCLK                  (CORE_CLOCK * 4U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 8U)) && ((CORE_CLOCK * 8U) <= MAX_SYSCLK))
        #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV8    /*!< SYSCLK divided by 8   */
        #define SYSCLK                  (CORE_CLOCK * 8U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 16U)) && ((CORE_CLOCK * 16U) <= MAX_SYSCLK))
        #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV16   /*!< SYSCLK divided by 16  */
        #define SYSCLK                  (CORE_CLOCK * 16U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 64U)) && ((CORE_CLOCK * 64U) <= MAX_SYSCLK))
        #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV64   /*!< SYSCLK divided by 64  */
        #define SYSCLK                  (CORE_CLOCK * 64U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 128U)) && ((CORE_CLOCK * 128U) <= MAX_SYSCLK))
        #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV128  /*!< SYSCLK divided by 128 */
        #define SYSCLK                  (CORE_CLOCK * 128U)
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 256U)) && ((CORE_CLOCK * 256U) <= MAX_SYSCLK))
        #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV256
        #define SYSCLK   (CORE_CLOCK * 256U)                         /*!< SYSCLK divided by 256 */
    #elif ((MIN_SYSCLK <= (CORE_CLOCK * 512U)) && ((CORE_CLOCK * 512U) <= MAX_SYSCLK))
        #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV512
        #define SYSCLK                  (CORE_CLOCK * 512U)          /*!< SYSCLK divided by 512 */
    #else
        #error "No valid CORE_CLOCK defined"
    #endif
#elif ((MIN_SYSCLK <= CORE_CLOCK)  && (CORE_CLOCK <= MAX_SYSCLK))
    #define RCU_CFG0_AHBPSC_SYSCLK    RCU_AHB_CKSYS_DIV1
    #define SYSCLK                  CORE_CLOCK
#else
    #error "CORE_CLOCK out of boundaries"
#endif

#define PLLmul    (SYSCLK / (SRC_CLK / ( PLLdiv+1U )))


/* PCLOCK1 (APB1) calculation */
#ifndef PCLOCK1
    #if (CORE_CLOCK < MAX_PCLOCK1)
        #define PCLOCK1    CORE_CLOCK
    #else
        #define PCLOCK1    MAX_PCLOCK1
    #endif
#endif

#define DIV1 (CORE_CLOCK / PCLOCK1)
#if ( (CORE_CLOCK % PCLOCK1) == 0U ) && ( PCLOCK1 <= MAX_PCLOCK1 )
    #if DIV1 == 1U
        #define RCU_APB1_PSC       RCU_APB1_CKAHB_DIV1
    #elif DIV1 == 2U
        #define RCU_APB1_PSC       RCU_APB1_CKAHB_DIV2
    #elif DIV1 == 4U
        #define RCU_APB1_PSC       RCU_APB1_CKAHB_DIV4
    #elif DIV1 == 8U
        #define RCU_APB1_PSC       RCU_APB1_CKAHB_DIV8
    #elif DIV1 == 16U
        #define RCU_APB1_PSC       RCU_APB1_CKAHB_DIV16
    #else
        #error "No valid value of peripheral clock defined. PCLOCK1 must be correctly defined."
    #endif
#else
    #error "No valid peripheral clock defined. PCLOCK1 must be correctly defined."
#endif

/* PCLOCK2 (APB2) calculation */
#ifndef PCLOCK2
    #if (CORE_CLOCK < MAX_PCLOCK2)
        #define PCLOCK2    CORE_CLOCK
    #else
        #define PCLOCK2    MAX_PCLOCK2
    #endif
#endif

#define DIV2 (CORE_CLOCK / PCLOCK2)
#if ( (CORE_CLOCK % PCLOCK2) == 0U ) && ( PCLOCK2 <= MAX_PCLOCK2 )
    #if DIV2 == 1U
        #define RCU_APB2_PSC       RCU_APB2_CKAHB_DIV1
    #elif DIV2 == 2U
        #define RCU_APB2_PSC       RCU_APB2_CKAHB_DIV2
    #elif DIV2 == 4U
        #define RCU_APB2_PSC       RCU_APB2_CKAHB_DIV4
    #elif DIV2 == 8U
        #define RCU_APB2_PSC       RCU_APB2_CKAHB_DIV8
    #elif DIV2 == 16U
        #define RCU_APB2_PSC       RCU_APB2_CKAHB_DIV16
    #else
        #error "No valid value of peripheral clock defined. PCLOCK2 must be correctly defined."
    #endif
#else
    #error "No valid peripheral clock defined. PCLOCK2 must be correctly defined."
#endif


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

#if !defined(MCAL_MPCM_INCLUDED)

/** \brief Sets the PLL */
static void BASIC_vSetPLL(uint32_t divFactorPLL, uint32_t mulFactorPLL);

/** \brief Initializes clock based on make configuration */
static void BASIC_vInitClock(void);

#endif /* !defined(MCAL_MPCM_INCLUDED) */


#if defined(MCAL_MPCM_INCLUDED)
// Axivion Next Line MisraC2012-8.9: Global variable can be declared inside function.
static const struct MPCM_PowerModesConfig customPowerModes =
{
    /* Check available configurations for your platform in mcpm_mc.h */
    &MPCM_CFG_DEFAULT,              // Take configuration from make process
    &MPCM_CFG_CORE_8_PCLK_8,        // Lowered clock speeds for low-power run
    NULL,                           // No special configuration for sleep mode
    NULL                            // No special configuration for stop mode
};
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS AND PRIVATE MACRO FUNCTION DEFINITIONS   v    */
/******************************************************************************/
// Axivion Next Line MisraC2012-8.7: Routine can be declared static in primary file
void BASIC_vInitPlatform(void)
{
    /* Set the clocks to default state as like after the hardware reset */

  #if defined(MCAL_MPCM_INCLUDED)
    if(MPCM_init(&customPowerModes) != MCAL_OK)
    {
        MCAL_error("Invalid MPCM configuration");
    }
  #else
    BASIC_vInitClock();
  #endif
    
    /* Enable clock of GPIO AF */
    RCU_APB2EN |= (uint32_t)RCU_APB2EN_AFEN;

#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    FSF_vBoot();
#endif
}

/******************************************************************************/
/* static void BASIC_vCheckVddMonitor(void)                                   */
/******************************************************************************/

#if !defined(MCAL_MPCM_INCLUDED)
static void BASIC_vInitClock(void)
{
    /* enable IRC8M */
    RCU_CTL |= RCU_CTL_IRC8MEN;
    while ((RCU_CTL & (uint32_t)RCU_CTL_IRC8MSTB) == 0U){}     // Wait for IRC8M is stable
    RcuModify(0x50);
    /* set IRC8M as a main clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    while ((RCU_CFG0 & RCU_CFG0_SCSS) != RCU_SCSS_IRC8M){}    // wait until IRC8M switched as clock source

    /* reset CTL registers */
#if (defined(GD32F30X_HD) || defined(GD32F30X_XD))
    /* reset HXTALEN, CKMEN and PLLEN bits */
    RCU_CTL &= ~(RCU_CTL_HXTALEN | RCU_CTL_CKMEN | RCU_CTL_PLLEN);
#elif defined(GD32F30X_CL)
    /* Reset HXTALEN, CKMEN, PLLEN, PLL1EN and PLL2EN bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN |RCU_CTL_PLL1EN | RCU_CTL_PLL2EN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN);
#endif

    /* reset HXTALBPS bit (HXTALBPS bit can be written only if the HXTALEN is 0) */
    RCU_CTL &= ~(RCU_CTL_HXTALBPS);

#ifdef EXT_CRYSTAL_OSC
    /* HSE clock on */
    RCU_CTL |= RCU_CTL_HXTALEN;
    while ((RCU_CTL & RCU_CTL_HXTALSTB) == 0U) {} // Wait until external crystal is stable
#endif
    
    BASIC_vSetPLL((uint32_t)PLLdiv, (uint32_t)PLLmul);    // set PLL

/* Feature to measure CORE_CLOCK on CK_OUT pin (PA8). Put the define below to the variant make file to activate this feature */
//defines += MEASUREMENT_ON=1
#if defined(MEASUREMENT_ON)
#define GPIO_CLT1_CTL8_AF_PP    0x02U
#define GPIO_CTL1_CTL8_POS      2U
    RCU_CFG0 |= RCU_CKOUT0SRC_CKSYS;                                      // system clock to CK_OUT pin: PA8
    RCU_APB2EN |= (uint32_t)RCU_APB2EN_PAEN;                              // Enable clock to port A
    RCU_APB2EN |= (uint32_t)RCU_APB2EN_AFEN;                              // Enable clock to alternate function
    GPIO_CTL1(GPIOA) &=  ~(GPIO_CTL1_MD8 | GPIO_CTL1_CTL8);               // clear mode setting for PA8
    GPIO_CTL1(GPIOA) |=  (GPIO_OSPEED_50MHZ | (GPIO_CLT1_CTL8_AF_PP << GPIO_CTL1_CTL8_POS));  // Enable alternate mode on PA8
    AFIO_CPSCTL |= AFIO_CPSCTL_CPS_EN;                //enable speed compensation, needed when IO output is faster then 50Mhz
    GPIOx_SPD(GPIOA) |= GPIO_SPD_SPD8;                //set high output speed for PA8
#endif // defined(MEASUREMENT_ON)
}


static void BASIC_vSetPLL(uint32_t divFactorPLL, uint32_t mulFactorPLL)
{
    uint32_t mulFactorPLL_aligned;
    
    if (mulFactorPLL < 16U)                  /* GD32F303 PLL Clock multiplier parameter is not linear, see RCU_CFG0_PLLMF ref. man. description */
    {
        mulFactorPLL = mulFactorPLL - 2U;   /* clock multiplier decreased by 2 for mulFactorPLL < 16 */
    }
    else
    {
        mulFactorPLL = mulFactorPLL - 1U;      /* clock multiplier decreased by 1 for mulFactorPLL >= 16 */
    }
    
    RCU_CTL &= ~RCU_CTL_PLLEN; // switch PLL off to access registers
    while((RCU_CTL & RCU_CTL_PLLSTB) != 0U){}    // wait until PLL is turned off
    
    /* reset RCU_CFG registers */
    RCU_CFG0 &= ~(RCU_CFG0_SCS | RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC | \
                 RCU_CFG0_ADCPSC | RCU_CFG0_ADCPSC_2 | RCU_CFG0_CKOUT0SEL | \
				 RCU_CFG0_PREDV0 | RCU_CFG0_USBDPSC | RCU_CFG0_USBDPSC_2 );
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4 | RCU_CFG0_PLLMF_5);
    RCU_CFG1 &= ~(RCU_CFG1_ADCPSC_3 | RCU_CFG1_PLLPRESEL);

    /* shift PLL multiply factor to proper PLLMF register position */
    mulFactorPLL_aligned = mulFactorPLL << RCU_CFG0_PLLMF_POS;
    RCU_CFG0 |= mulFactorPLL_aligned & RCU_CFG0_PLLMF;                              //set PLLMF[3:0]
    RCU_CFG0 |= ((mulFactorPLL >> 4) << RCU_CFG0_PLLMF4_POS) & RCU_CFG0_PLLMF_4;    //set PLLMF[4]
    RCU_CFG0 |= ((mulFactorPLL >> 5) << RCU_CFG0_PLLMF5_POS) & RCU_CFG0_PLLMF_5;    //set PLLMF[5]

#ifdef EXT_CRYSTAL_OSC
    RCU_CFG0 |= ((uint32_t)RCU_PLLSRC_HXTAL_IRC48M |           /* HXTAL selected as PLL input clock */
                RCU_CFG0_AHBPSC_SYSCLK |                       /* AHB (HCLK) prescaler */
                RCU_APB1_PSC   |                           /* APB1 (PCLK1) prescaler */
                RCU_APB2_PSC   );                          /* APB2 (PCLK2) prescaler */
                
    RCU_CFG0 |= (uint32_t)divFactorPLL << RCU_CFG0_PREDV0_POS;            /* Set HXTAL input clock divider */

#else //if not EXT_CRYSTAL_OSC
    (void) divFactorPLL;
    /* IRC8M is already set as PLL input clock (by default) */
    RCU_CFG0 |= ((uint32_t)RCU_PLLSRC_IRC8M_DIV2 |            /* AHB (HCLK) prescaler */
                RCU_APB1_PSC   |                           /* APB1 (PCLK1) prescaler */
                RCU_APB2_PSC   );                          /* APB2 (PCLK2) prescaler */
#endif

    RCU_CTL |= RCU_CTL_PLLEN;   // switch PLL on
    while((RCU_CTL & RCU_CTL_PLLSTB) == 0U){}  // wait until PLL ready

    /* now set the PLL as system clock, prescalers are 1 */
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;                          // select PLL as system clock
    while ((RCU_CFG0 & RCU_CFG0_SCSS) != RCU_SCSS_PLL){}  // wait until PLL stabilized as clock source
}

#endif /* !defined(MCAL_MPCM_INCLUDED) */

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void BASIC_vConfigPlatform (void)
{
    /* Clock Configuration*/
    BASIC_vInitPlatform();
    

#ifdef APP_VARIANT
    /* Here should be placed all application specific initialization */
    
    /* Set Priority of configurable interrupts */
    // setDefaultInterruptPriority();
    
    // HADC_vInit(0);

    /* all other initializations */
    // ...
#endif
}

