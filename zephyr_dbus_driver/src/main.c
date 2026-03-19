#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/autoconf.h> // Explicitly include generated Kconfig definitions
#include "dbus_app_layer.h" // Include the ported DBus Application Layer
#include "spi_abstraction.h" // Include the SPI abstraction layer
#include "dbus_driver_public.h" // Include the DBus driver public API
#include <zephyr/sys_clock.h> // For K_SECONDS macro

#define MOTOR0_ENABLE_ADDR   0x5000u
#define MOTOR0_SPEED_ADDR    0x5004u
#define MOTOR0_FEEDBACK_ADDR 0x5008u
#define MOTOR0_STATUS_ADDR   0x500Cu

#define MOTOR_STATUS_ENABLED   0x00000001u
#define MOTOR_STATUS_AVAILABLE 0x00000002u

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

static bool dbus_write_read_check(uint16_t addr, uint32_t write_val, uint32_t expected_read, const char *label)
{
    uint32_t read_val = 0;
    enum DBC_Error err;

    printk("SPI Validation: %s write 0x%08x -> addr 0x%04x\n", label, write_val, addr);

    err = DBCDRV_writeReg32((enum DBC_RegAddr)addr, write_val);
    if (err != DBC_OK) {
        printk("❌ SPI Validation: %s write failed (err=%d)\n", label, err);
        return false;
    }

    k_msleep(5);

    err = DBCDRV_readReg32((enum DBC_RegAddr)addr, &read_val);
    if (err != DBC_OK) {
        printk("❌ SPI Validation: %s read failed (err=%d)\n", label, err);
        return false;
    }

    printk("SPI Validation: %s readback 0x%08x (expected 0x%08x)\n",
           label, read_val, expected_read);

    if (read_val != expected_read) {
        printk("❌ SPI Validation: %s mismatch\n", label);
        return false;
    }

    printk("✅ SPI Validation: %s ok\n", label);
    return true;
}

static bool test_spi_validation_before_motor(void)
{
    bool all_ok = true;
    uint32_t status_val = 0;
    enum DBC_Error err;

    DBCDRV_setSpiMode(false, false);
    printk("SPI Validation [PRE_MOTOR_V1]: MODE0 selected (CPOL=0, CPHA=0).\n");

    all_ok &= dbus_write_read_check(DBC_SCRATCHPAD_ADDR, 0x12345678u, 0x12345678u, "scratchpad pattern 1");
    all_ok &= dbus_write_read_check(DBC_SCRATCHPAD_ADDR, 0xAAAAAAAAu, 0xAAAAAAAAu, "scratchpad pattern 2");
    all_ok &= dbus_write_read_check(DBC_SCRATCHPAD_ADDR, 0x00000000u, 0x00000000u, "scratchpad pattern 3");

    all_ok &= dbus_write_read_check(MOTOR0_ENABLE_ADDR, 1u, 1u, "motor0 enable=1");
    all_ok &= dbus_write_read_check(MOTOR0_SPEED_ADDR, 1200u, 1200u, "motor0 speed setpoint");
    all_ok &= dbus_write_read_check(MOTOR0_FEEDBACK_ADDR, 0u, 1200u, "motor0 speed feedback");

    err = DBCDRV_readReg32((enum DBC_RegAddr)MOTOR0_STATUS_ADDR, &status_val);
    if (err != DBC_OK) {
        printk("❌ SPI Validation: motor0 status read failed (err=%d)\n", err);
        all_ok = false;
    } else {
        uint32_t expected_status = MOTOR_STATUS_AVAILABLE | MOTOR_STATUS_ENABLED;
        printk("SPI Validation: motor0 status=0x%08x (expected 0x%08x)\n", status_val, expected_status);
        if (status_val != expected_status) {
            printk("❌ SPI Validation: motor0 status mismatch\n");
            all_ok = false;
        } else {
            printk("✅ SPI Validation: motor0 status ok\n");
        }
    }

    all_ok &= dbus_write_read_check(MOTOR0_ENABLE_ADDR, 0u, 0u, "motor0 enable=0");
    all_ok &= dbus_write_read_check(MOTOR0_FEEDBACK_ADDR, 0u, 0u, "motor0 feedback after disable");

    printk("SPI Validation summary: %s\n", all_ok ? "PASS" : "FAIL");
    return all_ok;
}

