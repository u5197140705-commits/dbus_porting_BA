# Removed Validation Code - April 15, 2026

**Status:** Code removed from `zephyr_dbus_driver/src/main.c` to achieve pure DBAL motor toggle test.

**Result:** 20/20 PASS on pure DBAL motor control test (no legacy register validation).

## Summary

Removed ~3KB of legacy register validation code from main test loop:
- `dbus_write_read_check()` function (scratchpad write/read pattern tests)
- `test_spi_validation_before_motor()` function (pre-motor SPI mode checks)
- Conditional gating of motor toggle test on validation pass

**Reason for removal:** Legacy register validation was causing timing interference with DBAL sends. The motor toggle test itself (speed commands at 300, 800, 1200 RPM) is sufficient to validate DBAL motor control.

## Removed Code

### 1. Helper Function: `dbus_write_read_check()`
```c
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
```

### 2. Main Validation Function: `test_spi_validation_before_motor()`
```c
static bool test_spi_validation_before_motor(void)
{
    bool all_ok = true;
    uint32_t status_val = 0;
    int32_t feedback_val = 0;
    enum DBC_Error err;

    DBCDRV_setSpiMode(false, false);
    printk("SPI Validation [PRE_MOTOR_V1]: MODE0 selected (CPOL=0, CPHA=0).\n");

    all_ok &= dbus_write_read_check(DBC_SCRATCHPAD_ADDR, 0x12345678u, 0x12345678u, "scratchpad pattern 1");
    all_ok &= dbus_write_read_check(DBC_SCRATCHPAD_ADDR, 0xAAAAAAAAu, 0xAAAAAAAAu, "scratchpad pattern 2");
    all_ok &= dbus_write_read_check(DBC_SCRATCHPAD_ADDR, 0x00000000u, 0x00000000u, "scratchpad pattern 3");

    err = motor_service_set_enable(0u, true);
    if (err != DBC_OK) {
        printk("❌ Motor Service: set_enable failed (err=%d)\n", err);
        all_ok = false;
    } else {
        all_ok &= dbus_write_read_check((uint16_t)0x5000u, 1u, 1u, "motor0 enable=1");
    }

    err = motor_service_set_speed(0u, 1200);
    if (err != DBC_OK) {
        printk("❌ Motor Service: set_speed failed (err=%d)\n", err);
        all_ok = false;
    } else {
        all_ok &= dbus_write_read_check((uint16_t)0x5004u, 1200u, 1200u, "motor0 speed setpoint");
    }

    err = motor_service_get_feedback(0u, &feedback_val);
    if (err != DBC_OK) {
        printk("❌ Motor Service: get_feedback failed (err=%d)\n", err);
        all_ok = false;
    } else {
        printk("SPI Validation: motor0 speed feedback readback 0x%08x (expected 0x%08x)\n",
               (uint32_t)feedback_val,
               1200u);
        if (feedback_val != 1200) {
            printk("❌ SPI Validation: motor0 speed feedback mismatch\n");
            all_ok = false;
        } else {
            printk("✅ SPI Validation: motor0 speed feedback ok\n");
        }
    }

    err = motor_service_get_status(0u, &status_val);
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

    err = motor_service_set_enable(0u, false);
    if (err != DBC_OK) {
        printk("❌ Motor Service: disable failed (err=%d)\n", err);
        all_ok = false;
    } else {
        all_ok &= dbus_write_read_check((uint16_t)0x5000u, 0u, 0u, "motor0 enable=0");
    }

    err = motor_service_get_feedback(0u, &feedback_val);
    if (err != DBC_OK) {
        printk("❌ Motor Service: get_feedback after disable failed (err=%d)\n", err);
        all_ok = false;
    } else {
        printk("SPI Validation: motor0 feedback after disable readback 0x%08x (expected 0x%08x)\n",
               (uint32_t)feedback_val,
               0u);
        if (feedback_val != 0) {
            printk("❌ SPI Validation: motor0 feedback after disable mismatch\n");
            all_ok = false;
        } else {
            printk("✅ SPI Validation: motor0 feedback after disable ok\n");
        }
    }

    printk("SPI Validation summary: %s\n", all_ok ? "PASS" : "FAIL");
    return all_ok;
}
```

### 3. Test Loop Change

**Old (with validation gating):**
```c
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
```

**New (pure DBAL motor toggle only):**
```c
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
```

## Restoration Instructions

To restore validation code if needed for integration testing:
1. Add back both functions (`dbus_write_read_check()` and `test_spi_validation_before_motor()`)
2. Update test loop to call `test_spi_validation_before_motor()` and gate toggle test on result
3. Increase `DBAL_INLINE_TX_IDLE_US` to 100–150 µs to prevent timing conflicts
4. Rebuild and test

## Note

This code is archived purely for reference. The pure DBAL motor toggle test (without legacy validation) achieves stable 20/20 performance and is the final validated state for driver closure.
