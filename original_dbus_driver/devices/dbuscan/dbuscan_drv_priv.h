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
 *  COMP_ABBREV      DBCDRV
 ******************************************************************************/


#ifndef DBUSCAN_DRV_PRIV_H
#define DBUSCAN_DRV_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    dbuscan_drv_priv.h
 *
 *  \ingroup dbuscan
 *
 *  \brief   Private declarations for unit dbuscan
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "mcal_channels.h"
#include "mcal/mcal_types.h"
#include "dbuscan_types.h"


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

/** \brief   Gets GPIO pin to be used for the external interrupt from interrupt pin (INT) of the DBusCAN chip.
 *
 *  \note    This function is used internally in dbuscan drivers
 *
 *  \return  pointer to structure MEXTI_Channel
 */
extern const struct MEXTI_Channel* DBCDRV_getMextiChannel(void);


/** \brief   Gets structure of values used for DBusCAN chip configuration (to be) stored in chip EEPROM.
 *
 *  \note    This function is used internally by DBCDRV_init function.
 *
 *  \return  configuration structure DBC_Cfg_t
 */
extern DBC_Cfg_t DBCDRV_getConfig(void);


/** \brief   Initializes MSPI, MEXTI and MDIO channels to be used for communication with the DBusCAN chip.
 *
 *  \note    This function is used internally by DBCDRV_init function.
 *
 *  \param   irqHandleCbFunc: callback function to be executed when external interrupt from the DBusCAN chip occurs
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_initComChannels(MCAL_CallbackFunction_t irqHandleCbFunc);


/** \brief   Initializes the DBusCAN chip for communication.
 *
 *  \note    This function is used internally by DLL_init function.
 *
 *  \param   irqHandleCbFunc: callback function for handling interrupts from chip
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_init(MCAL_CallbackFunction_t irqHandleCbFunc);


#ifdef APP_VARIANT
/** \brief  Disables detection of interrupts from DBusCAN chip
 * 
 *  \note   This function is used internally, not intended to be used by application.
 */
extern void DBCDRV_disableIrq(void);
#endif


/** \brief  Returns information whether power on reset of the device has occurred.
 *
 *  \note   This function is used internally, not intended to be used by application.
 *
 *  \return true or false
 */
extern bool DBCDRV_isPowerOnReset(void);


/** \brief  Sets notification that power on reset of the device has occurred.
 *
 *  \note   This function is used internally, not intended to be used by application.
 */
extern void DBCDRV_notifyPowerOnReset(void);


/** \brief   Writes data to the DBusCAN chip.
 *
 *  \param   addr:     start address where to write data (enum DBC_RegAddr)
 *  \param   writeBuf: pointer to the buffer with data to be written
 *  \param   len:      length of the data to be written
 *  \param   cmd:      write operation command (enum DBC_command)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_write(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_command cmd);


/** \brief   Writes data to the DBusCAN chip with non-blocking SPI communication.
 *
 *  \param   addr:     start address where to write data (enum DBC_RegAddr)
 *  \param   writeBuf: pointer to the buffer with data to be written
 *  \param   len:      length of the data to be written
 *  \param   cmd:      write operation command (enum DBC_command)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_writeNbl(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_command cmd);


/** \brief   Reads data from the DBusCAN chip.
 *
 *  \param   addr:    start address from where to read data (enum DBC_RegAddr)
 *  \param   readBuf: pointer to the buffer where read data should be stored
 *  \param   len:     length of the data to be read
 *  \param   cmd:     read operation command (enum DBC_command)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_read(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_command cmd);


/** \brief   Reads data from the DBusCAN chip with non-blocking SPI communication.
 *
 *  \param   addr:    start address from where to read data (enum DBC_RegAddr)
 *  \param   readBuf: pointer to the buffer where read data should be stored
 *  \param   len:     length of the data to be read
 *  \param   cmd:     read operation command (enum DBC_command)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_readNbl(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_command cmd);


/** \brief   Writes 4-byte register of the DBusCAN chip.
 *
 *  \param   addr: address of the register to be written (enum DBC_RegAddr)
 *  \param   data: data to be written (4 bytes)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data);


/** \brief   Writes 4-byte register of the DBusCAN chip with non-blocking SPI communication.
 *
 *  \param   addr: address of the register to be written (enum DBC_RegAddr)
 *  \param   data: data to be written (4 bytes)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_writeReg32Nbl(enum DBC_RegAddr addr, uint32_t data);


/** \brief   Reads data from the DBusCAN chip's register.
 *
 *  \param   addr: address of the register to be read (enum DBC_RegAddr)
 *
 *  \return  4 bytes of data
 */
extern uint32_t DBCDRV_readReg32(enum DBC_RegAddr addr);


/** \brief   Reads data from the DBusCAN chip's register with non-blocking SPI communication.
 *
 *  \note    The read data can be accessed using the function DBCDRV_getReadReg32Nbl
 *
 *  \param   addr: address of the register to be read (enum DBC_RegAddr)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_readReg32Nbl(enum DBC_RegAddr addr);


/** \brief   Returns the last data read by the function DBCDRV_readReg32Nbl
 *
 *  \return  uint32_t
 */
extern uint32_t DBCDRV_getReadReg32Nbl(void);


/** \brief   Returns information whether some interrupt event has occurred in the DBusCAN chip.
 *
 *  \note    This function is used internally in dbuscan drivers.
 *
 *  \return  bool
 */
extern bool DBCDRV_isIrqEvent(void);


/** \brief   Reads interrupt flags from the DBusCAN chip.
 *
 *  \note    This function is used internally in dbuscan drivers.
 *
 *  \param   irqBuf: pointer to the buffer in which read interrupt flags will be stored
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_readIrq(union DBC_IrqBuf *irqBuf);


/** \brief   Unlocks DBus configuration registers of DBusCAN chip.
 *  \note    This function is used internally in dbuscan drivers.
 */
extern enum DBC_Error DBCDRV_enableCfgDbus(void);


/** \brief   Locks DBus configuration registers of DBusCAN chip.
 *  \note    This function is used internally in dbuscan drivers.
 */
extern enum DBC_Error DBCDRV_disableCfgDbus(void);


/** \brief   Converts DBus decimal baudrate value (in hectobauds) to the baudrate value code used in the DBusCAN chip registers.
 *
 *  \note    This function is used internally e.g. by DBCDRV_getConfig function.
 *
 *  \param   baudrate: DBus baudrate value in hectobauds
 *
 *  \return  baudrate value code (see enum DBC_DbusBaudrate)
 */
extern uint16_t DBCDRV_convertDbusBaudValue(uint16_t baudrate);


/** \brief   Sets DBus baudrate in the DBusCAN chip.
 *
 *  \note    This function is used internally by DBus layers.
 *
 *  \param   baudrate: DBus baudrate value in hectobauds
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_setDbusBaudrate(uint16_t baudrate);


/** \brief   Returns information whether device revision includes all features.
 *
 *  \return  bool
 */
extern bool DBCDRV_isAllFeatureRevision(void);


#ifdef __cplusplus
}
#endif

#endif // DBUSCAN_DRV_PRIV_H