static bool run_motor0_toggle_cycle(void)
{
    const uint32_t test_speeds[] = { 300u, 800u, 1200u };
    bool all_ok = true;

    printk("Motor Toggle [MOTOR0_TOGGLE_V1]: Starting %u cycles.\n", (uint32_t)ARRAY_SIZE(test_speeds));

    for (size_t i = 0; i < ARRAY_SIZE(test_speeds); i++) {
        uint32_t status_val = 0;
        uint32_t feedback_val = 0;
        enum DBC_Error err;

        printk("Motor Toggle: cycle %u enable=1 speed=%u\n", (uint32_t)(i + 1u), test_speeds[i]);
        (void)DBCDRV_writeReg32((enum DBC_RegAddr)MOTOR0_ENABLE_ADDR, 1u);
        (void)DBCDRV_writeReg32((enum DBC_RegAddr)MOTOR0_SPEED_ADDR, test_speeds[i]);
        k_msleep(20);

        err = DBCDRV_readReg32((enum DBC_RegAddr)MOTOR0_STATUS_ADDR, &status_val);
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

        err = DBCDRV_readReg32((enum DBC_RegAddr)MOTOR0_FEEDBACK_ADDR, &feedback_val);
        if (err == DBC_OK) {
            printk("Motor Toggle: cycle %u feedback=%u\n", (uint32_t)(i + 1u), feedback_val);
            if (feedback_val != test_speeds[i]) {
                printk("❌ Motor Toggle: cycle %u feedback mismatch (expected %u)\n",
                       (uint32_t)(i + 1u), test_speeds[i]);
                all_ok = false;
            }
        } else {
            printk("Motor Toggle: cycle %u feedback read err=%d\n", (uint32_t)(i + 1u), err);
            all_ok = false;
        }

        printk("Motor Toggle: cycle %u enable=0\n", (uint32_t)(i + 1u));
        (void)DBCDRV_writeReg32((enum DBC_RegAddr)MOTOR0_ENABLE_ADDR, 0u);
        k_msleep(20);
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
    /* Wait for Pico SPI slave to complete its startup and enter the polling
     * loop before beginning any SPI exchanges. */
    k_msleep(1000);

    for (uint32_t run = 1u; run <= DBUS_REPEATABILITY_RUNS; run++) {
        bool pre_ok;
        bool toggle_ok = false;
        bool run_ok;

        printk("\n==== Repeat Run %u/%u ====\n", run, DBUS_REPEATABILITY_RUNS);

        pre_ok = test_spi_validation_before_motor();
        printk("Main: PRE_MOTOR_V1 finished: %s\n", pre_ok ? "PASS" : "FAIL");

        if (pre_ok) {
            toggle_ok = run_motor0_toggle_cycle();
        } else {
            printk("Main: skipping MOTOR0_TOGGLE_V1 due to validation failure.\n");
        }

        run_ok = pre_ok && toggle_ok;
        if (run_ok) {
            run_pass++;
        } else {
            run_fail++;
        }

        printk("Main: RUN %u RESULT: %s\n", run, run_ok ? "PASS" : "FAIL");

        k_msleep(50);
    }

    printk("\n==== Repeatability Summary ====\n");
    printk("Total runs: %u\n", DBUS_REPEATABILITY_RUNS);
    printk("PASS: %u\n", run_pass);
    printk("FAIL: %u\n", run_fail);
    printk("Overall: %s\n", (run_fail == 0u) ? "PASS" : "FAIL");

    return (run_fail == 0u) ? 0 : 1;
}
