#include "spi_abstraction.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h> // Required for K_MSEC_TO_MS
#include <zephyr/timing/timing.h> // Potentially required for K_MSEC_TO_MS
#include "dbus_app_layer.h" // For dbal_spi_rx_msg and K_MSGQ

#define SPI_RX_TIMEOUT_MS 100 // Define a timeout for SPI receive operations in milliseconds

// Define K_MSEC_TO_MS if not already defined (workaround for build issues)
#ifndef K_MSEC_TO_MS
#define K_MSEC_TO_MS(ms) (ms)
#endif

// Placeholder for the SPI device pointer
static const struct device *spi_dev;

// Global SPI configuration
static struct spi_config spi_cfg = {
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_SLAVE,
    .frequency = 1000000, // 1 MHz
    .slave = 0, // Assuming slave select 0
    // In slave mode, the CS line is controlled by the master.
    // No explicit SPI_HOLD_ON_CS or SPI_CS_ACTIVE_HIGH flags are needed here for slave operation,
    // as the slave reacts to the master's CS assertion.
};

// Static variable to store the registered RX callback
static spi_rx_callback_t rx_callback = NULL;

// Static variable to store the message queue for received data
static struct k_msgq *spi_rx_msg_queue = NULL;

// Buffer for asynchronous SPI receive
static uint8_t spi_async_rx_buffer[DBAL_SPI_RX_MSG_MAX_SIZE];

// SPI buffer set for asynchronous receive
static struct spi_buf spi_rx_buf = {
    .buf = spi_async_rx_buffer,
    .len = DBAL_SPI_RX_MSG_MAX_SIZE
};
static struct spi_buf_set spi_rx_buf_set = {
    .buffers = &spi_rx_buf,
    .count = 1
};

// Callback for asynchronous SPI transfers
static void spi_transceive_callback(const struct device *dev, int result, void *data) {
    ARG_UNUSED(dev);
    ARG_UNUSED(data);

    if (result == 0) {
        struct dbal_spi_rx_msg rx_msg;
        // Assuming the entire buffer was filled or a specific length was received
        // For slave mode, spi_transceive_cb returns the number of frames received.
        // We'll assume the full buffer was intended to be filled for now.
        rx_msg.len = DBAL_SPI_RX_MSG_MAX_SIZE;
        memcpy(rx_msg.data, spi_async_rx_buffer, rx_msg.len);

        if (spi_rx_msg_queue != NULL) {
            if (k_msgq_put(spi_rx_msg_queue, &rx_msg, K_NO_WAIT) != 0) {
                printk("SPI_ERROR: Failed to put RX message into queue (queue full). Data lost.\n");
            } else {
                printk("SPI: Async RX callback put message into queue (len: %u).\n", rx_msg.len);
            }
        } else {
            printk("SPI_ERROR: RX message queue not set in async callback. Data lost.\n");
        }
    } else {
        printk("SPI_ERROR: Asynchronous SPI transfer failed with result: %d. Data might be incomplete or corrupted.\n", result);
    }

    // Re-arm the asynchronous receive for continuous operation
    if (spi_transceive_cb(spi_dev, &spi_cfg, NULL, &spi_rx_buf_set, spi_transceive_callback, NULL) != 0) {
        printk("SPI_CRITICAL: Failed to re-arm asynchronous SPI receive. Further RX operations may be halted.\n");
    }
}

// Function to handle SPI receive timeouts
void spi_rx_timeout_handler(struct k_timer *timer_id) {
    ARG_UNUSED(timer_id);
    printk("SPI_WARN: RX timeout occurred. No SPI data received within %d ms.\n", (int)K_MSEC_TO_MS(SPI_RX_TIMEOUT_MS));
    // Optionally, you could clear the RX buffer or reset the SPI peripheral here.
    // For now, we just log a warning.
}

// Timer for SPI receive timeout
K_TIMER_DEFINE(spi_rx_timeout_timer, spi_rx_timeout_handler, NULL);

// Initializes the SPI abstraction layer.
bool spi_abstraction_init(void)
{
    spi_dev = DEVICE_DT_GET(DT_NODELABEL(flexcomm1)); // Use DT_NODELABEL for SPI device

    if (!device_is_ready(spi_dev)) {
        printk("SPI: Device %s is not ready\n", spi_dev->name);
        return false;
    }

    printk("SPI: Abstraction layer initialized.\n");

    // Start asynchronous receive in slave mode
    if (spi_transceive_cb(spi_dev, &spi_cfg, NULL, &spi_rx_buf_set, spi_transceive_callback, NULL) != 0) {
        printk("SPI_CRITICAL: Failed to start asynchronous SPI receive. SPI RX functionality will not be available.\n");
        return false;
    }
    printk("SPI: Asynchronous receive started in slave mode.\n");

    // Start the receive timeout timer
    k_timer_start(&spi_rx_timeout_timer, K_MSEC(SPI_RX_TIMEOUT_MS), K_NO_WAIT);
    printk("SPI: RX timeout timer started.\n");

    return true;
}

// Registers a callback function for SPI receive interrupts.
void spi_abstraction_register_rx_callback(spi_rx_callback_t callback) {
    rx_callback = callback;
    printk("SPI: RX callback registered.\n");
}

void spi_abstraction_set_rx_msg_queue(struct k_msgq *msg_q) {
    spi_rx_msg_queue = msg_q;
    printk("SPI: RX message queue set.\n");
}

// Sends data over SPI.
bool spi_abstraction_send(const uint8_t *data, uint8_t len)
{
    struct spi_buf tx_buf = {
        .buf = (void *)data,
        .len = len
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };

    // In slave mode, spi_write will wait for the master to provide clock and CS.
    // This is a blocking call. For non-blocking, spi_transceive_cb would be used.
    if (spi_write(spi_dev, &spi_cfg, &tx_bufs) != 0) {
        printk("SPI_ERROR: Failed to send message over SPI. Check master connection and configuration.\n");
        return false;
    }
    return true;
}

// Receives data over SPI.
// This function is now less relevant with asynchronous receive, but kept for completeness.
bool spi_abstraction_receive(uint8_t *buffer, uint8_t len)
{
    struct spi_buf rx_buf = {
        .buf = (void *)buffer,
        .len = len
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1
    };

    // In slave mode, spi_read will wait for the master to provide clock and CS.
    // This is a blocking call.
    if (spi_read(spi_dev, &spi_cfg, &rx_bufs) != 0) {
        printk("SPI_ERROR: Failed to receive message over SPI. Check master connection and configuration.\n");
        return false;
    }
    return true;
}

// Sends and receives data over SPI (full-duplex).
// This function is now less relevant with asynchronous receive, but kept for completeness.
bool spi_abstraction_transceive(const uint8_t *tx_data, uint8_t *rx_buffer, uint8_t len)
{
    struct spi_buf tx_buf = {
        .buf = (void *)tx_data,
        .len = len
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };

    struct spi_buf rx_buf = {
        .buf = (void *)rx_buffer,
        .len = len
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1
    };

    // In slave mode, spi_transceive will wait for the master to provide clock and CS.
    // This is a blocking call.
    if (spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs) != 0) {
        printk("SPI_ERROR: Failed to transceive message over SPI. Check master connection and configuration.\n");
        return false;
    }
    return true;
}