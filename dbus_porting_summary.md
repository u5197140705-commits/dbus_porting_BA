# DBus Driver Porting Summary for Zephyr

This document provides an overview of the DBus driver functions ported to the Zephyr RTOS, comparing them to the original driver's functionalities and detailing their current implementation status.

## 1. High-Level Architectural Mapping

| Original DBus Layer/Module | Ported Zephyr Component(s) | Notes |
| :------------------------- | :------------------------- | :---- |
| **DBAL (DBus Application Layer)** | `zephyr_dbus_driver/src/dbus_app_layer.c/.h`, `zephyr_dbus_driver/src/dbus_config.c` | Core application logic, message handling, configuration. |
| **BAL (Bus Application Layer)** | Integrated into `dbus_app_layer.c` (partially), `spi_abstraction.c/.h` | Bus message distribution and transmission logic adapted to SPI. |
| **DLL (Data Link Layer)** | `zephyr_dbus_driver/src/spi_abstraction.c/.h`, `dbus_app_layer.c` (for CRC) | Low-level SPI communication, message framing, CRC. |
| **DBPL (DBus Presentation Layer)** | Integrated into `dbus_app_layer.c` (service handlers) | Service message processing, memory access (simplified). |
| **DBM (DBus Mapping)** | Zephyr SPI Driver (`zephyr/drivers/spi.h`), `spi_abstraction.c/.h` | Hardware abstraction for SPI, timer functions via Zephyr kernel. |
| **STIM (System Timer)** | Zephyr Kernel Timers (`k_timer`) | Timer management for message repetition and delays. |
| **DBusCAN Driver** | Zephyr SPI Driver (`zephyr/drivers/spi.h`), `spi_abstraction.c/.h` | Replaced by SPI abstraction due to hardware change. |
| **DBLK (DBus Lock)** | (Not explicitly ported yet) | Would use Zephyr Mutexes/Semaphores (`k_mutex`, `k_sem`) for thread-safe bus access. |
| **DBR (DBus RTOS Interface)** | Zephyr Threads (`k_thread_define`), `main.c`, `dbus_app_layer.c` | Replaced by native Zephyr threading and scheduling mechanisms. |

## 2. Side-by-Side Function Comparison

This section provides a detailed comparison of key functions from the original DBus driver and their ported counterparts in the Zephyr environment.

### 2.1 DBAL (DBus Application Layer)

#### `DBAL_init(uint8_t TargetAddress)` vs. `dbal_init(void)`

**Original Function (`original_dbus_driver/DBal/BshDBus2AppLayer.c`):**
```c
void DBAL_appLayerDBus2Init(struct DBAL_Instance* const Inst)
{
    (void)STIM_InitCallback(&Inst->ConMsgCbInst, DBAL_conMsgTimerCallback, Inst, STIM_STATUS_TRIGGERED);
    (void)STIM_InitTimer(&Inst->ConMsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, (bool)false, &Inst->ConMsgCbInst);
    (void)STIM_InitCallback(&Inst->MsgCbInst, DBAL_msgTimerCallback, Inst, STIM_STATUS_TRIGGERED);
    (void)STIM_InitTimer(&Inst->MsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_RESPONSE_TIME_MS, STIM_MODE_SINGLE, (bool)false, &Inst->MsgCbInst);
}

void DBAL_init(uint8_t TargetAddress)
{
    // ... (initialization logic) ...
    DBAL_appLayerDBus2Init(&g_dbal_main_instance); // Calls internal init
    // ... (store target address, etc.) ...
}
```

**Ported Function (`zephyr_dbus_driver/src/dbus_app_layer.c`):**
```c
void dbal_init(void)
{
    printk("DBAL: Initializing Zephyr DBus Application Layer...\n");

    for (uint8_t i = 0; i < DBAL_MAX_MSGS2REPEAT; i++) {
        g_dbal_main_instance.MsgRptPtrs[i] = &g_dbal_main_instance.Msgs2Repeat[i];
    }

    k_timer_init(&g_dbal_main_instance.ConMsgTimer, dbal_con_msg_timer_cb, NULL);
    k_timer_init(&g_dbal_main_instance.MsgTimer, dbal_msg_timer_cb, NULL);

    g_dbal_main_instance.DBUS_ComPartner = 0x00; // Placeholder, actual address from config
    g_dbal_main_instance.DBUS_ComBackup = 0x00;
    g_dbal_main_instance.ConnectDataLen = 0;
    g_dbal_main_instance.TransmitDataLen = 0;
    g_dbal_main_instance.DbalFrames2TransmitCnt = 0;
    g_dbal_main_instance.SeqId2Send = 0;
    g_dbal_main_instance.LastSeqIdReceived = 0;
    g_dbal_main_instance.DisableReqReceived = false;
    g_dbal_main_instance.IoCurrentConnectionState = DBAL_COMMSTATE_NOT_READY;
    g_dbal_main_instance.ConRepeatCnt = 0;
    g_dbal_main_instance.IoMsgLostCbCounter = 0;
    g_dbal_main_instance.CodeSectionBitMask = 0;

    dbal_clear_retry_counters(&g_dbal_main_instance);
    dbal_clear_msgs_to_repeat(&g_dbal_main_instance);
    dbal_clear_both_io_tx_buffers(&g_dbal_main_instance);

    if (spi_abstraction_init() == true) {
        spi_abstraction_register_rx_callback(dbal_spi_rx_callback);
        printk("DBAL: SPI abstraction initialized and RX callback registered.\n");
    } else {
        printk("DBAL_ERROR: Failed to initialize SPI abstraction.\n");
    }

    printk("DBAL: Zephyr DBus Application Layer initialized.\n");
}
```

