#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa() if available, or you might need to implement it
#include <string.h> // For strcat and strcpy

//LCD definitions
#define LCD_RS PD2
#define LCD_EN PD3
#define LCD_D4 PD4
#define LCD_D5 PD5
#define LCD_D6 PD6
#define LCD_D7 PD7
#define LCD_PORT PORTD
#define LCD_DDR DDRD

void LCD_sendCommand(unsigned char command);
void LCD_displayCharacter(unsigned char data);
void LCD_init();
void LCD_displayString(const char *str);

void LCD_pulseEnable() {
    LCD_PORT |= (1 << LCD_EN);
    _delay_us(1); // Enable pulse width >= 300ns
    LCD_PORT &= ~(1 << LCD_EN);
    _delay_us(100); // Commands need >37us to settle
}

void LCD_sendNibble(unsigned char data) {
    LCD_PORT &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7); // Clear data bits
    if (data & 1) LCD_PORT |= (1 << LCD_D4);
    if (data & 2) LCD_PORT |= (1 << LCD_D5);
    if (data & 4) LCD_PORT |= (1 << LCD_D6);
    if (data & 8) LCD_PORT |= (1 << LCD_D7);
    LCD_pulseEnable();
}

void LCD_sendCommand(unsigned char command) {
    LCD_PORT &= ~(1 << LCD_RS); // Set to command mode
    LCD_sendNibble(command >> 4); // Send upper nibble first
    LCD_sendNibble(command & 0x0F); // Then lower nibble
}

void LCD_displayCharacter(unsigned char data) {
    LCD_PORT |= (1 << LCD_RS); // Set to data mode
    LCD_sendNibble(data >> 4); // Send upper nibble first
    LCD_sendNibble(data & 0x0F); // Then lower nibble
}

void LCD_displayString(const char *str) {
    while (*str != '\0') { // Loop through the string until the null terminator
        LCD_displayCharacter(*str); // Display the current character
        str++; // Move to the next character
    }
}

void LCD_init() {
    // Configure control and data pins as output
   LCD_DDR |= (1 << LCD_RS) | (1 << LCD_EN) | (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7); // Set control pins as output
    _delay_ms(40); // Wait for more than 40ms after Vcc rises to 2.7V
    

    // Initialization sequence for 4-bit mode
    LCD_sendNibble(0x03);
    _delay_ms(4.1);
    LCD_sendNibble(0x03);
    _delay_us(100);
    LCD_sendNibble(0x03);
    LCD_sendNibble(0x02); // Set to 4-bit interface
    
    // Function set: 4-bit/2-line/5x8 dots
    LCD_sendCommand(0x28); 
    // Display on, cursor off, blink off 
    LCD_sendCommand(0x0C);
    // Clear display
    LCD_sendCommand(0x01);
    _delay_ms(2);
    // Entry mode set: Increment cursor, no shift
    LCD_sendCommand(0x06);

} 