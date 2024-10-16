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
 *  COMP_ABBREV      I2C
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     i2c_data_mngr.cpp
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    I2C-data manager processing of the SSB-framework access layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "i2c_data_mngr.h"

using namespace ::SSBF;

/******************************************************************************/
/* STATIC ATTRIBUTE DECLARATIONS                                              */
/******************************************************************************/

/* --------- Begin: To be defined by the user --------- */
const struct MDIO_Channel MDIOB7_MI2C1_SDA_CFG =
{
    &MDIOB,
    MDIO_PIN7,
    MDIO_SCFG(MDIO_AF6, MDIO_SPEED_DEFAULT)
};

const struct MDIO_Channel MDIOB8_MI2C1_SCL_CFG =
{
    &MDIOB,
    MDIO_PIN8,
    MDIO_SCFG(MDIO_AF6, MDIO_SPEED_DEFAULT)
};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - */
const struct MDIO_Channel MDIOB9_MI2C1_SDA_CFG =
{
    &MDIOB,
    MDIO_PIN9,
    MDIO_SCFG(MDIO_AF6, MDIO_SPEED_DEFAULT)
};

const struct MDIO_Channel MDIOA9_MI2C1_SCL_CFG =
{
    &MDIOA,
    MDIO_PIN9,
    MDIO_SCFG(MDIO_AF6, MDIO_SPEED_DEFAULT)
};
/* --------- End: To be defined by the user ----------- */

const struct MI2C_Channel I2cDataMngr_c::I2cChannelConfigs[SSBF_MNGR_NUMBER_OF_I2C] =
        ///< MCAL-MI2C channel config. data
{            
    {   ///< MI2C1_SDA_PB7_SCL_PB8:
        &MI2C1,                // Defined in MI2C
        &MDIOB7_MI2C1_SDA_CFG, // Defined here
        &MDIOB8_MI2C1_SCL_CFG, // Defined here
        MI2C_SCFG_DEFAULT      // Defined in MI2C
    },
    {   ///< MI2C1_SDA_PB9_SCL_PA9:
        &MI2C1,                // Defined in MI2C
        &MDIOB9_MI2C1_SDA_CFG, // Defined here
        &MDIOA9_MI2C1_SCL_CFG, // Defined here
        MI2C_SCFG_DEFAULT      // Defined in MI2C
    }
};

const struct MI2C_Config I2cDataMngr_c::I2cGeneralConfigs[SSBF_MNGR_NUMBER_OF_I2C] =
        ///< MCAL-MI2C general config. data:
{
    {
        .baudrate   = MI2C_BAUDRATE_FAST,     // Transfer rate
        .mode       = MI2C_WORK_AS_MASTER,    // Mode: master
        .ownAddress = 0U,                     // Not used in master mode
#ifndef SSBF_WORK_AROUND_I2C_CALLBACK
        .commType   = MI2C_TYPE_HW,           // Operating cooperative
                                              // (non-blocking) and using
                                              // interrupts (callback),
                                              // also using the I2C-HW of uC
#else
        .commType   = MI2C_TYPE_SW,
#endif
        .intPullUps = true                    // Pull-up resistors used
    },
    {
        .baudrate   = MI2C_BAUDRATE_FAST,     // Transfer rate
        .mode       = MI2C_WORK_AS_MASTER,    // Mode: master
        .ownAddress = 0U,                     // Not used in master mode
#ifndef SSBF_WORK_AROUND_I2C_CALLBACK
        .commType   = MI2C_TYPE_HW,           // Operating cooperative
                                              // (non-blocking) and using
                                              // interrupts (callback),
                                              // also using the I2C-HW of uC
#else
        .commType   = MI2C_TYPE_SW,
#endif
        .intPullUps = true                    // Pull-up resistors used
    }
};

