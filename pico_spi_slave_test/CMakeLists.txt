cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico_spi_slave_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_spi_slave_test
    main.c
)

add_executable(pico_spi_slave_test_pico2
    main.c
)
target_compile_definitions(pico_spi_slave_test_pico2 PRIVATE PICO_NODE_SLOT=2)

add_executable(pico_spi_slave_test_pico2_speedtrace
    main.c
)
target_compile_definitions(pico_spi_slave_test_pico2_speedtrace PRIVATE
    PICO_NODE_SLOT=2
    PICO_SPEED_TRACE=1
    PICO_HEARTBEAT_ENABLE=0
    PICO_LOG_IDLE_FLUSH_US=500000u
    PICO_FIRMWARE_VERSION="dbal_motor_v1_onehot_v3_isoD_v23_pico2_speedtrace_idle_2026-07-03"
)

add_executable(pico_spi_slave_test_pico2_shadowtrace
    main.c
)
target_compile_definitions(pico_spi_slave_test_pico2_shadowtrace PRIVATE
    PICO_NODE_SLOT=2
    PICO_PREFER_CS_START_READ_REARM=0
    PICO_FORCE_FULL_TX_PRELOAD=1
    PICO_SPI_CPHA_SETTING=SPI_CPHA_1
    PICO_TX_TRANSFORM_SETTING=1
    PICO_EXACT8_AUTO_TRANSFORM=1
    PICO_FIRMWARE_VERSION="dbal_motor_v1_onehot_v3_isoD_v44_pico2_txrol1_2026-07-02"
)

add_executable(pico_spi_slave_test_selftest
    main.c
)
target_compile_definitions(pico_spi_slave_test_selftest PRIVATE
    PICO_STARTUP_SELF_TEST=1
    PICO_FIRMWARE_VERSION=\"dbal_motor_v1_onehot_v3_isoD_v24_selftest_pico1_2026-05-28\"
)

add_executable(pico_spi_slave_test_pico2_selftest
    main.c
)
target_compile_definitions(pico_spi_slave_test_pico2_selftest PRIVATE
    PICO_NODE_SLOT=2
    PICO_STARTUP_SELF_TEST=1
    PICO_FIRMWARE_VERSION=\"dbal_motor_v1_onehot_v3_isoD_v24_selftest_pico2_2026-05-28\"
)

add_executable(pico_spi_slave_debug
    main_debug.c
)

pico_enable_stdio_uart(pico_spi_slave_test 0)
pico_enable_stdio_usb(pico_spi_slave_test 1)

pico_enable_stdio_uart(pico_spi_slave_test_pico2 0)
pico_enable_stdio_usb(pico_spi_slave_test_pico2 1)

pico_enable_stdio_uart(pico_spi_slave_test_pico2_speedtrace 0)
pico_enable_stdio_usb(pico_spi_slave_test_pico2_speedtrace 1)

pico_enable_stdio_uart(pico_spi_slave_test_pico2_shadowtrace 0)
pico_enable_stdio_usb(pico_spi_slave_test_pico2_shadowtrace 1)

pico_enable_stdio_uart(pico_spi_slave_test_selftest 0)
pico_enable_stdio_usb(pico_spi_slave_test_selftest 1)

pico_enable_stdio_uart(pico_spi_slave_test_pico2_selftest 0)
pico_enable_stdio_usb(pico_spi_slave_test_pico2_selftest 1)

pico_enable_stdio_uart(pico_spi_slave_debug 0)
pico_enable_stdio_usb(pico_spi_slave_debug 1)

target_link_libraries(pico_spi_slave_test
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_pwm
)

target_link_libraries(pico_spi_slave_test_pico2
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_pwm
)

target_link_libraries(pico_spi_slave_test_pico2_speedtrace
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_pwm
)

target_link_libraries(pico_spi_slave_test_pico2_shadowtrace
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_pwm
)

target_link_libraries(pico_spi_slave_test_selftest
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_pwm
)

target_link_libraries(pico_spi_slave_test_pico2_selftest
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_pwm
)

target_link_libraries(pico_spi_slave_debug
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_pwm
)

pico_add_extra_outputs(pico_spi_slave_test)
pico_add_extra_outputs(pico_spi_slave_test_pico2)
pico_add_extra_outputs(pico_spi_slave_test_pico2_speedtrace)
pico_add_extra_outputs(pico_spi_slave_test_pico2_shadowtrace)
pico_add_extra_outputs(pico_spi_slave_test_selftest)
pico_add_extra_outputs(pico_spi_slave_test_pico2_selftest)
pico_add_extra_outputs(pico_spi_slave_debug)
