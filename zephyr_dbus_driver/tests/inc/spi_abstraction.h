#ifndef ZEPHYR_SPI_ABSTRACTION_H__
#define ZEPHYR_SPI_ABSTRACTION_H__

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/kernel.h> // For struct k_msgq
#include "mock_types.h" // Include the standalone mock types
#include <zephyr/drivers/spi.h> // For spi_callback_t and spi_transceive_cb



// Mock variables for spi_abstraction (declared as extern here)
extern bool mock_spi_send_called;
extern uint8_t mock_spi_send_data[128];
extern uint8_t mock_spi_send_len;
extern spi_rx_callback_t mock_rx_callback;
extern struct k_msgq *mock_spi_rx_msg_queue;

// Mock function declarations for spi_abstraction
int spi_abstraction_init(void);
int spi_abstraction_send(const uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len);
// The following functions are not part of the main spi_abstraction.h and are likely test-specific.
// They will be commented out or removed if not used in tests.
// bool spi_abstraction_receive(uint8_t *buffer, uint8_t len);
// bool spi_abstraction_transceive(const uint8_t *tx_data, uint8_t *rx_buffer, uint8_t len);
void spi_abstraction_register_rx_callback(spi_rx_callback_t callback);
void spi_abstraction_set_rx_msg_queue(struct k_msgq *msg_q);

// Functions for testing SPI RX timeout
uint32_t spi_abstraction_get_rx_timeout_count(void);
void spi_abstraction_reset_rx_timeout_count(void);

#endif // ZEPHYR_SPI_ABSTRACTION_H__