#define F_CPU 8000000UL         // Define system clock as 8 MHz
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <string.h>

// LCD pin definitions (4-bit mode control and data lines)
#define RS PB1
#define E PB0
#define LCD_RS_SIGNAL         0     // Register Select
#define LCD_RW_SIGNAL         1     // Read/Write (tied to GND)
#define LCD_ENABLE_SIGNAL     2     // Enable pin
#define LCD_C4_SIGNAL         4     // Data line D4
#define LCD_C5_SIGNAL         5     // Data line D5
#define LCD_C6_SIGNAL         6     // Data line D6
#define LCD_C7_SIGNAL         7     // Data line D7

#define LCD_dir DDRB          // LCD control/data pin direction
#define LCD_Port PORTB        // LCD data/control output port

// Send command to LCD
void LCDcmd(unsigned char cmd) {
    LCD_Port = (LCD_Port & 0x0F) | (cmd & 0xF0); // Send higher nibble
    LCD_Port &= ~(1 << RS);    // RS = 0 for command
    LCD_Port |= (1 << E);      // Enable high
    _delay_us(1);
    LCD_Port &= ~(1 << E);     // Enable low
    _delay_us(200);

    LCD_Port = (LCD_Port & 0x0F) | (cmd << 4);   // Send lower nibble
    LCD_Port |= (1 << E);
    _delay_us(1);
    LCD_Port &= ~(1 << E);
    _delay_ms(2);
}

// Send single character (data) to LCD
void LCDchar(unsigned char data) {
    LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0); // Send higher nibble
    LCD_Port |= (1 << RS);       // RS = 1 for data
    LCD_Port |= (1 << E);
    _delay_us(1);
    LCD_Port &= ~(1 << E);
    _delay_us(200);

    LCD_Port = (LCD_Port & 0x0F) | (data << 4);   // Send lower nibble
    LCD_Port |= (1 << E);
    _delay_us(1);
    LCD_Port &= ~(1 << E);
    _delay_ms(2);
}

// Display a string at a specific line and cursor position
void LCD_string(char *s, int LCD_line, int LCD_cursor_position) {
    if (LCD_line == 1)
        LCDcmd(0x80 + LCD_cursor_position); // Move cursor to line 1
    else
        LCDcmd(0xC0 + LCD_cursor_position); // Move cursor to line 2

    while (*s)
        LCDchar(*s++);
}

// Turn on blinking cursor (optional)
void LCD_cursor_blink() {
    LCDcmd(0x0F);
}

// Display a float value on LCD at specified position
void LCD_float_string(float LCD_number, int LCD_line, int LCD_cursor_position) {
    char buffer_LCD[13];
    dtostre(LCD_number, buffer_LCD, 5, 4); // Convert float to string
    LCD_string(buffer_LCD, LCD_line, LCD_cursor_position);
}

// Display an integer value on LCD at specified position
void LCD_int_string(int LCD_number, int LCD_line, int LCD_cursor_position) {
    char buffer_LCD[10];
    itoa(LCD_number, buffer_LCD, 10); // Convert int to string
    LCD_string(buffer_LCD, LCD_line, LCD_cursor_position);
}

// Initialize LCD in 4-bit mode
void LCDinit() {
    LCD_dir = 0xFF;           // Set LCD control and data lines as output
    _delay_ms(20);            // Initial delay for LCD power-up
    LCDcmd(0x33);             // Initialization sequence
    LCDcmd(0x32);
    LCDcmd(0x01);             // Clear display
    LCDcmd(0x28);             // 4-bit, 2-line, 5x8 font
    LCDcmd(0x0C);             // Display ON, cursor OFF
    LCDcmd(0x06);             // Entry mode (increment)
    LCDcmd(0x1C);             // Shift display right
    _delay_ms(2);
    LCDcmd(0x80);             // Move cursor to home position
}

// Display a string (basic version)
void LCD_String(char *str) {
    while (*str)
        LCDchar(*str++);
}

// Display a string at specific XY coordinate
void LCD_String_xy(char row, char pos, char *str) {
    if (row == 0 && pos < 16)
        LCDcmd((pos & 0x0F) | 0x80);
    else if (row == 1 && pos < 16)
        LCDcmd((pos & 0x0F) | 0xC0);

    LCD_String(str);
}

// Clear LCD screen
void LCD_clear() {
    LCDcmd(0x01);
    _delay_ms(2);
    LCDcmd(0x80);
}

// Move LCD cursor to home position
void LCD_home() {
    LCDcmd(1 << 1);
}

// Initialize ADC on channel 0
void ADC_init() {
    DDRA = 0x00;   // Set PORTA (ADC pins) as input
    ADCSRA = 0;
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADEN) | (1 << ADIE);  // Prescaler 64, enable ADC
    ADMUX = 0x00; 
		ADMUX |= (1<<REFS0);  // Use ADC0 (pin A0), AVCC as ref voltage
}

// Perform ADC read on ADC0
int ADC_read() {
    ADCSRA |= (1 << ADSC);         // Start conversion
    ADMUX = 0x40;                  // Channel 0
    while (!(ADCSRA & (1 << ADIF))); // Wait for conversion to finish
    ADCSRA |= (1 << ADIF);         // Clear interrupt flag
    _delay_ms(1);
    return ADCW;                   // Return ADC result (10-bit)
}

// Main function
int main() {
    LCDinit();     // Initialize LCD
    ADC_init();    // Initialize ADC

    float volt;

    while (1) {
        volt = (ADC_read() * 0.0488);   // Convert ADC value to voltage
        volt = volt / 10.00;            // Adjust scale for display
        _delay_ms(10);

        LCD_clear();                    // Clear screen
        LCD_float_string(volt, 1, 1);   // Display voltage
        _delay_ms(1000);                // Refresh every 1 second
    }
}
