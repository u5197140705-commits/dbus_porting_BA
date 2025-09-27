cmake_minimum_required(VERSION 3.20.0)


find_package(Zephyr COMPONENTS unittest REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dbus_driver_tests)
FILE(GLOB app_sources src/*.c ../src/dbus_app_layer.c) # Include all .c files in src/
target_sources(app PRIVATE ${app_sources})
target_include_directories(app PRIVATE inc ../inc)
target_compile_definitions(app PRIVATE CONFIG_SYS_CLOCK_TICKS_PER_SEC=1000) # Define for test build
target_compile_options(app PRIVATE -mfloat-abi=softfp -DARCH_STACK_PTR_ALIGN=8)