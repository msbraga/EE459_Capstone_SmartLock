#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

//buzzer definitions
#define BUZZER_PIN PC0
#define BUZZER_DDR DDRB
#define BUZZER_PORT PORTB
#define BUZZER_TOGGLE() (BUZZER_PORT ^= (1 << BUZZER_PIN))

void initializeBuzzer() {
    // Set the buzzer pin as an output
        DDRC |= (1 << BUZZER_PIN);

   // Setup PWM (e.g., using Timer1)
    TCCR1A |= (1 << COM1A0) | (1 << WGM10);  // Toggle OC1A on Compare Match, PWM, Phase Correct, 8-bit
    TCCR1B |= (1 << WGM12) | (1 << CS11);    // Prescaler set to 8
    OCR1A = 255;  // Adjust this value to change the frequency
    //trying line below
    //PORTB |= (1 << BUZZER_PIN); // Buzzer off (because the long pin is connected to ground)
}

void playTone() {
    // Enable the PWM signal on the pin
    TCCR1A |= (1 << COM1A0);

    PORTC |= (1 << BUZZER_PIN); // Start the sound
    _delay_ms(1000);  // Sound for 1 second
    // Disable the PWM signal to stop buzzing
    TCCR1A &= ~(1 << COM1A0);

    PORTC &= ~(1 << BUZZER_PIN); // Stop the sound
}