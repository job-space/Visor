#include "lcd1602_i2c_lib.h"
#include <string.h>

void LCD_ClearLine(uint8_t row)
{
    lcd1602_SetCursor(0, row);
    lcd1602_Print_text("                    ");
}

void LCD_PrintCentered(uint8_t row, const char *text)
{
    uint8_t len = strlen(text);
    uint8_t col = (20 - len) / 2;
    lcd1602_SetCursor(col, row);
    lcd1602_Print_text(text);
}
