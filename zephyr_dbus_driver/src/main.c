#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/autoconf.h> // Explicitly include generated Kconfig definitions
#include "dbus_app_layer.h" // Include the ported DBus Application Layer
#include "spi_abstraction.h" // Include the SPI abstraction layer

// Example DBus service callback for testing
void my_test_service_handler(const uint8_t* const data, uint8_t data_len) {
    printk("Main: Received DBus message in test service handler! DataLen: %u, Data: ", data_len);
    for (uint8_t i = 0; i < data_len; i++) {
        printk("0x%02x ", data[i]);
    }
    printk("\n");
}

int main(void)
{
    printk("Hello from Zephyr DBus Driver project!\n");

    // Initialize the DBus Application Layer
    dbal_init();

    // Initialize the SPI abstraction layer
    int ret_spi_init = spi_abstraction_init();
    if (ret_spi_init != 0) {
        printk("Main: SPI abstraction initialization failed with error: %d!\n", ret_spi_init);
        return 0; // Or handle error appropriately
    }
    printk("Main: SPI abstraction initialized successfully.\n");

    // Register DBus service handlers here for testing purposes
    dbal_register_service_handler(0x1001, DBAL_TYPE_CMD, my_test_service_handler);

    // Simulate sending a command response after a delay
    k_sleep(K_SECONDS(2));
    printk("Main: Simulating sending a command response...\n");
    uint8_t test_data[] = {0x01, 0x02, 0x03, 0x04};
    dbal_send_cmd_response(0x1001, 0x0001, test_data, sizeof(test_data));

    // Simulate sending an event after another delay
    k_sleep(K_SECONDS(2));
    printk("Main: Simulating sending an event...\n");
    uint8_t event_data[] = {0x05, 0x06};
    dbal_send_event(0x2001, 0x0002, event_data, sizeof(event_data));

    // Periodically send a test message over SPI
    uint8_t periodic_test_data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint32_t message_count = 0;
    while (1) {
        printk("Main: Sending periodic SPI test message %u (Tx Len: %zu)...\n", message_count++, sizeof(periodic_test_data));
        uint8_t rx_buffer[sizeof(periodic_test_data)];
        int ret = spi_abstraction_send(periodic_test_data, sizeof(periodic_test_data), rx_buffer, sizeof(rx_buffer));
        if (ret == 0) {
            printk("Main: SPI transceive successful. Received data:\n");
            for (uint8_t i = 0; i < sizeof(rx_buffer); i++) {
                printk("0x%02x ", rx_buffer[i]);
            }
            printk("\n");
        } else {
            printk("Main: SPI transceive failed: %d\n", ret);
        }
        k_sleep(K_SECONDS(1)); // Send every 1 second
    }
    return 0;
}