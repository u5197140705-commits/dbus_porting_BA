# D-Bus Driver SPI Integration Test Plan

This document outlines the plan for developing integration tests for the D-Bus driver that interact with actual SPI hardware, removing the mocks for `spi_abstraction`.

## 1. Identifying Test Files

*   **New Test File:** Create a new test file: `zephyr_dbus_driver/tests/src/test_dbus_spi_integration.c`. This file will house all integration tests that interact with the actual SPI hardware.
*   **Existing Test Files:** `zephyr_dbus_driver/tests/src/test_dbus_app_layer.c` will likely *not* need modification, as its purpose is unit testing with mocks.

## 2. Disabling/Removing `spi_abstraction` Mocks

*   The `spi_abstraction` mocks are currently excluded by `zephyr_dbus_driver/tests/CMakeLists.txt:7` (`list(REMOVE_ITEM app_sources ../src/spi_abstraction.c)`).
*   To disable mocks for integration tests, this line will need to be removed or commented out, and the actual `../src/spi_abstraction.c` will need to be explicitly included in the build for the integration test target.

## 3. Configuring the Zephyr Build System

*   **Kconfig:** The `zephyr_dbus_driver/prj.conf` already enables the necessary SPI configurations (`CONFIG_SPI=y`, `CONFIG_GPIO=y`, `CONFIG_SPI_INIT_PRIORITY=50`, `CONFIG_SPI_ASYNC=y`, `CONFIG_SPI_MCUX_FLEXCOMM=y`). These are suitable for using the actual SPI hardware. No further Kconfig changes are anticipated for disabling mocks, as this is handled at the CMake level.
*   **CMakeLists.txt (`zephyr_dbus_driver/tests/CMakeLists.txt`):**
    *   Modify `zephyr_dbus_driver/tests/CMakeLists.txt` to ensure the actual `../src/spi_abstraction.c` is included in the build for the integration test target. This involves removing or commenting out `list(REMOVE_ITEM app_sources ../src/spi_abstraction.c)`.
    *   Add the new integration test file `zephyr_dbus_driver/tests/src/test_dbus_spi_integration.c` to the test target's source list.

## 4. Suggested Initial Integration Test Cases

*   **Test Case 1: Basic SPI Device Presence Check:** Verify SPI bus functionality by reading a known register from an SPI device and asserting the expected value.
*   **Test Case 2: D-Bus Message Triggering SPI Write:** Set up a D-Bus service method to write to an SPI register via `spi_abstraction`. Call this method from the test and verify the write (e.g., by reading back the register).
*   **Test Case 3: SPI Event Triggering D-Bus Signal:** Configure the D-Bus driver to monitor an SPI input. Simulate an SPI event and verify that a corresponding D-Bus signal is emitted and received by the test.