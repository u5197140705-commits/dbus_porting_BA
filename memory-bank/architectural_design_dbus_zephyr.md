# Architectural Design: DBus Driver for Zephyr RTOS

## 1. Overview

This document outlines the architectural design for porting the existing DBus driver to the Zephyr RTOS environment. The goal is to integrate the DBus communication functionalities seamlessly with Zephyr's kernel services and device driver model, while maintaining the existing application layer interface as much as possible.

## 2. Key Architectural Decisions

### 2.1. Threading Model

The existing DBus driver likely operates within a single main loop or a simple bare-metal scheduler. In Zephyr, we will leverage its multi-threading capabilities to handle DBus communication efficiently and asynchronously.

-   **DBus Transmit Thread:** A dedicated Zephyr thread will be responsible for queuing and sending DBus messages. This thread will manage the transmission buffer and interact with the underlying SPI device driver.
-   **DBus Receive Thread:** Another dedicated Zephyr thread will handle incoming DBus messages. This thread will read data from the SPI device driver, parse DBus frames, and dispatch them to the appropriate application layer callbacks.
-   **Application Interaction:** Application modules will interact with the DBus driver through a set of public APIs, which will internally use Zephyr's IPC mechanisms to communicate with the DBus transmit/receive threads.

### 2.2. Inter-Process Communication (IPC)

Zephyr offers various IPC mechanisms. For the DBus driver, the following will be considered:

-   **Message Queues (`k_msgq`):**
    *   **Application to DBus Transmit Thread:** Application modules will place outgoing DBus messages into a message queue. The DBus Transmit Thread will consume messages from this queue. This decouples the application from the transmission timing.
    *   **DBus Receive Thread to Application:** Parsed incoming DBus messages (or events/callbacks) can be placed into a message queue for the application to process, or direct callbacks can be used if real-time processing is critical. Given the existing callback model, direct callbacks might be preferred for received messages, while a message queue could be used for internal driver events or status updates.
-   **Semaphores (`k_sem`) / Mutexes (`k_mutex`):**
    *   Used for protecting shared resources, such as internal driver state variables, message buffers, or critical sections during configuration.

### 2.3. Device Driver Integration (CAN/UART)

The existing DBus driver relies on low-level peripheral drivers (SPI). In Zephyr, these will be integrated using Zephyr's unified device model.
 
-   **Zephyr Device API:** The DBus driver will interact with the underlying communication peripheral (SPI) through Zephyr's generic device API. This promotes portability across different hardware platforms supported by Zephyr.
-   **Callbacks/Interrupts:** The Zephyr SPI drivers will be configured to generate callbacks or signal events upon message reception or transmission completion. These will trigger the DBus Receive/Transmit threads.

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
        DeviceDriver[Zephyr Device Driver: SPI]
        Kconfig[Kconfig]
        DeviceTree[Device Tree]
    end
 
    subgraph Hardware
        SPI_HW[SPI Hardware]
    end
 
    AppModule1 --> DBusAPI
    AppModule2 --> DBusAPI
 
    DBusAPI --> MessageQueueTx[Message Queue (Tx)]
    MessageQueueTx --> DBusTxThread
 
    DBusTxThread --> DeviceDriver: Send Data
    DeviceDriver --> SPI_HW: Transmit
 
    SPI_HW --> DeviceDriver: Receive Data
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
-   **Hardware Interaction:** Call Zephyr SPI device driver APIs to send messages. Handle transmission completion callbacks/signals.
-   **Retransmission Logic:** Adapt the existing retransmission logic (`DBAL_isMsgOfDBalType2Repeat`, `DBAL_saveMsg2Repeat`, `DBAL_msgTimerAction`) to use Zephyr timers (`k_timer`) and potentially a separate workqueue for deferred processing if needed.
 
### 4.3. DBus Receive Thread
 
