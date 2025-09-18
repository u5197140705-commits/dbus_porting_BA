#include "spi_abstraction.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>
#include "dbus_app_layer.h" // For dbal_spi_rx_msg and K_MSGQ

// Placeholder for the SPI device pointer
static const struct device *spi_dev;

// Global SPI configuration
static struct spi_config spi_cfg = {
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER,
    .frequency = 1000000, // 1 MHz
    .slave = 0 // Assuming slave select 0
};

// Static variable to store the registered RX callback
static spi_rx_callback_t rx_callback = NULL;

// Static variable to store the message queue for received data
static struct k_msgq *spi_rx_msg_queue = NULL;

// Placeholder for the SPI ISR
static void spi_rx_isr(const struct device *dev, void *user_data) {
    // This is a placeholder ISR. In a real implementation, this would read data
    // from the SPI peripheral. For now, we simulate receiving a message.
    // The actual data reception mechanism (e.g., DMA, polling in ISR) is board-specific.

    // For demonstration, let's assume we received a dummy message
    static uint8_t dummy_rx_data[DBAL_SPI_RX_MSG_MAX_SIZE] = {
        SPI_SOF_BYTE, 0x0A, 0xCC, // SOF, Length (10 bytes), Dummy CRC
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A // Dummy payload
    };
    uint8_t dummy_rx_len = 13; // SPI_HEADER_LEN + 10 bytes payload

    struct dbal_spi_rx_msg rx_msg;
    if (dummy_rx_len <= DBAL_SPI_RX_MSG_MAX_SIZE) {
        memcpy(rx_msg.data, dummy_rx_data, dummy_rx_len);
        rx_msg.len = dummy_rx_len;

        if (spi_rx_msg_queue != NULL) {
            if (k_msgq_put(spi_rx_msg_queue, &rx_msg, K_NO_WAIT) != 0) {
                printk("SPI_ERROR: Failed to put RX message into queue (queue full).\n");
            } else {
                printk("SPI: RX ISR put message into queue (len: %u).\n", rx_msg.len);
            }
        } else {
            printk("SPI_ERROR: RX message queue not set in ISR.\n");
        }
    } else {
        printk("SPI_ERROR: Received data too large for buffer in ISR.\n");
    }
}

// Initializes the SPI abstraction layer.
bool spi_abstraction_init(void)
{
    spi_dev = DEVICE_DT_GET(DT_ALIAS(spi0)); // Use DT_ALIAS for SPI device

    if (!device_is_ready(spi_dev)) {
        printk("SPI: Device %s is not ready\n", spi_dev->name);
        return false;
    }

    printk("SPI: Abstraction layer initialized.\n");

    // Configure SPI peripheral for interrupt-driven operation.
    // NOTE: The actual interrupt line setup (GPIO, IRQ controller) is board-specific
    // and typically handled via Device Tree overlays. This is a placeholder for
    // enabling the SPI RX interrupt at the driver level if supported.
    // For a real implementation, you would need to consult your board's documentation
    // and Zephyr's SPI driver API for interrupt configuration.
    // Example (conceptual):
    // spi_set_interrupt_handler(spi_dev, spi_rx_isr, NULL);
    // spi_enable_rx_interrupt(spi_dev);
    printk("SPI: Placeholder for interrupt configuration executed.\n");

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

    if (spi_write(spi_dev, &spi_cfg, &tx_bufs) != 0) {
        printk("SPI: Failed to send message\n");
        return false;
    }
    return true;
}

// Receives data over SPI.
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

    if (spi_read(spi_dev, &spi_cfg, &rx_bufs) != 0) {
        printk("SPI: Failed to receive message\n");
        return false;
    }
    return true;
}

// Sends and receives data over SPI (full-duplex).
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

    if (spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs) != 0) {
        printk("SPI: Failed to transceive message\n");
        return false;
    }
    return true;
}