/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          DBus Application Layer
 *  COMP_ABBREV      DBAL
 ******************************************************************************/

#ifndef BSH_DBUS2_APP_LAYER_DEBUG_H__
#define BSH_DBUS2_APP_LAYER_DEBUG_H__

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Public definitions and declarations for debugging messages in DBal
 *
 *  \details  Debug API from Mcu Framework is used.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <string.h>
#ifdef DBAL_DEBUG
#include "debug/debug_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#ifdef DOXY_DEVELOPERS_DOC
/** Compiler switch to activate debug messages in DBal
 */ #define DBAL_DEBUG
#endif
/*lint -save -e9026 No other choice than defining function like macro.*/

/*Macro below introduced NOT to print whole path to file, but just it's name.*/
/*lint --emacro((613),__FILENAME__) Just for debug, working fine.*/
/*lint --emacro((9027),__FILENAME__) Just for debug, working fine.*/
/*lint --emacro((9034),__FILENAME__) Just for debug, working fine.*/
/*lint --emacro((9050),__FILENAME__) Just for debug, working fine.*/
#define __FILENAME__  ((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))

#if   defined (DBAL_DEBUG)

#define DBAL_INFO(...)      DBALDB_Info(__FILENAME__, __LINE__, __VA_ARGS__);
#define DBAL_WARNING(...)   DBALDB_Warning(__FILENAME__, __LINE__, __VA_ARGS__);
#define DBAL_ERROR(...)     DBALDB_Error(__FILENAME__, __LINE__, __VA_ARGS__);

#elif defined (GOOGLETEST_INCLUDE_GTEST_GTEST_H_) /*For unittest in the frame of DBal development.*/

#define DBAL_INFO(...)      printf("%s ", __FILENAME__);printf("%d ", __LINE__);printf("[INFO] ");printf(__VA_ARGS__);printf("\n")
#define DBAL_WARNING(...)   printf("%s ", __FILENAME__);printf("%d ", __LINE__);printf("[WARNING] ");printf(__VA_ARGS__);printf("\n")
#define DBAL_ERROR(...)     printf("%s ", __FILENAME__);printf("%d ", __LINE__);printf("[ERROR] ");printf(__VA_ARGS__);printf("\n")

#else

#define DBAL_INFO(...)
#define DBAL_WARNING(...)
#define DBAL_ERROR(...)

#endif
/*lint -restore -e9026*/

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
#ifdef DBAL_DEBUG
/*lint -save -e970 Int needed.*/
void DBALDB_Info(const char* const File, int Line, const char* const Format, ...);//!< Prints DBal debug info
void DBALDB_Warning(const char* const File, int Line, const char* const Format, ...);//!< Prints DBal debug warning
void DBALDB_Error(const char* const File, int Line, const char* const Format, ...);//!< Prints DBal debug error
/*lint -restore -e970*/
#endif

#ifdef __cplusplus
}
#endif

#endif
