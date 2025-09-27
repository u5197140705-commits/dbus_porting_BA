#include <zephyr/ztest.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include "spi_abstraction.h" // Assuming this provides the interface to the SPI driver

#define SPI_DEVICE_NODE DT_NODELABEL(flexcomm1) // Use DT_NODELABEL for SPI device

static const struct device *spi_dev = DEVICE_DT_GET(SPI_DEVICE_NODE);

ZTEST_SUITE(dbus_spi_integration_suite, NULL, NULL, NULL, NULL, NULL);

ZTEST_TEST(test_spi_device_presence)
{
    // Verify that the SPI abstraction layer can be initialized
    zassert_true(spi_abstraction_init(), "SPI abstraction layer initialization failed");

    // Verify that the SPI device is ready
    zassert_true(device_is_ready(spi_dev), "SPI device is not ready after abstraction init");

    // Simulate a simple master-initiated transceive to check slave responsiveness
    uint8_t master_tx_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t master_rx_buf[sizeof(master_tx_buf)] = {0};

    struct spi_buf tx_buf = {
        .buf = master_tx_buf,
        .len = sizeof(master_tx_buf)
    };
    struct spi_buf rx_buf = {
        .buf = master_rx_buf,
        .len = sizeof(master_rx_buf)
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1
    };

    // This spi_config is for the test acting as a master
    struct spi_config master_spi_cfg = {
        .frequency = 1000000, // 1 MHz
        .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA,
        .slave = 0, // Assuming slave select 0
        .cs = NULL, // No software chip select
    };

    int ret = spi_transceive(spi_dev, &master_spi_cfg, &tx_bufs, &rx_bufs);
    zassert_equal(ret, 0, "Master-initiated SPI transceive failed with error: %d", ret);

    // In a slave-mode scenario, the slave might echo data or send a specific response.
    // For now, we just assert that the transceive itself was successful.
    // Further assertions would depend on the expected slave behavior.
    // For example, if the slave echoes, zassert_mem_equal(master_rx_buf, master_tx_buf, sizeof(master_tx_buf));
}

#include <zephyr/init.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(dbus_spi_integration_test, LOG_LEVEL_DBG);

// D-Bus service and object path
#define DBUS_BUS_NAME "com.zephyr.DbusSpiService"
#define DBUS_OBJECT_PATH "/com/zephyr/DbusSpi"
#define DBUS_INTERFACE_NAME "com.zephyr.DbusSpi"

// Forward declaration for the D-Bus method handler
static int set_spi_register_handler(struct dbus_conn *conn, struct dbus_message *msg);

// D-Bus method table
static const struct dbus_method dbus_methods[] = {
    DBUS_METHOD("SetSpiRegister", "yy", "", set_spi_register_handler),
    DBUS_METHOD_END
};

// D-Bus interface description
static const struct dbus_interface dbus_iface = {
    .name = DBUS_INTERFACE_NAME,
    .methods = dbus_methods,
};

// D-Bus object description
static const struct dbus_vtable dbus_vtable = {
    .interface = &dbus_iface,
};

// D-Bus object
static const struct dbus_object dbus_obj = {
    .path = DBUS_OBJECT_PATH,
    .vtable = &dbus_vtable_with_signals, // Use the vtable with signals
};

// D-Bus connection
static struct dbus_conn *dbus_conn;

// Global buffer to capture data sent by spi_abstraction_send
#define MAX_SPI_SEND_DATA_LEN 16
static uint8_t spi_sent_data_buffer[MAX_SPI_SEND_DATA_LEN];
static uint8_t spi_sent_data_len = 0;
static K_SEM_DEFINE(spi_send_sem, 0, 1); // Semaphore to signal data sent

// D-Bus method handler implementation
static int set_spi_register_handler(struct dbus_conn *conn, struct dbus_message *msg)
{
    uint8_t reg_addr;
    uint8_t value;
    int ret;

    if (!dbus_message_get_args(msg, NULL, DBUS_TYPE_BYTE, &reg_addr,
                               DBUS_TYPE_BYTE, &value, DBUS_TYPE_INVALID)) {
        LOG_ERR("Failed to parse D-Bus message arguments");
        return dbus_message_reply_error_invalid_args(msg);
    }

    LOG_INF("D-Bus: SetSpiRegister called with addr=0x%02x, value=0x%02x", reg_addr, value);

    // Call the actual SPI abstraction send function
    uint8_t data_to_send[] = {reg_addr, value};
    ret = spi_abstraction_send(data_to_send, sizeof(data_to_send));

    if (ret == false) { // spi_abstraction_send returns bool
        LOG_ERR("SPI send failed");
        return dbus_message_reply_error_internal(msg, "SPI send failed");
    }

    return dbus_message_reply_method_return(msg);
}

// Initialize D-Bus service
static int dbus_spi_service_init(void)
{
    int ret;

    dbus_conn = dbus_conn_get(DBUS_BUS_SYSTEM);
    if (!dbus_conn) {
        LOG_ERR("Failed to get D-Bus system bus connection");
        return -ENODEV;
    }

    ret = dbus_conn_register_object(dbus_conn, &dbus_obj); // Register with signals
    if (ret) {
        LOG_ERR("Failed to register D-Bus object: %d", ret);
        return ret;
    }

    ret = dbus_conn_request_name(dbus_conn, DBUS_BUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING);
    if (ret) {
        LOG_ERR("Failed to request D-Bus name: %d", ret);
        return ret;
    }

    LOG_INF("D-Bus SPI service initialized");
    return 0;
}

SYS_INIT(dbus_spi_service_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);


