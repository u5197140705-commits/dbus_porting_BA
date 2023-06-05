/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB_HAL_EFR32BG21
 *  COMP_ABBREV      HADC
 *  CREATED          25.11.2019
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief  This module contains HADCPackage1 settings of HAL ADC for Silicon Labs EFR32BG21.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hadc.h"
#include "hadcpriv.h"
#include "utility.h"

/******************************************************************************/
/* PUBLIC DATA DEFINTIONS                                                     */
/******************************************************************************/

const IADC_Init_t HADC_tInitial =
{
    .iadcClkSuspend0       = false,                           // suspend IADC_CLK until PRS0 trigger
    .iadcClkSuspend1       = false,                           // suspend IADC_CLK until PRS1 trigger
    .debugHalt             = false,                           // continue operation as normal during debug mode
    .warmup                = iadcWarmupKeepWarm,              // kept warmup
    .timebase              = 0U,                              // IADC_Init() will calculate the timebase using the currently defined CMU clock setting for the IADC
    .srcClkPrescale        = 1U,                              // user requested source clock divider (prescale+1)
    .timerCycles           = 4U,                              // Number of CLK_SRC_ADC cycles per TIMER event
    .greaterThanEqualThres = 0xFFFFU,                         // Digital window comparator greater-than or equal threshold
    .lessThanEqualThres    = 0x0000U                          // Digital window comparator less-than or equal threshold
};

const IADC_AllConfigs_t HADC_tConfigs =
{
    {
        {
            .adcMode        = iadcCfgModeNormal,              // IADC mode; Normal, High speed or High Accuracy
            .osrHighSpeed   = iadcCfgOsrHighSpeed2x,          // Over sampling ratio for High Speed and Normal modes
            .analogGain     = iadcCfgAnalogGain1x,            // Analog gain
            .reference      = iadcCfgReferenceVddx,           // Reference selection
            .twosComplement = iadcCfgTwosCompAuto,            // Two's complement reporting
            .adcClkPrescale = 15U,                            // ADC_CLK divider (prescale+1)
            .vRef           = 1U                              // Vref magnitude expressed in millivolts
        },
        {
            .adcMode        = iadcCfgModeNormal,              // IADC mode; Normal, High speed or High Accuracy
            .osrHighSpeed   = iadcCfgOsrHighSpeed4x,          // Over sampling ratio for High Speed and Normal modes
            .analogGain     = iadcCfgAnalogGain2x,            // Analog gain
            .reference      = iadcCfgReferenceVddx,           // Reference selection
            .twosComplement = iadcCfgTwosCompAuto,            // Two's complement reporting
            .adcClkPrescale = 15U,                            // ADC_CLK divider (prescale+1)
            .vRef           = 1U                              // Vref magnitude expressed in millivolts
        }
    }
};

const IADC_InitSingle_t HADC_tInitSingle =
{
    .alignment      = iadcAlignRight12,                       // Alignment of data in FIFO
    .showId         = false,                                  // Tag FIFO entry with single indicator (0x20)
    .dataValidLevel = iadcFifoCfgDvl1,                        // Data valid level is 1 before requesting DMA transfer
    .fifoDmaWakeup  = false,                                  // Wake-up DMA when FIFO reaches data valid level
    .triggerSelect  = iadcTriggerSelImmediate,                // Trigger selection
    .triggerAction  = iadcTriggerActionOnce,                  // Trigger action
    .singleTailgate = false,                                  // If true, wait until end of SCAN queue before single queue warmup and conversion
    .start          = false                                   // Start scan immediately
};

