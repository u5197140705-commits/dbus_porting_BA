/**************************************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 **************************************************************************************************
 *   PROJECT          RTOS (Generic SW)
 *   AUTHOR           Raundal
 *   CREATED          23.11.2017
 **************************************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/// \file Public configurations, definitions and declarations for module rtos_services_config.

#ifndef RTOS_SERVICES_CONFIG_H_
#define RTOS_SERVICES_CONFIG_H_


/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "os/rtos_defines.h"


/***************************************************************************************************
 * CONFIG: DEFAULT RTOS THREADS
 * *************************************************************************************************
 * \brief: If needed, activate the Default RTOS Threads here.
 ***************************************************************************************************/
#define RTOS_THREAD_POWER_MANAGEMENT    THREAD_ACTIVATED  // Power Management Thread with the lowest Priority
#define RTOS_THREAD_PM_PRIO             IDLE_PRIORITY
#define RTOS_STACK_SIZE_PM              1024U             // Stack size reserved for power management thread

/*    RTOS_HEAP_SIZE Size of heap memory (in octets), currently only applicable if Micrium is underlying RTOS.
 *    Size, in octets, of the general-purpose heap memory used as default memory segment.
 *    Default: 9216
 *    Low level formatting needs around 5K heap size.
 *    Core stack of file system requires around 20K of heap size.
 *    5632 is the case where low level formatting and core stack are both disabled.
 */
#define  RTOS_HEAP_SIZE                 5632U

/*  RTOS_MSG_QUEUE_POOL_ENTRIES is the size of the RTOS message pool (i.e. the number of messages that
 *  can be placed system-wide in message queues at the same time). This setting is only applicable to
 *  Micrium as underlying RTOS.
 *  The memory for a message in the pool is currently 16 bytes. So the required memory for the pool will
 *  be (RTOS_MSG_QUEUE_POOL_ENTRIES * 16) bytes.
 *
 *  PLEASE NOTE: The amount of memory is part of the Micrium heap, so the size needs to be included in
 *               the RTOS_HEAP_SIZE configuration.
 *
 *               Example: Heap size needed for general usage is 5632 bytes (5k default config).
 *                        Maximum number of simultaneous messages in message queues is 100.
 *                        5632 + 100 * sizeof(OS_MSG) == 7232
 *                        -> #define  RTOS_HEAP_SIZE 7232U
 */
#define  RTOS_MSG_QUEUE_POOL_ENTRIES    100U


/**************************************************************************************************/
/* GLOBAL FUNCTION DECLARATIONS                                                                   */
/**************************************************************************************************/
/*!
 *  \brief      Sets up Real time environment
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    Called from rtos_power_management for MicriumOS
 */
void RTOS_setupRealTimeEnvironment(void);


/*!
 *  \brief      Create all the needed RTOS Services
 *
 *  \param      none
 *
 *  \return     void
 *
 *  \details    Called from high level initialization
 */
void RTOS_createRTOSservices(void);

#endif

/*************************************** End of File **********************************************/
