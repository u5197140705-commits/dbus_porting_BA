cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr_spi_master_test_app)

target_sources(app PRIVATE
  src/main.c
  src/dbus_message.c
)

target_include_directories(app PRIVATE inc)