const IADC_SingleInput_t HADC_tSingleInput[] =
{
    {
        .negInput = iadcNegInputGnd,                              // Port/pin input for the negative side of the ADC
        .posInput = iadcPosInputPortDPin2,                        // Port/pin input for the positive side of the ADC
        .configId = 0U,                                           // Configuration id
        .compare  = false                                         // Perform digital window comparison on the result from this entry
    },
    {
        .negInput = iadcNegInputGnd,                              // Port/pin input for the negative side of the ADC
        .posInput = iadcPosInputPortDPin3,                        // Port/pin input for the positive side of the ADC
        .configId = 1U,                                           // Configuration id
        .compare  = false                                         // Perform digital window comparison on the result from this entry
    },
    {
        .negInput = iadcNegInputGnd,                              // Port/pin input for the negative side of the ADC
        .posInput = iadcPosInputPortDPin3,                        // Port/pin input for the positive side of the ADC
        .configId = 2U,                                           // Configuration id
        .compare  = false                                         // Perform digital window comparison on the result from this entry
    }
};

/*lint -esym(9003,HADC0_atConfig) better maintainability when not in block scope*/
const T_HADC_Config HADC0_atConfig[] =
{
    {
        &HADC_tInitial,
        &HADC_tConfigs,
        &HADC_tInitSingle,
        &HADC_tSingleInput[0]
    }
};

const uint8_t HADC_u8SingleInputCount = (uint8_t)UTI_NELEMENTS(HADC_tSingleInput);

/*lint -esym(9003,HADC_u8*) used not only in block scope*/
uint8_t HADC_u8CurrentPin = 0U;
uint8_t HADC_u8CurrentConfigIndex = 0U;
uint8_t HADC_u8Error = HADC_NO_ERROR;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void HADC0_vInit(uint8_t index)
{
    const T_HADC_Config *const ptCfg = &HADC0_atConfig[index];
    const uint8_t HADC_u8ConfigItemCount  = (uint8_t)UTI_NELEMENTS(HADC0_atConfig);

    if(index < HADC_u8ConfigItemCount)
    {
        HADC_u8CurrentPin = 0U;
        HADC_u8CurrentConfigIndex = index;

        // Reset IADC to reset configuration in case it has been modified by
        // other code
        IADC_reset(IADC0);

        // Select clock for IADC
        CMU_ClockSelectSet((uint8_t)cmuClock_IADCCLK, (uint32_t)cmuSelect_FSRCO);  // FSRCO - 20MHz

        IADC_init(IADC0, ptCfg->Init, ptCfg->AllConfigs);
        IADC_initSingle(IADC0, ptCfg->InitSingle, &ptCfg->SingleInput[HADC_u8CurrentPin]);

        // Allocate the analog bus for ADC0 inputs: .negInput and/or .posInput
        //GPIO->ABUSALLOC  |= GPIO_ABUSALLOC_AEVEN0_ADC0;   // allocate for even pin port A peripheral 0
        //GPIO->ABUSALLOC  |= GPIO_ABUSALLOC_AODD0_ADC0;    // allocate for odd  pin port A peripheral 0
        //GPIO->ABUSALLOC  |= GPIO_ABUSALLOC_AEVEN1_ADC0;   // allocate for even pin port A peripheral 1
        //GPIO->ABUSALLOC  |= GPIO_ABUSALLOC_AODD1_ADC0;    // allocate for odd  pin port A peripheral 1
        //GPIO->BBUSALLOC  |= GPIO_BBUSALLOC_BEVEN0_ADC0;   // allocate for even pin port B peripheral 0
        //GPIO->BBUSALLOC  |= GPIO_BBUSALLOC_BODD0_ADC0;    // allocate for odd  pin port B peripheral 0
        //GPIO->BBUSALLOC  |= GPIO_BBUSALLOC_BEVEN1_ADC0;   // allocate for even pin port B peripheral 1
        //GPIO->BBUSALLOC  |= GPIO_BBUSALLOC_BODD1_ADC0;    // allocate for odd  pin port B peripheral 1
        GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0;   // allocate for even pin port C and D peripheral 0
        //GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0;  // allocate for odd  pin port C and D peripheral 0
        //GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN1_ADC0; // allocate for even pin port C and D peripheral 1
        //GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD1_ADC0;  // allocate for odd  pin port C and D peripheral 1

    }
    else
    {
        /* error status, item of HADC0_atConfig[] does not exist */
        HADC_u8Error = HADC_CONFIG_ITEM_NOT_EXIST;
    }
}

