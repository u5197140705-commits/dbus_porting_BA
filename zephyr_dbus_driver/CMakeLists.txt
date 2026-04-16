cmake_minimum_required(VERSION 3.20.0)

if(NOT DEFINED DTC_OVERLAY_FILE OR DTC_OVERLAY_FILE STREQUAL "")
	set(_dbus_overlay "${CMAKE_CURRENT_SOURCE_DIR}/boards/frdm_rw612/frdm_rw612.overlay")
	if(EXISTS "${_dbus_overlay}")
		set(DTC_OVERLAY_FILE "${_dbus_overlay}" CACHE STRING "Default devicetree overlay" FORCE)
	endif()
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr_dbus_driver)

# Add your source files here
target_sources(app PRIVATE src/main.c src/dbus_app_layer.c src/spi_abstraction.c src/dbus_config.c src/dbus_driver.c src/StdCrc.c src/motor_service.c src/lcd_service.c src/ultrasonic_service.c)
target_include_directories(app PRIVATE inc src)
target_compile_definitions(app PRIVATE ARCH_STACK_PTR_ALIGN=8) # Define for main application build

