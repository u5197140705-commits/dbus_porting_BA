cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dbuscan_slave_stub)

# Add the source file
target_sources(app PRIVATE src/dbuscan_slave_stub.c)