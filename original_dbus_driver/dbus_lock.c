/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          IC Dbus
*   COMP_ABBREV      DBLK
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*   
*   \brief Public definitions for module dbus_lock. The module is used to ensure
*   dbus will not try to send, while operations contradicting this are going on,
*   but wait until the lock is released.
*   
*/


#include "dbus_lock.h"
#include "dbusdll.h"


static bool DBLK_isDbusLockActive = false;

bool DBLK_ApplyDbusLock(void)
{
    if((DLL_bIsDbusCommunicating() == false) && (DBLK_IsDbusLockActive() == false))
    {
        DBLK_isDbusLockActive = true;
        return true;
    }

    return false;
}

void DBLK_ReleaseDbusLock(void)
{
    DBLK_isDbusLockActive = false;
}

bool DBLK_IsDbusLockActive(void)
{
    return DBLK_isDbusLockActive;
}
