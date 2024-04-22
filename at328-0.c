#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy
#include <stdbool.h> // For bool, true, and false

#include "lcd.h"
#include "buzzer.h"
#include "gps.h"
#include "current.h"
#include "mux.h"
#include "fingerprint.h"
#include "bluetooth.h"

// Define the frequency of the microcontroller
//#define F_CPU 16000000UL

int main(void)
{
    //// END CURRENT SENSOR AND GPS !!!!!
    USART_Init(MYUBRR);
    SPI_init();
    LCD_init();
    Mux_init();

    //for current sensor
    adc_init();
    char output[128];

    //Clear and move cursor 
    LCD_sendCommand(0x01);
    _delay_ms(2);
    LCD_sendCommand(0x80);

    //select_Fingerprint();
    select_GPS();

    //char response[100];
    adc_select_channel(1); // Assuming PC1 is channel 1

    while (1) {


        uint16_t adc_value = adc_read();  
        float voltage = adc_value * (4.8 / 1023.0);  // Convert ADC value to voltage
        float sensitivity = 100.0 / 500.0;  // Sensitivity is 0.2 A/V
        float current = (voltage - 2.5) * sensitivity;  // Calculate current
        char adc_buffer[32]; // Ensure the buffer is large enough for the string and null terminator

        floatToStr(adc_buffer, adc_value, 3); // Formats the float with two decimal places
        LCD_set_cursor(0,0);
       // LCD_displayString("adc ");
        //LCD_set_cursor(0,5);
        LCD_displayString(adc_buffer);

        send_data_to_bluefruit("Hey");
        _delay_ms(100);
        LCD_set_cursor(1,1);
        LCD_displayString("Out");


        char current_buffer[32]; // Ensu
        floatToStr(current_buffer, current, 3); //
        LCD_set_cursor(1,0);
        //LCD_displayString("voltage:");
        //LCD_set_cursor(1,8);
        LCD_displayString(current_buffer);
        

        if (data_available) {
            // Handle data from the Bluetooth module
            char buffer[256]; // Adjust size based on expected data
            read_response(buffer, sizeof(buffer));
            data_available = false; // Reset the flag
        }

        //display_lat_lon();

        //Bluetooth tests 
        //_delay_ms(100); // Give some time for the module to respond
        //read_response(response, sizeof(response));
        //_delay_ms(1000); // Wait a bit after initialization
        //send_data_to_bluefruit("ok hi");
        //_delay_ms(1000); // Delay between commands or data transmissions

        _delay_ms(1000);
    }

    ////// END CURRENT SENSOR AND GPS

    ///// SELINAS FINGERPRINT TEST
    
    // LCD_init();
    // LCD_displayString("hi");

    // USART_Init(103); //9600 baud rate at 16MHz
    // Mux_init();
    // sei();


    // // Initial LCD set up 
    // LCD_sendCommand(0x01); // Clear display on the LCD when first flashing the code
    // _delay_ms(2);
    // LCD_sendCommand(0x80); // Set cursor at the beginning

    // LCD_displayString("Starting up");
    // _delay_ms(4000);

    
    // while(1){

    //     // Check for any data from the fingerprint sensor
    //     if (UCSR0A & (1<<RXC0)) {
    //         select_Fingerprint();  // Select fingerprint sensor if data is received
    //     } else {
    //         select_GPS();  // Otherwise, select GPS sensor
    //     }

    //     _delay_ms(100);  // Small delay to prevent bouncing between selections
    // }

    //////// END SELINA TEST FINGER PRINT

    //// START BUZZER !!!!!

    // // Initialize LCD 
    // LCD_DDR = 0xFF;
    // LCD_init();

    // //Initialize buzzer
    // initializeBuzzer();

    // //enable global interrupts
    // sei();

    // // Initial LCD set up 
    // LCD_sendCommand(0x01); // Clear display on the LCD when first flashing the code
    // _delay_ms(2);
    // LCD_sendCommand(0x80); // Set cursor at the beginning

    // LCD_displayString("Loading");
    // _delay_ms(4000);

    // while(1){
    //     LCD_sendCommand(0x01); // Clear display on the LCD when first flashing the code
    //     LCD_displayString("Listen for buzzer!");
    //     playTone();

    //     _delay_ms(3000); // Wait for a while 
    // }

    /// END BUZZER !!!!!

    return 0;   /* never reached */
}