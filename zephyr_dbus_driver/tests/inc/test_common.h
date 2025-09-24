#ifndef ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_
#define ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_

#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <string.h>
#include "dbus_app_layer.h"
#include "spi_abstraction.h"

// Mock variables for spi_abstraction
extern bool mock_spi_send_called;
extern uint8_t mock_spi_send_data[128];
extern uint8_t mock_spi_send_len;
extern spi_rx_callback_t mock_rx_callback;
extern struct k_msgq *mock_spi_rx_msg_queue;

// Mock function declarations for spi_abstraction
bool spi_abstraction_send(const uint8_t *data, uint8_t len);
void spi_abstraction_register_rx_callback(spi_rx_callback_t callback);
void spi_abstraction_set_rx_msg_queue(struct k_msgq *msg_q);
bool spi_abstraction_init(void);

// Test suite setup and teardown declarations
void dbal_setup(void *fixture);
void dbal_teardown(void *fixture);

#endif // ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_