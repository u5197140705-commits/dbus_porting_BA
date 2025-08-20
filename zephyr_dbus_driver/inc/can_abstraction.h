#ifndef ZEPHYR_CAN_ABSTRACTION_H__
#define ZEPHYR_CAN_ABSTRACTION_H__

#include <zephyr/kernel.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the CAN abstraction layer.
 *
 * This function initializes the underlying Zephyr CAN device.
 *
 * @return True if initialization is successful, false otherwise.
 */
bool can_abstraction_init(void);

/**
 * @brief Sends a CAN message.
 *
 * @param id The CAN message ID.
 * @param data Pointer to the data to send.
 * @param len Length of the data.
 *
 * @return True if the message is successfully queued for transmission, false otherwise.
 */
bool can_abstraction_send(uint32_t id, const uint8_t *data, uint8_t len);

/**
 * @brief Registers a callback for CAN message reception.
 *
 * @param callback The function to call when a CAN message is received.
 */
void can_abstraction_register_rx_callback(void (*callback)(uint32_t id, const uint8_t *data, uint8_t len));

#endif // ZEPHYR_CAN_ABSTRACTION_H__