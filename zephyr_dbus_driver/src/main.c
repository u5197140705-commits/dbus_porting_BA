#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "dbus_app_layer.h" // Include the ported DBus Application Layer
#include "can_abstraction.h" // Include the CAN abstraction layer

void main(void)
{
    printk("Hello from Zephyr DBus Driver project!\n");

    // Initialize the DBus Application Layer
    dbal_init();

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

    // Keep the main thread alive
    while (1) {
        k_sleep(K_SECONDS(10));
    }
}