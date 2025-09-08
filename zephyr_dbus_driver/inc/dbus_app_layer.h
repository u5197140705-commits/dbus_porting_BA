#ifndef ZEPHYR_DBUS_APP_LAYER_H__
#define ZEPHYR_DBUS_APP_LAYER_H__

#include <zephyr/kernel.h>
#include <stdbool.h>
#include <stdint.h>

// Define uchar for compatibility with original code
typedef unsigned char uchar;

// Forward declaration for internal instance structure, if needed
// Define some constants from the original DBAL for Zephyr porting
#define DBAL_MAX_MSGS2REPEAT        10U // Example value, adjust as needed
#define DBAL_DBUS_RETRY_MAX         3U  // Example value, adjust as needed
#define DBAL_RESPONSE_TIME_MS       100 // Example value, adjust as needed
#define DBAL_DBUS_RECOVERY_TIME_MS  500 // Example value, adjust as needed

// Define message index constants (from original bustypes.h or similar)
#define DBAL_MSG_INDEX_APP_LAYER_CONNECTION 0U
#define DBAL_MSG_INDEX_APP_LAYER_REQUEST    1U
#define DBAL_MSG_INDEX_APP_LAYER_RESPONSE   2U
#define DBAL_MSG_INDEX_CROSS_OFFSET         3U // Offset for cross-connection messages
#define DBAL_MSG_INDEX_APP_LAYER_CROSS_CON  (DBAL_MSG_INDEX_APP_LAYER_CONNECTION + DBAL_MSG_INDEX_CROSS_OFFSET)
#define DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ  (DBAL_MSG_INDEX_APP_LAYER_REQUEST + DBAL_MSG_INDEX_CROSS_OFFSET)
#define DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP (DBAL_MSG_INDEX_APP_LAYER_RESPONSE + DBAL_MSG_INDEX_CROSS_OFFSET)
#define DBAL_MSG_INDEX_COUNT                6U // Total number of message indices

// Define offsets within the DBus frame (from original BshDBus2AppLayer.c comments)
#define DBAL_MSG_SENDER                     0U
#define DBAL_MSG_PROTOCOL_TYPE              1U
#define DBAL_MSG_SEQID                      2U
#define DBAL_CON_MSG_TYPE                   2U
#define DBAL_CON_MSG_PROTOCOL_VERSION       3U
#define DBAL_CON_MSG_LEN                    4U // Length of connection message

#define DBAL_FRAME_PAYLOADLEN               1U
#define DBAL_FRAME_SERVICE_ID_HI            2U
#define DBAL_FRAME_SERVICE_ID_LO            3U
#define DBAL_FRAME_COMMAND_ID_HI            4U
#define DBAL_FRAME_COMMAND_ID_LO            5U
#define DBAL_FRAME_DATA_OFFSET              6U // Offset to actual data in a DBAL frame

#define BYTE_SIZE                           8U // For bit shifting

// Placeholder for DBAL_LAST_MSG2REPEAT (from original BshDBus2AppLayer.c)
#define DBAL_LAST_MSG2REPEAT                (DBAL_MAX_MSGS2REPEAT - 1U)

// Placeholder for DBAL_BUFFER_SIZE (from original BshDBus2AppLayer.c)
#define DBAL_BUFFER_SIZE                    128U // Example buffer size, adjust as needed

// SPI Message Framing
#define SPI_SOF_BYTE                        0xAA // Start of Frame byte
#define SPI_LENGTH_OFFSET                   1U   // Offset for length byte after SOF
#define SPI_HEADER_LEN                      2U   // SOF + Length byte

// Placeholder for DLL_ACK_STATUS values (from original dbusdll.h or similar)
#define DLL_ACK_NOT_RECEIVED                0U
#define DLL_ACK_OK                          1U
// Add other DLL_ACK_STATUS values as needed (e.g., DLL_ACK_BUSY, DLL_ACK_WRONG, DLL_ACK_TRANSMISSION_ABORTED)

// Placeholder for DBAL_ConnectionSmEvent (from original BshDBus2AppLayer_internal.h or similar)
enum DBAL_ConnectionSmEvent {
    DBAL_CON_SM_EVENT_ENABLE_REQUEST,
    DBAL_CON_SM_EVENT_ACCEPT,
    DBAL_CON_SM_EVENT_DISABLE_REQUEST,
    DBAL_CON_SM_EVENT_DISABLE_SILENT,
    DBAL_CON_SM_EVENT_REJECT,
    DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST,
    DBAL_CON_SM_EVENT_TEMP_DISABLE_REQUEST
};

// Placeholder for DBAL_Dbus2FrameType (from original BshDBus2AppLayer_internal.h or similar)
enum DBAL_Dbus2FrameType {
    DBAL_DBUS2_FRAME_TYPE_REQ,
    DBAL_DBUS2_FRAME_TYPE_RESP,
    DBAL_DBUS2_FRAME_TYPE_CON,
    DBAL_DBUS2_FRAME_TYPE_UNKNOWN
};

// Placeholder for DBAL_Msgs2RepeatStatus (from original BshDBus2AppLayer_internal.h or similar)
enum DBAL_Msgs2RepeatStatus {
    DBAL_MSG2REPEAT_NONE,
    DBAL_MSG2REPEAT_EXIST
};

// Placeholder for DBAL_CODE_SEC_TASK and other code section masks
#define DBAL_CODE_SEC_TASK                  0x01U
#define DBAL_CODE_SEC_REQ_RESP_POST         0x02U
#define DBAL_CODE_SEC_CR_REQ_RESP_POST      0x04U
#define DBAL_CODE_SEC_REP_TIMER             0x08U

// Placeholder for DBAL_ConnectionState (from original IoConnectionHandling.h or similar)
enum DBAL_ConnectionState {
    DBAL_CONNECTIONSTATE_DISCONNECTED,
    DBAL_CONNECTIONSTATE_CONNECTING,
    DBAL_CONNECTIONSTATE_CONNECTED,
    DBAL_CONNECTIONSTATE_DISCONNECTING
};

// Placeholder for DBALCR_ParticipantType (from original BshDBus2AppLayerCross.h)
enum DBALCR_ParticipantType
{
    DBALCR_PART_TYPE_NONE,
    DBALCR_PART_TYPE_CLIENT,
    DBALCR_PART_TYPE_SERVER
};

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

#ifdef CONFIG_DBAL_CROSS_CONNECTION
/** \struct DBALCR_ObjectTableEntry
 *
 * \brief   Structure for handling an incoming dbal cross-connection message.
 **/
struct DBALCR_ObjectTableEntry
{
    const void*                 ReceiveObject;
    uint16_t                    ServiceId;
    uint8_t                     ReceiveObjectSize;
};
#endif // CONFIG_DBAL_CROSS_CONNECTION


// Public function declarations (placeholders for now)
void dbal_init(void);
bool dbal_send_cmd_response(uint16_t service_id, uint16_t command_id, const uint8_t* data, uint8_t data_len);
bool dbal_send_query_response(uint16_t service_id, uint16_t command_id, const uint8_t* data, uint8_t data_len);
bool dbal_send_event(uint16_t service_id, uint16_t command_id, const uint8_t* data, uint8_t data_len);

#endif // ZEPHYR_DBUS_APP_LAYER_H__