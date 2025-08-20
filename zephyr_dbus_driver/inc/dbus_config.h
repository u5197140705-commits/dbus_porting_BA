#ifndef ZEPHYR_DBUS_CONFIG_H__
#define ZEPHYR_DBUS_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>

// Define constants from original DBal_cfg.c or derived from it
#define DBAL_OWN_NODE_ADDRESS       0x10U // Example: Own node address
#define DBAL_APPLIANCE_LAYER        0x01U // Example: Appliance layer protocol type
#define DBAL_PROTOCOL_VERSION       0x01U // Example: DBAL protocol version
#define DBAL_PRELIMINARY_PARTNER_ADDR 0x00U // Example: Preliminary partner address

// Placeholder for DBAL_ObjectTable and DBAL_ObjectTableSize
// These would typically be defined by the application using the DBAL
// For now, we'll provide a dummy structure.
struct DBAL_ReceiveObject; // Forward declaration
struct DBAL_ObjectTableEntry {
    const struct DBAL_ReceiveObject *ReceiveObject;
    const uint16_t                  ServiceId;
    const uint8_t                   ReceiveObjectCount;
};

// Example dummy table and size
extern const struct DBAL_ObjectTableEntry DBAL_ObjectTable[];
extern const uint8_t DBAL_ObjectTableSize;

// Placeholder for cross-connection configurations if DBAL_CROSS_CONNECTION is enabled
#ifdef CONFIG_DBAL_CROSS_CONNECTION
#define DBAL_CROSS_CONNECT_COUNT    1U // Example: Number of cross connections
extern uint8_t DBALCR_ComPartners[DBAL_CROSS_CONNECT_COUNT];
enum DBALCR_ParticipantType; // Forward declaration
extern enum DBALCR_ParticipantType DBALCR_RolesTowardPartners[DBAL_CROSS_CONNECT_COUNT];
extern uint16_t DBALCR_PingTimeMs;
#endif // CONFIG_DBAL_CROSS_CONNECTION

#endif // ZEPHYR_DBUS_CONFIG_H__