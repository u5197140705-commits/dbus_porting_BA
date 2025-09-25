#ifndef ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_
#define ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_

#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <string.h>
#include "dbus_app_layer.h"
#include "mock_types.h" // Include the standalone mock types
#include "spi_abstraction.h" // Include the mock spi_abstraction.h


// Test suite setup and teardown declarations
void dbal_setup(void *fixture);
void dbal_teardown(void *fixture);

#endif // ZEPHYR_DBUS_DRIVER_TESTS_INC_TEST_COMMON_H_