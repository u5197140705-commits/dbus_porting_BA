#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/structs/spi.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define PICO_FIRMWARE_VERSION "dbal_motor_v1_onehot_v3_isoD_v22_2026-05-22"

/* Non-blocking deferred log buffer: process_rx_frame must never call printf
 * directly — USB CDC printf blocks for milliseconds, which stalls the SPI
 * tight loop and causes TX FIFO underflow. Store messages here instead;
 * main loop flushes them between SPI calls. */
#define DLOG_ENTRIES 128u
#define DLOG_MSG_LEN 96u
#define LOG_IDLE_FLUSH_US 5000u
#define DLOG_FLUSH_BUDGET 4u
#define PICO_RUNTIME_LOG_FLUSH 0
#define PICO_HEARTBEAT_ENABLE 1
#define PICO_STARTUP_SELF_TEST 0
#define PICO_DIAG_FORCE_MOTOR0_ONLY 0
/* Build this firmware separately for each Pico side:
 * - PICO_NODE_SLOT=1 => owns logical motors 0 (A) and 2 (B)
 * - PICO_NODE_SLOT=2 => owns logical motors 1 (A) and 3 (B)
 */
#ifndef PICO_NODE_SLOT
#define PICO_NODE_SLOT 1
#endif
#define CS_END_DRAIN_TIMEOUT_US 200u
#define CS_END_DRAIN_IDLE_US 1u
#define RX_ACCUM_GAP_RESET_US 3000u
typedef struct { char msg[DLOG_MSG_LEN]; } dlog_entry_t;
static dlog_entry_t dlog_buf[DLOG_ENTRIES];
static unsigned int dlog_head = 0u;
static unsigned int dlog_tail = 0u;
static uint32_t last_spi_activity_us = 0u;
static char usb_cmd_buf[16];
static size_t usb_cmd_len = 0u;

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

static void dlog_flush_limited(unsigned int budget) {
#if PICO_RUNTIME_LOG_FLUSH
    while (dlog_tail != dlog_head && budget > 0u) {
        printf("%s", dlog_buf[dlog_tail].msg);
        dlog_tail = (dlog_tail + 1u) % DLOG_ENTRIES;
        budget--;
    }
#else
    (void)budget;
#endif
}

static void usb_console_poll(void)
{
    for (;;) {
        int ch = getchar_timeout_us(0);

        if (ch == PICO_ERROR_TIMEOUT) {
            return;
        }

        if (ch == '\r' || ch == '\n') {
            usb_cmd_buf[usb_cmd_len] = '\0';

            if (usb_cmd_len > 0u) {
                if (strcmp(usb_cmd_buf, "BOOTSEL") == 0 ||
                    strcmp(usb_cmd_buf, "bootsel") == 0 ||
                    strcmp(usb_cmd_buf, "B") == 0 ||
                    strcmp(usb_cmd_buf, "b") == 0) {
                    printf("[Pico SPI Slave] USB command '%s' -> rebooting to BOOTSEL\n",
                           usb_cmd_buf);
                    sleep_ms(50);
                    reset_usb_boot(0u, 0u);
                }
                usb_cmd_len = 0u;
            }

            continue;
        }

        if (usb_cmd_len < (sizeof(usb_cmd_buf) - 1u)) {
            usb_cmd_buf[usb_cmd_len++] = (char)ch;
        } else {
            usb_cmd_len = 0u;
        }
    }
}

static uint32_t reg_read(uint16_t addr);
static void reg_write(uint16_t addr, uint32_t value);
static void led_update_from_distance(uint32_t dist_mm);

#define PIN_MISO 19
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 16

#define PIN_MOTOR_PWMA 15
#define PIN_MOTOR_STBY 14
#define PIN_MOTOR_AIN1 13
#define PIN_MOTOR_AIN2 12

#define PIN_SONIC_TRIG 20
#define PIN_SONIC_ECHO 21

#define PIN_LCD_RS 0
#define PIN_LCD_E  1
#define PIN_LCD_D4 2
#define PIN_LCD_D5 3
#define PIN_LCD_D6 4
#define PIN_LCD_D7 5

/* Warning LED system: distance-based traffic light */
#define PIN_LED_GREEN  6
#define PIN_LED_YELLOW 7
#define PIN_LED_RED    8
#define LED_DIST_GREEN_MM  500u   /* > 500 mm  -> green */
#define LED_DIST_YELLOW_MM 200u   /* 200-500 mm -> yellow */
                                  /* < 200 mm  -> red */

/* Motor 2 – TB6612FNG channel B (STBY shared with channel A on GP14) */
#define PIN_MOTOR_BIN1 9
#define PIN_MOTOR_BIN2 10
#define PIN_MOTOR_PWMB 11

#define FRAME_SIZE 8
#define DBAL_MAX_FRAME_SIZE  32u  /* max DBAL frame: SOF+len+CRC+headers+payload */
#define PICO_MINIMAL_BLOCKING_RX 0
#define FW_ID_MAIN "MAIN_HWSSEL_V22_2026-05-22"
#define DBUS_CMD_READ  0x40
#define DBUS_CMD_READ_SHIFTED 0x20
#define DBUS_CMD_WRITE 0x60
#define DBUS_RSP_MARKER 0xA0
#define DBAL_CRC_POLY 0x07u
#define DBAL_CRC_INIT 0xFFu

#define MOTOR_COUNT 4
#define LOCAL_MOTOR_COUNT 2
#define MOTOR_REG_BASE   0x5000u
#define MOTOR_REG_STRIDE 0x10u
#define MOTOR_REG_ENABLE_OFFSET   0x0u
#define MOTOR_REG_SPEED_OFFSET    0x4u
#define MOTOR_REG_FEEDBACK_OFFSET 0x8u
#define MOTOR_REG_STATUS_OFFSET   0xCu

#define SONIC_REG_BASE            0x5100u
#define SONIC_REG_STRIDE          0x04u
#define SONIC_REG_DISTANCE_OFFSET 0x00u

#define SONIC_SENSOR_COUNT 1u
#define SONIC_TRIGGER_PERIOD_US 100000u
#define SONIC_ECHO_TIMEOUT_US   30000u

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

static uint8_t rx_frame_raw[DBAL_MAX_FRAME_SIZE];
static uint8_t tx_frame_desired[FRAME_SIZE];
static uint8_t tx_frame_wire[FRAME_SIZE];
static uint8_t last_read_rsp_desired[FRAME_SIZE] = {0};
static uint8_t last_read_rsp_wire[FRAME_SIZE] = {0};
static volatile uint32_t last_read_rsp_count = 0u;
static volatile uint16_t last_read_rsp_addr = 0u;
static volatile uint32_t last_read_rsp_value = 0u;
static volatile uint32_t last_cs_end_preload_count = 0u;
static volatile uint32_t last_cs_start_queue_count = 0u;
static volatile uint32_t current_active_tx_pushes = 0u;
static volatile uint32_t last_active_tx_pushes = 0u;
static volatile uint32_t last_tx_index_at_cs_fall = 0u;
static volatile uint32_t last_tx_index_at_cs_rise = 0u;

static size_t rx_index = 0;
static size_t tx_index = 0;
static bool tx_frame_prequeued = false;
static bool tx_read_response_pending = false;
static volatile bool cs_active_flag = false;
static volatile bool cs_start_pending = false;
static volatile bool cs_end_pending = false;
static bool cs_polled_prev = false;
static volatile uint32_t cs_fall_count = 0u;
static volatile uint32_t cs_rise_count = 0u;
static volatile uint32_t rx_byte_total = 0u;
static volatile uint32_t frame8_total = 0u;
static volatile uint32_t frame_dbal_total = 0u;
static volatile uint32_t frame_other_total = 0u;
static volatile uint32_t last_frame_len = 0u;
static volatile uint32_t motor_enable_cmd_count = 0u;
static volatile uint32_t motor_speed_cmd_count = 0u;
static volatile uint32_t last_motor_cmd_ms = 0u;   /* dead-man switch timestamp */
static volatile bool      motor_deadman_fired = false;
static volatile uint32_t frame8_decode_ok_count = 0u;
static volatile uint32_t frame8_write_count = 0u;
static volatile uint32_t frame8_read_count = 0u;
static volatile uint32_t frame8_motor_write_count = 0u;
static volatile uint32_t frame8_stream_try_count = 0u;
static volatile uint32_t frame8_stream_ok_count = 0u;
static uint8_t rx_stream_window[FRAME_SIZE] = {0};
static uint32_t rx_stream_fill = 0u;
static volatile uint32_t rx_stream_byte_count = 0u;
static volatile uint32_t frame8_fail_dump_count = 0u;
static uint8_t last_failed_raw_frame[FRAME_SIZE] = {0};
static uint32_t rx_last_byte_us = 0u;
static uint8_t sniff_frame[FRAME_SIZE] = {0};
static uint8_t sniff_len = 0u;
static uint8_t sniff_byte = 0u;
static uint8_t sniff_bit_count = 0u;
static bool sniff_prev_cs_active = false;
static bool sniff_prev_sck_level = false;
static volatile uint16_t last_service_id = 0u;
static volatile uint16_t last_command_id = 0u;
static volatile uint8_t last_motor_index = 0u;
static volatile int32_t last_motor_value = 0;
static volatile uint32_t sck_edge_count = 0u;
static volatile uint32_t mosi_toggle_count = 0u;
static volatile uint8_t last_frame8_cmd = 0u;
static volatile uint16_t last_frame8_addr = 0u;
static volatile uint32_t last_frame8_value = 0u;
static volatile bool sniff_finalize_pending = false;
static bool sck_prev_level = false;
static bool mosi_prev_level = false;