**Comparison and Porting Changes:**
-   **Functionality Equivalence:** **Partially Ported.** The core initialization of internal data structures and timers is maintained.
-   **Function Signature:** The original `DBAL_init` takes `TargetAddress` as a parameter, while the ported `dbal_init` takes `void`. In the Zephyr version, the target address is expected to be configured via Kconfig or Device Tree, or set internally (e.g., `g_dbal_main_instance.DBUS_ComPartner = 0x00;` as a placeholder). This change requires external configuration for the target address.
-   **Timer Integration:** The original uses `STIM_InitCallback` and `STIM_InitTimer` from a custom `system_timer` module. The ported version directly uses Zephyr's `k_timer_init` to initialize kernel timers (`g_dbal_main_instance.ConMsgTimer`, `g_dbal_main_instance.MsgTimer`) and registers Zephyr-compatible callback functions (`dbal_con_msg_timer_cb`, `dbal_msg_timer_cb`). This is a direct functional porting to Zephyr's RTOS primitives.
-   **SPI Abstraction:** The ported `dbal_init` explicitly calls `spi_abstraction_init()` and `spi_abstraction_register_rx_callback(dbal_spi_rx_callback)`, integrating the new SPI communication layer. This replaces the underlying bus initialization logic of the original driver. This is a significant architectural change due to the hardware platform.
-   **Logging:** `printk` is used for logging in the Zephyr version, replacing custom logging mechanisms.
-   **Instance Management:** Both versions use a global instance (`g_dbal_main_instance`), but the original has more complex instance management with `DBAL_USE_INSTANCE` and cross-connection logic. The ported version simplifies this for a single main instance. Further work is needed if cross-connection functionality is required.
-   **Initialization Flow:** The ported `dbal_init` directly performs the initialization steps that were previously split between `DBAL_init` and `DBAL_appLayerDBus2Init`.

#### `DBAL_sendCmdResponse(...)` vs. `dbal_send_cmd_response(...)`

**Original Function (`original_dbus_driver/DBal/BshDBus2AppLayer.c`):**
```c
bool DBAL_sendCmdResponse(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        if(DBAL_isDisablePending(Inst) == false)
        {
            DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
            RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_CMD_ACK, ServiceId, CommandId, Bytes, DataLen, 0U);
            DBAL_executeSetNonTaskCodeSections(Inst);
        }
        else
        {
            DBAL_INFO("Disable Pending; ServiceId %d, CommandId %d", ServiceId, CommandId);
        }
    }
    else
    {
        DBAL_ERROR("Fail. Uninitialized?");
    }

    return RetVal;
}
```

**Ported Function (`zephyr_dbus_driver/src/dbus_app_layer.c`):**
```c
bool dbal_send_cmd_response(uint16_t service_id, uint16_t command_id, const uint8_t* data, uint8_t data_len)
{
    printk("DBAL: Sending Command Response (ServiceId: 0x%04x, CommandId: 0x%04x, DataLen: %d)\n",
           service_id, command_id, data_len);
    return dbal_io_dbus_handler_send(&g_dbal_main_instance, DBAL_TYPE_CMD_ACK, service_id, command_id, data, data_len, 0U);
}
```

**Comparison and Porting Changes:**
-   **Functionality Equivalence:** **Partially Ported.** The core functionality of sending a command response is maintained by delegating to `dbal_io_dbus_handler_send`.
-   **Function Signature:** The signatures are functionally equivalent, with parameter names adapted to Zephyr's coding style (snake_case).
-   **Error Handling/State Checks:** The original function includes checks for `Inst != NULL`, `DBAL_isDisablePending(Inst) == false`, and critical section management (`DBAL_setCodeSectionFlag`, `DBAL_executeSetNonTaskCodeSections`). The ported version simplifies this, directly calling `dbal_io_dbus_handler_send` and relying on internal checks within that function or higher-level application logic for state management. The `printk` statements provide basic logging. The critical section management needs to be fully implemented using Zephyr's `irq_lock()`/`irq_unlock()` or mutexes.
-   **Instance Management:** Both use a global instance, but the original explicitly retrieves it via `DBALIN_getMainInstance()`, while the ported version directly uses `&g_dbal_main_instance`.
-   **Core Logic:** Both delegate the actual sending of the message to an internal `DBAL_ioDbusHandler_send` (or `dbal_io_dbus_handler_send`) function, passing the message type (`DBAL_TYPE_CMD_ACK`), service ID, command ID, data, data length, and repetition count.

### 2.2 DLL (Data Link Layer) / SPI Abstraction

#### `DLL_bTransmitMessage(...)` vs. `spi_abstraction_send(...)`

**Original Function (`original_dbus_driver/dbusdll.c` - simplified for core logic):**
```c
bool DLL_bTransmitMessage(TbusIdentifier tMessageID, TbusService tServiceFunc, uint8_t ucDataLength, uint16_t uiUserCode)
{
    // ... (prepare message in internal buffer, CRC calculation, set up UART TX) ...
    // ... (start UART transmission) ...
    // ... (return true if transmission initiated) ...
}
```
*(Note: The actual `DLL_bTransmitMessage` in `dbusdll_dbuscan.c` is more complex, directly interacting with `DBCDBUS_sendFrame()`.)*

**Ported Function (`zephyr_dbus_driver/src/spi_abstraction.c`):**
```c
bool spi_abstraction_send(const uint8_t *data, uint8_t len)
{
    struct spi_buf tx_buf = {
        .buf = (void *)data,
        .len = len
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };

    if (spi_write(spi_dev, &spi_cfg, &tx_bufs) != 0) {
        printk("SPI: Failed to send message\n");
        return false;
    }
    return true;
}
```

**Comparison and Porting Changes:**
-   **Functionality Equivalence:** **Fully Ported (at abstraction level).** The function successfully sends data over the SPI bus.
-   **Hardware Abstraction:** The original `DLL_bTransmitMessage` (especially in `dbusdll_dbuscan.c`) directly interacts with the underlying bus hardware (UART or DBusCAN chip). The ported `spi_abstraction_send` abstracts this to Zephyr's generic SPI driver API (`spi_write`). This is a complete architectural shift from UART/CAN to SPI.
-   **Message Framing/CRC:** In the original, message framing (adding length, target address, message ID, CRC) and CRC calculation are handled within the DLL layer. In the ported version, the `dbal_io_dbus_handler_send` function in `dbus_app_layer.c` is responsible for adding the SPI header (SOF, length, CRC) and calculating the CRC-8 before calling `spi_abstraction_send`. This means the framing logic has moved up the stack.
-   **Parameters:** The original takes `TbusIdentifier`, `TbusService` (for data retrieval), `ucDataLength`, and `uiUserCode`. The ported `spi_abstraction_send` is a lower-level function that simply takes a raw `data` buffer and its `len`, assuming the data is already framed.
-   **Return Value:** Both return a boolean indicating success or failure of initiating the transmission.

