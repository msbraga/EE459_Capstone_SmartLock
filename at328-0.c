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
bool unlocked_legally = false;
//bool unlocked_illegally = false;
bool prev_locked = false;

int main(void)
{
    //// Main Bike Program 
    Mux_init();
    USART_Init(MYUBRR);
    SPI_init();
    LCD_init();
    initializeBuzzer();

    //for current sensor
    adc_init();

    //Clear and move cursor 
    LCD_sendCommand(0x01);
    _delay_ms(2);
    LCD_sendCommand(0x80);

    select_GPS();

    adc_select_channel(1); // Assuming PC1 is channel 1



    while (1) {

       // bool fingerprint = is_finger_present();
        bool fingerprint = false;


        uint16_t adc_value = adc_read(); 

    
        char adc_buffer[32]; // Ensure the buffer is large enough for the string and null terminator

/*
        floatToStr(adc_buffer, adc_value, 3); // Formats the float with two decimal places
        LCD_set_cursor(0,0);
        LCD_displayString(adc_buffer); 
        _delay_ms(500); */
/*
        if(fingerprint) {
            LCD_displayString("unlocked");
            _delay_ms(500);
        } else {
            LCD_displayString("no");
            _delay_ms(500);
        } */

        if (unlocked_legally == false) {
           if (adc_value < 217) { //LOCKED - Change this threshold 
                locked_routine();
            }  
            else if (fingerprint){ //UNLOCKED 
                unlocked_routine();
            } else {
                broken_into_routine();
            } 
        } else if (unlocked_legally == true){
            LCD_set_cursor(0,0);
            LCD_displayString("Bike unlocked!");
            _delay_ms(4000);
            unlocked_legally = false;
            prev_locked = false;
        } 
    }
   

    return 0;   /* never reached */
}

void locked_routine() {
    if (prev_locked == false) {
        LCD_set_cursor(0,0);
        LCD_displayString("Bike is secure");
         LCD_set_cursor(1,0);
         LCD_displayString("Location...");
        _delay_ms(1000);
        //Clear
        LCD_sendCommand(0x01);
        _delay_ms(2);

         //Show location
        display_lat_lon();
        _delay_ms(300);
    }
    prev_locked = true;
}

void unlocked_routine() {
    LCD_set_cursor(0,0);
    LCD_displayString("Bike unlocked");
    _delay_ms(500);
    LCD_set_cursor(1,0);
    LCD_displayString("Location...");
    _delay_ms(1000);

    //Clear
    LCD_sendCommand(0x01);
    _delay_ms(2);

    //Show location
    display_lat_lon();
    _delay_ms(1000);
    unlocked_legally = true;
}

void broken_into_routine() {

    playTone();

    LCD_set_cursor(0,0);
    LCD_displayString("Bike stolen!");
    _delay_ms(1000);

    //Clear
    LCD_sendCommand(0x01);
    _delay_ms(2);

    prev_locked = false;

}
