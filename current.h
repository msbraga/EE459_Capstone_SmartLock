/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
//#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

void adc_init();

//set up for the curent sensor
uint8_t adc_read(uint8_t channel) {
    // Select ADC channel with safety mask
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    // Start single conversion
    ADCSRA |= (1<<ADSC);
    // Wait for conversion to complete
    while (ADCSRA & (1<<ADSC));
    // Return the 8-bit result
    return ADCH;
}

//so that the current and voltage read can be printed on the LCD
void floatToStr(char* outStr, float value, int decimalPlaces) {
    // Ensure decimalPlaces is between 0 and some reasonable upper limit
    decimalPlaces = decimalPlaces < 0 ? 0 : (decimalPlaces > 6 ? 6 : decimalPlaces);
    
    int intPart = (int)value; // Extract integer part
    float fractionalPart = value - (float)intPart; // Extract fractional part

    // Convert integer part to string
    itoa(intPart, outStr, 10); // Assuming itoa is available. If not, you might need a custom implementation

    if (decimalPlaces > 0) {
        // Add decimal point
        strcat(outStr, ".");

        // Handle fractional part
        for (int i = 0; i < decimalPlaces; ++i) {
            fractionalPart *= 10;
        }

        // Convert fractional part to integer
        int fracInt = (int)(fractionalPart + 0.5); // Adding 0.5 for rounding

        // Temporary string for fractional part
        char fracStr[8];
        itoa(fracInt, fracStr, 10);

        // Add leading zeros to fractional part if necessary
        int fracStrLen = strlen(fracStr);
        for (int i = fracStrLen; i < decimalPlaces; i++) {
            strcat(outStr, "0");
        }

        // Append fractional part to output string
        strcat(outStr, fracStr);
    }
}

//init for the current sensor
void adc_init() {
    // Reference AVcc, left adjust result (for 8-bit precision)
    ADMUX = (1<<REFS0) | (1<<ADLAR);
    // Enable ADC and set prescaler to 128
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

}