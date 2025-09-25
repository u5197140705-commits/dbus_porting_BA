#ifndef ZEPHYR_MOCK_TYPES_H__
#define ZEPHYR_MOCK_TYPES_H__

#include <stdint.h>

// Define a type for the SPI RX callback function
typedef void (*spi_rx_callback_t)(const uint8_t *data, uint8_t len);

#endif // ZEPHYR_MOCK_TYPES_H__