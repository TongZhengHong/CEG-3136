#include "mc9s12dg256.h"
#include "keyPad.h"

/* ----------------------------------------------------------------------
    Pulse widths: Clock is 24 MHz, period 41 2/3 ns
    Prescale factor 32: Counter tick time is 1 1/3 µs (1/750000)
    Keypad debounce delays for 2 ms
    Number of cycles needed for 2 ms delay: 0.002 / (1/750000) = 1500
----------------------------------------------------------------------- */
#define KEYPAD_DELAY 1500
#define KEYS_RELEASED 0x0F

static volatile unsigned char code = NOKEY;

void initKeyPad() {
    DDRA = 0b11110000;              // Set to PA4-7 as output and PA0-3 as input
    PUCR = 0b00000001;              // Enable pull-up resistors for Port A (bit 0 PUPAE) 

    TIOS |= 0b00010000;             // Set TC4 to Output Compare
    TIE  |= 0b00010000;             // Enable interrupt for TC4
}

char pollReadKey() {
    if (code == NOKEY) {            // ISR has not been called before
        CFORC |= 0b00010000;        // Force trigger timer 4 interrupt
        while (code == NOKEY);      // Wait for key result
    }
    
    char output = code;             // Code should != NOKEY here
    code = NOKEY;
    TC4 = TCNT + KEYPAD_DELAY;      // Debounce for 2 ms
    while (code == NOKEY);          // Wait unit no key has been pressed

    // Check that all keys have been released
    return (PORTA & KEYS_RELEASED) == KEYS_RELEASED 
        ? output
        : NOKEY;
}

char readKey() {
    do {
        PORTA = 0b00000000;             // Enable all rows
    } while ((PORTA & KEYS_RELEASED) == KEYS_RELEASED);   // Wait for a button press

    byte prevCode = translate(PORTA);
    code = NOKEY;
    TC4 = TCNT + KEYPAD_DELAY;          // Debounce for 2 ms
    while (code == NOKEY);

    if (prevCode == code) {             // Rising edge debounced
        return pollReadKey();
    }
    return NOKEY;
}

char translate(char portA) {
    switch (portA) {
    case 0b11101110:
        return '1'
    case 0b11101101:
        return '2'
    case 0b11101011:
        return '3'
    case 0b11100111:
        return 'a'
    case 0b11011110:
        return '4'
    case 0b11011101:
        return '5'
    case 0b11011011:
        return '6'
    case 0b11010111:
        return 'b'
    case 0b10111110:
        return '7'
    case 0b10111101:
        return '8'
    case 0b10111011:
        return '9'
    case 0b10110111:
        return 'c'
    case 0b01111110:
        return '*'
    case 0b01111101:
        return '0'
    case 0b01111011:
        return '#'
    case 0b01110111:
        return 'd'
    default:
        return BADCODE;
    }
}

void interrupt VectorNumber_Vtimch4 tc4_isr(void) {
    for (int i = 0; i < 4; i++) {
        byte outputMask = ~(1 << i);    // Invert due to pull up resistors
        PORTA |= outputMask << 4;       // Shift to upper 4 bits for output
        code = translate(PORTA);

        if (code != BADCODE) break;     // Terminate if valid code is obtained
    }
}