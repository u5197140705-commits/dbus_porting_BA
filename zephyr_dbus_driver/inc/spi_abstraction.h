#ifndef ZEPHYR_SPI_ABSTRACTION_H__
#define ZEPHYR_SPI_ABSTRACTION_H__

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/kernel.h> // For struct k_msgq
#include <zephyr/drivers/spi.h> // For spi_callback_t and spi_transceive_cb

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

// Define a type for the SPI RX callback function
typedef void (*spi_rx_callback_t)(const uint8_t *data, uint8_t len);

/**
 * @brief Registers a callback function for SPI receive interrupts.
 *
 * @param callback The function to be called when SPI data is received.
 */
void spi_abstraction_register_rx_callback(spi_rx_callback_t callback);

/**
 * @brief Sets the message queue for SPI received data.
 *
 * This function provides the SPI abstraction layer with a message queue
 * to put received SPI frames into.
 *
 * @param msg_q Pointer to the Zephyr message queue.
 */
void spi_abstraction_set_rx_msg_queue(struct k_msgq *msg_q);

/**
 * @brief Test helper to inject simulated SPI RX data.
 *
 * This function is intended for testing purposes to simulate incoming SPI data
 * without actual hardware interaction. It directly puts the data into the
 * internal message queue.
 *
 * @param data Pointer to the simulated data to inject.
 * @param len Length of the simulated data.
 */
void spi_abstraction_test_inject_rx_data(const uint8_t *data, uint8_t len);

#endif // ZEPHYR_SPI_ABSTRACTION_H__

// Test-only wrapper to expose the static spi_transceive_callback
void spi_abstraction_test_transceive_callback(const struct device *dev, int result, void *data);

// Test-only getter for spi_rx_timeout_count
uint32_t spi_abstraction_get_rx_timeout_count_for_test(void);

// Test-only function to reset spi_rx_timeout_count
void spi_abstraction_reset_rx_timeout_count_for_test(void);