# Architectural Design: DBus Driver for Zephyr RTOS

## 1. Overview

This document outlines the architectural design for porting the existing DBus driver to the Zephyr RTOS environment. The goal is to integrate the DBus communication functionalities seamlessly with Zephyr's kernel services and device driver model, while maintaining the existing application layer interface as much as possible.

## 2. Key Architectural Decisions

### 2.1. Threading Model

The existing DBus driver likely operates within a single main loop or a simple bare-metal scheduler. In Zephyr, we will leverage its multi-threading capabilities to handle DBus communication efficiently and asynchronously.

-   **DBus Transmit Thread:** A dedicated Zephyr thread will be responsible for queuing and sending DBus messages. This thread will manage the transmission buffer and interact with the underlying CAN/UART device driver.
-   **DBus Receive Thread:** Another dedicated Zephyr thread will handle incoming DBus messages. This thread will read data from the CAN/UART device driver, parse DBus frames, and dispatch them to the appropriate application layer callbacks.
-   **Application Interaction:** Application modules will interact with the DBus driver through a set of public APIs, which will internally use Zephyr's IPC mechanisms to communicate with the DBus transmit/receive threads.

### 2.2. Inter-Process Communication (IPC)

Zephyr offers various IPC mechanisms. For the DBus driver, the following will be considered:

-   **Message Queues (`k_msgq`):**
    *   **Application to DBus Transmit Thread:** Application modules will place outgoing DBus messages into a message queue. The DBus Transmit Thread will consume messages from this queue. This decouples the application from the transmission timing.
    *   **DBus Receive Thread to Application:** Parsed incoming DBus messages (or events/callbacks) can be placed into a message queue for the application to process, or direct callbacks can be used if real-time processing is critical. Given the existing callback model, direct callbacks might be preferred for received messages, while a message queue could be used for internal driver events or status updates.
-   **Semaphores (`k_sem`) / Mutexes (`k_mutex`):**
    *   Used for protecting shared resources, such as internal driver state variables, message buffers, or critical sections during configuration.

### 2.3. Device Driver Integration (CAN/UART)

The existing DBus driver relies on low-level peripheral drivers (CAN or UART). In Zephyr, these will be integrated using Zephyr's unified device model.

-   **Zephyr Device API:** The DBus driver will interact with the underlying communication peripheral (CAN or UART) through Zephyr's generic device API. This promotes portability across different hardware platforms supported by Zephyr.
-   **Callbacks/Interrupts:** The Zephyr CAN/UART drivers will be configured to generate callbacks or signal events upon message reception or transmission completion. These will trigger the DBus Receive/Transmit threads.

### 2.4. Configuration Management

The existing `DBal_cfg.c` file handles configuration. In Zephyr, configuration is typically managed via Kconfig and Device Tree.

-   **Kconfig:** Used for compile-time configuration options, such as enabling/disabling DBus features, setting buffer sizes, or defining communication parameters.
-   **Device Tree (`.dts`):** Used to describe the hardware peripherals (CAN/UART) and their properties, which the Zephyr device drivers will utilize.

## 3. High-Level Architecture Diagram (Mermaid)

