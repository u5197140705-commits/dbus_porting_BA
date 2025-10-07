#ifndef ZEPHYR_DBUS_DRIVER_INC_SPI_ABSTRACTION_H_
#define ZEPHYR_DBUS_DRIVER_INC_SPI_ABSTRACTION_H_

#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/kernel.h> // Required for struct k_msgq

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the SPI abstraction layer.
 *
 * This function initializes the underlying SPI device.
 *
 * @return 0 on success, or a negative errno code on failure.
 */
int spi_abstraction_init(void);

/**
 * @brief Sends and receives data over SPI.
 *
 * This function performs a full-duplex SPI transfer.
 *
 * @param tx_data Pointer to the transmit buffer.
 * @param tx_len Length of the transmit buffer in bytes.
 * @param rx_data Pointer to the receive buffer.
 * @param rx_len Length of the receive buffer in bytes.
 *
 * @return 0 on success, or a negative errno code on failure.
 */
int spi_abstraction_send(const uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len);

// Forward declaration for the message queue structure
struct dbal_spi_rx_msg;

/**
 * @brief Type definition for the SPI RX callback function.
 *
 * @param data Pointer to the received data buffer.
 * @param len Length of the received data in bytes.
 */
typedef void (*spi_rx_callback_t)(const uint8_t *data, uint8_t len);

/**
 * @brief Registers a callback function for SPI received data.
 *
 * @param callback The function to be called when SPI data is received.
 */
void spi_abstraction_register_rx_callback(spi_rx_callback_t callback);

/**
 * @brief Sets the message queue for SPI received data.
 *
 * @param msg_q Pointer to the K_MSGQ object where received data will be put.
 */
void spi_abstraction_set_rx_msg_queue(struct k_msgq *msg_q);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_DBUS_DRIVER_INC_SPI_ABSTRACTION_H_ */