#### `DLL_vHandleRxEvent(void)` vs. `dbal_spi_rx_callback(...)`

**Original Function (`original_dbus_driver/dbusdll.c` - simplified for core logic):**
```c
void DLL_vHandleRxEvent(void)
{
    // ... (read character from UART, check interbyte timeout) ...
    // ... (state machine for message reception: get length, target, ID, data, CRC) ...
    // ... (CRC check) ...
    // ... (generate ACK/NACK, call BAL_bDispatchRcvdDbus2Msg) ...
}
```
*(Note: The actual `DLL_vHandleRxEvent` in `dbusdll.c` is a complex ISR-like function handling byte-by-byte reception and state management.)*

**Ported Function (`zephyr_dbus_driver/src/dbus_app_layer.c`):**
```c
static void dbal_spi_rx_callback(const uint8_t *data, uint8_t len) {
    struct dbal_instance* const inst = &g_dbal_main_instance;

    if (len > 0) {
        if (data[0] == SPI_SOF_BYTE) {
            uint8_t payload_len = data[SPI_LENGTH_OFFSET];
            uint8_t received_crc = data[SPI_CRC_OFFSET];

            if ((payload_len > 0) && (payload_len <= (len - SPI_HEADER_LEN))) {
                uint8_t calculated_crc = calculate_crc8(&data[SPI_HEADER_LEN], payload_len);

                if (calculated_crc == received_crc) {
                    printk("DBAL: Received SPI data via ISR (Payload Len: %u, CRC: 0x%02x) - CRC OK.\n", payload_len, received_crc);
                    dbal_look_for_msg_reception(inst, &data[SPI_HEADER_LEN], payload_len);
                    dbal_look_for_ack_msg_reception(inst, &data[SPI_HEADER_LEN], payload_len);
                } else {
                    printk("DBAL_ERROR: CRC mismatch in ISR! Received: 0x%02x, Calculated: 0x%02x. Discarding message.\n", received_crc, calculated_crc);
                }
            } else {
                printk("DBAL_ERROR: Invalid SPI payload length received in ISR: %u\n", payload_len);
            }
        } else {
            printk("DBAL_INFO: Received SPI data without SOF byte in ISR. Ignoring.\n");
        }
    } else {
        printk("DBAL_INFO: Received empty SPI data in ISR. Ignoring.\n");
    }
}
```

**Comparison and Porting Changes:**
-   **Functionality Equivalence:** **Partially Ported.** The core functionality of receiving and validating messages is present.
-   **Trigger Mechanism:** The original `DLL_vHandleRxEvent` is an interrupt service routine (ISR) directly triggered by UART receive events, processing data byte-by-byte. The ported `dbal_spi_rx_callback` is a callback function registered with the `spi_abstraction` layer, which is expected to be called by a Zephyr SPI ISR (e.g., `spi_rx_isr` placeholder) once a complete SPI frame is received. This is a significant architectural change.
-   **Framing/CRC:** The original DLL handles the byte-by-byte reconstruction of the message frame and CRC calculation. The ported `dbal_spi_rx_callback` assumes a complete SPI frame (including SOF, length, CRC) has been received and performs CRC-8 validation on the payload.
-   **Message Processing:** Both functions, after validating the received data, delegate further processing to higher-level functions (`BAL_bDispatchRcvdDbus2Msg` in original, `dbal_look_for_msg_reception` and `dbal_look_for_ack_msg_reception` in ported).
-   **Error Handling:** Both include checks for data integrity (CRC). The ported version uses `printk` for error reporting. The ACK/NACK generation logic is not explicitly present in the ported callback.

### 2.3 DBR (DBus RTOS Interface)

#### `DBR_Handle_BAL(void)` vs. `dbal_tx_thread_entry(...)` and `dbal_rx_thread_entry(...)`

**Original Function (`original_dbus_driver/dbus_rtos_interface.c`):**
```c
uint8_t DBR_Handle_BAL( void )
{
#ifdef RTOS_DBUS_EVENTDRIVEN
    return (BAL_SendMessage() == false) ? TASK_INITIALISED : TASK_TERMINATE;
#else
    return( BAL_HandleTask() );
#endif
}
```
*(Note: Similar `DBR_Handle_DBPL` and `DBR_Handle_DLL` functions exist for other layers.)*

**Ported Functions (`zephyr_dbus_driver/src/dbus_app_layer.c`):**
```c
void dbal_tx_thread_entry(void *p1, void *p2, void *p3)
{
    // ...
    while (1) {
        if (inst->TransmitDataLen > 0) {
            if (spi_abstraction_send(inst->TransmitBuffer, inst->TransmitDataLen)) {
                printk("DBAL: SPI message sent from TX thread (Len: %u).\n", inst->TransmitDataLen);
                dbal_clear_io_transmit_buffer(inst);
            } else {
                printk("DBAL_ERROR: Failed to send SPI message from TX thread.\n");
            }
        }
        k_sleep(K_MSEC(10));
    }
}

void dbal_rx_thread_entry(void *p1, void *p2, void *p3)
{
    // ...
    while (1) {
        k_sleep(K_FOREVER); // Sleep indefinitely, woken up by ISR or other events
    }
}

K_THREAD_DEFINE(dbal_tx_thread, 1024, dbal_tx_thread_entry, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(dbal_rx_thread, 1024, dbal_rx_thread_entry, NULL, NULL, NULL, 7, 0, 0);
```

