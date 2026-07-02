#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/structs/spi.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef PICO_FIRMWARE_VERSION
#define PICO_FIRMWARE_VERSION "dbal_motor_v1_onehot_v3_isoD_v23_2026-05-28"
#endif

#ifndef PICO_SPEED_TRACE
#define PICO_SPEED_TRACE 0
#endif

/* Non-blocking deferred log buffer: process_rx_frame must never call printf
 * directly — USB CDC printf blocks for milliseconds, which stalls the SPI
 * tight loop and causes TX FIFO underflow. Store messages here instead;
 * main loop flushes them between SPI calls. */
#define DLOG_ENTRIES 128u
#define DLOG_MSG_LEN 96u
#ifndef PICO_LOG_IDLE_FLUSH_US
#define PICO_LOG_IDLE_FLUSH_US 5000u
#endif
#define DLOG_FLUSH_BUDGET 4u
#ifndef PICO_RUNTIME_LOG_FLUSH
#define PICO_RUNTIME_LOG_FLUSH 1
#endif
#ifndef PICO_HEARTBEAT_ENABLE
#define PICO_HEARTBEAT_ENABLE 1
#endif
#ifndef PICO_STARTUP_SELF_TEST
#define PICO_STARTUP_SELF_TEST 0
#endif
#ifndef PICO_USB_CONSOLE_ENABLE
#define PICO_USB_CONSOLE_ENABLE 1
#endif
#ifndef PICO_PREFER_CS_START_READ_REARM
#if PICO_NODE_SLOT == 2
#define PICO_PREFER_CS_START_READ_REARM 1
#else
#define PICO_PREFER_CS_START_READ_REARM 0
#endif
#endif
#ifndef PICO_FORCE_FIXED_READ_VALUE
#define PICO_FORCE_FIXED_READ_VALUE 0u
#endif
#ifndef PICO_DIAG_DEFAULT_TX_PATTERN
#define PICO_DIAG_DEFAULT_TX_PATTERN 0
#endif
#ifndef PICO_FORCE_FULL_TX_PRELOAD
#define PICO_FORCE_FULL_TX_PRELOAD 0
#endif
#ifndef PICO_SPI_CPHA_SETTING
#define PICO_SPI_CPHA_SETTING SPI_CPHA_0
#endif
#ifndef PICO_TX_TRANSFORM_SETTING
#define PICO_TX_TRANSFORM_SETTING 1
#endif
#ifndef PICO_EXACT8_AUTO_TRANSFORM
#define PICO_EXACT8_AUTO_TRANSFORM 0
#endif
#define PICO_DIAG_FORCE_MOTOR0_ONLY 0
/* Build this firmware separately for each Pico side:
 * - PICO_NODE_SLOT=1 => owns logical motors 0 (A) and 2 (B)
 * - PICO_NODE_SLOT=2 => owns logical motors 1 (A) and 3 (B)
 */
#ifndef PICO_NODE_SLOT
#define PICO_NODE_SLOT 1
#endif
#if PICO_NODE_SLOT == 2
#define CS_END_DRAIN_TIMEOUT_US 500u
#define CS_END_DRAIN_IDLE_US 2u
#else
#define CS_END_DRAIN_TIMEOUT_US 200u
#define CS_END_DRAIN_IDLE_US 1u
#endif
#define RX_ACCUM_GAP_RESET_US 3000u
typedef struct { char msg[DLOG_MSG_LEN]; } dlog_entry_t;
static dlog_entry_t dlog_buf[DLOG_ENTRIES];
static unsigned int dlog_head = 0u;
static unsigned int dlog_tail = 0u;
static uint32_t last_spi_activity_us = 0u;
static char usb_cmd_buf[16];
static size_t usb_cmd_len = 0u;

#define SPEED_TRACE_ENTRIES 64u
typedef struct {
    char kind;
    uint16_t addr;
    uint32_t value;
    uint32_t aux;
} speed_trace_entry_t;

static speed_trace_entry_t speed_trace_buf[SPEED_TRACE_ENTRIES];
static unsigned int speed_trace_head = 0u;
static unsigned int speed_trace_tail = 0u;
static volatile uint32_t trace_last_write_addr = 0u;
static volatile uint32_t trace_last_write_value = 0u;
static volatile uint32_t trace_last_read_addr = 0u;
static volatile uint32_t trace_last_read_value = 0u;
static volatile uint32_t trace_last_prep_addr = 0u;
static volatile uint32_t trace_last_prep_value = 0u;

