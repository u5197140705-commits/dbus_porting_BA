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

#ifndef DEBUG_EXTENDED_API_ERROR_CFG_H
#define DEBUG_EXTENDED_API_ERROR_CFG_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of configuration part of
 *            the debug_extended component: Exceptional error handling
 *
 *  \details  See also FNG_DebugExtendedDataUsersManual.md.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "sts_sim_test_uty.h"
#include "stdbool.h"
#include "debug_extended/error.h"
#include "debug_extended/user_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS- ACTIVATION SWITCHES                       */
/******************************************************************************/

/* Do not change the activation switches here. Use the global switches from user_cfg.h  */
#ifdef DBGX_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED
    #define DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED
#endif

#ifdef DBGX_EXCEPTIONAL_ERROR_SOURCE_LOCATION_ACTIVATED
    #define DBGE_EXCEPTIONAL_ERROR_SOURCE_LOCATION_ACTIVATED
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - USER CONFIGURATION                       */
/******************************************************************************/

/* Maximum number of the user defined error solving callback functions */
#define DBGE_LIMIT_OF_ERROR_SOLVING_FUNCTIONS            (uint8_t)15

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#ifdef DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED

    /** \brief  Register your error solving function to the debug_extended error solving mechanism
     *  \note   Call this function in each module, you want to use error solving
     *
     *  \param  ErrorSolvingFunction  Address of your application specific error handling function
     *  \return True   Application´s solving function was registered
     *          False  Application´s solving function was not registered (max number reached)
     */
    extern bool DBGX_registerErrorSolvingFunction(DBGE_ErrorSolvingFunction_t ErrorSolvingFunction);

    /** \brief  Initialize the error solving functions
     *  \note   Do not call this function in your application. Use the general DBGX_init()
     */
    extern void DBGE_init(void);

#else

    #define DBGX_registerErrorSolvingFunction(ErrorSolvingFunction)       (uint8_t)0

#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - INTERNAL CONFIGURATION                   */
/******************************************************************************/

/***************************
* Macro encapsulation for exceptional error handling at the error detection points,
* Access macros (do call them at the error detection points in the applications): */

/*lint -save -e9026 Macros encapsulate for configuration reasons; macros can be empty or can contain function calls. */

#ifdef DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED

    #ifdef DBGE_EXCEPTIONAL_ERROR_SOURCE_LOCATION_ACTIVATED

        /******************************
         * Macro encapsulation for exceptional error functions with source location.
         * Use this encapsulation here for your application.
         * A detailed parameter description can be found at the DBGX_handle..._Gen declaration. */

        /** Puts out an exceptional error via the debug_extended interface and saves it in the
         *  exceptional error memory, without error parameter, without pre-condition */
        #define DBGX_handleErr(ErrorCode)                                        \
            DBGX_writeErrorSourceLocation_Gen(__FILE__, __LINE__);               \
            DBGX_handleErr_Gen(ErrorCode)

        /** Puts out an exceptional error via the debug_extended interface and saves it in the
         * exceptional error memory, with error parameter, without pre-condition */
        #define DBGX_handleErrWithParam(ErrorCode, ErrorParameter)               \
            DBGX_writeErrorSourceLocation_Gen(__FILE__, __LINE__);               \
            DBGX_handleErrWithParam_Gen(ErrorCode, ErrorParameter)

        /** Puts out an exceptional error via the debug_extended interface and saves it in the
         * exceptional error memory, without error parameter, if pre-condition is fulfilled */
        #define DBGX_handleErrIf(Condition, ErrorCode)                            \
            DBGX_writeErrorSourceLocationCond_Gen(Condition, __FILE__, __LINE__); \
            DBGX_handleErrIf_Gen(Condition, ErrorCode)

        /** Puts out an exceptional error via the debug_extended interface and saves it in the
         * exceptional error memory, with error parameter, if pre-condition is fulfilled */
        #define DBGX_handleErrWithParamIf(Condition, ErrorCode, ErrorParameter)   \
            DBGX_writeErrorSourceLocationCond_Gen(Condition, __FILE__, __LINE__); \
            DBGX_handleErrWithParamIf_Gen(Condition, ErrorCode, ErrorParameter)

    #else /* From: ifdef DBGE_EXCEPTIONAL_ERROR_SOURCE_LOCATION_ACTIVATED */

        /******************************
         * Macro encapsulation for exceptional error functions with source location.
         * Use this encapsulation here for your application.
         * A detailed parameter description can be found at the DBGX_handle..._Gen declaration. */

        /** Puts out an exceptional error via the debug_extended interface and saves it in the exceptional
         *  error memory, without error parameter, without pre-condition */
        #define DBGX_handleErr(ErrorCode) \
            DBGX_handleErr_Gen(ErrorCode)

        /** Puts out an exceptional error via the debug_extended interface and saves it in the exceptional
         *  error memory, with error parameter, without pre-condition */
        #define DBGX_handleErrWithParam(ErrorCode, ErrorParameter) \
            DBGX_handleErrWithParam_Gen(ErrorCode, ErrorParameter)

        /** Puts out an exceptional error via the debug_extended interface and saves it in the exceptional
        *   error memory, without error parameter, if pre-condition is fulfilled */
        #define DBGX_handleErrIf(Condition, ErrorCode) \
            DBGX_handleErrIf_Gen(Condition, ErrorCode)

        /** Puts out an exceptional error via the debug_extended interface and saves it in the exceptional
         *  error memory, with error parameter, if pre-condition is fulfilled */
        #define DBGX_handleErrWithParamIf(Condition, ErrorCode, ErrorParameter) \
            DBGX_handleErrWithParamIf_Gen(Condition, ErrorCode, ErrorParameter)

    #endif  /* From: ifdef DBGE_EXCEPTIONAL_ERROR_SOURCE_LOCATION_ACTIVATED */

    /******************************
     * Macro encapsulation for reading exceptional errors.
     * Use this encapsulation here for your application.
     * A detailed parameter description can be found at the DBGX_read..._Gen declaration. */

    /** Reads the first occurred exceptional error after reset */
    #define DBGX_readFirstError(ErrorCode, ErrorParameter) \
            DBGX_readFirstError_Gen(ErrorCode, ErrorParameter)

    /** Reads the last occurred exceptional error after reset */
    #define DBGX_readLastError(ErrorCode, ErrorParameter) \
        DBGX_readLastError_Gen(ErrorCode, ErrorParameter)

#else /* From: ifdef DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED */

    /******************************
     * End macro encapsulation in case of deactivated exceptional error functions. */

    #define DBGX_handleErr(ErrorCode)                                          (uint8_t)0
    #define DBGX_handleErrWithParam(ErrorCode, ErrorParameter)                 (uint8_t)0
    #define DBGX_handleErrIf(Condition, ErrorCode)                             (uint8_t)0
    #define DBGX_handleErrWithParamIf(Condition, ErrorCode, ErrorParameter)    (uint8_t)0
    #define DBGX_readFirstError(ErrorCode, ErrorParameter)                     (uint8_t)0
    #define DBGX_readLastError(ErrorCode, ErrorParameter)                      (uint8_t)0

#endif /* From: ifdef DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED */

/*lint -restore -e9026*/


#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_API_ERROR_CFG_H */

