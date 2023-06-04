/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Renesas RA4
 *  COMP_ABBREV      BASIC
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file   basicMcXs.c
*
*   \brief  RA4 MCU initialization and configuration implementation.
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "basic.h"
#include "processor.h"
#include "bsp/bsp_api.h"
#include "hal_data.h"
#include "fsp/system.h"
#include "huart.h"
#include "hal_data.h"
#include "bsp_io.h"

#if defined(MCAL_MPCM_INCLUDED)
#include "mcal/mcal_includes.h"
#include "mcal/mcal_assert.h"
#endif

/*lint -save -e923 */
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
void BASIC_vInitClock(void);
static void BASIC_pmsarInit(void);
/******************************************************************************/
/* PUBLIC VARIABLE DECLARATIONS                                               */
/******************************************************************************/
/** System Clock Frequency (Core Clock) */
uint32_t SystemCoreClock;
extern uint16_t g_protect_counters[4];
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) __attribute__((weak));

void BASIC_vInitPlatform(void)
{
  #if defined(MCAL_MPCM_INCLUDED)
    static const struct MPCM_PowerModesConfig customPowerModes =
    {
        /* Check available configurations for your platform in mcpm_mc.h */
        &MPCM_CFG_DEFAULT,              // Take configuration from make process
        NULL,                           // Lowered clock speeds for low-power run
        NULL,                           // No special configuration for sleep mode
        NULL                            // No special configuration for stop mode
    };
    
    if(MPCM_init(&customPowerModes) != MCAL_OK)
    {
        MCAL_error("Invalid MPCM configuration");
    }
    
    /* Initialize ELC events that will be used to trigger NVIC interrupts. 
       Temporary solution start */
    bsp_irq_cfg();
    /* Initialize RA SystemCoreClock variable. We still use RA functions depending on it */
    SystemCoreClock = MPCM_getClockFreq(MPCM_CLOCK_ICLK);
    BASIC_pmsarInit();
    R_BSP_WarmStart(BSP_WARM_START_RESET);
    #if defined(UART_CHANNEL_2ND_DBUS)
     /* Configure pins by RA functions until 2-nd DBUS will use MCAL/MUART */
    R_BSP_WarmStart(BSP_WARM_START_POST_C);
    /* Temporary solution end */
    #endif // UART_CHANNEL_2ND_DBUS
  #else
    BASIC_vInitClock();
  #endif

}

void BASIC_vConfigPlatform (void)
{
    /* Clock Configuration*/
    BASIC_vInitPlatform();

    /* Enable clock on GPIO */


#ifdef APP_VARIANT
    /* Here should be placed all application specific initialization */
    //r_sci_uart_basic_example();
    /* Set Priority of configurable interrupts */
    // setDefaultInterruptPriority();

    // HADC_vInit(0);

    /* all other initializations */
    // ...
#endif
}

/*****************************************************************************/
/* This function is called at various points during the startup process.
 * This function is declared as a weak symbol higher up in this file because 
 * it is meant to be overridden by a user implemented version. One of the main
 * uses for this function is to call functional safety code during the startup
 * process. To use this function just copy this function into your own code 
 * and modify it to meet your needs.
 *
 * @param[in]  event    Where the code currently is in the start up process
 *****************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
        /* C runtime environment has not been setup so you cannot use globals.
         System clocks are not setup. */
        g_protect_pfswe_counter = 0;
        /* Used for holding reference counters for protection bits. */
        //g_protect_counters[4] = { 0, 0, 0, 0 };
        for(uint8_t i=0; i < 4u; i++)
        {
            g_protect_counters[i] =  0;
        }
    }

    if (BSP_WARM_START_POST_CLOCK == event)
    {
        /* C runtime environment has not been setup so you cannot use globals.
         Clocks have been initialized. */
    }
    else if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment, system clocks, and pins are all setup. */
        /* Configure pins. */
        (void)R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
    }
    else
    {
        /* Do nothing */
    }
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

void BASIC_vInitClock(void)
{
/* Call pre clock initialization hook. */
    R_BSP_WarmStart(BSP_WARM_START_RESET);

/* Configure system clocks. */
    bsp_clock_init();

/* Call post clock initialization hook. */
    R_BSP_WarmStart(BSP_WARM_START_POST_CLOCK);

/* Initialize SystemCoreClock variable. */
    SystemCoreClockUpdate();

    BASIC_pmsarInit();

/* Call Post C runtime initialization hook. */
    R_BSP_WarmStart(BSP_WARM_START_POST_C);

/* Initialize ELC events that will be used to trigger NVIC interrupts. */
    bsp_irq_cfg();

}

static void BASIC_pmsarInit(void)
{
#if FSP_PRIV_TZ_USE_SECURE_REGS
/* Ensure that the PMSAR registers are reset (Soft reset does not reset PMSAR). */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_SAR);
    for (uint32_t i = 0; i < 9u; i++)
    {
        R_PMISC->PMSAR[i].PMSAR = UINT16_MAX;
    }
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_SAR);
#endif // FSP_PRIV_TZ_USE_SECURE_REGS
}
/*lint -restore */
