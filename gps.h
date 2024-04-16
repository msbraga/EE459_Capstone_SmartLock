#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

enum {
    CMD=0,
    DATA,
};

char buf[100];
volatile int ind;
volatile char flag,stringReceived;
char gpgga[]={'$','G','P','G','G','A'};

char latitude[12];
char logitude[12];

void serialbegin() {

    // Enable USART, set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // Set baud rate
    UBRR0H = (BAUD_PRESCALE >> 8);
    UBRR0L = BAUD_PRESCALE;

    // Enable receiver and transmitter, enable receive complete interrupt
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

ISR(USART_RX_vect) {
    char ch = UDR0;  // Read received character from USART0 data register

    buf[ind] = ch;   // Store received character in buffer
    ind++;           // Increment buffer index

    if (ind < 7) {
        if (buf[ind - 1] != gpgga[ind - 1]) {  // Check for synchronization pattern "$GPGGA"
            ind = 0;  // Reset buffer index if synchronization pattern does not match
        }
    }

    if (ind >= 50) {
        stringReceived = 1;  // Set flag to indicate complete string reception
    }
}