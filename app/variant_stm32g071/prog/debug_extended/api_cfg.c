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
 *             debug_extended component: API layer
 *
 *  \details   See also FNG_DebugExtendedDataUsersManual.md
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdint.h>
#include "bsh_stdinc.h"

#include "debug_extended/user_cfg.h"
#include "debug_extended/interface.h"


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
    /* Callback functions of debug_extended rx-data and their module IDs */
    /* Each receive function has exactly one module, each module has exactly one receive function. */
    struct DBGX_ApiCallbacksCfg_s
    {
        DBGX_ReceiveDataFunctionOfApi_t       ReceiveDataFunctionsOfApi_CB[DBGX_MAX_NUMBER_MODULES_RX];
        uint16_t                              ModuleIDs[DBGX_MAX_NUMBER_MODULES_RX];
    };
#endif

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE DATA DECLARATIONS                                                  */
/******************************************************************************/

#if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED) || defined(DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED)
    static bool DBGX_Initialized = false;  // Redundancy check for DBGX_init()
#endif

#if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED)
    /* Filter reference in the configuration area (array of central configuration switches (flags)) */
    //lint -e778 binary operation may result in 0 --> ok here
    static struct DBGX_Filter_s DBGX_CentralConfigFilter_Cfg = {
        DBGX_NUMBER_FILTER_FLAG_WORDS,
        DBGX_FILTER_FLAG_WORDS_CENTRAL_CONFIG
    };

    /* Filter reference in the generic area (pointer to array of central switches (flags)) */
    static struct DBGX_FilterGen_s DBGX_CentralConfigFilter_Gen = {
        DBGX_NUMBER_FILTER_FLAG_WORDS,
        DBGX_CentralConfigFilter_Cfg.FilterFlagWords
    };
#endif

#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
    /* Callback functions of debug_extended Rx-data with their module IDs in the configuration area */
    static struct DBGX_ApiCallbacksCfg_s DBGX_ApiCallbacks_Cfg = {
        DBGX_RECEIVE_FUNCTIONS_OF_MODULES_INIT,
        DBGX_MODULE_IDS
    };

    /* Callback functions of debug_extended Rx-data with their module IDs in the generic area (pointer to array of functions) */
    static const struct DBGX_ApiCallbacksGen_s DBGX_ApiCallbacks_Gen = {/*lint !e9003 Better not static local for easier debugging and testing */
        DBGX_MAX_NUMBER_MODULES_RX,
        DBGX_ApiCallbacks_Cfg.ReceiveDataFunctionsOfApi_CB,
        DBGX_ApiCallbacks_Cfg.ModuleIDs
    };
#endif

#ifdef DBGX_INPUT_FUNCTIONS_ENABLED
    static DBGX_InputCallbackFunctions_t DBGX_InputCallbackFunctions_Cfg[DBGX_LIMIT_OF_INPUT_CALLBACK_FUNCTIONS];
    static uint8_t DBGX_NumberOfInputCallbackFunctions = 0;
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
#if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED) || defined(DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED)
    void DBGX_init(void)
    {
        if(DBGX_Initialized == false)
        {
            DBGX_Initialized = true;

            #ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
                static const char DBGX_ModuleAbbreviations_Cfg[] = DBGX_MODULE_ABBREVIATIONS;

                for(uint16_t RxModuleIndex = 0U ; RxModuleIndex < DBGX_MAX_NUMBER_MODULES_RX ; RxModuleIndex++)
                {
                    DBGX_ApiCallbacks_Cfg.ReceiveDataFunctionsOfApi_CB[RxModuleIndex] = NULL;
                }

                DBGX_initDebugExtendedAPI_ForCallbacks(&DBGX_ApiCallbacks_Gen);
                DBGX_initModuleAbbreviations( DBGX_ModuleAbbreviations_Cfg,
                                            (uint8_t)sizeof(DBGX_ModuleAbbreviations_Cfg) / DBGX_LEN_OF_MODULE_ABBREVIATION,
                                            DBGX_LEN_OF_MODULE_ABBREVIATION);
            #endif

            #if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED)
                DBGX_initDebugExtendedAPI_ForFilter(&DBGX_CentralConfigFilter_Gen);
                DBGP_initProtocols_Cfg();
                DBGI_initInterfaces_Cfg();
                #ifdef DBGX_DBG_ATTRIBUTES_ENABLED
                    DBGX_activateDbgAttributes();
                #endif
            #endif

            #ifdef DBGX_INPUT_FUNCTIONS_ENABLED
                DBGX_setInputCallbackFunctions(DBGX_InputCallbackFunctions_Cfg, &DBGX_NumberOfInputCallbackFunctions, DBGX_LIMIT_OF_INPUT_CALLBACK_FUNCTIONS);
            #endif

            #ifdef DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED
                DBGE_init();
            #endif
        }
    }
