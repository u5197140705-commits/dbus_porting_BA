#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/autoconf.h> // Explicitly include generated Kconfig definitions
#include "dbus_app_layer.h" // Include the ported DBus Application Layer
#include "spi_abstraction.h" // Include the SPI abstraction layer
#include "dbus_driver_public.h" // Include the DBus driver public API
#include "motor_service.h"
#include "lcd_service.h"
#include "ultrasonic_service.h"
#include <zephyr/sys_clock.h> // For K_SECONDS macro
#include <stdio.h>

#define MOTOR_STATUS_ENABLED   0x00000001u
#define MOTOR_STATUS_AVAILABLE 0x00000002u

#define MOTOR_REG_BASE            0x5000u
#define MOTOR_REG_STRIDE          0x10u
#define MOTOR_REG_ENABLE_OFFSET   0x0u
#define MOTOR_REG_SPEED_OFFSET    0x4u

#define DBAL_TEST_SERVICE_ID   0x7001u
#define DBAL_TEST_COMMAND_ID   0x0001u

#ifndef RW612_BUILD_MARKER
#define RW612_BUILD_MARKER "RW612 build marker: SHORT_ALL4_CONFIRM_V1_2026_05_28"
#endif

#ifndef RW612_BOOT_BANNER
#define RW612_BOOT_BANNER "SHORT_ALL4_CONFIRM_V1"
#endif

#ifndef RW612_MODE_LABEL
#define RW612_MODE_LABEL "SHORT_ALL4_CONFIRM_V1"
#endif

#ifndef DBUS_REPEATABILITY_RUNS
#define DBUS_REPEATABILITY_RUNS 1u
#endif

#ifndef DBUS_ENABLE_AUTO_MOTOR_TEST
#define DBUS_ENABLE_AUTO_MOTOR_TEST 1
#endif

#ifndef DBUS_CONTINUOUS_SECONDARY_TEST
#define DBUS_CONTINUOUS_SECONDARY_TEST 0
#endif

#ifndef DBUS_DISABLE_DBAL_BOOTSTRAP
#define DBUS_DISABLE_DBAL_BOOTSTRAP 1
#endif

#ifndef DBUS_SECONDARY_CS_HEARTBEAT
#define DBUS_SECONDARY_CS_HEARTBEAT 1
#endif

#ifndef DBUS_SECONDARY_CS_DIAG_ONLY
#define DBUS_SECONDARY_CS_DIAG_ONLY 0
#endif

#define CS_PROBE_GPIO_NODE DT_NODELABEL(hsgpio0)

/* Set to 1 for a dedicated physical SCK-line probe on RW612 GPIO7.
 * This mode pulses GPIO7 as plain GPIO and exits, so Pico can confirm
 * GP18 edge counting independent of Flexcomm SPI traffic. */
#ifndef DBUS_SCK_PROBE_ONLY
#define DBUS_SCK_PROBE_ONLY 0
#endif

/* Isolation mode for Pico2 clocking debug:
 * - Skip primary verification cycle
 * - Keep GPIO6 forced low while running secondary cycle
 * This removes cross-target side effects and guarantees the known
 * Flexcomm SCK-gate condition while measuring Pico2 RX behavior. */
#ifndef DBUS_FORCE_SECONDARY_ONLY
#define DBUS_FORCE_SECONDARY_ONLY 0
#endif

#ifndef DBUS_SKIP_GPIO_PROBES
#define DBUS_SKIP_GPIO_PROBES 1
#endif

#ifndef DBUS_ENABLE_READBACK_PROBE
#define DBUS_ENABLE_READBACK_PROBE 1
#endif

#ifndef DBUS_READBACK_PROBE_PRIMARY_ONLY
#define DBUS_READBACK_PROBE_PRIMARY_ONLY 0
#endif

#ifndef DBUS_ENABLE_ENDSWITCH_TEST
#define DBUS_ENABLE_ENDSWITCH_TEST 0
#endif

#ifndef DBUS_ENABLE_REGISTER_LOG_PROBE
#define DBUS_ENABLE_REGISTER_LOG_PROBE 1
#endif

#ifndef DBUS_PRIMARY_REGISTER_LOG_ONLY
#define DBUS_PRIMARY_REGISTER_LOG_ONLY 0
#endif

#ifndef DBUS_PRIMARY_LIVE_QUAD_READBACK_TEST
#define DBUS_PRIMARY_LIVE_QUAD_READBACK_TEST 0
#endif

#ifndef DBUS_ALL4_LIVE_QUAD_READBACK_TEST
#define DBUS_ALL4_LIVE_QUAD_READBACK_TEST 0
#endif

#ifndef DBUS_SECONDARY_LIVE_QUAD_READBACK_TEST
#define DBUS_SECONDARY_LIVE_QUAD_READBACK_TEST 0
#endif

#ifndef DBUS_FINAL_ALL4_SWITCH_TEST
#define DBUS_FINAL_ALL4_SWITCH_TEST 0
#endif

#ifndef DBUS_SHORT_ALL4_CONFIRM_TEST
#define DBUS_SHORT_ALL4_CONFIRM_TEST 1
#endif

#ifndef DBUS_SHORT_PRIMARY_STOPPED_READBACK_TEST
#define DBUS_SHORT_PRIMARY_STOPPED_READBACK_TEST 0
#endif

#ifndef DBUS_ALL_TOGETHER_TEST
#define DBUS_ALL_TOGETHER_TEST 0
#endif

#ifndef DBUS_LCD_SONIC_TEST
#define DBUS_LCD_SONIC_TEST 0
#endif

#define ENDSWITCH_TEST_VISUAL_RUN_MS 1500u
#define ENDSWITCH_TEST_SWITCH_RUN_MS 1800u
#define ENDSWITCH_TEST_ARM_MS        900u
#define ENDSWITCH_TEST_GAP_MS        250u
#define SHORT_CONFIRM_RUN_MS         3000u

#define MOTOR1_ENDSWITCH_MIN_PIN 15u
#define MOTOR1_ENDSWITCH_MAX_PIN 4u
#define MOTOR2_ENDSWITCH_MIN_PIN 5u
#define MOTOR2_ENDSWITCH_MAX_PIN 11u
#define MOTOR0_ENDSWITCH_MIN_PIN 1u
#define MOTOR0_ENDSWITCH_MAX_PIN 2u
#define MOTOR0_ENDSWITCH_POLL_MS 10u

struct motor_endswitch_binding {
    uint8_t motor;
    gpio_pin_t min_pin;
    gpio_pin_t max_pin;
};

static const struct motor_endswitch_binding motor_endswitch_bindings[] = {
    { 0u, MOTOR0_ENDSWITCH_MIN_PIN, MOTOR0_ENDSWITCH_MAX_PIN },
    { 1u, MOTOR1_ENDSWITCH_MIN_PIN, MOTOR1_ENDSWITCH_MAX_PIN },
    { 2u, MOTOR2_ENDSWITCH_MIN_PIN, MOTOR2_ENDSWITCH_MAX_PIN },
};

static void pulse_gpio_probe_pin(const struct device *gpio_dev,
                                 gpio_pin_t pin,
                                 uint32_t pulse_count,
                                 uint32_t low_time_us,
                                 uint32_t high_time_us)
{
    int ret;

    ret = gpio_pin_configure(gpio_dev, pin, GPIO_OUTPUT_HIGH);
    if (ret < 0) {
        printk("Main: GPIO probe configure pin %u failed ret=%d\n", (unsigned)pin, ret);
        return;
    }

    printk("Main: GPIO probe start pin=%u pulses=%u\n", (unsigned)pin, (unsigned)pulse_count);
    for (uint32_t i = 0u; i < pulse_count; i++) {
        (void)gpio_pin_set(gpio_dev, pin, 0);
        k_usleep(low_time_us);
        (void)gpio_pin_set(gpio_dev, pin, 1);
        k_usleep(high_time_us);
    }
    printk("Main: GPIO probe done pin=%u\n", (unsigned)pin);
}

static enum DBC_Error write_motor_reg32(uint8_t motor_index,
                                        uint16_t reg_offset,
                                        uint32_t value);
static enum DBCDRV_SpiTarget target_for_motor(uint8_t motor_index);
static bool run_quad_simultaneous_cycle_custom(uint32_t run_ms,
                                               uint32_t gap_ms,
                                               const char *label,
                                               uint32_t guard_motor_mask);
static void run_register_log_probe(void);
static void run_primary_live_quad_readback_test(void);
static void run_all4_live_quad_readback_test(void);
static void run_secondary_live_quad_readback_test(void);
static void run_final_all4_switch_test(const struct device *gpio_dev);
static void run_short_all4_confirm_test(const struct device *gpio_dev);
static void run_short_primary_stopped_readback_test(void);
static void run_all_together_test(const struct device *gpio_dev);
static void run_lcd_sonic_test(void);
static enum DBC_Error read_motor_reg32(uint8_t motor_index,
                                       uint16_t reg_offset,
                                       uint32_t *value);

static bool select_spi_target_with_settle(enum DBCDRV_SpiTarget target,
                                          const char *log_tag,
                                          const char *phase)
{
    enum DBC_Error err = DBCDRV_setSpiTarget(target);

    if (err != DBC_OK) {
        printk("%s %s target err=%d\n",
               (log_tag != NULL) ? log_tag : "Main:",
               (phase != NULL) ? phase : "spi",
               err);
        return false;
    }

    k_usleep(100u);
    return true;
}

