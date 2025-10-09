/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBCBL
 ******************************************************************************/


#ifndef DBUSCAN_BL_H
#define DBUSCAN_BL_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Public definitions and declarations related to BP2 communication via DBusCAN chip
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_types.h"


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/** \brief   Reads data from the DBus Rx FIFO of the DBusCAN chip.
 *
 *  \return  pointer to received data
 */
extern uint8_t* DBCBL_readData(uint16_t dataSize);


/** \brief   Sends data via the DBusCAN chip (over DBus line).
 *
 *  \param   dataBuf:  pointer to buffer with data
 *  \param   dataSize: size of data to be sent
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCBL_sendData(const uint8_t *dataBuf, uint16_t dataSize);


/** \brief   Returns information whether transmission over DBus line from the DBusCAN chip is ongoing.
 *
 *  \return  true or false
 */
extern bool DBCBL_isTxOngoing(void);


/** \brief   Reads and clears interrupt flags (except DBUS_IR_RF0N flag) from the DBusCAN chip
 *           and returns information whether there is any receive error.
 *
 *  \details Any interrupt flag set except the new DBus message received (DBUS_IR_RF0N) and DBus silent (DBUS_IR_DBUSSLNT) flag is considered as receive error.
 *
 *  \return  true or false
 */
extern bool DBCBL_clearIrq(void);


/** \brief Reads status about transmitted data from the DBusCAN chip.
 */
extern void DBCBL_readTxStatus(void);


/** \brief Empties Rx FIFO of the DBusCAN chip.
 */
extern void DBCBL_clearRxFifo(void);


/** \brief   Returns number of data received by the DBusCAN chip from DBus line.
 *
 *  \return  uint16_t
 */
extern uint16_t DBCBL_getRxDataSize(void);


#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif

#endif // DBUSCAN_BL_H
