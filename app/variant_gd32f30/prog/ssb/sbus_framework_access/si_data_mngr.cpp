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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     si_data_mngr.cpp
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    SI-data manager processing of the SSB-framework access layer
 *
 *  \details  SI means: Serial interface, abstracted from I2C or SPI
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "si_data_mngr.h"

using namespace ::SSBF;

/*lint -e40 Usage of nullptr does not cause errors and is recommended */

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* STATIC ATTRIBUTE DECLARATIONS                                              */
/******************************************************************************/
/* --------- Begin: Data to be defined by the user --------- */
const bool SiDataMngr_c::I2c_not_Spi[SSBF_MNGR_NUMBER_OF_HUBS] = 
             ///< Info, whether I2C or SPI as Host interface for each Hub instance
             ///<   true:  I2C
             ///<   false: SPI
             ///< Can be defined here by the user
             ///< @@ Still to be implemented on for more than one instance

#if   SSBF_MNGR_NUMBER_OF_HUBS == 1U
          {true};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 2U
          {true, true};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 3U
          {false, false, true};
#elif SSBF_MNGR_NUMBER_OF_HUBS == 4U
          {false, false, true, true};
#else
    #error SSBF_MNGR_NUMBER_OF_HUBS has wrong value
#endif

/* --------- End: Data to be defined by the user ----------- */

uint8_t SiDataMngr_c::SiDataMngrInstanceCnt = (uint8_t)0U;

/******************************************************************************/
/* METHODS                                                                    */
/******************************************************************************/
SiDataMngr_c::SiDataMngr_c(void)
{
    DBGX_logStrInt_SCN_SSB_CONSTR("Constructor: SiDataMngr_c: ", static_cast<uint32_t>(this),
                                  DBGX_UNSIGNED_HEXADECIMAL);

    SiDataMngrInstanceIdx = static_cast<uint8_t>(0U);
}

void SiDataMngr_c::initSiDataMngr(uint8_t i2cAddrOffsets)
{
    class I2cData_c *i2cDataObjPtr;
    class SpiData_c *spiDataObjPtr;

    SiDataMngrInstanceIdx = SiDataMngrInstanceCnt;
    SSBERR_handleErrDbgIf(SiDataMngrInstanceIdx 
                              >= static_cast<uint8_t>(SSBF_MNGR_NUMBER_OF_HUBS),
                          SSB_ERR_SIDATAMNGR_INSTANCE_IDX);
    SiDataMngrInstanceCnt++;

    DBGX_logStr_SCN_SSB_INIT("INI initSiDataMngr");

    if (I2c_not_Spi[SiDataMngrInstanceIdx] != false)
    {
        i2cDataObjPtr = getI2cDataObjPtr();             // From I2cData_c::

        SSBERR_handleErrDbgIf(i2cDataObjPtr == nullptr,
                              SSB_ERR_SIDATAMNGR_GET_I2COBJPTR);

        associateI2cDataObjPtr(i2cDataObjPtr);          // From SiData_c::
        initSiData(I2c_not_Spi[SiDataMngrInstanceIdx]); // From SiData_c::

        initI2cDataMngr(SiDataMngrInstanceIdx, i2cAddrOffsets);
                                                        // From I2cDataMngr_c::
    }
    else
    {
        spiDataObjPtr = getSpiDataObjPtr();             // From SpiData_c::
        SSBERR_handleErrDbgIf(spiDataObjPtr == nullptr,
                              SSB_ERR_SIDATAMNGR_GET_SPIOBJPTR);

        associateSpiDataObjPtr(spiDataObjPtr);          // From SiData_c::
        initSiData(I2c_not_Spi[SiDataMngrInstanceIdx]); // From SiData_c::

        initSpiDataMngr(SiDataMngrInstanceIdx);         // From SpiDataMngr_c:
    }
}

/*lint +e40 */

