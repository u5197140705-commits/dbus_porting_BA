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

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file
    \brief Public declarations for module dbus_lock. The module is used to ensure dbus will not try to send, while operations contradicting this are going on, but wait until the lock is released.
*/

#ifndef COMMON_PROG_DBUS_DBUS_LOCK_H_
#define COMMON_PROG_DBUS_DBUS_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 *  \brief Attempts to block dbus, so that other actions can be performed, which would contradict communication.
 *  \details Returns true, if attempt succeeded and false if dbus was sending or transmitting.
 *  \details DO NOT CALL FROM INTERRUPT HANDLER UNDER ANY CIRCUMSTANCES.
 *  */
bool DBLK_ApplyDbusLock(void);

/**
 *  \brief Releases lock on dbus, making communication possible again.
 *  \details Can be called from any place in code, including interrupt handlers.
 *  */
void DBLK_ReleaseDbusLock(void);

/**
 *  \brief Returns information, whether Dbus is locked for communication.
 *  \details If BAL_vTransmitMessage is called, while Dbus is locked, the message will be sent, after lock has been lifted.
 *  \details If messages are received, while Dbus is locked, processing will take place, after lock has been lifted.
 *  */
bool DBLK_IsDbusLockActive(void);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_PROG_DBUS_DBUS_LOCK_H_ */
