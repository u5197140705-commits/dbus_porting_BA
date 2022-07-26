/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      BASIC
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of basicMcXs.
 *
 *  \details  Model implementation of micro controller/platform initialisation and configuration.
 *            After call BASIC_vConfigPlatform function configures PLL/FLL to match
 *            desired core clock frequency set by CORE_CLOCK definition.
 *            - Clock source is automatically selected from PLL/FLL, based on
 *              which option is achievable (PLL is preffered).
 *            - Core clock frequency is selectable in range based on selected external
 *              oscillator (see "Settings for external oscillator")
 *            - Maximum supported core clock is 240 MHz.
 *            - Overclocking is possible but not recommended.
 *            - This implementation needs requires external oscillator to be present.
 *            - Supported external oscillators are: 10, 16, 20, 24, 25, 32, 48 and 50 MHz.
 *            - To set external oscillator frequency, set ext_osc make variable in
 *              application build file.
 *            - To set core clock frequency, set core_clock make variable in
 *              application build file.
 *            - Clock output to pin can be activated by adding "DEFINES += CLKOUT_ENABLE"
 *              in project configuration,
 */

/**************************************************************************************/
/*   Supported CORE_CLOCK frequencies supplied by PLL, based on external oscilator    */
/*------------------------------------------------------------------------------------*/
/*  EXT OSC    | PLL REF  |   PLL RANGE   |   SUPPORTED CORE CLOCK FREQ WITH PLL      */
/*------------------------------------------------------------------------------------*/
/* 10, 20 MHz  | 10 MHz   | 80  - 235 MHz | 80, 100, 120, 140, 160, 180, 200, 220 MHz */
/* 16, 32 MHz  | 16 MHz   | 128 - 240 MHz | 160, 240 MHz                              */
/* 24, 48 MHz  | 12 MHz   | 96  - 240 MHz | 120, 180, 240 MHz                         */
/* 25, 50 MHz  | 12.5 MHz | 100 - 225 MHz | 100, 150, 200 MHz                         */
/**************************************************************************************/

/**************************************************************************************/
/*   Supported CORE_CLOCK frequencies supplied by FLL, based on external oscilator    */
/*------------------------------------------------------------------------------------*/
/*  EXT OSC    |   FLL REF   |  FLL RANGE   |   SUPPORTED CORE CLOCK FREQ WITH FLL    */
/*------------------------------------------------------------------------------------*/
/* 10, 20 MHz  | 39.0625 kHz | 25 - 100 MHz | 50, 100 MHz                             */
/* 16, 32 MHz  | 31.25 kHz   | 20 - 80 MHz  | 20, 40, 60, 80 MHz                      */
/**************************************************************************************/

/**************************************************************/
/*    Table of individual bus clocks based on CORE_CLOCK      */
/*------------------------------------------------------------*/
/* CORE_CLOCK | Fast Periph clk | FlexBus clk | Bus/Flash clk */
/*------------------------------------------------------------*/
/* 20 MHz     | 20 MHz          | 20 MHz      | 20 MHz        */
/* 40 MHz     | 40 MHz          | 40 MHz      | 20 MHz        */
/* 50 MHz     | 50 MHz          | 50 MHz      | 25 MHz        */
/* 60 MHz     | 60 MHz          | 60 MHz      | 20 MHz        */
/* 80 MHz     | 80 MHz          | 40 MHz      | 20 MHz        */
/* 100 MHz    | 100 MHz         | 50 MHz      | 25 MHz        */
/* 120 MHz    | 120 MHz         | 60 MHz      | 24 MHz        */
/* 140 MHz    | 70 MHz          | 35 MHz      | 20 MHz        */
/* 150 MHz    | 75 MHz          | 50 MHz      | 25 MHz        */
/* 160 MHz    | 80 MHz          | 40 MHz      | 20 MHz        */
/* 180 MHz    | 90 MHz          | 60 MHz      | 22.5 MHz      */
/* 200 MHz    | 100 MHz         | 50 MHz      | 25 MHz        */
/* 220 MHz    | 110 MHz         | 55 MHz      | 25 MHz        */
/* 240 MHz    | 120 MHz         | 60 MHz      | 24 MHz        */
/**************************************************************/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "basic.h"
#include "processor.h"
#include "SegmentDef.h"
#ifdef APP_VARIANT
// #include "hadc.h" // for Mkv5 not existing yet
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
#include "fSafe.h"
#endif

