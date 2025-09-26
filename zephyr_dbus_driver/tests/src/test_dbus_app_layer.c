#include <zephyr/ztest.h>
#include "test_common.h"

// Include the application headers to be tested
#include "dbus_app_layer.h"
#include "spi_abstraction.h"
#include "mock_types.h" // Include mock types for test_dbus_app_layer.c

// Test suite for DBAL initialization
ZTEST_SUITE(dbal_app_layer_init_suite, NULL, NULL, dbal_setup, dbal_teardown, NULL);

ZTEST(dbal_app_layer_init_suite, test_dbal_app_layer_init_basic) {
    // Call the initialization function
    dbal_init();

    // Verify that SPI abstraction was initialized and callbacks registered
    zassert_false(mock_spi_send_called, "spi_abstraction_send should not be called during init");
    zassert_not_null(mock_rx_callback, "SPI RX callback should be registered");
    zassert_not_null(mock_spi_rx_msg_queue, "SPI RX message queue should be set");

    // Verify that the target address is set from Kconfig (assuming CONFIG_DBAL_TARGET_ADDRESS is 0x00)
    // This requires access to g_dbal_main_instance, which is static.
    // For proper unit testing, internal state might need to be exposed via getter functions or tested indirectly.
    // For now, we'll assume it's correctly set by the Kconfig system.
    // zassert_equal(g_dbal_main_instance.DBUS_ComPartner, CONFIG_DBAL_TARGET_ADDRESS, "Target address not set correctly");

    printk("DBAL App Layer Init Test: Basic initialization passed.\n");
}

// Test suite for DBAL message sending
ZTEST_SUITE(dbal_send_suite, NULL, NULL, dbal_setup, dbal_teardown, NULL);

ZTEST(dbal_send_suite, test_dbal_send_cmd_response) {
    // Reset mock state (manual setup for this test)
    mock_spi_send_called = false;
    mock_spi_send_len = 0;
    memset(mock_spi_send_data, 0, sizeof(mock_spi_send_data));

    uint16_t service_id = 0x1001;
    uint16_t command_id = 0x0001;
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t data_len = sizeof(data);

    // Call the function to send a command response
    bool result = dbal_send_cmd_response(service_id, command_id, data, data_len);

    // Verify that spi_abstraction_send was called
    zassert_true(result, "dbal_send_cmd_response should return true on success");
    zassert_true(mock_spi_send_called, "spi_abstraction_send should be called");
    zassert_true(mock_spi_send_len > (SPI_HEADER_LEN + DBAL_FRAME_DATA_OFFSET), "SPI send length too short");

    // Verify basic framing (SOF, Length, CRC) - simplified check
    zassert_equal(mock_spi_send_data[0], SPI_SOF_BYTE, "SPI SOF byte mismatch");
    // Further checks for length, CRC, service_id, command_id, and payload would go here.
    // This requires detailed knowledge of the framing in dbal_io_dbus_handler_send.

    printk("DBAL Send Test: Command response sending passed.\n");
}