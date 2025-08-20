#include "can_abstraction.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/sys/printk.h>

// Placeholder for the CAN device pointer
static const struct device *can_dev;

// Placeholder for RX callback
static void (*rx_callback)(uint32_t id, const uint8_t *data, uint8_t len) = NULL;

// Placeholder for CAN RX callback function (Zephyr specific)
void can_rx_callback_handler(const struct device *dev, struct can_frame *frame, void *user_data)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);

    if (rx_callback != NULL) {
        rx_callback(frame->id, frame->data, frame->dlc);
    }
}

// Initializes the CAN abstraction layer.
bool can_abstraction_init(void)
{
    can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_can_0)); // Assuming 'zephyr_can_0' is defined in DTS

    if (!device_is_ready(can_dev)) {
        printk("CAN: Device %s is not ready\n", can_dev->name);
        return false;
    }

    // Configure CAN controller (example: 125 kbit/s)
    struct can_timing timing = {
        .sjw = 1,
        .prop_seg = 2,
        .phase_seg1 = 3,
        .phase_seg2 = 4,
        .prescaler = 8,
    };

    if (can_set_timing(can_dev, &timing, NULL)) {
        printk("CAN: Failed to set timing\n");
        return false;
    }

    // Register RX filter (example: accept all messages)
    static const struct can_filter filter = {
        .id_type = CAN_ID_STANDARD,
        .rtr = CAN_RTR_DATA,
        .id = 0x0,
        .mask = 0x0,
    };
    can_set_filter(can_dev, can_rx_callback_handler, NULL, &filter);

    printk("CAN: Abstraction layer initialized.\n");
    return true;
}

// Sends a CAN message.
bool can_abstraction_send(uint32_t id, const uint8_t *data, uint8_t len)
{
    struct can_frame frame = {
        .id = id,
        .dlc = len,
        .rtr = CAN_RTR_DATA,
        .flags = 0,
    };
    memcpy(frame.data, data, len);

    if (can_send(can_dev, &frame, K_MSEC(100)) != 0) { // 100ms timeout
        printk("CAN: Failed to send message\n");
        return false;
    }
    return true;
}

// Registers a callback for CAN message reception.
void can_abstraction_register_rx_callback(void (*callback)(uint32_t id, const uint8_t *data, uint8_t len))
{
    rx_callback = callback;
}