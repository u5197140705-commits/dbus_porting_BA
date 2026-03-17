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
#define DBUS_RSP_MARKER 0xA0

#define MOTOR_COUNT 4
#define ACTIVE_MOTOR_COUNT 1
#define MOTOR_REG_BASE   0x5000u
#define MOTOR_REG_STRIDE 0x10u
#define MOTOR_REG_ENABLE_OFFSET   0x0u
#define MOTOR_REG_SPEED_OFFSET    0x4u
#define MOTOR_REG_FEEDBACK_OFFSET 0x8u
#define MOTOR_REG_STATUS_OFFSET   0xCu

#define MOTOR_STATUS_ENABLED   0x00000001u
#define MOTOR_STATUS_AVAILABLE 0x00000002u

typedef struct {
    uint16_t addr;
    uint32_t value;
    bool used;
} reg_entry_t;

static reg_entry_t reg_table[32];

typedef struct {
    uint32_t enable;
    int32_t speed_setpoint;
    int32_t speed_feedback;
    uint32_t status;
} motor_channel_t;

static motor_channel_t motors[MOTOR_COUNT];

static uint8_t rx_frame_raw[FRAME_SIZE];
static uint8_t tx_frame_desired[FRAME_SIZE];
static uint8_t tx_frame_wire[FRAME_SIZE];

static size_t rx_index = 0;
static size_t tx_index = 0;

typedef enum {
    TRANSFORM_IDENTITY = 0,
    TRANSFORM_ROL1,
    TRANSFORM_ROR1,
} bit_transform_t;

static bit_transform_t tx_transform = TRANSFORM_ROL1;

static inline uint8_t rol1(uint8_t value)
{
    return (uint8_t)((value << 1) | (value >> 7));
}

static inline uint8_t ror1(uint8_t value)
{
    return (uint8_t)((value >> 1) | (value << 7));
}

static inline uint8_t apply_transform(uint8_t value, bit_transform_t transform)
{
    switch (transform) {
        case TRANSFORM_IDENTITY:
            return value;
        case TRANSFORM_ROL1:
            return rol1(value);
        case TRANSFORM_ROR1:
            return ror1(value);
        default:
            return value;
    }
}

static bool decode_frame_with_transform(const uint8_t *raw_frame, uint8_t *decoded_frame, bit_transform_t transform)
{
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        decoded_frame[i] = apply_transform(raw_frame[i], transform);
    }

    uint8_t cmd = decoded_frame[0] & 0x60u;
    uint8_t len_words = decoded_frame[3];

    if ((cmd == DBUS_CMD_READ || cmd == DBUS_CMD_WRITE) && len_words == 1u) {
        return true;
    }

    return false;
}

static bool decode_rx_frame_auto(const uint8_t *raw_frame, uint8_t *decoded_frame, bit_transform_t *detected)
{
    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_IDENTITY)) {
        *detected = TRANSFORM_IDENTITY;
        return true;
    }

    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_ROL1)) {
        *detected = TRANSFORM_ROL1;
        return true;
    }

    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_ROR1)) {
        *detected = TRANSFORM_ROR1;
        return true;
    }

    return false;
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

static bool motor_reg_decode(uint16_t addr, size_t *motor_index, uint16_t *offset)
{
    if (addr < MOTOR_REG_BASE) {
        return false;
    }

    uint16_t rel = (uint16_t)(addr - MOTOR_REG_BASE);
    size_t index = rel / MOTOR_REG_STRIDE;
    uint16_t off = rel % MOTOR_REG_STRIDE;

    if (index >= MOTOR_COUNT) {
        return false;
    }

    *motor_index = index;
    *offset = off;
    return true;
}

static void motor_write(uint16_t addr, uint32_t value)
{
    size_t index = 0;
    uint16_t offset = 0;
    if (!motor_reg_decode(addr, &index, &offset)) {
        return;
    }

    if (index >= ACTIVE_MOTOR_COUNT) {
        return;
    }

    switch (offset) {
        case MOTOR_REG_ENABLE_OFFSET:
            motors[index].enable = (value != 0u) ? 1u : 0u;
            motors[index].status = MOTOR_STATUS_AVAILABLE |
                                   (motors[index].enable ? MOTOR_STATUS_ENABLED : 0u);
            if (!motors[index].enable) {
                motors[index].speed_feedback = 0;
            }
            break;
        case MOTOR_REG_SPEED_OFFSET:
            motors[index].speed_setpoint = (int32_t)value;
            if (motors[index].enable) {
                motors[index].speed_feedback = motors[index].speed_setpoint;
            } else {
                motors[index].speed_feedback = 0;
            }
            break;
        default:
            break;
    }
}

