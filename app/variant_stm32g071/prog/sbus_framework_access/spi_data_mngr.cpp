/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Smart Sensor Bus
 *  COMP_ABBREV      SPI
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     spi_data_mngr.cpp
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    SPI-data manager processing of the SSB-framework access layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "spi_data_mngr.h"

using namespace ::SSBF;

/*lint -e40 Usage of nullptr does not cause errors and is recommended */


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* STATIC ATTRIBUTE DECLARATIONS                                              */
/******************************************************************************/

/* --------- Begin: To be defined by the user --------- */

/*
const struct MDIO_Channel MDIOB3_MSPI1_SCLK_CFG =   // MDIOB3_MSPI1_SCK
{
    &MDIOB,
    MDIO_PIN3,
    MDIO_SCFG(MDIO_AF0, MDIO_SPEED_HIGH)
};
const struct MDIO_Channel MDIOB4_MSPI1_SMISO_CFG =  // MDIOB3_MSPI1_MISO
{
    &MDIOB,
    MDIO_PIN4,
    MDIO_SCFG(MDIO_AF0, MDIO_SPEED_HIGH)
};
const struct MDIO_Channel MDIOB5_MSPI1_SMOSI_CFG = // MDIOB3_MSPI1_MOSI
{
    &MDIOB,
    MDIO_PIN5,
    MDIO_SCFG(MDIO_AF0, MDIO_SPEED_HIGH)
};
const struct MDIO_Channel MDIOB3_MSPI1_CS_CFG =     // MDIOA4_MSPI1_NSS
{
    &MDIOA,
    MDIO_PIN4,
    MDIO_SCFG(MDIO_AF0, MDIO_SPEED_DEFAULT)
};
*/

const struct MSPI_Channel SpiChannelConfigs = 
{
    .mspi = &MSPI2,                     // mspi_mc.c
    .sclk = &MDIOB8_MSPI2_SCK,          // mcal_channels.c
    .miso = &MDIOB6_MSPI2_MISO,         // mcal_channels.c
    .mosi = &MDIOB7_MSPI2_MOSI,         // mcal_channels.c
    .cs   = &MDIOB9                     // mcal_channels.c
};

/*
const struct MSPI_Channel spiChannel = 
{
    .mspi = &MSPI1,                     // mspi_mc.c
    .sclk = &MDIOB3_MSPI1_SCLK_CFG,     // see below
    .miso = &MDIOB4_MSPI1_SMISO_CFG,    // see below
    .mosi = &MDIOB5_MSPI1_SMOSI_CFG,    // see below
    .cs   = &MDIOB3_MSPI1_CS_CFG        // see below
};
*/

const struct MSPI_Config SpiGeneralConfigs =
{
    .dataBits           = MSPI_DATA_BITS_8,             ///< Number of data bits
    .clockPolarity      = MSPI_CLOCK_POLARITY_0,        ///< Level on SCLK pin if nothing transmit
    .clockPhase         = MSPI_CLOCK_PHASE_0,           ///< Determines the polarity of SCLK
    .dataInvert         = MSPI_DATA_INVERT_DISABLED,    ///< Enable/Disable data inversion
    .frameFormat        = MSPI_FRAME_FORMAT_MSB_FIRST,  ///< Format for transmit and receive with MSB or LSB first (Most/Least Significant Bit)
    .chipSelMode        = MSPI_CHIP_SELECT_AUTO_LOW,    ///< Setting for chip select
    .clockFreq          = 100000U,                      ///< Generated frequency on SCLK pin
    .misoPullResistor   = MDIO_PULL_UP                  ///< Enable/Disable internal pull up/down resistor on MISO pin
};

struct MSPI_Handle SpiHandleForMcal;


const uint8_t SpiDataMngr_c::SpiInterfaceIdxs[SSBF_MNGR_NUMBER_OF_HUBS] =
{
#if   SSBF_MNGR_NUMBER_OF_HUBS == 1U
    SSBF_NO_INTERFACE_IDX
#elif SSBF_MNGR_NUMBER_OF_HUBS == 2U
    SSBF_NO_INTERFACE_IDX, SSBF_NO_INTERFACE_IDX
#elif SSBF_MNGR_NUMBER_OF_HUBS == 3U
    0U, 0U, SSBF_NO_INTERFACE_IDX
#elif SSBF_MNGR_NUMBER_OF_HUBS == 4U
    0U, 0U, SSBF_NO_INTERFACE_IDX, SSBF_NO_INTERFACE_IDX
#else
    #error SSBF_MNGR_NUMBER_OF_HUBS has wrong value
#endif
};           ///< The SPI-interface indexes depending on the data
             ///< manager instance index (is equal to the Hub instance
             ///< index)
             ///< @@ Still to be implemented on for more than one instance
/* --------- End: To be defined by the user ----------- */

/******************************************************************************/
/* METHODS                                                                    */
/******************************************************************************/
SpiDataMngr_c::SpiDataMngr_c(void)
{
    DBGX_logStrInt_SCN_SSB_CONSTR("Constructor: SpiDataMngr_c: ",
                                  (uint32_t)this, DBGX_UNSIGNED_HEXADECIMAL);
}

void SpiDataMngr_c::initSpiDataMngr(uint8_t instanceIdx)
{
    DBGX_logStr_SCN_SSB_INIT("INI initSpiDataMngr");

    SSBERR_handleErrDbgIf(instanceIdx >= (uint8_t)SSBF_MNGR_NUMBER_OF_HUBS,
                          SSB_ERR_SPIDATAMNGR_INSTANCE_IDX);

    setSpiIndexes(SpiInterfaceIdxs[instanceIdx], instanceIdx);
                                                       // From SpiData_c::

    initSpiData(&SpiChannelConfigs, &SpiGeneralConfigs);
                                                       // From SpiData_c::
}

/*lint +e40 */