static void init_endswitch_inputs(const struct device *gpio_dev)
{
    if (!device_is_ready(gpio_dev)) {
        printk("Main: end-switch GPIO device not ready\n");
        return;
    }

    for (size_t i = 0u; i < ARRAY_SIZE(motor_endswitch_bindings); i++) {
        const gpio_pin_t pins[] = {
            motor_endswitch_bindings[i].min_pin,
            motor_endswitch_bindings[i].max_pin,
        };

        for (size_t pin_index = 0u; pin_index < ARRAY_SIZE(pins); pin_index++) {
            int ret = gpio_pin_configure(gpio_dev, pins[pin_index], GPIO_INPUT | GPIO_PULL_UP);

            if (ret < 0) {
                printk("Main: end-switch pin %u configure failed ret=%d\n",
                       (unsigned)pins[pin_index], ret);
                continue;
            }

            ret = gpio_pin_get(gpio_dev, pins[pin_index]);
            printk("Main: end-switch pin %u ready state=%d (active-low)\n",
                   (unsigned)pins[pin_index], ret);
        }
    }
}

static bool motor_endswitch_active(const struct device *gpio_dev,
                                   uint32_t motor_mask,
                                   uint8_t *active_motor,
                                   gpio_pin_t *active_pin)
{
    if (!device_is_ready(gpio_dev)) {
        return false;
    }

    for (size_t i = 0u; i < ARRAY_SIZE(motor_endswitch_bindings); i++) {
        const struct motor_endswitch_binding *binding = &motor_endswitch_bindings[i];
        const gpio_pin_t pins[] = { binding->min_pin, binding->max_pin };

        if ((motor_mask & (1u << binding->motor)) == 0u) {
            continue;
        }

        for (size_t pin_index = 0u; pin_index < ARRAY_SIZE(pins); pin_index++) {
            int state = gpio_pin_get(gpio_dev, pins[pin_index]);

            if (state < 0) {
                printk("Main: end-switch pin %u read failed ret=%d\n",
                       (unsigned)pins[pin_index], state);
                continue;
            }

            if (state == 0) {
                if (active_motor != NULL) {
                    *active_motor = binding->motor;
                }
                if (active_pin != NULL) {
                    *active_pin = pins[pin_index];
                }
                return true;
            }
        }
    }

    return false;
}

static uint32_t motor_endswitch_active_mask(const struct device *gpio_dev,
                                            uint32_t motor_mask)
{
    uint32_t active_mask = 0u;

    if (!device_is_ready(gpio_dev)) {
        return 0u;
    }

    for (size_t i = 0u; i < ARRAY_SIZE(motor_endswitch_bindings); i++) {
        const struct motor_endswitch_binding *binding = &motor_endswitch_bindings[i];
        const gpio_pin_t pins[] = { binding->min_pin, binding->max_pin };

        if ((motor_mask & (1u << binding->motor)) == 0u) {
            continue;
        }

        for (size_t pin_index = 0u; pin_index < ARRAY_SIZE(pins); pin_index++) {
            int state = gpio_pin_get(gpio_dev, pins[pin_index]);

            if (state == 0) {
                active_mask |= (1u << binding->motor);
                break;
            }
        }
    }

    return active_mask;
}

static void stop_motor_now(uint8_t motor)
{
    enum DBC_Error err;

    if (!select_spi_target_with_settle(target_for_motor(motor),
                                       "Main:",
                                       "emergency stop")) {
        printk("Main: motor%u emergency stop target select failed\n", (unsigned)motor);
        return;
    }

    for (uint8_t attempt = 0u; attempt < 2u; attempt++) {
        err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
        if (err != DBC_OK) {
            printk("Main: motor%u emergency disable err=%d attempt=%u\n",
                   (unsigned)motor,
                   err,
                   (unsigned)(attempt + 1u));
        }

        err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, 0u);
        if (err != DBC_OK) {
            printk("Main: motor%u emergency speed0 err=%d attempt=%u\n",
                   (unsigned)motor,
                   err,
                   (unsigned)(attempt + 1u));
        }

        k_usleep(150u);
    }
}

static bool stop_motor_on_selected_target(uint8_t motor, const char *log_tag)
{
    enum DBC_Error err;

    for (uint8_t attempt = 0u; attempt < 2u; attempt++) {
        err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
        if (err != DBC_OK) {
            printk("%s motor%u disable err=%d attempt=%u\n",
                   (log_tag != NULL) ? log_tag : "Main:",
                   (unsigned)motor,
                   err,
                   (unsigned)(attempt + 1u));
            return false;
        }

        err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, 0u);
        if (err != DBC_OK) {
            printk("%s motor%u speed0 err=%d attempt=%u\n",
                   (log_tag != NULL) ? log_tag : "Main:",
                   (unsigned)motor,
                   err,
                   (unsigned)(attempt + 1u));
            return false;
        }

        k_usleep(150u);
    }

    return true;
}

static bool start_motor_now(uint8_t motor, uint32_t speed, const char *log_tag)
{
    enum DBC_Error err;

    if (!select_spi_target_with_settle(target_for_motor(motor),
                                       log_tag,
                                       "start")) {
        printk("%s motor%u target select failed\n",
               (log_tag != NULL) ? log_tag : "Main:",
               (unsigned)motor);
        return false;
    }

    err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
    if (err != DBC_OK) {
        printk("%s motor%u pre-disable err=%d\n",
               (log_tag != NULL) ? log_tag : "Main:",
               (unsigned)motor,
               err);
        return false;
    }

    err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, 0u);
    if (err != DBC_OK) {
        printk("%s motor%u pre-speed0 err=%d\n",
               (log_tag != NULL) ? log_tag : "Main:",
               (unsigned)motor,
               err);
        return false;
    }

    k_usleep(150u);

    for (uint8_t attempt = 0u; attempt < 2u; attempt++) {
        err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 1u);
        if (err != DBC_OK) {
            printk("%s motor%u enable err=%d attempt=%u\n",
                   (log_tag != NULL) ? log_tag : "Main:",
                   (unsigned)motor,
                   err,
                   (unsigned)(attempt + 1u));
            return false;
        }

        err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, speed);
        if (err != DBC_OK) {
            printk("%s motor%u speed err=%d attempt=%u\n",
                   (log_tag != NULL) ? log_tag : "Main:",
                   (unsigned)motor,
                   err,
                   (unsigned)(attempt + 1u));
            return false;
        }

        k_usleep(200u);
    }

    return true;
}

static bool start_motor_on_selected_target(uint8_t motor, uint32_t speed, const char *log_tag)
{
    enum DBC_Error err;

    err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
    if (err != DBC_OK) {
        printk("%s motor%u pre-disable err=%d\n",
               (log_tag != NULL) ? log_tag : "Main:",
               (unsigned)motor,
               err);
        return false;
    }

    err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, 0u);
    if (err != DBC_OK) {
        printk("%s motor%u pre-speed0 err=%d\n",
               (log_tag != NULL) ? log_tag : "Main:",
               (unsigned)motor,
               err);
        return false;
    }

    k_usleep(150u);

    for (uint8_t attempt = 0u; attempt < 2u; attempt++) {
        err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 1u);
        if (err != DBC_OK) {
            printk("%s motor%u enable err=%d attempt=%u\n",
                   (log_tag != NULL) ? log_tag : "Main:",
                   (unsigned)motor,
                   err,
                   (unsigned)(attempt + 1u));
            return false;
        }

        err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, speed);
        if (err != DBC_OK) {
            printk("%s motor%u speed err=%d attempt=%u\n",
                   (log_tag != NULL) ? log_tag : "Main:",
                   (unsigned)motor,
                   err,
                   (unsigned)(attempt + 1u));
            return false;
        }

        k_usleep(200u);
    }

    return true;
}

static bool wait_with_endswitch_guard(const struct device *gpio_dev,
                                      uint32_t run_ms,
                                      uint32_t motor_mask)
{
    uint32_t remaining_ms = run_ms;

    while (remaining_ms > 0u) {
        uint8_t active_motor = 0u;
        gpio_pin_t active_pin = 0u;
        uint32_t sleep_ms = (remaining_ms > MOTOR0_ENDSWITCH_POLL_MS)
            ? MOTOR0_ENDSWITCH_POLL_MS
            : remaining_ms;

        if (motor_endswitch_active(gpio_dev, motor_mask, &active_motor, &active_pin)) {
            const char *active_label = "UNKNOWN";

            for (size_t i = 0u; i < ARRAY_SIZE(motor_endswitch_bindings); i++) {
                const struct motor_endswitch_binding *binding = &motor_endswitch_bindings[i];

                if (binding->motor != active_motor) {
                    continue;
                }

                if (binding->min_pin == active_pin) {
                    active_label = "MIN";
                } else if (binding->max_pin == active_pin) {
                    active_label = "MAX";
                }
                break;
            }

            printk("Main: SWITCH PRESSED motor%u %s GPIO%u -> stopping motor\n",
                   (unsigned)active_motor,
                   active_label,
                   (unsigned)active_pin);
            stop_motor_now(active_motor);
            return false;
        }

        k_msleep(sleep_ms);
        remaining_ms -= sleep_ms;
    }

    return true;
}

static void stop_all_motors_now(void)
{
    for (uint8_t motor = 0u; motor < 4u; motor++) {
        stop_motor_now(motor);
    }
}

