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
 *  COMP_ABBREV      DBC
 ******************************************************************************/


#ifndef DBUSCAN_TYPES_H
#define DBUSCAN_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Public type definitions for unit dbuscan
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan.h"
#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
    #include "bustypes.h"   // TbusMessageIdentifier
#endif


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** \brief   DBus node address to be set in the DBusCAN chip
 */
#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
#define DBC_DBUS_NODE_ADDRESS    __DBUS_NODE_ADDRESS
#else
#define DBC_DBUS_NODE_ADDRESS    0u /* dummy value */
#endif


/** \brief   Definition of buffer size needed for data transmission
 */
#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
    #define DBC_WRITE_BUFFER_SIZE    (DBC_DBUS_TXF_HDR_SIZE + BUS_MESSAGE_ID_LEN + DLL_TRANSMIT_BUFFER_DATA_LENGTH + DBC_DBUS_CRC_SIZE)
#else
    #define DBC_WRITE_BUFFER_SIZE    (DBC_DBUS_BL_TXF_HDR_SIZE + 0x104u)  /* word-aligned buffer size for 0x100 BL data block size + BL checksum byte */
#endif


/** \brief    Macro for checking return value of a function.
 *
 *  \details  If a function does not return DBC_OK, parent
 *            function is exited with the same error value
 */
#define DBC_RETURN_ON_ERROR(function) do {enum DBC_Error retVal = (function); if(retVal != DBC_OK) {return retVal;}} while(false);//lint !e823 "definition of macro ends in semi-colon"


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief   Possible results of DBusCAN chip configuration
 *
 *  \details It is used as a return value for some of
 *           interface functions of the dbuscan component.
 */
enum DBC_Error
{
    DBC_OK = 0,    ///< Operation was successful
    DBC_ERROR      ///< Operation failed
};


/** \brief    Possible types of reset for the DBusCAN chip.
 */
enum DBC_Reset
{
    DBC_RST_DBUS_SOFT = DBC_DBUS_BCC_RST_MASK,     ///< Causes reset of the DBus Rx/Tx buffers and interrupts of the DBusCAN chip.
    DBC_RST_DBUS_HARD = DBC_DBUS_BCC_CSR_RST_MASK, ///< Causes reset of the whole DBus part of the DBusCAN chip (DBus Rx/Tx buffers, interrupts and registers).
    DBC_RST_FULL                                   ///< Causes reset of the whole DBusCAN chip.
};


/** \brief   Power modes of the DBusCAN chip
 */
enum DBC_PowerMode
{
    DBC_POWER_MODE_SLEEP   = 0,
    DBC_POWER_MODE_STANDBY = 1,
    DBC_POWER_MODE_NORMAL  = 2
};


/** \brief   Possible clock input frequency for the DBusCAN chip
 *
 *  \details One of the values listed is set in DBUS_DBR register
 *           of the DBusCAN chip. This set value must correspond
 *           with the real clock source set for the DBusCAN chip.
 */
enum DBC_ClockInput
{
    DBC_CLKIN_20M = 0,    ///< 20 MHz clock input
    DBC_CLKIN_40M = 1,    ///< 40 MHz clock input
    DBC_CLKIN_80M = 2     ///< 80 MHz clock input
};


/** \brief   Header data for communication with the DBusCAN chip over SPI
 *
 *  \details This SPI header data has to be added at the start of
 *           each SPI frame transferred to the DBusCAN chip.
 */
enum DBC_SpiHdrBytes
{
    DBC_SPI_HDR_BYTE_CMD,       ///< The operation code (command), which defines whether the frame is a read or a write (as well as byte order)
    DBC_SPI_HDR_BYTE_ADDR_HIGH, ///< The start address of the register that is being accessed (high byte)
    DBC_SPI_HDR_BYTE_ADDR_LOW,  ///< The start address of the register that is being accessed (low byte)
    DBC_SPI_HDR_BYTE_DATA_LEN   ///< The number of words (4 bytes per word) to be transferred
};


/** \brief   Operation codes for reading/writing data from the DBusCAN chip
 */
