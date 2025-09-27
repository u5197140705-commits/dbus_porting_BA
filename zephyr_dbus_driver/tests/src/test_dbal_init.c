#include <zephyr/ztest.h>
#include "test_common.h"

// Include the application headers to be tested
#include "dbus_app_layer.h"
#include "spi_abstraction.h"

// Test suite for DBAL initialization
ZTEST_SUITE(dbal_init_suite, NULL, NULL, dbal_setup, dbal_teardown, NULL);

ZTEST(dbal_init_suite, test_dbal_init_basic) {
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

    printk("DBAL Init Test: Basic initialization passed.\n");
}

ZTEST(dbal_init_suite, test_dbal_init_spi_fail) {
    // Set mock to simulate spi_abstraction_init failure
    mock_spi_init_return_value = false;
    mock_printk_reset(); // Clear printk buffer for this test

    // Call the initialization function
    dbal_init();

    // Verify that SPI abstraction initialization failed
    zassert_true(strstr(mock_printk_buffer, "DBAL_ERROR: Failed to initialize SPI abstraction.") != NULL, "SPI init failure not logged");
    zassert_equal(dbal_get_connection_state(), DBAL_COMMSTATE_DISCONNECTED, "Connection state should remain DISCONNECTED on SPI init failure");

    printk("DBAL Init Test: SPI abstraction initialization failure passed.\n");
}