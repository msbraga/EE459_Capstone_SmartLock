#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

#define FOSC 7.3728e6
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define BUFFER_SIZE 128
#define LATITUDE_BUFFER_SIZE 16
#define LONGITUDE_BUFFER_SIZE 16

#define MUX_SELECT_PIN PB0
#define MUX_SELECT_PORT PORTB
#define MUX_SELECT_DDR DDRB

char received_string[BUFFER_SIZE];
char latitude_buffer[LATITUDE_BUFFER_SIZE];
char longitude_buffer[LONGITUDE_BUFFER_SIZE];
volatile uint8_t received_index = 0;

void parse_gps_data(char* nmea_sentence);

void USART_Init(unsigned int ubrr) {
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Make PD0 as input (RX) and PD1 as output (TX) explicitly for clarity
    DDRD &= ~(1 << PD0); // Clear the PD0 bit in DDRD register to make it input
    DDRD |= (1 << PD1);  // Set the PD1 bit in DDRD register to make it output

    sei();
}

ISR(USART_RX_vect) {

    cli(); //disable interrupts while processing 

    char received_data = UDR0;

    if (received_data == '\n' || received_data == '\r') {  // End of a sentence
        received_string[received_index] = '\0';  // Null-terminate the string
        parse_gps_data(received_string);  // Parse the complete NMEA sentence
        received_index = 0;  // Reset index for the next sentence
    } else if (received_index < BUFFER_SIZE - 1) {
        received_string[received_index++] = received_data;  // Store byte and increment index
    } else {
        // Handle buffer overflow
        received_index = 0;  // Reset buffer index
    }

    sei(); //reenable interrupts 
}

void parse_gps_data(char* nmea_sentence) {
    char* token = strtok(nmea_sentence, ",");
    while(token != NULL) {
        // Check for GPGGA sentence
        if (strcmp(token, "$GPGGA") == 0) {
            // Skip one token (e.g., time) before latitude
            token = strtok(NULL, ",");

            // Latitude
            token = strtok(NULL, ",");
            strncpy(latitude_buffer, token, 2); // Extract degrees
            latitude_buffer[2] = '.'; // Add decimal point
            strncpy(latitude_buffer + 3, token + 2, 2); // Extract minutes
            strcat(latitude_buffer, " "); // Add a space
            token = strtok(NULL, ","); // Grab the direction
            strcat(latitude_buffer, token); // Append latitude hemisphere
            latitude_buffer[strlen(latitude_buffer)] = '\0'; // Null terminate

            // Longitude
            token = strtok(NULL, ",");
            strncpy(longitude_buffer, token, 3); // Extract degrees
            longitude_buffer[3] = '.'; // Add decimal point
            strncpy(longitude_buffer + 4, token + 3, 2); // Extract minutes
            longitude_buffer[6] = '\0'; // Null terminate
            return;
        }
        token = strtok(NULL, ",");
    }
}

void select_GPS() {

    // Set the select pin to 1 to select (GPS TX)
    MUX_SELECT_PORT |= (1 << MUX_SELECT_PIN);
    
}