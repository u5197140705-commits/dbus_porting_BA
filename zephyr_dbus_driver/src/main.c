#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/autoconf.h> // Explicitly include generated Kconfig definitions
#include "dbus_app_layer.h" // Include the ported DBus Application Layer
#include "spi_abstraction.h" // Include the SPI abstraction layer
#include "dbus_driver_public.h" // Include the DBus driver public API
#include <zephyr/sys_clock.h> // For K_SECONDS macro

// Example DBus service callback for testing
void my_test_service_handler(const uint8_t* const data, uint8_t data_len) {
    printk("Main: Received DBus message in test service handler! DataLen: %u, Data: ", data_len);
    for (uint8_t i = 0; i < data_len; i++) {
        printk("0x%02x ", data[i]);
    }
    printk("\n");
}

// Function to perform loopback test using DBCDRV functions
void test_dbus_driver_loopback(void)
{
    uint32_t write_val = 0x12345678;
    uint32_t read_val = 0;
    enum DBC_Error err;

    struct {
        bool cpol;
        bool cpha;
        const char *name;
    } modes[] = {
        { false, false, "MODE0 (CPOL=0, CPHA=0)" },
        { false, true,  "MODE1 (CPOL=0, CPHA=1)" },
        { true,  false, "MODE2 (CPOL=1, CPHA=0)" },
        { true,  true,  "MODE3 (CPOL=1, CPHA=1)" },
    };

    printk("DBCDRV Loopback Test [SWEEP_V2_2026_03_17]: Starting SPI mode sweep (4 modes).\n");

    for (size_t i = 0; i < ARRAY_SIZE(modes); i++) {
        DBCDRV_setSpiMode(modes[i].cpol, modes[i].cpha);
        k_msleep(2);

        printk("DBCDRV Loopback Test: [%s] Writing 0x%08x to DBC_SCRATCHPAD_ADDR (0x%02x)\n",
               modes[i].name, write_val, DBC_SCRATCHPAD_ADDR);

        err = DBCDRV_writeReg32(DBC_SCRATCHPAD_ADDR, write_val);
        if (err != DBC_OK) {
            printk("❌ DBCDRV Loopback Test: [%s] Write failed with error: %d\n", modes[i].name, err);
            continue;
        }

        k_msleep(5);

        printk("DBCDRV Loopback Test: [%s] Reading from DBC_SCRATCHPAD_ADDR (0x%02x)\n",
               modes[i].name, DBC_SCRATCHPAD_ADDR);

        err = DBCDRV_readReg32(DBC_SCRATCHPAD_ADDR, &read_val);
        if (err != DBC_OK) {
            printk("❌ DBCDRV Loopback Test: [%s] Read failed with error: %d\n", modes[i].name, err);
            continue;
        }

        printk("DBCDRV Loopback Test: [%s] TX (write) = 0x%08x, RX (read) = 0x%08x\n",
               modes[i].name, write_val, read_val);

        if (write_val == read_val) {
            printk("✅ DBCDRV Loopback Test: [%s] SUCCESS!\n", modes[i].name);
        } else if (read_val == 0x00000000 || read_val == 0xFFFFFFFF) {
            printk("⚠️ DBCDRV Loopback Test: [%s] Received all 0x%08x.\n", modes[i].name, read_val);
        } else {
            printk("❌ DBCDRV Loopback Test: [%s] MISMATCH (expected 0x%08x, got 0x%08x).\n",
                   modes[i].name, write_val, read_val);
        }
    }

    printk("DBCDRV Loopback Test complete.\n");
}



int main(void)
{
    printk("Hello from Zephyr DBus Driver project! [SWEEP_V2_2026_03_17]\n");

    // Perform loopback test using DBCDRV functions
    test_dbus_driver_loopback();

    printk("Main: Sweep-only test build complete. Halting further init.\n");
    return 0;
    
    // Initialize the DBus Driver
    enum DBC_Error dbus_driver_init_ret = DBCDRV_init();
    if (dbus_driver_init_ret != DBC_OK) {
        printk("Main: DBus Driver initialization failed with error: %d!\n", dbus_driver_init_ret);
        return 0; // Or handle error appropriately
    }
    printk("Main: DBus Driver initialized successfully.\n");



    // Initialize the DBus Application Layer
    dbal_init();

    // Initialize the SPI abstraction layer
    int ret_spi_init = spi_abstraction_init();
    if (ret_spi_init != 0) {
        printk("Main: SPI abstraction initialization failed with error: %d!\n", ret_spi_init);
        return 0; // Or handle error appropriately
    }
    printk("Main: SPI abstraction initialized successfully.\n");

    // Step 1: Confirm SPI is really active at runtime
    const struct device *spi_dev_check = DEVICE_DT_GET(DT_NODELABEL(flexcomm1));

    if (!device_is_ready(spi_dev_check)) {
        printk("❌ SPI device not ready!\n");
    } else {
        printk("✅ SPI device ready: %s\n", spi_dev_check->name);
    }

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
