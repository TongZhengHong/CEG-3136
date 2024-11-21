
#include "mc9s12dg256.h"
#include "SegDisp.h"

/* ----------------------------------------------------------------------
    Pulse widths: Clock is 24 MHz, period 41 2/3 ns
    Prescale factor 32: Counter tick time is 1 1/3 µs (1/750000)
    Segment display updates every 5 ms
    Number of cycles needed for 5 ms delay: 0.005 / (1/750000) = 3750
----------------------------------------------------------------------- */
#define SEG_DELAY 3750

static unsigned char segNumbers[4] = { 0x00, 0x00, 0x00, 0x00 };
static unsigned char dpState = 0b00000000;
const char segments[10] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111
};

static volatile int segDispPointer = 0; 

/*---------------------------------------------
Function: initDisp
Description: initializes hardware for the 
             7-segment displays.
-----------------------------------------------*/
void initDisp(void) {
    DDRB = 0xFF;        // Set PORTB pins to output
    DDRP = 0x0F;        // Set PORTP pins 0-3 to output
    clearDisp();

    TIOS |= 0b00001000; // Set TC3 to Output Compare
    TIE  |= 0b00001000; // Enable interrupt for TC3
    TC3 = TCNT + SEG_DELAY;
}

/*---------------------------------------------
Function: clearDisp
Description: Clears all displays.
-----------------------------------------------*/
void clearDisp(void) {
    PORTB = 0x00;       // Set PORTB pins to 0 (LOW)
    PTP = 0x0F;         // Set PORTP pins 0-3 to 1 (HIGH)

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
    if (dispNum <= 3) {
        if (ch >= '0' && ch <= '9') {
            char segment = segments[ch - 48];   // Offset acsii value to 0
            segNumbers[dispNum] = segment;      // Store segment value in memory

        } else if (ch == 'A' || ch == 'a') {
            segNumbers[dispNum] = 0b01110111;
        } else {
            segNumbers[dispNum] = 0;
        }
    }
}

void turnOnDP(int index) {
    if (index >= 0 && index <= 3) {
        dpState |= (1 << index);
    }
}

void turnOffDP(int index) {
    if (index >= 0 && index <= 3) {
        dpState &= ~(1 << index);
    }
}

void interrupt VectorNumber_Vtimch3 tc3_isr(void) {
    PORTB = segNumbers[segDispPointer];
    if (dpState & (1 << segDispPointer)) 
        PORTB |= 0b10000000;    // Enable DP bit 7 (MSB)

    PTP = ~(1 << segDispPointer);
    segDispPointer = (segDispPointer + 1) % 4;

    // Reset TC3 interrupt for next delay  
    TC3 += SEG_DELAY; 
}
