#include "test_common.h"
#include <stdarg.h> // For va_list and va_start/end
#include <stdio.h>  // For vsnprintf

// Mock variables for spi_abstraction (defined here)
bool mock_spi_send_called = false;
uint8_t mock_spi_send_data[128];
uint8_t mock_spi_send_len = 0;
spi_rx_callback_t mock_rx_callback = NULL;
struct k_msgq *mock_spi_rx_msg_queue = NULL;
bool mock_spi_init_return_value = true; // Default to true for successful init

// Variables to inject specific RX data for testing
uint8_t mock_rx_data_to_inject[128];
uint8_t mock_rx_len_to_inject = 0;
bool mock_rx_data_ready = false;

// Mock variables for k_timer
bool mock_k_timer_start_called = false;
bool mock_k_timer_stop_called = false;
k_timeout_t mock_k_timer_remaining_get_return_value = K_NO_WAIT;

// Mock variables for dbal_con_msg_timer_cb
bool mock_dbal_send_connection_message_called = false;
enum DBAL_ConnectionMessageType mock_dbal_send_connection_message_type = DBAL_CON_MSG_TYPE_COUNT; // Initialize to an invalid type
bool mock_dbal_handle_con_msg_tx_fail_called = false;

// Mock variables for dbal_msg_timer_cb and dbal_msg_timer_action
bool mock_dbal_check_task_code_section_flag_return_value = false;
bool mock_dbal_set_code_section_flag_called = false;
uint8_t mock_dbal_set_code_section_flag_mask = 0;
bool mock_dbal_is_msg_of_dbal_type_to_repeat_return_value = false;
bool mock_dbal_io_dbus_handler_send_called = false;
bool mock_dbal_io_dbus_handler_send_return_value = true; // Default to success
bool mock_dbal_clear_msgs_to_repeat_called = false;
bool mock_dbal_clear_retry_counters_called = false;
bool mock_dbal_connection_sm_called = false;
enum DBAL_ConnectionSmEvent mock_dbal_connection_sm_event = DBAL_CON_SM_EVENT_ENABLE_REQUEST; // Default to a valid event
bool mock_dbal_organize_msgs_to_repeat_called = false;

// Mock variables for spi_abstraction_init failure cases
const struct device *mock_device_dt_get_return_value = (const struct device *)1; // Default to a non-NULL value
bool mock_device_is_ready_return_value = true;
int mock_spi_transceive_cb_return_value = 0;

// Mock variables for spi_transceive_callback
int mock_k_msgq_put_return_value = 0; // Default to success
int mock_spi_transceive_cb_rearm_return_value = 0; // Default to success

// Mock variables for spi_abstraction_receive and spi_abstraction_transceive (blocking)
int mock_spi_read_return_value = 0; // Default to success
int mock_spi_transceive_blocking_return_value = 0; // Default to success

// Mock variables for spi_abstraction_receive and spi_abstraction_transceive (blocking)
int mock_spi_read_return_value = 0; // Default to success
int mock_spi_transceive_blocking_return_value = 0; // Default to success

// Mock variables for printk capture
char mock_printk_buffer[256];
size_t mock_printk_buffer_idx = 0;

void mock_printk_reset(void) {
    memset(mock_printk_buffer, 0, sizeof(mock_printk_buffer));
    mock_printk_buffer_idx = 0;
}

// Mock function for printk to capture output
int __wrap_printk(const char *fmt, ...) {
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = vsnprintf(mock_printk_buffer + mock_printk_buffer_idx,
                    sizeof(mock_printk_buffer) - mock_printk_buffer_idx,
                    fmt, args);
    va_end(args);

    if (ret > 0) {
        mock_printk_buffer_idx += ret;
        if (mock_printk_buffer_idx >= sizeof(mock_printk_buffer)) {
            mock_printk_buffer_idx = sizeof(mock_printk_buffer) - 1; // Prevent overflow
        }
    }
    return ret;
}

// Mock function implementations for dbal_con_msg_timer_cb
void __wrap_dbal_send_connection_message(struct dbal_instance* const inst, enum DBAL_ConnectionMessageType con_message_type) {
    ARG_UNUSED(inst);
    mock_dbal_send_connection_message_called = true;
    mock_dbal_send_connection_message_type = con_message_type;
    printk("MOCK: __wrap_dbal_send_connection_message called with type %u.\n", con_message_type);
}

void __wrap_dbal_handle_con_msg_tx_fail(struct dbal_instance* const inst) {
    ARG_UNUSED(inst);
    mock_dbal_handle_con_msg_tx_fail_called = true;
    printk("MOCK: __wrap_dbal_handle_con_msg_tx_fail called.\n");
}

// Mock function implementations for dbal_msg_timer_cb and dbal_msg_timer_action
bool __wrap_dbal_check_task_code_section_flag(const struct dbal_instance* const inst) {
    ARG_UNUSED(inst);
    printk("MOCK: __wrap_dbal_check_task_code_section_flag called. Returning %d.\n", mock_dbal_check_task_code_section_flag_return_value);
    return mock_dbal_check_task_code_section_flag_return_value;
}

