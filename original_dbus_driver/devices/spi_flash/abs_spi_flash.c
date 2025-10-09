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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  serial_flash
 *
 *  \brief    Implementation of serial SPI flash abstraction layer
 *
 *  \details  This interfaces abstracts the serial SPI flash commands.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "abs_spi_flash.h"
#include "spi_flash.h"

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \brief    A brief enum type description
 *            that has to be terminated by an empty line.
 *
 *  \details  A detailed enum type description that may span several lines
 *            and that has to be terminated by an empty line.
 */
enum ABSSF_command
{
    ABSSF_cmdWrEnable = 0x06,     ///< write enable command
    ABSSF_cmdWrStatusReg = 0x01,  ///< write status register
    ABSSF_cmdRdStatusReg = 0x05,  ///< read status register
    ABSSF_cmdRdId = 0x9F,         ///< read the manufacturer ID (JEDEC)
    ABSSF_cmdRdData = 0x03,       ///< read data
    ABSSF_cmdEraseSector = 0x20,  ///< erase 4k sector
    ABSSF_cmdEraseBlock32 = 0x52, ///< erase 32k block
    ABSSF_cmdEraseBlock64 = 0xD8, ///< erase 64k block
    ABSSF_cmdPageProgram = 0x02,  ///< page program 1-256 bytes
    ABSSF_cmdRelPowerDown = 0xAB, ///< release power down
    ABSSF_cmdPowerDown = 0xB9     ///< enter power down mode
};

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void ABSSF_doReadCommand(enum ABSSF_command cmd, uint32_t addr, uint32_t size, uint8_t *buffer);
static void ABSSF_doWriteCommand(enum ABSSF_command cmd, uint32_t addr, uint32_t size, const uint8_t *buffer);
static void ABSSF_doCmdOnly(enum ABSSF_command cmd, uint32_t addr);
static void ABSSF_doCmdEntry(enum ABSSF_command cmd, uint32_t addr);
static void ABSSF_writeEnable(void);
static uint32_t ABSSF_limitWriteSize(uint32_t addr, uint32_t size);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static const uint32_t ABSSF_noAddress = UINT32_MAX; ///< used when addr will not be transmitted
static ABSSF_jedecID_t ABSSF_jedecID;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*lint -e934 "Taking address of near auto variable 'xx' [MISRA 2012 Rule 1.3, required]" does not make sense on MCU platforms */

void ABSSF_init(void)
{ // do the interface initialization
    ABSSF_initInterface();
    ABSSF_jedecID = ABSSF_readID();
}

bool ABSSF_isBusy(void)
{
    uint8_t status;
    const uint8_t busy_mask = 0x01;
    /* read the status byte form external flash memory */
    ABSSF_doReadCommand(ABSSF_cmdRdStatusReg, ABSSF_noAddress, sizeof(status), &status);
    return (status & busy_mask) != 0u; /* busy if lowest flag in status is set */
}

void ABSSF_eraseSector(uint32_t addr)
{
    ABSSF_writeEnable();
    ABSSF_doCmdOnly(ABSSF_cmdEraseSector, addr);
}

void ABSSF_eraseBlock32(uint32_t addr)
{
    ABSSF_writeEnable();
    ABSSF_doCmdOnly(ABSSF_cmdEraseBlock32, addr);
}

void ABSSF_eraseBlock64(uint32_t addr)
{
    ABSSF_writeEnable();
    ABSSF_doCmdOnly(ABSSF_cmdEraseBlock64, addr);
}

uint32_t ABSSF_write(uint32_t addr, uint32_t size, const uint8_t *buffer)
{
    if(size==0u){return 0u;}
    uint32_t write_size = ABSSF_limitWriteSize(addr, size);
    ABSSF_writeEnable();
    ABSSF_doWriteCommand(ABSSF_cmdPageProgram, addr, write_size, buffer);
    return write_size;
}

void ABSSF_read(uint32_t addr, uint32_t size, uint8_t *buffer)
{
    if(size==0u){return;}
    ABSSF_doReadCommand(ABSSF_cmdRdData, addr, size, buffer);
}

