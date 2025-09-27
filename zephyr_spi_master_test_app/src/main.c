#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "dbus_message.h"

LOG_MODULE_REGISTER(spi_master_test_app, LOG_LEVEL_INF);

/* SPI */
/* SPI */
#define SPI_MASTER_LABEL "FLEXCOMM1" // Label of the SPI master controller
#define SPI_SLAVE_LABEL "DBUS_SPI_SLAVE" // Label of the SPI slave device

/* CS GPIO */
#define CS_GPIO_LABEL "HSGPIO_0" // Label of the CS GPIO port
#define CS_GPIO_PIN 10 // Example pin, adjust as needed

static const struct device *spi_master_dev;
static const struct device *cs_gpio_port;
static struct gpio_dt_spec cs_gpio;

static struct spi_config spi_cfg = {
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
    .frequency = 4000000, // 4 MHz
    .slave = 0, // Chip select 0
};

#define MAX_SPI_FRAME_SIZE (SPI_HEADER_LEN + DBUS_MAX_PAYLOAD_LEN)
static uint8_t tx_buffer[MAX_SPI_FRAME_SIZE];
static uint8_t rx_buffer[MAX_SPI_FRAME_SIZE];

/**
 * @brief Sends an SPI frame and receives a response.
 *
 * @param tx_data Pointer to the transmit data buffer.
 * @param tx_len Length of the transmit data.
 * @param rx_data Pointer to the receive data buffer.
 * @param rx_len Expected length of the receive data.
 * @return 0 on success, negative errno on failure.
 */
static int spi_send_and_receive(const uint8_t *tx_data, size_t tx_len,
                                  uint8_t *rx_data, size_t rx_len)
{
    int ret;
    struct spi_buf tx_buf = {
        .buf = (void *)tx_data,
        .len = tx_len
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };

    struct spi_buf rx_buf = {
        .buf = (void *)rx_data,
        .len = rx_len
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1
    };

    // Assert CS low
    gpio_pin_set_dt(&cs_gpio, 0);
    k_sleep(K_USEC(10)); // Small delay for slave to react

    ret = spi_transceive(spi_master_dev, &spi_cfg, &tx_bufs, &rx_bufs);
    if (ret < 0) {
        LOG_ERR("SPI transceive failed: %d", ret);
    }

    // Assert CS high
    gpio_pin_set_dt(&cs_gpio, 1);
    k_sleep(K_USEC(10)); // Small delay

    return ret;
}

void main(void)
{
    int ret;

    LOG_INF("SPI Master Test Application Started");

    spi_master_dev = device_get_binding(SPI_MASTER_LABEL);
    if (!spi_master_dev) {
        LOG_ERR("SPI master device %s not found!", SPI_MASTER_LABEL);
        return;
    }

    cs_gpio_port = device_get_binding(CS_GPIO_LABEL);
    if (!cs_gpio_port) {
        LOG_ERR("CS GPIO port %s not found!", CS_GPIO_LABEL);
        return;
    }

    cs_gpio.port = cs_gpio_port;
    cs_gpio.pin = CS_GPIO_PIN;
    cs_gpio.dt_flags = GPIO_ACTIVE_LOW; // Assuming active low CS

    ret = gpio_pin_configure_dt(&cs_gpio, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure CS GPIO pin %d: %d", cs_gpio.pin, ret);
        return;
    }

    LOG_INF("SPI Master and CS GPIO configured.");

    k_sleep(K_SECONDS(2)); // Give slave time to initialize

    while (1) {
        dbus_message_t cmd_msg;
        uint8_t cmd_data[] = {0x01, 0x02, 0x03, 0x04}; // Example command data
        size_t frame_len;

        // Scenario 1.1: Basic Command-Response
        LOG_INF("--- Scenario 1.1: Basic Command-Response ---");
        if (dbus_message_create_command(&cmd_msg, 0x1001, 0x0001, cmd_data, sizeof(cmd_data))) {
            if (dbus_message_to_spi_frame(&cmd_msg, tx_buffer, sizeof(tx_buffer), &frame_len)) {
                LOG_HEXDUMP_INF(tx_buffer, frame_len, "Sending Command:");

                ret = spi_send_and_receive(tx_buffer, frame_len, rx_buffer, MAX_SPI_FRAME_SIZE);
                if (ret == 0) {
                    LOG_HEXDUMP_INF(rx_buffer, MAX_SPI_FRAME_SIZE, "Received Response:");
                    // Further parsing of the response can be added here
                } else {
                    LOG_ERR("Failed to send/receive SPI command.");
                }
            } else {
                LOG_ERR("Failed to convert DBus message to SPI frame.");
            }
        } else {
            LOG_ERR("Failed to create DBus command message.");
        }

        k_sleep(K_SECONDS(5)); // Wait before sending next command
    }
}