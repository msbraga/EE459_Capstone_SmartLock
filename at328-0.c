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

// Define the frequency of the microcontroller
#define F_CPU 16000000UL

int main(void)
{
    //// START GPS AND MUX !!!!!

    LCD_DDR = 0xFF;
    LCD_init();
    USART_Init(MYUBRR);
    Mux_init();
    sei(); 

    //Clear and move cursor 
    LCD_sendCommand(0x01);
    _delay_ms(2);
    LCD_sendCommand(0x80);

    //select_Fingerprint();
    select_GPS();

    while (1) {

        //Gets data but is random
        LCD_displayString((const char *)received_string);
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