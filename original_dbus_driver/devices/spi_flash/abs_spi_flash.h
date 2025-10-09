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


#ifndef ABS_SPI_FLASH_H
#define ABS_SPI_FLASH_H

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
 *  \brief    Public declarations for serial SPI NOR flash abstraction layer.
 *
 *  \details For the purpose of holding memory partitions in external SPI flash
 *  not all of the typical SPI flash commands are required.
 *  Also most vendors came with different extension of standard which makes
 *  it difficult to provide a second source. Therefore this driver supports
 *  a minimum common command set.
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
/** Structure for the JEDEC device ID */
typedef struct ABSSF_jedecID_s
{
    uint8_t manufacturer;  ///< identifier for manufacturer
    uint8_t type;          ///< type of memory
    uint8_t capacity;      ///< capacity of the device
}ABSSF_jedecID_t;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/** \brief Initializes interface and device.
 *  \note  This must be called before any other interface.
 */
void ABSSF_init(void);


/** \brief Returns true if any write or erase operation is in progress
 *  \details Do not call any interface when device is busy
 *  \return true when device is busy, false otherwise.
 */
bool ABSSF_isBusy(void);


/** \brief Erases a memory sector (usually 4k)
 *  \note Use ABSSF_isBusy() to check if another operation is in progress
 *
 *  \param addr address on a sector start (upper layer is responsible for alignment)
 */
void ABSSF_eraseSector(uint32_t addr);


/** \brief Erases a 32k block of memory
 *  \note Use ABSSF_isBusy() to check if another operation is in progress
 *
 *  \param addr address on a block start (upper layer is responsible for alignment)
 */
void ABSSF_eraseBlock32(uint32_t addr);


/** \brief Erases a 64k block of memory
 *  \note Use ABSSF_isBusy() to check if another operation is in progress
 *
 *  \param addr address on a block start (upper layer is responsible for alignment)
 */
void ABSSF_eraseBlock64(uint32_t addr);


/** \brief Writes data from buffer to the flash. The maximum size depends on flash device.
 *  \note Use ABSSF_isBusy() to check if another operation is in progress
 *  \note Currently supported flashes has a page size of 256 bytes, future devices might have
 *        other page size.
 *  \details The bytes until next page border will be written to flash.
 *   If e.g. address is 0x....10h and page size is 256, maximal 240 bytes can be written.
 *
 *  \param addr address of data to be written
 *  \param size   number of bytes to write. Might be even more then the page size.
 *                The driver decides the number of bytes written in one step
 *                (see command description).
 *  \param buffer pointer to data buffer to be written
 *  \return number of bytes which has been written. 
 */
uint32_t ABSSF_write(uint32_t addr, uint32_t size, const uint8_t* buffer);


/** \brief reads data form the flash to the buffer.
 *  \note Use ABSSF_isBusy() to check if another operation is in progress
 *
 *  \param addr address of data to read form
 *  \param size   number of bytes to read
 *  \param buffer pointer to data buffer to hold read data
 */
void ABSSF_read(uint32_t addr, uint32_t size, uint8_t* buffer);


/** \brief returns the JEDEC ID of the device
 *  \return the JEDEC id structure (3 Bytes)
 */
ABSSF_jedecID_t ABSSF_readID(void);


/** \brief Enters the power down state
    \note The upper layer is responsible to wake up the device after power down.
 */
void ABSSF_powerDown(void);


/** \brief Wakes up the device after power down */
void ABSSF_releasePowerDown(void);


/** \brief returns the size of memory in bytes
 *  
 *  \return size in bytes
 */
uint32_t ABSSF_getMemorySize(void);


#ifdef __cplusplus
}
#endif

#endif // ABS_SPI_FLASH_H
