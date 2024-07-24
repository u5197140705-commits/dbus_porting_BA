/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
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

#ifndef DEBUG_EXTENDED_FILTERS_AUT_H
#define DEBUG_EXTENDED_FILTERS_AUT_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Automatic generated filter definitions for the debug_extended component
 *
 *  \details  This file is generated each time calling make
 */


/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/

/*AXIVION Disable Style MisraC2012-20.7 : Macro parameter not enclosed in parentheses */
/*AXIVION Disable Style MisraC2012-20.10 : The # and ## preprocessor operators should not be used */

/* The following block of definitions is used for all filtered DBGX_logInt functions to ensure better usability. 
   The macro encapsulation enables the ability to select the corresponding internal functions, according to the number of arguments. 
   E.g.: If format parameter is explicitly given, use function A, if default format parameter is to be used, use function B*/

// The following macros are used to select the function to be called, according from the number of arguments in the variadic argument.
#define DBGX_LOG_INT_GET_MACRO(firstParam, secondParam, NAME, ...)                          NAME
#define DBGX_LOG_INT_ARR_GET_MACRO(firstParam, secondParam, thirdParam, NAME, ...)          NAME


// The following macros are used to build function calls for: filtered DBGX_logInt
#define DBGX_LOG_INT_DEFAULT_FORMAT(Name, Filter, Integer)                                  Name##_op_Wrapper (Filter, Integer)
#define DBGX_LOG_INT_EXPLICIT_FORMAT(Name, Filter, Integer, Format)                         Name##_Wrapper (Filter, Integer, Format)
#define DBGX_LOG_INT_VARIABLES(Name, Filter, ...)                                           DBGX_LOG_INT_GET_MACRO(__VA_ARGS__, DBGX_LOG_INT_EXPLICIT_FORMAT, DBGX_LOG_INT_DEFAULT_FORMAT)(Name, Filter, __VA_ARGS__)

// The following macros are used to build function calls for: filtered DBGX_logStrInt
#define DBGX_LOG_STR_INT_DEFAULT_FORMAT(Name, Filter, String, Integer)                      Name##_op_Wrapper (Filter, String, Integer)
#define DBGX_LOG_STR_INT_EXPLICIT_FORMAT(Name, Filter, String, Integer, Format)             Name##_Wrapper (Filter, String, Integer, Format)
#define DBGX_LOG_STR_INT_VARIABLES(Name, Filter, String, ...)                               DBGX_LOG_INT_GET_MACRO(__VA_ARGS__, DBGX_LOG_STR_INT_EXPLICIT_FORMAT, DBGX_LOG_STR_INT_DEFAULT_FORMAT)(Name, Filter, String, __VA_ARGS__)

// The following calls (logStrIntLocat, logStrIntLocatFull) are equal to filtered DBGX_logStrInt
#define DBGX_LOG_STR_INT_LOCAT_VARIABLES(Name, Filter, String, ...)                         DBGX_LOG_STR_INT_VARIABLES(Name, Filter, String, __VA_ARGS__)
#define DBGX_LOG_STR_INT_LOCAT_FULL_VARIABLES(Name, Filter, String, ...)                    DBGX_LOG_STR_INT_VARIABLES(Name, Filter, String, __VA_ARGS__)


// The following macros are used to build function calls for: filtered DBGX_logIntArr
#define DBGX_LOG_INT_ARR_DEFAULT_FORMAT(Name, Filter, IntegerArrayAddress, Number)              Name##_op_Wrapper (Filter, IntegerArrayAddress, Number, (uint8_t)sizeof(IntegerArrayAddress[0]))
#define DBGX_LOG_INT_ARR_EXPLICIT_FORMAT(Name, Filter, IntegerArrayAddress, Number, Format)     Name##_Wrapper (Filter, IntegerArrayAddress, Number, ((Format) | DBGX_ADD_INT_LEN_TO_FORMAT(IntegerArrayAddress[0])))
#define DBGX_LOG_INT_ARR_VARIABLES(Name, Filter, ...)                                           DBGX_LOG_INT_ARR_GET_MACRO(__VA_ARGS__, DBGX_LOG_INT_ARR_EXPLICIT_FORMAT, DBGX_LOG_INT_ARR_DEFAULT_FORMAT)(Name, Filter, __VA_ARGS__)