/* Full GPIO sweep: GP0-GP26 edge bitmask (bit N = GP N has seen a transition). */
#define DIAG_FULL_PINS 27u
static bool diag_full_prev[DIAG_FULL_PINS];
static volatile uint32_t diag_full_edges[DIAG_FULL_PINS];
static volatile uint32_t diag_full_bitmask; /* set bit = any edge seen */

typedef enum {
    TRANSFORM_IDENTITY = 0,
    TRANSFORM_ROL1,
    TRANSFORM_ROR1,
    TRANSFORM_SERIAL_ROL1,
    TRANSFORM_SERIAL_ROR1,
} bit_transform_t;

static bit_transform_t tx_transform = TRANSFORM_ROL1;

static bool process_rx_frame(void);
static bool addr_is_valid(uint16_t addr);
static void motor_write(uint16_t addr, uint32_t value);
static uint32_t motor_read(uint16_t addr);
static inline bool cs_is_active(void);
static inline void spi_slave_rearm(spi_inst_t *spi);
static inline size_t spi_slave_queue_current_tx_frame(spi_inst_t *spi);
static inline size_t spi_slave_force_queue_current_tx_frame(spi_inst_t *spi);
static void prepare_tx_frame_wire(void);
static void prepare_tx_frame_identity(void);
static void set_default_tx_pattern(void);
static void prepare_read_response_frame(uint16_t addr, uint32_t value);

static void sniff_process_frame(void)
{
    last_frame_len = sniff_len;

    bool missing_tail_byte = false;

    if (sniff_len < (FRAME_SIZE - 1u)) {
        if (sniff_len > 0u) {
            frame_other_total++;
        }
        return;
    }

    if (sniff_len == (FRAME_SIZE - 1u)) {
        missing_tail_byte = true;
    }

    uint8_t cmd = sniff_frame[0];
    uint16_t addr = ((uint16_t)sniff_frame[1] << 8) | sniff_frame[2];
    uint8_t len_words = sniff_frame[3];

    if ((cmd != DBUS_CMD_WRITE && cmd != DBUS_CMD_READ) || len_words != 1u || !addr_is_valid(addr)) {
        frame_other_total++;
        memcpy(last_failed_raw_frame, sniff_frame, FRAME_SIZE);
        return;
    }

    frame8_total++;
    frame8_decode_ok_count++;
    last_frame8_cmd = cmd;
    last_frame8_addr = addr;

    if (cmd == DBUS_CMD_WRITE) {
        uint32_t value =
            (uint32_t)sniff_frame[4] |
            ((uint32_t)sniff_frame[5] << 8) |
            ((uint32_t)sniff_frame[6] << 16) |
            ((uint32_t)(missing_tail_byte ? 0u : sniff_frame[7]) << 24);
        last_frame8_value = value;
        frame8_write_count++;
        if (addr >= MOTOR_REG_BASE && addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
            frame8_motor_write_count++;
        }
        motor_write(addr, value);
        reg_write(addr, value);
        set_default_tx_pattern();
        tx_read_response_pending = false;
        /* Only prepare the next frame here. The actual SPI rearm/preload must
         * wait until cs_end drain completes so we do not flush/reseed the SSP
         * while BSY may still reflect the just-finished transaction. */
        tx_frame_prequeued = false;
                if (addr >= MOTOR_REG_BASE && addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
                    last_motor_cmd_ms = to_ms_since_boot(get_absolute_time());
                    motor_deadman_fired = false;
                }
        return;
    }

    if (cmd == DBUS_CMD_READ) {
        uint32_t value = reg_read(addr);
        if (addr >= MOTOR_REG_BASE && addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
            value = motor_read(addr);
        }
        last_frame8_value = value;
        frame8_read_count++;
        last_read_rsp_count++;
        last_read_rsp_addr = addr;
        last_read_rsp_value = value;
        prepare_read_response_frame(addr, value);
    }
}

static inline void sniff_update(void)
{
    if (sniff_finalize_pending) {
        sniff_finalize_pending = false;
        if (sniff_prev_cs_active) {
            sniff_process_frame();
            sniff_len = 0u;
            sniff_byte = 0u;
            sniff_bit_count = 0u;
            sniff_prev_cs_active = false;
        }
        sniff_prev_sck_level = gpio_get(PIN_SCK);
    }

    bool cs_active = !gpio_get(PIN_CS);
    bool sck_level = gpio_get(PIN_SCK);

    if (cs_active && !sniff_prev_cs_active) {
        sniff_len = 0u;
        sniff_byte = 0u;
        sniff_bit_count = 0u;
    }

    if (!cs_active && sniff_prev_cs_active) {
        sniff_process_frame();
        sniff_len = 0u;
        sniff_byte = 0u;
        sniff_bit_count = 0u;
    }

    if (cs_active && !sniff_prev_sck_level && sck_level) {
        sniff_byte = (uint8_t)((sniff_byte << 1u) | (gpio_get(PIN_MOSI) ? 1u : 0u));
        sniff_bit_count++;
        if (sniff_bit_count == 8u) {
            if (sniff_len < FRAME_SIZE) {
                sniff_frame[sniff_len++] = sniff_byte;
            }
            sniff_byte = 0u;
            sniff_bit_count = 0u;
        }
    }

    sniff_prev_sck_level = sck_level;
    sniff_prev_cs_active = cs_active;
}

static void handle_rx_byte(uint8_t rx_byte)
{
    if (rx_index < DBAL_MAX_FRAME_SIZE) {
        rx_frame_raw[rx_index++] = rx_byte;
        rx_byte_total++;
        rx_last_byte_us = time_us_32();
        /* Do NOT call process_rx_frame() here and do NOT reset rx_index.
         * handle_rx_byte() is called from the bottom service loop which runs
         * while CS is still active (mid-transfer).  Calling process_rx_frame()
         * here triggers spi_slave_rearm() mid-transfer, flushing the TX FIFO
         * and causing bytes 1-7 of the response to underflow to 0x00.
         * Frame processing is done safely in the cs_end handler of
         * service_spi_frame(), after CS deasserts. */
    }

    /* Streaming window: accumulate only — do NOT call process_rx_frame().
     * The streaming path fires process_rx_frame() on every incoming byte
     * (once the window is full), which means spi_slave_rearm() would be
     * called dozens of times per second mid-transfer.  Decode happens in
     * the cs_end handler; the stream window is kept for future diagnostics. */
    if (rx_stream_fill < FRAME_SIZE) {
        rx_stream_window[rx_stream_fill++] = rx_byte;
    } else {
        for (size_t i = 0u; i < (FRAME_SIZE - 1u); i++) {
            rx_stream_window[i] = rx_stream_window[i + 1u];
        }
        rx_stream_window[FRAME_SIZE - 1u] = rx_byte;
    }

    rx_stream_byte_count++;
    frame8_stream_try_count++;
}

static uint motor_pwma_slice = 0u;
static uint motor_pwma_channel = 0u;
static uint motor_pwmb_slice = 0u;
static uint motor_pwmb_channel = 0u;
static const char *const motor_names[MOTOR_COUNT] = {
    "motor1",
    "motor2",
    "motor3",
    "motor4",
};

#if PICO_NODE_SLOT == 1
#define LOCAL_LOGICAL_MOTOR_A 0u
#define LOCAL_LOGICAL_MOTOR_B 2u
#elif PICO_NODE_SLOT == 2
#define LOCAL_LOGICAL_MOTOR_A 1u
#define LOCAL_LOGICAL_MOTOR_B 3u
#else
#error "PICO_NODE_SLOT must be 1 or 2"
#endif

static bool logical_motor_to_local_slot(size_t logical_index, size_t *local_slot)
{
    if (logical_index == LOCAL_LOGICAL_MOTOR_A) {
        *local_slot = 0u;
        return true;
    }
    if (logical_index == LOCAL_LOGICAL_MOTOR_B) {
        *local_slot = 1u;
        return true;
    }
    return false;
}

static size_t local_peer_logical_motor(size_t logical_index)
{
    return (logical_index == LOCAL_LOGICAL_MOTOR_A) ? LOCAL_LOGICAL_MOTOR_B : LOCAL_LOGICAL_MOTOR_A;
}

static bool any_local_motor_enabled(void)
{
    return (motors[LOCAL_LOGICAL_MOTOR_A].enable != 0u) ||
           (motors[LOCAL_LOGICAL_MOTOR_B].enable != 0u);
}

typedef enum {
    SONIC_IDLE = 0,
    SONIC_WAIT_RISE,
    SONIC_WAIT_FALL,
} sonic_state_t;

static sonic_state_t sonic_state = SONIC_IDLE;
static uint32_t sonic_last_trigger_us = 0u;
static uint32_t sonic_echo_start_us = 0u;
static uint32_t sonic_distance_mm = 0u;

