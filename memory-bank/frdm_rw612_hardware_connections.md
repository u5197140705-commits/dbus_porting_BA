# FRDM-RW612 Board: Hardware Connections for Flashing and Debugging

This document details the physical hardware connections required for flashing and debugging the FRDM-RW612 development board.

## 1. USB Cable Connection

The FRDM-RW612 board features a built-in OpenSDA debug probe, which provides both flashing and a virtual COM port for serial communication over a single USB connection.

*   **Board Side:** Connect a USB Micro-B cable to the **OpenSDA USB port** (often labeled "SDA USB" or similar) on the FRDM-RW612 board.
*   **Host Machine Side:** Connect the other end of the USB cable (USB Type-A) to an available USB port on your host development machine.

This connection will enumerate two devices on your host: a mass storage device (for drag-and-drop flashing) and a virtual COM port (for serial console).

## 2. Debug Probe (Onboard OpenSDA)

The FRDM-RW612 board integrates an OpenSDA debug probe, eliminating the need for an external debugger for most common flashing and debugging tasks.

*   **Connection:** The OpenSDA debug probe is accessed via the **OpenSDA USB port** as described in the "USB Cable Connection" section. No separate debug probe connection is typically required.
*   **Functionality:** The OpenSDA provides a standard SWD (Serial Wire Debug) interface to the target RW612 microcontroller.
*   **External Debug Probes (Optional):** If an external debug probe (e.g., J-Link, MCUXpresso IDE Link) is preferred or required for advanced debugging features, it would typically connect to the **SWD header** (often a 10-pin or 20-pin header) on the FRDM-RW612 board. The specific pinout would be detailed in the board's schematic or user manual. In this case, the external probe would connect to the SWD header on the board and to the host machine via its own USB cable.

## 3. Serial Console Connection

The FRDM-RW612 board's OpenSDA debug probe provides a virtual COM port over the same USB connection used for flashing and debugging. This allows for easy serial communication with the target microcontroller.

*   **Connection:** The serial console is established automatically when the FRDM-RW612 board is connected to the host machine via the **OpenSDA USB port** (as described in section 1). The host operating system will enumerate a virtual COM port (e.g., `COMx` on Windows, `/dev/ttyUSBx` or `/dev/ttyACMx` on Linux/macOS).
*   **UART Pins:** The OpenSDA typically routes the target microcontroller's primary UART (e.g., `LPUART0` or `USART0`) to this virtual COM port. Refer to the FRDM-RW612 schematic for the exact UART pins used.
*   **Baud Rate:** The standard baud rate for Zephyr applications on the FRDM-RW612 is usually **115200 baud**. Other common settings are 8 data bits, no parity, 1 stop bit (8N1).
*   **Terminal Emulator:** To view the serial output, use a terminal emulator program on your host machine (e.g., PuTTY, Tera Term, minicom, screen, VS Code's Serial Monitor extension). Configure the emulator to connect to the enumerated virtual COM port with the correct baud rate and settings.