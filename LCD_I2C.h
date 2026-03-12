#ifndef LCD_I2C_H
#define	LCD_I2C_H

#include <xc.h>
#include <stdint.h>

// Dirección I2C del LCD (Comúnmente 0x27 o 0x3F)
#define LCD_ADDR 0x27

void LCD_Init(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char* str);
void LCD_Clear(void);
void LCD_PrintWrapped(char* line1, char* line2);

#endif