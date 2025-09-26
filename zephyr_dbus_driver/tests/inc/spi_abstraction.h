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
bool spi_abstraction_init(void);
bool spi_abstraction_send(const uint8_t *data, uint8_t len);
bool spi_abstraction_receive(uint8_t *buffer, uint8_t len);
bool spi_abstraction_transceive(const uint8_t *tx_data, uint8_t *rx_buffer, uint8_t len);
void spi_abstraction_register_rx_callback(spi_rx_callback_t callback);
void spi_abstraction_set_rx_msg_queue(struct k_msgq *msg_q);

#endif // ZEPHYR_SPI_ABSTRACTION_H__