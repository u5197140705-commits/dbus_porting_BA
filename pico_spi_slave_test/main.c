#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/structs/spi.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define PICO_FIRMWARE_VERSION "cs_aligned_tx_v2_noblk_motorpins"

/* Non-blocking deferred log buffer: process_rx_frame must never call printf
 * directly — USB CDC printf blocks for milliseconds, which stalls the SPI
 * tight loop and causes TX FIFO underflow. Store messages here instead;
 * main loop flushes them between SPI calls. */
#define DLOG_ENTRIES 64u
#define DLOG_MSG_LEN 96u
typedef struct { char msg[DLOG_MSG_LEN]; } dlog_entry_t;
static dlog_entry_t dlog_buf[DLOG_ENTRIES];
static unsigned int dlog_head = 0u;
static unsigned int dlog_tail = 0u;

static void dlog(const char *fmt, ...) {
    unsigned int next = (dlog_head + 1u) % DLOG_ENTRIES;
    if (next == dlog_tail) return;  /* buffer full, drop entry */
    va_list args;
    va_start(args, fmt);
    vsnprintf(dlog_buf[dlog_head].msg, DLOG_MSG_LEN - 1u, fmt, args);
    dlog_buf[dlog_head].msg[DLOG_MSG_LEN - 1u] = '\0';
    va_end(args);
    dlog_head = next;
}

static void dlog_flush(void) {
    while (dlog_tail != dlog_head) {
        printf("%s", dlog_buf[dlog_tail].msg);
        dlog_tail = (dlog_tail + 1u) % DLOG_ENTRIES;
    }
}

#define PIN_MISO 19
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 16

#define PIN_MOTOR_PWMA 15
#define PIN_MOTOR_STBY 14
#define PIN_MOTOR_AIN1 13
#define PIN_MOTOR_AIN2 12

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
static bool last_cs_state = false; // cs_is_active(): false=idle(high), true=active(low)

typedef enum {
    TRANSFORM_IDENTITY = 0,
    TRANSFORM_ROL1,
    TRANSFORM_ROR1,
    TRANSFORM_SERIAL_ROL1,
    TRANSFORM_SERIAL_ROR1,
} bit_transform_t;

static bit_transform_t tx_transform = TRANSFORM_ROL1;

static uint motor_pwma_slice = 0u;
static uint motor_pwma_channel = 0u;
static const char *const motor_names[MOTOR_COUNT] = {
    "motor1",
    "motor2",
    "motor3",
    "motor4",
};

static uint16_t speed_to_pwm_level(int32_t speed_setpoint)
{
    int32_t magnitude = speed_setpoint;
    if (magnitude < 0) {
        magnitude = -magnitude;
    }
    if (magnitude > 1200) {
        magnitude = 1200;
    }

    return (uint16_t)((magnitude * 65535) / 1200);
}

static void apply_motor_outputs(size_t motor_index)
{
    if (motor_index >= ACTIVE_MOTOR_COUNT) {
        return;
    }

    bool enabled = (motors[motor_index].enable != 0u);
    int32_t speed = motors[motor_index].speed_setpoint;
    bool forward = speed >= 0;

    gpio_put(PIN_MOTOR_STBY, enabled ? 1 : 0);

    if (!enabled || speed == 0) {
        gpio_put(PIN_MOTOR_AIN1, 0);
        gpio_put(PIN_MOTOR_AIN2, 0);
        pwm_set_chan_level(motor_pwma_slice, motor_pwma_channel, 0u);
        return;
    }

    gpio_put(PIN_MOTOR_AIN1, forward ? 1 : 0);
    gpio_put(PIN_MOTOR_AIN2, forward ? 0 : 1);
    pwm_set_chan_level(motor_pwma_slice, motor_pwma_channel, speed_to_pwm_level(speed));
}

static const char *motor_name_for_index(size_t motor_index)
{
    if (motor_index < MOTOR_COUNT) {
        return motor_names[motor_index];
    }

    return "motor?";
}

