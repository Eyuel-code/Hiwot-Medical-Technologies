#ifndef LCD_H
#define LCD_H
#include <avr/io.h>
#include <util/delay.h>

#define LCD_PORT   PORTD
#define LCD_DDR    DDRD
#define LCD_RS_PIN PD2
#define LCD_E_PIN  PD3
#define LCD_D4_PIN PD4
#define LCD_D5_PIN PD5
#define LCD_D6_PIN PD6
#define LCD_D7_PIN PD7

void lcd_init(void);
void lcd_string(char* str);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
#endif
