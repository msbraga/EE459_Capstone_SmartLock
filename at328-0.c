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

void locked_routine(); 
void broken_into_routine();
void unlocked_routine(); 

int main(void)
{
    //// Main Bike Program 
    USART_Init(MYUBRR);
    SPI_init();
    LCD_init();
    Mux_init();
    initializeBuzzer();

    //for current sensor
    adc_init();

    //Clear and move cursor 
    LCD_sendCommand(0x01);
    _delay_ms(2);
    LCD_sendCommand(0x80);

    select_GPS();

    adc_select_channel(1); // Assuming PC1 is channel 1

    bool fingerprint = true; 

    while (1) {

        uint16_t adc_value = adc_read();  

        if (adc_value < 200) { //LOCKED - Change this threshold 
            locked_routine();
        } else if (fingerprint){ //UNLOCKED 
            unlocked_routine();
        } else {
            broken_into_routine();
        }
    }

    return 0;   /* never reached */
}

void locked_routine() {
    LCD_set_cursor(0,0);
    LCD_displayString("Bike is secure");
    _delay_ms(1000);
    LCD_set_cursor(1,0);
    LCD_displayString("Location...");
    _delay_ms(1000);

    //Clear
    LCD_sendCommand(0x01);
    _delay_ms(2);

    //Show location
    display_lat_lon();
    _delay_ms(3000);
}

void unlocked_routine() {
    LCD_set_cursor(0,0);
    LCD_displayString("Bike unlocked");
    _delay_ms(1000);
    LCD_set_cursor(1,0);
    LCD_displayString("Location...");
    _delay_ms(1000);

    //Clear
    LCD_sendCommand(0x01);
    _delay_ms(2);

    //Show location
    display_lat_lon();
    _delay_ms(3000);
}

void broken_into_routine() {

    playTone();

    LCD_set_cursor(0,0);
    LCD_displayString("Bike stolen!");
    _delay_ms(1000);
    LCD_set_cursor(1,0);
    LCD_displayString("Location...");
    _delay_ms(1000);

    //Clear
    LCD_sendCommand(0x01);
    _delay_ms(2);

    //Show location
    display_lat_lon();
    _delay_ms(3000);
}