static void lcd_pulse_enable(void)
{
    gpio_put(PIN_LCD_E, 1);
    sleep_us(1);
    gpio_put(PIN_LCD_E, 0);
    sleep_us(40);
}

static void lcd_write_nibble(uint8_t nibble)
{
    gpio_put(PIN_LCD_D4, (nibble >> 0) & 0x01u);
    gpio_put(PIN_LCD_D5, (nibble >> 1) & 0x01u);
    gpio_put(PIN_LCD_D6, (nibble >> 2) & 0x01u);
    gpio_put(PIN_LCD_D7, (nibble >> 3) & 0x01u);
    lcd_pulse_enable();
}

static void lcd_send_byte(bool is_data, uint8_t value)
{
    gpio_put(PIN_LCD_RS, is_data ? 1 : 0);
    lcd_write_nibble((uint8_t)(value >> 4));
    lcd_write_nibble((uint8_t)(value & 0x0Fu));
}

static void lcd_cmd(uint8_t cmd)
{
    lcd_send_byte(false, cmd);
    if (cmd == 0x01u || cmd == 0x02u) {
        sleep_ms(2);
    }
}

static void lcd_data(uint8_t data)
{
    lcd_send_byte(true, data);
}

static void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t base = (row == 0u) ? 0x00u : 0x40u;
    if (col > 15u) {
        col = 15u;
    }
    lcd_cmd((uint8_t)(0x80u | (base + col)));
}

static void lcd_clear(void)
{
    lcd_cmd(0x01u);
}

static void lcd_print(uint8_t row, uint8_t col, const char *text, uint8_t text_len)
{
    if (row > 1u) {
        row = 1u;
    }
    if (col > 15u) {
        col = 15u;
    }

    lcd_set_cursor(row, col);
    for (uint8_t i = 0u; i < text_len && (col + i) < 16u; i++) {
        lcd_data((uint8_t)text[i]);
    }
}

static void lcd_init_1602(void)
{
    gpio_init(PIN_LCD_RS);
    gpio_set_dir(PIN_LCD_RS, GPIO_OUT);
    gpio_put(PIN_LCD_RS, 0);

    gpio_init(PIN_LCD_E);
    gpio_set_dir(PIN_LCD_E, GPIO_OUT);
    gpio_put(PIN_LCD_E, 0);

    gpio_init(PIN_LCD_D4);
    gpio_set_dir(PIN_LCD_D4, GPIO_OUT);
    gpio_put(PIN_LCD_D4, 0);
    gpio_init(PIN_LCD_D5);
    gpio_set_dir(PIN_LCD_D5, GPIO_OUT);
    gpio_put(PIN_LCD_D5, 0);
    gpio_init(PIN_LCD_D6);
    gpio_set_dir(PIN_LCD_D6, GPIO_OUT);
    gpio_put(PIN_LCD_D6, 0);
    gpio_init(PIN_LCD_D7);
    gpio_set_dir(PIN_LCD_D7, GPIO_OUT);
    gpio_put(PIN_LCD_D7, 0);

    sleep_ms(20);
    gpio_put(PIN_LCD_RS, 0);
    lcd_write_nibble(0x03u);
    sleep_ms(5);
    lcd_write_nibble(0x03u);
    sleep_us(150);
    lcd_write_nibble(0x03u);
    lcd_write_nibble(0x02u);

    lcd_cmd(0x28u);
    lcd_cmd(0x0Cu);
    lcd_cmd(0x06u);
    lcd_clear();
    lcd_print(0u, 0u, "DBAL SPI READY", 14u);
}

static void sonic_init(void)
{
    gpio_init(PIN_SONIC_TRIG);
    gpio_set_dir(PIN_SONIC_TRIG, GPIO_OUT);
    gpio_put(PIN_SONIC_TRIG, 0);

    gpio_init(PIN_SONIC_ECHO);
    gpio_set_dir(PIN_SONIC_ECHO, GPIO_IN);
    gpio_pull_down(PIN_SONIC_ECHO);

    sonic_state = SONIC_IDLE;
    sonic_last_trigger_us = time_us_32();
    sonic_distance_mm = 0u;
    reg_write((uint16_t)(SONIC_REG_BASE + SONIC_REG_DISTANCE_OFFSET), sonic_distance_mm);
}

static void sonic_poll(void)
{
    uint32_t now_us = time_us_32();
    bool echo_high = gpio_get(PIN_SONIC_ECHO);

    switch (sonic_state) {
        case SONIC_IDLE:
            if ((uint32_t)(now_us - sonic_last_trigger_us) >= SONIC_TRIGGER_PERIOD_US) {
                gpio_put(PIN_SONIC_TRIG, 1);
                busy_wait_us_32(10u);
                gpio_put(PIN_SONIC_TRIG, 0);
                sonic_last_trigger_us = time_us_32();
                sonic_state = SONIC_WAIT_RISE;
            }
            break;

        case SONIC_WAIT_RISE:
            if (echo_high) {
                sonic_echo_start_us = now_us;
                sonic_state = SONIC_WAIT_FALL;
            } else if ((uint32_t)(now_us - sonic_last_trigger_us) >= SONIC_ECHO_TIMEOUT_US) {
                sonic_distance_mm = 0u;
                reg_write((uint16_t)(SONIC_REG_BASE + SONIC_REG_DISTANCE_OFFSET), sonic_distance_mm);
                led_update_from_distance(sonic_distance_mm);
                sonic_state = SONIC_IDLE;
            }
            break;

        case SONIC_WAIT_FALL:
            if (!echo_high) {
                uint32_t pulse_us = (uint32_t)(now_us - sonic_echo_start_us);
                sonic_distance_mm = (pulse_us * 343u) / 2000u;
                reg_write((uint16_t)(SONIC_REG_BASE + SONIC_REG_DISTANCE_OFFSET), sonic_distance_mm);
                led_update_from_distance(sonic_distance_mm);
                sonic_state = SONIC_IDLE;
            } else if ((uint32_t)(now_us - sonic_echo_start_us) >= SONIC_ECHO_TIMEOUT_US) {
                sonic_distance_mm = 0u;
                reg_write((uint16_t)(SONIC_REG_BASE + SONIC_REG_DISTANCE_OFFSET), sonic_distance_mm);
                led_update_from_distance(sonic_distance_mm);
                sonic_state = SONIC_IDLE;
            }
            break;

        default:
            sonic_state = SONIC_IDLE;
            break;
    }
}

static void led_init(void)
{
    const uint led_pins[] = { PIN_LED_GREEN, PIN_LED_YELLOW, PIN_LED_RED };
    for (size_t i = 0u; i < 3u; i++) {
        gpio_init(led_pins[i]);
        gpio_set_dir(led_pins[i], GPIO_OUT);
        gpio_put(led_pins[i], 0);
    }
}

/* Update RGB warning LEDs based on measured distance.
 *  > LED_DIST_GREEN_MM  : green only
 *  > LED_DIST_YELLOW_MM : yellow only
 *  <= LED_DIST_YELLOW_MM: red only
 *  0 (no echo)          : all off
 */
static void led_update_from_distance(uint32_t dist_mm)
{
    bool green  = false;
    bool yellow = false;
    bool red    = false;

    if (dist_mm > 0u) {
        if (dist_mm > LED_DIST_GREEN_MM) {
            green = true;
        } else if (dist_mm > LED_DIST_YELLOW_MM) {
            yellow = true;
        } else {
            red = true;
        }
    }

    gpio_put(PIN_LED_GREEN,  green  ? 1 : 0);
    gpio_put(PIN_LED_YELLOW, yellow ? 1 : 0);
    gpio_put(PIN_LED_RED,    red    ? 1 : 0);
}

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
    size_t local_slot = 0u;

    if (motor_index >= MOTOR_COUNT) {
        return;
    }

    if (!logical_motor_to_local_slot(motor_index, &local_slot)) {
        return;
    }

    /* TB6612 STBY is shared on each board; keep it high if any locally owned
     * logical motor is enabled. */
    bool stby_needed = any_local_motor_enabled();

#if PICO_DIAG_FORCE_MOTOR0_ONLY
    if (local_slot == 1u) {
        /* Diagnostic isolation mode: local channel B is forced off regardless
         * of incoming commands. */
        gpio_put(PIN_MOTOR_BIN1, 0);
        gpio_put(PIN_MOTOR_BIN2, 0);
        pwm_set_chan_level(motor_pwmb_slice, motor_pwmb_channel, 0u);
        motors[motor_index].enable = 0u;
        motors[motor_index].speed_setpoint = 0;
        motors[motor_index].speed_feedback = 0;
        motors[motor_index].status = MOTOR_STATUS_AVAILABLE;
        stby_needed = any_local_motor_enabled();
        gpio_put(PIN_MOTOR_STBY, stby_needed ? 1 : 0);
        return;
    }