static bool run_single_motor_endswitch_window(const struct device *gpio_dev,
                                              uint8_t motor,
                                              gpio_pin_t expected_pin,
                                              const char *expected_label,
                                              uint32_t speed,
                                              uint32_t run_ms)
{
    gpio_pin_t active_pin = 0u;
    uint8_t active_motor = 0u;

        printk("Main: test motor%u / expect %s on GPIO%u / speed=%u\n",
           (unsigned)motor,
            expected_label,
           (unsigned)expected_pin,
           (unsigned)speed);
        printk("Main: press only this switch during this run\n");
    printk("Main: arm window %ums\n", (unsigned)ENDSWITCH_TEST_ARM_MS);
    k_msleep(ENDSWITCH_TEST_ARM_MS);

    if (!start_motor_now(motor, speed, "Main: test")) {
        return false;
    }

    printk("Main: motor%u running for up to %ums, press GPIO%u\n",
           (unsigned)motor,
           (unsigned)run_ms,
           (unsigned)expected_pin);

    if (device_is_ready(gpio_dev)) {
        uint32_t remaining_ms = run_ms;

        while (remaining_ms > 0u) {
            uint32_t sleep_ms = (remaining_ms > MOTOR0_ENDSWITCH_POLL_MS)
                ? MOTOR0_ENDSWITCH_POLL_MS
                : remaining_ms;

            if (motor_endswitch_active(gpio_dev,
                                       (1u << motor),
                                       &active_motor,
                                       &active_pin)) {
                     printk("Main: SWITCH PRESSED expected motor%u %s GPIO%u / observed motor%u GPIO%u\n",
                              (unsigned)motor,
                              expected_label,
                              (unsigned)expected_pin,
                              (unsigned)active_motor,
                              (unsigned)active_pin);
                stop_motor_now(active_motor);
                if (active_motor == motor && active_pin == expected_pin) {
                                  printk("Main: switch test PASS motor%u %s GPIO%u\n",
                           (unsigned)motor,
                              expected_label,
                           (unsigned)expected_pin);
                    return true;
                }

                      printk("Main: switch test FAIL motor%u expected %s GPIO%u got motor%u GPIO%u\n",
                       (unsigned)motor,
                          expected_label,
                       (unsigned)expected_pin,
                       (unsigned)active_motor,
                       (unsigned)active_pin);
                return false;
            }

            k_msleep(sleep_ms);
            remaining_ms -= sleep_ms;
        }
    } else {
        k_msleep(run_ms);
    }

    stop_motor_now(motor);
        printk("Main: switch test TIMEOUT motor%u %s GPIO%u\n",
           (unsigned)motor,
            expected_label,
           (unsigned)expected_pin);
    return false;
}

static void run_guided_endswitch_test(const struct device *gpio_dev)
{
    static const struct {
        uint8_t motor;
        gpio_pin_t pin;
        const char *label;
        uint32_t speed;
    } steps[] = {
        { 0u, MOTOR0_ENDSWITCH_MIN_PIN, "MIN", 420u },
        { 0u, MOTOR0_ENDSWITCH_MAX_PIN, "MAX", 420u },
        { 1u, MOTOR1_ENDSWITCH_MIN_PIN, "MIN", 520u },
        { 1u, MOTOR1_ENDSWITCH_MAX_PIN, "MAX", 520u },
        { 2u, MOTOR2_ENDSWITCH_MIN_PIN, "MIN", 460u },
        { 2u, MOTOR2_ENDSWITCH_MAX_PIN, "MAX", 460u },
    };
    uint32_t pass_count = 0u;

    printk("Main: END SWITCH TEST mode active\n");
    printk("Main: step 1/7 short quad preview (%ums)\n",
           (unsigned)ENDSWITCH_TEST_VISUAL_RUN_MS);
    printk("Main: do not press any end switch during the quad preview\n");

    if (!run_quad_simultaneous_cycle_custom(ENDSWITCH_TEST_VISUAL_RUN_MS,
                                            ENDSWITCH_TEST_GAP_MS,
                                            "QUAD_SIMUL_SHORT_V1",
                                            0u)) {
        printk("Main: short quad preview reported errors\n");
    }

    stop_all_motors_now();
    k_msleep(ENDSWITCH_TEST_GAP_MS);

    printk("Main: step 2/7..7/7 single-switch checks\n");
    printk("Main: each step arms briefly, then runs one motor until the named switch stops it\n");

    for (size_t i = 0u; i < ARRAY_SIZE(steps); i++) {
         printk("Main: switch step %u/%u -> motor%u %s on GPIO%u\n",
               (unsigned)(i + 1u),
             (unsigned)ARRAY_SIZE(steps),
             (unsigned)steps[i].motor,
             steps[i].label,
             (unsigned)steps[i].pin);

        if (run_single_motor_endswitch_window(gpio_dev,
                                              steps[i].motor,
                                              steps[i].pin,
                                steps[i].label,
                                              steps[i].speed,
                                              ENDSWITCH_TEST_SWITCH_RUN_MS)) {
            pass_count++;
        }

        stop_all_motors_now();
        k_msleep(ENDSWITCH_TEST_GAP_MS);
    }

    printk("Main: END SWITCH TEST summary pass=%u fail=%u\n",
           (unsigned)pass_count,
           (unsigned)(ARRAY_SIZE(steps) - pass_count));
}

static void run_motor0_endswitch_test(const struct device *gpio_dev)
{
    run_guided_endswitch_test(gpio_dev);
}

static void run_final_all4_switch_test(const struct device *gpio_dev)
{
    printk("Main: FINAL all-4 plus switch test mode active\n");
    printk("Main: phase 1/2 merged all-4 live readback\n");
    run_all4_live_quad_readback_test();

    stop_all_motors_now();
    k_msleep(ENDSWITCH_TEST_GAP_MS);

    printk("Main: phase 2/2 guided end-switch checks\n");
    init_endswitch_inputs(gpio_dev);
    run_guided_endswitch_test(gpio_dev);
}

static void run_short_all4_confirm_test(const struct device *gpio_dev)
{
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } primary_motors[] = {
        { 0u, 0x00000123u },
        { 2u, 0x00000789u },
    };
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } secondary_motors[] = {
        { 1u, 0x000001F4u },
        { 3u, 0x0000044Cu },
    };
    uint32_t remaining_ms = SHORT_CONFIRM_RUN_MS;
    uint32_t switch_hit_mask = 0u;
    uint32_t switchable_mask = (1u << 0) | (1u << 1) | (1u << 2);
    uint32_t switch_released_mask = 0u;

    printk("Main: SHORT all-4 confirm test start\n");
    printk("Main: goal = write + readback + switch activity within %ums\n",
           (unsigned)SHORT_CONFIRM_RUN_MS);

    init_endswitch_inputs(gpio_dev);
    stop_all_motors_now();
    k_msleep(100u);

    printk("Main: phase 1/2 run motors with immediate switch guard\n");

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                      "Main:",
                                      "short confirm primary start")) {
        for (size_t i = 0u; i < ARRAY_SIZE(primary_motors); i++) {
            if (!start_motor_on_selected_target(primary_motors[i].motor,
                                                primary_motors[i].speed,
                                                "Main: short confirm")) {
                printk("Main: short confirm start failed motor=%u\n",
                       (unsigned)primary_motors[i].motor);
            }
        }
    }

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                      "Main:",
                                      "short confirm secondary start")) {
        for (size_t i = 0u; i < ARRAY_SIZE(secondary_motors); i++) {
            if (!start_motor_on_selected_target(secondary_motors[i].motor,
                                                secondary_motors[i].speed,
                                                "Main: short confirm")) {
                printk("Main: short confirm start failed motor=%u\n",
                       (unsigned)secondary_motors[i].motor);
            }
        }
    }

    printk("Main: switch window active for %ums, press any corresponding switch to stop that motor early\n",
           (unsigned)SHORT_CONFIRM_RUN_MS);

    if (device_is_ready(gpio_dev)) {
        uint32_t initially_active_mask = motor_endswitch_active_mask(gpio_dev, switchable_mask);

        if (initially_active_mask != 0u) {
            printk("Main: initial active switch mask=0x%02x; those motors will ignore current low state until released\n",
                   (unsigned)initially_active_mask);
        }
        switch_released_mask = switchable_mask & ~initially_active_mask;
    } else {
        switch_released_mask = switchable_mask;
    }

    while (remaining_ms > 0u) {
        gpio_pin_t active_pin = 0u;
        uint8_t active_motor = 0u;
        uint32_t sleep_ms = (remaining_ms > MOTOR0_ENDSWITCH_POLL_MS)
            ? MOTOR0_ENDSWITCH_POLL_MS
            : remaining_ms;

        if (device_is_ready(gpio_dev)) {
            uint32_t active_mask = motor_endswitch_active_mask(gpio_dev, switchable_mask);
            uint32_t released_now_mask = switchable_mask & ~active_mask;

            switch_released_mask |= released_now_mask;
        }

        if (device_is_ready(gpio_dev) &&
            switchable_mask != 0u &&
            motor_endswitch_active(gpio_dev, switchable_mask & switch_released_mask, &active_motor, &active_pin)) {
            const char *active_label = "UNKNOWN";

            for (size_t i = 0u; i < ARRAY_SIZE(motor_endswitch_bindings); i++) {
                const struct motor_endswitch_binding *binding = &motor_endswitch_bindings[i];

                if (binding->motor != active_motor) {
                    continue;
                }

                if (binding->min_pin == active_pin) {
                    active_label = "MIN";
                } else if (binding->max_pin == active_pin) {
                    active_label = "MAX";
                }
                break;
            }

            printk("Main: short confirm switch motor%u %s GPIO%u -> stopping that motor\n",
                   (unsigned)active_motor,
                   active_label,
                   (unsigned)active_pin);
            stop_motor_now(active_motor);
            switch_hit_mask |= (1u << active_motor);
            switchable_mask &= ~(1u << active_motor);
                 switch_released_mask &= ~(1u << active_motor);
            continue;
        }

        k_msleep(sleep_ms);
        remaining_ms -= sleep_ms;
    }

    stop_all_motors_now();
    printk("Main: short confirm switch summary motor0=%u motor1=%u motor2=%u\n",
           (unsigned)((switch_hit_mask >> 0) & 0x1u),
           (unsigned)((switch_hit_mask >> 1) & 0x1u),
           (unsigned)((switch_hit_mask >> 2) & 0x1u));

    printk("Main: phase 2/2 compact write+readback confirmation while motors are stopped\n");

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                      "Main:",
                                      "short confirm primary readback")) {
        for (size_t i = 0u; i < ARRAY_SIZE(primary_motors); i++) {
            uint32_t enable_value = 0u;
            uint32_t speed_value = 0u;
            enum DBC_Error err;

            err = write_motor_reg32(primary_motors[i].motor,
                                    MOTOR_REG_ENABLE_OFFSET,
                                    0u);
            if (err != DBC_OK) {
                printk("Main: short confirm preset motor=%u enable write err=%d\n",
                       (unsigned)primary_motors[i].motor,
                       err);
                continue;
            }

            err = write_motor_reg32(primary_motors[i].motor,
                                    MOTOR_REG_SPEED_OFFSET,
                                    primary_motors[i].speed);
            if (err != DBC_OK) {
                printk("Main: short confirm preset motor=%u speed write err=%d\n",
                       (unsigned)primary_motors[i].motor,
                       err);
                continue;
            }

            err = read_motor_reg32(primary_motors[i].motor,
                                   MOTOR_REG_ENABLE_OFFSET,
                                   &enable_value);
            if (err != DBC_OK) {
                printk("Main: short confirm readback motor=%u enable err=%d\n",
                       (unsigned)primary_motors[i].motor,
                       err);
                continue;
            }

            err = read_motor_reg32(primary_motors[i].motor,
                                   MOTOR_REG_SPEED_OFFSET,
                                   &speed_value);
            if (err != DBC_OK) {
                printk("Main: short confirm readback motor=%u speed err=%d\n",
                       (unsigned)primary_motors[i].motor,
                       err);
                continue;
            }

            printk("Main: short confirm readback motor=%u expected=0x%08x speed=0x%08x enable=0x%08x\n",
                   (unsigned)primary_motors[i].motor,
                   (unsigned)primary_motors[i].speed,
                   (unsigned)speed_value,
                   (unsigned)enable_value);
        }
    }

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                      "Main:",
                                      "short confirm secondary readback")) {
        for (size_t i = 0u; i < ARRAY_SIZE(secondary_motors); i++) {
            uint32_t enable_value = 0u;
            uint32_t speed_value = 0u;
            enum DBC_Error err;

            err = write_motor_reg32(secondary_motors[i].motor,
                                    MOTOR_REG_ENABLE_OFFSET,
                                    0u);
            if (err != DBC_OK) {
                printk("Main: short confirm preset motor=%u enable write err=%d\n",
                       (unsigned)secondary_motors[i].motor,
                       err);
                continue;
            }

            err = write_motor_reg32(secondary_motors[i].motor,
                                    MOTOR_REG_SPEED_OFFSET,
                                    secondary_motors[i].speed);
            if (err != DBC_OK) {
                printk("Main: short confirm preset motor=%u speed write err=%d\n",
                       (unsigned)secondary_motors[i].motor,
                       err);
                continue;
            }

            err = read_motor_reg32(secondary_motors[i].motor,
                                   MOTOR_REG_ENABLE_OFFSET,
                                   &enable_value);
            if (err != DBC_OK) {
                printk("Main: short confirm readback motor=%u enable err=%d\n",
                       (unsigned)secondary_motors[i].motor,
                       err);
                continue;
            }

            err = read_motor_reg32(secondary_motors[i].motor,
                                   MOTOR_REG_SPEED_OFFSET,
                                   &speed_value);
            if (err != DBC_OK) {
                printk("Main: short confirm readback motor=%u speed err=%d\n",
                       (unsigned)secondary_motors[i].motor,
                       err);
                continue;
            }

            printk("Main: short confirm readback motor=%u expected=0x%08x speed=0x%08x enable=0x%08x\n",
                   (unsigned)secondary_motors[i].motor,
                   (unsigned)secondary_motors[i].speed,
                   (unsigned)speed_value,
                   (unsigned)enable_value);
        }
    }

    printk("Main: SHORT all-4 confirm test end\n");
}

