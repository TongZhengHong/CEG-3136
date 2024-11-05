/*--------------------------------------------
File: SegDisp.c
Description:  Segment Display Module
---------------------------------------------*/

#include <stdtypes.h>
#include "mc9s12dg256.h"
#include "SegDisp.h"
#include "delay_asm.h"

static unsigned char segNumbers[4] = { 0x00, 0x00, 0x00, 0x00 };
const char segments[10] = {
    0b10011111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111
}

// Prototypes for internal functions

/*---------------------------------------------
Function: initDisp
Description: initializes hardware for the 
             7-segment displays.
-----------------------------------------------*/
void initDisp(void) {
    DDRB = 0xFF;        // Set PORTB pins to output
    DDRP = 0x0F;        // Set PORTP pins 0-3 to output
    clearDisp();
}

/*---------------------------------------------
Function: clearDisp
Description: Clears all displays.
-----------------------------------------------*/
void clearDisp(void) {
    PORTB = 0x00;       // Set PORTB pins to 0 (LOW)
    PTP = 0x0F;       // Set PORTP pins 0-3 to 1 (HIGH)

    segNumbers[0] = 0;
    segNumbers[1] = 0;
    segNumbers[2] = 0;
    segNumbers[3] = 0;
}

/*---------------------------------------------
Function: setCharDisplay
Description: Receives an ASCII character (ch)
             and translates it to the corresponding code to 
             display on 7-segment display.  Code
             is stored in appropriate element of
             codes for identified display (dispNum).
-----------------------------------------------*/
void setCharDisplay(char ch, byte dispNum) {
    if (dispNum <= 3 && ch >= '0' && ch <= '9') {
        char segment = segments[ch - 48]; // Offset acsii value to 0
        segNumbers[dispNum] = segment;  // Store segment value in memory
    }
}

/*---------------------------------------------
Function: segDisp
Description: Displays the codes in the code display table 
             (contains four character codes) on the 4 displays 
             for a period of 100 milliseconds by displaying 
             the characters on the displays for 5 millisecond 
             periods.
-----------------------------------------------*/
void segDisp(void) {
    char pointer = 0;
    int delayCount = 100;

    while (delayCount > 0) {
        if (delayCount % 5 == 0) {
            PORTB = segNumbers[pointer];
            PTP = ~(1 << pointer);
            pointer = pointer >= 3 ? 0 : pointer + 1;
        }
        delayCount--;
        delayms(1);
    }
}