#endif

    bool enabled = (motors[motor_index].enable != 0u);
    int32_t speed = motors[motor_index].speed_setpoint;
    bool forward = speed >= 0;

    if (local_slot == 0u) {
        /* Local channel A */
        if (!enabled || speed == 0) {
            gpio_put(PIN_MOTOR_AIN1, 0);
            gpio_put(PIN_MOTOR_AIN2, 0);
            pwm_set_chan_level(motor_pwma_slice, motor_pwma_channel, 0u);
        } else {
            gpio_put(PIN_MOTOR_AIN1, forward ? 1 : 0);
            gpio_put(PIN_MOTOR_AIN2, forward ? 0 : 1);
            pwm_set_chan_level(motor_pwma_slice, motor_pwma_channel, speed_to_pwm_level(speed));
        }
    } else {
        /* Local channel B */
        if (!enabled || speed == 0) {
            gpio_put(PIN_MOTOR_BIN1, 0);
            gpio_put(PIN_MOTOR_BIN2, 0);
            pwm_set_chan_level(motor_pwmb_slice, motor_pwmb_channel, 0u);
        } else {
            gpio_put(PIN_MOTOR_BIN1, forward ? 1 : 0);
            gpio_put(PIN_MOTOR_BIN2, forward ? 0 : 1);
            pwm_set_chan_level(motor_pwmb_slice, motor_pwmb_channel, speed_to_pwm_level(speed));
        }
    }

    gpio_put(PIN_MOTOR_STBY, stby_needed ? 1 : 0);
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

    /* Motor 2 — TB6612FNG channel B, STBY shared with channel A (GP14) */
    gpio_init(PIN_MOTOR_BIN1);
    gpio_set_dir(PIN_MOTOR_BIN1, GPIO_OUT);
    gpio_put(PIN_MOTOR_BIN1, 0);

    gpio_init(PIN_MOTOR_BIN2);
    gpio_set_dir(PIN_MOTOR_BIN2, GPIO_OUT);
    gpio_put(PIN_MOTOR_BIN2, 0);

    gpio_set_function(PIN_MOTOR_PWMB, GPIO_FUNC_PWM);
    motor_pwmb_slice = pwm_gpio_to_slice_num(PIN_MOTOR_PWMB);
    motor_pwmb_channel = pwm_gpio_to_channel(PIN_MOTOR_PWMB);
    pwm_set_wrap(motor_pwmb_slice, 65535u);
    pwm_set_chan_level(motor_pwmb_slice, motor_pwmb_channel, 0u);
    pwm_set_enabled(motor_pwmb_slice, true);
}

static void run_startup_motor_self_test(void)
{
#if PICO_STARTUP_SELF_TEST
    const uint16_t duty = speed_to_pwm_level(700);

    /* Short direct pulses to verify motor driver/power/wiring independent of DBus/SPI. */
    gpio_put(PIN_MOTOR_STBY, 1);

    gpio_put(PIN_MOTOR_AIN1, 1);
    gpio_put(PIN_MOTOR_AIN2, 0);
    pwm_set_chan_level(motor_pwma_slice, motor_pwma_channel, duty);
    sleep_ms(300);
    pwm_set_chan_level(motor_pwma_slice, motor_pwma_channel, 0u);
    gpio_put(PIN_MOTOR_AIN1, 0);
    gpio_put(PIN_MOTOR_AIN2, 0);
    sleep_ms(150);

    gpio_put(PIN_MOTOR_BIN1, 1);
    gpio_put(PIN_MOTOR_BIN2, 0);
    pwm_set_chan_level(motor_pwmb_slice, motor_pwmb_channel, duty);
    sleep_ms(300);
    pwm_set_chan_level(motor_pwmb_slice, motor_pwmb_channel, 0u);
    gpio_put(PIN_MOTOR_BIN1, 0);
    gpio_put(PIN_MOTOR_BIN2, 0);

    gpio_put(PIN_MOTOR_STBY, 0);
    printf("[Pico SPI Slave] startup self-test complete (A+B pulse)\n");
#endif
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

static uint8_t dbal_crc8(const uint8_t *data, size_t len)
{
    uint8_t crc = DBAL_CRC_INIT;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8u; b++) {
            if ((crc & 0x80u) != 0u) {
                crc = (uint8_t)((crc << 1u) ^ DBAL_CRC_POLY);
            } else {
                crc <<= 1u;
            }
        }
    }
    return crc;
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

    if (addr >= SONIC_REG_BASE &&
        addr < (SONIC_REG_BASE + (SONIC_SENSOR_COUNT * SONIC_REG_STRIDE))) {
        return true;
    }

    return false;
}

static bool decoded_frame_is_valid(const uint8_t *decoded_frame)
{
    uint8_t cmd = decoded_frame[0] & 0x60u;
    uint8_t len_words = decoded_frame[3];
    uint16_t addr = ((uint16_t)decoded_frame[1] << 8) | decoded_frame[2];

    if (cmd == DBUS_CMD_READ_SHIFTED) {
        cmd = DBUS_CMD_READ;
    }

    if (cmd != DBUS_CMD_READ && cmd != DBUS_CMD_WRITE) {
        return false;
    }

    if (!addr_is_valid(addr)) {
        return false;
    }

    if (len_words != 1u) {
        dlog("[DBAL] len_anom cmd=0x%02x addr=0x%04x len=%u\n", cmd, addr, (unsigned)len_words);
    }

    return true;
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

    if (index >= MOTOR_COUNT) {
        return;
    }

    {
        size_t local_slot = 0u;
        if (!logical_motor_to_local_slot(index, &local_slot)) {
            return;
        }
    }

#if PICO_DIAG_FORCE_MOTOR0_ONLY
    {
        size_t local_slot = 0u;
        if (logical_motor_to_local_slot(index, &local_slot) && local_slot == 1u) {
            /* Ignore local channel-B commands in isolation mode and keep outputs low. */
            motors[index].enable = 0u;
            motors[index].speed_setpoint = 0;
            motors[index].speed_feedback = 0;
            motors[index].status = MOTOR_STATUS_AVAILABLE;
            apply_motor_outputs(index);
            return;
        }
    }
#endif

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

/* Current capture logs show RW612 receiving the Pico's prepared bytes nearly
 * verbatim. Treat tx_frame_desired as the actual on-wire frame and copy it
 * unchanged into tx_frame_wire. */
static void prepare_tx_frame_wire(void)
{
    prepare_tx_frame_identity();
}

static void prepare_tx_frame_identity(void)
{
    memcpy(tx_frame_wire, tx_frame_desired, FRAME_SIZE);
}

static void set_default_tx_pattern(void)
{
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        tx_frame_desired[i] = 0xFF;
    }
    prepare_tx_frame_wire();
}

static void prepare_read_response_frame(uint16_t addr, uint32_t value)
{
    uint8_t next_frame[FRAME_SIZE] = {0};
    bool same_pending_response;

    next_frame[0] = DBUS_RSP_MARKER;
    next_frame[1] = (uint8_t)(addr >> 8);
    next_frame[2] = (uint8_t)(addr & 0xFFu);
    next_frame[3] = 0x01u;
    next_frame[4] = (uint8_t)(value);
    next_frame[5] = (uint8_t)(value >> 8);
    next_frame[6] = (uint8_t)(value >> 16);
    next_frame[7] = (uint8_t)(value >> 24);

    same_pending_response = tx_read_response_pending &&
                            tx_index < FRAME_SIZE &&
                            (memcmp(last_read_rsp_desired, next_frame, FRAME_SIZE) == 0);

    memcpy(tx_frame_desired, next_frame, FRAME_SIZE);
    prepare_tx_frame_wire();
    memcpy(last_read_rsp_desired, tx_frame_desired, FRAME_SIZE);
    memcpy(last_read_rsp_wire, tx_frame_wire, FRAME_SIZE);
    tx_read_response_pending = true;

    if (!same_pending_response) {
        tx_index = 0u;
        tx_frame_prequeued = false;
    }
}

/* DBAL wire frame layout (all bytes after per-byte ROL1 decode):
 *  [0]       SOF = 0xAA
 *  [1]       payload_len  (= total_frame - 3)
 *  [2]       CRC8(bytes[3..total-1])
 *  [3]       sender       (0x01)
 *  [4]       protocol type (0x01)
 *  [5]       sequence ID
 *  [6..9]    reserved/zero (header gap)
 *  [10]      inner data_len
 *  [11]      service_id_hi
 *  [12]      service_id_lo
 *  [13]      command_id_hi
 *  [14]      command_id_lo
 *  [15..]    payload
 */
#define DBAL_WIRE_SOF          0xAAu
#define DBAL_WIRE_SVC_HI_OFF   11u
#define DBAL_WIRE_SVC_LO_OFF   12u
#define DBAL_WIRE_CMD_HI_OFF   13u
#define DBAL_WIRE_CMD_LO_OFF   14u
#define DBAL_WIRE_DATA_LEN_OFF 10u
#define DBAL_WIRE_DATA_START   15u
#define DBAL_WIRE_MIN_FRAME    16u
#define DBAL_MOTOR_SERVICE_ID  0x7100u
#define DBAL_MOTOR_CMD_ENABLE  0x0001u
#define DBAL_MOTOR_CMD_SPEED   0x0002u
#define DBAL_LCD_SERVICE_ID    0x7102u
#define DBAL_LCD_CMD_CLEAR     0x0001u
#define DBAL_LCD_CMD_PRINT     0x0002u
#define DBAL_LCD_CMD_SET_CURSOR 0x0003u