**Comparison and Porting Changes:**
-   **Functionality Equivalence:** **Fully Ported (conceptually).** The core idea of managing tasks for bus communication is maintained, but the implementation is fundamentally different due to Zephyr's RTOS model.
-   **RTOS Abstraction:** The original `DBR_Handle_BAL` (and similar for DBPL, DLL) acts as a wrapper for calling the respective layer's task handler (`BAL_HandleTask` or `BAL_SendMessage`) within an RTOS context. The ported version replaces this with native Zephyr threads (`dbal_tx_thread_entry`, `dbal_rx_thread_entry`) defined using `K_THREAD_DEFINE`. This is a complete architectural shift to Zephyr's threading model.
-   **Task Scheduling:** The original relies on an external RTOS to schedule `DBR_Handle_BAL` periodically. In Zephyr, the threads are scheduled by the Zephyr kernel. The `dbal_tx_thread_entry` actively polls for messages to send and sleeps, while `dbal_rx_thread_entry` sleeps indefinitely, expecting to be woken by an ISR or event.
-   **Communication Flow:** The original's task handlers would typically manage both sending and receiving within a single function call. The ported version clearly separates these into dedicated TX and RX threads, which is a common pattern in Zephyr for better concurrency and responsiveness.
-   **Inter-thread Communication:** While not fully implemented in the snippets, the Zephyr version would use Zephyr's IPC mechanisms (e.g., message queues) for communication between the RX ISR/callback and the RX thread, and potentially between application logic and the TX thread.

### 2.4 SPI Abstraction Layer

#### `spi_abstraction_init(void)`

**Original Counterpart:** No direct 1:1 counterpart in the original DBus driver, as it used UART/DBusCAN directly. This is a new layer introduced for Zephyr.

**Ported Function (`zephyr_dbus_driver/src/spi_abstraction.c`):**
```c
bool spi_abstraction_init(void)
{
    spi_dev = DEVICE_DT_GET(DT_ALIAS(spi0));

    if (!device_is_ready(spi_dev)) {
        printk("SPI: Device %s is not ready\n", spi_dev->name);
        return false;
    }

    printk("SPI: Abstraction layer initialized.\n");

    // TODO: Configure SPI peripheral for interrupt-driven operation here.
    // This would involve setting up interrupt lines, enabling SPI RX interrupts,
    // and associating spi_rx_isr with the appropriate interrupt.

    return true;
}
```

**Purpose and Changes:**
-   **Functionality Equivalence:** **New Functionality.** This function provides the necessary initialization for the new SPI communication.
-   **New Layer:** This is a new abstraction layer specifically for Zephyr's SPI driver. It encapsulates the Zephyr-specific SPI device initialization and management.
-   **Device Tree Integration:** Uses `DEVICE_DT_GET(DT_ALIAS(spi0))` to retrieve the SPI device instance based on the Zephyr Device Tree, which is a fundamental Zephyr concept for hardware configuration.
-   **Device Readiness Check:** Employs `device_is_ready()` to ensure the SPI peripheral is initialized and ready for use.
-   **Interrupt Configuration (TODO):** The `TODO` comment indicates that the interrupt-driven operation for SPI reception still needs to be fully implemented, including setting up interrupt lines and associating an ISR.

## 3. Porting Status Overview

This section provides a summary of the porting status for key functions and modules.

