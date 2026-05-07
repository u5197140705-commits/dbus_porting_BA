#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/spi.h>
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

#define DBAL_TEST_SERVICE_ID   0x7001u
#define DBAL_TEST_COMMAND_ID   0x0001u

#ifndef DBUS_REPEATABILITY_RUNS
#define DBUS_REPEATABILITY_RUNS 20u
#endif

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


/* Send enable + speed to motor_index on whichever Pico is currently selected.
 * Returns true on success. */
static bool send_motor_cmd(uint8_t motor_index, bool enable, int32_t speed)
{
    enum DBC_Error err;

    err = motor_service_set_enable(motor_index, enable);
    if (err != DBC_OK) {
        printk("send_motor_cmd: enable=%u motor=%u err=%d\n", (unsigned)enable, motor_index, err);
        return false;
    }
    if (enable) {
        err = motor_service_set_speed(motor_index, speed);
        if (err != DBC_OK) {
            printk("send_motor_cmd: speed=%ld motor=%u err=%d\n", (long)speed, motor_index, err);
            return false;
        }
    }
    return true;
}

static bool run_motor0_toggle_cycle(void)
{
    const uint32_t test_speeds[] = { 300u, 800u, 1200u };
    bool all_ok = true;

    printk("Motor Toggle [MOTOR0_TOGGLE_V1]: Starting %u cycles.\n", (uint32_t)ARRAY_SIZE(test_speeds));

    for (size_t i = 0; i < ARRAY_SIZE(test_speeds); i++) {
        uint32_t status_val = 0;
        int32_t feedback_val = 0;
        uint16_t distance_mm = 0u;
        char line0[17];
        char line1[17];
        enum DBC_Error err;

        printk("Motor Toggle: cycle %u enable=1 speed=%u (idx0+idx1)\n", (uint32_t)(i + 1u), test_speeds[i]);
        err = motor_service_set_enable(0u, true);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u enable write err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
            continue;
        }
        err = motor_service_set_enable(1u, true);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u enable write idx1 err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }

        err = motor_service_set_speed(0u, (int32_t)test_speeds[i]);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u speed write err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
            continue;
        }
        err = motor_service_set_speed(1u, (int32_t)test_speeds[i]);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u speed write idx1 err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }
        k_msleep(1000);

        err = motor_service_get_status(0u, &status_val);
        if (err == DBC_OK) {
            printk("Motor Toggle: cycle %u status=0x%08x\n", (uint32_t)(i + 1u), status_val);
            {
                uint32_t expected_status = MOTOR_STATUS_AVAILABLE | MOTOR_STATUS_ENABLED;
                if (status_val != expected_status) {
                    printk("❌ Motor Toggle: cycle %u status mismatch (expected 0x%08x)\n",
                           (uint32_t)(i + 1u), expected_status);
                    all_ok = false;
                }
            }
        } else {
            printk("Motor Toggle: cycle %u status read err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }

        err = motor_service_get_feedback(0u, &feedback_val);
        if (err == DBC_OK) {
            printk("Motor Toggle: cycle %u feedback=%d\n", (uint32_t)(i + 1u), feedback_val);
            if ((uint32_t)feedback_val != test_speeds[i]) {
                printk("❌ Motor Toggle: cycle %u feedback mismatch (expected %u)\n",
                       (uint32_t)(i + 1u), test_speeds[i]);
                all_ok = false;
            }
        } else {
            printk("Motor Toggle: cycle %u feedback read err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }

        err = ultrasonic_service_get_distance_mm(0u, &distance_mm);
        if (err == DBC_OK) {
            printk("Motor Toggle: cycle %u distance=%u mm\n", (uint32_t)(i + 1u), (unsigned)distance_mm);
        } else {
            printk("Motor Toggle: cycle %u distance read err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }

        (void)snprintf(line0, sizeof(line0), "SPD %4u RPM ", (unsigned)test_speeds[i]);
        (void)snprintf(line1, sizeof(line1), "DIST %4u MM ", (unsigned)distance_mm);
        (void)lcd_service_print(0u, 0u, 0u, line0);
        (void)lcd_service_print(0u, 1u, 0u, line1);
        (void)lcd_service_print(0u, 0u, 13u, "   ");
        (void)lcd_service_print(0u, 1u, 13u, "   ");

        printk("Motor Toggle: cycle %u enable=0 (idx0+idx1)\n", (uint32_t)(i + 1u));
        err = motor_service_set_enable(0u, false);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u disable write err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }
        err = motor_service_set_enable(1u, false);
        if (err != DBC_OK) {
            printk("Motor Toggle: cycle %u disable write idx1 err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }
        k_msleep(200);
    }

    printk("Motor Toggle [MOTOR0_TOGGLE_V1]: Complete.\n");
    return all_ok;
}



int main(void)
{
    uint32_t run_pass = 0u;
    uint32_t run_fail = 0u;

    printk("Hello from Zephyr DBus Driver project! [PRE_MOTOR_V1]\n");
    printk("Repeatability mode: %u run(s).\n", DBUS_REPEATABILITY_RUNS);

    dbal_bootstrap_phase1();

    (void)lcd_service_clear(0u);
    (void)lcd_service_print(0u, 0u, 0u, "DBAL READY");

    /* Wait for Pico SPI slave to complete its startup and enter the polling
     * loop before beginning any SPI exchanges. */
    k_msleep(1000);

    for (uint32_t run = 1u; run <= DBUS_REPEATABILITY_RUNS; run++) {
        bool toggle_ok;

        printk("\n==== Repeat Run %u/%u ====\n", run, DBUS_REPEATABILITY_RUNS);

        toggle_ok = run_motor0_toggle_cycle();

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

    return (run_fail == 0u) ? 0 : 1;
}
