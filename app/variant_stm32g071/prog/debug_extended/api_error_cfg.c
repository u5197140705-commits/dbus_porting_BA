/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBGX
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup   debug_extended
 *
 *  \brief     Definitions and declarations of configuration part of the
 *             debug_extended component: Exceptional error handling
 *
 *  \details   See also FNG_DebugExtendedDataUsersManual.md
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include "api_error_cfg.h"

#ifdef DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

static DBGE_ErrorSolvingFunction_t DBGE_ErrorSolvingFunctions_Cfg[DBGE_LIMIT_OF_ERROR_SOLVING_FUNCTIONS];
static uint8_t DBGE_NumberOfErrorSolvingFunctions = 0;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

bool DBGX_registerErrorSolvingFunction(DBGE_ErrorSolvingFunction_t ErrorSolvingFunction)
{
    if(DBGE_NumberOfErrorSolvingFunctions < DBGE_LIMIT_OF_ERROR_SOLVING_FUNCTIONS)
    {
        DBGE_ErrorSolvingFunctions_Cfg[DBGE_NumberOfErrorSolvingFunctions] = ErrorSolvingFunction;
        DBGE_NumberOfErrorSolvingFunctions ++;
        return true;
    }
    else
    {
        return false;
    }
}


void DBGE_init(void)
{
    DBGX_setErrorSolvingFunctions(DBGE_ErrorSolvingFunctions_Cfg, &DBGE_NumberOfErrorSolvingFunctions, DBGE_LIMIT_OF_ERROR_SOLVING_FUNCTIONS);
}


#endif // From: #ifdef DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED
