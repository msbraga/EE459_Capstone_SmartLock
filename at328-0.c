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

// Define the frequency of the microcontroller
#define F_CPU 16000000UL

int main(void)
{

    // Initialize LCD 
    LCD_DDR = 0xFF;
    LCD_init();

    //Initialize buzzer
    initializeBuzzer();

    //enable global interrupts
    sei();

    // Initial LCD set up 
    LCD_sendCommand(0x01); // Clear display on the LCD when first flashing the code
    _delay_ms(2);
    LCD_sendCommand(0x80); // Set cursor at the beginning

    LCD_displayString("Loading");
    _delay_ms(4000);

    while(1){
        LCD_sendCommand(0x01); // Clear display on the LCD when first flashing the code
        LCD_displayString("Listen for buzzer!");
        playTone();

        _delay_ms(3000); // Wait for a while 
    }

    // !!!!! GPS WORK IN PROGRESS CODE !!!!!

    // serialbegin();
    // // Initialize LCD 
    // LCD_DDR = 0xFF;
    // LCD_init();

    // // Initial LCD set up 
    // LCD_sendCommand(0x01); // Clear display on the LCD when first flashing the code
    // _delay_ms(2);
    // LCD_sendCommand(0x80); // Set cursor at the beginning

    // LCD_displayString("GPS Interfacing");
    // _delay_ms(4000);
    // LCD_sendCommand(0x80); // Set cursor at the beginning
    // LCD_displayString("Waiting for GPS");
    // _delay_ms(2000);
    // sei();

    // while(1){

    //     if(stringReceived == 1){ //Problem is that string received is never 1
            
    //         cli();
    //         for(int i=0;i<ind;i++) {
    //             LCD_displayString(&buf[i]); 
    //         }
    //         ind=0;
    //         stringReceived=0;

    //         LCD_sendCommand(0x01);
    //         _delay_ms(1000);

    //         LCD_sendCommand(0x80);
    //         LCD_displayString("Lat:");
            
    //         for(int i=15;i<27;i++) {
    //             latitude[i]=buf[i];
    //             LCD_displayString(&latitude[i]);
    //         }

    //         LCD_sendCommand(0xC0);
    //         LCD_displayString("Log:");

    //         for(int i=29;i<41;i++) {
    //             logitude[i]=buf[i];
    //             LCD_displayString(&logitude[i]);
    //         }

    //         _delay_ms(2000);
    //         sei();
    //     }
    // } 

    return 0;   /* never reached */
}