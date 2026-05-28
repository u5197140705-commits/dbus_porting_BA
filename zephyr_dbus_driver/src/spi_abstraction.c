#include "spi_abstraction.h"
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include "dbus_driver_public.h"

LOG_MODULE_REGISTER(spi_abstraction, LOG_LEVEL_ERR);

// Get the SPI device from device tree
// Assuming flexcomm1 is the first instance of "nxp,lpc-spi" compatible device
#define SPI_DEV_NODE DT_NODELABEL(flexcomm1)
#define DBAL_SPI_FREQUENCY_HZ 10000u

#if !DT_NODE_HAS_STATUS(SPI_DEV_NODE, okay)
#error "SPI device not enabled in device tree"
#endif

static const struct device *spi_dev = DEVICE_DT_GET(SPI_DEV_NODE);
static const struct device *cs_gpio_dev = DEVICE_DT_GET(DT_NODELABEL(hsgpio0));

#define DBUS_CS_GPIO_PRIMARY_PIN   6u
#define DBUS_CS_GPIO_SECONDARY_PIN 10u

static gpio_pin_t spi_abstraction_get_target_cs_pin(void)
{
    enum DBCDRV_SpiTarget target = DBCDRV_getSpiTarget();

    if (target == DBCDRV_SPI_TARGET_SECONDARY_PICO) {
        return (gpio_pin_t)DBUS_CS_GPIO_SECONDARY_PIN;
    }

    return (gpio_pin_t)DBUS_CS_GPIO_PRIMARY_PIN;
}

static int spi_abstraction_set_cs(gpio_pin_t pin, bool active)
{
    if (!device_is_ready(cs_gpio_dev)) {
        return -ENODEV;
    }

    return gpio_pin_set(cs_gpio_dev, pin, active ? 0 : 1);
}

int spi_abstraction_init(void)
{
    LOG_DBG("Checking SPI device readiness for %s...", spi_dev->name);
    if (!device_is_ready(spi_dev)) {
        LOG_ERR("SPI device %s not ready!", spi_dev->name);
        return -ENODEV;
    }

    if (!device_is_ready(cs_gpio_dev)) {
        LOG_ERR("CS GPIO device not ready!");
        return -ENODEV;
    }

    if (gpio_pin_configure(cs_gpio_dev, DBUS_CS_GPIO_SECONDARY_PIN, GPIO_OUTPUT_HIGH) < 0) {
        LOG_ERR("Failed to configure secondary CS pin %u", DBUS_CS_GPIO_SECONDARY_PIN);
        return -EIO;
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
        /* Keep DBAL event traffic at the same conservative rate as the
         * proven-stable legacy register path. The Pico slave is polled in
         * software and sporadically truncated 20-byte DBAL frames at 8 MHz. */
        .frequency = DBAL_SPI_FREQUENCY_HZ,
        .slave = 0, // Assuming single slave on CS0
        .cs = NULL,
    };

    enum DBCDRV_SpiTarget target = DBCDRV_getSpiTarget();
    gpio_pin_t cs_pin = spi_abstraction_get_target_cs_pin();

    /* Leave GPIO6 to Flexcomm HW SSEL0; manual CS only on GPIO10. */
    if (target == DBCDRV_SPI_TARGET_SECONDARY_PICO) {
        (void)spi_abstraction_set_cs((gpio_pin_t)DBUS_CS_GPIO_SECONDARY_PIN, false);
        if (spi_abstraction_set_cs(cs_pin, true) < 0) {
            LOG_ERR("Failed to assert CS pin %u", (unsigned)cs_pin);
            return -EIO;
        }
    }

    LOG_DBG("Calling spi_transceive for %s...", spi_dev->name);
    int ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);

    if (target == DBCDRV_SPI_TARGET_SECONDARY_PICO) {
        (void)spi_abstraction_set_cs(cs_pin, false);
    }

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