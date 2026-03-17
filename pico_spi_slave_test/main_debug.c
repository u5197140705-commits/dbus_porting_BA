#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"

#define PIN_MISO 19
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 16

#define REG_COUNT 16

typedef struct {
    uint16_t addr;
    uint32_t value;
} reg_entry_t;

static reg_entry_t registers[REG_COUNT];
static uint8_t spi_rx_buffer[256];
static uint8_t spi_tx_buffer[256];
static int frame_count = 0;

static uint32_t reg_read(uint16_t addr) {
    for (size_t index = 0; index < REG_COUNT; index++) {
        if (registers[index].addr == addr) {
            return registers[index].value;
        }
    }
    return 0;
}

static void reg_write(uint16_t addr, uint32_t value) {
    printf("[REG_WRITE] addr=0x%04x, value=0x%08x\n", addr, value);
    for (size_t index = 0; index < REG_COUNT; index++) {
        if (registers[index].addr == addr) {
            registers[index].value = value;
            return;
        }
    }
    // Create new register if not found
    for (size_t index = 0; index < REG_COUNT; index++) {
        if (registers[index].addr == 0 && index > 0) {
            registers[index].addr = addr;
            registers[index].value = value;
            return;
        }
    }
}

static void cs_callback(uint gpio, uint32_t events) {
    // CS fell (assert)
    if (events & GPIO_IRQ_EDGE_FALL) {
        printf("[CS_ASSERT] Frame #%d starting\n", frame_count++);
        memset(spi_rx_buffer, 0, sizeof(spi_rx_buffer));
        memset(spi_tx_buffer, 0, sizeof(spi_tx_buffer));
    }
    // CS rose (deassert)
    else if (events & GPIO_IRQ_EDGE_RISE) {
        printf("[CS_DEASSERT] Frame complete\n");
        // Log what was received
        printf("RX: ");
        for (int i = 0; i < 8; i++) {
            printf("%02x ", spi_rx_buffer[i]);
        }
        printf("\n");
        
        // Parse frame
        if (spi_rx_buffer[0] != 0) {
            uint8_t cmd = spi_rx_buffer[0];
            uint16_t addr = ((uint16_t)spi_rx_buffer[1] << 8) | spi_rx_buffer[2];
            uint8_t len_words = spi_rx_buffer[3];
            
            printf("CMD=0x%02x, ADDR=0x%04x, LEN=%u words\n", cmd, addr, len_words);
            
            bool is_write = (cmd & 0x60U) == 0x60U;
            bool is_read = (cmd & 0x60U) == 0x40U;
            
            if (is_write) {
                uint32_t value = ((uint32_t)spi_rx_buffer[4]) |
                                ((uint32_t)spi_rx_buffer[5] << 8) |
                                ((uint32_t)spi_rx_buffer[6] << 16) |
                                ((uint32_t)spi_rx_buffer[7] << 24);
                printf("  -> WRITE operation, value=0x%08x\n", value);
                reg_write(addr, value);
            } else if (is_read) {
                uint32_t value = reg_read(addr);
                printf("  -> READ operation, returning 0x%08x\n", value);
                // Prepare TX response
                spi_tx_buffer[0] = 0x00;
                spi_tx_buffer[1] = 0x00;
                spi_tx_buffer[2] = 0x00;
                spi_tx_buffer[3] = 0x00;
                spi_tx_buffer[4] = (uint8_t)(value);
                spi_tx_buffer[5] = (uint8_t)(value >> 8);
                spi_tx_buffer[6] = (uint8_t)(value >> 16);
                spi_tx_buffer[7] = (uint8_t)(value >> 24);
            }
        }
    }
}

int main() {
    stdio_init_all();
    sleep_ms(500);
    printf("\n=== PICO SPI SLAVE DEBUG MODE ===\n");
    
    // Initialize registers
    registers[0] = {0x0000, 0x00000000};
    registers[1] = {0x0004, 0x00000000};
    registers[2] = {0x000C, 0x00000000};
    registers[3] = {0x0010, 0x00000000};
    registers[4] = {0x0014, 0x00000000};
    registers[5] = {0x001C, 0x00000000};
    registers[6] = {0x4018, 0x00000000};
    
    // Initialize SPI slave
    spi_init(spi0, 1000000); // 1 MHz for now
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    
    // CS as input with interrupt
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_IN);
    gpio_set_pulls(PIN_CS, true, false); // Pull-up
    
    // Set up CS interrupt
    gpio_set_irq_enabled_with_callback(PIN_CS, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &cs_callback);
    
    printf("SPI Slave initialized\n");
    printf("Waiting for CS transitions...\n");
    
    // Main loop: perform SPI transfers when CS is low
    while (1) {
        if (!gpio_get(PIN_CS)) { // CS is low (active)
            // Do a non-blocking SPI read/write
            spi_read_blocking(spi0, 0x00, spi_rx_buffer, 8);
            spi_write_blocking(spi0, spi_tx_buffer, 8);
        }
        sleep_ms(1);
    }
    
    return 0;
}
