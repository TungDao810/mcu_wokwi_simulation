#define F_CPU 8000000UL   // System clock frequency
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

// LCD Pin definitions
#define RS PB1
#define E  PB0

#define LCD_RS_SIGNAL         0
#define LCD_RW_SIGNAL         1
#define LCD_ENABLE_SIGNAL     2
#define LCD_C4_SIGNAL         4
#define LCD_C5_SIGNAL         5
#define LCD_C6_SIGNAL         6
#define LCD_C7_SIGNAL         7

#define LCD_dir DDRB
#define LCD_Port PORTB

// ---------------- LCD Functions ----------------

void LCDcmd(unsigned char cmd) {
	LCD_Port = (LCD_Port & 0x0f) | (cmd & 0xf0);  // High nibble
	LCD_Port &= ~(1 << RS);
	LCD_Port |= (1 << E);
	_delay_us(1);
	LCD_Port &= ~(1 << E);
	_delay_us(200);
	
	LCD_Port = (LCD_Port & 0x0f) | (cmd << 4);    // Low nibble
	LCD_Port |= (1 << E);
	_delay_us(1);
	LCD_Port &= ~(1 << E);
	_delay_ms(2);
}

void LCDchar(unsigned char data) {
	LCD_Port = (LCD_Port & 0x0f) | (data & 0xf0);
	LCD_Port |= (1 << RS);
	LCD_Port |= (1 << E);
	_delay_us(1);
	LCD_Port &= ~(1 << E);
	_delay_us(200);
	
	LCD_Port = (LCD_Port & 0x0f) | (data << 4);
	LCD_Port |= (1 << E);
	_delay_us(1);
	LCD_Port &= ~(1 << E);
	_delay_ms(2);
}

void LCD_string(char *s, int LCD_line, int LCD_cursor_position) {
	if (LCD_line == 1)
		LCDcmd(0x80 + LCD_cursor_position);  // Line 1 start
	else
		LCDcmd(0xC0 + LCD_cursor_position);  // Line 2 start
	
	while (*s) {
		LCDchar(*s++);
	}
}

void LCD_float_string(float LCD_number, int LCD_line, int LCD_cursor_position) {
	char buffer_LCD[13];
	dtostre(LCD_number, buffer_LCD, 5, 4); // Float to string
	LCD_string(buffer_LCD, LCD_line, LCD_cursor_position);
}

void LCDinit() {
	LCD_dir = 0xff;             // Set LCD control port as output
	_delay_ms(20);
	LCDcmd(0x33);               // Initialize in 4-bit mode
	LCDcmd(0x32);
	LCDcmd(0x01);               // Clear display
	LCDcmd(0x28);               // 2 line, 5x8 font
	LCDcmd(0x0C);               // Display ON, cursor OFF
	LCDcmd(0x06);               // Increment cursor
	LCDcmd(0x1C);               // Display shift right
	_delay_ms(2);
	LCDcmd(0x80);               // Return home
}

void LCD_clear() {
	LCDcmd(0x01);               // Clear screen
	_delay_ms(2);
	LCDcmd(0x80);               // Cursor to home
}

// ---------------- ADC Functions (Unused in this program) ----------------

void ADC_init() {
	DDRA = 0x00;
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADEN) | (1 << ADIE); // Prescaler 64, ADC enable
	ADMUX |= (1 << REFS0) | (1 << ADLAR); // AVCC reference, left adjust result, ADC0
}

int ADC_read() {
	ADCSRA |= (1 << ADSC);              // Start conversion
	while (!(ADCSRA & (1 << ADIF))) {}  // Wait for conversion
	ADCSRA |= (1 << ADIF);              // Clear flag
	_delay_ms(1);
	return ADCW;
}

// ---------------- Timer1 Delay Function (0.1s) ----------------

void T1delay(void) {
	TCNT1 = 0;
	TCCR1B = (1 << CS12);         // Prescaler = 256
	OCR1A = 3125 - 1;             // For 0.1s delay @ 8MHz with 256 prescaler
	while ((TIFR1 & (1 << OCF1A)) == 0);
	TCCR1B = 0x00;
	TIFR1 = (1 << OCF1A);
}

// ---------------- External Interrupt INT1 Setup ----------------

void INT_0init(void) {
	cli();                         // Disable global interrupts
	DDRD &= ~(1 << PD1);           // INT1 (PD1) as input
	PORTD |= (1 << PD1);           // Enable pull-up on INT1
	EICRA |= (1 << ISC10);         // Trigger on any logical change
	EIMSK |= (1 << INT1);          // Enable INT1
	sei();                         // Enable global interrupts
}

// ---------------- Main Program ----------------

int value_check = 1;    // Stopwatch state: 1 = stopped, 0 = running
float count = 0;        // Time counter

int main() {
	LCDinit();
	INT_0init();

	while (1) {
		LCD_string("timer start", 1, 1);

		if (value_check == 1)
			count = 0; // Reset when stopped

		if (value_check == 0) {
			count += 0.1;      // Increment time by 0.1s
			T1delay();         // Delay of 0.1s
		}

		LCD_float_string(count, 2, 1);  // Show current time on LCD
	}
}

// ---------------- External Interrupt Service Routine ----------------

ISR(INT1_vect) {
	EIMSK &= ~(1 << INT1);        // Disable INT1 to avoid bouncing
	EIFR |= (1 << INTF1);         // Clear interrupt flag
	value_check = !value_check;   // Toggle stopwatch state
	_delay_ms(100);               // Software debounce
	EIMSK |= (1 << INT1);         // Re-enable INT1
}
