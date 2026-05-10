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

#ifndef DBUS_REPEATABILITY_RUNS
#define DBUS_REPEATABILITY_RUNS 1u
#endif

#ifndef DBUS_CONTINUOUS_SECONDARY_TEST
#define DBUS_CONTINUOUS_SECONDARY_TEST 1
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
#define DBUS_FORCE_SECONDARY_ONLY 1
#endif

#ifndef DBUS_SKIP_GPIO_PROBES
#define DBUS_SKIP_GPIO_PROBES 1
#endif

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


/* Send enable + speed to motor_index on whichever Pico is currently selected.
 * Returns true on success. */
static bool run_motor0_toggle_cycle(void)
{
    const uint32_t test_speeds[] = { 300u };
    bool all_ok = true;

    printk("Motor Toggle [MOTOR0_TOGGLE_V1]: Starting %u cycles.\n", (uint32_t)ARRAY_SIZE(test_speeds));

    for (size_t i = 0; i < ARRAY_SIZE(test_speeds); i++) {
        enum DBC_Error err;

        printk("Motor Toggle: cycle %u enable=1 speed=%u (idx0+idx1)\n", (uint32_t)(i + 1u), test_speeds[i]);
        err = write_motor_reg32(0u, MOTOR_REG_ENABLE_OFFSET, 1u);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u enable write err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
            continue;
        }
        err = write_motor_reg32(1u, MOTOR_REG_ENABLE_OFFSET, 1u);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u enable write idx1 err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }

        err = write_motor_reg32(0u, MOTOR_REG_SPEED_OFFSET, (uint32_t)test_speeds[i]);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u speed write err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
            continue;
        }
        err = write_motor_reg32(1u, MOTOR_REG_SPEED_OFFSET, (uint32_t)test_speeds[i]);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u speed write idx1 err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }
        k_msleep(600);

        printk("Motor Toggle: cycle %u enable=0 (idx0+idx1)\n", (uint32_t)(i + 1u));
        err = write_motor_reg32(0u, MOTOR_REG_ENABLE_OFFSET, 0u);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u disable write err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }
        err = write_motor_reg32(1u, MOTOR_REG_ENABLE_OFFSET, 0u);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u disable write idx1 err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }
        k_msleep(150);
    }

    printk("Motor Toggle [MOTOR0_TOGGLE_V1]: Complete.\n");
    return all_ok;
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

    return run_motor0_toggle_cycle();
}

static uint32_t g_secondary_cycle_counter = 0u;



int main(void)
{
    uint32_t run_pass = 0u;
    uint32_t run_fail = 0u;
    enum DBC_Error target_err;
    enum DBC_Error pulse_err;
    const struct device *probe_gpio = DEVICE_DT_GET(CS_PROBE_GPIO_NODE);

    printk("Hello from Zephyr DBus Driver project! [PRE_MOTOR_V1]\n");
    printk("Main: mode=DIRECT_REGISTER_MOTOR_WRITES_V1\n");
    printk("Repeatability mode: %u run(s).\n", DBUS_REPEATABILITY_RUNS);

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

            target_err = DBCDRV_setSpiTarget(DBCDRV_SPI_TARGET_SECONDARY_PICO);
            printk("Main: SPI target select secondary -> %s (err=%d)\n",
                (target_err == DBC_OK) ? "OK" : "FAIL",
                target_err);
            DBCDRV_logSpiRouting("main_after_target_select");

#if !DBUS_SKIP_GPIO_PROBES
        if (!device_is_ready(probe_gpio)) {
            printk("Main: GPIO probe device not ready\n");
        } else {
            pulse_gpio_probe_pin(probe_gpio, 10u, 5u, 20000u, 20000u);
            pulse_gpio_probe_pin(probe_gpio, 11u, 5u, 20000u, 20000u);
        }
#endif

#if !DBUS_SKIP_GPIO_PROBES
            pulse_err = DBCDRV_pulseCs(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                           5u,
                           20000u,
                           20000u);
            printk("Main: CS2 pulse test (5x,20ms/20ms) -> %s (err=%d)\n",
                (pulse_err == DBC_OK) ? "OK" : "FAIL",
                pulse_err);
            DBCDRV_logSpiRouting("main_after_cs2_pulse_test");
#endif

    (void)lcd_service_clear(0u);
    (void)lcd_service_print(0u, 0u, 0u, "DBAL READY");

    /* Wait for Pico SPI slave to complete its startup and enter the polling
     * loop before beginning any SPI exchanges. */
    k_msleep(1000);

    for (uint32_t run = 1u; run <= DBUS_REPEATABILITY_RUNS; run++) {
        bool primary_ok;
        bool secondary_ok;
        bool toggle_ok;

        printk("\n==== Repeat Run %u/%u ====\n", run, DBUS_REPEATABILITY_RUNS);

#if DBUS_FORCE_SECONDARY_ONLY
        primary_ok = true;
        secondary_ok = false;

        printk("Main: starting SECONDARY-ONLY verification cycle\n");
        secondary_ok = run_toggle_cycle_for_target(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                                   "secondary_only_cycle");
#else
        printk("Main: starting PRIMARY verification cycle\n");
        primary_ok = run_toggle_cycle_for_target(DBCDRV_SPI_TARGET_PRIMARY_PICO,
                                                 "primary_cycle");

        printk("Main: starting SECONDARY verification cycle\n");
        secondary_ok = run_toggle_cycle_for_target(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                                   "secondary_cycle");
#endif

        toggle_ok = primary_ok && secondary_ok;

        if (toggle_ok) {
            run_pass++;
        } else {
            run_fail++;
        }

        printk("Main: RUN %u RESULT: %s\n", run, toggle_ok ? "PASS" : "FAIL");

        k_msleep(50);
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
            enum DBC_Error pulse_err = DBCDRV_pulseCs(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                                      1u,
                                                      50000u,
                                                      50000u);
            printk("Main: CS-only diag pulse -> %s (err=%d)\n",
                   (pulse_err == DBC_OK) ? "OK" : "FAIL",
                   pulse_err);
            DBCDRV_logSpiRouting("cs_diag_only");
            k_msleep(200);
            continue;
        }
#endif
#if DBUS_SECONDARY_CS_HEARTBEAT
        {
            enum DBC_Error pulse_err = DBCDRV_pulseCs(DBCDRV_SPI_TARGET_SECONDARY_PICO,
                                                      1u,
                                                      200u,
                                                      200u);
            if (pulse_err != DBC_OK) {
                printk("Main: CS heartbeat pulse failed err=%d\n", pulse_err);
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

    return (run_fail == 0u) ? 0 : 1;
}
