#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> 
#include <string.h> 

//mux definitions
#define MUX_SELECT_PIN PB0
#define MUX_SELECT_PORT PORTB
#define MUX_SELECT_DDR DDRB

const uint8_t finger_check_command[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x03, 0x01, 0x00, 0x04};  // Example command

#define UART_PORT PD1     // UART port number

void Mux_init() {

    // Set the MUX select pin as an output
    MUX_SELECT_DDR |= (1 << MUX_SELECT_PIN);

    // Set GPS as default
    MUX_SELECT_PORT |= (1 << MUX_SELECT_PIN);
}

// Function to send data over UART
void uart_write(const uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        // Wait until the UART transmit buffer is ready
        while (!(UCSR0A & (1 << UDRE0)));  // UDRE0: UART Data Register Empty flag

        // Send the data
        UDR0 = data[i];  // Write data to the UART Data Register
    }
}

// Function to initialize the fingerprint sensor
void initialize_fingerprint_sensor() {
    // Send the initialization command to the sensor
    uart_write(finger_check_command, sizeof(finger_check_command));
}

// Function to read data from UART
uint8_t uart_read() {
    // Wait until data is available in the UART receive buffer
    while (!(UCSR0A & (1 << RXC0)));  // RXC0: UART Receive Complete flag

    return UDR0;  // Read and return the received byte
}


// Function to check if a finger is present on the fingerprint sensor
bool is_finger_present() {
    uart_write(finger_check_command, sizeof(finger_check_command));  // Send command

    uint8_t response[12];  // Buffer to read response (adjust size as needed)
    for (size_t i = 0; i < sizeof(response); i++) {
        response[i] = uart_read();  // Read the data
    }

    // Determine if a finger is detected (adjust based on the sensor's datasheet)
    return (response[9] == 0x00);  // Example condition indicating finger presence
}



