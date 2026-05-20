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

#define RW612_BUILD_MARKER "RW612 build marker: SECONDARY_READBACK_V1_2026_05_19"

#ifndef DBUS_REPEATABILITY_RUNS
#define DBUS_REPEATABILITY_RUNS 1u
#endif

#ifndef DBUS_ENABLE_AUTO_MOTOR_TEST
#define DBUS_ENABLE_AUTO_MOTOR_TEST 0
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
#define DBUS_FORCE_SECONDARY_ONLY 1
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
            k_msleep(RUN_MS);

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
    static const uint32_t m0_speeds[] = { 200u, 650u, 1100u };
    static const uint32_t m1_speeds[] = { 500u, 1000u, 250u };
    static const uint32_t m2_speeds[] = { 800u, 300u, 900u };
    static const uint32_t m3_speeds[] = { 1100u, 750u, 450u };
    static const uint32_t RUN_MS      = 3000u;
    static const uint32_t GAP_MS      = 500u;
    const size_t NUM_ROUNDS = ARRAY_SIZE(m0_speeds);
    bool all_ok = true;
    enum DBC_Error target_err;

    printk("Motor Toggle [QUAD_SIMUL_V1]: Starting %u rounds.\n", (uint32_t)NUM_ROUNDS);

    for (size_t i = 0u; i < NUM_ROUNDS; i++) {
        enum DBC_Error err;
        const uint32_t speeds[4] = {
            m0_speeds[i],
            m1_speeds[i],
            m2_speeds[i],
            m3_speeds[i],
        };

        printk("Motor Toggle [QUAD_SIMUL_V1]: round %u/%u speeds m0=%u m1=%u m2=%u m3=%u\n",
               (uint32_t)(i + 1u), (uint32_t)NUM_ROUNDS,
               m0_speeds[i], m1_speeds[i], m2_speeds[i], m3_speeds[i]);

        for (uint8_t motor = 0u; motor < 4u; motor++) {
            target_err = DBCDRV_setSpiTarget(target_for_motor(motor));
            if (target_err != DBC_OK) {
                printk("Motor Toggle [QUAD_SIMUL_V1]: round %u motor%u target err=%d\n",
                       (uint32_t)(i + 1u), motor, target_err);
                all_ok = false;
            }
            k_usleep(100u);

            err = write_motor_reg32(motor, MOTOR_REG_SPEED_OFFSET, speeds[motor]);
            if (err != DBC_OK) {
                printk("Motor Toggle [QUAD_SIMUL_V1]: round %u motor%u speed err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }
        }

        /* Issue enable commands in a second pass so all motors start closer in time. */
        for (uint8_t motor = 0u; motor < 4u; motor++) {
            target_err = DBCDRV_setSpiTarget(target_for_motor(motor));
            if (target_err != DBC_OK) {
                printk("Motor Toggle [QUAD_SIMUL_V1]: round %u motor%u target-enable err=%d\n",
                       (uint32_t)(i + 1u), motor, target_err);
                all_ok = false;
            }
            err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 1u);
            if (err != DBC_OK) {
                printk("Motor Toggle [QUAD_SIMUL_V1]: round %u motor%u enable err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }
        }

        printk("Motor Toggle [QUAD_SIMUL_V1]: all motors running for %ums\n", RUN_MS);
        k_msleep(RUN_MS);

        for (uint8_t motor = 0u; motor < 4u; motor++) {
            target_err = DBCDRV_setSpiTarget(target_for_motor(motor));
            if (target_err != DBC_OK) {
                printk("Motor Toggle [QUAD_SIMUL_V1]: round %u motor%u target-stop err=%d\n",
                       (uint32_t)(i + 1u), motor, target_err);
                all_ok = false;
            }
            err = write_motor_reg32(motor, MOTOR_REG_ENABLE_OFFSET, 0u);
            if (err != DBC_OK) {
                printk("Motor Toggle [QUAD_SIMUL_V1]: round %u motor%u disable err=%d\n",
                       (uint32_t)(i + 1u), motor, err);
                all_ok = false;
            }
        }

        k_msleep(GAP_MS);
    }

    printk("Motor Toggle [QUAD_SIMUL_V1]: All rounds complete.\n");
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

static uint32_t g_secondary_cycle_counter = 0u;



int main(void)
{
    uint32_t run_pass = 0u;
    uint32_t run_fail = 0u;
    enum DBC_Error target_err;
    enum DBC_Error pulse_err;
    const struct device *probe_gpio = DEVICE_DT_GET(CS_PROBE_GPIO_NODE);

    printk("Hello from Zephyr DBus Driver project! [PRE_MOTOR_V1]\n");
    printk("%s\n", RW612_BUILD_MARKER);
    printk("Main: mode=DIRECT_REGISTER_MOTOR_WRITES_V1\n");
        printk("Repeatability mode: %u run(s). auto_test=%u\n",
            DBUS_REPEATABILITY_RUNS,
            (unsigned)DBUS_ENABLE_AUTO_MOTOR_TEST);

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
