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
 *  COMP_ABBREV      DBCDRV
 ******************************************************************************/


#ifndef DBUSCAN_DRV_H
#define DBUSCAN_DRV_H

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
 *  \brief   Public definitions and declarations for unit dbuscan
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "mcal_channels.h"
#include "dbuscan.h"
#include "dbuscan_types.h"
#include "dbuscan_drv_priv.h"

/******************************************************************************/
/* DEPENDENCIES                                                               */
/******************************************************************************/
#define DEP_DBC    MOD_DLL, ///< dependencies of module


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define DBCDRV_DBUS_RX_FIFO_SIZE                512u  ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Rx FIFO buffer. Must be a multiple of 4 bytes.


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
extern bool DBCDRV_eepromWriteEnable; ///< Enables or disables write to the chip EEPROM via DBCDRV_HandleTask()


/** \brief   Configuration of SPI peripheral unit for communication with the DBusCAN chip
 */
extern const struct MSPI_Config DBCDRV_mspiCfg;


/** \brief   SPI peripheral unit and pins used for communication with the DBusCAN chip
 */
extern const struct MSPI_Channel DBCDRV_mspiChannel;


#ifdef DBUSCAN_WITH_BBL_SPI
/** \brief   GPIO pin used as chip select for the DBusCAN chip
 */
extern const struct MDIO_Channel* DBCDRV_csPin;
#endif


#ifdef DBUSCAN_DMA_USED
/** \brief   MDMA peripheral unit used for communication with the DBusCAN chip
 */
extern const struct MDMA_Periph DBCDRV_dmaPeriph;


/** \brief   MDMA Tx channel used communication with the DBusCAN chip
 */
extern struct MDMA_Channel DBCDRV_dmaTxChannel;


/** \brief   MDMA Rx channel used communication with the DBusCAN chip
 */
extern struct MDMA_Channel DBCDRV_dmaRxChannel;
#endif // DBUSCAN_DMA_USED


/** \brief   MSPI Handle for SPI communication with DBusCAN
 */
extern struct MSPI_Handle DBCDRV_mspiHandle;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief   Handles write to chip's EEPROM. It is called from scheduler.
 *
 *  \return  uint8_t
 */
extern uint8_t DBCDRV_HandleTask(void);


/** \brief   Resets the DBusCAN chip.
 *
 *  \details Reset is triggered by the SPI frame sent to the DBusCAN chip to set a reset flag in a register.
 *
 *  \param   rstType: SOFT / HARD / FULL (enum DBC_Reset)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_reset(enum DBC_Reset rstType);


/** \brief   Resets the DBusCAN chip by special toggling of the SPI pins.
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_spiReset(void);


/** \brief   Sets the power mode of the DBusCAN chip.
 *
 *  \param   mode: power mode to be set (enum DBC_PowerMode)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_setPowerMode(enum DBC_PowerMode mode);


/** \brief   Returns device status stored in DBCDRV_spiBuff.
 *
 *  \note    This function returns global status flags from the register Reg0820[7:0]
 *           of the last SPI communication.
 *
 *  \return  uint8_t - DBCDRV_spiBuff - The global status flags Reg0820[7:0].
 */
extern uint8_t DBCDRV_getStatus(void);


#ifdef DBUSCAN_SPI_CRC_USED
/** \brief   Informs whether CRC error occurred during the last SPI read operation.
 *
 *  \note    This function checks the flag variable indicating if the CRC error occurred during the last SPI read operation.
 *
 *  \return  bool
 */
extern bool DBCDRV_isSpiCrcReadError(void);


/**
 * \brief   Checks CRC for the read operations over SPI.
 *
 * \note    This function compares the CRC value received from the DBusCAN chip
 *          with the calculated CRC value over the data received from the DBusCAN chip.
 *
 * \param   readData: Pointer to the data array read from the SPI interface.
 */
extern void DBCDRV_checkSpiCrcInReadOp(uint8_t *readData);


/** \brief   Informs whether CRC error occurred during the last SPI write operation.
 *
 *  \note    This function checks the flag indicating if a CRC error occurred during the last SPI write operation.
 *           If a CRC error was detected, it returns `true`.
 *
 *  \return  bool
 */
extern bool DBCDRV_isSpiCrcWriteError(void);
#endif // DBUSCAN_SPI_CRC_USED


#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif

#endif // DBUSCAN_DRV_H
