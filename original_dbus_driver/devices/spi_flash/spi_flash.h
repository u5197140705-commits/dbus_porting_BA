/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  serial_flash
 *
 *  \brief    Public definitions for serial SPI flash configuration.
 *  \details  These interfaces must be implemented project specific depending
 *            on the present hardware. 
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Initializes the Interface of the external serial flash.
 *            that has to be terminated by an empty line.
 *
 *  \details  The used GPIOs and SPI need to be initialized.
 */
void ABSSF_initInterface(void);


/** \brief Reads data form the SPI interface connected to external flash.
 *
 *  \param size number of bytes to read
 *  \param buffer pointer to buffer to hold read data
 *  \note caller is responsible that the buffer can hold at least size
 *        amount of data
 */
void ABSSF_readData(uint32_t size, uint8_t* buffer);


/** \brief Writes data to the SPI interface connected to external flash.
 *
 *  \param size number of bytes to write
 *  \param buffer pointer to buffer which holds the data to write
 */
void ABSSF_writeData(uint32_t size, const uint8_t* buffer);


/** Select the SPI interface of flash device */
void ABSSF_selectChip(void);


/** Unselect the SPI interface of flash device */
void ABSSF_unselectChip(void);



#ifdef __cplusplus
}
#endif

#endif // SPI_FLASH_H