static bool process_dbal_frame(size_t count)
{
    uint8_t decoded[DBAL_MAX_FRAME_SIZE];
    uint8_t reconstructed[DBAL_MAX_FRAME_SIZE];
    const bit_transform_t transforms[] = {
        TRANSFORM_IDENTITY,
        TRANSFORM_ROL1,
        TRANSFORM_ROR1,
        TRANSFORM_SERIAL_ROR1,
        TRANSFORM_SERIAL_ROL1,
    };

    if (count < DBAL_WIRE_MIN_FRAME || count > DBAL_MAX_FRAME_SIZE) {
        dlog("[DBAL] bad count=%u\n", (unsigned)count);
        return false;
    }

    for (size_t t = 0u; t < (sizeof(transforms) / sizeof(transforms[0])); t++) {
        bit_transform_t transform = transforms[t];

        if (transform == TRANSFORM_SERIAL_ROR1) {
            decoded[0] = (uint8_t)(rx_frame_raw[0] >> 1);
            for (size_t i = 1; i < count; i++) {
                decoded[i] = (uint8_t)((rx_frame_raw[i] >> 1) | ((rx_frame_raw[i - 1] & 0x01u) << 7));
            }
        } else if (transform == TRANSFORM_SERIAL_ROL1) {
            for (size_t i = 0; i < (count - 1u); i++) {
                decoded[i] = (uint8_t)((rx_frame_raw[i] << 1) | (rx_frame_raw[i + 1] >> 7));
            }
            decoded[count - 1u] = (uint8_t)(rx_frame_raw[count - 1u] << 1);
        } else {
            for (size_t i = 0; i < count; i++) {
                decoded[i] = apply_transform(rx_frame_raw[i], transform);
            }
        }

        /* Frames can be skewed/corrupted at the first byte (observed raw0=0x54).
         * Search small offsets and identify DBAL by service/cmd/data signature,
         * not SOF alone. */
        for (size_t base = 0u; base <= 2u; base++) {
            if (base >= count) {
                break;
            }

            if ((base + DBAL_WIRE_DATA_START) >= count) {
                continue;
            }

            size_t svc_hi_off = base + DBAL_WIRE_SVC_HI_OFF;
            size_t svc_lo_off = base + DBAL_WIRE_SVC_LO_OFF;
            size_t cmd_hi_off = base + DBAL_WIRE_CMD_HI_OFF;
            size_t cmd_lo_off = base + DBAL_WIRE_CMD_LO_OFF;
            size_t dlen_off   = base + DBAL_WIRE_DATA_LEN_OFF;
            size_t data_start = base + DBAL_WIRE_DATA_START;

            if (svc_lo_off >= count || cmd_lo_off >= count || dlen_off >= count) {
                continue;
            }

            if ((base + 3u) >= count) {
                continue;
            }

            uint8_t payload_len = decoded[base + 1u];
            size_t expected_total = (size_t)payload_len + 3u;
            size_t frame_end = base + expected_total;
            bool missing_tail_byte = false;

            if (expected_total < DBAL_WIRE_MIN_FRAME) {
                continue;
            }

            if (frame_end > count) {
                if (frame_end != (count + 1u)) {
                    continue;
                }
                missing_tail_byte = true;
            }

            uint16_t svc_id  = ((uint16_t)decoded[svc_hi_off] << 8u)
                              | decoded[svc_lo_off];
            uint16_t cmd_id  = ((uint16_t)decoded[cmd_hi_off] << 8u)
                              | decoded[cmd_lo_off];
            uint8_t data_len = decoded[dlen_off];
            uint8_t sender = decoded[base + 3u];
            uint8_t proto  = decoded[base + 4u];

            if (missing_tail_byte) {
                bool recoverable_speed_tail = (sender == 0x01u) &&
                                              (proto == 0x01u) &&
                                              (svc_id == DBAL_MOTOR_SERVICE_ID) &&
                                              (cmd_id == DBAL_MOTOR_CMD_SPEED) &&
                                              (data_len == 5u) &&
                                              ((data_start + data_len) == frame_end);
                if (!recoverable_speed_tail) {
                    continue;
                }
            }

            if (frame_end > count && !missing_tail_byte) {
                continue;
            }

            uint8_t rx_crc = decoded[base + 2u];
            uint8_t calc_crc;
            if (missing_tail_byte) {
                memcpy(reconstructed, decoded, count);
                reconstructed[count] = 0u;
                calc_crc = dbal_crc8(&reconstructed[base + 3u], expected_total - 3u);
            } else {
                calc_crc = dbal_crc8(&decoded[base + 3u], expected_total - 3u);
            }
            if (rx_crc != calc_crc) {
                continue;
            }

            dlog("[DBAL] svc=0x%04x cmd=0x%04x dlen=%u t=%u b=%u\n",
                 svc_id, cmd_id, data_len, (unsigned)transform, (unsigned)base);

            if (sender != 0x01u || proto != 0x01u) {
                continue;
            }

            if (svc_id != DBAL_MOTOR_SERVICE_ID && svc_id != DBAL_LCD_SERVICE_ID) {
                continue;
            }

            if ((data_start + data_len) != frame_end) {
                continue;
            }

            if (svc_id == DBAL_MOTOR_SERVICE_ID) {
                if ((cmd_id == DBAL_MOTOR_CMD_ENABLE && data_len != 2u) ||
                    (cmd_id == DBAL_MOTOR_CMD_SPEED && data_len != 5u)) {
                    continue;
                }
            }

            uint8_t motor_index = (data_len >= 1u) ? decoded[data_start] : 0u;

            last_service_id = svc_id;
            last_command_id = cmd_id;
            last_motor_index = motor_index;

            if (svc_id == DBAL_MOTOR_SERVICE_ID) {
                if (cmd_id == DBAL_MOTOR_CMD_ENABLE) {
                    uint8_t enable = (data_len >= 2u) ? decoded[data_start + 1u] : 0u;
                    motor_enable_cmd_count++;
                    last_motor_value = (int32_t)enable;
                    dlog("[DBAL] motor[%u] enable=%u\n", motor_index, enable);
                    motor_write((uint16_t)(MOTOR_REG_BASE
                                           + (uint16_t)motor_index * MOTOR_REG_STRIDE
                                           + MOTOR_REG_ENABLE_OFFSET),
                                (uint32_t)enable);
                    return true;
                }

                if (cmd_id == DBAL_MOTOR_CMD_SPEED && data_len >= 5u) {
                    uint32_t speed_high = missing_tail_byte ? 0u : ((uint32_t)decoded[data_start + 4u] << 24u);
                    int32_t speed = (int32_t)(
                          (uint32_t)decoded[data_start + 1u]
                        | ((uint32_t)decoded[data_start + 2u] << 8u)
                        | ((uint32_t)decoded[data_start + 3u] << 16u)
                        | speed_high);
                    motor_speed_cmd_count++;
                    last_motor_value = speed;
                    if (missing_tail_byte) {
                        dlog("[DBAL] recovered truncated speed frame t=%u b=%u\n",
                             (unsigned)transform, (unsigned)base);
                    }
                    dlog("[DBAL] motor[%u] speed=%ld\n", motor_index, (long)speed);
                    motor_write((uint16_t)(MOTOR_REG_BASE
                                           + (uint16_t)motor_index * MOTOR_REG_STRIDE
                                           + MOTOR_REG_SPEED_OFFSET),
                                (uint32_t)speed);
                    return true;
                }
            }

            if (svc_id == DBAL_LCD_SERVICE_ID) {
                if (cmd_id == DBAL_LCD_CMD_CLEAR) {
                    if (data_len < 1u) {
                        continue;
                    }
                    lcd_clear();
                    return true;
                }

                if (cmd_id == DBAL_LCD_CMD_SET_CURSOR) {
                    if (data_len < 3u) {
                        continue;
                    }
                    lcd_set_cursor(decoded[data_start + 1u], decoded[data_start + 2u]);
                    return true;
                }

                if (cmd_id == DBAL_LCD_CMD_PRINT) {
                    if (data_len < 4u) {
                        continue;
                    }
                    {
                        uint8_t row = decoded[data_start + 1u];
                        uint8_t col = decoded[data_start + 2u];
                        uint8_t text_len = (uint8_t)(data_len - 3u);
                        lcd_print(row, col, (const char *)&decoded[data_start + 3u], text_len);
                    }
                    return true;
                }
            }
        }
    }

    dlog("[DBAL] unparsed count=%u raw0=0x%02x\n", (unsigned)count, rx_frame_raw[0]);
    return false;
}

