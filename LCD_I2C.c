#include "LCD_I2C.h"
#include <xc.h>

#define FCY 40535000UL
#include <libpic30.h>

// Pines: SCL = RB8, SDA = RB9
#define SCL_PIN     _LATB8
#define SDA_PIN     _LATB9
#define SDA_IN      _RB9
#define SCL_TRIS    _TRISB8
#define SDA_TRIS    _TRISB9

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RS        0x01

// ?? Bit-bang I2C ?????????????????????????????????????
static void SDA_HIGH(void) { SDA_TRIS = 1; }  // open-drain: soltar = alto
static void SDA_LOW(void)  { SDA_TRIS = 0; SDA_PIN = 0; }
static void SCL_HIGH(void) { SCL_TRIS = 1; }
static void SCL_LOW(void)  { SCL_TRIS = 0; SCL_PIN = 0; }

static void I2C_Start(void) {
    SDA_HIGH(); __delay_us(5);
    SCL_HIGH(); __delay_us(5);
    SDA_LOW();  __delay_us(5);  // SDA baja con SCL alto = START
    SCL_LOW();  __delay_us(5);
}

static void I2C_Stop(void) {
    SDA_LOW();  __delay_us(5);
    SCL_HIGH(); __delay_us(5);
    SDA_HIGH(); __delay_us(5);  // SDA sube con SCL alto = STOP
}

static void I2C_WriteByte(uint8_t byte) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        if (byte & 0x80) SDA_HIGH(); else SDA_LOW();
        __delay_us(2);
        SCL_HIGH(); __delay_us(5);
        SCL_LOW();  __delay_us(5);
        byte <<= 1;
    }
    // ACK: soltar SDA y pulsar SCL
    SDA_HIGH();
    __delay_us(2);
    SCL_HIGH(); __delay_us(5);
    SCL_LOW();  __delay_us(5);
}

static void I2C_Send(uint8_t data) {
    I2C_Start();
    I2C_WriteByte(LCD_ADDR << 1);  // dirección + W
    I2C_WriteByte(data);
    I2C_Stop();
    __delay_us(100);
}

// ?? LCD ??????????????????????????????????????????????
static void LCD_Write_Nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = (nibble & 0xF0) | mode | LCD_BACKLIGHT;
    I2C_Send(data);
    I2C_Send(data | LCD_ENABLE);
    __delay_us(2);
    I2C_Send(data & ~LCD_ENABLE);
    __delay_us(100);
}

static void LCD_Byte(uint8_t val, uint8_t mode) {
    LCD_Write_Nibble(val & 0xF0, mode);
    LCD_Write_Nibble((val << 4) & 0xF0, mode);
}

void LCD_Command(uint8_t cmd) {
    LCD_Byte(cmd, 0);
    __delay_us(50);
}

void LCD_Data(uint8_t data) {
    LCD_Byte(data, LCD_RS);
}

void LCD_Init(void) {
    // Configurar pines como salida digital inicialmente
    SCL_TRIS = 0; SCL_PIN = 1;
    SDA_TRIS = 0; SDA_PIN = 1;
    __delay_ms(100);

    LCD_Write_Nibble(0x30, 0); __delay_ms(10);
    LCD_Write_Nibble(0x30, 0); __delay_ms(5);
    LCD_Write_Nibble(0x30, 0); __delay_ms(2);
    LCD_Write_Nibble(0x20, 0); __delay_ms(2);

    LCD_Command(0x28); __delay_ms(2);
    LCD_Command(0x0C); __delay_ms(2);
    LCD_Command(0x06); __delay_ms(2);
    LCD_Command(0x01);
    __delay_ms(5);
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t pos[] = {0x80, 0xC0, 0x94, 0xD4};
    if (row < 4) LCD_Command(pos[row] + col);
}

void LCD_Print(char* str) {
    while (*str) LCD_Data(*str++);
}

void LCD_Clear(void) {
    LCD_Command(0x01);
    __delay_ms(5);
}
void LCD_PrintWrapped(char* line1, char* line2) {
    LCD_SetCursor(0, 0);
    LCD_Print("                ");  // 16 espacios
    LCD_SetCursor(1, 0);
    LCD_Print("                ");  // 16 espacios
    LCD_SetCursor(0, 0);
    LCD_Print(line1);
    LCD_SetCursor(1, 0);
    LCD_Print(line2);
}