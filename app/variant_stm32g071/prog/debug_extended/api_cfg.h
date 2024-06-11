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

#ifndef DEBUG_EXTENDED_API_CFG_H
#define DEBUG_EXTENDED_API_CFG_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of configuration part of the
 *            debug_extended component: API layer
 *
 *  \details  See also FNG_DebugExtendedDataUsersManual.md
 *            The API related layer provides the following:
 *            - For debug_extended output data:
 *              * Access for putting out of debug_extended data
 *              * Filtering of debug_extended output data
 *            - For debug_extended input data:
 *              * Access for putting in of debug_extended data
 *              * Callback management of debug_extended input data for application
 *                protocol (e.g. touch)
 *            - For exceptional errors (see api_error_cfg.h):
 *              * Access for putting out of exceptional errors
 *              * Optional linking to exceptional error solving function of application
 *              * Access for reading of exceptional errors from the error memory
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "debug_extended/api.h"
#include "debug_extended/filters_aut.h"
#include "debug_extended/user_cfg.h"
#include "debug_extended/protocol_cfg.h"
#include "debug_extended/api_error_cfg.h"
#include "sts_sim_test_uty.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - ACTIVATION SWITCHES                      */
/******************************************************************************/

/* Do not change the activation switches here. Use the global switches from user_cfg.h  */
#ifdef DBGX_USR_TEXT_DAT_FUNCTIONS_ENABLED
    #define DBGX_TEXT_DAT_FUNCTIONS_ENABLED
#endif

#ifdef DBGX_USR_BIN_DAT_FUNCTIONS_ENABLED
    #define DBGX_BIN_DAT_FUNCTIONS_ENABLED
#endif

#ifdef DBGX_USR_DBG_ATTRIBUTES_ENABLED
    #define DBGX_DBG_ATTRIBUTES_ENABLED
#endif


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - USER CONFIGURATION                       */
/******************************************************************************/

