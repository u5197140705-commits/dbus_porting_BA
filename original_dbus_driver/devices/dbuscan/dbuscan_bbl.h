/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBCBBL
 ******************************************************************************/

#ifndef DBUSCAN_BBL_H
#define DBUSCAN_BBL_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    dbuscan_bbl.h
 *
 *  \ingroup dbuscan
 *
 *  \brief   Wrapper definitions and declarations for DBusCAN with BBL (Building Blocks) SPI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "dbuscan_types.h"


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
extern enum DBC_Error DBCBBL_init(void);
extern enum DBC_Error DBCBBL_transfer(const uint8_t *const writeBuf, const uint16_t writeLen, uint8_t *const readBuf, const uint16_t readLen);
extern enum DBC_Error DBCBBL_transferBlocking(const uint8_t *const writeBuf, const uint16_t writeLen, uint8_t *const readBuf, const uint16_t readLen);
extern enum DBC_Error DBCBBL_spiBusAcquire(void);
extern enum DBC_Error DBCBBL_spiBusRelease(void);


#ifdef __cplusplus
}
#endif

#endif // DBUSCAN_BBL_H
