#include "spi_abstraction.h"
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(spi_abstraction, LOG_LEVEL_DBG);

// Get the SPI device from device tree
// Assuming flexcomm1 is the first instance of "nxp,lpc-spi" compatible device
#define SPI_DEV_NODE DT_NODELABEL(flexcomm1)

#if !DT_NODE_HAS_STATUS(SPI_DEV_NODE, okay)
#error "SPI device not enabled in device tree"
#endif

static const struct device *spi_dev = DEVICE_DT_GET(SPI_DEV_NODE);

int spi_abstraction_init(void)
{
    LOG_DBG("Checking SPI device readiness for %s...", spi_dev->name);
    if (!device_is_ready(spi_dev)) {
        LOG_ERR("SPI device %s not ready!", spi_dev->name);
        return -ENODEV;
    }
    LOG_INF("SPI device %s is ready.", spi_dev->name);
    return 0;
}

int spi_abstraction_send(const uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len)
{
    struct spi_buf tx_buf = {
        .buf = (void *)tx_data,
        .len = tx_len
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };

    struct spi_buf rx_buf = {
        .buf = rx_data,
        .len = rx_len
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1
    };

    struct spi_config spi_cfg = {
        .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER,
        .frequency = 8000000, // Matches device tree
        .slave = 0, // Assuming single slave on CS0
        .cs = SPI_CS_CONTROL_INIT(SPI_DEV_NODE, 0),
    };

    LOG_DBG("Calling spi_transceive for %s...", spi_dev->name);
    int ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);
    if (ret) {
        LOG_ERR("SPI transceive failed for %s: %d", spi_dev->name, ret);
    } else {
        LOG_DBG("SPI transceive successful for %s.", spi_dev->name);
    }
    return ret;
}

static spi_rx_callback_t g_spi_rx_callback = NULL;
static struct k_msgq *g_spi_rx_msg_queue = NULL;

void spi_abstraction_register_rx_callback(spi_rx_callback_t callback)
{
    g_spi_rx_callback = callback;
    LOG_DBG("SPI RX callback registered.");
}

void spi_abstraction_set_rx_msg_queue(struct k_msgq *msg_q)
{
    g_spi_rx_msg_queue = msg_q;
    LOG_DBG("SPI RX message queue set.");
}