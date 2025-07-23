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
#include "register_processing/register_access_mngr.h"

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
                                  static_cast<uint32_t>(this), DBGX_UNSIGNED_HEXADECIMAL);
}

void RegisterAccessMngr_c::initRegisterAccessMngr(uint8_t i2cAddrOffsets)
{
    class SiData_c *siDataObjPtr;
    class Timers_c *timersObjPtr;

    /* ---------------------------------------------------------- */

    DBGX_logStr_SCN_SSB_INIT("INI initRegisterAccessMngr");

    /* ---------------------------------------------------------- */

    siDataObjPtr = getSiDataObjPtr();       // From SSBF::SiData_c::
    SSBERR_handleErrDbgIf(siDataObjPtr == nullptr, SSB_ERR_REGXSMNGR_GET_OBJPTR);

    associateSiDataObjPtr(siDataObjPtr);    // From RegisterAccess_c::

    timersObjPtr = getTimersObjPtr();       // From SSBF::Timers_c::
    SSBERR_handleErrDbgIf(timersObjPtr == nullptr, SSB_ERR_TIMERSMNGR_GET_OBJPTR);

    associateTimersObjPtr(timersObjPtr);
                                            // From RegisterAccess_c::

    initRegisterAccess();                   // From RegisterAccess_c::
    initSiDataMngr(i2cAddrOffsets);         // From SSBF::SiDataMngr_c::
    initTimersMngr();                       // From SSBF::TimersMngr_c::
}

/*lint +e40 @@ */

