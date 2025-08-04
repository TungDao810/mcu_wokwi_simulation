#define F_CPU 8000000            // Define the system clock frequency (8 MHz)
#include <avr/io.h>

uint16_t compared_value_1 = 7812;   // Compare match value for ON delay (approx. 0.5s)
uint16_t compared_value_2 = 31249;  // Compare match value for OFF delay (approx. 2s)

// Function prototype
void T1delay(uint16_t n);

int main(void)
{
    // Set PC1 (PORTC pin 1) as output
    DDRC |= (1 << PC1);
	
    while (1) 
    {
        // Turn LED ON
        PORTC |= (1 << PC1);
        T1delay(compared_value_1); // Delay ~0.5s

        // Turn LED OFF
        PORTC &= ~(1 << PC1);
        T1delay(compared_value_2); // Delay ~2s
    }
}

// Timer1 Delay Function using CTC (Clear Timer on Compare Match) Mode
void T1delay(uint16_t n)
{
    TCNT1 = 0;        // Reset Timer1 counter
    OCR1A = n;        // Set output compare value
    
    // Set Timer1 to:
    // - CTC mode: WGM12 = 1, WGM13 = 0
    // - Prescaler = 1024: CS12 = 1, CS10 = 1
    TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12);

    // Wait until the Output Compare Flag is set
    while ((TIFR1 & (1 << OCF1A)) == 0);

    // Stop Timer1
    TCCR1B = 0x00;

    // Clear interrupt flags
    TIFR1 |= (1 << OCF1A) | (1 << TOV1);
}
