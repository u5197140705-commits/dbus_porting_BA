#include "spi_abstraction.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>

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

// Placeholder for the SPI ISR
static void spi_rx_isr(const struct device *dev, void *user_data) {
    // In a real implementation, this ISR would read data from the SPI peripheral
    // and then call the registered rx_callback.
    // For now, it's a placeholder.
    printk("SPI: RX ISR triggered (placeholder).\n");
    // Example: if (rx_callback) { rx_callback(received_data, received_len); }
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

    // TODO: Configure SPI peripheral for interrupt-driven operation here.
    // This would involve setting up interrupt lines, enabling SPI RX interrupts,
    // and associating spi_rx_isr with the appropriate interrupt.
    // For example: spi_set_cs_gpio(spi_dev, &cs_gpio);
    //              spi_set_interrupt_handler(spi_dev, spi_rx_isr, NULL);
    //              spi_enable_rx_interrupt(spi_dev);

    return true;
}

// Registers a callback function for SPI receive interrupts.
void spi_abstraction_register_rx_callback(spi_rx_callback_t callback) {
    rx_callback = callback;
    printk("SPI: RX callback registered.\n");
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