# Zephyr DBus Driver Project Build Log

## Date
2025-09-08

## Build Command
```bash
export ZEPHYR_BASE=/home/wis3re/zephyrproject/zephyr && \
export ZEPHYR_SDK_INSTALL_DIR=/home/wis3re/zephyr-sdk-0.17.4 && \
rm -rf /home/wis3re/dbus_porting/zephyr_dbus_driver/build && \
cmake -B /home/wis3re/dbus_porting/zephyr_dbus_driver/build -GNinja -DBOARD=frdm_rw612 -S /home/wis3re/dbus_porting/zephyr_dbus_driver && \
cmake --build /home/wis3re/dbus_porting/zephyr_dbus_driver/build
```

## Build Output (Initial Build with Warnings)
```
Loading Zephyr default modules (Zephyr base).
-- Application: /home/wis3re/dbus_porting/zephyr_dbus_driver
-- CMake version: 3.22.1
-- Found Python3: /usr/bin/python3 (found suitable version "3.10.12", minimum required is "3.10") found components: Interpreter 
-- Cache files will be written to: /home/wis3re/.cache/zephyr
-- Zephyr version: 4.2.99 (/home/wis3re/zephyrproject/zephyr), build: v4.2.0-2872-g81413f07ce18
[15/161] Building C object CMakeFiles/app.dir/src/dbus_app_layer.c.objc.objplete.c.objc.objpe-to-str.h, include/generated/zephyr/otype-to-size.hude/generated/zephyr/syscall_list.h
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:563:13: warning: 'dbal_call_service_callback' defined but not used [-Wunused-function]
  563 | static bool dbal_call_service_callback(const struct dbal_instance* const inst, enum DBAL_MessageType dbal_type, uint16_t service_id, uint16_t command_id, const uint8_t* const bytes, uint8_t dbal_payload_len) { return false; }
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:562:13: warning: 'dbal_handle_con_msg' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:561:13: warning: 'dbal_is_received_req_resp_msg_to_be_ignored' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:560:13: warning: 'dbal_is_received_req_resp_msg_corrupt' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:559:13: warning: 'dbal_look_for_ack_msg_reception' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:558:13: warning: 'dbal_look_for_msg_reception' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:540:13: warning: 'dbal_check_for_next_msgs_to_send_and_trigger' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:539:13: warning: 'dbal_req_resp_tx_post_action' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:538:13: warning: 'dbal_con_msg_tx_post_repeat_trigger' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:537:16: warning: 'dbal_get_last_sending_status' defined but not used [-Wunused-function]
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:453:13: warning: 'dbal_save_msg_to_repeat' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:452:13: warning: 'dbal_is_msg_to_repeat_saved' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:399:13: warning: 'dbal_handle_req_resp_msg_tx_fail' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:341:13: warning: 'dbal_is_disable_pending' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:339:13: warning: 'dbal_is_io_tx_repeat_buffer_empty' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:338:13: warning: 'dbal_is_io_transmit_buffer_empty' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:333:13: warning: 'dbal_execute_set_non_task_code_sections' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:314:13: warning: 'dbal_is_event_already_acknowledged' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:308:13: warning: 'dbal_reset_events_to_ack' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:296:13: warning: 'dbal_is_msg_of_ack_dbal_type' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:278:30: warning: 'dbal_get_response_dbal_type' defined but not used [-Wunused-function]
      |                              ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:262:30: warning: 'dbal_convert_uint8_to_dbal_type' defined but not used [-Wunused-function]
      |                              ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:257:13: warning: 'dbal_is_temp_con_message' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:250:13: warning: 'dbal_is_con_transmit_index' defined but not used [-Wunused-function]
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~
/home/wis3re/dbus_porting/zephyr_dbus_driver/src/dbus_app_layer.c:94:13: warning: 'dbal_call_service_callback' defined but not used [-Wunused-function]
   94 | static bool dbal_call_service_callback(const struct dbal_instance* const inst, enum DBAL_MessageType dbal_type, uint16_t service_id, uint16_t command_id, const uint8_t* const bytes, uint8_t dbal_payload_len) { return false; }
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~
[161/161] Linking C executable zephyr/zephyr.elfphyr_final.dir/isr_tables.c.objc.obj
Memory region         Used Size  Region Size  %age Used
           FLASH:       36944 B        64 MB      0.06%
             RAM:       10760 B       960 KB      1.09%
            SMU1:        510 KB       510 KB    100.00%
            SMU2:        140 KB       140 KB    100.00%
        IDT_LIST:          0 GB        32 KB      0.00%
Generating files from /home/wis3re/dbus_porting/zephyr_dbus_driver/build/zephyr/zephyr.elf for board: frdm_rw612
```

