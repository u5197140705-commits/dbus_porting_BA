#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define PIN_MISO 19
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 16

#define FRAME_LEN 8

static const uint8_t debug_response[FRAME_LEN] = {
    0x00, 0x00, 0x00, 0x00,
    0x11, 0x22, 0x33, 0x44
};

int main(void)
{
    stdio_init_all();
    sleep_ms(1200);
    printf("PICO SPI SLAVE DEBUG START\n");

    spi_init(spi0, 1000 * 1000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    spi_set_slave(spi0, true);

    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    uint8_t rx_frame[FRAME_LEN] = {0};
    uint8_t tx_frame[FRAME_LEN] = {0};
    uint8_t frame_index = 0;
    bool frame_preloaded = false;

    while (true) {
        if (gpio_get(PIN_CS)) {
            frame_index = 0;
            frame_preloaded = false;
            tight_loop_contents();
            continue;
        }

        if (!frame_preloaded) {
            while (!spi_is_writable(spi0)) {
            }
            spi_get_hw(spi0)->dr = debug_response[0];
            frame_preloaded = true;
        }

        if (!spi_is_readable(spi0)) {
            tight_loop_contents();
            continue;
        }

        uint8_t rx = (uint8_t)spi_get_hw(spi0)->dr;
        rx_frame[frame_index] = rx;

        uint8_t next_index = (uint8_t)((frame_index + 1U) % FRAME_LEN);
        uint8_t tx = debug_response[next_index];
        tx_frame[frame_index] = tx;

        while (!spi_is_writable(spi0)) {
        }
        spi_get_hw(spi0)->dr = tx;

        frame_index++;
        if (frame_index >= FRAME_LEN) {
            printf("RX: %02X %02X %02X %02X %02X %02X %02X %02X | TX: %02X %02X %02X %02X %02X %02X %02X %02X\n",
                   rx_frame[0], rx_frame[1], rx_frame[2], rx_frame[3],
                   rx_frame[4], rx_frame[5], rx_frame[6], rx_frame[7],
                   tx_frame[0], tx_frame[1], tx_frame[2], tx_frame[3],
                   tx_frame[4], tx_frame[5], tx_frame[6], tx_frame[7]);
            frame_index = 0;
            frame_preloaded = false;
        }
    }
}