static void run_short_primary_stopped_readback_test(void)
{
    printk("Main: SHORT primary stopped-readback mode active\n");
    run_register_log_probe();
}

static void run_all_together_test(const struct device *gpio_dev)
{
    printk("Main: ALL TOGETHER test mode active\n");
    printk("Main: Pico1 local LCD should keep showing live sonic distance during this run\n");
    printk("Main: phase 1/2 = proven live all-4 motor run\n");
    run_all4_live_quad_readback_test();
    k_msleep(ENDSWITCH_TEST_GAP_MS);

    printk("Main: phase 2/2 = switch-aware confirmation\n");
    run_short_all4_confirm_test(gpio_dev);
}

static void run_lcd_sonic_test(void)
{
    char line_buf[LCD_TEXT_MAX_LEN + 1u];
    uint16_t distance_mm = 0u;
    uint16_t last_good_distance_mm = 0u;
    bool have_good_distance = false;
    enum DBC_Error err;

    printk("Main: LCD + sonic test mode active\n");

    if (!select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                       "Main:",
                                       "lcd sonic target")) {
        printk("Main: LCD + sonic test target select failed\n");
        return;
    }

    (void)lcd_service_clear(0u);
    (void)lcd_service_print(0u, 0u, 0u, "DISTANCE");
    (void)lcd_service_print(0u, 0u, 8u, "        ");
    (void)lcd_service_print(0u, 1u, 0u, "starting...");
    k_msleep(200u);

    while (true) {
        err = ultrasonic_service_get_distance_mm(0u, &distance_mm);

        if (err == DBC_OK) {
            last_good_distance_mm = distance_mm;
            have_good_distance = true;
            (void)snprintf(line_buf, sizeof(line_buf), "%4u mm     ", (unsigned)distance_mm);
            (void)lcd_service_print(0u, 1u, 0u, line_buf);
        } else if (have_good_distance) {
            (void)snprintf(line_buf, sizeof(line_buf), "%4u mm     ", (unsigned)last_good_distance_mm);
            (void)lcd_service_print(0u, 1u, 0u, line_buf);
        }

        /* Re-send both lines every cycle so occasional dropped LCD DBAL frames
         * do not leave the display stuck with only the header. */
        (void)lcd_service_print(0u, 0u, 0u, "DISTANCE");
        (void)lcd_service_print(0u, 0u, 8u, "        ");
        k_msleep(100u);
    }
}

static bool run_quad_simultaneous_cycle_custom(uint32_t run_ms,
                                               uint32_t gap_ms,
                                               const char *label,
                                               uint32_t guard_motor_mask)
{
    static const uint32_t m0_speeds[] = { 200u };
    static const uint32_t m1_speeds[] = { 500u };
    static const uint32_t m2_speeds[] = { 800u };
    static const uint32_t m3_speeds[] = { 1100u };
    const size_t NUM_ROUNDS = ARRAY_SIZE(m0_speeds);
    const struct device *endswitch_gpio = DEVICE_DT_GET(CS_PROBE_GPIO_NODE);
    static const uint8_t primary_motors[] = { 0u, 2u };
    static const uint8_t secondary_motors[] = { 1u, 3u };
    bool all_ok = true;

    printk("Motor Toggle [%s]: Starting %u rounds.\n", label, (uint32_t)NUM_ROUNDS);

    for (size_t i = 0u; i < NUM_ROUNDS; i++) {
        const uint32_t speeds[4] = {
            m0_speeds[i],
            m1_speeds[i],
            m2_speeds[i],
            m3_speeds[i],
        };

        printk("Motor Toggle [%s]: round %u/%u speeds m0=%u m1=%u m2=%u m3=%u\n",
               label,
               (uint32_t)(i + 1u),
               (uint32_t)NUM_ROUNDS,
               m0_speeds[i],
               m1_speeds[i],
               m2_speeds[i],
               m3_speeds[i]);

         stop_all_motors_now();
         k_msleep(50u);

        if (!select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                           "Motor Toggle",
                                           "primary start")) {
            printk("Motor Toggle [%s]: round %u primary target select failed\n",
                   label,
                   (uint32_t)(i + 1u));
            all_ok = false;
        } else {
            for (size_t idx = 0u; idx < ARRAY_SIZE(primary_motors); idx++) {
                uint8_t motor = primary_motors[idx];

                if (!start_motor_on_selected_target(motor, speeds[motor], "Motor Toggle")) {
                    printk("Motor Toggle [%s]: round %u motor%u start sequence failed\n",
                           label,
                           (uint32_t)(i + 1u),
                           motor);
                    all_ok = false;
                }
            }
        }

        k_usleep(300u);

        if (!select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                           "Motor Toggle",
                                           "secondary start")) {
            printk("Motor Toggle [%s]: round %u secondary target select failed\n",
                   label,
                   (uint32_t)(i + 1u));
            all_ok = false;
        } else {
            for (size_t idx = 0u; idx < ARRAY_SIZE(secondary_motors); idx++) {
                uint8_t motor = secondary_motors[idx];

                if (!start_motor_on_selected_target(motor, speeds[motor], "Motor Toggle")) {
                    printk("Motor Toggle [%s]: round %u motor%u start sequence failed\n",
                           label,
                           (uint32_t)(i + 1u),
                           motor);
                    all_ok = false;
                }
            }
        }

        k_usleep(300u);

        printk("Motor Toggle [%s]: all motors running for %ums\n", label, (unsigned)run_ms);
        if (guard_motor_mask != 0u) {
            if (!wait_with_endswitch_guard(endswitch_gpio, run_ms, guard_motor_mask)) {
                printk("Motor Toggle [%s]: simultaneous run interrupted by end-switch\n", label);
            }
        } else {
            k_msleep(run_ms);
        }

        if (!select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                           "Motor Toggle",
                                           "primary stop")) {
            printk("Motor Toggle [%s]: round %u primary stop target select failed\n",
                   label,
                   (uint32_t)(i + 1u));
            all_ok = false;
        } else {
            for (size_t idx = 0u; idx < ARRAY_SIZE(primary_motors); idx++) {
                uint8_t motor = primary_motors[idx];

                if (!stop_motor_on_selected_target(motor, "Motor Toggle")) {
                    printk("Motor Toggle [%s]: round %u motor%u stop sequence failed\n",
                           label,
                           (uint32_t)(i + 1u),
                           motor);
                    all_ok = false;
                }
            }
        }

        k_usleep(300u);

        if (!select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                           "Motor Toggle",
                                           "secondary stop")) {
            printk("Motor Toggle [%s]: round %u secondary stop target select failed\n",
                   label,
                   (uint32_t)(i + 1u));
            all_ok = false;
        } else {
            for (size_t idx = 0u; idx < ARRAY_SIZE(secondary_motors); idx++) {
                uint8_t motor = secondary_motors[idx];

                if (!stop_motor_on_selected_target(motor, "Motor Toggle")) {
                    printk("Motor Toggle [%s]: round %u motor%u stop sequence failed\n",
                           label,
                           (uint32_t)(i + 1u),
                           motor);
                    all_ok = false;
                }
            }
        }

        k_msleep(gap_ms);
    }

    printk("Motor Toggle [%s]: All rounds complete.\n", label);
    return all_ok;
}