static void motor_gpio_init(void)
{
    gpio_init(PIN_MOTOR_STBY);
    gpio_set_dir(PIN_MOTOR_STBY, GPIO_OUT);
    gpio_put(PIN_MOTOR_STBY, 0);

    gpio_init(PIN_MOTOR_AIN1);
    gpio_set_dir(PIN_MOTOR_AIN1, GPIO_OUT);
    gpio_put(PIN_MOTOR_AIN1, 0);

    gpio_init(PIN_MOTOR_AIN2);
    gpio_set_dir(PIN_MOTOR_AIN2, GPIO_OUT);
    gpio_put(PIN_MOTOR_AIN2, 0);

    gpio_set_function(PIN_MOTOR_PWMA, GPIO_FUNC_PWM);
    motor_pwma_slice = pwm_gpio_to_slice_num(PIN_MOTOR_PWMA);
    motor_pwma_channel = pwm_gpio_to_channel(PIN_MOTOR_PWMA);
    pwm_set_wrap(motor_pwma_slice, 65535u);
    pwm_set_chan_level(motor_pwma_slice, motor_pwma_channel, 0u);
    pwm_set_enabled(motor_pwma_slice, true);
}

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

static bool addr_is_valid(uint16_t addr)
{
    if (addr == 0x001Cu) {
        return true;
    }

    if (addr >= MOTOR_REG_BASE &&
        addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
        return true;
    }

    return false;
}

static bool decoded_frame_is_valid(const uint8_t *decoded_frame)
{
    uint8_t cmd = decoded_frame[0];
    uint8_t len_words = decoded_frame[3];
    uint16_t addr = ((uint16_t)decoded_frame[1] << 8) | decoded_frame[2];

    if ((cmd != DBUS_CMD_READ && cmd != DBUS_CMD_WRITE) || len_words != 1u) {
        return false;
    }

    return addr_is_valid(addr);
}

static bool decode_frame_with_transform(const uint8_t *raw_frame, uint8_t *decoded_frame, bit_transform_t transform)
{
    if (transform == TRANSFORM_SERIAL_ROR1) {
        decoded_frame[0] = (uint8_t)(raw_frame[0] >> 1);
        for (size_t i = 1; i < FRAME_SIZE; i++) {
            decoded_frame[i] = (uint8_t)((raw_frame[i] >> 1) | ((raw_frame[i - 1] & 0x01u) << 7));
        }
    } else if (transform == TRANSFORM_SERIAL_ROL1) {
        for (size_t i = 0; i < (FRAME_SIZE - 1); i++) {
            decoded_frame[i] = (uint8_t)((raw_frame[i] << 1) | (raw_frame[i + 1] >> 7));
        }
        decoded_frame[FRAME_SIZE - 1] = (uint8_t)(raw_frame[FRAME_SIZE - 1] << 1);
    } else {
        for (size_t i = 0; i < FRAME_SIZE; i++) {
            decoded_frame[i] = apply_transform(raw_frame[i], transform);
        }
    }

    return decoded_frame_is_valid(decoded_frame);
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

    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_SERIAL_ROR1)) {
        *detected = TRANSFORM_SERIAL_ROR1;
        return true;
    }

    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_SERIAL_ROL1)) {
        *detected = TRANSFORM_SERIAL_ROL1;
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
            apply_motor_outputs(index);
            dlog("[PICO] %s enable=%u\n", motor_name_for_index(index), (unsigned)motors[index].enable);
            break;
        case MOTOR_REG_SPEED_OFFSET:
            motors[index].speed_setpoint = (int32_t)value;
            if (motors[index].enable) {
                motors[index].speed_feedback = motors[index].speed_setpoint;
            } else {
                motors[index].speed_feedback = 0;
            }
            apply_motor_outputs(index);
            dlog("[PICO] %s speed=%ld\n", motor_name_for_index(index), (long)motors[index].speed_setpoint);
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
    /* The link applies SERIAL_ROR1 (1-bit right-shift of the entire MISO bit
     * stream, carry propagates from LSB of each byte to MSB of the next).
     * Pre-compensate with SERIAL_ROL1 (1-bit left-shift, carry from MSB of
     * each byte into LSB of the previous byte) so that after the link's
     * SERIAL_ROR1 the RW612 receives the intended TX frame exactly.
     *
     * W[i] = (desired[i] << 1) | (desired[i+1] >> 7)  for i < FRAME_SIZE-1
     * W[FRAME_SIZE-1] = desired[FRAME_SIZE-1] << 1
     *
     * This is subtle: per-byte ROL1 is correct for bytes where the LSB from
     * the carry chain happens to match bit7 of the same byte, but fails when
     * adjacent bytes have differing MSBs (e.g. 0xb0 followed by 0x04, or the
     * len=0x01 byte preceding 0xaa data).
     */
    for (size_t i = 0u; i < (FRAME_SIZE - 1u); i++) {
        tx_frame_wire[i] = (uint8_t)((tx_frame_desired[i] << 1u) | (tx_frame_desired[i + 1u] >> 7u));
    }
    tx_frame_wire[FRAME_SIZE - 1u] = (uint8_t)(tx_frame_desired[FRAME_SIZE - 1u] << 1u);
}