#if defined(MCAL_MPCM_INCLUDED)
#include "mcal/mcal_includes.h"
#include "mcal/mcal_assert.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/


/*lint -esym(750,HIGH_SPEED_RUN*)  local macro not referenced  MISRA 2012 Rule 2.5, advisory]*/
#define HIGH_SPEED_RUN_MODE     (3U)    ///< For PMCTRL register -> controls entry into low-power Run and Stop modes.
#define HIGH_SPEED_RUN_MASK     (0x01U << 7U)   ///< Current power mode is HSRUN

#if !defined(MCAL_MPCM_INCLUDED)

/*lint -esym(750,FLL_*)  local macro not referenced  MISRA 2012 Rule 2.5, advisory]*/

#define MHZ(x)                  ((x) * 1000000UL) ///< Macro for inputing clock frequency in MHz
#define PRDIV(x)                ((x) - 1U)        ///< Adjustment for PRDIV register segment (PLL External Reference Divide Factor)
#define VDIV(x)                 ((x) - 16U)       ///< Adjustment for VDIV register segment (PLL VCO Divide Factor)
#define DRST_DRS(x)             ((((x) / 640U) - 1U) & 0x03U) ///< Adjustment for DRST_DRS register segment (DCO Range Select)

/* Defines of internal reference MUX */
#define IRCS_SLOW               (0U)    ///< Slow IRC selection value - 32.768 kHz
#define IRCS_FAST               (1U)    ///< Fast IRC selection value - 4 MHz

/* Range of external oscillator */
/*lint -esym(750,RANGE_*)  local macro not referenced  MISRA 2012 Rule 2.5, advisory]*/
#define RANGE_LOW               (0U)    ///< External clock range 32 kHz
#define RANGE_HIGH              (1U)    ///< External clock range 8 to 32 MHz
#define RANGE_VERY_HIGH         (2U)    ///< External clock range 32 to 50 MHz

#define CLKS_FLL_PLL            (0U)    ///< Clock source selection of FLL/PLL in MCGOUTCLK

#define PORT_MUX_AF5            (5U)    ///< PORT alternate function selection


/** \brief    Basic core clock setting checks
 *
 *  \details  This section is for checking if CORE_CLOCK is defined and correctly selected
 *            Overclocking of device is possible, but not recommended as it may cause
 *            unexpected behaviour.
 */
#ifndef CORE_CLOCK
    #error "CORE_CLOCK is not defined. Check project settings."
#elif CORE_CLOCK > 240U
    #warning "Selected CORE_CLOCK is out of bounds. Overclocking may cause unexpected behaviour."
#endif


/** \brief    Settings for external oscillator
 *
 *  \details  This section is for checking of external oscillator settings.
 *            External oscillators with frequency 10, 16, 20, 24, 25, 32, 48 and 50 MHz are supported
 */

#if defined(EXT_CRYSTAL_OSC)
    #define EXT_CRYSTAL_OSC_HZ      MHZ(EXT_CRYSTAL_OSC)
#elif !defined(EXT_CRYSTAL_OSC_HZ)
    #error "External oscillator is required."
#endif /* EXT_CRYSTAL_OSC_HZ */


#if (EXT_CRYSTAL_OSC_HZ >= MHZ(32U)) && (EXT_CRYSTAL_OSC_HZ <= MHZ(50U))
    #define EXT_OSC_RANGE       RANGE_VERY_HIGH
