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

void Mux_init() {

    // Set the MUX select pin as an output
    MUX_SELECT_DDR |= (1 << MUX_SELECT_PIN);
}