static void set_default_tx_pattern(void)
{
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        tx_frame_desired[i] = 0xA5;
    }
    prepare_tx_frame_wire();
}

static bool process_rx_frame(void)
{
    uint8_t decoded[FRAME_SIZE];
    bit_transform_t detected = TRANSFORM_ROL1;

    if (!decode_rx_frame_auto(rx_frame_raw, decoded, &detected)) {
        /* Ignore invalid frames (e.g. RW612 dummy clocks during readback).
         * Do NOT overwrite tx_frame_wire here, otherwise a prepared response
         * can be clobbered by default 0xA5 before the master receives it. */
           dlog("[PICO] FAILED raw: %02x %02x %02x %02x %02x %02x %02x %02x\n",
               rx_frame_raw[0], rx_frame_raw[1], rx_frame_raw[2], rx_frame_raw[3],
               rx_frame_raw[4], rx_frame_raw[5], rx_frame_raw[6], rx_frame_raw[7]);
        return false;
    }

    /* tx_transform is always TRANSFORM_ROL1: the link applies ROR1 to MISO
     * (CPHA mismatch), so Pico must pre-compensate with ROL1 regardless of
     * which decode transform was detected for the incoming MOSI bytes. */
    (void)detected;

    uint8_t cmd = decoded[0] & 0x60u;
    uint16_t addr = ((uint16_t)decoded[1] << 8) | decoded[2];
    dlog("[PICO] RX cmd=0x%02x addr=0x%04x\n", cmd, addr);

    if (cmd == DBUS_CMD_WRITE) {
        uint32_t value_decoded =
            (uint32_t)decoded[4] |
            ((uint32_t)decoded[5] << 8) |
            ((uint32_t)decoded[6] << 16) |
            ((uint32_t)decoded[7] << 24);

        uint32_t value_raw =
            (uint32_t)rx_frame_raw[4] |
            ((uint32_t)rx_frame_raw[5] << 8) |
            ((uint32_t)rx_frame_raw[6] << 16) |
            ((uint32_t)rx_frame_raw[7] << 24);

        uint32_t value = value_decoded;
        if (value_decoded == 0u && value_raw != 0u) {
            value = value_raw;
        }

        motor_write(addr, value);
        reg_write(addr, value);
        dlog("[PICO] WRITE addr=0x%04x val=0x%08x\n", addr, value);
        set_default_tx_pattern();
        tx_index = 0;
        return true;
    }

    if (cmd == DBUS_CMD_READ) {
        uint32_t value = reg_read(addr);
        if (addr >= MOTOR_REG_BASE && addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
            value = motor_read(addr);
        }
        dlog("[PICO] READ addr=0x%04x val=0x%08x\n", addr, value);
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
        tx_index = 0;
        /* Preload TX FIFO immediately — do not wait for the service_spi_frame
         * TX loading loop which may run after CS goes high and miss the window.
         * At this point all 8 command bytes have been received so the FIFO is
         * empty and can accept the full 8-byte response now. */
        {
            spi_hw_t *hw = spi_get_hw(spi0);
            while (spi_is_writable(spi0) && tx_index < FRAME_SIZE) {
                hw->dr = tx_frame_wire[tx_index++];
            }
        }
        return true;
    }

    set_default_tx_pattern();
    tx_index = 0;
    return false;
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
    bool current_cs_state = cs_is_active();

    if (!last_cs_state && current_cs_state) {
        /* New transaction started: align both RX and TX to frame start. */
        rx_index = 0;
        tx_index = 0;
        /* Prime TX immediately at CS-assert so first clocks do not see zeros. */
        while (cs_is_active() && spi_is_writable(spi) && tx_index < FRAME_SIZE) {
            hw->dr = tx_frame_wire[tx_index++];
        }
    }

    if (last_cs_state && !current_cs_state) {
        /* Transaction ended: discard partial frame residue, if any. */
        rx_index = 0;
    }

    last_cs_state = current_cs_state;

    while (spi_is_readable(spi)) {
        uint8_t rx_byte = (uint8_t)hw->dr;

        if (rx_index < FRAME_SIZE) {
            rx_frame_raw[rx_index++] = rx_byte;
        }

        /* Keep feeding TX during active transaction, not only after RX drains. */
        while (cs_is_active() && spi_is_writable(spi) && tx_index < FRAME_SIZE) {
            hw->dr = tx_frame_wire[tx_index++];
        }

        if (rx_index == FRAME_SIZE) {
            (void)process_rx_frame();
            rx_index = 0;
        }
    }

    /* Only feed TX FIFO while CS is active; otherwise tx_index may advance
     * during idle and de-synchronize the next transaction response. */
    while (cs_is_active() && spi_is_writable(spi) && tx_index < FRAME_SIZE) {
        hw->dr = tx_frame_wire[tx_index];
        tx_index++;
    }
}