#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
    /******************************
    * For Tx or Rx debug_extended data, e.g. for touch: */

    /* Module IDs on PC for Rx and Tx in microcontroller */
    /*   (coded in 1st half byte in the frame ID to PC / from PC): */

    #define DBGX_MODULE_TOUCH_STREAM_OUT      0x0000  // For debug_extended protocol of touch GUI on PC
    #define DBGX_MODULE_TEMPERATURE           0x0001
    #define DBGX_MODULE_MOTOR_CONTROL         0x0002
    #define DBGX_MODULE_3                     0x0003  // Available and renameable for any module
    #define DBGX_MODULE_SYSTEM_MASTER         0x0004
    #define DBGX_MODULE_TOUCH_RW_MSG          0x0005  // For debug_extended protocol of touch GUI on PC
    #define DBGX_MODULE_6                     0x0006  // Available and renameable for any module
    #define DBGX_MODULE_7                     0x0007  // Available and renameable for any module
    #define DBGX_MODULE_8                     0x0008  // Available and renameable for any module
    #define DBGX_MODULE_9                     0x0009  // Available and renameable for any module
    #define DBGX_MODULE_A                     0x000A  // Available and renameable for any module
    #define DBGX_MODULE_B                     0x000B  // Available and renameable for any module
    #define DBGX_MODULE_C                     0x000C  // Available and renameable for any module
    #define DBGX_MODULE_D                     0x000D  // Available and renameable for any module
    #define DBGX_MODULE_TEXT                  0x000E  // Text application modul (functions DBGX_log...)
    #define DBGX_MODULE_FURTHER               0x000F  // Base ID for future modules
                                                      //   (offset ID in payload of touch derived message frame)

    /* Data functions on PC for Tx or Rx in microcontroller */
    /*   (coded in 2nd half byte in the frame ID to PC / from PC): */

    #define DBGX_FUNCTION_TOUCH_STREAM_OUT_1  0x0000  // For debug_extended protocol of touch GUI on PC
    #define DBGX_FUNCTION_TOUCH_R_REQ_RESP    0x0002  // For debug_extended protocol of touch GUI on PC
    #define DBGX_FUNCTION_TOUCH_W_REQ_RESP    0x0007  // For debug_extended protocol of touch GUI on PC

    /* Module abbreviations in the debug_extended output: */
    /*   (with DBGX_MODULE_... as index) */
    #define DBGX_MODULE_ABBREVIATIONS \
    {                                 \
        (char)'T', (char)'C',         \
        (char)'T', (char)'E',         \
        (char)'M', (char)'L',         \
        (char)'M', (char)'3',         \
        (char)'S', (char)'M',         \
        (char)'T', (char)'C',         \
        (char)'M', (char)'6',         \
        (char)'M', (char)'7',         \
        (char)'M', (char)'8',         \
        (char)'M', (char)'9',         \
        (char)'M', (char)'A',         \
        (char)'M', (char)'B',         \
        (char)'M', (char)'C',         \
        (char)'M', (char)'D',         \
        (char)'T', (char)'T',         \
        (char)'F', (char)'U'          \
    }

    /******************************
    * For the callback functions for Rx debug_extended data, e.g. for touch: */

    /* Number of modules to be debugged with own callback function for Rx debug_extended data each: */
    /* That is the number of data modules, that can be notified by application: */
    /* (Is the same as the number of receivers, e.g. touch, temperature regulation, ...) */
    #define DBGX_MAX_NUMBER_MODULES_RX   5U

    /* Composition of the data module IDs (for array initialization): */
    /* (Only fill in the modules with Rx debug_extended data here) */
    #define DBGX_MODULE_IDS        \
    {                              \
        DBGX_MODULE_TOUCH_RW_MSG,  \
        DBGX_MODULE_TEMPERATURE,   \
        DBGX_MODULE_SYSTEM_MASTER, \
        DBGX_MODULE_TEXT,          \
        DBGX_MODULE_MOTOR_CONTROL  \
    }

    /* Initial composition of the Rx functions of the modules: */
    #define DBGX_RECEIVE_FUNCTIONS_OF_MODULES_INIT \
    {                                              \
        NULL,                                      \
        NULL,                                      \
        NULL,                                      \
        NULL,                                      \
        NULL                                       \
    }

#endif /* From: ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED */


/******************************
* Input callback functions */

#ifdef DBGX_INPUT_FUNCTIONS_ENABLED
    /* Maximum number of the user defined input callback functions */
    #define DBGX_LIMIT_OF_INPUT_CALLBACK_FUNCTIONS           (uint8_t)5
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS - INTERNAL CONFIGURATION                   */
/******************************************************************************/

#define DBGX_LEN_OF_MODULE_ABBREVIATION  (uint8_t)2

/******************************
 * The following table shows all usable options for the parameter "format".
 * It is relevant for all api functions writing integer values, e.g. DBGX_logInt, DBGX_logIntArr, ...
 *
 *       Format parameter          |                  description
 * --------------------------------|-----------------------------------------------------------------------
 *                                 |
 *   DBGX_UNSIGNED_DUAL            |      unsigned input     and     output representation with base 2
 *   DBGX_UNSIGNED_OCTAL           |      unsigned input     and     output representation with base 8
 *   DBGX_UNSIGNED_DECIMAL         |      unsigned input     and     output representation with base 10
 *   DBGX_UNSIGNED_HEXADECIMAL     |      unsigned input     and     output representation with base 16
 *                                 |
 *   DBGX_SIGNED_DUAL              |        signed input     and     output representation with base 2
 *   DBGX_SIGNED_OCTAL             |        signed input     and     output representation with base 8
 *   DBGX_SIGNED_DECIMAL           |        signed input     and     output representation with base 10
 *   DBGX_SIGNED_HEXADECIMAL       |        signed input     and     output representation with base 16
 */


/******************************
* Macro encapsulation for writing functions of debug_extended data (DBGX_log...) */

