# NXP RW612 Flexcomm SPI Reference

## SPI Mode (CPOL/CPHA)
The NXP RW612's Flexcomm peripheral, when configured in SPI mode, supports all four standard SPI modes (0-3). This means both Clock Polarity (CPOL) and Clock Phase (CPHA) are selectable.

*   **CPOL (Clock Polarity)**:
    *   `0`: SCK (Serial Clock) is idle low.
    *   `1`: SCK is idle high.
*   **CPHA (Clock Phase)**:
    *   `0`: Data is captured on the **first edge** of the clock (when the clock changes away from its idle state).
    *   `1`: Data is captured on the **second edge** of the clock (following the idle transition).

There is no fixed default mode; CPOL/CPHA must be configured in the CFG register or device tree according to the requirements of the connected slave device.

## Baud Rate (Clock Frequency)
The SPI clock frequency (`f_SPI`) is derived from the Flexcomm functional clock (FCLK) divided by a value from the DIV register.

The formula for the baud rate is:
`f_SPI = FCLK / (DIVVAL + 1)`

*   **DIVVAL**: This value is "minus-1 encoded," meaning:
    *   `0` corresponds to FCLK/1
    *   `1` corresponds to FCLK/2
    *   ... up to `0xFFFF` which corresponds to FCLK/65536.

There is no fixed baud rate. It is entirely determined by the chosen divider and the FCLK source (e.g., SFRO, FFRO, PLL). On typical RW61x EVKs, FCLK for Flexcomm is commonly set to 48 MHz or 60 MHz. With `DIVVAL=0` (i.e., DIV=1), the maximum SPI clock can be 24 MHz or 30 MHz, respectively.

## Device Tree Configuration Summary for Flexcomm1 as SPI
To configure Flexcomm1 as an SPI master in the device tree overlay:
*   Set `spi-cpol` and/or `spi-cpha` properties based on the slave device's datasheet.
*   Set `spi-max-frequency` (or `clock-frequency`) to the desired baud rate, considering the FCLK and available dividers. A common safe default is 8 MHz with Mode 0.

This information is based on the RW610/RW612 reference manual.