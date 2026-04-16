#ifndef LCD_SERVICE_H__
#define LCD_SERVICE_H__

#include <stdint.h>

#include "dbus_driver_public.h"

#define LCD_SERVICE_ID            0x7102u
#define LCD_CMD_CLEAR             0x0001u
#define LCD_CMD_PRINT             0x0002u
#define LCD_CMD_SET_CURSOR        0x0003u
#define LCD_TEXT_MAX_LEN          13u

enum DBC_Error lcd_service_clear(uint8_t display_index);
enum DBC_Error lcd_service_set_cursor(uint8_t display_index, uint8_t row, uint8_t col);
enum DBC_Error lcd_service_print(uint8_t display_index, uint8_t row, uint8_t col, const char *text);

#endif