/*lint -save -e9026 Macros encapsulate for configuration reasons; macros can be empty or can contain function calls. */
/*lint -save -e835 -e845 The left/right operand of some macro encapsulations in the logInt functions can result in 0. */
/*AXIVION Disable Style MisraC2012-20.7 : Macro parameter not enclosed in parentheses */

#ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED

    /******************************
     * Macro encapsulation for filtered api functions.
     * Use the encapsulation function from filter_aut.h for your application, not any functions in the encapsulation.
     * A detailed parameter description can be found at the DBGX_log..._Gen declaration. */

    #define DBGX_logChar(CallingFilter, Char) \
        DBGX_logChar_Cfg(CallingFilter, Char)

    #define DBGX_logStr(CallingFilter, String) \
        DBGX_logStr_Cfg(CallingFilter, String)

    #define DBGX_logInt(CallingFilter, Integer, Format) \
        DBGX_logInt_Cfg(CallingFilter, (&Integer), (Format | DBGX_ADD_INT_LEN_TO_FORMAT(Integer)))

    #define DBGX_logIntArr(CallingFilter, IntegerArrayAddress, NumberOfArrayElements, Format) \
        DBGX_logIntArr_Cfg(CallingFilter, IntegerArrayAddress, NumberOfArrayElements, (Format | DBGX_ADD_INT_LEN_TO_FORMAT(IntegerArrayAddress[0])))

    #define DBGX_logTime(CallingFilter) \
        DBGX_logTime_Cfg(CallingFilter)

    #define DBGX_logLocat(CallingFilter) \
        DBGX_logLocat_Cfg(CallingFilter, __FILE__, __LINE__)

    #define DBGX_logStrStr(CallingFilter, StringA, StringB) \
        DBGX_logStrStr_Cfg(CallingFilter, StringA, StringB)

    #define DBGX_logStrInt(CallingFilter, String, Integer, Format) \
        DBGX_logStrInt_Cfg(CallingFilter, String, (&Integer), (Format | DBGX_ADD_INT_LEN_TO_FORMAT(Integer)))

    #define DBGX_logStrIntArr(CallingFilter, String, IntegerArrayAddress, NumberOfArrayElements, Format) \
        DBGX_logStrIntArr_Cfg(CallingFilter, String, IntegerArrayAddress, NumberOfArrayElements, (Format | DBGX_ADD_INT_LEN_TO_FORMAT(IntegerArrayAddress[0])))

    #define DBGX_logStrStrLocat(CallingFilter, StringA, StringB) \
        DBGX_logStrStrLocat_Cfg(CallingFilter, StringA, StringB, __func__)

    #define DBGX_logStrStrLocatFull(CallingFilter, StringA, StringB) \
        DBGX_logStrStrLocatFull_Cfg(CallingFilter, StringA, StringB, __FILE__, __LINE__, __func__)

    #define DBGX_logStrIntLocat(CallingFilter, String, Integer, Format) \
        DBGX_logStrIntLocat_Cfg(CallingFilter, String, (&Integer), (Format | DBGX_ADD_INT_LEN_TO_FORMAT(Integer)), __func__)

    #define DBGX_logStrIntLocatFull(CallingFilter, String, Integer, Format) \
        DBGX_logStrIntLocatFull_Cfg(CallingFilter, String, (&Integer), (Format | DBGX_ADD_INT_LEN_TO_FORMAT(Integer)),  __FILE__, __LINE__, __func__)


    /******************************
     * Macro encapsulation for non-filtered api functions.
     * Use this encapsulation here for your application, not any functions from the inside of the encapsulation.
     * A detailed parameter description can be found at the DBGX_log..._Gen declaration. */

    #define DBGX_logChar_NonFiltered(Char) \
        DBGX_logChar_NonFiltered_Gen(Char)

    #define DBGX_logStr_NonFiltered(String) \
        DBGX_logStr_NonFiltered_Gen(String)

    #define DBGX_logInt_NonFiltered(Integer, Format) \
        DBGX_logInt_NonFiltered_Gen((&Integer), (Format | DBGX_ADD_INT_LEN_TO_FORMAT(Integer)))

    #define DBGX_logIntArr_NonFiltered(IntegerArrayAddress, NumberOfArrayElements, Format) \
        DBGX_logIntArr_NonFiltered_Gen(IntegerArrayAddress, NumberOfArrayElements, (Format | DBGX_ADD_INT_LEN_TO_FORMAT(IntegerArrayAddress[0])))

    #define DBGX_logTime_NonFiltered() \
        DBGX_logTime_NonFiltered_Gen()

    #define DBGX_logLocat_NonFiltered() \
        DBGX_logLocat_NonFiltered_Gen(__FILE__, __LINE__)

    #define DBGX_logStrStr_NonFiltered(StringA, StringB) \
        DBGX_logStrStr_NonFiltered_Gen(StringA, StringB)

    #define DBGX_logStrInt_NonFiltered(String, Integer, Format) \
        DBGX_logStrInt_NonFiltered_Gen(String, (&Integer), (Format | DBGX_ADD_INT_LEN_TO_FORMAT(Integer)))

    #define DBGX_logStrIntArr_NonFiltered(String, IntegerArrayAddress, NumberOfArrayElements, Format) \
        DBGX_logStrIntArr_NonFiltered_Gen(String, IntegerArrayAddress, NumberOfArrayElements, (Format | DBGX_ADD_INT_LEN_TO_FORMAT(IntegerArrayAddress[0])))

    #define DBGX_logStrStrLocat_NonFiltered(StringA, StringB) \
        DBGX_logStrStrLocat_NonFiltered_Gen(StringA, StringB, __func__)

    #define DBGX_logStrStrLocatFull_NonFiltered(StringA, StringB) \
        DBGX_logStrStrLocatFull_NonFiltered_Gen(StringA, StringB, __FILE__, __LINE__, __func__)

    #define DBGX_logStrIntLocat_NonFiltered(String, Integer, Format) \
        DBGX_logStrIntLocat_NonFiltered_Gen(String, (&Integer), (Format | DBGX_ADD_INT_LEN_TO_FORMAT(Integer)), __func__)

    #define DBGX_logStrIntLocatFull_NonFiltered(String, Integer, Format) \
        DBGX_logStrIntLocatFull_NonFiltered_Gen(String, (&Integer), (Format | DBGX_ADD_INT_LEN_TO_FORMAT(Integer)), __FILE__, __LINE__, __func__)