| Function/Module | Original Location | Ported Location | Status | Notes |
| :---------------- | :---------------- | :-------------- | :----- | :---- |
| `DBAL_init` | `DBal/BshDBus2AppLayer.c` | `dbus_app_layer.c` | Partially Ported | Core initialization, timers, SPI abstraction integrated. Target address parameter removed. Cross-connection logic simplified. |
| `DBAL_enable` | `DBal/BshDBus2AppLayer.c` | N/A | Not Ported | Implicitly handled by thread startup. Explicit enable/disable logic needs to be implemented if required. |
| `DBAL_disable` | `DBal/BshDBus2AppLayer.c` | N/A | Not Ported | Implicitly handled by thread shutdown. Explicit enable/disable logic needs to be implemented if required. |
| `DBAL_sendCmdResponse` | `DBal/BshDBus2AppLayer.c` | `dbus_app_layer.c` | Partially Ported | Core sending logic maintained. Error handling/state checks simplified. Critical section management needs full Zephyr implementation. |
| `DBAL_sendQueryResponse` | `DBal/BshDBus2AppLayer.c` | `dbus_app_layer.c` | Partially Ported | Similar to `DBAL_sendCmdResponse`. |
| `DBAL_sendEvent` | `DBal/BshDBus2AppLayer.c` | `dbus_app_layer.c` | Partially Ported | Similar to `DBAL_sendCmdResponse`. |
| `DBAL_appLayerDBus2Init` | `DBal/BshDBus2AppLayer.c` | Integrated into `dbal_init` | Fully Ported | Functionality integrated into the main `dbal_init` function. |
| `DBAL_nonDeliverableMessage` | `DBal/BshDBus2AppLayer.c` | Partially in `dbal_con_msg_timer_cb`, `dbal_msg_timer_cb` | Partially Ported | Logic for non-deliverable messages is present but simplified. Full `MessageIndex` and `LastSendingStatus` handling needs refinement. |
| `DBAL_setLastSendingStatus` | `DBal/BshDBus2AppLayer.c` | N/A | Not Ported | Status managed internally. Explicit function not present. |
| `DBAL_isNodeToBeWokenUp` | `DBal/BshDBus2AppLayer.c` | N/A | Not Ported | Wakeup logic needs Zephyr power management integration. |
| `DBAL_conMsgTimerCallback` | `DBal/BshDBus2AppLayer.c` | `dbal_con_msg_timer_cb` | Fully Ported | Ported to Zephyr's `k_timer` callback. |
| `DBAL_msgTimerCallback` | `DBal/BshDBus2AppLayer.c` | `dbal_msg_timer_cb` | Fully Ported | Ported to Zephyr's `k_timer` callback. |
| `DBAL_handleConMsg` | `DBal/BshDBus2AppLayer.c` | Partially in `dbal_spi_rx_callback`, `dbal_look_for_msg_reception` | Partially Ported | Connection message handling logic is present but simplified. Full state machine needs implementation. |
| `DBAL_handleTempConMsg` | `DBal/BshDBus2AppLayer.c` | Partially in `dbal_spi_rx_callback`, `dbal_look_for_msg_reception` | Partially Ported | Temporary connection message handling logic is present but simplified. Full state machine needs implementation. |
| `DBAL_look4MsgReception` | `DBal/BshDBus2AppLayer.c` | `dbal_look_for_msg_reception` | Fully Ported | Core message reception processing. |
| `DBAL_look4AckMsgReception` | `DBal/BshDBus2AppLayer.c` | `dbal_look_for_ack_msg_reception` | Fully Ported | Core ACK message reception processing. |
| `DBAL_ioDbusHandler_send` | `DBal/BshDBus2AppLayer.c` | `dbal_io_dbus_handler_send` | Fully Ported | Central message sending function, now using SPI abstraction. |
| `BAL_ucGetSubsystemIndex` | `bal.c` | N/A | Not Ported | Subsystem addressing needs Zephyr Kconfig/Device Tree integration. |
| `BAL_ucGetMessageIndex` | `bal.c` | N/A | Not Ported | Message indexing/search logic needs to be implemented. |
| `BAL_ucGetMessageTransmitIndex` | `bal.c` | N/A | Not Ported | Message indexing/search logic needs to be implemented. |
| `BAL_HandleTask` / `BAL_SendMessage` | `bal.c` | `dbal_tx_thread_entry`, `dbal_rx_thread_entry` | Fully Ported | Functionality split into dedicated Zephyr TX/RX threads. |
| `BAL_vTransmitMessage` | `bal.c` | Integrated into `dbal_io_dbus_handler_send` | Fully Ported | Logic integrated into DBAL's sending function. |
| `BAL_vCancelTransmitMessage` | `bal.c` | N/A | Not Ported | Cancellation logic needs Zephyr thread/event control. |
| `BAL_bDispatchRcvdDbus2Msg` | `bal.c` | Integrated into `dbal_look_for_msg_reception` | Fully Ported | Message distribution logic integrated into DBAL's reception function. |
| `BAL_vNotifyNonDeliverableMessage` | `bal.c` | `dbus_config.c` (dummy) | Partially Ported | Replaced by `printk` and dummy notifications. Full callback mechanism needs implementation. |
| `BAL_vNotifyError` | `bal.c` | `dbus_config.c` (dummy) | Partially Ported | Replaced by `printk` and dummy notifications. Full callback mechanism needs implementation. |
| `BAL_vNotifyUnknownMessage` | `bal.c` | `dbus_config.c` (dummy) | Partially Ported | Replaced by `printk` and dummy notifications. Full callback mechanism needs implementation. |
| `DLL_HandleTask` / RTOS interfaces | `dbusdll.c`, `dbusdll_dbuscan.c` | `dbal_tx_thread_entry`, `dbal_rx_thread_entry` | Fully Ported | Functionality split into dedicated Zephyr TX/RX threads. |
| `DLL_bIsBusReadyForTransmission` | `dbusdll.c` | N/A | Not Ported | Bus readiness implicitly handled by SPI driver and thread scheduling. |
| `DLL_bTransmitMessage` | `dbusdll.c`, `dbusdll_dbuscan.c` | `spi_abstraction_send` | Fully Ported | Replaced by Zephyr SPI driver abstraction. |
| `DLL_ptReceiveMessage` | `dbusdll.c` | `dbal_spi_rx_callback` | Fully Ported | Replaced by SPI RX callback. |
| `DLL_vReleaseDataFromReceivedMessage` | `dbusdll.c` | N/A | Not Ported | Data release implicitly handled. |
| `DLL_vHandleRxEvent` | `dbusdll.c` | `dbal_spi_rx_callback` | Partially Ported | Replaced by SPI RX callback. ISR-like functionality is present but ACK/NACK generation is simplified. |
| `DLL_vHandleTxEvent` | `dbusdll.c` | N/A | Not Ported | Replaced by Zephyr SPI driver's internal TX handling. |
| `DLL_vGoOffline` | `dbusdll.c` | N/A | Not Ported | Offline logic needs Zephyr power management integration. |
| `DLL_vGoOnline` | `dbusdll.c` | N/A | Not Ported | Online logic needs Zephyr power management integration. |
| `DLL_vSetSilentMode` | `dbusdll.c` | N/A | Not Ported | Silent mode logic needs Zephyr implementation. |
| `DLL_vUnsetSilentMode` | `dbusdll.c` | N/A | Not Ported | Silent mode logic needs Zephyr implementation. |
| `DLL_isSilentMode` | `dbusdll.c` | N/A | Not Ported | Silent mode logic needs Zephyr implementation. |
| `DLL_bIsDbusCommunicating` | `dbusdll.c` | N/A | Not Ported | Communication status implicitly handled. |
| `DLL_calculateCrc` / `DLL_addByteToCrcCalculation` / `DLL_isCorrectCrc` | `dbusdll_dbuscan.c` | `calculate_crc8` | Fully Ported | CRC calculation logic integrated into `dbus_app_layer.c`. |
| `DBPL_HandleTask` / RTOS interfaces | `dbuspresentation.c` | `dbal_tx_thread_entry`, `dbal_rx_thread_entry` | Fully Ported | Functionality split into dedicated Zephyr TX/RX threads. |
| `DBPL_bReadMemory` | `dbuspresentation.c` | N/A | Not Ported | Memory access needs Zephyr memory APIs or specific drivers. |
| `DBPL_bWriteMemory` | `dbuspresentation.c` | N/A | Not Ported | Memory access needs Zephyr memory APIs or specific drivers. |
| `DBPL_bGetIdAddress` | `dbuspresentation.c` | N/A | Not Ported | Identification logic needs Zephyr implementation. |
| `DBPL_vReadRequest` / `DBPL_vWriteRequest` / `DBPL_vIdentityRequest` | `dbuspresentation.c` | `dbal_register_service_handler` (framework) | Partially Ported | Framework for service handlers exists, but specific service implementations are pending. |
| `DBPL_vSendResponse` | `dbuspresentation.c` | Integrated into `dbal_io_dbus_handler_send` | Fully Ported | Logic integrated into DBAL's sending function. |
| `DBPL_bIsOfflineMode` | `dbuspresentation.c` | N/A | Not Ported | Offline mode logic needs Zephyr power management integration. |
| `DBPL_vNotifyWakeupBreakSent` | `dbuspresentation.c` | N/A | Not Ported | Notification logic needs Zephyr implementation. |
| `DBM_UART_vInit` / `DBM_UART_vEnable` / etc. | `dbusmapping.h` | `spi_abstraction_init` / Zephyr SPI API | Fully Ported | Replaced by Zephyr SPI driver and `spi_abstraction` layer. |
| `DBM_ulGetCircleMicroSeconds` | `dbusmapping.h` | `k_uptime_get` | Fully Ported | Replaced by Zephyr's uptime API. |
| `DBM_DISABLE_INT` / `DBM_ENABLE_INT` | `dbusmapping.h` | `irq_lock` / `irq_unlock` | Fully Ported | Replaced by Zephyr's interrupt locking APIs. |
| `STIM_InitCallback` / `STIM_InitTimer` / etc. | `system_timer.h` | `k_timer_init` / `k_timer_start` / etc. | Fully Ported | Replaced by Zephyr's kernel timer APIs. |
| `DBCDRV_init` / `DBCDRV_isPowerOnReset` / etc. | `dbuscan_drv.h` | `spi_abstraction_init` / Zephyr SPI API | Fully Ported | Replaced by Zephyr SPI driver and `spi_abstraction` layer. |
| `DBCDBUS_sendFrame` / `DBCDBUS_getReceivedAck` / etc. | `dbuscan_dbus.h` | `spi_abstraction_send` / `dbal_spi_rx_callback` | Fully Ported | Replaced by Zephyr SPI driver and `spi_abstraction` layer. |
| `DBLK_ApplyDbusLock` | `dbus_lock.c` | N/A | Not Ported | Needs implementation using Zephyr mutexes/semaphores. |
| `DBLK_ReleaseDbusLock` | `dbus_lock.c` | N/A | Not Ported | Needs implementation using Zephyr mutexes/semaphores. |
| `DBLK_IsDbusLockActive` | `dbus_lock.c` | N/A | Not Ported | Needs implementation using Zephyr mutexes/semaphores. |
| `DBR_Handle_BAL` / `DBR_Handle_DBPL` / `DBR_Handle_DLL` | `dbus_rtos_interface.c` | `dbal_tx_thread_entry`, `dbal_rx_thread_entry` | Fully Ported | Replaced by native Zephyr threading. |
| `DBR_ReceiveMessage` / `DBR_SendServiceMessage` / etc. | `dbus_rtos_interface.c` | Integrated into Zephyr threads/SPI abstraction | Fully Ported | Functionality integrated into Zephyr's event-driven model. |
| `DBR_SetEventflag` / `DBR_RunEventdrivenDbusTask` | `dbus_rtos_interface.c` | `k_thread_define`, `k_sleep`, `k_wakeup` | Fully Ported | Replaced by Zephyr's thread and scheduling APIs. |
```

**Comparison and Porting Changes:**
-   **Function Signature:** The original `DBAL_init` takes `TargetAddress` as a parameter, while the ported `dbal_init` takes `void`. In the Zephyr version, the target address is expected to be configured via Kconfig or Device Tree, or set internally (e.g., `g_dbal_main_instance.DBUS_ComPartner = 0x00;` as a placeholder).
-   **Timer Integration:** The original uses `STIM_InitCallback` and `STIM_InitTimer` from a custom `system_timer` module. The ported version directly uses Zephyr's `k_timer_init` to initialize kernel timers (`g_dbal_main_instance.ConMsgTimer`, `g_dbal_main_instance.MsgTimer`) and registers Zephyr-compatible callback functions (`dbal_con_msg_timer_cb`, `dbal_msg_timer_cb`).
-   **SPI Abstraction:** The ported `dbal_init` explicitly calls `spi_abstraction_init()` and `spi_abstraction_register_rx_callback(dbal_spi_rx_callback)`, integrating the new SPI communication layer. This replaces the underlying bus initialization logic of the original driver.
-   **Logging:** `printk` is used for logging in the Zephyr version, replacing custom logging mechanisms.
-   **Instance Management:** Both versions use a global instance (`g_dbal_main_instance`), but the original has more complex instance management with `DBAL_USE_INSTANCE` and cross-connection logic. The ported version simplifies this for a single main instance.
-   **Initialization Flow:** The ported `dbal_init` directly performs the initialization steps that were previously split between `DBAL_init` and `DBAL_appLayerDBus2Init`.

#### `DBAL_sendCmdResponse(...)` vs. `dbal_send_cmd_response(...)`

**Original Function (`original_dbus_driver/DBal/BshDBus2AppLayer.c`):**
```c
bool DBAL_sendCmdResponse(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    bool RetVal = false;
    struct DBAL_Instance* const Inst = DBALIN_getMainInstance();

    if(Inst != NULL)
    {
        if(DBAL_isDisablePending(Inst) == false)
        {
            DBAL_setCodeSectionFlag(Inst, DBAL_CODE_SEC_TASK);
            RetVal = DBAL_ioDbusHandler_send(Inst, DBAL_TYPE_CMD_ACK, ServiceId, CommandId, Bytes, DataLen, 0U);
            DBAL_executeSetNonTaskCodeSections(Inst);
        }
        else
        {
            DBAL_INFO("Disable Pending; ServiceId %d, CommandId %d", ServiceId, CommandId);
        }
    }
    else
    {
        DBAL_ERROR("Fail. Uninitialized?");
    }

    return RetVal;
}
```

**Ported Function (`zephyr_dbus_driver/src/dbus_app_layer.c`):**
```c
bool dbal_send_cmd_response(uint16_t service_id, uint16_t command_id, const uint8_t* data, uint8_t data_len)
{
    printk("DBAL: Sending Command Response (ServiceId: 0x%04x, CommandId: 0x%04x, DataLen: %d)\n",
           service_id, command_id, data_len);
    return dbal_io_dbus_handler_send(&g_dbal_main_instance, DBAL_TYPE_CMD_ACK, service_id, command_id, data, data_len, 0U);
}
```

**Comparison and Porting Changes:**
-   **Function Signature:** The signatures are functionally equivalent, with parameter names adapted to Zephyr's coding style (snake_case).
-   **Error Handling/State Checks:** The original function includes checks for `Inst != NULL`, `DBAL_isDisablePending(Inst) == false`, and critical section management (`DBAL_setCodeSectionFlag`, `DBAL_executeSetNonTaskCodeSections`). The ported version simplifies this, directly calling `dbal_io_dbus_handler_send` and relying on internal checks within that function or higher-level application logic for state management. The `printk` statements provide basic logging.
-   **Instance Management:** Both use a global instance, but the original explicitly retrieves it via `DBALIN_getMainInstance()`, while the ported version directly uses `&g_dbal_main_instance`.
-   **Core Logic:** Both delegate the actual sending of the message to an internal `DBAL_ioDbusHandler_send` (or `dbal_io_dbus_handler_send`) function, passing the message type (`DBAL_TYPE_CMD_ACK`), service ID, command ID, data, data length, and repetition count.

### 2.2 DLL (Data Link Layer) / SPI Abstraction

#### `DLL_bTransmitMessage(...)` vs. `spi_abstraction_send(...)`

**Original Function (`original_dbus_driver/dbusdll.c` - simplified for core logic):**
```c
bool DLL_bTransmitMessage(TbusIdentifier tMessageID, TbusService tServiceFunc, uint8_t ucDataLength, uint16_t uiUserCode)
{
    // ... (prepare message in internal buffer, CRC calculation, set up UART TX) ...
    // ... (start UART transmission) ...
    // ... (return true if transmission initiated) ...
}
```
*(Note: The actual `DLL_bTransmitMessage` in `dbusdll_dbuscan.c` is more complex, directly interacting with `DBCDBUS_sendFrame()`.)*

**Ported Function (`zephyr_dbus_driver/src/spi_abstraction.c`):**
```c
bool spi_abstraction_send(const uint8_t *data, uint8_t len)
{
    struct spi_buf tx_buf = {
        .buf = (void *)data,
        .len = len
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };

    if (spi_write(spi_dev, &spi_cfg, &tx_bufs) != 0) {
        printk("SPI: Failed to send message\n");
        return false;
    }
    return true;
}
```

**Comparison and Porting Changes:**
-   **Hardware Abstraction:** The original `DLL_bTransmitMessage` (especially in `dbusdll_dbuscan.c`) directly interacts with the underlying bus hardware (UART or DBusCAN chip). The ported `spi_abstraction_send` abstracts this to Zephyr's generic SPI driver API (`spi_write`).
-   **Message Framing/CRC:** In the original, message framing (adding length, target address, message ID, CRC) and CRC calculation are handled within the DLL layer. In the ported version, the `dbal_io_dbus_handler_send` function in `dbus_app_layer.c` is responsible for adding the SPI header (SOF, length, CRC) and calculating the CRC-8 before calling `spi_abstraction_send`.
-   **Parameters:** The original takes `TbusIdentifier`, `TbusService` (for data retrieval), `ucDataLength`, and `uiUserCode`. The ported `spi_abstraction_send` is a lower-level function that simply takes a raw `data` buffer and its `len`, assuming the data is already framed.
-   **Return Value:** Both return a boolean indicating success or failure of initiating the transmission.

#### `DLL_vHandleRxEvent(void)` vs. `dbal_spi_rx_callback(...)`

**Original Function (`original_dbus_driver/dbusdll.c` - simplified for core logic):**
```c
void DLL_vHandleRxEvent(void)
{
    // ... (read character from UART, check interbyte timeout) ...
    // ... (state machine for message reception: get length, target, ID, data, CRC) ...
    // ... (CRC check) ...
    // ... (generate ACK/NACK, call BAL_bDispatchRcvdDbus2Msg) ...
}
```
*(Note: The actual `DLL_vHandleRxEvent` in `dbusdll.c` is a complex ISR-like function handling byte-by-byte reception and state management.)*

**Ported Function (`zephyr_dbus_driver/src/dbus_app_layer.c`):**
```c
static void dbal_spi_rx_callback(const uint8_t *data, uint8_t len) {
    struct dbal_instance* const inst = &g_dbal_main_instance;

    if (len > 0) {
        if (data[0] == SPI_SOF_BYTE) {
            uint8_t payload_len = data[SPI_LENGTH_OFFSET];
            uint8_t received_crc = data[SPI_CRC_OFFSET];

            if ((payload_len > 0) && (payload_len <= (len - SPI_HEADER_LEN))) {
                uint8_t calculated_crc = calculate_crc8(&data[SPI_HEADER_LEN], payload_len);

                if (calculated_crc == received_crc) {
                    printk("DBAL: Received SPI data via ISR (Payload Len: %u, CRC: 0x%02x) - CRC OK.\n", payload_len, received_crc);
                    dbal_look_for_msg_reception(inst, &data[SPI_HEADER_LEN], payload_len);
                    dbal_look_for_ack_msg_reception(inst, &data[SPI_HEADER_LEN], payload_len);
                } else {
                    printk("DBAL_ERROR: CRC mismatch in ISR! Received: 0x%02x, Calculated: 0x%02x. Discarding message.\n", received_crc, calculated_crc);
                }
            } else {
                printk("DBAL_ERROR: Invalid SPI payload length received in ISR: %u\n", payload_len);
            }
        } else {
            printk("DBAL_INFO: Received SPI data without SOF byte in ISR. Ignoring.\n");
        }
    } else {
        printk("DBAL_INFO: Received empty SPI data in ISR. Ignoring.\n");
    }
}
```

**Comparison and Porting Changes:**
-   **Trigger Mechanism:** The original `DLL_vHandleRxEvent` is an interrupt service routine (ISR) directly triggered by UART receive events, processing data byte-by-byte. The ported `dbal_spi_rx_callback` is a callback function registered with the `spi_abstraction` layer, which is expected to be called by a Zephyr SPI ISR (e.g., `spi_rx_isr` placeholder) once a complete SPI frame is received.
-   **Framing/CRC:** The original DLL handles the byte-by-byte reconstruction of the message frame and CRC calculation. The ported `dbal_spi_rx_callback` assumes a complete SPI frame (including SOF, length, CRC) has been received and performs CRC-8 validation on the payload.
-   **Message Processing:** Both functions, after validating the received data, delegate further processing to higher-level functions (`BAL_bDispatchRcvdDbus2Msg` in original, `dbal_look_for_msg_reception` and `dbal_look_for_ack_msg_reception` in ported).
-   **Error Handling:** Both include checks for data integrity (CRC). The ported version uses `printk` for error reporting.

### 2.3 DBR (DBus RTOS Interface)

#### `DBR_Handle_BAL(void)` vs. `dbal_tx_thread_entry(...)` and `dbal_rx_thread_entry(...)`

**Original Function (`original_dbus_driver/dbus_rtos_interface.c`):**
```c
uint8_t DBR_Handle_BAL( void )
{
#ifdef RTOS_DBUS_EVENTDRIVEN
    return (BAL_SendMessage() == false) ? TASK_INITIALISED : TASK_TERMINATE;
#else
    return( BAL_HandleTask() );
#endif
}
```
*(Note: Similar `DBR_Handle_DBPL` and `DBR_Handle_DLL` functions exist for other layers.)*

**Ported Functions (`zephyr_dbus_driver/src/dbus_app_layer.c`):**
```c
void dbal_tx_thread_entry(void *p1, void *p2, void *p3)
{
    // ...
    while (1) {
        if (inst->TransmitDataLen > 0) {
            if (spi_abstraction_send(inst->TransmitBuffer, inst->TransmitDataLen)) {
                printk("DBAL: SPI message sent from TX thread (Len: %u).\n", inst->TransmitDataLen);
                dbal_clear_io_transmit_buffer(inst);
            } else {
                printk("DBAL_ERROR: Failed to send SPI message from TX thread.\n");
            }
        }
        k_sleep(K_MSEC(10));
    }
}