#elif EXT_CRYSTAL_OSC_HZ >= MHZ(8U)
    #define EXT_OSC_RANGE       RANGE_HIGH
#elif (EXT_CRYSTAL_OSC_HZ >= 32768U) && (EXT_CRYSTAL_OSC_HZ <= 39063U)
    #define EXT_OSC_RANGE       RANGE_LOW
#else
    #error "Selected EXT_CRYSTAL_OSC_HZ out of bounds."
#endif

#if (EXT_CRYSTAL_OSC_HZ == MHZ(25U)) || (EXT_CRYSTAL_OSC_HZ == MHZ(50U))
    #define PLL_FREQ        (12500000U)                             ///< PLL reference 12.5 MHz           (output 100 - 225 MHz)

#elif (EXT_CRYSTAL_OSC_HZ == MHZ(24U)) || (EXT_CRYSTAL_OSC_HZ == MHZ(48U))
    #define PLL_FREQ        MHZ(12U)                                ///< PLL reference 12 MHz             (output 96 - 240 MHz)

#elif (EXT_CRYSTAL_OSC_HZ == MHZ(10U)) || (EXT_CRYSTAL_OSC_HZ == MHZ(20U))
    #define PLL_FREQ        MHZ(10U)                                ///< PLL reference 10 MHz             (output 80 - 235 MHz)
    #define FLL_FREQ        (390625UL)                              ///< FLL reference 39062.5 Hz * 10    (output 25 - 100 MHz)

#elif (EXT_CRYSTAL_OSC_HZ == MHZ(16U)) || (EXT_CRYSTAL_OSC_HZ == MHZ(32U))
    #define PLL_FREQ        MHZ(16U)                                ///< PLL reference 16 MHz             (output 128 - 240 MHz)
    #define FLL_FREQ        (312500UL)                              ///< FLL reference 31250.0 Hz * 10    (output 20 - 80 MHz)

#else
    #error "Selected EXT_CRYSTAL_OSC_HZ frequency not supported."
#endif



/** \brief    Calculation of PLL settings
 *
 *  \details  This section is for calculating FLL division and multiplication factor for PLL.
 *            If selected core clock can be set, PLL_ACHIEVABLE will be defined. PLL reference
 *            clock after division must be in range <8, 16> MHz.
 *            PLL_DIV - division of input clock, range
 *            PLL_MUL - multiplication of PLL reference clock, range <16, 47>
 */
#ifdef PLL_FREQ
    #define PLL_DIV     (uint16_t)(EXT_CRYSTAL_OSC_HZ / PLL_FREQ)   // Calculate division factor
    #if (MHZ(CORE_CLOCK * 2U) % PLL_FREQ) == 0U             // Check if set core clock is reachable with PLL
        #define PLL_MUL (MHZ(CORE_CLOCK * 2U) / PLL_FREQ)   // Calculate multiplication factor
        #if (PLL_MUL >= 16u) && (PLL_MUL <= 47u)            // Check if multiplication is in bounds
            #define PLL_ACHIEVABLE
        #endif
    #endif
#endif


/** \brief    Calculation of FLL settings
 *
 *  \details  This section is for calculating FLL division and multiplication factor for FLL.
 *            If selected core clock can be set, FLL_ACHIEVABLE will be defined. FLL reference
 *            clock after division must be in range <31.25, 39.0625> kHz.
 *            FLL_DIV - division of input clock, [32, 64, 128, 256, 512, 1024] for external oscilator >= 3 MHz
 *            FLL_MUL - multiplication of PLL reference clock, must be dividable by 640 (732 if DMX32 is set)
 */
