/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT         DBus Application Layer
 *  COMP_ABBREV     DBAL
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of Debug messages for DBal.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#ifdef DBAL_DEBUG
/*lint -save -e530 -e586 -e718 -e746 -e829  -e934 -e955 -e970 -e9034 Lint not seeing, that functions are attracted from compiler specific files.
 * Just for debugging, so masking ok.*/
#include "BshDBus2AppLayer_debug.h"
#include <stdarg.h>
#include <stdio.h>

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifndef DBAL_DBG_MAX_LEN
#define DBAL_DBG_MAX_LEN 100//!< Maximum length of a DBal debug message.
#endif
/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBALDB_Info(const char* const File, int Line, const char* const Format, ...)
{
    char Buffer[DBAL_DBG_MAX_LEN] = {0};
    va_list Args;
    va_start(Args, Format);
    vsprintf(Buffer, Format, Args);
    va_end(Args);

    DBG_writeMessageLevel(DBG_INFO);
    DBG_writeString(File);
    DBG_writeChar(' ');
    DBG_writeInt(Line);
    DBG_writeChar(' ');
    DBG_writeString(Buffer);
    DBG_writeEndLine();
}

void DBALDB_Warning(const char* const File, int Line, const char* const Format, ...)
{
    char Buffer[DBAL_DBG_MAX_LEN] = {0};
    va_list Args;
    va_start(Args, Format);
    vsprintf(Buffer, Format, Args);
    va_end(Args);

    DBG_writeMessageLevel(DBG_WARNING);
    DBG_writeString(File);
    DBG_writeChar(' ');
    DBG_writeInt(Line);
    DBG_writeChar(' ');
    DBG_writeString(Buffer);
    DBG_writeEndLine();
}

void DBALDB_Error(const char* const File, int Line, const char* const Format, ...)
{
    char Buffer[DBAL_DBG_MAX_LEN] = {0};
    va_list Args;
    va_start(Args, Format);
    vsprintf(Buffer, Format, Args);
    va_end(Args);

    DBG_writeMessageLevel(DBG_ERROR);
    DBG_writeString(File);
    DBG_writeChar(' ');
    DBG_writeInt(Line);
    DBG_writeChar(' ');
    DBG_writeString(Buffer);
    DBG_writeEndLine();
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/*lint -restore -e530 -e586 -e718 -e746 -e829 -e934 -e955 -e970 -e9034*/

#endif/*DBAL_DEBUG*/