// The following macros are used to build function calls for: filtered DBGX_logStrIntArr
#define DBGX_LOG_STR_INT_ARR_DEFAULT_FORMAT(Name, Filter, String, IntegerArrayAddress, Number)            Name##_op_Wrapper (Filter, String, IntegerArrayAddress, Number, (uint8_t)sizeof(IntegerArrayAddress[0]))
#define DBGX_LOG_STR_INT_ARR_EXPLICIT_FORMAT(Name, Filter, String, IntegerArrayAddress, Number, Format)   Name##_Wrapper (Filter, String, IntegerArrayAddress, Number, ((Format) | DBGX_ADD_INT_LEN_TO_FORMAT(IntegerArrayAddress[0])))
#define DBGX_LOG_STR_INT_ARR_VARIABLES(Name, Filter, String, ...)                                         DBGX_LOG_INT_ARR_GET_MACRO(__VA_ARGS__, DBGX_LOG_STR_INT_ARR_EXPLICIT_FORMAT, DBGX_LOG_STR_INT_ARR_DEFAULT_FORMAT)(Name, Filter, String, __VA_ARGS__)

// The following calls (logStrIntLocat, logStrIntLocatFull) are equal to filtered DBGX_logStrIntArr
#define DBGX_LOG_STR_INT_ARR_LOCAT_VARIABLES(Name, Filter, String, ...)                         DBGX_LOG_STR_INT_ARR_VARIABLES(Name, Filter, String, __VA_ARGS__)
#define DBGX_LOG_STR_INT_ARR_LOCAT_FULL_VARIABLES(Name, Filter, String, ...)                    DBGX_LOG_STR_INT_ARR_VARIABLES(Name, Filter, String, __VA_ARGS__)


// The following macros are used to build function calls for: non-filtered DBGX_logInt
#define DBGX_LOG_INT_DEFAULT_FORMAT_NON_FILTERED(Name, Integer)                                  Name##_op_Wrapper ((uint32_t)Integer)
#define DBGX_LOG_INT_EXPLICIT_FORMAT_NON_FILTERED(Name, Integer, Format)                         Name##_Wrapper ((uint32_t)Integer, Format)
#define DBGX_LOG_INT_VARIABLES_NON_FILTERED(Name, ...)                                           DBGX_LOG_INT_GET_MACRO(__VA_ARGS__, DBGX_LOG_INT_EXPLICIT_FORMAT_NON_FILTERED, DBGX_LOG_INT_DEFAULT_FORMAT_NON_FILTERED)(Name, __VA_ARGS__)

// The following macros are used to build function calls for: non_filtered DBGX_logStrInt
#define DBGX_LOG_STR_INT_DEFAULT_FORMAT_NON_FILTERED(Name, String, Integer)                      Name##_op_Wrapper (String, Integer)
#define DBGX_LOG_STR_INT_EXPLICIT_FORMAT_NON_FILTERED(Name, String, Integer, Format)             Name##_Wrapper (String, Integer, Format)
#define DBGX_LOG_STR_INT_VARIABLES_NON_FILTERED(Name, String, ...)                               DBGX_LOG_INT_GET_MACRO(__VA_ARGS__, DBGX_LOG_STR_INT_EXPLICIT_FORMAT_NON_FILTERED, DBGX_LOG_STR_INT_DEFAULT_FORMAT_NON_FILTERED)(Name, String, __VA_ARGS__)