enum DBC_command
{
    DBC_READ_L  = 0x40u,    ///< Read  - Low data byte first
    DBC_READ_H  = 0x41u,    ///< Read  - High data byte first
    DBC_WRITE_L = 0x60u,    ///< Write - Low data byte first
    DBC_WRITE_H = 0x61u     ///< Write - High data byte first
};


/** \brief   Possible DBus baud rates to be set in the DBusCAN chip
 */
enum DBC_DbusBaudrate
{
    DBC_DBUS_BAUD_9600  = 0,
    DBC_DBUS_BAUD_19200 = 1,
    DBC_DBUS_BAUD_38400 = 2,
    DBC_DBUS_BAUD_57600 = 3,
    DBC_DBUS_BAUD_125K  = 4,
    DBC_DBUS_BAUD_250K  = 5,
    DBC_DBUS_BAUD_500K  = 6,
    DBC_DBUS_BAUD_1M    = 7
};


/** \brief   Type definition of buffer for data transmission to the DBusCAN chip over SPI
 *
 *  \details Consists of 4-byte SPI header + data
 */
typedef PACKED struct
{
    uint8_t spiHdr[DBC_SPI_HDR_SIZE];
    PACKED union
    {
        uint8_t           bytes[DBC_WRITE_BUFFER_SIZE + DBC_SPI_CRC_SIZE];
        uint32_t          words[WORD_SIZEOF(DBC_WRITE_BUFFER_SIZE + DBC_SPI_CRC_SIZE)];
        DBC_DBUS_TXF_t    dbusTxfHdr;
        DBC_DBUS_BL_TXF_t dbusBlTxfHdr;
    };
} DBC_WriteBuf_t;


/** \brief   Type definition of buffer for reading all interrupt flags at once from the DBusCAN chip over SPI
 */
union DBC_IrqBuf
{
    //! @brief Buffer as a structure
    struct _irq
    {
        uint32_t spiHdr;      ///< Reserved word due to the SPI header data
        uint32_t globalFlags; ///< Position of global (device) interrupt flags
        uint32_t canFlags;    ///< Position of CAN interrupt flags
        uint32_t dbusFlags;   ///< Position of DBus interrupt flags
#ifdef DBUSCAN_SPI_CRC_USED
        uint32_t crc;         ///< Position of CRC value
#endif
    } irq;                    ///< Buffer defined as a structure
    //! @brief Buffer as an array
    uint8_t irqArray[sizeof(struct _irq)];
};


union DBC_Hdr
{
    struct _hdr_t
    {
        uint8_t cmd;
        uint8_t addrHigh;
        uint8_t addrLow;
        uint8_t dataLen;
    } t;
    uint8_t array[sizeof(struct _hdr_t)];
    uint32_t word;
};


union DBC_SpiBuf
{
    union DBC_Hdr hdr;
    struct _d0
    {
        union DBC_Hdr hdr;
        uint32_t data0;
#ifdef DBUSCAN_SPI_CRC_USED
        uint32_t crc;         ///< Position of CRC value
#endif
    } d0;
    struct _d1
    {
        union DBC_Hdr hdr;
        uint32_t data0;
        uint32_t data1;
#ifdef DBUSCAN_SPI_CRC_USED
        uint32_t crc;         ///< Position of CRC value
#endif
    } d1;
    struct _d2
    {
        union DBC_Hdr hdr;
        uint32_t data0;
        uint32_t data1;
        uint32_t data2;
#ifdef DBUSCAN_SPI_CRC_USED
        uint32_t crc;         ///< Position of CRC value
#endif
    } d2;                     ///< Buffer defined as a structure
    uint8_t array[sizeof(struct _d2)];
    uint32_t word[sizeof(struct _d2) / WORD_SIZE];
#ifdef DBUSCAN_SPI_CRC_USED
    DBC_WriteBuf_t txBuf;
#endif
};


/** \brief   Type definition for structure used for the DBusCAN chip configuration
 */
typedef DBC_EEP_t DBC_Cfg_t;


#ifdef __cplusplus
}
#endif

#endif // DBUSCAN_TYPES_H