// Example DBus service callback for testing
void my_test_service_handler(const uint8_t* const data, uint8_t data_len) {
    printk("Main: Received DBus message in test service handler! DataLen: %u, Data: ", data_len);
    for (uint8_t i = 0; i < data_len; i++) {
        printk("0x%02x ", data[i]);
    }
    printk("\n");
}

static void dbal_bootstrap_phase1(void)
{
    bool handler_ok;

    printk("DBAL Bootstrap [PHASE1]: init start\n");
    dbal_init();

    handler_ok = dbal_register_service_handler(DBAL_TEST_SERVICE_ID,
                                               DBAL_TYPE_EVENT,
                                               my_test_service_handler);

    printk("DBAL Bootstrap [PHASE1]: handler registration=%s (service=0x%04x type=%u)\n",
           handler_ok ? "OK" : "FAIL",
           DBAL_TEST_SERVICE_ID,
           DBAL_TYPE_EVENT);

    printk("DBAL Bootstrap [PHASE1]: connection_state=%d\n", dbal_get_connection_state());

    {
        uint8_t boot_payload[2] = { 0x50u, 0x31u };
        bool event_ok = dbal_send_event(DBAL_TEST_SERVICE_ID,
                                        DBAL_TEST_COMMAND_ID,
                                        boot_payload,
                                        sizeof(boot_payload));
        printk("DBAL Bootstrap [PHASE1]: startup event send=%s\n", event_ok ? "OK" : "FAIL");
    }
}

static enum DBC_Error write_motor_reg32(uint8_t motor_index, uint16_t reg_offset, uint32_t value)
{
    uint16_t addr = (uint16_t)(MOTOR_REG_BASE + ((uint16_t)motor_index * MOTOR_REG_STRIDE) + reg_offset);
    enum DBC_Error err = DBCDRV_writeReg32((enum DBC_RegAddr)addr, value);

    printk("Main: DIRECT_REG_WRITE motor=%u addr=0x%04x val=0x%08x err=%d\n",
           (unsigned)motor_index,
           (unsigned)addr,
           (unsigned)value,
           err);

    return err;
}

static enum DBC_Error read_motor_reg32(uint8_t motor_index, uint16_t reg_offset, uint32_t *value)
{
    uint16_t addr = (uint16_t)(MOTOR_REG_BASE + ((uint16_t)motor_index * MOTOR_REG_STRIDE) + reg_offset);
    enum DBC_Error err = DBCDRV_readReg32((enum DBC_RegAddr)addr, value);

    if (err == DBC_OK) {
        printk("Main: DIRECT_REG_READ motor=%u addr=0x%04x val=0x%08x err=%d\n",
               (unsigned)motor_index,
               (unsigned)addr,
               (unsigned)*value,
               err);
    } else {
        printk("Main: DIRECT_REG_READ motor=%u addr=0x%04x err=%d\n",
               (unsigned)motor_index,
               (unsigned)addr,
               err);
    }

    return err;
}

static enum DBCDRV_SpiTarget target_for_motor(uint8_t motor_index)
{
    return ((motor_index & 0x1u) == 0u)
        ? DBCDRV_SPI_TARGET_PRIMARY_PICO
        : DBCDRV_SPI_TARGET_SECONDARY_PICO;
}


/* 2-round finite test for all 4 logical motors.
 * Mapping:
 * - motor0 (A) on Pico1 PRIMARY
 * - motor1 (A) on Pico2 SECONDARY
 * - motor2 (B) on Pico1 PRIMARY
 * - motor3 (B) on Pico2 SECONDARY
 *
 * Each round runs motors individually (one at a time) with target switching,
 * then performs a full shutdown on all 4 motors. */