#else /* From: ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED */

    /******************************
     * End macro encapsulation in case of deactivated text protocol. */

    #define DBGX_logChar(CallingFilter, Char)
    #define DBGX_logStr(CallingFilter, String)
    #define DBGX_logInt(CallingFilter, Integer, Format)
    #define DBGX_logIntArr(CallingFilter, IntegerArrayAddress, NumberOfArrayElements, Format)
    #define DBGX_logTime(CallingFilter)
    #define DBGX_logLocat(CallingFilter)
    #define DBGX_logStrStr(CallingFilter, StringA, StringB)
    #define DBGX_logStrInt(CallingFilter, String, Integer, Format)
    #define DBGX_logStrIntArr(CallingFilter, String, IntegerArrayAddress, NumberOfArrayElements, Format)
    #define DBGX_logStrStrLocat(CallingFilter, StringA, StringB)
    #define DBGX_logStrStrLocatFull(CallingFilter, StringA, StringB)
    #define DBGX_logStrIntLocat(CallingFilter, String, Integer, Format)
    #define DBGX_logStrIntLocatFull(CallingFilter, String, Integer, Format)

    #define DBGX_logChar_NonFiltered(Char)
    #define DBGX_logStr_NonFiltered(String)
    #define DBGX_logInt_NonFiltered(Integer, Format)
    #define DBGX_logIntArr_NonFiltered(IntegerArrayAddress, NumberOfArrayElements, Format)
    #define DBGX_logTime_NonFiltered()
    #define DBGX_logLocat_NonFiltered()
    #define DBGX_logStrStr_NonFiltered(StringA, StringB)
    #define DBGX_logStrInt_NonFiltered(String, Integer, Format)
    #define DBGX_logStrIntArr_NonFiltered(String, IntegerArrayAddress, NumberOfArrayElements, Format)
    #define DBGX_logStrStrLocat_NonFiltered(StringA, StringB)
    #define DBGX_logStrStrLocatFull_NonFiltered(StringA, StringB)
    #define DBGX_logStrIntLocat_NonFiltered(String, Integer, Format)
    #define DBGX_logStrIntLocatFull_NonFiltered(String, Integer, Format)

