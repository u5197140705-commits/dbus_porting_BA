cmake_minimum_required(VERSION 3.20.0)


project(dbus_driver_tests)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
target_sources(app PRIVATE src/test_common.c src/test_dbus_app_layer.c src/test_dbal_init.c src/minimal_test.c)
target_include_directories(app PRIVATE inc ../inc ${CMAKE_CURRENT_SOURCE_DIR}/inc)