static bool run_dual_motor_simul_cycle(void)
{
    static const uint32_t m0_speeds[] = { 220u, 360u };
    static const uint32_t m1_speeds[] = { 360u, 180u };
    static const uint32_t m2_speeds[] = { 260u, 300u };
    static const uint32_t m3_speeds[] = { 320u, 200u };
    static const uint32_t RUN_MS      = 1200u;
    static const uint32_t GAP_MS      = 250u;
    const size_t NUM_ROUNDS = ARRAY_SIZE(m0_speeds);
    const struct device *endswitch_gpio = DEVICE_DT_GET(CS_PROBE_GPIO_NODE);
    bool all_ok = true;
    enum DBC_Error target_err;

    printk("Motor Toggle [QUAD_INDIVIDUAL_V1]: Starting %u rounds.\n", (uint32_t)NUM_ROUNDS);

    for (size_t i = 0u; i < NUM_ROUNDS; i++) {
        enum DBC_Error err;

        const uint32_t speeds[4] = {
            m0_speeds[i],
            m1_speeds[i],
            m2_speeds[i],
            m3_speeds[i],
        };

        printk("Motor Toggle: round %u/%u speeds m0=%u m1=%u m2=%u m3=%u\n",
               (uint32_t)(i + 1u), (uint32_t)NUM_ROUNDS,
               m0_speeds[i], m1_speeds[i], m2_speeds[i], m3_speeds[i]);

        for (uint8_t motor = 0u; motor < 4u; motor++) {
            enum DBCDRV_SpiTarget target = target_for_motor(motor);

            target_err = DBCDRV_setSpiTarget(target);
            if (target_err != DBC_OK) {
                printk("Motor Toggle: round %u motor%u target err=%d\n",
                       (uint32_t)(i + 1u), motor, target_err);
                all_ok = false;
            }
            k_usleep(100u);

            err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
            if (err != DBC_OK) {
                printk("Motor Toggle: round %u motor%u pre disable err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, 0u);
            if (err != DBC_OK) {
                printk("Motor Toggle: round %u motor%u pre speed0 err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 1u);
            if (err != DBC_OK) {
                printk("Motor Toggle: round %u motor%u enable err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, speeds[motor]);
            if (err != DBC_OK) {
                printk("Motor Toggle: round %u motor%u speed err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            printk("Motor Toggle: round %u motor%u running at %u for %ums\n",
                   (uint32_t)(i + 1u), motor, speeds[motor], RUN_MS);
            if (motor == 0u) {
                bool completed = wait_with_endswitch_guard(endswitch_gpio,
                                                          RUN_MS,
                                                          (1u << motor));

                if (!completed) {
                    printk("Motor Toggle: round %u motor%u interrupted by end-switch\n",
                           (uint32_t)(i + 1u), motor);
                }
            } else {
                k_msleep(RUN_MS);
            }

            err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
            if (err != DBC_OK) {
                printk("Motor Toggle: round %u motor%u disable err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            k_msleep(GAP_MS);
        }
    }

    printk("Motor Toggle [QUAD_INDIVIDUAL_V1]: All rounds complete.\n");

    /* Final forced shutdown: send enable=0 to all 4 motors with delay
     * to ensure each Pico receives and processes the stop command. */
    printk("Motor Toggle: final shutdown sequence\n");
    for (uint8_t attempt = 0u; attempt < 5u; attempt++) {
        k_msleep(100);

        for (uint8_t motor = 0u; motor < 4u; motor++) {
            target_err = DBCDRV_setSpiTarget(target_for_motor(motor));
            (void)target_err;
            (void)write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
        }
        
        printk("Motor Toggle: shutdown attempt %u/5\n", (unsigned)(attempt + 1u));
    }
    printk("Motor Toggle: shutdown complete.\n");
    return all_ok;
}

static bool run_secondary_only_cycle(void)
{
    static const uint8_t motors[] = { 1u, 3u };
    static const uint32_t m1_speeds[] = { 360u, 180u };
    static const uint32_t m3_speeds[] = { 320u, 200u };
    static const uint32_t RUN_MS      = 1200u;
    static const uint32_t GAP_MS      = 250u;
    const size_t NUM_ROUNDS = ARRAY_SIZE(m1_speeds);
    bool all_ok = true;

    printk("Motor Toggle [SECONDARY_ONLY_V1]: Starting %u rounds.\n", (uint32_t)NUM_ROUNDS);

    for (size_t i = 0u; i < NUM_ROUNDS; i++) {
        const uint32_t speeds[] = { m1_speeds[i], m3_speeds[i] };

        printk("Motor Toggle [SECONDARY_ONLY_V1]: round %u/%u speeds m1=%u m3=%u\n",
               (uint32_t)(i + 1u), (uint32_t)NUM_ROUNDS,
               (unsigned)m1_speeds[i], (unsigned)m3_speeds[i]);

        for (size_t idx = 0u; idx < ARRAY_SIZE(motors); idx++) {
            uint8_t motor = motors[idx];
            enum DBC_Error target_err;
            enum DBC_Error err;

            target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_SECONDARY_PICO);
            if (target_err != DBC_OK) {
                printk("Motor Toggle [SECONDARY_ONLY_V1]: round %u motor%u target err=%d\n",
                       (uint32_t)(i + 1u), motor, target_err);
                all_ok = false;
            }
            k_usleep(100u);

            err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
            if (err != DBC_OK) {
                printk("Motor Toggle [SECONDARY_ONLY_V1]: round %u motor%u pre disable err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, 0u);
            if (err != DBC_OK) {
                printk("Motor Toggle [SECONDARY_ONLY_V1]: round %u motor%u pre speed0 err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 1u);
            if (err != DBC_OK) {
                printk("Motor Toggle [SECONDARY_ONLY_V1]: round %u motor%u enable err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, speeds[idx]);
            if (err != DBC_OK) {
                printk("Motor Toggle [SECONDARY_ONLY_V1]: round %u motor%u speed err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            printk("Motor Toggle [SECONDARY_ONLY_V1]: round %u motor%u running at %u for %ums\n",
                   (uint32_t)(i + 1u), motor, (unsigned)speeds[idx], (unsigned)RUN_MS);
            k_msleep(RUN_MS);

            err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
            if (err != DBC_OK) {
                printk("Motor Toggle [SECONDARY_ONLY_V1]: round %u motor%u disable err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }

            k_msleep(GAP_MS);
        }
    }

    printk("Motor Toggle [SECONDARY_ONLY_V1]: All rounds complete.\n");
    printk("Motor Toggle [SECONDARY_ONLY_V1]: final shutdown sequence\n");

    for (uint8_t attempt = 0u; attempt < 5u; attempt++) {
        k_msleep(100u);
        (void)DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_SECONDARY_PICO);
        (void)write_motor_reg32(1u, MOTOR_REG_ENABLE_OFFSET, 0u);
        (void)write_motor_reg32(3u, MOTOR_REG_ENABLE_OFFSET, 0u);
        printk("Motor Toggle [SECONDARY_ONLY_V1]: shutdown attempt %u/5\n",
               (unsigned)(attempt + 1u));
    }

    printk("Motor Toggle [SECONDARY_ONLY_V1]: shutdown complete.\n");
    return all_ok;
}

/* Finite test where all 4 motors are enabled in each round with
 * independent speeds, run concurrently, then stopped together. */
static bool run_quad_simultaneous_cycle(void)
{
    return run_quad_simultaneous_cycle_custom(3000u, 500u, "QUAD_SIMUL_V1", 0x07u);
}

static bool run_toggle_cycle_for_target(enum DBCDRV_SpiTarget target, const char *label)
{
    enum DBC_Error err;
    err = DBCDRV_setSpiTarget(target);
    printk("Main: target switch -> %s err=%d\n", label, err);
    DBCDRV_logSpiRouting(label);
    if (err != DBC_OK) {
        return false;
    }

#if DBUS_FORCE_SECONDARY_ONLY
    if (target == DBCDRV_SPI_TARGET_SECONDARY_PICO) {
        return run_secondary_only_cycle();
    }
#endif

    return run_dual_motor_simul_cycle();
}

static void run_readback_probe(void)
{
#if DBUS_ENABLE_READBACK_PROBE
#if DBUS_READBACK_PROBE_PRIMARY_ONLY
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } probes[] = {
        { 0u, 0x00000456u },
    };
#if DBUS_FORCE_SECONDARY_ONLY
    printk("Main: readback probe overriding secondary-only mode to test Pico1\n");
#endif
#elif DBUS_FORCE_SECONDARY_ONLY
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } probes[] = {
        { 1u, 0x00000456u },
    };
#else
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } probes[] = {
        { 0u, 0x00000123u },
        { 1u, 0x00000456u },
    };
#endif

    printk("Main: readback probe start\n");

    for (size_t i = 0u; i < ARRAY_SIZE(probes); i++) {
        uint32_t read_value = 0u;
        enum DBC_Error err;

        err = DBCDRV_setSpiTarget(target_for_motor(probes[i].motor));
        printk("Main: readback probe target motor=%u err=%d\n",
               (unsigned)probes[i].motor,
               err);
        if (err != DBC_OK) {
            continue;
        }

        err = write_motor_reg32(probes[i].motor, MOTOR_REG_SPEED_OFFSET, probes[i].speed);
        if (err != DBC_OK) {
            printk("Main: readback probe write failed motor=%u err=%d\n",
                   (unsigned)probes[i].motor,
                   err);
            continue;
        }

         err = write_motor_reg32(probes[i].motor, MOTOR_REG_ENABLE_OFFSET, 1u);
         if (err != DBC_OK) {
             printk("Main: readback probe enable failed motor=%u err=%d\n",
                 (unsigned)probes[i].motor,
                 err);
             continue;
         }

         printk("Main: readback probe motor=%u visible run start\n",
             (unsigned)probes[i].motor);
         k_msleep(300u);

        k_usleep(200u);

        err = read_motor_reg32(probes[i].motor, MOTOR_REG_SPEED_OFFSET, &read_value);
        printk("Main: readback probe result motor=%u expected=0x%08x got=0x%08x err=%d\n",
               (unsigned)probes[i].motor,
               (unsigned)probes[i].speed,
               (unsigned)read_value,
               err);

         (void)write_motor_reg32(probes[i].motor, MOTOR_REG_ENABLE_OFFSET, 0u);
    }

    printk("Main: readback probe end\n");
#endif
}

static void run_register_log_probe(void)
{
#if DBUS_ENABLE_REGISTER_LOG_PROBE
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } probes[] = {
        { 0u, 0x00000123u },
        { 2u, 0x00000789u },
    };

    printk("Main: register log probe start (primary motors 0 and 2 only)\n");

    for (size_t i = 0u; i < ARRAY_SIZE(probes); i++) {
        uint32_t enable_value = 0u;
        uint32_t speed_value = 0u;
        enum DBC_Error err;

        if (!select_spi_target_with_settle(target_for_motor(probes[i].motor),
                                           "Main:",
                                           "register log probe")) {
            printk("Main: register log probe target select failed motor=%u\n",
                   (unsigned)probes[i].motor);
            continue;
        }

        err = write_motor_reg32(probes[i].motor, MOTOR_REG_ENABLE_OFFSET, 0u);
        if (err != DBC_OK) {
            printk("Main: register log probe disable failed motor=%u err=%d\n",
                   (unsigned)probes[i].motor,
                   err);
            continue;
        }

        err = write_motor_reg32(probes[i].motor, MOTOR_REG_SPEED_OFFSET, probes[i].speed);
        if (err != DBC_OK) {
            printk("Main: register log probe speed write failed motor=%u err=%d\n",
                   (unsigned)probes[i].motor,
                   err);
            continue;
        }

        k_usleep(200u);

        err = read_motor_reg32(probes[i].motor, MOTOR_REG_ENABLE_OFFSET, &enable_value);
        if (err != DBC_OK) {
            printk("Main: register log probe enable read failed motor=%u err=%d\n",
                   (unsigned)probes[i].motor,
                   err);
            continue;
        }

        err = read_motor_reg32(probes[i].motor, MOTOR_REG_SPEED_OFFSET, &speed_value);
        if (err != DBC_OK) {
            printk("Main: register log probe speed read failed motor=%u err=%d\n",
                   (unsigned)probes[i].motor,
                   err);
            continue;
        }

        printk("Main: register log probe motor=%u wrote_speed=0x%08x read_speed=0x%08x read_enable=0x%08x\n",
               (unsigned)probes[i].motor,
               (unsigned)probes[i].speed,
               (unsigned)speed_value,
               (unsigned)enable_value);
    }

    printk("Main: register log probe end\n");
#endif
}

static void run_primary_live_quad_readback_test(void)
{
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } primary_probes[] = {
        { 0u, 0x00000123u },
        { 2u, 0x00000789u },
    };
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } secondary_support[] = {
        { 1u, 0x000001F4u },
        { 3u, 0x0000044Cu },
    };

    printk("Main: live quad primary readback test start\n");
    stop_all_motors_now();
    k_msleep(100u);

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                      "Main:",
                                      "live quad primary start")) {
        for (size_t i = 0u; i < ARRAY_SIZE(primary_probes); i++) {
            if (!start_motor_on_selected_target(primary_probes[i].motor,
                                                primary_probes[i].speed,
                                                "Main: live quad")) {
                printk("Main: live quad start failed motor=%u\n",
                       (unsigned)primary_probes[i].motor);
            }
        }
    }

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                      "Main:",
                                      "live quad secondary start")) {
        for (size_t i = 0u; i < ARRAY_SIZE(secondary_support); i++) {
            if (!start_motor_on_selected_target(secondary_support[i].motor,
                                                secondary_support[i].speed,
                                                "Main: live quad")) {
                printk("Main: live quad start failed motor=%u\n",
                       (unsigned)secondary_support[i].motor);
            }
        }
    }

    printk("Main: all 4 motors commanded on, probing primary readback while running\n");
    k_msleep(250u);

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                      "Main:",
                                      "live quad primary probe")) {
        for (size_t i = 0u; i < ARRAY_SIZE(primary_probes); i++) {
            uint32_t enable_value = 0u;
            uint32_t speed_value = 0u;
            enum DBC_Error err;

            err = read_motor_reg32(primary_probes[i].motor,
                                   MOTOR_REG_ENABLE_OFFSET,
                                   &enable_value);
            if (err != DBC_OK) {
                printk("Main: live quad enable read failed motor=%u err=%d\n",
                       (unsigned)primary_probes[i].motor,
                       err);
                continue;
            }

            err = read_motor_reg32(primary_probes[i].motor,
                                   MOTOR_REG_SPEED_OFFSET,
                                   &speed_value);
            if (err != DBC_OK) {
                printk("Main: live quad speed read failed motor=%u err=%d\n",
                       (unsigned)primary_probes[i].motor,
                       err);
                continue;
            }

            printk("Main: live quad probe motor=%u expected_speed=0x%08x read_speed=0x%08x read_enable=0x%08x\n",
                   (unsigned)primary_probes[i].motor,
                   (unsigned)primary_probes[i].speed,
                   (unsigned)speed_value,
                   (unsigned)enable_value);
        }
    }

    k_msleep(350u);
    stop_all_motors_now();
    printk("Main: live quad primary readback test end\n");
}

static void run_all4_live_quad_readback_test(void)
{
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } primary_probes[] = {
        { 0u, 0x00000123u },
        { 2u, 0x00000789u },
    };
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } secondary_start[] = {
        { 1u, 0x000001F4u },
        { 3u, 0x0000044Cu },
    };
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } secondary_probes[] = {
        { 3u, 0x0000044Cu },
        { 1u, 0x000001F4u },
    };

    printk("Main: live quad all-4 readback test start\n");
    stop_all_motors_now();
    k_msleep(100u);

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                      "Main:",
                                      "live quad primary start")) {
        for (size_t i = 0u; i < ARRAY_SIZE(primary_probes); i++) {
            if (!start_motor_on_selected_target(primary_probes[i].motor,
                                                primary_probes[i].speed,
                                                "Main: live quad")) {
                printk("Main: live quad start failed motor=%u\n",
                       (unsigned)primary_probes[i].motor);
            }
        }
    }

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                      "Main:",
                                      "live quad secondary start")) {
        for (size_t i = 0u; i < ARRAY_SIZE(secondary_start); i++) {
            if (!start_motor_on_selected_target(secondary_start[i].motor,
                                                secondary_start[i].speed,
                                                "Main: live quad")) {
                printk("Main: live quad start failed motor=%u\n",
                       (unsigned)secondary_start[i].motor);
            }
        }
    }

    printk("Main: all 4 motors commanded on, probing primary then secondary readback while running\n");
    k_msleep(250u);

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                      "Main:",
                                      "live quad primary probe")) {
        for (size_t i = 0u; i < ARRAY_SIZE(primary_probes); i++) {
            uint32_t enable_value = 0u;
            uint32_t speed_value = 0u;
            enum DBC_Error err;

            err = read_motor_reg32(primary_probes[i].motor,
                                   MOTOR_REG_ENABLE_OFFSET,
                                   &enable_value);
            if (err != DBC_OK) {
                printk("Main: live quad primary enable read failed motor=%u err=%d\n",
                       (unsigned)primary_probes[i].motor,
                       err);
                continue;
            }

            err = read_motor_reg32(primary_probes[i].motor,
                                   MOTOR_REG_SPEED_OFFSET,
                                   &speed_value);
            if (err != DBC_OK) {
                printk("Main: live quad primary speed read failed motor=%u err=%d\n",
                       (unsigned)primary_probes[i].motor,
                       err);
                continue;
            }

            printk("Main: live quad probe motor=%u expected_speed=0x%08x read_speed=0x%08x read_enable=0x%08x\n",
                   (unsigned)primary_probes[i].motor,
                   (unsigned)primary_probes[i].speed,
                   (unsigned)speed_value,
                   (unsigned)enable_value);
        }
    }

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                      "Main:",
                                      "live quad secondary probe")) {
        for (size_t i = 0u; i < ARRAY_SIZE(secondary_probes); i++) {
            uint32_t enable_value = 0u;
            uint32_t warmup_speed_value = 0u;
            uint32_t speed_value = 0u;
            enum DBC_Error err;
            enum DBC_Error warmup_err;

            err = read_motor_reg32(secondary_probes[i].motor,
                                   MOTOR_REG_ENABLE_OFFSET,
                                   &enable_value);
            if (err != DBC_OK) {
                printk("Main: live quad secondary enable read failed motor=%u err=%d\n",
                       (unsigned)secondary_probes[i].motor,
                       err);
                continue;
            }

            warmup_err = read_motor_reg32(secondary_probes[i].motor,
                                          MOTOR_REG_SPEED_OFFSET,
                                          &warmup_speed_value);
            if (warmup_err != DBC_OK) {
                printk("Main: live quad secondary warmup speed read failed motor=%u err=%d\n",
                       (unsigned)secondary_probes[i].motor,
                       warmup_err);
                continue;
            }

            err = read_motor_reg32(secondary_probes[i].motor,
                                   MOTOR_REG_SPEED_OFFSET,
                                   &speed_value);
            if (err != DBC_OK) {
                printk("Main: live quad secondary speed read failed motor=%u err=%d\n",
                       (unsigned)secondary_probes[i].motor,
                       err);
                continue;
            }

            printk("Main: live quad probe motor=%u expected_speed=0x%08x warmup=0x%08x warmup_err=%d read_speed=0x%08x read_enable=0x%08x\n",
                   (unsigned)secondary_probes[i].motor,
                   (unsigned)secondary_probes[i].speed,
                   (unsigned)warmup_speed_value,
                   warmup_err,
                   (unsigned)speed_value,
                   (unsigned)enable_value);
        }
    }

    k_msleep(350u);
    stop_all_motors_now();
    printk("Main: live quad all-4 readback test end\n");
}

