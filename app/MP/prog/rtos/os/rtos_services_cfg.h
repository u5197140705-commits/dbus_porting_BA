/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          RTOS (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          23.11.2017
 ******************************************************************************/


/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Public definitions and declarations for module rtos_services_config.

#ifndef RTOS_SERVICES_CONFIG_H_
#define RTOS_SERVICES_CONFIG_H_


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "os/rtos_defines.h"


/***************************************************************************************************
 * CONFIG: DEFAULT RTOS THREADS
 * *************************************************************************************************
 * \brief: 	If needed, activate the Default RTOS Threads here.
 ***************************************************************************************************/
#define RTOS_THREAD_IDLE    THREAD_DEACTIVATED  // An Idle Thread with the lowest Priority


/**************************************************************************************************/
/* GLOBAL FUNCTION DECLARATIONS                                                                   */
/**************************************************************************************************/
void RTOS_setupRealTimeEnvironment(void);

#endif

/********************************************* End of File ******************************************************/