void __wrap_dbal_set_code_section_flag(struct dbal_instance* const inst, uint8_t mask) {
    ARG_UNUSED(inst);
    mock_dbal_set_code_section_flag_called = true;
    mock_dbal_set_code_section_flag_mask = mask;
    printk("MOCK: __wrap_dbal_set_code_section_flag called with mask 0x%02x.\n", mask);
}

bool __wrap_dbal_is_msg_of_dbal_type_to_repeat(enum DBAL_MessageType dbal_type) {
    ARG_UNUSED(dbal_type);
    printk("MOCK: __wrap_dbal_is_msg_of_dbal_type_to_repeat called. Returning %d.\n", mock_dbal_is_msg_of_dbal_type_to_repeat_return_value);
    return mock_dbal_is_msg_of_dbal_type_to_repeat_return_value;
}

bool __wrap_dbal_io_dbus_handler_send(struct dbal_instance* const inst, enum DBAL_MessageType dbal_type, uint16_t service_id, uint16_t command_id, const uint8_t* const bytes, uint8_t data_len, uint8_t repetition) {
    ARG_UNUSED(inst);
    ARG_UNUSED(dbal_type);
    ARG_UNUSED(service_id);
    ARG_UNUSED(command_id);
    ARG_UNUSED(bytes);
    ARG_UNUSED(data_len);
    ARG_UNUSED(repetition);
    mock_dbal_io_dbus_handler_send_called = true;
    printk("MOCK: __wrap_dbal_io_dbus_handler_send called. Returning %d.\n", mock_dbal_io_dbus_handler_send_return_value);
    return mock_dbal_io_dbus_handler_send_return_value;
}

void __wrap_dbal_clear_msgs_to_repeat(struct dbal_instance* const inst) {
    ARG_UNUSED(inst);
    mock_dbal_clear_msgs_to_repeat_called = true;
    printk("MOCK: __wrap_dbal_clear_msgs_to_repeat called.\n");
}

void __wrap_dbal_clear_retry_counters(struct dbal_instance* const inst) {
    ARG_UNUSED(inst);
    mock_dbal_clear_retry_counters_called = true;
    printk("MOCK: __wrap_dbal_clear_retry_counters called.\n");
}

void __wrap_dbal_connection_sm(struct dbal_instance* const inst, enum DBAL_ConnectionSmEvent event) {
    ARG_UNUSED(inst);
    mock_dbal_connection_sm_called = true;
    mock_dbal_connection_sm_event = event;
    printk("MOCK: __wrap_dbal_connection_sm called with event %u.\n", event);
}

void __wrap_dbal_organize_msgs_to_repeat(struct dbal_instance* const inst) {
    ARG_UNUSED(inst);
    mock_dbal_organize_msgs_to_repeat_called = true;
    printk("MOCK: __wrap_dbal_organize_msgs_to_repeat called.\n");
}

// Mock function implementations for spi_abstraction_init failure cases
const struct device *__wrap_device_dt_get(const struct dt_node_label *label) {
    ARG_UNUSED(label);
    printk("MOCK: __wrap_device_dt_get called. Returning %p.\n", mock_device_dt_get_return_value);
    return mock_device_dt_get_return_value;
}

bool __wrap_device_is_ready(const struct device *dev) {
    ARG_UNUSED(dev);
    printk("MOCK: __wrap_device_is_ready called. Returning %d.\n", mock_device_is_ready_return_value);
    return mock_device_is_ready_return_value;
}

int __wrap_spi_transceive_cb(const struct device *dev, const struct spi_config *config,
                             const struct spi_buf_set *tx_bufs, const struct spi_buf_set *rx_bufs,
                             spi_callback_t callback, void *user_data) {
    ARG_UNUSED(dev);
    ARG_UNUSED(config);
    ARG_UNUSED(tx_bufs);
    ARG_UNUSED(rx_bufs);
    // For re-arming, we need to call the callback directly if it's for RX.
    // In spi_abstraction.c, spi_transceive_cb is called with NULL for tx_bufs for RX re-arming.
    if (tx_bufs == NULL && callback != NULL) {
        // This is a re-arm call for RX. We need to simulate the callback being called.
        // The result of the re-arm itself is controlled by mock_spi_transceive_cb_rearm_return_value.
        printk("MOCK: __wrap_spi_transceive_cb (re-arm) called. Returning %d.\n", mock_spi_transceive_cb_rearm_return_value);
        // We don't call the callback here, as the actual spi_transceive_callback will be called by the test.
        return mock_spi_transceive_cb_rearm_return_value;
    }
    printk("MOCK: __wrap_spi_transceive_cb (initial) called. Returning %d.\n", mock_spi_transceive_cb_return_value);
    return mock_spi_transceive_cb_return_value;
}

