#include "lcd_service.h"

#include <stddef.h>
#include <string.h>

#include "dbus_app_layer.h"

static bool lcd_service_display_valid(uint8_t display_index)
{
    return display_index == 0u;
}

enum DBC_Error lcd_service_clear(uint8_t display_index)
{
    uint8_t payload[1];

    if (!lcd_service_display_valid(display_index)) {
        return DBC_ERROR;
    }

    payload[0] = display_index;

    if (!dbal_send_event(LCD_SERVICE_ID, LCD_CMD_CLEAR, payload, sizeof(payload))) {
        return DBC_ERROR;
    }

    return DBC_OK;
}

enum DBC_Error lcd_service_set_cursor(uint8_t display_index, uint8_t row, uint8_t col)
{
    uint8_t payload[3];

    if (!lcd_service_display_valid(display_index)) {
        return DBC_ERROR;
    }

    if (row > 1u) {
        row = 1u;
    }

    if (col > 15u) {
        col = 15u;
    }

    payload[0] = display_index;
    payload[1] = row;
    payload[2] = col;

    if (!dbal_send_event(LCD_SERVICE_ID, LCD_CMD_SET_CURSOR, payload, sizeof(payload))) {
        return DBC_ERROR;
    }

    return DBC_OK;
}

enum DBC_Error lcd_service_print(uint8_t display_index, uint8_t row, uint8_t col, const char *text)
{
    uint8_t payload[3u + LCD_TEXT_MAX_LEN];
    size_t text_len = 0u;

    if (!lcd_service_display_valid(display_index) || text == NULL) {
        return DBC_ERROR;
    }

    if (row > 1u) {
        row = 1u;
    }

    if (col > 15u) {
        col = 15u;
    }

    while (text[text_len] != '\0' && text_len < LCD_TEXT_MAX_LEN) {
        text_len++;
    }

    payload[0] = display_index;
    payload[1] = row;
    payload[2] = col;
    memcpy(&payload[3], text, text_len);

    if (!dbal_send_event(LCD_SERVICE_ID, LCD_CMD_PRINT, payload, (uint8_t)(3u + text_len))) {
        return DBC_ERROR;
    }

    return DBC_OK;
}
