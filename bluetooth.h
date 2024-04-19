#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

//for bluetooth 
#define CS_BLUEFRUIT     PB1
#define BLUEFRUIT_SPI_CS_PORT  PORTB
#define BLUEFRUIT_SPI_CS_DDR   DDRB
#define MOSI     PB3
#define MISO     PB4
#define SCK      PB5
#define SS       PB2  // Even if not used, must be output to stay in Master mode

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