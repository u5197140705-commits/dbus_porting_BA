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

#ifndef SPI_DATA_MNGR_H
#define SPI_DATA_MNGR_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     spi_data_mngr.h
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    Interfacing of the SPI-data manager processing of the SSB-framework
 *            access layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
extern "C" {
#include "mcal/mspi.h"
#include "mcal/mspi_types.h"
#include "mcal_channels.h"
#include "mspi_mc.h"

#include "bsh_stdinc.h"
#include "debug_mapping.h"
}

#include "ssbf_mngr_common.h"
#include "spi_data.h"


/* --------- Begin: To be defined by the user --------- */
/// extern const struct MDIO_Channel MDIOB9_MI2C1_SDA_CFG;
/// extern const struct MDIO_Channel MDIOA9_MI2C1_SCL_CFG;
/* --------- End: To be defined by the user ----------- */

/******************************************************************************/
/* CLASS-IF DEFINITION                                                        */
/******************************************************************************/
namespace SSBF
{
    //lint -e1790 We ignore the Info 1790: public base 'SSBF::I2cData_c' of 'SSBF::I2cDataMngr_c' has no non-destructor virtual functions
    //     Because it is completely incomprehensible, especially the detailed description in the manual
    
    class SpiDataMngr_c : public SpiData_c
    {
    private:
///        static const struct MSPI_Channel SpiChannelConfigs[SSBF_MNGR_NUMBER_OF_I2C];
///        static const struct MI2C_Config SpiGeneralConfigs[SSBF_MNGR_NUMBER_OF_I2C];
                     ///< See the explanations in spi_data_mngr.cpp
                     ///< (at the user-defined initializations)
                     //@@ To be defined similar to i2c_data_mngr.h

        static const uint8_t SpiInterfaceIdxs[SSBF_MNGR_NUMBER_OF_HUBS];

    public:
        /** \brief  Creates the object of this class
        */
        SpiDataMngr_c(void);

        /** \brief  Initializes the layer of this manager class
        *
        *   \param  instanceIdx    Index of the instance of this class
        *                          (0, ..., SSBF_MNGR_NUMBER_OF_HUBS-1)
        *
        *   \details
        */
        void initSpiDataMngr(uint8_t instanceIdx);
    };
    //lint +e1790
}

#endif // From: #ifndef SPI_DATA_MNGR_H