static void run_secondary_live_quad_readback_test(void)
{
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } primary_support[] = {
        { 0u, 0x00000123u },
        { 2u, 0x00000789u },
    };
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } secondary_start[] = {
        { 1u, 0x000001F4u },
        { 3u, 0x0000044Cu },
    };
    static const struct {
        uint8_t motor;
        uint32_t speed;
    } secondary_probes[] = {
        { 3u, 0x0000044Cu },
        { 1u, 0x000001F4u },
    };

    printk("Main: live quad secondary readback test start\n");
    stop_all_motors_now();
    k_msleep(100u);

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                      "Main:",
                                      "live quad primary start")) {
        for (size_t i = 0u; i < ARRAY_SIZE(primary_support); i++) {
            if (!start_motor_on_selected_target(primary_support[i].motor,
                                                primary_support[i].speed,
                                                "Main: live quad")) {
                printk("Main: live quad start failed motor=%u\n",
                       (unsigned)primary_support[i].motor);
            }
        }
    }

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                      "Main:",
                                      "live quad secondary start")) {
        for (size_t i = 0u; i < ARRAY_SIZE(secondary_start); i++) {
            if (!start_motor_on_selected_target(secondary_start[i].motor,
                                                secondary_start[i].speed,
                                                "Main: live quad")) {
                printk("Main: live quad start failed motor=%u\n",
                       (unsigned)secondary_start[i].motor);
            }
        }
    }

    printk("Main: all 4 motors commanded on, probing secondary readback while running (order: motor3 then motor1)\n");
    k_msleep(250u);

    if (select_spi_target_with_settle(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                      "Main:",
                                      "live quad secondary probe")) {
        for (size_t i = 0u; i < ARRAY_SIZE(secondary_probes); i++) {
            uint32_t enable_value = 0u;
            uint32_t warmup_speed_value = 0u;
            uint32_t speed_value = 0u;
            enum DBC_Error err;
            enum DBC_Error warmup_err;

            err = read_motor_reg32(secondary_probes[i].motor,
                                   MOTOR_REG_ENABLE_OFFSET,
                                   &enable_value);
            if (err != DBC_OK) {
                printk("Main: live quad enable read failed motor=%u err=%d\n",
                       (unsigned)secondary_probes[i].motor,
                       err);
                continue;
            }

            warmup_err = read_motor_reg32(secondary_probes[i].motor,
                                          MOTOR_REG_SPEED_OFFSET,
                                          &warmup_speed_value);
            if (warmup_err != DBC_OK) {
                printk("Main: live quad warmup speed read failed motor=%u err=%d\n",
                       (unsigned)secondary_probes[i].motor,
                       warmup_err);
                continue;
            }

            err = read_motor_reg32(secondary_probes[i].motor,
                                   MOTOR_REG_SPEED_OFFSET,
                                   &speed_value);
            if (err != DBC_OK) {
                printk("Main: live quad speed read failed motor=%u err=%d\n",
                       (unsigned)secondary_probes[i].motor,
                       err);
                continue;
            }

                 if (secondary_probes[i].motor == 1u) {
                static const uint32_t delay_sweep_us[] = { 0u, 100u, 500u };
                uint32_t sweep_values[ARRAY_SIZE(delay_sweep_us)] = { speed_value, 0u, 0u };
                int sweep_errs[ARRAY_SIZE(delay_sweep_us)] = { err, 0, 0 };

                for (size_t sweep_index = 1u; sweep_index < ARRAY_SIZE(delay_sweep_us); sweep_index++) {
                    k_usleep(delay_sweep_us[sweep_index]);
                    sweep_errs[sweep_index] = read_motor_reg32(secondary_probes[i].motor,
                                                               MOTOR_REG_SPEED_OFFSET,
                                                               &sweep_values[sweep_index]);
                    if (sweep_errs[sweep_index] != DBC_OK) {
                        printk("Main: live quad delayed speed read failed motor=%u delay_us=%u err=%d\n",
                               (unsigned)secondary_probes[i].motor,
                               (unsigned)delay_sweep_us[sweep_index],
                               sweep_errs[sweep_index]);
                    }
                }

                  printk("Main: live quad probe motor=%u expected_speed=0x%08x read_enable=0x%08x warmup=0x%08x warmup_err=%d delay0=0x%08x err0=%d delay100=0x%08x err100=%d delay500=0x%08x err500=%d\n",
                       (unsigned)secondary_probes[i].motor,
                       (unsigned)secondary_probes[i].speed,
                       (unsigned)enable_value,
                      (unsigned)warmup_speed_value,
                      warmup_err,
                       (unsigned)sweep_values[0],
                       sweep_errs[0],
                       (unsigned)sweep_values[1],
                       sweep_errs[1],
                       (unsigned)sweep_values[2],
                       sweep_errs[2]);
                  continue;
                 }

                 printk("Main: live quad probe motor=%u expected_speed=0x%08x warmup=0x%08x warmup_err=%d read_speed=0x%08x read_enable=0x%08x\n",
                     (unsigned)secondary_probes[i].motor,
                     (unsigned)secondary_probes[i].speed,
                     (unsigned)warmup_speed_value,
                     warmup_err,
                     (unsigned)speed_value,
                     (unsigned)enable_value);
        }
    }

    k_msleep(350u);
    stop_all_motors_now();
    printk("Main: live quad secondary readback test end\n");
}

static uint32_t g_secondary_cycle_counter = 0u;