// The following calls (logStrIntLocat, logStrIntLocatFull) are equal to non_filtered DBGX_logStrInt
#define DBGX_LOG_STR_INT_LOCAT_VARIABLES_NON_FILTERED(Name, String, ...)                         DBGX_LOG_STR_INT_VARIABLES_NON_FILTERED(Name, String, __VA_ARGS__)
#define DBGX_LOG_STR_INT_LOCAT_FULL_VARIABLES_NON_FILTERED(Name, String, ...)                    DBGX_LOG_STR_INT_VARIABLES_NON_FILTERED(Name, String, __VA_ARGS__)


// The following macros are used to build function calls for: non_filtered DBGX_logInt
#define DBGX_LOG_INT_ARR_DEFAULT_FORMAT_NON_FILTERED(Name, IntegerArrayAddress, Number)              Name##_op_Wrapper (IntegerArrayAddress, Number, (uint8_t)sizeof(IntegerArrayAddress[0]))
#define DBGX_LOG_INT_ARR_EXPLICIT_FORMAT_NON_FILTERED(Name, IntegerArrayAddress, Number, Format)     Name##_Wrapper (IntegerArrayAddress, Number, ((Format) | DBGX_ADD_INT_LEN_TO_FORMAT(IntegerArrayAddress[0])))
#define DBGX_LOG_INT_ARR_VARIABLES_NON_FILTERED(Name, ...)                                           DBGX_LOG_INT_ARR_GET_MACRO(__VA_ARGS__, DBGX_LOG_INT_ARR_EXPLICIT_FORMAT_NON_FILTERED, DBGX_LOG_INT_ARR_DEFAULT_FORMAT_NON_FILTERED)(Name, __VA_ARGS__)

// The following macros are used to build function calls for: non_filtered DBGX_logStrInt
#define DBGX_LOG_STR_INT_ARR_DEFAULT_FORMAT_NON_FILTERED(Name, String, IntegerArrayAddress, Number)            Name##_op_Wrapper (String, IntegerArrayAddress, Number, (uint8_t)sizeof(IntegerArrayAddress[0]))
#define DBGX_LOG_STR_INT_ARR_EXPLICIT_FORMAT_NON_FILTERED(Name, String, IntegerArrayAddress, Number, Format)   Name##_Wrapper (String, IntegerArrayAddress, Number, ((Format) | DBGX_ADD_INT_LEN_TO_FORMAT(IntegerArrayAddress[0])))
#define DBGX_LOG_STR_INT_ARR_VARIABLES_NON_FILTERED(Name, String, ...)                                         DBGX_LOG_INT_ARR_GET_MACRO(__VA_ARGS__, DBGX_LOG_STR_INT_ARR_EXPLICIT_FORMAT_NON_FILTERED, DBGX_LOG_STR_INT_ARR_DEFAULT_FORMAT_NON_FILTERED)(Name, String, __VA_ARGS__)

// The following calls (logStrIntLocat, logStrIntLocatFull) are equal to non_filtered DBGX_logStrInt
#define DBGX_LOG_STR_INT_ARR_LOCAT_VARIABLES_NON_FILTERED(Name, String, ...)                         DBGX_LOG_STR_INT_ARR_VARIABLES_NON_FILTERED(Name, String, __VA_ARGS__)
#define DBGX_LOG_STR_INT_ARR_LOCAT_FULL_VARIABLES_NON_FILTERED(Name, String, ...)                    DBGX_LOG_STR_INT_ARR_VARIABLES_NON_FILTERED(Name, String, __VA_ARGS__)

/*AXIVION Enable Style MisraC2012-20.7 */
/*AXIVION Enable Style MisraC2012-20.10 */


// Values of the calling filters in DBGX_CentralConfigFilter_Cfg.FilterFlagWords[]:

// Index       Value    Mask                                     Encode  Decode  Shift
// of                                                            Bit     Bit     value
//                                                               Shift   Shift
// - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Level Word  0        -                                        -       -
// Scn.  Word  1,2, ..  0000.0000.0000.0000.WWWW.WWWW.WWWW.WWWW  0       0
// Lev.  Bit   1 .. 32  bbbb.bb00.0000.0000.0000.0000.0000.0000  <<26    >>26    DBGX_LEV_BIT_IDX_SHIFT
// Scn.  Bit   1 .. 32  0000.00BB.BBBB.0000.0000.0000.0000.0000  <<20    >>20    DBGX_SCN_BIT_IDX_SHIFT


