cmake_minimum_required(VERSION 3.20.0)


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dbus_driver_tests)
FILE(GLOB app_sources src/*.c ../src/dbus_app_layer.c)
list(REMOVE_ITEM app_sources ../src/spi_abstraction.c)
target_sources(app PRIVATE ${app_sources})
target_include_directories(app PRIVATE inc ../inc ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_compile_options(app PRIVATE -mfloat-abi=softfp -DARCH_STACK_PTR_ALIGN=8)