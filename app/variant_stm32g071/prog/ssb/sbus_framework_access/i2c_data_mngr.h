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

#ifndef I2C_DATA_MNGR_H
#define I2C_DATA_MNGR_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     i2c_data_mngr.h
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    Interfacing of the I2C-data manager processing of the SSB-framework
 *            access layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
extern "C" {

#include "bsh_stdinc.h"
#include "debug_mapping.h"
}

#include "ssb_project_cfg.h"
#include "ssbf_mngr_common.h"
#include "i2c_data.h"

#ifdef SSBCFG_I2C_USED

/* --------- Begin: To be defined by the user --------- */

#ifdef SSBCFG_STM32G071_I2C_HW0
extern const struct MDIO_Channel MDIOB9_MI2C1_SDA_CFG;
extern const struct MDIO_Channel MDIOA9_MI2C1_SCL_CFG;
#endif

#ifdef SSBCFG_GD32F303_I2C_HW0
extern const struct MDIO_Channel MDIOPB9_MI2C1_SDA_CFG;
extern const struct MDIO_Channel MDIOPB8_MI2C1_SCL_CFG;
#endif

/* --------- End: To be defined by the user ----------- */

/******************************************************************************/
/* CLASS-IF DEFINITION                                                        */
/******************************************************************************/
namespace SSBF
{
    class I2cDataMngr_c : public I2cData_c
    {
    private:
        static const struct MI2C_Channel I2cChannelConfigs[SSBF_MNGR_NUMBER_OF_I2C];
        static const struct MI2C_Config I2cGeneralConfigs[SSBF_MNGR_NUMBER_OF_I2C];
        static const uint8_t I2cInterfaceIdxs[SSBF_MNGR_NUMBER_OF_HUBS];
        static uint8_t I2cHubAddresses[SSBF_MNGR_NUMBER_OF_HUBS];
                     ///< See the explanations in i2c_data_mngr.cpp
                     ///< (at the user-defined initializations)

    public:
        /** \brief  Creates the object of this class
        */
        I2cDataMngr_c(void);

        /** \brief  Initializes the layer of this manager class
        *
        *   \param  instanceIdx    Index of the instance of this class
        *                          (0, ..., SSBF_MNGR_NUMBER_OF_HUBS-1)
        *
        *   \param  i2cAddrOffsets Offsets of the I2C-addresses of the 4 Hubs
        *                          (Hub 0 has the two lowest order bits and so on)
        *
        *   \details
        */
        void initI2cDataMngr(uint8_t instanceIdx, uint8_t i2cAddrOffsets);
    };
}

#endif   // From: #ifdef SSBCFG_I2C_USED
#endif // From: #ifndef I2C_DATA_MNGR_H