#ifdef FLL_FREQ
    #define FLL_DIV     ((EXT_CRYSTAL_OSC_HZ * 10U) / FLL_FREQ) // Calculate division factor
    #define FLL_MUL     (MHZ(CORE_CLOCK * 10U) / FLL_FREQ)      // Calculate multiplication factor
    #if ((FLL_MUL % 640U) == 0U) && ((FLL_MUL / 640U) <= 4U)    // Check if set core clock is in bounds and reachable with FLL
        #define FLL_ACHIEVABLE
    #endif
#endif



/** \brief    Calculation of Fast peripheral clock divider
 *
 *  \details  Fast peripheral clock must not exceed 120 MHz. Fast peripheral
 *            clock must be integer value and ideally has to be MCGOUTCLK / 2.
 */
#if (CORE_CLOCK % PCLOCK) == 0U
    #define FAST_PERIPH_DIV     ((CORE_CLOCK / PCLOCK) - 1U)
#else
    #error "CORE_CLOCK has to be dividable by PCLOCK without reminder"
#endif


/** \brief    Calculation of best Flash Bus clock divider
 *
 *  \details  Flash Bus clock must not exceed 27.5 MHz and has to be integer divide of MCGOUTCLK
 *            and an integer divide of the Fast Peripheral clock.
 *            CORE_CLOCK has to be integer multiplier of Flash Bus clock.
 *            PCLOCK has to be integer multiplier of Flash Bus clock.
 */
#if(MHZ(PCLOCK) <= 27500000U)
        #define BUS_FLASH_DIV   FAST_PERIPH_DIV
#elif ((MHZ(PCLOCK) / 27500000U) <= (16U/(FAST_PERIPH_DIV+1U)))
        #define BUS_FLASH_DIV   ((((MHZ(PCLOCK) / 27500000U) + 1U) * (FAST_PERIPH_DIV+1U)) - 1U)
#else
    #error "Flash Bus Clock frequency not reachable with selected CORE_CLOCK."
#endif


/** \brief    Calculation of best FlexBus clock divider
 *
 *  \details  FlexBus clock must not exceed 60 MHz and has to be integer divide of MCGOUTCLK
 *            Supported frequencies of Flash Bus Clock are  60, 50, and and 40 MHz.
 */
#if (MHZ(CORE_CLOCK) % MHZ(60u)) == 0U                           // 60 MHz
    #define FLEX_BUS_DIV     ((MHZ(CORE_CLOCK) / (MHZ(60u))) - 1U)

#elif (MHZ(CORE_CLOCK) % MHZ(55u)) == 0U                         // 55 MHz
    #define FLEX_BUS_DIV     ((MHZ(CORE_CLOCK) / (MHZ(55u))) - 1U)

#elif (MHZ(CORE_CLOCK) % MHZ(50u)) == 0U                         // 50 MHz
    #define FLEX_BUS_DIV     ((MHZ(CORE_CLOCK) / (MHZ(50u))) - 1U)

#elif (MHZ(CORE_CLOCK) % MHZ(40u)) == 0U                         // 40 MHz
    #define FLEX_BUS_DIV     ((MHZ(CORE_CLOCK) / (MHZ(40u))) - 1U)

#elif (MHZ(CORE_CLOCK) % MHZ(35u)) == 0U                         // 35 MHz
    #define FLEX_BUS_DIV     ((MHZ(CORE_CLOCK) / (MHZ(35u))) - 1U)

#elif (MHZ(CORE_CLOCK) % MHZ(20u)) == 0U                         // 20 MHz
    #define FLEX_BUS_DIV     ((MHZ(CORE_CLOCK) / (MHZ(20u))) - 1U)

#else
    #error "FlexBus Cock frequency not reachable with selected CORE_CLOCK."
#endif

#endif /* !defined(MCAL_MPCM_INCLUDED) */

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

#if !defined(MCAL_MPCM_INCLUDED)

