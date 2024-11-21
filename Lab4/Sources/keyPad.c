#include "mc9s12dg256.h"
#include "keyPad.h"

/* ----------------------------------------------------------------------
    Pulse widths: Clock is 24 MHz, period 41 2/3 ns
    Prescale factor 32: Counter tick time is 1 1/3 µs (1/750000)
    Keypad debounce delays for 2 ms
    Number of cycles needed for 2 ms delay: 0.01 / (1/750000) = 7500
----------------------------------------------------------------------- */
#define TEN_MS 7500
#define KEYS_RELEASED 0x0F

char translate(char);
char getKeyCode(void);

volatile unsigned char keyCode = NOKEY;

void initKeyPad() {
    DDRA = 0b11110000;              // Set to PA4-7 as output and PA0-3 as input
    PUCR = 0b00000001;              // Enable pull-up resistors for Port A (bit 0 PUPAE) 

    TIOS |= (1 << 4);               // Set TC4 to Output Compare
    TIE  |= (1 << 4);               // Enable interrupt for TC4

    TC4 = TCNT + TEN_MS;
    keyCode = NOKEY;
}

char pollReadKey() {
    if (keyCode == NOKEY) 
        return NOKEY;
    else {
        char ch = keyCode;
        keyCode = NOKEY;
        return ch;
    }
}

char readKey() {
    // Wait until a key has been pressed
    while (keyCode == NOKEY);

    // Obtain code and reset keyCode
    char ch = keyCode;
    keyCode = NOKEY;
    return ch;
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

char getKeyCode() {
    for (int i = 0; i < 4; i++) {
        PORTA |=  ~(1 << (i + 4));      // Shift to upper 4 bits for output pins
        code = translate(PORTA);

        if (code != BADCODE) break;     // Terminate if valid code is obtained
    }
}

#define WAIT_PRESS          1
#define DEBOUNCE_PRESS      2
#define WAIT_RELEASE        3
#define DEBOUNCE_RELEASE    4

void interrupt VectorNumber_Vtimch4 tc4_isr(void) {
    static byte state = WAIT_PRESS;
    static byte prevCode;

    switch (state) {
    case WAIT_PRESS:
        prevCode = PORTA;
        // Move to debounce if any key has been pressed
        if (PORTA & KEYS_RELEASED != KEYS_RELEASED)
            state = DEBOUNCE_PRESS;
        break;

    case DEBOUNCE_PRESS:
        if (PORTA == prevCode) {
            prevCode = getKeyCode();
            state = WAIT_RELEASE; 

        } else state = WAIT_PRESS;
        break;

    case WAIT_RELEASE:
        if (PORTA == KEYS_RELEASED) 
            state = DEBOUNCE_RELEASE;
        break;

    case DEBOUNCE_RELEASE:
        if (PORTA == KEYS_RELEASED) {
            keyCode = prevCode;
            state = WAIT_PRESS

        } else state = WAIT_RELEASE;
        break;
    
    default:
        break;
    }

    TC4 += TEN_MS; 
}
