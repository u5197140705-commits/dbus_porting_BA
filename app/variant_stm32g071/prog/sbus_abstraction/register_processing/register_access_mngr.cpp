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
 *  COMP_ABBREV      REGXS
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     register_access_mngr.cpp
 *
 *  \ingroup  sbus_abstraction/register_processing
 *
 *  \brief    Register access manager of the SSB-abstraction layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "register_access_mngr.h"

using namespace ::SSBAL::SSBR;
using namespace ::SSBF;

/*lint -e40 Usage of nullptr does not cause errors and is recommended @@ */

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* STATIC ATTRIBUTE DECLARATIONS                                              */
/******************************************************************************/

/******************************************************************************/
/* METHODS                                                                    */
/******************************************************************************/
RegisterAccessMngr_c::RegisterAccessMngr_c(void)
{
    DBGX_logStrInt_SCN_SSB_CONSTR("Constructor: RegisterAccessMngr_c: ",
                                  (uint32_t)this, DBGX_UNSIGNED_HEXADECIMAL);
}

void RegisterAccessMngr_c::initRegisterAccessMngr(uint8_t i2cAddrOffsets)
{
    class SiData_c *siDataObjPtr;

    /* ---------------------------------------------------------- */

    DBGX_logStr_SCN_SSB_INIT("Initialization: initRegisterAccessMngr ");

    /* ---------------------------------------------------------- */

    siDataObjPtr = getSiDataObjPtr();       // From SSBF::SiData_c::
    SSBERR_handleErrDbgIf(siDataObjPtr == nullptr, SSB_ERR_REGXSMNGR_GET_OBJPTR);

    associateSiDataObjPtr(siDataObjPtr);    // From RegisterAccess_c::

    initRegisterAccess();                   // From RegisterAccess_c::
    initSiDataMngr(i2cAddrOffsets);         // From SSBF::SiDataMngr_c::
}

/*lint +e40 @@ */