static bool process_rx_frame(void)
{
    uint8_t decoded[FRAME_SIZE];
    uint8_t rotated[FRAME_SIZE];
    bit_transform_t detected = TRANSFORM_ROL1;
    uint8_t detected_rotation = 0u;

    for (uint8_t rotation = 0u; rotation < FRAME_SIZE; rotation++) {
        for (size_t i = 0u; i < FRAME_SIZE; i++) {
            rotated[i] = rx_frame_raw[(i + rotation) % FRAME_SIZE];
        }

        if (decode_rx_frame_auto(rotated, decoded, &detected)) {
            detected_rotation = rotation;
            goto frame_decoded;
        }
    }

    /* Ignore invalid frames (e.g. RW612 dummy clocks during readback).
     * Do NOT overwrite tx_frame_wire here, otherwise a prepared response
     * can be clobbered by default 0xA5 before the master receives it. */
    if (frame8_fail_dump_count < 5u) {
        frame8_fail_dump_count++;
        printf("[Pico SPI Slave] decode fail raw=%02x %02x %02x %02x %02x %02x %02x %02x\n",
               rx_frame_raw[0], rx_frame_raw[1], rx_frame_raw[2], rx_frame_raw[3],
               rx_frame_raw[4], rx_frame_raw[5], rx_frame_raw[6], rx_frame_raw[7]);
    }
    memcpy(last_failed_raw_frame, rx_frame_raw, FRAME_SIZE);
       dlog("[PICO] FAILED raw: %02x %02x %02x %02x %02x %02x %02x %02x\n",
           rx_frame_raw[0], rx_frame_raw[1], rx_frame_raw[2], rx_frame_raw[3],
           rx_frame_raw[4], rx_frame_raw[5], rx_frame_raw[6], rx_frame_raw[7]);
    return false;

frame_decoded:
    if (detected_rotation != 0u) {
        dlog("[PICO] RX rotation=%u\n", (unsigned)detected_rotation);
    }

    /* tx_transform is always TRANSFORM_ROL1: the link applies ROR1 to MISO
     * (CPHA mismatch), so Pico must pre-compensate with ROL1 regardless of
     * which decode transform was detected for the incoming MOSI bytes. */
    (void)detected;
    frame8_decode_ok_count++;

    uint8_t cmd = decoded[0] & 0x60u;
    if (cmd == DBUS_CMD_READ_SHIFTED) {
        cmd = DBUS_CMD_READ;
    }
    uint16_t addr = ((uint16_t)decoded[1] << 8) | decoded[2];
    last_frame8_cmd = cmd;
    last_frame8_addr = addr;
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
        last_frame8_value = value;
        frame8_write_count++;
        if (addr >= MOTOR_REG_BASE &&
            addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
            frame8_motor_write_count++;
        }

        motor_write(addr, value);
        reg_write(addr, value);
        dlog("[PICO] WRITE addr=0x%04x val=0x%08x\n", addr, value);
        set_default_tx_pattern();
        tx_read_response_pending = false;
        tx_index = 0u;
        tx_frame_prequeued = false;
        return true;
    }

    if (cmd == DBUS_CMD_READ) {
        uint32_t value = reg_read(addr);
        frame8_read_count++;
        if (addr >= MOTOR_REG_BASE && addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
            value = motor_read(addr);
        }
        last_frame8_value = value;
        dlog("[PICO] READ addr=0x%04x val=0x%08x\n", addr, value);
        last_read_rsp_count++;
        last_read_rsp_addr = addr;
        last_read_rsp_value = value;
        prepare_read_response_frame(addr, value);
        return true;
    }

    set_default_tx_pattern();
    tx_read_response_pending = false;
    tx_index = 0u;
    tx_frame_prequeued = false;
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

static inline void spi_slave_rearm(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);
    /* Flush TX and RX FIFOs by toggling SSE (SSP Enable bit).
     * Per ARM PL022 spec: clearing SSE resets both FIFOs.
     * This is safer than spi_deinit()+spi_init() because:
     *   - It stays in slave mode the entire time (no master-mode transition).
     *   - It does NOT drive the SSEL/GP17 pin as an output (master-mode
     *     deinit/init briefly drives GP17 HIGH, which cs_poll_update reads
     *     as a spurious CS-deassert, corrupting the rx_index state machine).
     *   - It does NOT require reinitialising baudrate / format / slave flags.
     */
    hw_clear_bits(&hw->cr1, SPI_SSPCR1_SSE_BITS);  /* SSE=0: flush FIFOs */
    hw_set_bits(&hw->cr1, SPI_SSPCR1_SSE_BITS);    /* SSE=1: re-enable   */
}

static inline size_t spi_slave_continue_tx_frame(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);
    uint32_t start_us = time_us_32();

    while (tx_index < FRAME_SIZE) {
        if (spi_is_writable(spi)) {
            hw->dr = tx_frame_wire[tx_index++];
            continue;
        }
        if ((uint32_t)(time_us_32() - start_us) >= 200u) {
            break;
        }
        tight_loop_contents();
    }

    return tx_index;
}

static inline size_t spi_slave_fill_tx_frame_now(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);

    while (spi_is_writable(spi) && tx_index < FRAME_SIZE) {
        hw->dr = tx_frame_wire[tx_index++];
    }

    return tx_index;
}

static inline size_t spi_slave_queue_current_tx_frame(spi_inst_t *spi)
{
    /* Preserve progress for a pending read response so repeated RW612 dummy
     * retries can collect bytes 1..7 instead of restarting at byte 0 on
     * every CS pulse. For idle/default traffic, restart from the beginning. */
    if (!tx_read_response_pending || tx_index >= FRAME_SIZE) {
        tx_index = 0u;
    }

    /* Count only bytes the SSP reports as accepted. Idle-time preload may
     * only take a prefix of the frame; preserve that prefix and let cs_start
     * plus the active service loop feed the remainder. */
    return spi_slave_continue_tx_frame(spi);
}

static inline size_t spi_slave_force_queue_current_tx_frame(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);

    tx_index = 0u;
    for (size_t i = 0u; i < FRAME_SIZE; i++) {
        hw->dr = tx_frame_wire[i];
        tx_index++;
    }

    return tx_index;
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

static inline void mark_spi_activity(void)
{
    last_spi_activity_us = time_us_32();
}

static void spi_slave_set_miso_active(bool active)
{
    /* When this slave is inactive, make MISO a plain high-impedance input with
     * no pulls so a powered sibling Pico cannot be loaded by this pad state. */
    if (active) {
        gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
        gpio_set_oeover(PIN_MISO, GPIO_OVERRIDE_NORMAL);
        return;
    }

    gpio_init(PIN_MISO);
    gpio_set_dir(PIN_MISO, GPIO_IN);
    gpio_disable_pulls(PIN_MISO);
}

static void cs_gpio_irq_handler(uint gpio, uint32_t events)
{
    if (gpio != PIN_CS) {
        return;
    }

    if ((events & GPIO_IRQ_EDGE_FALL) != 0u) {
        cs_active_flag = true;
        cs_fall_count++;
        last_tx_index_at_cs_fall = (uint32_t)tx_index;
        current_active_tx_pushes = 0u;
        spi_slave_set_miso_active(true);
        cs_start_pending = true;
    }

    if ((events & GPIO_IRQ_EDGE_RISE) != 0u) {
        cs_active_flag = false;
        cs_rise_count++;
        last_tx_index_at_cs_rise = (uint32_t)tx_index;
        last_active_tx_pushes = current_active_tx_pushes;
        sniff_finalize_pending = true;
        spi_slave_set_miso_active(false);
        cs_end_pending = true;
    }
}

static void cs_poll_update(void)
{
    bool cs_active_now = !gpio_get(PIN_CS);

    if (cs_active_now == cs_polled_prev) {
        return;
    }

    cs_polled_prev = cs_active_now;

    if (cs_active_now) {
        cs_active_flag = true;
        cs_fall_count++;
        spi_slave_set_miso_active(true);
        cs_start_pending = true;
    } else {
        cs_active_flag = false;
        cs_rise_count++;
        spi_slave_set_miso_active(false);
        cs_end_pending = true;
    }
}

static inline void sample_spi_pin_activity(void)
{
    bool sck_level = gpio_get(PIN_SCK);
    bool mosi_level = gpio_get(PIN_MOSI);

    if (sck_level != sck_prev_level) {
        sck_edge_count++;
        sck_prev_level = sck_level;
    }

    if (mosi_level != mosi_prev_level) {
        mosi_toggle_count++;
        mosi_prev_level = mosi_level;
    }

    for (size_t i = 0u; i < DIAG_FULL_PINS; i++) {
        bool level = gpio_get((uint)i);
        if (level != diag_full_prev[i]) {
            diag_full_edges[i]++;
            diag_full_bitmask |= (1u << i);
            diag_full_prev[i] = level;
        }
    }
}

