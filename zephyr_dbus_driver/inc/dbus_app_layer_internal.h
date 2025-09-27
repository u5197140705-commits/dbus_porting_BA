#ifndef ZEPHYR_DBUS_APP_LAYER_INTERNAL_H__
#define ZEPHYR_DBUS_APP_LAYER_INTERNAL_H__

#include <zephyr/kernel.h>
#include <stdbool.h>
#include <stdint.h>
#include <zephyr/sys/atomic.h>
#include "dbus_app_layer.h" // Include main header for DBAL_MessageType, DBAL_CommState, DBAL_MAX_MSGS2REPEAT, DBAL_ioMsgLostCb

// Internal structure for a message to be repeated
struct dbal_msg_to_repeat {
    bool IsSlotOccupied;
    uint8_t MsgRetryCounter;
    enum DBAL_MessageType DbalType; // Assuming DBAL_MessageType is defined in dbus_app_layer.h
    uint16_t ServiceId;
    uint16_t CommandId;
    uint8_t Datalen;
    uint8_t Data[64]; // Example max data length, adjust as needed
};

// Internal structure for the DBAL instance
struct dbal_instance {
    uint8_t DBUS_ComPartner;
    uint8_t DBUS_ComBackup;
    uint8_t SendRetryCounter[3]; // Assuming 3 message types for now (connection, request, response)
    struct k_timer ConMsgTimer; // Zephyr timer for connection messages
    struct k_timer MsgTimer;    // Zephyr timer for general messages
    uint8_t ConnectTransmitBuffer[4]; // Example size for connection messages
    uint8_t ConnectDataLen;
    uint8_t TransmitBuffer[128]; // Example max transmit buffer size
    uint8_t TransmitDataLen;
    uint8_t DbalFrames2TransmitCnt;
    uint8_t SeqId2Send;
    uint8_t LastSeqIdReceived;
    bool DisableReqReceived;
    enum DBAL_CommState IoCurrentConnectionState; // Placeholder for connection state
    struct dbal_msg_to_repeat Msgs2Repeat[DBAL_MAX_MSGS2REPEAT]; // Assuming DBAL_MAX_MSGS2REPEAT is defined in dbus_app_layer.h
    struct dbal_msg_to_repeat* MsgRptPtrs[DBAL_MAX_MSGS2REPEAT]; // Pointers for managing the queue
    uint8_t ConRepeatCnt;
    uint8_t LastSendingStatus[3]; // Placeholder for DLL_ACK_STATUS
    DBAL_ioMsgLostCb IoMsgLostCbArray[1]; // Placeholder for message lost callback
    uint8_t IoMsgLostCbCounter;
    uint8_t CodeSectionBitMask; // Placeholder for critical section flags
};

// Global instance for simplicity, or could be managed via a context pointer
extern struct dbal_instance g_dbal_main_instance;

#endif // ZEPHYR_DBUS_APP_LAYER_INTERNAL_H__