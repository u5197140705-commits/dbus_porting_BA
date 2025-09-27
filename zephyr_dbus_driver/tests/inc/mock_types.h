#ifndef ZEPHYR_MOCK_TYPES_H__
#define ZEPHYR_MOCK_TYPES_H__

#include <stdint.h>
#include <zephyr/kernel.h> // For struct k_msgq
#include <zephyr/drivers/spi.h> // For spi_callback_t and other SPI types
#include <zephyr/devicetree.h> // For struct dt_node_label

// Define a type for the SPI RX callback function
typedef void (*spi_rx_callback_t)(const uint8_t *data, uint8_t len);

// Mock variables for spi_abstraction (declared as extern here)
extern bool mock_spi_send_called;
extern uint8_t mock_spi_send_data[128];
extern uint8_t mock_spi_send_len;
extern spi_rx_callback_t mock_rx_callback;
extern struct k_msgq *mock_spi_rx_msg_queue;
extern struct k_msgq dbal_spi_rx_msg_queue; // Declare the actual message queue as extern
extern bool mock_spi_init_return_value; // Mock return value for spi_abstraction_init

// Variables to inject specific RX data for testing
extern uint8_t mock_rx_data_to_inject[128];
extern uint8_t mock_rx_len_to_inject;
extern bool mock_rx_data_ready;

// Mock function declaration for __wrap_spi_abstraction_send
bool __wrap_spi_abstraction_send(const uint8_t *data, uint8_t len);

// Mock function declaration for __wrap_printk
int __wrap_printk(const char *fmt, ...);
extern char mock_printk_buffer[256];
extern size_t mock_printk_buffer_idx;
extern void mock_printk_reset(void);

// Mock variables for k_timer
extern bool mock_k_timer_start_called;
extern bool mock_k_timer_stop_called;
extern k_timeout_t mock_k_timer_remaining_get_return_value;

// Mock variables and functions for dbal_con_msg_timer_cb
extern bool mock_dbal_send_connection_message_called;
extern enum DBAL_ConnectionMessageType mock_dbal_send_connection_message_type;
extern bool mock_dbal_handle_con_msg_tx_fail_called;

void __wrap_dbal_send_connection_message(struct dbal_instance* inst, enum DBAL_ConnectionMessageType con_message_type);
void __wrap_dbal_handle_con_msg_tx_fail(struct dbal_instance* inst);

// Mock variables and functions for dbal_msg_timer_cb and dbal_msg_timer_action
extern bool mock_dbal_check_task_code_section_flag_return_value;
extern bool mock_dbal_set_code_section_flag_called;
extern uint8_t mock_dbal_set_code_section_flag_mask;
extern bool mock_dbal_is_msg_of_dbal_type_to_repeat_return_value;
extern bool mock_dbal_io_dbus_handler_send_called;
extern bool mock_dbal_io_dbus_handler_send_return_value;
extern bool mock_dbal_clear_msgs_to_repeat_called;
extern bool mock_dbal_clear_retry_counters_called;
extern bool mock_dbal_connection_sm_called;
extern enum DBAL_ConnectionSmEvent mock_dbal_connection_sm_event;
extern bool mock_dbal_organize_msgs_to_repeat_called;

bool __wrap_dbal_check_task_code_section_flag(const struct dbal_instance* const inst);
void __wrap_dbal_set_code_section_flag(struct dbal_instance* inst, uint8_t mask);
bool __wrap_dbal_is_msg_of_dbal_type_to_repeat(enum DBAL_MessageType dbal_type);
bool __wrap_dbal_io_dbus_handler_send(struct dbal_instance* inst, enum DBAL_MessageType dbal_type, uint16_t service_id, uint16_t command_id, const uint8_t* bytes, uint8_t data_len, uint8_t repetition);
void __wrap_dbal_clear_msgs_to_repeat(struct dbal_instance* inst);
void __wrap_dbal_clear_retry_counters(struct dbal_instance* inst);
void __wrap_dbal_connection_sm(struct dbal_instance* inst, enum DBAL_ConnectionSmEvent event);
void __wrap_dbal_organize_msgs_to_repeat(struct dbal_instance* inst);

// Mock variables and functions for spi_abstraction_init failure cases
extern const struct device *mock_device_dt_get_return_value;
extern bool mock_device_is_ready_return_value;
extern int mock_spi_transceive_cb_return_value;

const struct device *__wrap_device_dt_get(const void *label); // Changed to void* to avoid dt_node_label definition issues
bool __wrap_device_is_ready(const struct device *dev);
int __wrap_spi_transceive_cb(const struct device *dev, const struct spi_config *config,
                             const struct spi_buf_set *tx_bufs, const struct spi_buf_set *rx_bufs,
                             spi_callback_t callback, void *user_data);

// Mock variables and functions for spi_transceive_callback
extern int mock_k_msgq_put_return_value;
extern int mock_spi_transceive_cb_rearm_return_value;

int __wrap_k_msgq_put(struct k_msgq *msgq, const void *data, k_timeout_t timeout);

// Mock variables and functions for spi_abstraction_receive and spi_abstraction_transceive (blocking)
extern int mock_spi_read_return_value;
extern int mock_spi_transceive_blocking_return_value;

int __wrap_spi_read(const struct device *dev, const struct spi_config *config,
                    const struct spi_buf_set *rx_bufs);
int __wrap_spi_transceive(const struct device *dev, const struct spi_config *config,
                          const struct spi_buf_set *tx_bufs, const struct spi_buf_set *rx_bufs);

#endif // ZEPHYR_MOCK_TYPES_H__