## Build Output (After fixing redefinition and adding `__attribute__((unused))` to `dbus_app_layer.c` and updating `spi_abstraction.c` to use global `spi_cfg`)
```
Loading Zephyr default modules (Zephyr base).
-- Application: /home/wis3re/dbus_porting/zephyr_dbus_driver
-- CMake version: 3.22.1
-- Found Python3: /usr/bin/python3 (found suitable version "3.10.12", minimum required is "3.10") found components: Interpreter 
-- Cache files will be written to: /home/wis3re/.cache/zephyr
-- Zephyr version: 4.2.99 (/home/wis3re/zephyrproject/zephyr), build: v4.2.0-2872-g81413f07ce18
[161/161] Linking C executable zephyr/zephyr.elfphyr_final.dir/isr_tables.c.objc.obj
Memory region         Used Size  Region Size  %age Used
           FLASH:       36948 B        64 MB      0.06%
             RAM:       10776 B       960 KB      1.10%
            SMU1:        510 KB       510 KB    100.00%
            SMU2:        140 KB       140 KB    100.00%
        IDT_LIST:          0 GB        32 KB      0.00%
Generating files from /home/wis3re/dbus_porting/zephyr_dbus_driver/build/zephyr/zephyr.elf for board: frdm_rw612
```

## Build Output (After integrating `spi_abstraction` with `dbus_app_layer` and adding SPI message framing definitions)
```
Loading Zephyr default modules (Zephyr base).
-- Application: /home/wis3re/dbus_porting/zephyr_dbus_driver
-- CMake version: 3.22.1
-- Found Python3: /usr/bin/python3 (found suitable version "3.10.12", minimum required is "3.10") found components: Interpreter 
-- Cache files will be written to: /home/wis3re/.cache/zephyr
-- Zephyr version: 4.2.99 (/home/wis3re/zephyrproject/zephyr), build: v4.2.0-2872-g81413f07ce18
[161/161] Linking C executable zephyr/zephyr.elfphyr_final.dir/isr_tables.c.objc.obj
Memory region         Used Size  Region Size  %age Used
           FLASH:       37136 B        64 MB      0.06%
             RAM:       10776 B       960 KB      1.10%
            SMU1:        510 KB       510 KB    100.00%
            SMU2:        140 KB       140 KB    100.00%
        IDT_LIST:          0 GB        32 KB      0.00%
Generating files from /home/wis3re/dbus_porting/zephyr_dbus_driver/build/zephyr/zephyr.elf for board: frdm_rw612
```

## Build Output (After adding placeholder for SPI RX interrupt callback in `spi_abstraction.h` and `spi_abstraction.c`)
```
Loading Zephyr default modules (Zephyr base).
-- Application: /home/wis3re/dbus_porting/zephyr_dbus_driver
-- CMake version: 3.22.1
-- Found Python3: /usr/bin/python3 (found suitable version "3.10.12", minimum required is "3.10") found components: Interpreter 
-- Cache files will be written to: /home/wis3re/.cache/zephyr
-- Zephyr version: 4.2.99 (/home/wis3re/zephyrproject/zephyr), build: v4.2.0-2872-g81413f07ce18
[161/161] Linking C executable zephyr/zephyr.elfphyr_final.dir/isr_tables.c.objc.obj
Memory region         Used Size  Region Size  %age Used
           FLASH:       37136 B        64 MB      0.06%
             RAM:       10776 B       960 KB      1.10%
            SMU1:        510 KB       510 KB    100.00%
            SMU2:        140 KB       140 KB    100.00%
        IDT_LIST:          0 GB        32 KB      0.00%
Generating files from /home/wis3re/dbus_porting/zephyr_dbus_driver/build/zephyr/zephyr.elf for board: frdm_rw612