int main(void)
{
    stdio_init_all();
    printf("[Pico SPI Slave] Firmware version: %s\n", PICO_FIRMWARE_VERSION);
        printf("[Pico SPI Slave] SPI0 pins: MOSI=GP%u CSn=GP%u SCK=GP%u MISO=GP%u\n",
            PIN_MOSI, PIN_CS, PIN_SCK, PIN_MISO);
        printf("[Pico SPI Slave] Motor1 pins: PWMA=GP%u STBY=GP%u AIN1=GP%u AIN2=GP%u\n",
            PIN_MOTOR_PWMA, PIN_MOTOR_STBY, PIN_MOTOR_AIN1, PIN_MOTOR_AIN2);
    uint32_t last_heartbeat_ms = 0;
    
    status_led_init();

    memset(reg_table, 0, sizeof(reg_table));
    memset(motors, 0, sizeof(motors));
    motor_gpio_init();
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
        dlog_flush();
        status_led_set(cs_is_active());

        uint32_t now_ms = to_ms_since_boot(get_absolute_time());
        if ((now_ms - last_heartbeat_ms) >= 10000u) {
             printf("[Pico SPI Slave] alive version=%s cs=%u rx_index=%u tx_index=%u %s_en=%u %s_spd=%ld\n",
                   PICO_FIRMWARE_VERSION,
                   cs_is_active() ? 1u : 0u,
                   (unsigned)rx_index,
                 (unsigned)tx_index,
                 motor_name_for_index(0u),
                 (unsigned)motors[0].enable,
                 motor_name_for_index(0u),
                 (long)motors[0].speed_setpoint);
            last_heartbeat_ms = now_ms;
        }

        tight_loop_contents();
    }
}
