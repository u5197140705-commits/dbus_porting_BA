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
 *  COMP_ABBREV      SI
 ******************************************************************************/

#ifndef SI_DATA_MNGR_H
#define SI_DATA_MNGR_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     si_data_mngr.h
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    Interfacing of the SI-data manager processing of the SSB-framework
 *            access layer
 *
 *  \details  SI means: Serial interface, abstracted from I2C or SPI
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
extern "C" {

#include "bsh_stdinc.h"
#include "sbus_framework_access/debug_mapping.h"
}

#include "errors.h"
#include "si_data.h"
#include "i2c_data_mngr.h"
#include "spi_data_mngr.h"

/******************************************************************************/
/* CLASS-IF DEFINITION                                                        */
/******************************************************************************/
namespace SSBF
{
    class SiDataMngr_c : public SiData_c, public I2cDataMngr_c, public SpiDataMngr_c
    {
    private:
        static const bool I2c_not_Spi[SSBF_MNGR_NUMBER_OF_HUBS];
                     ///< See the explanations in i2c_data_mngr.cpp
                     ///< (at the user-defined initializations)

        static uint8_t SiDataMngrInstanceCnt;
                     ///< Counts the instances of this class for setting
                     ///< SiDataMngrInstanceIdx

        uint8_t SiDataMngrInstanceIdx = (uint8_t)0U;
                     ///< Instance index of this class

    public:
        /** \brief  Creates the object of this class and of the inherited classes
        *           and initializes the generic part of the SI
        *
        *   \details
        */
        SiDataMngr_c(void);

        /** \brief  Initializes the layer of this manager class
        *           and starts initializing the next layer below 
        *
        *   \details
        */
        void initSiDataMngr(uint8_t i2cAddrOffsets);
    };
}

#endif // From: #ifndef SI_DATA_MNGR_H