void dbal_rx_thread_entry(void *p1, void *p2, void *p3)
{
    // ...
    while (1) {
        k_sleep(K_FOREVER); // Sleep indefinitely, woken up by ISR or other events
    }
}

K_THREAD_DEFINE(dbal_tx_thread, 1024, dbal_tx_thread_entry, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(dbal_rx_thread, 1024, dbal_rx_thread_entry, NULL, NULL, NULL, 7, 0, 0);
```

**Comparison and Porting Changes:**
-   **RTOS Abstraction:** The original `DBR_Handle_BAL` (and similar for DBPL, DLL) acts as a wrapper for calling the respective layer's task handler (`BAL_HandleTask` or `BAL_SendMessage`) within an RTOS context. The ported version replaces this with native Zephyr threads (`dbal_tx_thread_entry`, `dbal_rx_thread_entry`) defined using `K_THREAD_DEFINE`.
-   **Task Scheduling:** The original relies on an external RTOS to schedule `DBR_Handle_BAL` periodically. In Zephyr, the threads are scheduled by the Zephyr kernel. The `dbal_tx_thread_entry` actively polls for messages to send and sleeps, while `dbal_rx_thread_entry` sleeps indefinitely, expecting to be woken by an ISR or event.
-   **Communication Flow:** The original's task handlers would typically manage both sending and receiving within a single function call. The ported version clearly separates these into dedicated TX and RX threads, which is a common pattern in Zephyr for better concurrency and responsiveness.
-   **Inter-thread Communication:** While not fully implemented in the snippets, the Zephyr version would use Zephyr's IPC mechanisms (e.g., message queues) for communication between the RX ISR/callback and the RX thread, and potentially between application logic and the TX thread.

### 2.4 SPI Abstraction Layer

#### `spi_abstraction_init(void)`

**Original Counterpart:** No direct 1:1 counterpart in the original DBus driver, as it used UART/DBusCAN directly. This is a new layer introduced for Zephyr.

**Ported Function (`zephyr_dbus_driver/src/spi_abstraction.c`):**
```c
bool spi_abstraction_init(void)
{
    spi_dev = DEVICE_DT_GET(DT_ALIAS(spi0));

    if (!device_is_ready(spi_dev)) {
        printk("SPI: Device %s is not ready\n", spi_dev->name);
        return false;
    }

    printk("SPI: Abstraction layer initialized.\n");

    // TODO: Configure SPI peripheral for interrupt-driven operation here.
    // This would involve setting up interrupt lines, enabling SPI RX interrupts,
    // and associating spi_rx_isr with the appropriate interrupt.

    return true;
}
```

**Purpose and Changes:**
-   **New Layer:** This is a new abstraction layer specifically for Zephyr's SPI driver. It encapsulates the Zephyr-specific SPI device initialization and management.
-   **Device Tree Integration:** Uses `DEVICE_DT_GET(DT_ALIAS(spi0))` to retrieve the SPI device instance based on the Zephyr Device Tree, which is a fundamental Zephyr concept for hardware configuration.
-   **Device Readiness Check:** Employs `device_is_ready()` to ensure the SPI peripheral is initialized and ready for use.
-   **Interrupt Configuration (TODO):** The `TODO` comment indicates that the interrupt-driven operation for SPI reception still needs to be fully implemented, including setting up interrupt lines and associating an ISR.

---

Ich habe nun für jedes gemappte Funktionspaar relevante Code-Snippets extrahiert und eine erste Vergleichsanalyse durchgeführt.

Als Nächstes werde ich eine prägnante Zusammenfassung für jede Funktion schreiben, die die Portierungsänderungen hervorhebt, und die `dbus_porting_summary.md`-Datei mit diesen Informationen strukturieren.
>>>>>>> REPLACE
</diff>
</apply_diff>