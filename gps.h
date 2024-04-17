#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#define BUFFER_SIZE 128

#define MUX_SELECT_PIN PB0
#define MUX_SELECT_PORT PORTB
#define MUX_SELECT_DDR DDRB

volatile char received_string[BUFFER_SIZE];
volatile uint8_t received_index = 0;

void USART_Init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

ISR(USART_RX_vect) {
    char received_data = UDR0;
    received_string[received_index++] = received_data;

    if (received_data == '\r' || received_data == '\n' || received_index >= BUFFER_SIZE) {
        received_string[received_index] = '\0';
        received_index = 0;
    }
}

void select_GPS() {

    // Set the select pin to 1 to select (GPS TX)
    MUX_SELECT_PORT |= (1 << MUX_SELECT_PIN);
    
}