static void service_spi_frame(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);

    if (cs_end_pending) {
        uint32_t drain_start_us;
        uint32_t last_rx_us;

        cs_end_pending = false;
        /* If the active service loop exited immediately after CS rose, the
         * sniff decoder may not have consumed that final CS-high transition
         * yet. Force one pass here so the just-finished frame prepares the
         * correct tx_frame_wire before we rearm and preload the SSP. */
        sniff_update();
        /* Keep draining until the RX FIFO has stayed idle
         * briefly, or a short overall timeout expires. This is more robust
         * than a fixed one-shot delay when the final byte lands slightly late
         * relative to CS rise detection. */
        drain_start_us = time_us_32();
        last_rx_us = drain_start_us;
        for (;;) {
            bool saw_rx = false;

            while (spi_is_readable(spi) && rx_index < DBAL_MAX_FRAME_SIZE) {
                handle_rx_byte((uint8_t)hw->dr);
                last_rx_us = time_us_32();
                saw_rx = true;
            }

            if (!saw_rx) {
                uint32_t now_us = time_us_32();
                if ((uint32_t)(now_us - last_rx_us) >= CS_END_DRAIN_IDLE_US) {
                    break;
                }
                if ((uint32_t)(now_us - drain_start_us) >= CS_END_DRAIN_TIMEOUT_US) {
                    break;
                }
            }
        }

        /* PL022 RX capture on this link is not reliable enough to decode
         * commands. Keep the byte count only as a diagnostic and drop the raw
         * bytes; sniff_update() already decoded the authoritative frame while
         * CS was active. */
        last_frame_len = (uint32_t)rx_index;
        if (rx_index > 0u) {
            frame_other_total++;
            rx_index = 0u;
        }

        /* The just-finished frame has now been sniff-decoded and the drain is
         * complete, so it is safe to preload the next TX frame.
         *
         * For pending read responses on Pico2, partial cs_end preload has been
         * observed to stall at a 5-byte prefix (`qend=5`, `tx=5`), which is
         * enough for marker/header leakage but not for the payload bytes to
         * escape. Skip preload in that case and queue the full frame only at
         * cs_start instead. */
        if (!tx_read_response_pending) {
            spi_slave_rearm(spi);
            last_cs_end_preload_count = (uint32_t)spi_slave_queue_current_tx_frame(spi);
            tx_frame_prequeued = (last_cs_end_preload_count == FRAME_SIZE);
        } else {
            last_cs_end_preload_count = 0u;
            tx_frame_prequeued = false;
        }
    }

    if (cs_start_pending) {
        cs_start_pending = false;
        /* Preserve any bytes already accepted during the cs_end preload. If we
         * rearm here, we flush that prefix right before the master clocks the
         * frame. Also do not blindly advance tx_index to 8: on this link the
         * SSP may only accept one start-of-frame byte immediately, and the
         * active loop must be allowed to feed the remaining tail. */
        mark_spi_activity();
        last_cs_start_queue_count = (uint32_t)spi_slave_fill_tx_frame_now(spi);
        tx_frame_prequeued = (last_cs_start_queue_count == FRAME_SIZE);
    }

    if (cs_is_active() || ((hw->sr & 0x10u) != 0u)) {
        uint32_t active_start_us = time_us_32();

        /* Once CS is asserted, stay in a tight local service loop until it
         * deasserts (or a short guard timeout fires). Opportunistic one-shot
         * polling was only capturing the first byte of each 8-byte transfer,
         * which left RX at lflen=1 and caused TX underruns to repeat byte 0. */
        while (cs_is_active() || ((hw->sr & 0x10u) != 0u)) {
            bool progressed = false;

            if ((cs_is_active() || ((hw->sr & 0x10u) != 0u)) && tx_index < FRAME_SIZE) {
                size_t tx_before = tx_index;
                size_t tx_after = spi_slave_fill_tx_frame_now(spi);

                if (tx_after > tx_before) {
                    current_active_tx_pushes += (uint32_t)(tx_after - tx_before);
                    progressed = true;
                }
            }

            while (spi_is_readable(spi)) {
                uint8_t rx_byte = (uint8_t)hw->dr;
                mark_spi_activity();
                handle_rx_byte(rx_byte);
                progressed = true;
            }

            sample_spi_pin_activity();
            sniff_update();

            if (!progressed) {
                tight_loop_contents();
            }

            if ((uint32_t)(time_us_32() - active_start_us) >= 10000u) {
                break;
            }
        }
    }
}

