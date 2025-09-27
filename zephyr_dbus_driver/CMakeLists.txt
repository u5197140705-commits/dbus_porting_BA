cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr_dbus_driver)

# Add your source files here
target_sources(app PRIVATE src/main.c src/dbus_app_layer.c src/spi_abstraction.c src/dbus_config.c)
target_include_directories(app PRIVATE inc)
target_compile_definitions(app PRIVATE ARCH_STACK_PTR_ALIGN=8) # Define for main application build