// Static filter definitions
// The list below holds all defined level and scenario filters of the current software project.
// In case of static activation of the filter, a definition in done according to the enumeration with filter flag word and bit.
// In case of static deactivation of the filter, the definition will stay empty.
// You can use this overview as guidance, to not double define a scenario name.

/*      Scenario name                    Index of filter bit                         Index of filter word      Value in DbgAttributes*/

#define DBGX_SCN_SSB_CBACK_APP          
#define DBGX_SCN_DBGX_TOUCH              (((uint32_t)1 << DBGX_SCN_BIT_IDX_SHIFT)    | (uint32_t)1 )           /* 0x0101 */  
#define DBGX_SCN_DBGX_POWER             



// Dynamic filter definitions
// The definitions below hold all static activated filters and their corresponding dynamic filter flag.
// This table should be used as reference, to activate/deactivate the dynamic filtered debug functions.
// In case the static filter is switched off for a scenario, it will not be shown here, as a dynamic activation will not have any affect.

// Initialization of the filter flag words: DBGX_CentralConfigFilter_Cfg.FilterFlagWords[]:
#define DBGX_NUMBER_FILTER_FLAG_WORDS   2

/*        Filter Activation    Index of filter    Value in         Scenario Name                 Filter value in array */
/*        Flag                 bit-1      word    DbgAttributes                                  DEC         HEX       */
#define DBGX_FILTER_FLAG_WORDS_CENTRAL_CONFIG {                                                                           \
    (                                                                                                                     \
    ),                                                                                                                    \
    (                                                                                                                     \
          ((uint32_t)1 <<      (1-1))     /* 1    0x0101           DBGX_TOUCH                    1          0X00000001 */ \
    )                                                                                                                     \
}


/******************************************************************************/
// Macro definitions for standard variant
/******************************************************************************/
// The code generator will create macros here, if a standard variant is compiled.

#ifndef DBGX_DOCTEST_CONFIG_ENABLED


#define DBGX_logChar_SCN_SSB_CBACK_APP(char)                                                                  
#define DBGX_logStr_SCN_SSB_CBACK_APP(string)                                                                 
#define DBGX_logInt_SCN_SSB_CBACK_APP(__logIntVar__...)                                                       
#define DBGX_logIntArr_SCN_SSB_CBACK_APP(__logIntArrVar__...)                                                 
#define DBGX_logTime_SCN_SSB_CBACK_APP()                                                                      
#define DBGX_logLocat_SCN_SSB_CBACK_APP()                                                                     
#define DBGX_logStrStr_SCN_SSB_CBACK_APP(string_a, string_b)                                                  
#define DBGX_logStrInt_SCN_SSB_CBACK_APP(string, __logStrIntVar__...)                                         
#define DBGX_logStrIntArr_SCN_SSB_CBACK_APP(string, __logStrIntArrVar__...)                                   
#define DBGX_logStrStrLocat_SCN_SSB_CBACK_APP(string_a, string_b)                                             
#define DBGX_logStrStrLocatFull_SCN_SSB_CBACK_APP(string_a, string_b)                                         
#define DBGX_logStrIntLocat_SCN_SSB_CBACK_APP(string, __logStrIntLocatVar__...)                               
#define DBGX_logStrIntLocatFull_SCN_SSB_CBACK_APP(string, __logStrIntLocatFullVar__...)                       

