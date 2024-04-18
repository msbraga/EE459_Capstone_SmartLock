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

//for bluetooth 
#define CS_BLUEFRUIT     PB1
#define BLUEFRUIT_SPI_CS_PORT  PORTB
#define BLUEFRUIT_SPI_CS_DDR   DDRB
#define MOSI     PB3
#define MISO     PB4
#define SCK      PB5
#define SS       PB2  // Even if not used, must be output to stay in Master mode

char* temp;

char received_string[BUFFER_SIZE];
char latitude_buffer[LATITUDE_BUFFER_SIZE];
char longitude_buffer[LONGITUDE_BUFFER_SIZE];
volatile uint8_t received_index = 0;

//for bluetooth 
void SPI_init() {
    // Set MOSI, SCK, and CS pins as outputs, MISO as input
    DDRB |= (1 << MOSI) | (1 << SCK) | (1 << CS_BLUEFRUIT);
    DDRB &= ~(1 << MISO);

    // Set CS pin high to deactivate all SPI devices
    PORTB |= (1 << CS_BLUEFRUIT);

    // Initialize SPI settings here if not already done
    // Example: Master mode, set clock rate, etc.
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uint8_t spi_transfer(uint8_t data) {
    // Start transmission by writing data to the SPI data register
    SPDR = data;
    // Wait for transmission complete (SPIF bit set in SPSR)
    while (!(SPSR & (1<<SPIF)));
     //LCD_displayString("got here?: ");

    // Return the received data
    return SPDR;
}

void select_bluefruit() {
    PORTB &= ~(1 << CS_BLUEFRUIT); // Activate Bluefruit
    _delay_ms(10);
}

void deselect_bluefruit() {
    PORTB |= (1 << CS_BLUEFRUIT);  // Deactivate Bluefruit
}

void send_data_to_bluefruit(const char* data) {
    select_bluefruit();
   while (*data) {
        spi_transfer(*data++);
    }
    spi_transfer('\r'); // Carriage return
    spi_transfer('\n'); // New line
    deselect_bluefruit();
}

void read_response(char* buffer, uint16_t buffer_size) {
    select_bluefruit();
    for (uint16_t i = 0; i < buffer_size; i++) {
        buffer[i] = spi_transfer(0x00); // Sending dummy bytes to read response
        if (buffer[i] == '\n') break; // Assume response ends with a newline
    }
    deselect_bluefruit();
}

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
}

void parse_gps_data(char* nmea_sentence) {
    if (strncmp(nmea_sentence, "$GPGGA,", 7) == 0) {  // Check if it's a GPGGA sentence

        temp = nmea_sentence;

        char* token = strtok(nmea_sentence, ",");
        int token_count = 0;
        
        while (token != NULL) {
            token = strtok(NULL, ",");
            token_count++;
            
            if (token_count == 2) {  // Latitude
                strncpy(latitude_buffer, token, LATITUDE_BUFFER_SIZE - 1);
                latitude_buffer[LATITUDE_BUFFER_SIZE - 1] = '\0';  // Ensure null-termination
                token = strtok(NULL, ",");  // Get N or S
                if (token && token[0] == 'S') {
                    strncat(latitude_buffer, " S", 2);  // Append S if south
                } else {
                    strncat(latitude_buffer, " N", 2);  // Append N otherwise
                }
            } else if (token_count == 4) {  // Longitude
                strncpy(longitude_buffer, token, LONGITUDE_BUFFER_SIZE - 1);
                longitude_buffer[LONGITUDE_BUFFER_SIZE - 1] = '\0';  // Ensure null-termination
                token = strtok(NULL, ",");  // Get E or W
                if (token && token[0] == 'W') {
                    strncat(longitude_buffer, " W", 2);  // Append W if west
                } else {
                    strncat(longitude_buffer, " E", 2);  // Append E otherwise
                }
                break;  // No need to continue parsing after longitude
            }
        }
    }
}



void select_GPS() {

    // Set the select pin to 1 to select (GPS TX)
    MUX_SELECT_PORT |= (1 << MUX_SELECT_PIN);
    
}