int main(void)
{
    stdio_init_all();
    sleep_ms(50); // Keep startup short so SPI slave is ready before RW612 traffic starts
    printf("[Pico SPI Slave] FW_ID=%s\n", FW_ID_MAIN);
    printf("[Pico SPI Slave] Firmware version: %s\n", PICO_FIRMWARE_VERSION);
    printf("[Pico SPI Slave] USB command: send BOOTSEL + Enter to reboot into UF2 mode\n");
    printf("[Pico SPI Slave] Node slot=%u logical-map: A->motor%u B->motor%u\n",
           (unsigned)PICO_NODE_SLOT,
           (unsigned)(LOCAL_LOGICAL_MOTOR_A + 1u),
           (unsigned)(LOCAL_LOGICAL_MOTOR_B + 1u));
        printf("[Pico SPI Slave] SPI0 pins: MOSI=GP%u CSn=GP%u SCK=GP%u MISO=GP%u\n",
            PIN_MOSI, PIN_CS, PIN_SCK, PIN_MISO);
        printf("[Pico SPI Slave] MotorA pins: PWMA=GP%u STBY=GP%u AIN1=GP%u AIN2=GP%u\n",
            PIN_MOTOR_PWMA, PIN_MOTOR_STBY, PIN_MOTOR_AIN1, PIN_MOTOR_AIN2);
        printf("[Pico SPI Slave] MotorB pins: PWMB=GP%u STBY=GP%u BIN1=GP%u BIN2=GP%u\n",
            PIN_MOTOR_PWMB, PIN_MOTOR_STBY, PIN_MOTOR_BIN1, PIN_MOTOR_BIN2);
    uint32_t last_heartbeat_ms = 0;
    bool led_on = false;
    
    status_led_init();

    memset(reg_table, 0, sizeof(reg_table));
    memset(motors, 0, sizeof(motors));
    motor_gpio_init();
    run_startup_motor_self_test();
    led_init();
    sonic_init();
    lcd_init_1602();
    for (size_t i = 0; i < MOTOR_COUNT; i++) {
        motors[i].status = 0u;
    }
    motors[LOCAL_LOGICAL_MOTOR_A].status = MOTOR_STATUS_AVAILABLE;
    motors[LOCAL_LOGICAL_MOTOR_B].status = MOTOR_STATUS_AVAILABLE;
    memset(rx_frame_raw, 0, sizeof(rx_frame_raw));
    set_default_tx_pattern();
    mark_spi_activity();

    /* Boot-time pin probe: read GP16 (MOSI) and GP18 (SCK) before SPI init.
     * pull-up reads 1 = pin is floating/disconnected.
     * pull-up reads 0 = pin is externally driven low.
     * pull-down reads 1 = pin is externally driven high. */
    {
        bool pu16, pd16, pu18, pd18;
        gpio_init(PIN_MOSI); gpio_set_dir(PIN_MOSI, GPIO_IN);
        gpio_pull_up(PIN_MOSI); sleep_us(50); pu16 = gpio_get(PIN_MOSI);
        gpio_pull_down(PIN_MOSI); sleep_us(50); pd16 = gpio_get(PIN_MOSI);
        gpio_disable_pulls(PIN_MOSI);

        gpio_init(PIN_SCK); gpio_set_dir(PIN_SCK, GPIO_IN);
        gpio_pull_up(PIN_SCK); sleep_us(50); pu18 = gpio_get(PIN_SCK);
        gpio_pull_down(PIN_SCK); sleep_us(50); pd18 = gpio_get(PIN_SCK);
        gpio_disable_pulls(PIN_SCK);

        printf("[PinProbe] GP%u(MOSI): pu=%u pd=%u  GP%u(SCK): pu=%u pd=%u\n",
               PIN_MOSI, pu16, pd16, PIN_SCK, pu18, pd18);
        printf("[PinProbe] MOSI %s  SCK %s\n",
               (pu16 && !pd16) ? "FLOATING(disconnected?)" :
               (!pu16)         ? "driven-LOW-externally" : "driven-HIGH-externally",
               (pu18 && !pd18) ? "FLOATING(disconnected?)" :
               (!pu18)         ? "driven-LOW-externally" : "driven-HIGH-externally");
    }

    spi_init(spi0, 1000 * 1000);
    /* Match the active RW612 register-write path (Mode 0). */
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    spi_set_slave(spi0, true);
    
    /* Ensure RX is ready: drain any stale data from FIFO before we start. */
    {
        spi_hw_t *hw = spi_get_hw(spi0);
        while (spi_is_readable(spi0)) {
            (void)hw->dr;  /* Drain stale RX bytes */
        }
    }

    /* Use hardware SSEL for CS: the SSP slave only fills its RX FIFO when
     * it sees a proper chip-select assertion via GPIO_FUNC_SPI.  A GPIO-IRQ
     * approach keeps CS as a plain GPIO, so the SSP hardware never detects
     * frame boundaries and the RX FIFO stays empty. */
    gpio_set_function(PIN_CS,   GPIO_FUNC_SPI);  /* Hardware SSEL */
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    cs_active_flag = !gpio_get(PIN_CS);
    cs_polled_prev = cs_active_flag;
    spi_slave_set_miso_active(cs_active_flag);
    tx_index = 0u;
    sck_prev_level = gpio_get(PIN_SCK);
    mosi_prev_level = gpio_get(PIN_MOSI);
    for (size_t i = 0u; i < DIAG_FULL_PINS; i++) {
        diag_full_prev[i] = gpio_get((uint)i);
        diag_full_edges[i] = 0u;
    }
    diag_full_bitmask = 0u;
    /* Keep hardware SSEL on GP17 for the PL022 slave, but also latch edges via
     * GPIO IRQ so service_spi_frame() sees CS transitions immediately instead
     * of waiting for the next polling pass. */
    gpio_set_irq_enabled_with_callback(PIN_CS,
                                       GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                                       true,
                                       &cs_gpio_irq_handler);

    /* Brief startup indicator only — keep delay minimal so Pico is ready
     * before the SPI master (RW612) begins its first exchange. */
    status_led_set(true);
    sleep_ms(50);
    status_led_set(false);

    while (true) {
        bool cs_active;
        uint32_t now_us;
        uint32_t now_ms;

#if PICO_MINIMAL_BLOCKING_RX
#error "PICO_MINIMAL_BLOCKING_RX must remain disabled"
#else
        sample_spi_pin_activity();
        sniff_update();
        service_spi_frame(spi0);
        sonic_poll();
        cs_active = cs_is_active();
    #endif

        if (led_on != cs_active) {
            status_led_set(cs_active);
            led_on = cs_active;
        }

        now_us = time_us_32();
        now_ms = to_ms_since_boot(get_absolute_time());

        if (PICO_RUNTIME_LOG_FLUSH &&
            !cs_active && rx_index == 0 && (uint32_t)(now_us - last_spi_activity_us) >= LOG_IDLE_FLUSH_US) {
            dlog_flush_limited(DLOG_FLUSH_BUDGET);
        }

        if (!cs_active && rx_index == 0u) {
            usb_console_poll();
        }

        if (PICO_HEARTBEAT_ENABLE &&
            (now_ms - last_heartbeat_ms) >= 2000u) {
                                /* Dead-man switch: if any motor is enabled but no motor command has
                                 * arrived in the last 15 seconds, force all motors off. This handles
                                 * the case where the RW612 reboots mid-test and never sends disable. */
                                {
                                    bool any_motor_on = any_local_motor_enabled();
                                    bool timed_out = (last_motor_cmd_ms > 0u) &&
                                                     ((now_ms - last_motor_cmd_ms) >= 15000u);
                                    if (any_motor_on && timed_out && !motor_deadman_fired) {
                                        motor_deadman_fired = true;
                                        printf("[Pico DEADMAN] No motor cmd for 15s - forcing all motors OFF\n");
                                        motors[LOCAL_LOGICAL_MOTOR_A].enable = 0u;
                                        motors[LOCAL_LOGICAL_MOTOR_B].enable = 0u;
                                        apply_motor_outputs(LOCAL_LOGICAL_MOTOR_A);
                                        apply_motor_outputs(LOCAL_LOGICAL_MOTOR_B);
                                    }
                                }

                         {
                             spi_hw_t *ssp = spi_get_hw(spi0);
                             printf("[Pico SPI Slave] ssp cr0=0x%08lx cr1=0x%08lx sr=0x%08lx cpsr=0x%08lx\n",
                                    (unsigned long)ssp->cr0,
                                    (unsigned long)ssp->cr1,
                                    (unsigned long)ssp->sr,
                                    (unsigned long)ssp->cpsr);
                         }
                         /* Build per-pin edge count string for active pins only. */
                         char gpe_str[128] = "";
                         int gpe_pos = 0;
                         for (size_t gp = 0u; gp < DIAG_FULL_PINS; gp++) {
                             if (diag_full_edges[gp] > 0u) {
                                 gpe_pos += snprintf(gpe_str + gpe_pos, sizeof(gpe_str) - (size_t)gpe_pos,
                                                     "GP%u:%lu ", (unsigned)gp, (unsigned long)diag_full_edges[gp]);
                             }
                         }
                         if (gpe_pos == 0) { snprintf(gpe_str, sizeof(gpe_str), "(none)"); }
                                                                                                 printf("[Pico SPI Slave] alive fw=%s version=%s cs=%u rx=%u tx=%u rxt=%lu scke=%lu most=%lu gpe_mask=0x%08lx gpe=[%s] f8=%lu f8ok=%lu f8w=%lu f8r=%lu f8mw=%lu f8s=%lu f8sok=%lu lfr=%02x%02x%02x%02x%02x%02x%02x%02x lf8_cmd=0x%02x lf8_addr=0x%04x lf8_val=0x%08lx txd=%02x%02x%02x%02x%02x%02x%02x%02x txw=%02x%02x%02x%02x%02x%02x%02x%02x lrrn=%lu lrra=0x%04x lrrv=0x%08lx lrrd=%02x%02x%02x%02x%02x%02x%02x%02x lrrw=%02x%02x%02x%02x%02x%02x%02x%02x qend=%lu qstart=%lu txfall=%lu txrise=%lu txact=%lu fdb=%lu foth=%lu lflen=%lu csf=%lu csr=%lu men=%lu mspd=%lu last_svc=0x%04x last_cmd=0x%04x last_idx=%u last_val=%ld mA_idx=%u mA_en=%u mA_spd=%ld mB_idx=%u mB_en=%u mB_spd=%ld\n",
                     FW_ID_MAIN,
                   PICO_FIRMWARE_VERSION,
                   cs_active ? 1u : 0u,
                   (unsigned)rx_index,
                   (unsigned)tx_index,
                 (unsigned long)rx_byte_total,
                                 (unsigned long)sck_edge_count,
                                 (unsigned long)mosi_toggle_count,
                                 (unsigned long)diag_full_bitmask,
                                 gpe_str,
                 (unsigned long)frame8_total,
                                 (unsigned long)frame8_decode_ok_count,
                                 (unsigned long)frame8_write_count,
                                 (unsigned long)frame8_read_count,
                                 (unsigned long)frame8_motor_write_count,
                                 (unsigned long)frame8_stream_try_count,
                                 (unsigned long)frame8_stream_ok_count,
                                 (unsigned)last_failed_raw_frame[0],
                                 (unsigned)last_failed_raw_frame[1],
                                 (unsigned)last_failed_raw_frame[2],
                                 (unsigned)last_failed_raw_frame[3],
                                 (unsigned)last_failed_raw_frame[4],
                                 (unsigned)last_failed_raw_frame[5],
                                 (unsigned)last_failed_raw_frame[6],
                                 (unsigned)last_failed_raw_frame[7],
                                 (unsigned)last_frame8_cmd,
                                 (unsigned)last_frame8_addr,
                                 (unsigned long)last_frame8_value,
                                                                 (unsigned)tx_frame_desired[0],
                                                                 (unsigned)tx_frame_desired[1],
                                                                 (unsigned)tx_frame_desired[2],
                                                                 (unsigned)tx_frame_desired[3],
                                                                 (unsigned)tx_frame_desired[4],
                                                                 (unsigned)tx_frame_desired[5],
                                                                 (unsigned)tx_frame_desired[6],
                                                                 (unsigned)tx_frame_desired[7],
                                                                 (unsigned)tx_frame_wire[0],
                                                                 (unsigned)tx_frame_wire[1],
                                                                 (unsigned)tx_frame_wire[2],
                                                                 (unsigned)tx_frame_wire[3],
                                                                 (unsigned)tx_frame_wire[4],
                                                                 (unsigned)tx_frame_wire[5],
                                                                 (unsigned)tx_frame_wire[6],
                                                                 (unsigned)tx_frame_wire[7],
                                                                 (unsigned long)last_read_rsp_count,
                                                                 (unsigned)last_read_rsp_addr,
                                                                 (unsigned long)last_read_rsp_value,
                                                                 (unsigned)last_read_rsp_desired[0],
                                                                 (unsigned)last_read_rsp_desired[1],
                                                                 (unsigned)last_read_rsp_desired[2],
                                                                 (unsigned)last_read_rsp_desired[3],
                                                                 (unsigned)last_read_rsp_desired[4],
                                                                 (unsigned)last_read_rsp_desired[5],
                                                                 (unsigned)last_read_rsp_desired[6],
                                                                 (unsigned)last_read_rsp_desired[7],
                                                                 (unsigned)last_read_rsp_wire[0],
                                                                 (unsigned)last_read_rsp_wire[1],
                                                                 (unsigned)last_read_rsp_wire[2],
                                                                 (unsigned)last_read_rsp_wire[3],
                                                                 (unsigned)last_read_rsp_wire[4],
                                                                 (unsigned)last_read_rsp_wire[5],
                                                                 (unsigned)last_read_rsp_wire[6],
                                                                 (unsigned)last_read_rsp_wire[7],
                                                                 (unsigned long)last_cs_end_preload_count,
                                                                 (unsigned long)last_cs_start_queue_count,
                                                                 (unsigned long)last_tx_index_at_cs_fall,
                                                                 (unsigned long)last_tx_index_at_cs_rise,
                                                                 (unsigned long)last_active_tx_pushes,
                 (unsigned long)frame_dbal_total,
                 (unsigned long)frame_other_total,
                 (unsigned long)last_frame_len,
                   (unsigned long)cs_fall_count,
                   (unsigned long)cs_rise_count,
                   (unsigned long)motor_enable_cmd_count,
                   (unsigned long)motor_speed_cmd_count,
                   (unsigned)last_service_id,
                   (unsigned)last_command_id,
                   (unsigned)last_motor_index,
                   (long)last_motor_value,
                   (unsigned)(LOCAL_LOGICAL_MOTOR_A + 1u),
                   (unsigned)motors[LOCAL_LOGICAL_MOTOR_A].enable,
                   (long)motors[LOCAL_LOGICAL_MOTOR_A].speed_setpoint,
                   (unsigned)(LOCAL_LOGICAL_MOTOR_B + 1u),
                   (unsigned)motors[LOCAL_LOGICAL_MOTOR_B].enable,
                   (long)motors[LOCAL_LOGICAL_MOTOR_B].speed_setpoint);
            last_heartbeat_ms = now_ms;
        }

        tight_loop_contents();
    }
}
