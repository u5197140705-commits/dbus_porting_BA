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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Implementation of drivers used for BP2 communication via DBusCAN chip
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_drv.h"
#include "dbuscan_bl.h"
#include "utility.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define DBCBL_RX_BUFFER_SIZE  (260u) ///< The max. data size received in bootloader mode (aligned to word size)


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

uint8_t* DBCBL_readData(uint16_t dataSize)
{
    static uint8_t spiBuffer[DBCBL_RX_BUFFER_SIZE + DBC_SPI_HDR_SIZE];

    dataSize = (uint16_t)UTI_WORD_CEIL((uint32_t)dataSize);
    if (dataSize > DBCBL_RX_BUFFER_SIZE)
    {
        DBCBL_clearRxFifo(); // discard too big data from Rx FIFO
        return NULL;
    }
    (void)DBCDRV_read(DBC_DBUS_RX_TX_FIFO_ADDR, spiBuffer, dataSize, DBC_READ_L);
    return &spiBuffer[DBC_SPI_HDR_SIZE];
}

enum DBC_Error DBCBL_sendData(const uint8_t *dataBuf, uint16_t dataSize)
{
    enum DBC_Error retState = DBC_ERROR;
    uint32_t regVal;
    const uint16_t txBufSize = (uint16_t)(DBC_WRITE_BUFFER_SIZE - DBC_DBUS_BL_TXF_HDR_SIZE);
    static DBC_WriteBuf_t txBuf;

    if (0u != dataSize)
    {
        // Verify that Tx FIFO has free element to accept new frame
        regVal = DBCDRV_readReg32(DBC_DBUS_TXFQS_ADDR);
        uint8_t txFifoFreeLvl = (uint8_t)(regVal & DBC_DBUS_TXFQS_TFFL_MASK);
        if ((txFifoFreeLvl > 0u) && (txFifoFreeLvl <= DBC_DBUS_FIFO_MAX_MSG_CNT))
        {
            // Verify that Tx FIFO has enough space for the whole frame
            if (dataSize <= ((DBC_DBUS_TXFQS_TFDA_MASK & regVal) >> DBC_DBUS_TXFQS_TFDA_POS))
            {
                if (dataSize > txBufSize)
                {
                    // Data length is longer than the current transmit buffer, reduce to maximum buffer length.
                    dataSize = txBufSize;
                }
                txBuf.dbusBlTxfHdr.MSG_LEN = dataSize;
                memcpy(&txBuf.bytes[DBC_DBUS_BL_TXF_DATA_OFFSET], dataBuf, dataSize); //lint !e669 possible data overrun- this is handled by the check above

                dataSize = (uint16_t)UTI_WORD_CEIL((uint32_t)dataSize + DBC_DBUS_BL_TXF_HDR_SIZE); // Align data size for writing so that we avoid SPI overflow error
                retState = DBCDRV_write(DBC_DBUS_RX_TX_FIFO_ADDR, &txBuf, dataSize, DBC_WRITE_L);
            }
        }
    }
    return retState;
}

bool DBCBL_isTxOngoing(void)
{
    uint32_t irqFlags;

    if (true == DBCDRV_isIrqEvent())
    {
        irqFlags = DBCDRV_readReg32(DBC_DBUS_IR_ADDR);
        if (0u != (irqFlags & DBC_DBUS_IR_TEFN_MASK))
        {
            // Clear both DBus Tx Status FIFO new entry flag and DBus Tx FIFO empty flag
            (void)DBCDRV_writeReg32(DBC_DBUS_IR_ADDR, (uint32_t)(DBC_DBUS_IR_TEFN_MASK | DBC_DBUS_IR_TFE_MASK));
            return false;
        }
    }
    return true;
}

bool DBCBL_clearIrq(void)
{
    bool isRxError = false;
    static union DBC_IrqBuf irqBuf;  //lint !e9018 union used needed and not dangerous

    if (true == DBCDRV_isIrqEvent())
    {
        if (DBC_OK != DBCDRV_readIrq(&irqBuf))
        {
            return true;
        }
        if (0u != (~DBC_IF_GLOBAL_FAULT_FLAG_MASK & irqBuf.irq.globalFlags))  // the last eight global interrupt flags are read-only
        {
            // clear global interrupt flags
            (void)DBCDRV_writeReg32(DBC_IF_ADDR, irqBuf.irq.globalFlags);
            isRxError = true;
        }
        if (0u != (DBC_IF_SPIERR_MASK & irqBuf.irq.globalFlags))
        {
            // SPI error - clear SPI status flags
            uint32_t intFlags = DBCDRV_readReg32(DBC_STATUS_ADDR);
            (void)DBCDRV_writeReg32(DBC_STATUS_ADDR, intFlags);
            isRxError = true;
        }
        if (0u != (DBC_IF_DBUS_CAN_MASK & irqBuf.irq.globalFlags))
        {
            if (0u != (irqBuf.irq.dbusFlags & ~(DBC_DBUS_IR_RF0N_MASK | DBC_DBUS_IR_DBUSSLNT_MASK)))
            {   // consider as receive error every flag except the DBUS_IR_RF0N and DBUS_IR_DBUSSLNT flag
                isRxError = true;
            }
            if (0u != (irqBuf.irq.dbusFlags & ~DBC_DBUS_IR_RF0N_MASK))
            {   // clear all DBus flags except the DBUS_IR_RF0N flag
                (void)DBCDRV_writeReg32(DBC_DBUS_IR_ADDR, irqBuf.irq.dbusFlags);
            }
        }
    }
    return isRxError;
}

void DBCBL_readTxStatus(void)
{
    (void)DBCDRV_read(DBC_DBUS_TXSF_ADDR, NULL, DBC_DBUS_TXSF_SIZE, DBC_READ_L);
}

void DBCBL_clearRxFifo(void)
{
    (void)DBCDRV_writeReg32(DBC_DBUS_BCC_ADDR, DBC_DBUS_BCC_RXFIFO_CLR_MASK);
}

uint16_t DBCBL_getRxDataSize(void)
{
    uint32_t rxDataSize = DBCDRV_readReg32(DBC_DBUS_RXF0S_ADDR);
    rxDataSize  &= DBC_DBUS_RXF0S_RF0MS_MASK;
    rxDataSize >>= DBC_DBUS_RXF0S_RF0MS_POS;
    return (uint16_t)rxDataSize;
}
