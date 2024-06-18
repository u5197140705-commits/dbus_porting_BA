/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Smart Sensor Bus
 *  COMP_ABBREV      SSB_HUBC
 ******************************************************************************/

#ifndef ATSSB_HANDLE_TASK_H
#define ATSSB_HANDLE_TASK_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     atssb_handle_task.h
 *
 *  \ingroup  app/ATSSB
 *
 *  \brief    Interface of Application Task for Smart Sensor Bus
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/******************************************************************************/
/* C-PREPROCESSOR DEFINITIONS                                                 */
/******************************************************************************/
#define DEP_ATSSB MOD_SSBF,
/******************************************************************************/
/* FUNCTION PROTOTYPES                                                        */
/******************************************************************************/
/**
 * \brief   Callback for application data
 *
 * \param   eventToken      Contains Hub-Index and further elements according to
 *                          ssbf_common_c.h, the following elements are relevant
 *                          for for the loop results:
 *                          -SSB_EVT_CLIENT_MASK
 *                          -SSB_EVT_TYPE_MASK
 *          eventDataPtr    Pointer to the data delivered (e.g. the loop results)
 *          eventDataLen    Number of bytes delivered via eventDataPtr
 *
 * \return  none
 */
extern void ATSSB_doForHubCAPICallback(     uint16_t eventToken,
                                            const uint8_t *eventDataPtr,
                                            uint8_t eventDataLen );


/**
 * \brief   Handletask for ATSSB
 *
 * \param   none
 *
 * \return  Taskstate
 *          -TASK_NOT_INITIALISED - Handler not initialized
 *          -TASK_INITIALISED     - Task initialized and in run state
 *
 */
extern uint8_t ATSSB_handleTask(void);

#endif //ATSSB_HANDLE_TASK_H

