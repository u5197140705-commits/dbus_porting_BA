/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <stdlib.h>

#include "bshg_spi.h"

#define BSHG_SPI_BUFFER_LENGTH 500
#define BSHG_SPI DT_NODELABEL(flexcomm1)

static const struct device *const BSHG_spiDev = DEVICE_DT_GET(BSHG_SPI);

struct spi_dt_spec BSHG_devSpec =
{
	.bus = BSHG_spiDev,
	.config = {
		.frequency = 125000u,
		.operation = SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8), // Mode 0 (CPOL=0, CPHA=0) are default
		.slave = 0u,
		.cs = { .gpio = { .port = NULL, .pin = 0, .dt_flags = 0 } } // Explicitly initialize cs as a struct
	}
};

static uint8_t BSHG_safbuf_testdata[BSHG_SPI_BUFFER_LENGTH] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
uint8_t BSHG_safbuf2[BSHG_SPI_BUFFER_LENGTH]; // Removed static, now extern
struct spi_buf BSHG_txb =
{
	/** Valid pointer to a data buffer, or NULL otherwise */
	.buf = BSHG_safbuf_testdata,
	/** Length of the buffer @a buf in bytes.
	 * If @a buf is NULL, length which as to be sent as dummy bytes (as TX
	 * buffer) or the length of bytes that should be skipped (as RX buffer).
	 */
	.len = 10
};

struct spi_buf BSHG_rxb =
{
	/** Valid pointer to a data buffer, or NULL otherwise */
	.buf = BSHG_safbuf2,
	/** Length of the buffer @a buf in bytes.
	 * If @a buf is NULL, length which as to be sent as dummy bytes (as TX
	 * buffer) or the length of bytes that should be skipped (as RX buffer).
	 */
	.len = 10
};

struct spi_buf_set BSHG_txBufs =
{
	/** Pointer to an array of spi_buf, or NULL */
	.buffers = (const struct spi_buf *)&BSHG_txb,
	/** Length of the array (number of buffers) pointed by @a buffers */
	.count = 1u
};

struct spi_buf_set BSHG_rxBufs =
{
	/** Pointer to an array of spi_buf, or NULL */
	.buffers = (const struct spi_buf *)&BSHG_rxb,
	/** Length of the array (number of buffers) pointed by @a buffers */
	.count = 1u
};





//return 0 for success
int BSHG_spiTxRxShortcutFromHexstring(const char* data, int length)
{
    if(spi_is_ready_dt(&BSHG_devSpec)==true)
    {
        BSHG_spiSetMsgDataFromHexstring(data, length);
        return spi_transceive(	BSHG_spiDev,
                                &BSHG_devSpec.config,
                                (const struct spi_buf_set *)&BSHG_txBufs,
                                (const struct spi_buf_set *)&BSHG_rxBufs);
    }
    else
    {
        return 1;
    }
}

int BSHG_spiTxRxShortcutFromBinarray(const uint8_t* data, int length)
{
    if(spi_is_ready_dt(&BSHG_devSpec)==true)
    {
        BSHG_spiSetMsgDataFromBinArray(data, length);
        return spi_transceive(	BSHG_spiDev, &BSHG_devSpec.config,
                                  &BSHG_txBufs,
                                  &BSHG_rxBufs);
    }
    else
    {
        return 1;
    }
}

void BSHG_spiSetMsgDataFromHexstring(const char* data, int length)
{
    uint16_t i,j=0u;
    char byteVal[3] = {0u,0u,0u};
    BSHG_txb.len = length;
    BSHG_rxb.len = length;

    for(i=0u; i<length; i++)
    {
        byteVal[0] = data[j];
        byteVal[1] = data[j+1];
        BSHG_safbuf_testdata[i] = strtoul(byteVal, NULL, 16);
        j+=2u;
    }
}

void BSHG_spiSetMsgDataFromBinArray(const uint8_t* data, int length)
{
    uint16_t i = 0u;
    BSHG_txb.len = length;
    BSHG_rxb.len = length;

    if(length > BSHG_SPI_BUFFER_LENGTH)
    {
        length = BSHG_SPI_BUFFER_LENGTH; // Limit
    }

    for(i=0u; i<length; i++)
    {
        BSHG_safbuf_testdata[i] = data[i];
    }
}