-   **Priority and Stack Size:** Determine appropriate thread priority and stack size.
-   **Hardware Interaction:** Loop, waiting on `k_poll` or `k_sem_take` (signaled by the SPI driver's ISR) for incoming data.
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
-   **Interrupt Handling:** How are the current low-level SPI interrupts handled, and how will this map to Zephyr's interrupt handling mechanisms?

## 6. Zephyr D-Bus Driver Unit Test Framework Analysis

The Zephyr D-Bus driver unit test framework in `zephyr_dbus_driver/tests` is structured as follows:

1.  **Test File Structure:**
    Test files are organized under `zephyr_dbus_driver/tests/src/`. Examples include:
    *   [`minimal_test.c`](zephyr_dbus_driver/tests/src/minimal_test.c): A basic test demonstrating `ZTEST_SUITE` and `ZTEST`.
    *   [`test_dbal_init.c`](zephyr_dbus_driver/tests/src/test_dbal_init.c): Focuses on the initialization of the D-Bus Abstraction Layer (DBAL).
    *   [`test_dbus_app_layer.c`](zephyr_dbus_driver/tests/src/test_dbus_app_layer.c): Tests the D-Bus application layer's message sending functionalities.
    *   [`test_spi_abstraction.c`](zephyr_dbus_driver/tests/src/test_spi_abstraction.c): Unit tests for the `spi_abstraction` module, primarily focusing on its internal logic and interactions with mocks.
    *   [`test_dbus_spi_integration.c`](zephyr_dbus_driver/tests/src/test_dbus_spi_integration.c): An integration test that verifies the interaction between D-Bus and the SPI abstraction layer, including D-Bus method calls triggering SPI writes and D-Bus signals triggered by simulated SPI events.
    Test-specific headers like [`spi_abstraction.h`](zephyr_dbus_driver/tests/inc/spi_abstraction.h) and [`mock_types.h`](zephyr_dbus_driver/tests/inc/mock_types.h) are located in `zephyr_dbus_driver/tests/inc/`.

2.  **Test Suite and Test Case Definition:**
    *   **Test Suites:** Defined using [`ZTEST_SUITE(suite_name, fixture_suite_setup, fixture_suite_teardown, fixture_test_setup, fixture_test_teardown, fixture_test_each_case_teardown)`](zephyr_dbus_driver/tests/src/minimal_test.c:3). For example, `ZTEST_SUITE(minimal_suite, NULL, NULL, NULL, NULL, NULL);` in [`minimal_test.c`](zephyr_dbus_driver/tests/src/minimal_test.c:3).
    *   **Test Cases:** Defined using [`ZTEST(suite_name, test_case_name)`](zephyr_dbus_driver/tests/src/minimal_test.c:5). Each test case is a C function.

3.  **Assertions:**
    The framework uses standard Zephyr ZTest assertions:
    *   [`zassert_true(condition, message)`](zephyr_dbus_driver/tests/src/minimal_test.c:7): Asserts that a condition is true.
    *   [`zassert_false(condition, message)`](zephyr_dbus_driver/tests/src/test_spi_abstraction.c:55): Asserts that a condition is false.
    *   [`zassert_equal(actual, expected, message)`](zephyr_dbus_driver/tests/src/test_dbus_spi_integration.c:50): Asserts that two values are equal.
    *   [`zassert_not_null(pointer, message)`](zephyr_dbus_driver/tests/src/test_dbus_spi_integration.c:179): Asserts that a pointer is not NULL.
    *   [`zassert_mem_equal(actual_ptr, expected_ptr, size, message)`](zephyr_dbus_driver/tests/src/test_spi_abstraction.c:50): Asserts that two memory blocks are equal.

4.  **Setup and Teardown Functions:**
    Setup and teardown functions are defined within the `ZTEST_SUITE` macro.
    *   `dbal_setup(void)`: A setup function used in [`test_dbal_init.c`](zephyr_dbus_driver/tests/src/test_dbal_init.c:9) and [`test_dbus_app_layer.c`](zephyr_dbus_driver/tests/src/test_dbus_app_layer.c:10) to prepare the test environment before each test case.
    *   `dbal_teardown(void *fixture)`: A teardown function used in the same suites to clean up resources after each test case.
    *   [`spi_abstraction_setup(void)`](zephyr_dbus_driver/tests/src/test_spi_abstraction.c:17) and [`spi_abstraction_teardown(void *fixture)`](zephyr_dbus_driver/tests/src/test_spi_abstraction.c:29) are used in the `spi_abstraction_suite` to reset mock states.

5.  **Mocking Strategy:**
    External dependencies, particularly `spi_abstraction`, are mocked using a combination of:
    *   **Global Mock Variables:** Declared in [`mock_types.h`](zephyr_dbus_driver/tests/inc/mock_types.h) (e.g., `mock_spi_send_called`, `mock_spi_send_data`, `mock_spi_send_len`, `mock_rx_callback`, `mock_spi_rx_msg_queue`). These variables track calls and capture data for verification.
    *   **Wrapper Functions (`__wrap_`):** The `__wrap_spi_abstraction_send` function in [`mock_types.h`](zephyr_dbus_driver/tests/inc/mock_types.h:18) and implemented in [`test_dbus_app_layer.c`](zephyr_dbus_driver/tests/src/test_dbus_app_layer.c:136) demonstrates the use of linker wrapping to intercept calls to `spi_abstraction_send` and redirect them to a mock implementation. This allows simulating success or failure of the underlying SPI operations without interacting with actual hardware.
    *   **Test Helper Functions:** Functions like `spi_abstraction_test_inject_rx_data` (mentioned in [`test_dbus_spi_integration.c`](zephyr_dbus_driver/tests/src/test_dbus_spi_integration.c:278)) are used to simulate events or data from mocked components.

6.  **Execution Mechanism:**
    *   **Configuration:** The `prj.conf` file enables Zephyr's ZTest framework with `CONFIG_ZTEST=y` (though it's commented out in the provided `prj.conf`, it's a standard configuration for enabling testing). Other relevant configurations include `CONFIG_SPI=y`, `CONFIG_GPIO=y`, and `CONFIG_DBAL_CROSS_CONNECTION=y`, which enable necessary drivers and features for the D-Bus and SPI components.
    *   **Compilation and Execution:** Zephyr tests are typically compiled and executed using the `west build` command. The `west build` command, when run in the application directory (e.g., `zephyr_dbus_driver`), will build the application along with any enabled tests. The tests are then executed on the target board or an emulator. The ZTest framework integrates with the Zephyr build system, allowing test suites and cases to be discovered and run automatically. The `boards/frdm_rw612.overlay` file suggests that these tests are intended to run on the NXP FRDM-RW612 board, configuring device tree nodes like `flexcomm1` for SPI.