#define DBGX_logChar_SCN_DBGX_TOUCH(char)                                                                     DBGX_logChar_Wrapper(DBGX_SCN_DBGX_TOUCH, char)
#define DBGX_logStr_SCN_DBGX_TOUCH(string)                                                                    DBGX_logStr_Wrapper(DBGX_SCN_DBGX_TOUCH, string)
#define DBGX_logInt_SCN_DBGX_TOUCH(__logIntVar__...)                                                          DBGX_LOG_INT_VARIABLES(DBGX_logInt, DBGX_SCN_DBGX_TOUCH, __logIntVar__)
#define DBGX_logIntArr_SCN_DBGX_TOUCH(__logIntArrVar__...)                                                    DBGX_LOG_INT_ARR_VARIABLES(DBGX_logIntArr, DBGX_SCN_DBGX_TOUCH, __logIntArrVar__)
#define DBGX_logTime_SCN_DBGX_TOUCH()                                                                         DBGX_logTime_Wrapper(DBGX_SCN_DBGX_TOUCH)
#define DBGX_logLocat_SCN_DBGX_TOUCH()                                                                        DBGX_logLocat_Wrapper(DBGX_SCN_DBGX_TOUCH)
#define DBGX_logStrStr_SCN_DBGX_TOUCH(string_a, string_b)                                                     DBGX_logStrStr_Wrapper(DBGX_SCN_DBGX_TOUCH, string_a, string_b)
#define DBGX_logStrInt_SCN_DBGX_TOUCH(string, __logStrIntVar__...)                                            DBGX_LOG_STR_INT_VARIABLES(DBGX_logStrInt, DBGX_SCN_DBGX_TOUCH, string, __logStrIntVar__)
#define DBGX_logStrIntArr_SCN_DBGX_TOUCH(string, __logStrIntArrVar__...)                                      DBGX_LOG_STR_INT_ARR_VARIABLES(DBGX_logStrIntArr, DBGX_SCN_DBGX_TOUCH, string, __logStrIntArrVar__)
#define DBGX_logStrStrLocat_SCN_DBGX_TOUCH(string_a, string_b)                                                DBGX_logStrStrLocat_Wrapper(DBGX_SCN_DBGX_TOUCH, string_a, string_b)
#define DBGX_logStrStrLocatFull_SCN_DBGX_TOUCH(string_a, string_b)                                            DBGX_logStrStrLocatFull_Wrapper(DBGX_SCN_DBGX_TOUCH, string_a, string_b)
#define DBGX_logStrIntLocat_SCN_DBGX_TOUCH(string, __logStrIntLocatVar__...)                                  DBGX_LOG_STR_INT_LOCAT_VARIABLES(DBGX_logStrIntLocat, DBGX_SCN_DBGX_TOUCH, string, __logStrIntLocatVar__)
#define DBGX_logStrIntLocatFull_SCN_DBGX_TOUCH(string, __logStrIntLocatFullVar__...)                          DBGX_LOG_STR_INT_LOCAT_FULL_VARIABLES(DBGX_logStrIntLocatFull, DBGX_SCN_DBGX_TOUCH, string, __logStrIntLocatFullVar__)

#define DBGX_logChar_SCN_DBGX_POWER(char)                                                                     
#define DBGX_logStr_SCN_DBGX_POWER(string)                                                                    
#define DBGX_logInt_SCN_DBGX_POWER(__logIntVar__...)                                                          
#define DBGX_logIntArr_SCN_DBGX_POWER(__logIntArrVar__...)                                                    
#define DBGX_logTime_SCN_DBGX_POWER()                                                                         
#define DBGX_logLocat_SCN_DBGX_POWER()                                                                        
#define DBGX_logStrStr_SCN_DBGX_POWER(string_a, string_b)                                                     
#define DBGX_logStrInt_SCN_DBGX_POWER(string, __logStrIntVar__...)                                            
#define DBGX_logStrIntArr_SCN_DBGX_POWER(string, __logStrIntArrVar__...)                                      
#define DBGX_logStrStrLocat_SCN_DBGX_POWER(string_a, string_b)                                                
#define DBGX_logStrStrLocatFull_SCN_DBGX_POWER(string_a, string_b)                                            
#define DBGX_logStrIntLocat_SCN_DBGX_POWER(string, __logStrIntLocatVar__...)                                  
#define DBGX_logStrIntLocatFull_SCN_DBGX_POWER(string, __logStrIntLocatFullVar__...)                          




