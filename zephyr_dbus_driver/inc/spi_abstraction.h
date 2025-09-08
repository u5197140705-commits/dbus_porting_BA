#ifndef ZEPHYR_SPI_ABSTRACTION_H__
#define ZEPHYR_SPI_ABSTRACTION_H__

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the SPI abstraction layer.
 *
 * This function initializes the underlying Zephyr SPI device.
 *
 * @return True if initialization is successful, false otherwise.
 */
bool spi_abstraction_init(void);

/**
 * @brief Sends data over SPI.
 *
 * @param data Pointer to the data to send.
 * @param len Length of the data.
 *
 * @return True if the data is successfully sent, false otherwise.
 */
bool spi_abstraction_send(const uint8_t *data, uint8_t len);

/**
 * @brief Receives data over SPI.
 *
 * @param buffer Pointer to the buffer to store received data.
 * @param len Length of the data to receive.
 *
 * @return True if data is successfully received, false otherwise.
 */
bool spi_abstraction_receive(uint8_t *buffer, uint8_t len);

/**
 * @brief Sends and receives data over SPI (full-duplex).
 *
 * @param tx_data Pointer to the data to send.
 * @param rx_buffer Pointer to the buffer to store received data.
 * @param len Length of the data to send/receive.
 *
 * @return True if the transaction is successful, false otherwise.
 */
bool spi_abstraction_transceive(const uint8_t *tx_data, uint8_t *rx_buffer, uint8_t len);

// TODO: Add a function prototype for registering an SPI RX interrupt callback
// void spi_abstraction_register_rx_callback(spi_rx_callback_t callback);

#endif // ZEPHYR_SPI_ABSTRACTION_H__