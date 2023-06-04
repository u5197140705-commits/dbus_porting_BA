/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / SiLabs EFR32xG21
 *  COMP_ABBREV      BASIC
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
#include "em_chip.h"
#include "sl_device_init_hfrco.h"
#include "sl_device_init_hfxo.h"
#include "sl_device_init_lfxo.h"
#include "sl_device_init_clocks.h"
#include "sl_device_init_emu.h"

#if defined(RTOS)
#include "int_prio.h"
#include "em_cmu.h"
#include "sl_hfxo_manager.h"
#endif // defined(RTOS)
#if defined(BLUETOOTH_MODULE)
#include "nvm3_default.h"
#include "sl_power_manager.h"
#include "cpu.h"
#endif // defined(BLUETOOTH_MODULE)


#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fs_psm.h"
#endif


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void BASIC_deviceInitNvic(void);
static void BASIC_deviceInitHfrco(void);
static void BASIC_hfxoManagerInitHardware(void);
static void BASIC_initHfxo(void);
static void BASIC_initLfxo(void);
static void BASIC_initClocks(void);
static void BASIC_initEmu(void);
#if defined(BLUETOOTH_MODULE) && defined(RTOS)
static void BASIC_nvm3InitDefault(void);
static void BASIC_cpuInit(void);
static void BASIC_powerManagerInit(void);
#endif // defined(BLUETOOTH_MODULE) && defined(RTOS)


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*lint -e522 -e523 function 'BASIC_deviceInitNvic'and'BASIC_hfxoManagerInitHardware'
 *  lack side effects when RTOS is not used, because they are empty then */
void BASIC_vInitPlatform(void)
{
    CHIP_Init();                // Device errata
    BASIC_deviceInitNvic();   // nvic priority 
    BASIC_deviceInitHfrco();
    BASIC_hfxoManagerInitHardware(); //Initialize HFXO Manager module.
    BASIC_initHfxo();           // initilize the HFXO and tune it
    BASIC_initLfxo();           // initilize the LFXO
    BASIC_initClocks();         // initialize SYSCLK, etc.
    BASIC_initEmu();           // initializes the Energy Management Unit
#if defined(BLUETOOTH_MODULE) && defined(RTOS)
    BASIC_nvm3InitDefault();   // initialization of NVM3 only if it is used
    BASIC_cpuInit();           // initialize CPU module - needed for OS RTOS 
    BASIC_powerManagerInit();   // initialize Power Manager module
#endif // defined(BLUETOOTH_MODULE) && defined(RTOS)

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


#ifdef APP_VARIANT
    /* Here should be placed all application specific initialization */

    /* Set Priority of configurable interrupts */
    // setDefaultInterruptPriority();

    // HADC_vInit(0);

    /* all other initializations */
    // ...
#endif
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
/** initialize the HFXO */
static void BASIC_initHfxo(void)
{
    (void) sl_device_init_hfxo();
}

/**
 * Initialize LFXO
 *
 * @details
 * Configure the low frequency crystal oscillator using settings in the
 * configuration header `sl_device_init_lfxo_config.h`.
 *
 * The precision of the oscillator can be indicated using the configuration
 * option `SL_DEVICE_INIT_LFXO_PRECISION`. This is used by certain wireless
 * stacks to optimize wakeup time for scheduled events (such as Bluetooth
 * advertising).
 */
static void BASIC_initLfxo(void)
{
#if !defined(BLUETOOTH_MODULE)
    (void) sl_device_init_lfxo();
#endif
}

static void BASIC_deviceInitNvic(void)
{
#if defined(RTOS)
    INTP_setApplicationPriority(); // set NVIC priority to default
#endif
}

static void BASIC_hfxoManagerInitHardware(void)
{
#if defined(RTOS)
    (void) sl_hfxo_manager_init_hardware(); // HFXO Manager module hardware specific initialization.
#endif
}

static void BASIC_deviceInitHfrco(void)
{
    (void) sl_device_init_hfrco();
}

/**
 * Configure the clock tree and enable clocks
 *
 * @details
 * Automatically configures the clock tree to use the appropriate HF and LF
 * clock sources depending on which other device initialization components are
 * present.
 *
 */
static void BASIC_initClocks(void)
{
    (void) sl_device_init_clocks();
}

/**
 * Initialize EMU
 *
 * @details
 * Initializes the Energy Management Unit by configuring Energy Mode 4 (EM4)
 * according to settings in the configuration header
 * `sl_device_init_emu_config.h`.
 *
 * On Series 2 devices, debugging in EM2 is enabled by default by ensuring
 * that power to the debug power domain is sustained on EM2 entry.
 *
 * @note Voltage scaling for Energy Modes 0 to 3 are configured by the
 * @ref power_manager.
 */
static void BASIC_initEmu(void)
{
    (void) sl_device_init_emu();
}

#if defined(BLUETOOTH_MODULE) && defined(RTOS)
static void BASIC_nvm3InitDefault(void)
{
    (void) nvm3_initDefault();
}

/* Initialize CPU module - needed for OS RTOS */
static void BASIC_cpuInit(void)
{
#if defined(RTOS)
    CPU_Init();
#endif
}

/*
 * Initialize Power Manager module.
 */
static void BASIC_powerManagerInit(void)
{
#if defined(RTOS)
    (void) sl_power_manager_init();
#endif
}
#endif // defined(BLUETOOTH_MODULE) && defined(RTOS)