#endif /* From: ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED */


#ifdef DBGX_INPUT_FUNCTIONS_ENABLED

    /** \brief  Receives a string via the debug_extended interface
     *  \note   Use this encapsulation here for your application, not any
     *          functions from the inside of the encapsulation
     *  \param  StringAddress  Address of the allocated memory, the input string will be saved into
     *  \param  StringLen      Length of the allocated array, the input string will be saved into
     */
    #define DBGX_getString(StringAddress, StringLen) \
        DBGX_getString_Gen(StringAddress, StringLen)

#else

    #define DBGX_getString(StringAddress, StringLen)                       (uint8_t)0
    #define DBGX_registerInputCallbackFunction(InputCallbackFunction)      (uint8_t)0

#endif

/*lint -restore -e835 -e845 -e9026 */
/*AXIVION Enable Style MisraC2012-20.7 */


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

#if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED)
    /* structure of the filter declarations in the applications */
    struct DBGX_Filter_s
    {
        uint8_t  NumberFilterFlagWords;
        uint32_t FilterFlagWords[DBGX_NUMBER_FILTER_FLAG_WORDS];
    }; /* Do not use it as const, because filters can be dynamically overwritten */
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED) || defined(DBGE_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED)
    /** \brief  Initializes the debug_extended API
     *  \note   Use this function here for your application
     */
    extern void DBGX_init(void);
#endif

#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
    /** \brief  Sends the data of the application protocol (e.g. touch data)
     *          via the debug_extended interface, filtered
     *  \note   Use this function here for your application
     *
     *  \param  Module_ID      ID of the module (e.g touch), needed for the PC GUI tool
     *  \param  CallingFilter  Debug_extended output data filter settings at the calling location
     *  \param  Function       Function of the debug_extended output data, needed for the PC GUI tool
     *  \param  Data           Pointer to payload (user data) of debug_extended output
     *                         data for the PC GUI tool
     *  \param  DataLen        Number of bytes of payload (user data) of debug_extended
     *                         output data for the PC GUI tool
     *  \return Error status (==0: no error; < 0: error)
     */
    extern int32_t DBGX_sendData(uint16_t Module_ID, uint32_t CallingFilter, uint16_t Function, uint8_t *Data, uint16_t DataLen);
#endif

#ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED
    /** \brief  Wrapper for sending a character via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logChar in your application
     *  \sa     DBGX_logChar_Gen
     */
    extern void DBGX_logChar_Cfg(uint32_t CallingFilter, char Char);

    /** \brief  Wrapper for sending a string via the debug_extended interface, NULL-termination
     *          character not included, filtered
     *  \note   Use the encapsulating DBGX_logStr in your application
     *  \sa     DBGX_logStr_Gen
     */
    extern void DBGX_logStr_Cfg(uint32_t CallingFilter, const char *String);

    /** \brief  Wrapper for sending an integer value as characters via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logInt in your application
     *  \sa     DBGX_logInt_Gen
     */
    extern void DBGX_logInt_Cfg(uint32_t CallingFilter, const void *Int, uint32_t Format);

    /** \brief  Wrapper for sending an array of integer values as characters via the debug_extended
     *          interface, array elements separated by space, filtered
     *  \note   Use the encapsulating DBGX_logIntArr in your application
     *  \sa     DBGX_logIntArr_Gen
     */
    extern void DBGX_logIntArr_Cfg(uint32_t CallingFilter, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format);

    /** \brief  Wrapper for sending a decimal time stamp as characters via the debug_extended
     *          interface, in microseconds, filtered
     *  \note   Use the encapsulating DBGX_logTime in your application
     *  \sa     DBGX_logTime_Gen
     */
    extern void DBGX_logTime_Cfg(uint32_t CallingFilter);

    /** \brief  Wrapper for sending the source location as characters via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logLocat in your application
     *  \sa     DBGX_logLocat_Gen
     */
    extern void DBGX_logLocat_Cfg(uint32_t CallingFilter, const char *File, uint32_t Line);

    /** \brief  Wrapper for sending a concatenation of two strings via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logStrStr in your application
     *  \sa     DBGX_logStrStr_Gen
     */
    extern void DBGX_logStrStr_Cfg(uint32_t CallingFilter, const char *StringA, const char *StringB);

    /** \brief  Wrapper for sending a concatenation of a string and an integer via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logStrInt in your application
     *  \sa     DBGX_logStrInt_Gen
     */
    extern void DBGX_logStrInt_Cfg(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format);

    /** \brief  Wrapper for sending a concatenation of a string and an integer array via the debug_extended
     *          interface, filtered
     *  \note   Use the encapsulating DBGX_logStrIntArr in your application
     *  \sa     DBGX_logStrIntArr_Gen
     */
    extern void DBGX_logStrIntArr_Cfg(uint32_t CallingFilter, const char *String, const void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format);

    /** \brief  Wrapper for sending a concatenation of two strings and the source location (just function name)
     *          as characters via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logStrStrLocat in your application
     *  \sa     DBGX_logStrStrLocat_Gen
     */
    extern void DBGX_logStrStrLocat_Cfg(uint32_t CallingFilter, const char *StringA, const char *StringB, const char *Function);

    /** \brief  Wrapper for sending a concatenation of two strings and the source location
     *          (filename, line number, function name) as characters via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logStrStrLocatFull in your application
     *  \sa     DBGX_logStrStrLocatFull_Gen
     */
    extern void DBGX_logStrStrLocatFull_Cfg(uint32_t CallingFilter, const char *StringA, const char *StringB, const char *File, uint32_t Line, const char *Function);

    /** \brief  Wrapper for sending a concatenation of a string, an integer and the source location (just function name)
     *          as characters via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logStrIntLocat in your application
     *  \sa     DBGX_logStrIntLocat_Gen
     */
    extern void DBGX_logStrIntLocat_Cfg(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format, const char *Function);

    /** \brief  Wrapper for sending a concatenation of a string, an integer and the source location
     *          (filename, line number, function name) as characters via the debug_extended interface, filtered
     *  \note   Use the encapsulating DBGX_logStrIntLocatFull in your application
     *  \sa     DBGX_logStrIntLocatFull_Gen
     */
    extern void DBGX_logStrIntLocatFull_Cfg(uint32_t CallingFilter, const char *String, const void *IntegerAddress, uint32_t Format, const char *File, uint32_t Line, const char *Function);
#endif /* From: ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED */


#ifdef DBGX_INPUT_FUNCTIONS_ENABLED
    /** \brief  Register your callback function to the interface input mechanism
     *  \note   Call this function in each module, you want to use error solving
     *
     *  \param  InputCallbackFunction  The address of your application specific input handling function
     *  \return True, if the application´s function was registered
     *          False, if the application´s function was not registered (max number of solving functions reached)
     */
    extern bool DBGX_registerInputCallbackFunction(DBGX_InputCallbackFunctions_t InputCallbackFunction);
#endif


#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_API_CFG_H */

