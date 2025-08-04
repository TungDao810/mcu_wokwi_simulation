#define F_CPU 8000000UL               // Define clock speed for delay functions
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <string.h>

// LCD control pin definitions (4-bit mode)
#define RS PB1                        // Register Select pin (0 = command, 1 = data)
#define E  PB0                        // Enable pin
#define LCD_dir DDRB                 // Data direction register for LCD pins
#define LCD_port PORTB               // Data port used for LCD

// ---------------------- LCD Low-Level Functions ----------------------

// Send command to LCD (e.g., clear, set cursor, scroll, etc.)
void LCD_cmd(unsigned char cmd)
{
    LCD_port = (LCD_port & 0x0F) | (cmd & 0xF0);   // Send high nibble
    LCD_port &= ~(1 << RS);                        // RS = 0 for command
    LCD_port |= (1 << E);                          // E = 1 to latch
    _delay_us(1);
    LCD_port &= ~(1 << E);                         // E = 0 to finish

    _delay_us(200);

    LCD_port = (LCD_port & 0x0F) | (cmd << 4);     // Send low nibble
    LCD_port |= (1 << E);
    _delay_us(1);
    LCD_port &= ~(1 << E);

    _delay_ms(2);                                  // Command execution delay
}

// Send single character to LCD
void LCD_char(unsigned char ch)
{
    LCD_port = (LCD_port & 0x0F) | (ch & 0xF0);     // Send high nibble
    LCD_port |= (1 << RS);                          // RS = 1 for data
    LCD_port |= (1 << E);
    _delay_us(1);
    LCD_port &= ~(1 << E);

    _delay_us(200);

    LCD_port = (LCD_port & 0x0F) | (ch << 4);       // Send low nibble
    LCD_port |= (1 << E);
    _delay_us(1);
    LCD_port &= ~(1 << E);

    _delay_ms(2);                                   // Character print delay
}

// ---------------------- LCD Utility Functions ----------------------

// Display a string (null-terminated)
void LCD_string (char *str)
{
    for (int i = 0; str[i] != 0; i++)
    {
        LCD_char(str[i]);
    }
}

// Display string at specific LCD line and position
void LCD_string_pos(char *str, int line, int pos)
{
    if ((line == 1) && (pos < 16))
        LCD_cmd((pos & 0x0F) | 0x80);      // First row
    else if ((line == 2) && (pos < 16))
        LCD_cmd((pos & 0x0F) | 0xC0);      // Second row

    while (*str)
        LCD_char(*str++);
}

// Display integer at specified LCD location
void LCD_string_int(int n, int line, int pos)
{
    char buffer_LCD[10];
    itoa(n, buffer_LCD, 10);                    // Convert integer to string
    LCD_string_pos(buffer_LCD, line, pos);
}

// Display float number at specified LCD location
void LCD_string_float(float n, int line, int pos)
{
    char buffer_LCD[20];
    dtostre(n, buffer_LCD, 5, 4);              // Convert float to string with 5 digits, scientific
    LCD_string_pos(buffer_LCD, line, pos);
}

// Clear entire LCD screen
void LCD_clear()
{
    LCD_cmd(0x01);                              // Clear display
    _delay_ms(2);
    LCD_cmd(0x80);                              // Move cursor to start
}

// Move LCD cursor to home position
void LCD_home()
{
    LCD_cmd(0x02);                              // Return home
}

// Initialize LCD in 4-bit mode
void LCD_init()
{
    LCD_dir = 0xFF;                             // Set entire PORTB as output
    _delay_ms(20);                              // LCD power-up delay

    LCD_cmd(0x33);                              // Initialization sequence
    LCD_cmd(0x32);
    LCD_cmd(0x01);                              // Clear display
    LCD_cmd(0x28);                              // 4-bit mode, 2 lines, 5x8 dots
    LCD_cmd(0x0C);                              // Display ON, cursor OFF
    LCD_cmd(0x06);                              // Entry mode: increment
    LCD_cmd(0x1C);                              // Shift display right
    _delay_ms(2);
    LCD_cmd(0x80);                              // Cursor at beginning of line 1
}

// ---------------------- Main Program ----------------------

int main()
{
    LCD_init();                                // Initialize LCD

    LCD_cmd(0x80);                              // Set cursor to line 1, position 0
    LCD_string("Dao Manh Tung");               // Display name

    LCD_cmd(0xC0);                              // Set cursor to line 2
    LCD_string("ECE2022");                     // Display student info

    LCD_cmd(0x05);                              // Set display shift + blink speed (slow)

    while(1)
    {
        LCD_cmd(0x1C);                          // Shift display right (scroll effect)
        _delay_ms(500);                         // Wait before next scroll
    }
}
