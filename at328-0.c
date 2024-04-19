#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

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

    //// START GPS AND MUX !!!!!
    USART_Init(MYUBRR);
    SPI_init();
    LCD_init();
    Mux_init();

    //Clear and move cursor 
    LCD_sendCommand(0x01);
    _delay_ms(2);
    LCD_sendCommand(0x80);

    select_GPS();
    
    //Bluetooth stuff ??
    //send_data_to_bluefruit("AT+BLEUART=on");  // Initialize UART service
    _delay_ms(100); 
    //char response[100];

    while (1) {

        //Bluetooth tests 

        //_delay_ms(100); // Give some time for the module to respond
        //read_response(response, sizeof(response));
        //_delay_ms(1000); // Wait a bit after initialization
        //send_data_to_bluefruit("ok hi");
        //_delay_ms(1000); // Delay between commands or data transmissions
       
        LCD_set_cursor(0,0);
        LCD_displayString("lat: ");
        LCD_set_cursor(0,5);
        LCD_displayString((const char *)latitude_buffer);
        LCD_set_cursor(1,0);
        LCD_displayString("lon: ");
        LCD_set_cursor(1,5);
        LCD_displayString((const char *)longitude_buffer);
        _delay_ms(1000);
    }

    ////// END GPS AND FINGERPRINT 

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