static uint32_t motor_read(uint16_t addr)
{
    size_t index = 0;
    uint16_t offset = 0;
    if (!motor_reg_decode(addr, &index, &offset)) {
        return 0u;
    }

    switch (offset) {
        case MOTOR_REG_ENABLE_OFFSET:
            return motors[index].enable;
        case MOTOR_REG_SPEED_OFFSET:
            return (uint32_t)motors[index].speed_setpoint;
        case MOTOR_REG_FEEDBACK_OFFSET:
            return (uint32_t)motors[index].speed_feedback;
        case MOTOR_REG_STATUS_OFFSET:
            return motors[index].status;
        default:
            return 0u;
    }
}

static void prepare_tx_frame_wire(void)
{
    /* Header bytes [0..3]: sent without transform so RW612 can validate
     * marker + address + len directly after the link's ROR1. Header bytes
     * are pre-compensated individually: ROL1(plain) so link ROR1 gives plain. */
    for (size_t i = 0; i < 4u; i++) {
        tx_frame_wire[i] = rol1(tx_frame_desired[i]);
    }
    /* Data bytes [4..7]: same ROL1 pre-compensation for payload. */
    for (size_t i = 4u; i < FRAME_SIZE; i++) {
        tx_frame_wire[i] = apply_transform(tx_frame_desired[i], tx_transform);
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
    bit_transform_t detected = TRANSFORM_ROL1;

    if (!decode_rx_frame_auto(rx_frame_raw, decoded, &detected)) {
        /* Ignore invalid frames (e.g. RW612 dummy clocks during readback).
         * Do NOT overwrite tx_frame_wire here, otherwise a prepared response
         * can be clobbered by default 0xA5 before the master receives it. */
        return;
    }

    /* tx_transform is always TRANSFORM_ROL1: the link applies ROR1 to MISO
     * (CPHA mismatch), so Pico must pre-compensate with ROL1 regardless of
     * which decode transform was detected for the incoming MOSI bytes. */
    (void)detected;

    uint8_t cmd = decoded[0] & 0x60u;
    uint16_t addr = ((uint16_t)decoded[1] << 8) | decoded[2];

    if (cmd == DBUS_CMD_WRITE) {
        uint32_t value =
            (uint32_t)decoded[4] |
            ((uint32_t)decoded[5] << 8) |
            ((uint32_t)decoded[6] << 16) |
            ((uint32_t)decoded[7] << 24);

        motor_write(addr, value);
        reg_write(addr, value);
        set_default_tx_pattern();
        return;
    }

    if (cmd == DBUS_CMD_READ) {
        uint32_t value = reg_read(addr);
        if (addr >= MOTOR_REG_BASE && addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
            value = motor_read(addr);
        }
        memset(tx_frame_desired, 0, sizeof(tx_frame_desired));
        tx_frame_desired[0] = DBUS_RSP_MARKER;
        tx_frame_desired[1] = (uint8_t)(addr >> 8);
        tx_frame_desired[2] = (uint8_t)(addr & 0xFFu);
        tx_frame_desired[3] = 0x01u;
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

static void service_spi_frame(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);

    while (spi_is_readable(spi)) {
        uint8_t rx_byte = (uint8_t)hw->dr;
        if (rx_index < FRAME_SIZE) {
            rx_frame_raw[rx_index++] = rx_byte;
        }

        if (rx_index >= FRAME_SIZE) {
            process_rx_frame();
            rx_index = 0;
            tx_index = 0;
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
    memset(motors, 0, sizeof(motors));
    for (size_t i = 0; i < MOTOR_COUNT; i++) {
        motors[i].status = (i < ACTIVE_MOTOR_COUNT) ? MOTOR_STATUS_AVAILABLE : 0u;
    }
    memset(rx_frame_raw, 0, sizeof(rx_frame_raw));
    set_default_tx_pattern();

    spi_init(spi0, 1000 * 1000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_set_slave(spi0, true);

    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    /* Brief startup indicator only — keep delay minimal so Pico is ready
     * before the SPI master (RW612) begins its first exchange. */
    status_led_set(true);
    sleep_ms(50);
    status_led_set(false);

    while (true) {
        service_spi_frame(spi0);
        status_led_set(cs_is_active());

        tight_loop_contents();
    }
}