// Wrapping of non_filtered api functions

#define DBGX_logChar_NonFiltered(char)                                                                        DBGX_logChar_NonFiltered_Wrapper(char)
#define DBGX_logStr_NonFiltered(string)                                                                       DBGX_logStr_NonFiltered_Wrapper(string)
#define DBGX_logInt_NonFiltered(__logIntVar__...)                                                             DBGX_LOG_INT_VARIABLES_NON_FILTERED(DBGX_logInt_NonFiltered, __logIntVar__)
#define DBGX_logIntArr_NonFiltered(__logIntArrVar__...)                                                       DBGX_LOG_INT_ARR_VARIABLES_NON_FILTERED(DBGX_logIntArr_NonFiltered, __logIntArrVar__)
#define DBGX_logTime_NonFiltered()                                                                            DBGX_logTime_NonFiltered_Wrapper()
#define DBGX_logLocat_NonFiltered()                                                                           DBGX_logLocat_NonFiltered_Wrapper()
#define DBGX_logStrStr_NonFiltered(string_a, string_b)                                                        DBGX_logStrStr_NonFiltered_Wrapper(string_a, string_b)
#define DBGX_logStrInt_NonFiltered(string, __logStrIntVar__...)                                               DBGX_LOG_STR_INT_VARIABLES_NON_FILTERED(DBGX_logStrInt_NonFiltered, string, __logStrIntVar__)
#define DBGX_logStrIntArr_NonFiltered(string, __logStrIntArrVar__...)                                         DBGX_LOG_STR_INT_ARR_VARIABLES_NON_FILTERED(DBGX_logStrIntArr_NonFiltered, string, __logStrIntArrVar__)
#define DBGX_logStrStrLocat_NonFiltered(string_a, string_b)                                                   DBGX_logStrStrLocat_NonFiltered_Wrapper(string_a, string_b)
#define DBGX_logStrStrLocatFull_NonFiltered(string_a, string_b)                                               DBGX_logStrStrLocatFull_NonFiltered_Wrapper(string_a, string_b)
#define DBGX_logStrIntLocat_NonFiltered(string, __logStrIntLocatVar__...)                                     DBGX_LOG_STR_INT_LOCAT_VARIABLES_NON_FILTERED(DBGX_logStrIntLocat_NonFiltered, string, __logStrIntLocatVar__)
#define DBGX_logStrIntLocatFull_NonFiltered(string, __logStrIntLocatFullVar__...)                             DBGX_LOG_STR_INT_LOCAT_FULL_VARIABLES_NON_FILTERED(DBGX_logStrIntLocatFull_NonFiltered, string, __logStrIntLocatFullVar__)


#endif // DBGX_DOCTEST_CONFIG_ENABLED

/******************************************************************************/
// Macro definitions for doctest variant (Empty macros for compatibility reasons)
/******************************************************************************/
// The code generator will create macros here, if the doctest variant is compiled.

#ifdef DBGX_DOCTEST_CONFIG_ENABLED


#define DBGX_logChar_SCN_SSB_CBACK_APP(char) 
#define DBGX_logStr_SCN_SSB_CBACK_APP(string) 
#define DBGX_logInt_SCN_SSB_CBACK_APP(__logIntVar__...) 
#define DBGX_logIntArr_SCN_SSB_CBACK_APP(__logIntArrVar__...) 
#define DBGX_logTime_SCN_SSB_CBACK_APP() 
#define DBGX_logLocat_SCN_SSB_CBACK_APP() 
#define DBGX_logStrStr_SCN_SSB_CBACK_APP(string_a, string_b) 
#define DBGX_logStrInt_SCN_SSB_CBACK_APP(string, __logStrIntVar__...) 
#define DBGX_logStrIntArr_SCN_SSB_CBACK_APP(string, __logStrIntArrVar__...) 
#define DBGX_logStrStrLocat_SCN_SSB_CBACK_APP(string_a, string_b) 
#define DBGX_logStrStrLocatFull_SCN_SSB_CBACK_APP(string_a, string_b) 
#define DBGX_logStrIntLocat_SCN_SSB_CBACK_APP(string, __logStrIntLocatVar__...) 
#define DBGX_logStrIntLocatFull_SCN_SSB_CBACK_APP(string, __logStrIntLocatFullVar__...) 