/*lint -esym(749,BASIC_*) "local enumeration constant not referenced [MISRA 2012 Rule 2.5, advisory]" */
/** \brief    Master clock (MCGOUTCLK) selection.
 *
 *  \details  Enumeration type for selecting which clock will be used as master clock.
 *            Master clock is used as source for core clock, flash and majority of peripherals
 */
enum BASIC_MasterClock
{
    MASTER_CLOCK_FLL = 0U,      ///< Frequency Locked Loop (FLL)
    MASTER_CLOCK_IRC = 1U,      ///< Internal reference clock (IRCSCLK)
    MASTER_CLOCK_ERC = 2U,      ///< External reference clock (OSCCLK)
    MASTER_CLOCK_PLL = 3U       ///< Phase Locked Loop (PLL)
};

/** \brief    Internal reference clock selection for IRCSCLK bus.
 *
 *  \details  Enumeration type for selecting between slow and fast internal
 *            reference clocks. For fast IRC there is also clock divider.
 */
enum BASIC_IRC_Select
{
    IRC_FIRC_4_MHZ      = 0U,   ///< Fast IRC undivided         (4 MHz)
    IRC_FIRC_2_MHZ      = 1U,   ///< Fast IRC divided by 2      (2 MHz)
    IRC_FIRC_1_MHZ      = 2U,   ///< Fast IRC divided by 4      (1 MHz)
    IRC_FIRC_500_KHZ    = 3U,   ///< Fast IRC divided by 8      (500 kHz)
    IRC_FIRC_250_KHZ    = 4U,   ///< Fast IRC divided by 16     (250 kHz)
    IRC_FIRC_125_KHZ    = 5U,   ///< Fast IRC divided by 32     (125 kHz)
    IRC_FIRC_62500_HZ   = 6U,   ///< Fast IRC divided by 64     (62.500 kHz)
    IRC_FIRC_31250_HZ   = 7U,   ///< Fast IRC divided by 128    (31.250 kHz)
    IRC_SIRC_32_KHZ     = 8U    ///< Slow IRC undivided         (32.768 kHz)
};

/** \brief    Clock output to pin selection.
 *
 *  \details  Enumeration type for selecting which clock bus will be selected
 *            as output to CLKOUT pin using SIM_SOPT2_CLKOUTSEL. Values are
 */
enum BASIC_ClockOutput
{
    CLKOUT_FLEX_BUS         = 0U,   ///< FlexBus = MCGOUTCLK / SIM_CLKDIV1_OUTDIV2
    CLKOUT_FLASH_CLOCK      = 2U,   ///< FlashClock = MCGOUTCLK / SIM_CLKDIV1_OUTDIV4
    CLKOUT_LPO_CLOCK        = 3U,   ///< LPO clock (1 kHz)
    CLKOUT_MCGIRCLK         = 4U,   ///< Internal reference clock (slow/fast)
    CLKOUT_OSCERCLK_UDIV    = 5U,   ///< External undivided clock
    CLKOUT_OSCERCLK         = 6U    ///< External clock divided by OSC0_OSC_DIV_ERPS
};

#endif /* !defined(MCAL_MPCM_INCLUDED) */

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

#if !defined(MCAL_MPCM_INCLUDED)
/** \brief   Function for initialisation of clock tree.
 */
void BASIC_vInitClock(void);


/** \brief    Selection of master clock input.
 *
 *  \details  This function sets mater clock (MCGOUTCLK) based on input parameter.
 *            After switching, function waits for status flag if the transition is over.
 */
void BASIC_SetMasterClockInput(enum BASIC_MasterClock masterClock);


/** \brief    Selection of internal reference clock input.
 *
 *  \details  This function sets configures which internal reference clock will be used
 *            as input for master clock. In case of fast IRC, also divider can be selected.
 */
void BASIC_ConfigureInternalOsc(enum BASIC_IRC_Select ircSelect);


/** \brief    Configuration of external oscillator.
 *
 *  \details  This function enables external oscillator and sets correct range.
 */
void BASIC_ConfigureExternalOsc(void);