ZTEST_TEST(test_dbus_message_triggering_spi_write)
{
    int ret;
    struct dbus_message *msg;
    struct dbus_message *reply;
    uint8_t test_reg_addr = 0x10;
    uint8_t test_value = 0xCD;
    uint8_t expected_sent_data[] = {test_reg_addr, test_value};

    // Reset capture buffer and semaphore
    memset(spi_sent_data_buffer, 0, sizeof(spi_sent_data_buffer));
    spi_sent_data_len = 0;
    k_sem_reset(&spi_send_sem);

    // Create a D-Bus method call message
    msg = dbus_message_new_method_call(DBUS_BUS_NAME, DBUS_OBJECT_PATH,
                                       DBUS_INTERFACE_NAME, "SetSpiRegister");
    zassert_not_null(msg, "Failed to create D-Bus message");

    // Append arguments to the message
    dbus_message_append_args(msg, DBUS_TYPE_BYTE, &test_reg_addr,
                             DBUS_TYPE_BYTE, &test_value, DBUS_TYPE_INVALID);

    // Send the D-Bus message and get a reply
    ret = dbus_conn_send_with_reply_and_block(dbus_conn, msg, &reply, DBUS_TIMEOUT_USE_DEFAULT);
    zassert_equal(ret, 0, "D-Bus method call failed: %d", ret);
    zassert_not_null(reply, "D-Bus method call returned no reply");
    zassert_equal(dbus_message_get_type(reply), DBUS_MESSAGE_TYPE_METHOD_RETURN,
                  "D-Bus reply was not a method return, type: %d", dbus_message_get_type(reply));

    dbus_message_unref(msg);
    dbus_message_unref(reply);

    // Now, act as an SPI master to receive the data sent by the slave (our driver)
    uint8_t master_rx_buf[sizeof(expected_sent_data)] = {0};
    uint8_t master_tx_dummy_buf[sizeof(expected_sent_data)] = {0}; // Master sends dummy data to clock out slave's data

    struct spi_buf tx_buf = {
        .buf = master_tx_dummy_buf,
        .len = sizeof(master_tx_dummy_buf)
    };
    struct spi_buf rx_buf = {
        .buf = master_rx_buf,
        .len = sizeof(master_rx_buf)
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1
    };

    struct spi_config master_spi_cfg = {
        .frequency = 1000000, // 1 MHz
        .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA,
        .slave = 0, // Assuming slave select 0
        .cs = NULL, // No software chip select
    };

    // Give the slave a moment to prepare its send operation
    k_sleep(K_MSEC(10));

    ret = spi_transceive(spi_dev, &master_spi_cfg, &tx_bufs, &rx_bufs);
    zassert_equal(ret, 0, "Master-initiated SPI transceive failed to receive slave data: %d", ret);

    // Assert that the received data matches the expected data
    zassert_mem_equal(master_rx_buf, expected_sent_data, sizeof(expected_sent_data),
                       "Received SPI data does not match expected data");
}

// D-Bus signal definition
#define DBUS_SIGNAL_NAME "SpiEventOccurred"

// Global flag to indicate if the signal was received
static volatile bool spi_event_signal_received = false;

// D-Bus signal handler
static int spi_event_signal_handler(struct dbus_conn *conn, struct dbus_message *msg)
{
    LOG_INF("D-Bus: SpiEventOccurred signal received");
    spi_event_signal_received = true;
    return 0;
}

// D-Bus signal table
static const struct dbus_signal dbus_signals[] = {
    DBUS_SIGNAL(DBUS_SIGNAL_NAME, "", ""),
    DBUS_SIGNAL_END
};

// Update D-Bus interface description to include signals
static const struct dbus_interface dbus_iface_with_signals = {
    .name = DBUS_INTERFACE_NAME,
    .methods = dbus_methods,
    .signals = dbus_signals,
};

// Update D-Bus object description to use the interface with signals
static const struct dbus_vtable dbus_vtable_with_signals = {
    .interface = &dbus_iface_with_signals,
};

// Update D-Bus object
static const struct dbus_object dbus_obj_with_signals = {
    .path = DBUS_OBJECT_PATH,
    .vtable = &dbus_vtable_with_signals,
};

// Function to simulate an SPI event (to be called by the test)
// In a real scenario, this would be triggered by an actual hardware interrupt
void spi_abstraction_simulate_event(void)
{
    // Use the test helper function to inject simulated RX data
    uint8_t simulated_rx_data[] = {0xAA, 0xBB, 0xCC};
    spi_abstraction_test_inject_rx_data(simulated_rx_data, sizeof(simulated_rx_data));
}


ZTEST_TEST(test_spi_event_triggering_dbus_signal)
{
    int ret;
    struct dbus_message *msg;

    spi_event_signal_received = false; // Reset flag for this test

    // Add a match rule to receive the signal
    ret = dbus_conn_add_match(dbus_conn,
                              "type='signal',interface='" DBUS_INTERFACE_NAME "',member='" DBUS_SIGNAL_NAME "'",
                              spi_event_signal_handler);
    zassert_equal(ret, 0, "Failed to add D-Bus match rule: %d", ret);

    // Simulate an SPI event
    spi_abstraction_simulate_event();

    // Give some time for the D-Bus signal to be processed
    k_sleep(K_MSEC(100));

    // Assert that the signal was received
    zassert_true(spi_event_signal_received, "D-Bus signal SpiEventOccurred was not received");

    // Remove the match rule
    ret = dbus_conn_remove_match(dbus_conn,
                                 "type='signal',interface='" DBUS_INTERFACE_NAME "',member='" DBUS_SIGNAL_NAME "'",
                                 spi_event_signal_handler);
    zassert_equal(ret, 0, "Failed to remove D-Bus match rule: %d", ret);
}