```mermaid
graph TD
    subgraph Application Layer
        AppModule1[Application Module 1]
        AppModule2[Application Module 2]
    end

    subgraph DBus Driver (Zephyr)
        direction LR
        DBusAPI[DBus Public API]
        DBusTxThread[DBus Transmit Thread]
        DBusRxThread[DBus Receive Thread]
        DBusInternalLogic[DBus Internal Logic]
    end

    subgraph Zephyr RTOS
        Kernel[Zephyr Kernel: Threads, IPC, Timers]
        DeviceDriver[Zephyr Device Driver: CAN/UART]
        Kconfig[Kconfig]
        DeviceTree[Device Tree]
    end

    subgraph Hardware
        CAN_UART_HW[CAN/UART Hardware]
    end

    AppModule1 --> DBusAPI
    AppModule2 --> DBusAPI

    DBusAPI --> MessageQueueTx[Message Queue (Tx)]
    MessageQueueTx --> DBusTxThread

    DBusTxThread --> DeviceDriver: Send Data
    DeviceDriver --> CAN_UART_HW: Transmit

    CAN_UART_HW --> DeviceDriver: Receive Data
    DeviceDriver --> DBusRxThread

    DBusRxThread --> DBusInternalLogic: Parse & Dispatch
    DBusInternalLogic --> Callbacks[Application Callbacks]

    DBusAPI --> DBusInternalLogic: Direct Calls/Config
    DBusInternalLogic --> Kernel: OS Services
    DBusTxThread --> Kernel: OS Services
    DBusRxThread --> Kernel: OS Services

    Kernel --> DeviceDriver
    Kconfig --> ZephyrRTOS[Zephyr RTOS Build]
    DeviceTree --> ZephyrRTOS
    ZephyrRTOS --> DeviceDriver
```

## 4. Detailed Design Considerations

### 4.1. DBus Public API

The existing `DBAL_sendCmdResponse`, `DBAL_sendQueryResponse`, `DBAL_sendEvent` (and their `DBALCR_` counterparts for cross-connection) will be retained as the primary interface for the application. These functions will be adapted to:
-   Place messages into the `MessageQueueTx`.
-   Handle return status based on queueing success.

### 4.2. DBus Transmit Thread

-   **Priority and Stack Size:** Determine appropriate thread priority and stack size based on message volume and real-time requirements.
-   **Message Processing:** Loop, waiting on `k_msgq_get` for outgoing messages.
-   **Hardware Interaction:** Call Zephyr CAN/UART device driver APIs to send messages. Handle transmission completion callbacks/signals.
-   **Retransmission Logic:** Adapt the existing retransmission logic (`DBAL_isMsgOfDBalType2Repeat`, `DBAL_saveMsg2Repeat`, `DBAL_msgTimerAction`) to use Zephyr timers (`k_timer`) and potentially a separate workqueue for deferred processing if needed.

### 4.3. DBus Receive Thread

-   **Priority and Stack Size:** Determine appropriate thread priority and stack size.
-   **Hardware Interaction:** Loop, waiting on `k_poll` or `k_sem_take` (signaled by the CAN/UART driver's ISR) for incoming data.
-   **Frame Parsing:** Implement the existing DBus frame parsing logic (`DBAL_look4MsgReception`, `DBAL_look4AckMsgReception`).
-   **Callback Dispatch:** Invoke the appropriate application callbacks (`DBAL_Service`, `DBALCR_Service`) for received messages.
-   **Acknowledgement Handling:** Process acknowledgements and update the retransmission queue.

### 4.4. Error Handling and Logging

-   Integrate existing error handling (`DBAL_ntfUnknownDBalFrameReceived`, `DBAL_ntfCorruptReqRespDbus2FrameReceived`) with Zephyr's logging subsystem (`LOG_INF`, `LOG_WRN`, `LOG_ERR`).
-   Utilize Zephyr's `k_panic` or `k_oops` for unrecoverable errors.

### 4.5. Timer Management

-   Replace existing `STIM` (System Timer) usage with Zephyr's `k_timer` API for message retransmission and connection state management.

## 5. Open Questions / Areas for Further Investigation

-   **Exact Zephyr Board Support:** Which specific Zephyr BSP will be used for the target microcontroller (e.g., `stm32g071` or `gd32f30`)? This will influence the specific device driver APIs.
-   **Existing `bsh_stdinc.h` and other common includes:** How will these common utility headers be integrated or replaced with Zephyr equivalents?
-   **Memory Management:** Detailed analysis of memory usage for message queues, thread stacks, and buffers to ensure fit within target constraints.
-   **Interrupt Handling:** How are the current low-level CAN/UART interrupts handled, and how will this map to Zephyr's interrupt handling mechanisms?