#define DBGX_logChar_SCN_DBGX_TOUCH(char) 
#define DBGX_logStr_SCN_DBGX_TOUCH(string) 
#define DBGX_logInt_SCN_DBGX_TOUCH(__logIntVar__...) 
#define DBGX_logIntArr_SCN_DBGX_TOUCH(__logIntArrVar__...) 
#define DBGX_logTime_SCN_DBGX_TOUCH() 
#define DBGX_logLocat_SCN_DBGX_TOUCH() 
#define DBGX_logStrStr_SCN_DBGX_TOUCH(string_a, string_b) 
#define DBGX_logStrInt_SCN_DBGX_TOUCH(string, __logStrIntVar__...) 
#define DBGX_logStrIntArr_SCN_DBGX_TOUCH(string, __logStrIntArrVar__...) 
#define DBGX_logStrStrLocat_SCN_DBGX_TOUCH(string_a, string_b) 
#define DBGX_logStrStrLocatFull_SCN_DBGX_TOUCH(string_a, string_b) 
#define DBGX_logStrIntLocat_SCN_DBGX_TOUCH(string, __logStrIntLocatVar__...) 
#define DBGX_logStrIntLocatFull_SCN_DBGX_TOUCH(string, __logStrIntLocatFullVar__...) 

#define DBGX_logChar_SCN_DBGX_POWER(char) 
#define DBGX_logStr_SCN_DBGX_POWER(string) 
#define DBGX_logInt_SCN_DBGX_POWER(__logIntVar__...) 
#define DBGX_logIntArr_SCN_DBGX_POWER(__logIntArrVar__...) 
#define DBGX_logTime_SCN_DBGX_POWER() 
#define DBGX_logLocat_SCN_DBGX_POWER() 
#define DBGX_logStrStr_SCN_DBGX_POWER(string_a, string_b) 
#define DBGX_logStrInt_SCN_DBGX_POWER(string, __logStrIntVar__...) 
#define DBGX_logStrIntArr_SCN_DBGX_POWER(string, __logStrIntArrVar__...) 
#define DBGX_logStrStrLocat_SCN_DBGX_POWER(string_a, string_b) 
#define DBGX_logStrStrLocatFull_SCN_DBGX_POWER(string_a, string_b) 
#define DBGX_logStrIntLocat_SCN_DBGX_POWER(string, __logStrIntLocatVar__...) 
#define DBGX_logStrIntLocatFull_SCN_DBGX_POWER(string, __logStrIntLocatFullVar__...) 




// Wrapping of non_filtered api functions

#define DBGX_logChar_NonFiltered(char)
#define DBGX_logStr_NonFiltered(string)
#define DBGX_logInt_NonFiltered(__logIntVar__...)
#define DBGX_logIntArr_NonFiltered(__logIntArrVar__...)
#define DBGX_logTime_NonFiltered()
#define DBGX_logLocat_NonFiltered()
#define DBGX_logStrStr_NonFiltered(string_a, string_b)
#define DBGX_logStrInt_NonFiltered(string, __logStrIntVar__...)
#define DBGX_logStrIntArr_NonFiltered(string, __logStrIntArrVar__...)
#define DBGX_logStrStrLocat_NonFiltered(string_a, string_b)
#define DBGX_logStrStrLocatFull_NonFiltered(string_a, string_b)
#define DBGX_logStrIntLocat_NonFiltered(string, __logStrIntLocatVar__...)
#define DBGX_logStrIntLocatFull_NonFiltered(string, __logStrIntLocatFullVar__...)


#endif // DBGX_DOCTEST_CONFIG_ENABLED

#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_FILTERS_AUT_H */
