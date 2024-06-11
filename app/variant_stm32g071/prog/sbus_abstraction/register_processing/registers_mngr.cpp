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
 *  COMP_ABBREV      REGS
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     registers_mngr.cpp
 *
 *  \ingroup  sbus_abstraction/register_processing
 *
 *  \brief    Register processing manager of the SSB-abstraction layer
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "registers_mngr.h"

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
RegistersMngr_c::RegistersMngr_c(void)
{
    DBGX_logStrInt_SCN_SSB_CONSTR("Constructor: RegistersMngr_c: ",(uint32_t)this,
                                  DBGX_UNSIGNED_HEXADECIMAL);
}

void RegistersMngr_c::initRegistersMngr(uint8_t i2cAddrOffsets)
{
    class RegisterAccess_c *registerAccessObjPtr;

    DBGX_logStr_SCN_SSB_INIT("Initialization: initRegistersMngr");

    registerAccessObjPtr = getRegisterAccessObjPtr();
                                                  // From RegisterAccess_c::
                                           
    SSBERR_handleErrDbgIf(registerAccessObjPtr == nullptr, 
                          SSB_ERR_REGISTERSMNGR_GET_OBJPTR);

    associateRegisterAccessObjPtr(registerAccessObjPtr);
                                                  // From Registers_c::

    initRegistersProcessing();                    // From Registers_c::
    initRegisterAccessMngr(i2cAddrOffsets);       // From RegisterAccessMngr_c::
}

/*lint +e40 @@ */
