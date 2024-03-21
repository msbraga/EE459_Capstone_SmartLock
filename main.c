#include <avr/io.h>

int main(void)
{
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output
    PORTC |= 1 << PC1;          // Enable pull-up for switch on PORTC bit 1

    while (1) {
        if (PINC & (1 << PC1))  // Read the button
            PORTC &= ~(1 << PC0);  // Not pressed, LED off
        else
            PORTC |= 1 << PC0;  // Pressed, LED on
    }

    return 0;   /* never reached */
}