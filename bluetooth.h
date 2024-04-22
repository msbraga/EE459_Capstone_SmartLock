#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

//for bluetooth 
#define CS_BLUEFRUIT     PB1
#define IRQ_BLUEFRUIT PB2
#define BLUEFRUIT_SPI_CS_PORT  PORTB
#define BLUEFRUIT_SPI_CS_DDR   DDRB
#define MOSI PB3
#define MISO PB4
#define SCK PB5
#define SS PB2  // Even if not used, must be output to stay in Master mode


volatile int data_available = 1; // Flag to indicate data from the BLE module

//for bluetooth 
void SPI_init() {
    // Set MOSI, SCK, and CS pins as outputs, MISO as input
    DDRB |= (1 << MOSI) | (1 << SCK) | (1 << CS_BLUEFRUIT);
    DDRB &= ~(1 << MISO);

    // Set CS pin high to deactivate all SPI devices
    PORTB |= (1 << CS_BLUEFRUIT);

     // Set the IRQ pin as input with a pull-up resistor
    DDRB &= ~(1 << IRQ_BLUEFRUIT); // IRQ as input
    PORTB |= (1 << IRQ_BLUEFRUIT); // Enable pull-up

    // Initialize SPI settings here if not already done
    // Example: Master mode, set clock rate, etc.
   // SPCR &= ~((1 << SPR0) | (1 << SPR1));
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

      // Ensure SPI2X is not set in SPSR (single speed)
    SPSR &= ~(1 << SPI2X);

    // Set default SPI mode (e.g., Mode 0: CPOL = 0, CPHA = 0)
    SPCR &= ~((1 << CPOL) | (1 << CPHA));

    // Set data order (MSB-first)
    SPCR &= ~(1 << DORD);

    // Enable external interrupts on IRQ pin (falling edge)
    EICRA = (1 << ISC01); // Trigger on falling edge
    EIMSK = (1 << INT0); // Enable external interrupt INT0
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
    _delay_us(100);
}

void deselect_bluefruit() {
    PORTB |= (1 << CS_BLUEFRUIT);  // Deactivate Bluefruit
    _delay_us(100);
}


void send_data_to_bluefruit(const char* data) {
    select_bluefruit();
   int count = 0;
   while (*data) {
        if (count == 1) {
           LCD_set_cursor(1,10);
           LCD_displayString("one");

        }
         SPDR = *data++; // Send data via SPI
        while (!(SPSR & (1 << SPIF))); // Wait for transfer to complete
        if ( count % 2 == 0) {
            LCD_set_cursor(1,5);
             LCD_displayString("ev  ");
             _delay_ms(250);

        } else {
            LCD_set_cursor(1,5);
            LCD_displayString("odd");
            _delay_ms(250);
        }
        count++;
        LCD_set_cursor(1,1);
        LCD_displayString("out");
        _delay_ms(250);
    }
    SPDR = '\r'; // Send data via SPI
     while (!(SPSR & (1 << SPIF))); // Wait for transfer to complete
    SPDR = '\n'; // Send data via SPI
     while (!(SPSR & (1 << SPIF))); // Wait for transfer to complete
    LCD_set_cursor(1,12);
    LCD_displayString("done");
    //spi_transfer('\r'); // Carriage return
    //spi_transfer('\n'); // New line
    deselect_bluefruit();
}

void read_response(char* buffer, uint16_t buffer_size) {
    select_bluefruit();
    uint16_t i;
    for (i = 0; i < buffer_size; i++) {
        buffer[i] = spi_transfer(0x00); // Sending dummy bytes to read response
        if (buffer[i] == '\n') break; // Assume response ends with a newline
    }
    deselect_bluefruit();
}

ISR(INT0_vect) { // ISR for INT0 (IRQ)
    data_available = 1; // Set flag when IRQ is triggered
}