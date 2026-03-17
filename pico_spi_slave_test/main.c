#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/structs/spi.h"
#include <string.h>

#define PIN_MISO 19
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 16

#define FRAME_SIZE 8
#define DBUS_CMD_READ  0x40
#define DBUS_CMD_WRITE 0x60

typedef struct {
    uint16_t addr;
    uint32_t value;
    bool used;
} reg_entry_t;

static reg_entry_t reg_table[32];

static uint8_t rx_frame_raw[FRAME_SIZE];
static uint8_t tx_frame_desired[FRAME_SIZE];
static uint8_t tx_frame_wire[FRAME_SIZE];

static bool cs_prev_high = true;
static size_t rx_index = 0;
static size_t tx_index = 0;

static inline uint8_t rol1(uint8_t value)
{
    return (uint8_t)((value << 1) | (value >> 7));
}

static inline uint8_t decode_rx_byte(uint8_t wire_value)
{
    return rol1(wire_value);
}

static inline uint8_t encode_tx_byte(uint8_t desired_value)
{
    return rol1(desired_value);
}

static uint32_t reg_read(uint16_t addr)
{
    for (size_t i = 0; i < (sizeof(reg_table) / sizeof(reg_table[0])); i++) {
        if (reg_table[i].used && reg_table[i].addr == addr) {
            return reg_table[i].value;
        }
    }

    return 0;
}

static void reg_write(uint16_t addr, uint32_t value)
{
    for (size_t i = 0; i < (sizeof(reg_table) / sizeof(reg_table[0])); i++) {
        if (reg_table[i].used && reg_table[i].addr == addr) {
            reg_table[i].value = value;
            return;
        }
    }

    for (size_t i = 0; i < (sizeof(reg_table) / sizeof(reg_table[0])); i++) {
        if (!reg_table[i].used) {
            reg_table[i].used = true;
            reg_table[i].addr = addr;
            reg_table[i].value = value;
            return;
        }
    }
}

static void prepare_tx_frame_wire(void)
{
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        tx_frame_wire[i] = encode_tx_byte(tx_frame_desired[i]);
    }
}

static void set_default_tx_pattern(void)
{
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        tx_frame_desired[i] = 0xA5;
    }
    prepare_tx_frame_wire();
}

static void process_rx_frame(void)
{
    uint8_t decoded[FRAME_SIZE];
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        decoded[i] = decode_rx_byte(rx_frame_raw[i]);
    }

    uint8_t cmd = decoded[0] & 0x60u;
    uint16_t addr = ((uint16_t)decoded[1] << 8) | decoded[2];

    if (cmd == DBUS_CMD_WRITE) {
        uint32_t value =
            (uint32_t)decoded[4] |
            ((uint32_t)decoded[5] << 8) |
            ((uint32_t)decoded[6] << 16) |
            ((uint32_t)decoded[7] << 24);

        reg_write(addr, value);
        set_default_tx_pattern();
        return;
    }

    if (cmd == DBUS_CMD_READ) {
        uint32_t value = reg_read(addr);
        memset(tx_frame_desired, 0, sizeof(tx_frame_desired));
        tx_frame_desired[4] = (uint8_t)(value);
        tx_frame_desired[5] = (uint8_t)(value >> 8);
        tx_frame_desired[6] = (uint8_t)(value >> 16);
        tx_frame_desired[7] = (uint8_t)(value >> 24);
        prepare_tx_frame_wire();
        return;
    }

    set_default_tx_pattern();
}

static inline void status_led_init(void)
{
#ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
#endif
}

static inline void status_led_set(bool on)
{
#ifdef PICO_DEFAULT_LED_PIN
    gpio_put(PICO_DEFAULT_LED_PIN, on ? 1 : 0);
#else
    (void)on;
#endif
}

static inline void spi_slave_fill_tx_fifo(spi_inst_t *spi, uint8_t value)
{
    spi_hw_t *hw = spi_get_hw(spi);

    while (spi_is_writable(spi)) {
        hw->dr = value;
    }
}

static inline bool spi_slave_drain_rx_fifo(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);
    bool received = false;

    while (spi_is_readable(spi)) {
        (void)hw->dr;
        received = true;
    }

    return received;
}

static inline bool cs_is_active(void)
{
    return !gpio_get(PIN_CS);
}

static void on_cs_assert(void)
{
    rx_index = 0;
    tx_index = 0;
}

static void on_cs_deassert(void)
{
    if (rx_index >= FRAME_SIZE) {
        process_rx_frame();
    } else {
        set_default_tx_pattern();
    }
}

static void service_spi_frame(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);

    while (spi_is_readable(spi)) {
        uint8_t rx_byte = (uint8_t)hw->dr;
        if (rx_index < FRAME_SIZE) {
            rx_frame_raw[rx_index++] = rx_byte;
        }
    }

    while (spi_is_writable(spi)) {
        uint8_t tx_byte = tx_frame_wire[tx_index % FRAME_SIZE];
        hw->dr = tx_byte;
        tx_index++;
    }
}

int main(void)
{
    status_led_init();

    memset(reg_table, 0, sizeof(reg_table));
    memset(rx_frame_raw, 0, sizeof(rx_frame_raw));
    set_default_tx_pattern();

    spi_init(spi0, 1000 * 1000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_set_slave(spi0, true);

    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    for (int i = 0; i < 3; i++) {
        status_led_set(true);
        sleep_ms(100);
        status_led_set(false);
        sleep_ms(100);
    }

    while (true) {
        bool cs_active = cs_is_active();

        if (cs_active && cs_prev_high) {
            on_cs_assert();
        }
        if (!cs_active && !cs_prev_high) {
            on_cs_deassert();
        }

        cs_prev_high = !cs_active;

        if (cs_active) {
            service_spi_frame(spi0);
            status_led_set(true);
        } else {
            spi_slave_fill_tx_fifo(spi0, tx_frame_wire[0]);
            (void)spi_slave_drain_rx_fifo(spi0);
            status_led_set(false);
        }

        tight_loop_contents();
    }
}
