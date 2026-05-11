cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico_spi_slave_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_spi_slave_test
    main.c
)

add_executable(pico_spi_slave_debug
    main_debug.c
)

pico_enable_stdio_uart(pico_spi_slave_test 0)
pico_enable_stdio_usb(pico_spi_slave_test 1)

pico_enable_stdio_uart(pico_spi_slave_debug 0)
pico_enable_stdio_usb(pico_spi_slave_debug 1)

target_link_libraries(pico_spi_slave_test
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
pico_add_extra_outputs(pico_spi_slave_debug)