ABSSF_jedecID_t ABSSF_readID(void)
{
    union jedecID_u //lint !e9018 union keyword should not be used. MisraC2012 19.2 advisory
    {
        ABSSF_jedecID_t id_struct;
        uint8_t idbuf[sizeof(ABSSF_jedecID_t)];
    };
    union jedecID_u id; /*lint !e9018 declaration of union based type 
                          [MISRA 2012 Rule 19.2, advisory] */
    ABSSF_doReadCommand(ABSSF_cmdRdId, ABSSF_noAddress, sizeof(ABSSF_jedecID_t), id.idbuf);
    return id.id_struct;
}

void ABSSF_powerDown(void) { ABSSF_doCmdOnly(ABSSF_cmdPowerDown, ABSSF_noAddress); }

void ABSSF_releasePowerDown(void) { ABSSF_doCmdOnly(ABSSF_cmdRelPowerDown, ABSSF_noAddress); }

uint32_t ABSSF_getMemorySize(void)
{
    uint8_t capMask = 0x1Fu;
    return (uint32_t)1u << (ABSSF_jedecID.capacity & capMask);
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief Sends the write enable command */
static void ABSSF_writeEnable(void) { ABSSF_doCmdOnly(ABSSF_cmdWrEnable, ABSSF_noAddress); }

/** \brief Handles a complete write command including entry sequence and data transmission
 *
 *  \param cmd the command to be executed
 *  \param addr address to the according command. ABSSF_noAddress means address will not be sent
 *  \param size   number of bytes to write
 *  \param buffer pointer to data buffer to be written
 */
static void ABSSF_doWriteCommand(enum ABSSF_command cmd, uint32_t addr, uint32_t size, const uint8_t *buffer)
{
    ABSSF_selectChip();
    ABSSF_doCmdEntry(cmd, addr);
    ABSSF_writeData(size, buffer);
    ABSSF_unselectChip();
}

/** \brief Handles a complete read command including entry sequence and data transmission
 *
 *  \param cmd the command to be executed
 *  \param addr address to the according command. ABSSF_noAddress means address will not be sent
 *  \param size   number of bytes to read
 *  \param buffer pointer to data buffer to hold read data
 */
static void ABSSF_doReadCommand(enum ABSSF_command cmd, uint32_t addr, uint32_t size, uint8_t *buffer)
{
    ABSSF_selectChip();
    ABSSF_doCmdEntry(cmd, addr);
    ABSSF_readData(size, buffer);
    ABSSF_unselectChip();
}

/** \brief Handles complete command without reading/sending data.
 *
 *  \param cmd the command to be executed
 *  \param addr the address to the according command. ABSSF_noAddress means address will not be sent
 */
static void ABSSF_doCmdOnly(enum ABSSF_command cmd, uint32_t addr)
{
    ABSSF_selectChip();
    ABSSF_doCmdEntry(cmd, addr);
    ABSSF_unselectChip();
}

/** \brief Initiates any flash command
 *  \details Commands are always starting with the command ID which id optional
 *           followed by a 3 byte address.
 *           
 *  \param cmd the command to be executed
 *  \param addr the address to the according command. ABSSF_noAddress means address will not be sent
 */
static void ABSSF_doCmdEntry(enum ABSSF_command cmd, uint32_t addr)
{ /* command,  address high byte,   address middle byte, address low byte */
    uint8_t buf[] = {(uint8_t)cmd, (uint8_t)(addr >> 16u), (uint8_t)(addr >> 8u), (uint8_t)(addr)};
    uint32_t size = (addr == ABSSF_noAddress) ? sizeof(uint8_t) : sizeof(buf);
    ABSSF_writeData(size, buf);
}

/** \brief Limits the write size according SPI flash data sheet.
 *  \details during programming a page border shall not be included,
 *  the maximum size to write is 0x100.
 *
 *  \param addr address to write to
 *  \param size the number of bytes to be written
 *  \return limited size to be written in a single access to the given address
 */
static uint32_t ABSSF_limitWriteSize(uint32_t addr, uint32_t size)
{
    const uint32_t page_size = 0x100u;            // might be different for other SPI flashes
    const uint32_t page_start = addr % page_size; // start address inside page
    if ((size + page_start) > page_size)
    { // size to next page border
        size = page_size - page_start;
    }
    return size;
}
