#ifndef ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_
#define ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_

#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <string.h>
#include "dbus_app_layer.h"
#include "mock_types.h" // Include the standalone mock types
#include "spi_abstraction.h" // Include the mock spi_abstraction.h

// Placeholder definitions for DBAL constants used in tests
#define DBAL_OWN_NODE_ADDRESS 0x00 // Example value, adjust as per actual system config
#define SPI_SOF_BYTE          0xAA // Example value, adjust as per actual SPI protocol

// Common DBus frame constants for testing
#define DBAL_SERVICE_ID_TEST  0x1001
#define DBAL_COMMAND_ID_TEST  0x0001
#define DBAL_STATUS_OK        0x00
#define DBAL_FRAME_MIN_LEN    10 // SOF (1) + Length Field (2) + Service ID (2) + Command ID (2) + Payload (1) + CRC (2)
#define DBAL_FRAME_DATA_LEN   7  // Service ID (2) + Command ID (2) + Payload (1) + CRC (2)

// Test suite setup and teardown declarations
void dbal_setup(void *fixture);
void dbal_teardown(void *fixture);

#endif // ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_