uint8_t I2cDataMngr_c::I2cHubAddresses[SSBF_MNGR_NUMBER_OF_HUBS] =
{
#if   SSBF_MNGR_NUMBER_OF_HUBS == 1U
    SSBF_I2C_ADDR_HUB_0
#elif SSBF_MNGR_NUMBER_OF_HUBS == 2U
    SSBF_I2C_ADDR_HUB_0, SSBF_I2C_ADDR_HUB_1
#elif SSBF_MNGR_NUMBER_OF_HUBS == 3U
    SSBF_NO_I2C_ADDR, SSBF_NO_I2C_ADDR, SSBF_I2C_ADDR_HUB_0
#elif SSBF_MNGR_NUMBER_OF_HUBS == 4U
    SSBF_NO_I2C_ADDR, SSBF_NO_I2C_ADDR, SSBF_I2C_ADDR_HUB_0, SSBF_I2C_ADDR_HUB_1
#else
    #error SSBF_MNGR_NUMBER_OF_HUBS has wrong value
#endif
};
             ///< The I2C-Hub addresses (slave addresses)
             ///< See RegI2CAddr and pending items in the ASIC-spec

const uint8_t I2cDataMngr_c::I2cInterfaceIdxs[SSBF_MNGR_NUMBER_OF_HUBS] =
{
#if   SSBF_MNGR_NUMBER_OF_HUBS == 1U
    0U
#elif SSBF_MNGR_NUMBER_OF_HUBS == 2U
    0U, 0U
#elif SSBF_MNGR_NUMBER_OF_HUBS == 3U
    SSBF_NO_INTERFACE_IDX, SSBF_NO_INTERFACE_IDX, 0U
#elif SSBF_MNGR_NUMBER_OF_HUBS == 4U
    SSBF_NO_INTERFACE_IDX, SSBF_NO_INTERFACE_IDX, 0U, 0U
#else
    #error SSBF_MNGR_NUMBER_OF_HUBS has wrong value
#endif
};           ///< The I2C-interface indexes depending on the data
             ///< manager instance index (is equal to the Hub instance
             ///< index)
             ///< @@ Still to be implemented on for more than one instance
/* --------- End: To be defined by the user ----------- */

/******************************************************************************/
/* METHODS                                                                    */
/******************************************************************************/
I2cDataMngr_c::I2cDataMngr_c(void)
{
    DBGX_logStrInt_SCN_SSB_CONSTR("Constructor: I2cDataMngr_c: ",
                                  (uint32_t)this, DBGX_UNSIGNED_HEXADECIMAL);
}

void I2cDataMngr_c::initI2cDataMngr(uint8_t instanceIdx, uint8_t i2cAddrOffsets)
{
    DBGX_logStr_SCN_SSB_INIT("INI initI2cDataMngr");

    SSBERR_handleErrDbgIf(instanceIdx >= (uint8_t)SSBF_MNGR_NUMBER_OF_HUBS,
                          SSB_ERR_I2CDATAMNGR_INSTANCE_IDX);

#if SSBF_MNGR_NUMBER_OF_HUBS >= 1U
    I2cDataMngr_c::I2cHubAddresses[0] =
        SSBF_I2C_ADDR_BASE + (i2cAddrOffsets & (uint8_t)0x03U);
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS >= 2U
    I2cDataMngr_c::I2cHubAddresses[1] =
        SSBF_I2C_ADDR_BASE + ((i2cAddrOffsets & (uint8_t)0x0CU) >> 2);
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS >= 3U
    I2cDataMngr_c::I2cHubAddresses[2] =
        SSBF_I2C_ADDR_BASE + ((i2cAddrOffsets & (uint8_t)0x30U) >> 4);
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS == 4U
    I2cDataMngr_c::I2cHubAddresses[3] =
        SSBF_I2C_ADDR_BASE + ((i2cAddrOffsets & (uint8_t)0xC0U) >> 6);
#endif

    initI2cData(I2cChannelConfigs, I2cGeneralConfigs,
                I2cHubAddresses, I2cInterfaceIdxs,
                instanceIdx);                        // From I2cData_c::
}