/** \brief    Configuration of PLL (Phase Locked Loop).
 *
 *  \details  This function enables configures PLL based on input parameters.
 *            After configuring function waits for PLL to lock.
 */
void BASIC_ConfigurePLL(uint16_t pllDiv, uint8_t pllMul);


/** \brief    Configuration of FLL (Frequency Locked Loop).
 *
 *  \details  This function enables configures FLL based on input parameters.
 *            Input to FLL is currently set to external oscillator.
 */
void BASIC_ConfigureFLL(uint16_t fllDiv, uint16_t fllMul);


/** \brief    Configuration of clock output to CLKOUT pin.
 *
 *   \details Calling this function is pin PTA6 configured CLKOUT alternate function.
 *            Parameter outputType defines which clock will be outputed to CLKOUT.
 */
void BASIC_ConfigureClockOutput(enum BASIC_ClockOutput outputType);

#endif /* !defined(MCAL_MPCM_INCLUDED) */

/******************************************************************************/
/* PRIVATE DATA DEFINTIONS                                                    */
/******************************************************************************/

/**
*   \brief    Set flash memory options bytes.
*   \details  The values of FPROT3_BYTE, FPROT2_BYTE, ... FDPROT_BYTE
*             can be set set/changed in Derivative.inc file
*             WARNING!!! DO NOT REMOVE OPTION BYTES!!! WITHOUT OPTIONBYTES MCU WILL BE BLOCKED!!!
*/
SDEF_SetSegmentConst(OPTION_BYTE)
const uint8_t __OPTION_BYTE[8]__attribute__((used))={FPROT3_BYTE,FPROT2_BYTE,FPROT1_BYTE,FPROT0_BYTE,FSEC_BYTE,FOPT_BYTE,0xFF,0xFF};
SDEF_SetSegmentConst_Default()

