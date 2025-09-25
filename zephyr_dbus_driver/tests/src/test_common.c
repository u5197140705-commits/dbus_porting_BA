#include "test_common.h"

// Mock variables for spi_abstraction (defined here)
bool mock_spi_send_called = false;
uint8_t mock_spi_send_data[128];
uint8_t mock_spi_send_len = 0;
spi_rx_callback_t mock_rx_callback = NULL;
struct k_msgq *mock_spi_rx_msg_queue = NULL;

// Mock function implementations for spi_abstraction
bool spi_abstraction_init(void) {
    printk("MOCK: spi_abstraction_init called.\n");
    return true;
}

bool spi_abstraction_send(const uint8_t *data, uint8_t len) {
    printk("MOCK: spi_abstraction_send called with len %u.\n", len);
    mock_spi_send_called = true;
    if (len > 0 && len <= 128) {
        memcpy(mock_spi_send_data, data, len);
        mock_spi_send_len = len;
    }
    return true;
}

bool spi_abstraction_receive(uint8_t *buffer, uint8_t len) {
    printk("MOCK: spi_abstraction_receive called with len %u.\n", len);
    // Simulate receiving some data if needed for specific tests
    return true;
}

bool spi_abstraction_transceive(const uint8_t *tx_data, uint8_t *rx_buffer, uint8_t len) {
    printk("MOCK: spi_abstraction_transceive called with len %u.\n", len);
    // Simulate transceive operation
    return true;
}

void spi_abstraction_register_rx_callback(spi_rx_callback_t callback) {
    printk("MOCK: spi_abstraction_register_rx_callback called.\n");
    mock_rx_callback = callback;
}

void spi_abstraction_set_rx_msg_queue(struct k_msgq *msg_q) {
    printk("MOCK: spi_abstraction_set_rx_msg_queue called.\n");
    mock_spi_rx_msg_queue = msg_q;
}

// Setup function for the test suite
void dbal_setup(void *fixture) {
    ARG_UNUSED(fixture);
    // Perform any setup needed before each test in the suite
    // For example, reset global state or initialize modules
    mock_spi_send_called = false;
    mock_spi_send_len = 0;
    memset(mock_spi_send_data, 0, sizeof(mock_spi_send_data));
    // dbal_init(); // Initialize DBAL for each test if needed
}

// Teardown function for the test suite
void dbal_teardown(void *fixture) {
    ARG_UNUSED(fixture);
    // Perform any cleanup needed after each test in the suite
}