// Mock function implementations for spi_transceive_callback
int __wrap_k_msgq_put(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
    ARG_UNUSED(msgq);
    ARG_UNUSED(data);
    ARG_UNUSED(timeout);
    printk("MOCK: __wrap_k_msgq_put called. Returning %d.\n", mock_k_msgq_put_return_value);
    return mock_k_msgq_put_return_value;
}

// Mock function implementations for k_timer
void __wrap_k_timer_start(struct k_timer *timer, k_timeout_t duration, k_timeout_t period) {
    ARG_UNUSED(timer);
    ARG_UNUSED(duration);
    ARG_UNUSED(period);
    mock_k_timer_start_called = true;
    printk("MOCK: k_timer_start called.\n");
}

void __wrap_k_timer_stop(struct k_timer *timer) {
    ARG_UNUSED(timer);
    mock_k_timer_stop_called = true;
    printk("MOCK: k_timer_stop called.\n");
}

k_timeout_t __wrap_k_timer_remaining_get(struct k_timer *timer) {
    ARG_UNUSED(timer);
    printk("MOCK: k_timer_remaining_get called. Returning %lld.\n", mock_k_timer_remaining_get_return_value.ticks);
    return mock_k_timer_remaining_get_return_value;
}

// Mock function implementations for spi_abstraction
bool spi_abstraction_init(void) {
    printk("MOCK: spi_abstraction_init called. Returning %d.\n", mock_spi_init_return_value);
    return mock_spi_init_return_value;
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
    mock_spi_send_called = true;
    if (len > 0 && len <= sizeof(mock_spi_send_data)) {
        memcpy(mock_spi_send_data, tx_data, len);
        mock_spi_send_len = len;
    }

    if (rx_buffer != NULL && len > 0) {
        if (mock_rx_data_ready) {
            // Use injected RX data if available
            uint8_t copy_len = (len < mock_rx_len_to_inject) ? len : mock_rx_len_to_inject;
            memcpy(rx_buffer, mock_rx_data_to_inject, copy_len);
            mock_rx_data_ready = false; // Reset for next test
            printk("MOCK: Injected RX data used (len: %u).\n", copy_len);
        } else {
            // Simulate a basic command-response if no injected data
            uint8_t dummy_response[] = {
                SPI_SOF_BYTE, // SOF
                0x07, 0x00,   // Length (7 bytes: Service ID, Command ID, Payload, CRC)
                0x01, 0x10,   // Service ID (0x1001)
                0x01, 0x00,   // Command ID (0x0001)
                0x00,         // Payload (Status OK)
                0x00, 0x00    // Dummy CRC
            };
            uint8_t response_len = sizeof(dummy_response);

            if (len >= response_len) {
                memcpy(rx_buffer, dummy_response, response_len);
            } else {
                memcpy(rx_buffer, dummy_response, len);
            }
            printk("MOCK: Dummy RX data used (len: %u).\n", response_len);
        }
    }
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
    mock_rx_len_to_inject = 0;
    mock_rx_data_ready = false;
    memset(mock_rx_data_to_inject, 0, sizeof(mock_rx_data_to_inject));
    mock_printk_reset(); // Reset printk buffer for each test
    dbal_reset_service_handlers_for_test(); // Reset service handlers for each test
    mock_spi_init_return_value = true; // Reset mock SPI init return value to success
    mock_k_timer_start_called = false;
    mock_k_timer_stop_called = false;
    mock_k_timer_remaining_get_return_value = K_NO_WAIT;
    mock_dbal_send_connection_message_called = false;
    mock_dbal_send_connection_message_type = DBAL_CON_MSG_TYPE_COUNT;
    mock_dbal_handle_con_msg_tx_fail_called = false;
    mock_dbal_check_task_code_section_flag_return_value = false;
    mock_dbal_set_code_section_flag_called = false;
    mock_dbal_set_code_section_flag_mask = 0;
    mock_dbal_is_msg_of_dbal_type_to_repeat_return_value = false;
    mock_dbal_io_dbus_handler_send_called = false;
    mock_dbal_io_dbus_handler_send_return_value = true;
    mock_dbal_clear_msgs_to_repeat_called = false;
    mock_dbal_clear_retry_counters_called = false;
    mock_dbal_connection_sm_called = false;
    mock_dbal_connection_sm_event = DBAL_CON_SM_EVENT_ENABLE_REQUEST;
    mock_dbal_organize_msgs_to_repeat_called = false;
    mock_device_dt_get_return_value = (const struct device *)1;
    mock_device_is_ready_return_value = true;
    mock_spi_transceive_cb_return_value = 0;
    mock_k_msgq_put_return_value = 0;
    mock_spi_transceive_cb_rearm_return_value = 0;
    mock_spi_read_return_value = 0;
    mock_spi_transceive_blocking_return_value = 0;
    // dbal_init(); // Initialize DBAL for each test if needed
}

// Teardown function for the test suite
void dbal_teardown(void *fixture) {
    ARG_UNUSED(fixture);
    // Perform any cleanup needed after each test in the suite
}