/**
*   \brief    Set Backdoor Comparison Key.
*   \details  todo
*/
SDEF_SetSegmentConst(BACKDOOR_KEY)
const uint8_t __BACKDOOR_KEY[8]__attribute__((used))={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
SDEF_SetSegmentConst_Default()

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


/** \brief    Clock initialization to selected frequencies.
*
*   \details  This function set a system clock to selected frequency and
*             dividers (OUTDIV1 to OUTDIV4) to 1.
*             Only external crystals with frequency 16 or 24 MHz are supproted.
*             If CLKOUT_ENABLE is uncommented then output from Flexbus Clock is
*             available on PTA6 pin.
*/
void BASIC_vInitPlatform(void)
{
  #if defined(MCAL_MPCM_INCLUDED)
    const struct MPCM_PowerModesConfig customPowerModes =
    {
        /* Check available configurations for your platform in mcpm_mc.h */
        &MPCM_CFG_DEFAULT,              // Take configuration from make process
        NULL,                           // No low power run configuration
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

#ifdef APP_VARIANT
/* APP_VARIANT is used because only in application make sense using these
 * caches. And also RomLoader (FW1) has problems to fit when this is present. */

/* Enable ICACHE - inline function from CMSIS */
    SCB_InvalidateICache();
    SCB_EnableICache();

    /* Enable DCACHE - inline function from CMSIS */
    SCB_InvalidateDCache();  //Turns off D-Cache
    SCB_EnableDCache();      //Turns on D-Cache

    /* Power Mode Protection register. This register provides protection for
     * entry into any low-power run or stop mode. The enabling of the low-power
     * run or stop mode occurs by configuring the Power Mode Control register
     * (PMCTRL). The PMPROT register can be written only once after any system
     * reset !!! */

    /* SMC_SetPowerModeProtection should be 0xA2 */
    SMC_PMPROT = (uint8_t)(SMC_PMPROT_AVLLS_MASK | \
                  SMC_PMPROT_AVLP_MASK  | \
                  SMC_PMPROT_AHSRUN_MASK);

    SMC_PMCTRL_RUNM = HIGH_SPEED_RUN_MODE;  // SMC_SetPowerModeHsrun = 3

    while (SMC_PMSTAT != HIGH_SPEED_RUN_MASK) {
    }
#endif  //#ifdef APP_VARIANT

#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    FSF_vBoot();
#endif
}

/** \brief    Configure Platform.
*
*   \details  This function is calling the function (BASIC_vInitPlatform) to set the system clock and
*             enabling the clock for HUART and GPIO units.
*/
void BASIC_vConfigPlatform(void)
{
    /* Clock initialization */
    BASIC_vInitPlatform();

    /* Enable clocks to all GPIO ports */
    SIM_SCGC5 |=    (SIM_SCGC5_PORTA_MASK | \
                     SIM_SCGC5_PORTB_MASK | \
                     SIM_SCGC5_PORTC_MASK | \
                     SIM_SCGC5_PORTD_MASK | \
                     SIM_SCGC5_PORTE_MASK);

#if defined (HUART0_USED)
    SIM_SCGC4_UART0 = 1;
#elif defined (HUART1_USED)
    SIM_SCGC4_UART1 = 1;
#elif defined (HUART2_USED)
    SIM_SCGC4_UART2 = 1;
#elif defined (HUART3_USED)
    SIM_SCGC4_UART3 = 1;
#elif defined (HUART4_USED)
    SIM_SCGC1_UART4 = 1;
#elif defined (HUART5_USED)
    SIM_SCGC1_UART5 = 1;
#endif
#if defined(TIME_DEBUG_MODE)
 SIM_SCGC6_PIT = 1u;  ///< Enable peripheral clock for PIT in case of TIME_DEBUG_MODE
#endif
#ifdef APP_VARIANT
   /* Here should be placed all application specific initialization */
   // HADC0_vInit(0);
#endif

}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

#if !defined(MCAL_MPCM_INCLUDED)

void BASIC_vInitClock(void)
{
    /* Set safe clock divisions */
    SIM_CLKDIV1 = 0x01170000U;

#ifdef CLKOUT_ENABLE
    BASIC_ConfigureClockOutput(CLKOUT_OSCERCLK_UDIV);
#endif /* CLKOUT_ENABLE */

    /* Configure internal 4 MHz clock */
    BASIC_ConfigureInternalOsc(IRC_FIRC_4_MHZ);

    /* Switch to internal clock */
    BASIC_SetMasterClockInput(MASTER_CLOCK_IRC);

    /* Configure external oscillator */
    BASIC_ConfigureExternalOsc();

    /* Configure and set PLL as clock source if possible */
#if defined(PLL_ACHIEVABLE)
    BASIC_ConfigurePLL(PLL_DIV, (uint8_t)PLL_MUL);
    BASIC_SetMasterClockInput(MASTER_CLOCK_PLL);

    /* In case PLL can not be used, configure and use FLL */
#elif defined(FLL_ACHIEVABLE)
    BASIC_ConfigureFLL((uint16_t)FLL_DIV, (uint16_t)FLL_MUL);
    BASIC_SetMasterClockInput(MASTER_CLOCK_FLL);

#else
    #warning "Selected core frequency is not reachable with PLL or FLL"
#endif /* FLL_ACHIEVABLE */

    /* Set optimal clock divisions */
    SIM_CLKDIV1 = ((uint32_t)FAST_PERIPH_DIV << SIM_CLKDIV1_OUTDIV2_POS)  |
                  ((uint32_t)FLEX_BUS_DIV << SIM_CLKDIV1_OUTDIV3_POS)     |
                  ((uint32_t)BUS_FLASH_DIV << SIM_CLKDIV1_OUTDIV4_POS);
}


void BASIC_SetMasterClockInput(enum BASIC_MasterClock masterClock)
{
    uint8_t masterClockSelect = (uint8_t)masterClock;
    uint8_t masterClockStatus = (uint8_t)masterClock;

    if(masterClock == MASTER_CLOCK_PLL)
    {
        masterClockSelect = CLKS_FLL_PLL;       // In case of PLL, select is same as FLL
        MCG_C6_PLLS = ON;                       // PLL selection
        while(MCG_S_PLLST == OFF) {}            // Wait to switch to PLL
    }
    if(masterClock == MASTER_CLOCK_FLL)
    {
        MCG_C6_PLLS = OFF;                      // FLL selection
        while(MCG_S_PLLST == ON) {}             // Wait to switch to FLL
    }

    MCG_C1_CLKS = masterClockSelect;            // Select FLL/PLL as input to MCGOUTCLK
    while(MCG_S_CLKST != masterClockStatus) {}  // Wait to switch to MCGOUTCLK to PLL
}


void BASIC_ConfigureInternalOsc(enum BASIC_IRC_Select ircSelect)
{
    if(ircSelect < IRC_SIRC_32_KHZ)
    {
        MCG_SC_FCRDIV = (uint8_t)ircSelect;     // Divider to 1
        MCG_C2_IRCS = IRCS_FAST;                // Select fast internal oscillator as clock source
    }
    else
    {
        MCG_C2_IRCS = IRCS_SLOW;                // Select slow internal oscillator as clock source
    }
}


void BASIC_ConfigureExternalOsc(void)
{
    /* Enable external crystal */
    OSC0_CR_ERCLKEN = ON;

    /* Configure range, gain and external reference clock select */
    MCG_C2 |= (uint8_t)(MCG_C2_EREFS_MASK | MCG_C2_HGO_MASK | ( MCG_C2_RANGE_MASK & (EXT_OSC_RANGE << MCG_C2_RANGE_POS) ) );

    /* Wait for external clock to stabilize */
    while (MCG_S_OSCINIT0 == OFF) {}
}


void BASIC_ConfigurePLL(uint16_t pllDiv, uint8_t pllMul)
{
    MCG_C6_PLLS = OFF;                          // Disable PLL before setting
    while(MCG_S_PLLST == ON) {}                 // Wait for PLL to switch off
    MCG_C5_PRDIV = (uint8_t)PRDIV(pllDiv);      // Set PLL division
    MCG_C6_VDIV = VDIV(pllMul);                 // Set PLL multiplication
    MCG_C5_PLLCLKEN = ON;                       // Enable PLL
    while(MCG_S_LOCK0 == OFF) {}                // Wait for PLL to lock
}


void BASIC_ConfigureFLL(uint16_t fllDiv, uint16_t fllMul)
{
    uint8_t sqrtFllDiv;

    // Calculate simple square root of fllMul (fllMul has to be 2^(sqrtFllMul-1))
    for(sqrtFllDiv = 0U; (fllDiv >> ((uint16_t)sqrtFllDiv + 1u)) != 0U; sqrtFllDiv++) {}

    MCG_C1_IREFS = OFF;                         // Input to FLL is from external oscillator
    MCG_C4_DMX32 = OFF;                         // Set range selection for FLL 32.25 - 39.0625 kHz
    MCG_C1_FRDIV = sqrtFllDiv - 5U;             // Set FLL division (0 - division by 32 for HIGH_RANGE of ext osc)
    MCG_C4_DRST_DRS = (uint8_t)DRST_DRS(fllMul);         // Set multiplication factor
}


void BASIC_ConfigureClockOutput(enum BASIC_ClockOutput outputType)
{
    SIM_SCGC5_PORTC = ON;                       // Enable clock to PORT
    SIM_SOPT2_CLKOUTSEL = (uint8_t)outputType;  // Set output clock selection
    PORTC_PCR3_MUX = PORT_MUX_AF5;              // Set alternate function
}

#endif /* !defined(MCAL_MPCM_INCLUDED) */