int main(void)
{
    uint32_t run_pass = 0u;
    uint32_t run_fail = 0u;
    enum DBC_Error target_err;
    enum DBC_Error pulse_err;
    const struct device *probe_gpio = DEVICE_DT_GET(CS_PROBE_GPIO_NODE);

    printk("Hello from Zephyr DBus Driver project! [%s]\n", RW612_BOOT_BANNER);
    printk("%s\n", RW612_BUILD_MARKER);
    printk("Main: mode=%s\n", RW612_MODE_LABEL);
    printk("Repeatability mode: %u run(s). auto_test=%u\n",
           DBUS_REPEATABILITY_RUNS,
           (unsigned)DBUS_ENABLE_AUTO_MOTOR_TEST);

#if DBUS_ENABLE_ENDSWITCH_TEST
    init_endswitch_inputs(probe_gpio);
    run_motor0_endswitch_test(probe_gpio);
    run_register_log_probe();
    return 0;
#endif

#if DBUS_SHORT_ALL4_CONFIRM_TEST
    run_short_all4_confirm_test(probe_gpio);
    return 0;
#endif

#if DBUS_ALL_TOGETHER_TEST
    run_all_together_test(probe_gpio);
    return 0;
#endif

#if DBUS_LCD_SONIC_TEST
    run_lcd_sonic_test();
    return 0;
#endif

#if DBUS_SHORT_PRIMARY_STOPPED_READBACK_TEST
    run_short_primary_stopped_readback_test();
    return 0;
#endif

#if DBUS_FINAL_ALL4_SWITCH_TEST
    run_final_all4_switch_test(probe_gpio);
    return 0;
#endif

#if DBUS_ALL4_LIVE_QUAD_READBACK_TEST
    printk("Main: ALL-4 live quad readback mode active\n");
    run_all4_live_quad_readback_test();
    return 0;
#endif

#if DBUS_SECONDARY_LIVE_QUAD_READBACK_TEST
    printk("Main: SECONDARY live quad readback mode active\n");
    run_secondary_live_quad_readback_test();
    return 0;
#endif

#if DBUS_PRIMARY_LIVE_QUAD_READBACK_TEST
    printk("Main: PRIMARY live quad readback mode active\n");
    run_primary_live_quad_readback_test();
    return 0;
#endif

#if DBUS_PRIMARY_REGISTER_LOG_ONLY
    printk("Main: PRIMARY register log-only mode active\n");
    run_register_log_probe();
    return 0;
#endif

#if DBUS_SCK_PROBE_ONLY
    if (!device_is_ready(probe_gpio)) {
        printk("Main: GPIO probe device not ready (SCK probe)\n");
        return -1;
    }
    printk("Main: SCK probe-only mode ENABLED. Pulsing RW612 GPIO7...\n");
    pulse_gpio_probe_pin(probe_gpio, 7u, 100u, 1000u, 1000u);
    printk("Main: SCK probe-only complete.\n");
    return 0;
#endif

#if !DBUS_DISABLE_DBAL_BOOTSTRAP
    dbal_bootstrap_phase1();
#else
    printk("Main: DBAL bootstrap disabled for SPI isolation\n");
#endif

#if DBUS_READBACK_PROBE_PRIMARY_ONLY
    target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_PRIMARY_PICO);
    printk("Main: SPI target select primary -> %s (err=%d)\n",
        (target_err == DBC_OK) ? "OK" : "FAIL",
        target_err);
#else
    target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_SECONDARY_PICO);
    printk("Main: SPI target select secondary -> %s (err=%d)\n",
        (target_err == DBC_OK) ? "OK" : "FAIL",
        target_err);
#endif
    DBCDRV_logSpiRouting("main_after_target_select");

#if !DBUS_SKIP_GPIO_PROBES
    if (!device_is_ready(probe_gpio)) {
        printk("Main: GPIO probe device not ready\n");
    } else {
        pulse_gpio_probe_pin(probe_gpio, 10u, 5u, 20000u, 20000u);
        pulse_gpio_probe_pin(probe_gpio, 11u, 5u, 20000u, 20000u);
    }
    pulse_err = DBCDRV_pulseCs(DBCDRV_SPI_TARGET_SECONDARY_PICO, 5u, 20000u, 20000u);
    printk("Main: CS2 pulse test (5x,20ms/20ms) -> %s (err=%d)\n",
        (pulse_err == DBC_OK) ? "OK" : "FAIL", pulse_err);
    DBCDRV_logSpiRouting("main_after_cs2_pulse_test");
#else
    (void)pulse_err;
    (void)probe_gpio;
#endif

#if !DBUS_DISABLE_DBAL_BOOTSTRAP
    (void)lcd_service_clear(0u);
    (void)lcd_service_print(0u, 0u, 0u, "DBAL READY");
#endif

    /* Wait for Pico SPI slave to complete its startup. */
    k_msleep(1000);

    /* Boot-time pre-disable: ensure motors start OFF even if a previous
     * firmware run left them enabled (e.g. after a reboot).
     * Mapping: PRIMARY owns motors 0+2, SECONDARY owns motors 1+3. */
    printk("Main: boot-time motor pre-disable%s\n",
           DBUS_FORCE_SECONDARY_ONLY ? " (secondary-only)" : "");
    for (uint8_t pd = 0u; pd < 3u; pd++) {
    #if DBUS_READBACK_PROBE_PRIMARY_ONLY
        target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_PRIMARY_PICO);
        (void)write_motor_reg32(0u, MOTOR_REG_ENABLE_OFFSET, 0u);
        (void)write_motor_reg32(2u, MOTOR_REG_ENABLE_OFFSET, 0u);
    #else
#if !DBUS_FORCE_SECONDARY_ONLY
        /* Disable motors 0+2 on PRIMARY */
        target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_PRIMARY_PICO);
        (void)write_motor_reg32(0u, MOTOR_REG_ENABLE_OFFSET, 0u);
        (void)write_motor_reg32(2u, MOTOR_REG_ENABLE_OFFSET, 0u);
#endif
        /* Disable motors 1+3 on SECONDARY */
        target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_SECONDARY_PICO);
        (void)write_motor_reg32(1u, MOTOR_REG_ENABLE_OFFSET, 0u);
        (void)write_motor_reg32(3u, MOTOR_REG_ENABLE_OFFSET, 0u);
#endif

        k_msleep(150u);
    }
    printk("Main: boot-time pre-disable done\n");

    if (DBUS_ENABLE_AUTO_MOTOR_TEST) {
        for (uint32_t run = 1u; run <= DBUS_REPEATABILITY_RUNS; run++) {
            bool toggle_ok;

            printk("\n==== Repeat Run %u/%u ====\n", run, DBUS_REPEATABILITY_RUNS);

            if (DBUS_FORCE_SECONDARY_ONLY) {
                printk("Main: starting secondary-only motor test (motor1,motor3)\n");
                toggle_ok = run_toggle_cycle_for_target(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                                        "secondary_only_cycle");
            } else {
                printk("Main: starting QUAD simultaneous motor test (motor0..motor3)\n");
                toggle_ok = run_quad_simultaneous_cycle();
            }

            if (toggle_ok) {
                run_pass++;
            } else {
                run_fail++;
            }
            printk("Main: RUN %u RESULT: %s\n", run, toggle_ok ? "PASS" : "FAIL");
            k_msleep(50);
        }
    } else {
        printk("Main: auto motor test disabled (DBUS_ENABLE_AUTO_MOTOR_TEST=0)\n");
    }

    printk("\n==== Repeatability Summary ====\n");
    printk("Total runs: %u\n", DBUS_REPEATABILITY_RUNS);
    printk("PASS: %u\n", run_pass);
    printk("FAIL: %u\n", run_fail);
    printk("Overall: %s\n", (run_fail == 0u) ? "PASS" : "FAIL");

#if DBUS_CONTINUOUS_SECONDARY_TEST
    printk("Main: entering continuous secondary SPI test mode\n");
    while (1) {
#if DBUS_SECONDARY_CS_DIAG_ONLY
        {
            enum DBC_Error ce = DBCDRV_pulseCs(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                               1u, 50000u, 50000u);
            printk("Main: CS-only diag pulse -> %s (err=%d)\n",
                   (ce == DBC_OK) ? "OK" : "FAIL", ce);
            DBCDRV_logSpiRouting("cs_diag_only");
            k_msleep(200);
            continue;
        }
#endif
#if DBUS_SECONDARY_CS_HEARTBEAT
        {
            enum DBC_Error ce = DBCDRV_pulseCs(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                               1u, 200u, 200u);
            if (ce != DBC_OK) {
                printk("Main: CS heartbeat pulse failed err=%d\n", ce);
            }
        }
#endif
        bool ok = run_toggle_cycle_for_target(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                              "secondary_continuous_cycle");
        g_secondary_cycle_counter++;
        printk("Main: continuous secondary cycle result=%s\n", ok ? "PASS" : "FAIL");
        if ((g_secondary_cycle_counter % 5u) == 0u) {
            printk("Main: secondary SPI cycles=%u\n", g_secondary_cycle_counter);
        }
        k_msleep(300);
    }
#endif

    /* Final stop command burst before exit. */
    printk("Main: final motor stop before exit%s\n",
           DBUS_FORCE_SECONDARY_ONLY ? " (secondary-only)" : "");
    for (uint8_t i = 0u; i < 3u; i++) {
    #if DBUS_READBACK_PROBE_PRIMARY_ONLY
        target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_PRIMARY_PICO);
        (void)write_motor_reg32(0u, MOTOR_REG_ENABLE_OFFSET, 0u);
        (void)write_motor_reg32(2u, MOTOR_REG_ENABLE_OFFSET, 0u);
    #else
#if !DBUS_FORCE_SECONDARY_ONLY
        target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_PRIMARY_PICO);
        (void)write_motor_reg32(0u, MOTOR_REG_ENABLE_OFFSET, 0u);
        (void)write_motor_reg32(2u, MOTOR_REG_ENABLE_OFFSET, 0u);
#endif
        target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_SECONDARY_PICO);
        (void)write_motor_reg32(1u, MOTOR_REG_ENABLE_OFFSET, 0u);
        (void)write_motor_reg32(3u, MOTOR_REG_ENABLE_OFFSET, 0u);
    #endif
        k_msleep(100u);
    }

    run_readback_probe();

    printk("Main: test complete, exiting main\n");
    return 0;
}
