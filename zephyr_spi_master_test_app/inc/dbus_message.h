#ifndef DBUS_MESSAGE_H
#define DBUS_MESSAGE_H

#include <zephyr/types.h>
#include <stddef.h>

/* SPI Protocol Definitions */
#define SPI_SOF_BYTE            0xAA
#define SPI_LENGTH_OFFSET       1
#define SPI_CRC_OFFSET          2
#define SPI_HEADER_LEN          3   /* SOF + Length + CRC */
#define DBUS_MAX_PAYLOAD_LEN    252 /* Max payload length (255 - SPI_HEADER_LEN) */

/* DBus Message Types (from original driver) */
typedef enum {
    DBAL_TYPE_CMD = 0x01,
    DBAL_TYPE_CMD_ACK = 0x02,
    DBAL_TYPE_QUERY = 0x03,
    DBAL_TYPE_QUERY_ACK = 0x04,
    DBAL_TYPE_EVENT = 0x05,
    DBAL_TYPE_CONNECTION_REQ = 0x06,
    DBAL_TYPE_CONNECTION_ACK = 0x07,
    DBAL_TYPE_PING_REQ = 0x08,
    DBAL_TYPE_PING_ACK = 0x09,
    DBAL_TYPE_DISABLE_REQ = 0x0A,
    DBAL_TYPE_DISABLE_ACK = 0x0B,
    DBAL_TYPE_UNKNOWN = 0xFF
} dbal_message_type_t;

/* DBus Frame Offsets (within payload, after SPI header) */
#define DBAL_FRAME_MESSAGE_TYPE_OFFSET  0
#define DBAL_FRAME_SERVICE_ID_HI        1
#define DBAL_FRAME_SERVICE_ID_LO        2
#define DBAL_FRAME_COMMAND_ID_HI        3
#define DBAL_FRAME_COMMAND_ID_LO        4
#define DBAL_FRAME_DATA_OFFSET          5 /* Start of actual data within DBus payload */

/* Generic DBus Message Structure (for construction) */
typedef struct {
    dbal_message_type_t type;
    uint16_t service_id;
    uint16_t command_id;
    uint8_t data[DBUS_MAX_PAYLOAD_LEN - DBAL_FRAME_DATA_OFFSET]; /* Max data length */
    uint8_t data_len;
} dbus_message_t;

/* Function Prototypes */
uint8_t calculate_crc8(const uint8_t *data, size_t len);
bool dbus_message_create_command(dbus_message_t *msg, uint16_t service_id, uint16_t command_id, const uint8_t *data, uint8_t data_len);
bool dbus_message_create_query(dbus_message_t *msg, uint16_t service_id, uint16_t command_id, const uint8_t *data, uint8_t data_len);
bool dbus_message_create_event(dbus_message_t *msg, uint16_t service_id, uint16_t command_id, const uint8_t *data, uint8_t data_len);
bool dbus_message_to_spi_frame(const dbus_message_t *msg, uint8_t *spi_frame_buffer, size_t buffer_size, size_t *frame_len);

#endif /* DBUS_MESSAGE_H */