#ifndef ZEPHYR_DBUS_APP_LAYER_H__
#define ZEPHYR_DBUS_APP_LAYER_H__

#include <zephyr/kernel.h>
#include <stdbool.h>
#include <stdint.h>

// Forward declaration for internal instance structure, if needed
// Define some constants from the original DBAL for Zephyr porting
#define DBAL_MAX_MSGS2REPEAT        10U // Example value, adjust as needed
#define DBAL_DBUS_RETRY_MAX         3U  // Example value, adjust as needed
#define DBAL_RESPONSE_TIME_MS       100 // Example value, adjust as needed
#define DBAL_DBUS_RECOVERY_TIME_MS  500 // Example value, adjust as needed

// Forward declaration for internal instance structure
struct dbal_instance;

// Prototype for timer callbacks (Zephyr k_timer_handler_t signature)
void dbal_con_msg_timer_cb(struct k_timer *timer_id);
void dbal_msg_timer_cb(struct k_timer *timer_id);

/** \enum   DBAL_CommState
 *
 * \brief   Value that represents the current communication status.
 *
 * \details For meaning of values see general DBal specification.
**/
enum DBAL_CommState
{
    DBAL_COMMSTATE_NOT_READY = 0,
    DBAL_COMMSTATE_READY
};

/** \enum   DBAL_MessageType
 *
 * \brief   Value that represents the DBal specific message type for communication.
 *
 * \details For meaning of values see general DBal specification.
**/
enum DBAL_MessageType
{
    DBAL_TYPE_CMD,
    DBAL_TYPE_CMD_ACK,
    DBAL_TYPE_QUERY,
    DBAL_TYPE_QUERY_ACK,
    DBAL_TYPE_EVENT,
    DBAL_TYPE_EVENT_ACK,
    DBAL_TYPE_UNKNOWN
};

/** \enum   DBAL_ConnectionMessageType
 *
 * \brief   Value that represents the action in the connection handling.
 *
 * \details For meaning of values see general DBal specification.
**/
enum DBAL_ConnectionMessageType
{
    DBAL_CON_ENABLE_REQUEST = 0,
    DBAL_CON_ENABLE_RESPONSE,
    DBAL_CON_DISABLE_REQUEST,
    DBAL_CON_DISABLE_RESPONSE,
    DBAL_CON_PING_REQUEST,
    DBAL_CON_PING_RESPONSE,
    DBAL_CON_TEMP_ENABLE_REQUEST,
    DBAL_CON_TEMP_ENABLE_RESPONSE,
    DBAL_CON_TEMP_DISABLE_REQUEST,
    DBAL_CON_TEMP_DISABLE_RESPONSE,
    DBAL_CON_MSG_TYPE_COUNT
};

typedef void (*DBAL_ioCommStateCb)(enum DBAL_CommState CommState);
typedef void (*DBAL_ioMsgLostCb)(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen);

/** \struct DBAL_Identifier
 *
 * \brief   Structure for the identification of a DBal message with a given ServiceId.
 *
 * \details For meaning of parameters see general DBal specification.
**/
struct DBAL_Identifier
{
    uint16_t                        CommandId;
    enum DBAL_MessageType           Type;
};

typedef void (*DBAL_Service)(const uint8_t* const Data, uint8_t DataLen);

/** \struct DBAL_ReceiveObject
 *
 * \brief   Structure for handling an incoming dbal message.
**/
struct DBAL_ReceiveObject
{
    struct DBAL_Identifier      Identifier;
    DBAL_Service                Service;
};

/** \struct DBAL_ObjectTableEntry
 *
 * \brief   Structure describing a receiving object table for all the DBal frames defined under a specific ServiceId.
 *
 * \details For meaning of parameter "ServiceId" see general DBal specification.
**/
struct DBAL_ObjectTableEntry
{
    const struct DBAL_ReceiveObject *ReceiveObject;
    const uint16_t                  ServiceId;
    const uint8_t                   ReceiveObjectCount;
};

// Public function declarations (placeholders for now)
void dbal_init(void);
bool dbal_send_cmd_response(uint16_t service_id, uint16_t command_id, const uint8_t* data, uint8_t data_len);
bool dbal_send_query_response(uint16_t service_id, uint16_t command_id, const uint8_t* data, uint8_t data_len);
bool dbal_send_event(uint16_t service_id, uint16_t command_id, const uint8_t* data, uint8_t data_len);

#endif // ZEPHYR_DBUS_APP_LAYER_H__