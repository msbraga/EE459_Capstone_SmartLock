#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
//#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

//#define F_CPU 16000000UL  // Clock Speed

void adc_init() {
    // Reference AVcc, left adjust result (for 8-bit precision)
    ADMUX = (1<<REFS0);
    //ADMUX = (1<<REFS0) | (1<<ADLAR);
    // Set the ADC clock prescaler (e.g., divide by 128 for 16MHz CPU)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Enable the ADC
    ADCSRA |= (1 << ADEN);
}

void adc_select_channel(uint8_t channel) {
    // Clear the bottom 4 bits (channel selection bits)
    ADMUX &= 0xF0;
    // Set the channel
    ADMUX |= (channel & 0x0F);
}

// uint16_t adc_read(uint16_t channel) {

//     ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // Clear the channel bits and set them according to the desired channel
//     //ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

//     ADCSRA |= (1 << ADSC);                     // Start conversion
//     while (ADCSRA & (1 << ADSC));              // Wait for conversion to complete
//     return ADC;                                // Return the ADC value
// }

//set up for the current sensor
uint16_t adc_read() {

    // Start single conversion
    ADCSRA |= (1<<ADSC);
    // Wait for conversion to complete
    while (ADCSRA & (1<<ADSC));
    // Return the 8-bit result
    return ADC;
}

void uint16_to_string(uint16_t num, char *str) {
    char *p = str;
    uint16_t shifter = num;
    
    // Move to where representation ends
    do {
        ++p;
        shifter = shifter / 10;
    } while (shifter);
    *p = '\0';  // Terminate string

    // Move back, inserting digits as you go
    do {
        *--p = '0' + (num % 10);
        num /= 10;
    } while (num);
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