static void dlog(const char *fmt, ...) {
#if PICO_SPEED_TRACE
    (void)fmt;
    return;
#else
    unsigned int next = (dlog_head + 1u) % DLOG_ENTRIES;
    if (next == dlog_tail) return;  /* buffer full, drop entry */
    va_list args;
    va_start(args, fmt);
    vsnprintf(dlog_buf[dlog_head].msg, DLOG_MSG_LEN - 1u, fmt, args);
    dlog_buf[dlog_head].msg[DLOG_MSG_LEN - 1u] = '\0';
    va_end(args);
    dlog_head = next;
#endif
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

static void speed_trace_push(char kind, uint16_t addr, uint32_t value, uint32_t aux)
{
#if PICO_SPEED_TRACE
    unsigned int next = (speed_trace_head + 1u) % SPEED_TRACE_ENTRIES;

    if (next == speed_trace_tail) {
        return;
    }

    speed_trace_buf[speed_trace_head].kind = kind;
    speed_trace_buf[speed_trace_head].addr = addr;
    speed_trace_buf[speed_trace_head].value = value;
    speed_trace_buf[speed_trace_head].aux = aux;
    speed_trace_head = next;
#else
    (void)kind;
    (void)addr;
    (void)value;
    (void)aux;
#endif
}

static void speed_trace_flush_limited(unsigned int budget)
{
#if PICO_SPEED_TRACE && PICO_RUNTIME_LOG_FLUSH
    while (speed_trace_tail != speed_trace_head && budget > 0u) {
        const speed_trace_entry_t *entry = &speed_trace_buf[speed_trace_tail];

        switch (entry->kind) {
        case 'W':
            printf("[PICO] W %04x=%08lx raw=%08lx\n",
                   (unsigned)entry->addr,
                   (unsigned long)entry->value,
                   (unsigned long)entry->aux);
            break;
        case 'R':
            printf("[PICO] R %04x=%08lx\n",
                   (unsigned)entry->addr,
                   (unsigned long)entry->value);
            break;
        case 'P':
            printf("[PICO] P %04x=%08lx same=%lu\n",
                   (unsigned)entry->addr,
                   (unsigned long)entry->value,
                   (unsigned long)entry->aux);
            break;
        default:
            break;
        }

        speed_trace_tail = (speed_trace_tail + 1u) % SPEED_TRACE_ENTRIES;
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

#define TRACE_REG_BASE            0x5120u
#define TRACE_REG_LAST_WRITE_ADDR 0x00u
#define TRACE_REG_LAST_WRITE_VAL  0x04u
#define TRACE_REG_LAST_READ_ADDR  0x08u
#define TRACE_REG_LAST_READ_VAL   0x0Cu
#define TRACE_REG_LAST_PREP_ADDR  0x10u
#define TRACE_REG_LAST_PREP_VAL   0x14u
#define TRACE_REG_COUNTS0         0x18u
#define TRACE_REG_COUNTS1         0x1Cu

#define SONIC_SENSOR_COUNT 1u
#define SONIC_TRIGGER_PERIOD_US 60000u
#define SONIC_ECHO_TIMEOUT_US   30000u
#define SONIC_MIN_DISTANCE_MM     20u
#define SONIC_MAX_DISTANCE_MM   4000u
#define SONIC_TIMEOUT_HOLD_CYCLES 2u
#define SONIC_LCD_REFRESH_MS    150u

#ifndef PICO_LOCAL_SONIC_LCD_REFRESH
#define PICO_LOCAL_SONIC_LCD_REFRESH 1
#endif

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
static volatile uint32_t last_read_decode_motor1_count = 0u;
static volatile uint32_t last_read_decode_motor3_count = 0u;
static volatile uint32_t pending_rsp_replace_count = 0u;
static volatile uint32_t pending_rsp_replace_diff_addr_count = 0u;
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
static bool tx_read_response_retire_on_cs_end = false;
static bool tx_force_rearm_on_next_cs_start = false;
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
static volatile uint32_t frame7_read_promote_count = 0u;
static volatile uint32_t frame7_speed_write_promote_count = 0u;
static volatile uint32_t frame7_speed_write_motor1_count = 0u;
static volatile uint32_t frame7_speed_write_motor3_count = 0u;
static volatile uint32_t frame7_write_fifo_count = 0u;
static volatile uint32_t frame7_write_sniff_count = 0u;
static volatile uint32_t frame7_write_tail_drop_count = 0u;
static volatile uint32_t frame7_write_decode_count = 0u;
static volatile uint32_t partial_len1_count = 0u;
static volatile uint32_t partial_len2_count = 0u;
static volatile uint32_t partial_len3_count = 0u;
static volatile uint32_t partial_len4_count = 0u;
static volatile uint32_t partial_len5_count = 0u;
static volatile uint32_t partial_len6_count = 0u;
static volatile uint32_t partial_len7_count = 0u;
static volatile uint32_t partial_len2_4000_count = 0u;
static volatile uint32_t partial_len2_4050_count = 0u;
static volatile uint32_t partial_len2_6000_count = 0u;
static volatile uint32_t partial_len2_6050_count = 0u;
static volatile uint32_t partial_len2_other_count = 0u;
static volatile uint32_t stale_rx1_drop_count = 0u;
static volatile uint32_t stale_rx1_00_count = 0u;
static volatile uint32_t stale_rx1_40_count = 0u;
static volatile uint32_t stale_rx1_50_count = 0u;
static volatile uint32_t stale_rx1_60_count = 0u;
static volatile uint32_t stale_rx1_01_count = 0u;
static volatile uint32_t stale_rx1_a0_count = 0u;
static volatile uint32_t stale_rx1_other_count = 0u;
static volatile uint16_t last_write7_addr = 0u;
static volatile uint32_t last_write7_value = 0u;
static uint8_t last_write7_fifo_raw[FRAME_SIZE - 1u] = {0};
static uint8_t last_write7_sniff_raw[FRAME_SIZE - 1u] = {0};
static uint8_t last_partial_frame_raw[FRAME_SIZE - 1u] = {0};
static uint8_t last_partial_len2_raw[2] = {0};
static uint8_t last_partial_len2_other_raw[2] = {0};
static volatile uint8_t last_write7_tail_byte = 0u;
static volatile uint8_t last_stale_rx1_byte = 0u;
static volatile uint8_t last_partial_frame_len = 0u;
static volatile uint16_t last_write7_decode_addr = 0u;
static volatile uint32_t last_write7_decode_value = 0u;
static volatile uint8_t last_write7_decode_rotation = 0u;
static volatile uint8_t last_write7_decode_transform = 0u;
static bool pending_write7_tail_byte = false;
static uint8_t rx_stream_window[FRAME_SIZE] = {0};
static uint32_t rx_stream_fill = 0u;
static volatile uint32_t rx_stream_byte_count = 0u;
static volatile uint32_t frame8_fail_dump_count = 0u;
static uint8_t last_failed_raw_frame[FRAME_SIZE] = {0};
static uint32_t rx_last_byte_us = 0u;
static uint8_t sniff_frame[DBAL_MAX_FRAME_SIZE] = {0};
static uint8_t sniff_len = 0u;
static uint8_t sniff_byte = 0u;
static uint8_t sniff_bit_count = 0u;
static bool sniff_prev_cs_active = false;
static bool sniff_prev_sck_level = false;
static volatile uint16_t last_service_id = 0u;
static volatile uint16_t last_command_id = 0u;
static volatile uint8_t last_motor_index = 0u;
static volatile int32_t last_motor_value = 0;
static volatile uint32_t speed_trace_seq = 0u;
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

static bit_transform_t tx_transform = (bit_transform_t)PICO_TX_TRANSFORM_SETTING;

static bool process_rx_frame(void);
static bool process_dbal_frame(size_t count);
static void lcd_print(uint8_t row, uint8_t col, const char *text, uint8_t text_len);
static bool addr_is_valid(uint16_t addr);
static bool raw_frame_is_safe_read_near_match(const uint8_t *raw_frame, size_t len);
static bool raw_frame_is_safe_motor_speed_write_near_match(const uint8_t *raw_frame, size_t len);
static bool decode_write7_candidate(const uint8_t *raw_frame,
                                    uint16_t *addr,
                                    uint32_t *value,
                                    bit_transform_t *transform,
                                    uint8_t *rotation);
static bool decode_frame_with_transform(const uint8_t *raw_frame, uint8_t *decoded_frame, bit_transform_t transform);
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

static void retire_pending_read_response(void)
{
    dlog("[PICO] RETIRE_READ addr=0x%04x val=0x%08lx txi=%u prequeued=%u retire_on_end=%u bytes=%02x %02x %02x %02x %02x %02x %02x %02x\n",
         (unsigned)last_read_rsp_addr,
         (unsigned long)last_read_rsp_value,
         (unsigned)tx_index,
         tx_frame_prequeued ? 1u : 0u,
         tx_read_response_retire_on_cs_end ? 1u : 0u,
         tx_frame_wire[0], tx_frame_wire[1], tx_frame_wire[2], tx_frame_wire[3],
         tx_frame_wire[4], tx_frame_wire[5], tx_frame_wire[6], tx_frame_wire[7]);
    tx_read_response_pending = false;
    tx_read_response_retire_on_cs_end = false;
    tx_frame_prequeued = false;
    set_default_tx_pattern();
    tx_index = 0u;
}

static bool is_motor_speed_addr(uint16_t addr)
{
    return (addr == (MOTOR_REG_BASE + (1u * MOTOR_REG_STRIDE) + MOTOR_REG_SPEED_OFFSET)) ||
           (addr == (MOTOR_REG_BASE + (3u * MOTOR_REG_STRIDE) + MOTOR_REG_SPEED_OFFSET));
}

static void sniff_process_frame(void)
{
    last_frame_len = sniff_len;

    bool missing_tail_byte = false;

    /* RW612 8-byte register traffic should be owned by the cs_end RX-FIFO
     * decode path. On Pico2 bench, the PL022 path can repeatedly retain only
     * one stale byte while the GPIO sniffer sees complete 8-byte command
     * frames. Promote only exact 8-byte sniff captures through the same
     * validator/decoder; 7-byte near-frames remain diagnostic-only because
     * they can carry shifted data and corrupt register state. */
    if (sniff_len == FRAME_SIZE) {
        dlog("[PICO] SNIFF_SHORT len=%u raw=%02x %02x %02x %02x %02x %02x %02x %02x\n",
             (unsigned)sniff_len,
             sniff_frame[0], sniff_frame[1], sniff_frame[2], sniff_frame[3],
             sniff_frame[4], sniff_frame[5], sniff_frame[6], sniff_frame[7]);
        memcpy(rx_frame_raw, sniff_frame, FRAME_SIZE);
        frame8_total++;
        if (process_rx_frame()) {
            frame8_stream_ok_count++;
        } else {
            frame_other_total++;
        }
        return;
    }

    if (raw_frame_is_safe_read_near_match(sniff_frame, sniff_len)) {
        dlog("[PICO] SNIFF_READ7 pad0 raw=%02x %02x %02x %02x %02x %02x %02x\n",
             sniff_frame[0], sniff_frame[1], sniff_frame[2], sniff_frame[3],
             sniff_frame[4], sniff_frame[5], sniff_frame[6]);
        memcpy(rx_frame_raw, sniff_frame, sniff_len);
        rx_frame_raw[FRAME_SIZE - 1u] = 0x00u;
        frame8_total++;
        frame7_read_promote_count++;
        if (process_rx_frame()) {
            frame8_stream_ok_count++;
        } else {
            frame_other_total++;
        }
        return;
    }

    if (raw_frame_is_safe_motor_speed_write_near_match(sniff_frame, sniff_len)) {
        uint16_t addr = (uint16_t)(((uint16_t)sniff_frame[1] << 8) | sniff_frame[2]);
        uint32_t value = (uint32_t)sniff_frame[4] |
                         ((uint32_t)sniff_frame[5] << 8) |
                         ((uint32_t)sniff_frame[6] << 16);
        dlog("[PICO] SNIFF_WRITE7 pad0 raw=%02x %02x %02x %02x %02x %02x %02x\n",
             sniff_frame[0], sniff_frame[1], sniff_frame[2], sniff_frame[3],
             sniff_frame[4], sniff_frame[5], sniff_frame[6]);
        frame7_speed_write_promote_count++;
        frame7_write_sniff_count++;
        last_write7_addr = addr;
        last_write7_value = value;
        memcpy(last_write7_sniff_raw, sniff_frame, FRAME_SIZE - 1u);
        if (addr == (MOTOR_REG_BASE + (1u * MOTOR_REG_STRIDE) + MOTOR_REG_SPEED_OFFSET)) {
            frame7_speed_write_motor1_count++;
        } else if (addr == (MOTOR_REG_BASE + (3u * MOTOR_REG_STRIDE) + MOTOR_REG_SPEED_OFFSET)) {
            frame7_speed_write_motor3_count++;
        }
        return;
    }

    if (sniff_len == (FRAME_SIZE - 1u)) {
        uint16_t decoded_addr = 0u;
        uint32_t decoded_value = 0u;
        bit_transform_t decoded_transform = TRANSFORM_IDENTITY;
        uint8_t decoded_rotation = 0u;

        if (decode_write7_candidate(sniff_frame,
                                    &decoded_addr,
                                    &decoded_value,
                                    &decoded_transform,
                                    &decoded_rotation)) {
            frame7_write_decode_count++;
            last_write7_decode_addr = decoded_addr;
            last_write7_decode_value = decoded_value;
            last_write7_decode_rotation = decoded_rotation;
            last_write7_decode_transform = (uint8_t)decoded_transform;
            dlog("[PICO] SNIFF_WRITE7_DECODE t=%u r=%u raw=%02x %02x %02x %02x %02x %02x %02x -> addr=0x%04x val=0x%08lx\n",
                 (unsigned)decoded_transform,
                 (unsigned)decoded_rotation,
                 sniff_frame[0], sniff_frame[1], sniff_frame[2], sniff_frame[3],
                 sniff_frame[4], sniff_frame[5], sniff_frame[6],
                 (unsigned)decoded_addr,
                 (unsigned long)decoded_value);
        }
    }

    if (sniff_len <= FRAME_SIZE) {
        if (sniff_len >= (FRAME_SIZE - 1u)) {
            dlog("[PICO] SNIFF_SHORT len=%u raw=%02x %02x %02x %02x %02x %02x %02x %02x\n",
                 (unsigned)sniff_len,
                 sniff_frame[0], sniff_frame[1], sniff_frame[2], sniff_frame[3],
                 sniff_frame[4], sniff_frame[5], sniff_frame[6], sniff_frame[7]);
        }
        return;
    }

    if (sniff_len > FRAME_SIZE) {
        memcpy(rx_frame_raw, sniff_frame, sniff_len);
        if (!process_dbal_frame(sniff_len)) {
            frame_other_total++;
            memcpy(last_failed_raw_frame, sniff_frame, FRAME_SIZE);
        }
        return;
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
            if (sniff_len < DBAL_MAX_FRAME_SIZE) {
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
static volatile uint32_t sonic_last_trigger_us = 0u;
static volatile uint32_t sonic_echo_start_us = 0u;
static volatile uint32_t sonic_echo_pulse_us = 0u;
static volatile bool sonic_echo_ready = false;
static uint32_t sonic_distance_mm = 0u;
static uint32_t sonic_last_good_distance_mm = 0u;
static uint8_t sonic_timeout_streak = 0u;
static uint32_t sonic_lcd_last_refresh_ms = 0u;
static uint32_t sonic_lcd_last_distance_mm = UINT32_MAX;

static void sonic_lcd_refresh(bool force)
{
#if PICO_LOCAL_SONIC_LCD_REFRESH && (PICO_NODE_SLOT == 1)
    char line_buf[17];
    uint32_t now_ms = to_ms_since_boot(get_absolute_time());
    uint32_t elapsed_ms = (uint32_t)(now_ms - sonic_lcd_last_refresh_ms);

    if (!force && elapsed_ms < SONIC_LCD_REFRESH_MS) {
        return;
    }

    if (!force &&
        sonic_distance_mm == sonic_lcd_last_distance_mm &&
        elapsed_ms < (SONIC_LCD_REFRESH_MS * 4u)) {
        return;
    }

    snprintf(line_buf, sizeof(line_buf), "%4lu mm        ", (unsigned long)sonic_distance_mm);
    lcd_print(0u, 0u, "DISTANCE        ", 16u);
    lcd_print(1u, 0u, line_buf, (uint8_t)strlen(line_buf));
    sonic_lcd_last_refresh_ms = now_ms;
    sonic_lcd_last_distance_mm = sonic_distance_mm;
#else
    (void)force;
#endif
}

static void sonic_publish_distance(uint32_t distance_mm)
{
    sonic_distance_mm = distance_mm;
    reg_write((uint16_t)(SONIC_REG_BASE + SONIC_REG_DISTANCE_OFFSET), sonic_distance_mm);
    led_update_from_distance(sonic_distance_mm);
    sonic_lcd_refresh(false);
}

static bool sonic_distance_is_plausible(uint32_t distance_mm)
{
    return (distance_mm >= SONIC_MIN_DISTANCE_MM) &&
           (distance_mm <= SONIC_MAX_DISTANCE_MM);
}

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
#if PICO_LOCAL_SONIC_LCD_REFRESH && (PICO_NODE_SLOT == 1)
    lcd_print(0u, 0u, "DISTANCE        ", 16u);
    lcd_print(1u, 0u, "starting...      ", 16u);
#else
    lcd_print(0u, 0u, "DBAL SPI READY", 14u);
#endif
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
    sonic_echo_start_us = 0u;
    sonic_echo_pulse_us = 0u;
    sonic_echo_ready = false;
    sonic_distance_mm = 0u;
    sonic_last_good_distance_mm = 0u;
    sonic_timeout_streak = 0u;
    sonic_lcd_last_refresh_ms = 0u;
    sonic_lcd_last_distance_mm = UINT32_MAX;
    sonic_publish_distance(sonic_distance_mm);
    sonic_lcd_refresh(true);
}

static void sonic_poll(void)
{
    uint32_t now_us = time_us_32();

    switch (sonic_state) {
        case SONIC_IDLE:
            if ((uint32_t)(now_us - sonic_last_trigger_us) >= SONIC_TRIGGER_PERIOD_US) {
                gpio_put(PIN_SONIC_TRIG, 1);
                busy_wait_us_32(10u);
                gpio_put(PIN_SONIC_TRIG, 0);
                sonic_last_trigger_us = time_us_32();
                sonic_echo_ready = false;
                sonic_state = SONIC_WAIT_RISE;
            }
            break;

        case SONIC_WAIT_RISE:
            if ((uint32_t)(now_us - sonic_last_trigger_us) >= SONIC_ECHO_TIMEOUT_US) {
                sonic_timeout_streak++;
                if (sonic_timeout_streak >= SONIC_TIMEOUT_HOLD_CYCLES) {
                    sonic_last_good_distance_mm = 0u;
                    sonic_publish_distance(0u);
                } else {
                    sonic_publish_distance(sonic_last_good_distance_mm);
                }
                sonic_state = SONIC_IDLE;
            }
            break;

        case SONIC_WAIT_FALL:
            if (sonic_echo_ready) {
                uint32_t pulse_us = sonic_echo_pulse_us;
                uint32_t distance_mm = (pulse_us * 343u) / 2000u;

                sonic_echo_ready = false;
                if (sonic_distance_is_plausible(distance_mm)) {
                    sonic_last_good_distance_mm = distance_mm;
                    sonic_timeout_streak = 0u;
                    sonic_publish_distance(distance_mm);
                } else {
                    sonic_publish_distance(sonic_last_good_distance_mm);
                }
                sonic_state = SONIC_IDLE;
            } else if ((uint32_t)(now_us - sonic_echo_start_us) >= SONIC_ECHO_TIMEOUT_US) {
                sonic_timeout_streak++;
                if (sonic_timeout_streak >= SONIC_TIMEOUT_HOLD_CYCLES) {
                    sonic_last_good_distance_mm = 0u;
                    sonic_publish_distance(0u);
                } else {
                    sonic_publish_distance(sonic_last_good_distance_mm);
                }
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

    if (addr >= TRACE_REG_BASE &&
        addr < (TRACE_REG_BASE + 0x20u)) {
        return true;
    }

    return false;
}

static uint32_t trace_reg_read(uint16_t addr)
{
    switch ((uint16_t)(addr - TRACE_REG_BASE)) {
        case TRACE_REG_LAST_WRITE_ADDR:
            return trace_last_write_addr;
        case TRACE_REG_LAST_WRITE_VAL:
            return trace_last_write_value;
        case TRACE_REG_LAST_READ_ADDR:
            return trace_last_read_addr;
        case TRACE_REG_LAST_READ_VAL:
            return trace_last_read_value;
        case TRACE_REG_LAST_PREP_ADDR:
            return trace_last_prep_addr;
        case TRACE_REG_LAST_PREP_VAL:
            return trace_last_prep_value;
        case TRACE_REG_COUNTS0:
            return (frame8_write_count & 0xFFFFu) | ((frame8_read_count & 0xFFFFu) << 16);
        case TRACE_REG_COUNTS1:
            return (last_read_rsp_count & 0xFFFFu) | ((frame8_decode_ok_count & 0xFFFFu) << 16);
        default:
            return 0u;
    }
}

static bool decoded_frame_is_valid(const uint8_t *decoded_frame)
{
    uint8_t cmd = decoded_frame[0];
    uint8_t len_words = decoded_frame[3];
    uint16_t addr = ((uint16_t)decoded_frame[1] << 8) | decoded_frame[2];

    if (cmd != DBUS_CMD_READ &&
        cmd != DBUS_CMD_READ_SHIFTED &&
        cmd != DBUS_CMD_WRITE) {
        return false;
    }

    if (!addr_is_valid(addr)) {
        return false;
    }

    if (len_words != 1u) {
        return false;
    }

    return true;
}

static bool raw_frame_is_safe_read_near_match(const uint8_t *raw_frame, size_t len)
{
    uint16_t addr;

    if (len != (FRAME_SIZE - 1u)) {
        return false;
    }

    if (raw_frame[0] != DBUS_CMD_READ) {
        return false;
    }

    addr = (uint16_t)(((uint16_t)raw_frame[1] << 8) | raw_frame[2]);
    if (!addr_is_valid(addr)) {
        return false;
    }

    if (raw_frame[3] != 0x01u) {
        return false;
    }

    return raw_frame[4] == 0x00u &&
           raw_frame[5] == 0x00u &&
           raw_frame[6] == 0x00u;
}

static bool raw_frame_is_safe_motor_speed_write_near_match(const uint8_t *raw_frame, size_t len)
{
    uint16_t addr;

    if (len != (FRAME_SIZE - 1u)) {
        return false;
    }

    if (raw_frame[0] != DBUS_CMD_WRITE) {
        return false;
    }

    addr = (uint16_t)(((uint16_t)raw_frame[1] << 8) | raw_frame[2]);
    if (!is_motor_speed_addr(addr)) {
        return false;
    }

    if (raw_frame[3] != 0x01u) {
        return false;
    }

    return true;
}

static bool decode_write7_candidate(const uint8_t *raw_frame,
                                    uint16_t *addr,
                                    uint32_t *value,
                                    bit_transform_t *transform,
                                    uint8_t *rotation)
{
    uint8_t padded[FRAME_SIZE] = {0};
    uint8_t rotated[FRAME_SIZE];
    uint8_t decoded[FRAME_SIZE];
    const bit_transform_t transforms[] = {
        TRANSFORM_IDENTITY,
        TRANSFORM_ROL1,
        TRANSFORM_ROR1,
        TRANSFORM_SERIAL_ROL1,
        TRANSFORM_SERIAL_ROR1,
    };

    memcpy(padded, raw_frame, FRAME_SIZE - 1u);

    for (uint8_t raw_rotation = 0u; raw_rotation < FRAME_SIZE; raw_rotation++) {
        for (size_t i = 0u; i < FRAME_SIZE; i++) {
            rotated[i] = padded[(i + raw_rotation) % FRAME_SIZE];
        }

        for (size_t transform_index = 0u; transform_index < (sizeof(transforms) / sizeof(transforms[0])); transform_index++) {
            bit_transform_t current_transform = transforms[transform_index];

            if (!decode_frame_with_transform(rotated, decoded, current_transform)) {
                continue;
            }

            if (decoded[0] != DBUS_CMD_WRITE) {
                continue;
            }

            *addr = (uint16_t)(((uint16_t)decoded[1] << 8) | decoded[2]);
            if (!is_motor_speed_addr(*addr)) {
                continue;
            }

            *value = (uint32_t)decoded[4] |
                     ((uint32_t)decoded[5] << 8) |
                     ((uint32_t)decoded[6] << 16) |
                     ((uint32_t)decoded[7] << 24);
            *transform = current_transform;
            *rotation = raw_rotation;
            return true;
        }
    }

    return false;
}

static bool classify_failed_read8_candidate(const uint8_t *raw_frame,
                                            uint16_t *addr,
                                            bit_transform_t *transform,
                                            uint8_t *rotation,
                                            uint8_t *cmd,
                                            uint8_t *len_words)
{
    uint8_t rotated[FRAME_SIZE];
    uint8_t decoded[FRAME_SIZE];
    const bit_transform_t transforms[] = {
        TRANSFORM_IDENTITY,
        TRANSFORM_ROL1,
        TRANSFORM_ROR1,
        TRANSFORM_SERIAL_ROL1,
        TRANSFORM_SERIAL_ROR1,
    };

    for (uint8_t raw_rotation = 0u; raw_rotation < FRAME_SIZE; raw_rotation++) {
        for (size_t i = 0u; i < FRAME_SIZE; i++) {
            rotated[i] = raw_frame[(i + raw_rotation) % FRAME_SIZE];
        }

        for (size_t transform_index = 0u; transform_index < (sizeof(transforms) / sizeof(transforms[0])); transform_index++) {
            bit_transform_t current_transform = transforms[transform_index];

            if (current_transform == TRANSFORM_SERIAL_ROR1) {
                decoded[0] = (uint8_t)(rotated[0] >> 1);
                for (size_t i = 1; i < FRAME_SIZE; i++) {
                    decoded[i] = (uint8_t)((rotated[i] >> 1) | ((rotated[i - 1] & 0x01u) << 7));
                }
            } else if (current_transform == TRANSFORM_SERIAL_ROL1) {
                for (size_t i = 0; i < (FRAME_SIZE - 1u); i++) {
                    decoded[i] = (uint8_t)((rotated[i] << 1) | (rotated[i + 1] >> 7));
                }
                decoded[FRAME_SIZE - 1u] = (uint8_t)(rotated[FRAME_SIZE - 1u] << 1);
            } else {
                for (size_t i = 0; i < FRAME_SIZE; i++) {
                    decoded[i] = apply_transform(rotated[i], current_transform);
                }
            }

            *cmd = (uint8_t)(decoded[0] & 0x60u);
            *addr = (uint16_t)(((uint16_t)decoded[1] << 8) | decoded[2]);
            *len_words = decoded[3];

            if (*cmd != DBUS_CMD_READ && *cmd != DBUS_CMD_READ_SHIFTED) {
                continue;
            }

            if (!addr_is_valid(*addr)) {
                continue;
            }

            if (*len_words != 1u) {
                continue;
            }

            *transform = current_transform;
            *rotation = raw_rotation;
            return true;
        }
    }

    return false;
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

#if PICO_EXACT8_AUTO_TRANSFORM
    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_SERIAL_ROR1)) {
        *detected = TRANSFORM_SERIAL_ROR1;
        return true;
    }

    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_ROR1)) {
        *detected = TRANSFORM_ROR1;
        return true;
    }

    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_SERIAL_ROL1)) {
        *detected = TRANSFORM_SERIAL_ROL1;
        return true;
    }

    if (decode_frame_with_transform(raw_frame, decoded_frame, TRANSFORM_ROL1)) {
        *detected = TRANSFORM_ROL1;
        return true;
    }
#endif

    return false;
}

static uint32_t reg_read(uint16_t addr)
{
    if (addr >= TRACE_REG_BASE &&
        addr < (TRACE_REG_BASE + 0x20u)) {
        return trace_reg_read(addr);
    }

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

static void prepare_tx_frame_wire(void)
{
    switch (tx_transform) {
        case TRANSFORM_IDENTITY:
            prepare_tx_frame_identity();
            break;
        case TRANSFORM_SERIAL_ROR1:
            tx_frame_wire[0] = (uint8_t)(tx_frame_desired[0] >> 1u);
            for (size_t i = 1u; i < FRAME_SIZE; i++) {
                tx_frame_wire[i] = (uint8_t)((tx_frame_desired[i] >> 1u) |
                                             ((tx_frame_desired[i - 1u] & 0x01u) << 7u));
            }
            break;
        case TRANSFORM_SERIAL_ROL1:
            for (size_t i = 0u; i < (FRAME_SIZE - 1u); i++) {
                tx_frame_wire[i] = (uint8_t)((tx_frame_desired[i] << 1u) |
                                             (tx_frame_desired[i + 1u] >> 7u));
            }
            tx_frame_wire[FRAME_SIZE - 1u] = (uint8_t)(tx_frame_desired[FRAME_SIZE - 1u] << 1u);
            break;
        case TRANSFORM_ROL1:
        case TRANSFORM_ROR1:
            for (size_t i = 0u; i < FRAME_SIZE; i++) {
                tx_frame_wire[i] = apply_transform(tx_frame_desired[i], tx_transform);
            }
            break;
        default:
            prepare_tx_frame_identity();
            break;
    }
}

static void prepare_tx_frame_identity(void)
{
    memcpy(tx_frame_wire, tx_frame_desired, FRAME_SIZE);
}

static void set_default_tx_pattern(void)
{
#if PICO_DIAG_DEFAULT_TX_PATTERN
    static const uint8_t diag_pattern[FRAME_SIZE] = {
        0xA6u, 0x59u, 0xC3u, 0x3Cu, 0xF0u, 0x0Fu, 0x96u, 0x69u,
    };

    memcpy(tx_frame_desired, diag_pattern, FRAME_SIZE);
#else
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        tx_frame_desired[i] = 0xFF;
    }
#endif
    prepare_tx_frame_wire();
}

static void prepare_read_response_frame(uint16_t addr, uint32_t value)
{
    uint8_t next_frame[FRAME_SIZE] = {0};
    bool same_pending_response;
    bool replacing_pending_response;
    bool replacing_different_addr;

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
    replacing_pending_response = tx_read_response_pending && !same_pending_response;
    replacing_different_addr = tx_read_response_pending &&
                               (((uint16_t)last_read_rsp_addr) != addr);

    if (replacing_pending_response) {
        pending_rsp_replace_count++;
    }
    if (replacing_different_addr) {
        pending_rsp_replace_diff_addr_count++;
    }

    memcpy(tx_frame_desired, next_frame, FRAME_SIZE);
    prepare_tx_frame_wire();
    memcpy(last_read_rsp_desired, tx_frame_desired, FRAME_SIZE);
    memcpy(last_read_rsp_wire, tx_frame_wire, FRAME_SIZE);
    tx_read_response_pending = true;
    tx_read_response_retire_on_cs_end = false;

#if PICO_SPEED_TRACE
    if (is_motor_speed_addr(addr)) {
        trace_last_prep_addr = addr;
        trace_last_prep_value = value;
        speed_trace_seq++;
        speed_trace_push('P', addr, value, same_pending_response ? 1u : 0u);
    }
#else
    if (is_motor_speed_addr(addr)) {
        trace_last_prep_addr = addr;
        trace_last_prep_value = value;
    }
        dlog("[PICO] PREP_READ addr=0x%04x val=0x%08lx same=%u repl=%u diffaddr=%u txi=%u bytes=%02x %02x %02x %02x %02x %02x %02x %02x\n",
         (unsigned)addr,
         (unsigned long)value,
         same_pending_response ? 1u : 0u,
            replacing_pending_response ? 1u : 0u,
            replacing_different_addr ? 1u : 0u,
         (unsigned)tx_index,
         tx_frame_wire[0], tx_frame_wire[1], tx_frame_wire[2], tx_frame_wire[3],
         tx_frame_wire[4], tx_frame_wire[5], tx_frame_wire[6], tx_frame_wire[7]);
#endif

    if (!same_pending_response) {
        tx_index = 0u;
        tx_frame_prequeued = false;
        tx_force_rearm_on_next_cs_start = PICO_PREFER_CS_START_READ_REARM ? true : false;
        dlog("[PICO] PREP_READ_ARM addr=0x%04x force_rearm=%u pending=%u bytes=%02x %02x %02x %02x %02x %02x %02x %02x\n",
             (unsigned)addr,
             tx_force_rearm_on_next_cs_start ? 1u : 0u,
             tx_read_response_pending ? 1u : 0u,
             tx_frame_wire[0], tx_frame_wire[1], tx_frame_wire[2], tx_frame_wire[3],
             tx_frame_wire[4], tx_frame_wire[5], tx_frame_wire[6], tx_frame_wire[7]);
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
                    dlog("[DBAL] lcd clear\n");
                    lcd_clear();
                    return true;
                }

                if (cmd_id == DBAL_LCD_CMD_SET_CURSOR) {
                    if (data_len < 3u) {
                        continue;
                    }
                    dlog("[DBAL] lcd cursor row=%u col=%u\n",
                         (unsigned)decoded[data_start + 1u],
                         (unsigned)decoded[data_start + 2u]);
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
                        dlog("[DBAL] lcd print row=%u col=%u len=%u text='%.*s'\n",
                             (unsigned)row,
                             (unsigned)col,
                             (unsigned)text_len,
                             (int)text_len,
                             (const char *)&decoded[data_start + 3u]);
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
    uint16_t failed_read_addr = 0u;
    bit_transform_t failed_read_transform = TRANSFORM_IDENTITY;
    uint8_t failed_read_rotation = 0u;
    uint8_t failed_read_cmd = 0u;
    uint8_t failed_read_len_words = 0u;

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
    if (classify_failed_read8_candidate(rx_frame_raw,
                                        &failed_read_addr,
                                        &failed_read_transform,
                                        &failed_read_rotation,
                                        &failed_read_cmd,
                                        &failed_read_len_words)) {
        dlog("[PICO] FAILED_READ_CAND cmd=0x%02x addr=0x%04x len=%u t=%u rot=%u raw=%02x %02x %02x %02x %02x %02x %02x %02x\n",
             failed_read_cmd,
             (unsigned)failed_read_addr,
             (unsigned)failed_read_len_words,
             (unsigned)failed_read_transform,
             (unsigned)failed_read_rotation,
             rx_frame_raw[0], rx_frame_raw[1], rx_frame_raw[2], rx_frame_raw[3],
             rx_frame_raw[4], rx_frame_raw[5], rx_frame_raw[6], rx_frame_raw[7]);
    }
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
        if (is_motor_speed_addr(addr)) {
            trace_last_write_addr = addr;
            trace_last_write_value = value;
#if PICO_SPEED_TRACE
            speed_trace_seq++;
            speed_trace_push('W', addr, value, value_raw);
#else
            dlog("[PICO] FRAME_WRITE addr=0x%04x chosen=0x%08lx decoded=0x%08lx raw=0x%08lx rx=%02x %02x %02x %02x %02x %02x %02x %02x dec=%02x %02x %02x %02x %02x %02x %02x %02x\n",
                 (unsigned)addr,
                 (unsigned long)value,
                 (unsigned long)value_decoded,
                 (unsigned long)value_raw,
                 rx_frame_raw[0], rx_frame_raw[1], rx_frame_raw[2], rx_frame_raw[3],
                 rx_frame_raw[4], rx_frame_raw[5], rx_frame_raw[6], rx_frame_raw[7],
                 decoded[0], decoded[1], decoded[2], decoded[3],
                 decoded[4], decoded[5], decoded[6], decoded[7]);
#endif
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
        tx_force_rearm_on_next_cs_start = true;
        return true;
    }

    if (cmd == DBUS_CMD_READ) {
        uint32_t value = reg_read(addr);
        frame8_read_count++;
        if (addr == (MOTOR_REG_BASE + (1u * MOTOR_REG_STRIDE) + MOTOR_REG_SPEED_OFFSET)) {
            last_read_decode_motor1_count++;
        } else if (addr == (MOTOR_REG_BASE + (3u * MOTOR_REG_STRIDE) + MOTOR_REG_SPEED_OFFSET)) {
            last_read_decode_motor3_count++;
        }
        if (addr >= MOTOR_REG_BASE && addr < (MOTOR_REG_BASE + (MOTOR_COUNT * MOTOR_REG_STRIDE))) {
            value = motor_read(addr);
        }
#if PICO_FORCE_FIXED_READ_VALUE
        value = PICO_FORCE_FIXED_READ_VALUE;
#endif
        last_frame8_value = value;
#if PICO_SPEED_TRACE
        if (is_motor_speed_addr(addr)) {
            trace_last_read_addr = addr;
            trace_last_read_value = value;
            speed_trace_seq++;
            speed_trace_push('R', addr, value, 0u);
        } else {
            dlog("[PICO] READ addr=0x%04x val=0x%08x\n", addr, value);
        }
#else
        if (is_motor_speed_addr(addr)) {
            trace_last_read_addr = addr;
            trace_last_read_value = value;
        }
        dlog("[PICO] READ addr=0x%04x val=0x%08x\n", addr, value);
#endif
        last_read_rsp_count++;
        last_read_rsp_addr = addr;
        last_read_rsp_value = value;
        dlog("[PICO] READ_STAGE addr=0x%04x val=0x%08lx pending_before=%u txi=%u\n",
             (unsigned)addr,
             (unsigned long)value,
             tx_read_response_pending ? 1u : 0u,
             (unsigned)tx_index);
        prepare_read_response_frame(addr, value);
        return true;
    }

    set_default_tx_pattern();
    tx_read_response_pending = false;
    tx_index = 0u;
    tx_frame_prequeued = false;
    tx_force_rearm_on_next_cs_start = true;
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
    size_t queued_index;

    /* Preserve progress for a pending read response so repeated RW612 dummy
     * retries can collect bytes 1..7 instead of restarting at byte 0 on
     * every CS pulse. For idle/default traffic, restart from the beginning. */
    if (!tx_read_response_pending) {
        tx_index = 0u;
    }

    /* Count only bytes the SSP reports as accepted. Idle-time preload may
     * only take a prefix of the frame; preserve that prefix and let cs_start
     * plus the active service loop feed the remainder. */
    queued_index = spi_slave_continue_tx_frame(spi);
    if (tx_read_response_pending && tx_index >= FRAME_SIZE) {
        tx_read_response_retire_on_cs_end = true;
    }
    return queued_index;
}

static inline size_t spi_slave_force_queue_current_tx_frame(spi_inst_t *spi)
{
    spi_hw_t *hw = spi_get_hw(spi);
    uint32_t start_us = time_us_32();

    /* Blindly writing FRAME_SIZE times to hw->dr without checking
     * spi_is_writable() can silently drop bytes if the FIFO write pointer
     * has not settled yet right after spi_slave_rearm() toggles SSE off/on.
     * A dropped write is not reported by the hardware, so tx_index used to
     * claim a full queue even though only a prefix (often just byte 0)
     * actually landed, and the PL022 then held the last driven bit level for
     * the rest of the transfer. Poll spi_is_writable() like the other
     * queue helpers so every byte is confirmed before advancing. */
    if (!tx_read_response_pending) {
        tx_index = 0u;
    }
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

    if (tx_read_response_pending && tx_index >= FRAME_SIZE) {
        tx_read_response_retire_on_cs_end = true;
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
    /* Keep MISO in SPI function at all times. The last known-good all-4 motor
     * state used this behavior, and toggling MISO away from SPI has previously
     * broken the slave receiver on this link. */
    (void)active;
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
}

static void cs_gpio_irq_handler(uint gpio, uint32_t events)
{
    if (gpio == PIN_SONIC_ECHO) {
        uint32_t now_us = time_us_32();

        if ((events & GPIO_IRQ_EDGE_RISE) != 0u && sonic_state == SONIC_WAIT_RISE) {
            sonic_echo_start_us = now_us;
            sonic_state = SONIC_WAIT_FALL;
            sonic_echo_ready = false;
        }

        if ((events & GPIO_IRQ_EDGE_FALL) != 0u && sonic_state == SONIC_WAIT_FALL) {
            sonic_echo_pulse_us = (uint32_t)(now_us - sonic_echo_start_us);
            sonic_echo_ready = true;
        }
        return;
    }

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
        if (tx_read_response_retire_on_cs_end && tx_read_response_pending && tx_index >= FRAME_SIZE) {
            retire_pending_read_response();
        }

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

        /* Exact 8-byte register frames used by the RW612 motor path were
         * previously decoded successfully from the PL022 RX FIFO at cs_end.
         * Keep sniff decoding for longer DBAL traffic, but restore direct
         * frame decoding here for 8-byte register transactions. */
        last_frame_len = (uint32_t)rx_index;
        if (rx_index == FRAME_SIZE) {
            frame8_total++;
            pending_write7_tail_byte = false;
            (void)process_rx_frame();
            rx_index = 0u;
        } else if (rx_index > 0u) {
            last_partial_frame_len = (uint8_t)rx_index;
            memset(last_partial_frame_raw, 0, sizeof(last_partial_frame_raw));
            if (rx_index <= (FRAME_SIZE - 1u)) {
                memcpy(last_partial_frame_raw, rx_frame_raw, rx_index);
            }

            switch (rx_index) {
            case 1u:
                partial_len1_count++;
                break;
            case 2u:
                partial_len2_count++;
                last_partial_len2_raw[0] = rx_frame_raw[0];
                last_partial_len2_raw[1] = rx_frame_raw[1];
                dlog("[PICO] PARTIAL2 raw=%02x %02x\n", rx_frame_raw[0], rx_frame_raw[1]);
                if (rx_frame_raw[0] == 0x40u && rx_frame_raw[1] == 0x00u) {
                    partial_len2_4000_count++;
                } else if (rx_frame_raw[0] == 0x40u && rx_frame_raw[1] == 0x50u) {
                    partial_len2_4050_count++;
                } else if (rx_frame_raw[0] == 0x60u && rx_frame_raw[1] == 0x00u) {
                    partial_len2_6000_count++;
                } else if (rx_frame_raw[0] == 0x60u && rx_frame_raw[1] == 0x50u) {
                    partial_len2_6050_count++;
                } else {
                    partial_len2_other_count++;
                    last_partial_len2_other_raw[0] = rx_frame_raw[0];
                    last_partial_len2_other_raw[1] = rx_frame_raw[1];
                }
                break;
            case 3u:
                partial_len3_count++;
                break;
            case 4u:
                partial_len4_count++;
                break;
            case 5u:
                partial_len5_count++;
                break;
            case 6u:
                partial_len6_count++;
                break;
            case 7u:
                partial_len7_count++;
                break;
            default:
                break;
            }

            if (rx_index == (FRAME_SIZE - 1u) && rx_frame_raw[0] == DBUS_CMD_WRITE) {
                frame7_write_fifo_count++;
                memcpy(last_write7_fifo_raw, rx_frame_raw, FRAME_SIZE - 1u);
                pending_write7_tail_byte = true;
                dlog("[PICO] FIFO_WRITE7 raw=%02x %02x %02x %02x %02x %02x %02x\n",
                     rx_frame_raw[0], rx_frame_raw[1], rx_frame_raw[2], rx_frame_raw[3],
                     rx_frame_raw[4], rx_frame_raw[5], rx_frame_raw[6]);
            } else {
                pending_write7_tail_byte = false;
            }
            frame_other_total++;
            rx_index = 0u;
        }

        /* The just-finished frame has now been decoded and the drain is
         * complete, so it is safe to preload the next TX frame.
         *
         * Keep the conservative Pico2 behavior here: pending read responses
         * are not preloaded at cs_end, and are instead queued from cs_start
         * after a rearm. This was the least-bad behavior on bench. */
        if (!PICO_PREFER_CS_START_READ_REARM || !tx_read_response_pending) {
              spi_slave_rearm(spi);
    #if PICO_FORCE_FULL_TX_PRELOAD
              last_cs_end_preload_count = (uint32_t)spi_slave_force_queue_current_tx_frame(spi);
    #else
              last_cs_end_preload_count = (uint32_t)spi_slave_queue_current_tx_frame(spi);
    #endif
            tx_frame_prequeued = (last_cs_end_preload_count == FRAME_SIZE);
            dlog("[PICO] CS_END preload=%lu pending=%u prequeued=%u txi=%u\n",
                 (unsigned long)last_cs_end_preload_count,
                 tx_read_response_pending ? 1u : 0u,
                 tx_frame_prequeued ? 1u : 0u,
                 (unsigned)tx_index);
        } else {
            last_cs_end_preload_count = 0u;
            tx_frame_prequeued = false;
            dlog("[PICO] CS_END defer_read pending=1 txi=%u bytes=%02x %02x %02x %02x %02x %02x %02x %02x\n",
                 (unsigned)tx_index,
                 tx_frame_wire[0], tx_frame_wire[1], tx_frame_wire[2], tx_frame_wire[3],
                 tx_frame_wire[4], tx_frame_wire[5], tx_frame_wire[6], tx_frame_wire[7]);
        }
    }

    if (cs_start_pending) {
        cs_start_pending = false;
        if (rx_index != 0u) {
            if (pending_write7_tail_byte && rx_index == 1u) {
                frame7_write_tail_drop_count++;
                last_write7_tail_byte = rx_frame_raw[0];
                dlog("[PICO] CS_START write7_tail byte=%02x prev=%02x %02x %02x %02x %02x %02x %02x\n",
                     rx_frame_raw[0],
                     last_write7_fifo_raw[0], last_write7_fifo_raw[1], last_write7_fifo_raw[2],
                     last_write7_fifo_raw[3], last_write7_fifo_raw[4], last_write7_fifo_raw[5],
                     last_write7_fifo_raw[6]);
            }
            if (rx_index == 1u) {
                uint8_t stale_byte = rx_frame_raw[0];

                stale_rx1_drop_count++;
                last_stale_rx1_byte = stale_byte;
                switch (stale_byte) {
                case 0x00u:
                    stale_rx1_00_count++;
                    break;
                case 0x40u:
                    stale_rx1_40_count++;
                    break;
                case 0x50u:
                    stale_rx1_50_count++;
                    break;
                case 0x60u:
                    stale_rx1_60_count++;
                    break;
                case 0x01u:
                    stale_rx1_01_count++;
                    break;
                case 0xA0u:
                    stale_rx1_a0_count++;
                    break;
                default:
                    stale_rx1_other_count++;
                    break;
                }

                dlog("[PICO] CS_START dropping stale rx_index=1 byte=%02x last_byte_us=%lu\n",
                     stale_byte,
                     (unsigned long)rx_last_byte_us);
            } else {
                dlog("[PICO] CS_START dropping stale rx_index=%u last_byte_us=%lu\n",
                     (unsigned)rx_index,
                     (unsigned long)rx_last_byte_us);
            }
            rx_index = 0u;
        }
        pending_write7_tail_byte = false;
        /* Preserve any bytes already accepted during the cs_end preload. If we
         * rearm here, we flush that prefix right before the master clocks the
         * frame. Also do not blindly advance tx_index to 8: on this link the
         * SSP may only accept one start-of-frame byte immediately, and the
         * active loop must be allowed to feed the remaining tail. */
#if PICO_NODE_SLOT == 2
        if (tx_force_rearm_on_next_cs_start ||
            (PICO_PREFER_CS_START_READ_REARM && tx_read_response_pending)) {
            /* On the isolated RW612<->Pico2 readback path, trusting any
             * queued SSP prefix across CS pulses still produces circularly
             * shifted or stale replies. Start each pending read response from
             * a freshly rearmed FIFO and byte 0. */
            spi_slave_rearm(spi);
          tx_index = 0u;
            tx_frame_prequeued = false;
              dlog("[PICO] CS_START rearm force=%u pending_read=%u txi=%u bytes=%02x %02x %02x %02x %02x %02x %02x %02x\n",
                 tx_force_rearm_on_next_cs_start ? 1u : 0u,
                 tx_read_response_pending ? 1u : 0u,
                  (unsigned)tx_index,
                  tx_frame_wire[0], tx_frame_wire[1], tx_frame_wire[2], tx_frame_wire[3],
                  tx_frame_wire[4], tx_frame_wire[5], tx_frame_wire[6], tx_frame_wire[7]);
            tx_force_rearm_on_next_cs_start = false;
        }
#endif
        mark_spi_activity();
    #if PICO_FORCE_FULL_TX_PRELOAD
        last_cs_start_queue_count = (uint32_t)spi_slave_force_queue_current_tx_frame(spi);
    #else
        last_cs_start_queue_count = (uint32_t)spi_slave_fill_tx_frame_now(spi);
    #endif
        tx_frame_prequeued = (last_cs_start_queue_count == FRAME_SIZE);
           dlog("[PICO] CS_START queue=%lu pending=%u prequeued=%u txi=%u bytes=%02x %02x %02x %02x %02x %02x %02x %02x\n",
             (unsigned long)last_cs_start_queue_count,
             tx_read_response_pending ? 1u : 0u,
             tx_frame_prequeued ? 1u : 0u,
               (unsigned)tx_index,
               tx_frame_wire[0], tx_frame_wire[1], tx_frame_wire[2], tx_frame_wire[3],
               tx_frame_wire[4], tx_frame_wire[5], tx_frame_wire[6], tx_frame_wire[7]);
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
    /* Default path uses Mode 0. Pico2 experiments can override CPHA to test
     * whether command-capture collapse is caused by a phase mismatch at the
     * slave front-end. */
    spi_set_format(spi0, 8, SPI_CPOL_0, PICO_SPI_CPHA_SETTING, SPI_MSB_FIRST);
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
    gpio_set_irq_enabled(PIN_SONIC_ECHO,
                         GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                         true);

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
            !cs_active && rx_index == 0 && (uint32_t)(now_us - last_spi_activity_us) >= PICO_LOG_IDLE_FLUSH_US) {
            dlog_flush_limited(DLOG_FLUSH_BUDGET);
            speed_trace_flush_limited(DLOG_FLUSH_BUDGET);
        }

        if (PICO_USB_CONSOLE_ENABLE && !cs_active && rx_index == 0u) {
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
                                                                                                                                                                                                 printf("[Pico SPI Slave] alive fw=%s version=%s cs=%u rx=%u tx=%u rxt=%lu scke=%lu most=%lu gpe_mask=0x%08lx gpe=[%s] f8=%lu f8ok=%lu f8w=%lu f8r=%lu f8mw=%lu f8s=%lu f8sok=%lu fr7=%lu fw7=%lu fw71=%lu fw73=%lu fwf7=%lu fws7=%lu fwt=%lu fwd7=%lu pl1=%lu pl2=%lu pl3=%lu pl4=%lu pl5=%lu pl6=%lu pl7=%lu p24000=%lu p24050=%lu p26000=%lu p26050=%lu p2oth=%lu lp2raw=%02x%02x lp2othraw=%02x%02x lpflen=%u lpfraw=%02x%02x%02x%02x%02x%02x%02x s1=%lu s100=%lu s140=%lu s150=%lu s160=%lu s101=%lu s1a0=%lu s1oth=%lu ls1=0x%02x lwtb=0x%02x lw7a=0x%04x lw7v=0x%08lx lwd7a=0x%04x lwd7v=0x%08lx lwd7r=%u lwd7t=%u rd1=%lu rd3=%lu rpl=%lu rplda=%lu lfr=%02x%02x%02x%02x%02x%02x%02x%02x lf8_cmd=0x%02x lf8_addr=0x%04x lf8_val=0x%08lx txd=%02x%02x%02x%02x%02x%02x%02x%02x txw=%02x%02x%02x%02x%02x%02x%02x%02x lrrn=%lu lrra=0x%04x lrrv=0x%08lx lrrd=%02x%02x%02x%02x%02x%02x%02x%02x lrrw=%02x%02x%02x%02x%02x%02x%02x%02x qend=%lu qstart=%lu txfall=%lu txrise=%lu txact=%lu fdb=%lu foth=%lu lflen=%lu csf=%lu csr=%lu men=%lu mspd=%lu last_svc=0x%04x last_cmd=0x%04x last_idx=%u last_val=%ld mA_idx=%u mA_en=%u mA_spd=%ld mB_idx=%u mB_en=%u mB_spd=%ld\n",
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
                                 (unsigned long)frame7_read_promote_count,
                                 (unsigned long)frame7_speed_write_promote_count,
                                 (unsigned long)frame7_speed_write_motor1_count,
                                 (unsigned long)frame7_speed_write_motor3_count,
                                 (unsigned long)frame7_write_fifo_count,
                                 (unsigned long)frame7_write_sniff_count,
                                 (unsigned long)frame7_write_tail_drop_count,
                                 (unsigned long)frame7_write_decode_count,
                                 (unsigned long)partial_len1_count,
                                 (unsigned long)partial_len2_count,
                                 (unsigned long)partial_len3_count,
                                 (unsigned long)partial_len4_count,
                                 (unsigned long)partial_len5_count,
                                 (unsigned long)partial_len6_count,
                                 (unsigned long)partial_len7_count,
                                 (unsigned long)partial_len2_4000_count,
                                 (unsigned long)partial_len2_4050_count,
                                 (unsigned long)partial_len2_6000_count,
                                 (unsigned long)partial_len2_6050_count,
                                 (unsigned long)partial_len2_other_count,
                                 last_partial_len2_raw[0], last_partial_len2_raw[1],
                                 last_partial_len2_other_raw[0], last_partial_len2_other_raw[1],
                                 (unsigned)last_partial_frame_len,
                                 last_partial_frame_raw[0], last_partial_frame_raw[1], last_partial_frame_raw[2],
                                 last_partial_frame_raw[3], last_partial_frame_raw[4], last_partial_frame_raw[5],
                                 last_partial_frame_raw[6],
                                 (unsigned long)stale_rx1_drop_count,
                                 (unsigned long)stale_rx1_00_count,
                                 (unsigned long)stale_rx1_40_count,
                                 (unsigned long)stale_rx1_50_count,
                                 (unsigned long)stale_rx1_60_count,
                                 (unsigned long)stale_rx1_01_count,
                                 (unsigned long)stale_rx1_a0_count,
                                 (unsigned long)stale_rx1_other_count,
                                 (unsigned)last_stale_rx1_byte,
                                 (unsigned)last_write7_tail_byte,
                                 (unsigned)last_write7_addr,
                                 (unsigned long)last_write7_value,
                                 (unsigned)last_write7_decode_addr,
                                 (unsigned long)last_write7_decode_value,
                                 (unsigned)last_write7_decode_rotation,
                                 (unsigned)last_write7_decode_transform,
                                 (unsigned long)last_read_decode_motor1_count,
                                 (unsigned long)last_read_decode_motor3_count,
                                 (unsigned long)pending_rsp_replace_count,
                                 (unsigned long)pending_rsp_replace_diff_addr_count,
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