#endif /* From: if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED) || defined(DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED) */

#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
    int32_t DBGX_sendData(uint16_t Module_ID, uint32_t CallingFilter, uint16_t Function, uint8_t *Data, uint16_t DataLen)
    {
        int32_t ErrorStatus;
        ErrorStatus = DBGX_sendData_Gen(Module_ID, CallingFilter, Function, Data, DataLen);
        return ErrorStatus;
    }
#endif

#ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED
    void DBGX_logChar_Cfg(uint32_t CallingFilter, char Char)
    {
        DBGX_logChar_Gen(CallingFilter, Char);
    }

    void DBGX_logStr_Cfg(uint32_t CallingFilter, const char *String)
    {
        DBGX_logStr_Gen(CallingFilter, String);
    }

    void DBGX_logInt_Cfg(uint32_t CallingFilter, const void *Int, uint32_t Format)
    {
        DBGX_logInt_Gen(CallingFilter, Int, Format);
    }

    void DBGX_logIntArr_Cfg(uint32_t CallingFilter, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format)
    {
        DBGX_logIntArr_Gen(CallingFilter, IntArray, NumberOfArrayElements, Format);
    }

    void DBGX_logTime_Cfg(uint32_t CallingFilter)
    {
        DBGX_logTime_Gen(CallingFilter);
    }

    void DBGX_logLocat_Cfg(uint32_t CallingFilter, const char *File, uint32_t Line)
    {
        DBGX_logLocat_Gen(CallingFilter, File, Line);
    }

    void DBGX_logStrStr_Cfg(uint32_t CallingFilter, const char *StringA, const char *StringB)
    {
        DBGX_logStrStr_Gen(CallingFilter, StringA, StringB);
    }

    void DBGX_logStrInt_Cfg(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format)
    {
        DBGX_logStrInt_Gen(CallingFilter, String, IntegerAddress, Format);
    }

    void DBGX_logStrIntArr_Cfg(uint32_t CallingFilter, const char *String, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format)
    {
        DBGX_logStrIntArr_Gen(CallingFilter, String, IntArray, NumberOfArrayElements, Format);
    }

    void DBGX_logStrStrLocat_Cfg(uint32_t CallingFilter, const char *StringA, const char *StringB, const char *Function)
    {
        DBGX_logStrStrLocat_Gen(CallingFilter, StringA, StringB, Function);
    }

    void DBGX_logStrStrLocatFull_Cfg(uint32_t CallingFilter, const char *StringA, const char *StringB, const char *File, uint32_t Line, const char *Function)
    {
        DBGX_logStrStrLocatFull_Gen(CallingFilter, StringA, StringB, File, Line, Function);
    }

    void DBGX_logStrIntLocat_Cfg(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format, const char *Function)
    {
        DBGX_logStrIntLocat_Gen(CallingFilter, String, IntegerAddress, Format, Function);
    }

    void DBGX_logStrIntLocatFull_Cfg(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format, const char *File, uint32_t Line, const char *Function)
    {
        DBGX_logStrIntLocatFull_Gen(CallingFilter, String, IntegerAddress, Format, File, Line, Function);
    }
#endif /* ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED */

#ifdef DBGX_INPUT_FUNCTIONS_ENABLED
    bool DBGX_registerInputCallbackFunction(DBGX_InputCallbackFunctions_t InputCallbackFunction)
    {
        if(DBGX_NumberOfInputCallbackFunctions < DBGX_LIMIT_OF_INPUT_CALLBACK_FUNCTIONS)
        {
            DBGX_InputCallbackFunctions_Cfg[DBGX_NumberOfInputCallbackFunctions] = InputCallbackFunction;
            DBGX_NumberOfInputCallbackFunctions ++;
            return true;
        }
        else
        {
            return false;
        }
    }
#endif /* ifdef DBGX_INPUT_FUNCTIONS_ENABLED */

