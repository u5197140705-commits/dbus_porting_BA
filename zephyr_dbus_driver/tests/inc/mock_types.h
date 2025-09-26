#ifndef ZEPHYR_MOCK_TYPES_H__
#define ZEPHYR_MOCK_TYPES_H__

#include <stdint.h>
#include <zephyr/kernel.h> // For struct k_msgq

// Define a type for the SPI RX callback function
typedef void (*spi_rx_callback_t)(const uint8_t *data, uint8_t len);

// Mock variables for spi_abstraction (declared as extern here)
extern bool mock_spi_send_called;
extern uint8_t mock_spi_send_data[128];
extern uint8_t mock_spi_send_len;
extern spi_rx_callback_t mock_rx_callback;
extern struct k_msgq *mock_spi_rx_msg_queue;

#endif // ZEPHYR_MOCK_TYPES_H__