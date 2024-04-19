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
        char *dot_position = strchr(token, '.');
        if (dot_position != NULL) {
            size_t degrees_len = dot_position - token;
            strncpy(latitude_buffer, token, degrees_len);
            latitude_buffer[degrees_len] = '\0'; // Null terminate the degrees part
            char minutes_str[6];
            snprintf(minutes_str, sizeof(minutes_str), ".%.2s ", dot_position + 1);
            strcat(latitude_buffer, minutes_str);
            strcat(latitude_buffer, strtok(NULL, ","));
        }

        // Longitude
        token = strtok(NULL, ",");
        dot_position = strchr(token, '.');
        if (dot_position != NULL) {
            size_t degrees_len = dot_position - token;
            strncpy(longitude_buffer, token, degrees_len);
            longitude_buffer[degrees_len] = '\0'; // Null terminate the degrees part
            char minutes_str[6];
            snprintf(minutes_str, sizeof(minutes_str), ".%.2s ", dot_position + 1);
            strcat(longitude_buffer, minutes_str);
            strcat(longitude_buffer, strtok(NULL, ","));
        }


            // // Latitude
            // token = strtok(NULL, ",");
            // size_t dot_position = strcspn(token, ".");
            // if (dot_position != strlen(token)) {
            //     strncpy(latitude_buffer, token, dot_position); // Copy the characters before the period
            //     latitude_buffer[dot_position] = '\0'; // Null terminate the degrees part

            //     size_t len = strlen(latitude_buffer);
            //     if (len > 2) { // Check if there are enough characters for adding the period

            //         char minutes_str[3];
            //         strncpy(minutes_str, latitude_buffer + len - 2, 2);
            //         minutes_str[2] = '\0'; // Null terminate minutes_str

            //         latitude_buffer[len - 2] = '.';
            //         latitude_buffer[len - 1] = minutes_str[0];
            //         latitude_buffer[len] = minutes_str[1];  
            //         latitude_buffer[len + 1] = ' '; // Add space before direction

            //         // Concatenate latitude direction
            //         token = strtok(NULL, ",");
            //         strcat(latitude_buffer, token);

            //         latitude_buffer[strlen(latitude_buffer)] = '\0'; 
            //     }
            // }

            // // Longitude
            // token = strtok(NULL, ",");
            // dot_position = strcspn(token, ".");
            // if (dot_position != strlen(token)) {
            //     strncpy(longitude_buffer, token, dot_position); // Copy the characters before the period
            //     longitude_buffer[dot_position] = '\0'; // Null terminate the degrees part

            //     size_t len = strlen(longitude_buffer);
            //     if (len > 2) { // Check if there are enough characters for adding the period

            //         char minutes_str[3];
            //         strncpy(minutes_str, longitude_buffer + len - 2, 2);
            //         minutes_str[2] = '\0'; // Null terminate minutes_str

            //         longitude_buffer[len - 2] = '.';
            //         longitude_buffer[len - 1] = minutes_str[0];
            //         longitude_buffer[len] = minutes_str[1];  
            //         longitude_buffer[len + 1] = ' '; // Add space before direction

            //         // Concatenate longitude direction
            //         token = strtok(NULL, ",");
            //         strcat(longitude_buffer, token);

            //         longitude_buffer[strlen(longitude_buffer)] = '\0'; 
            //     }
            // }

            return;
        }
        token = strtok(NULL, ",");
    }
}

void display_lat_lon() {
    LCD_set_cursor(0,0);
    LCD_displayString("lat: ");
    LCD_set_cursor(0,5);
    LCD_displayString((const char *)latitude_buffer);
    LCD_set_cursor(1,0);
    LCD_displayString("lon: ");
    LCD_set_cursor(1,5);
    LCD_displayString((const char *)longitude_buffer);
}

void select_GPS() {

    // Set the select pin to 1 to select (GPS TX)
    MUX_SELECT_PORT |= (1 << MUX_SELECT_PIN);
    
}