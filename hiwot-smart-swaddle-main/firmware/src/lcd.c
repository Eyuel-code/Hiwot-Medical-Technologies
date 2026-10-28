#include "lcd.h"

static void lcd_pulse_enable(void) {
    LCD_PORT |= (1 << LCD_E_PIN);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_E_PIN);
    _delay_us(100);
}

static void lcd_write4bits(uint8_t nibble) {
    // CORRECTED: Removed spaces between < and <
    if (nibble & 0x01) LCD_PORT |= (1 << LCD_D4_PIN); else LCD_PORT &= ~(1 << LCD_D4_PIN);
    if (nibble & 0x02) LCD_PORT |= (1 << LCD_D5_PIN); else LCD_PORT &= ~(1 << LCD_D5_PIN);
    if (nibble & 0x04) LCD_PORT |= (1 << LCD_D6_PIN); else LCD_PORT &= ~(1 << LCD_D6_PIN);
    if (nibble & 0x08) LCD_PORT |= (1 << LCD_D7_PIN); else LCD_PORT &= ~(1 << LCD_D7_PIN);
    lcd_pulse_enable();
}

void lcd_command(uint8_t cmd) {
    LCD_PORT &= ~(1 << LCD_RS_PIN);
    lcd_write4bits(cmd >> 4);
    lcd_write4bits(cmd & 0x0F);
    if (cmd == 0x01 || cmd == 0x02) _delay_ms(2);
}

void lcd_data(uint8_t data) {
    LCD_PORT |= (1 << LCD_RS_PIN);
    lcd_write4bits(data >> 4);
    lcd_write4bits(data & 0x0F);
}

void lcd_string(char* str) {
    while (*str) lcd_data(*str++);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_command(address);
}

void lcd_init(void) {
    LCD_DDR |= (1 << LCD_RS_PIN) | (1 << LCD_E_PIN) |
               (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) |
               (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);
    _delay_ms(50);
    LCD_PORT &= ~(1 << LCD_RS_PIN);
    lcd_write4bits(0x03); _delay_ms(5);
    lcd_write4bits(0x03); _delay_us(150);
    lcd_write4bits(0x03);
    lcd_write4bits(0x02);
    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
    _delay_ms(2);
}