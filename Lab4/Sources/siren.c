
#include "mc9s12dg256.h"
#include "siren.h"

/* ----------------------------------------------------------------------
    Pulse widths: Clock is 24 MHz, period 41 2/3 ns
    Prescale factor 32: Counter tick time is 1 1/3 µs (1/750000)
    For a 440 Hz signal, each period is 2272.72 µs (1/440)
    To achieve a 50% duty cycle, high and low time are 1136.36 us (1/880)
    Pin should toggle every 852 cycles (750000/880)
----------------------------------------------------------------------- */
#define SIREN_PULSE 852

void initSiren() {
    TIOS |= 0b00100000; // Set TC5 to Output Compare mode
}

void turnOnSiren() {
    // Set toggle mode on output pin of timer channel 5
    TCTL1 |= 0b00001100; // Set OC5 pin to HIGH
    CFORC |= 0b00100000; // Force the 5V on the pin
    TCTL1 &= 0b11110111; // Clear OM5 to 0 for toggle mode

    TIE |= 0b00100000;  // Enable interrupt for TC5

    TC5 = TCNT + SIREN_PULSE; // Initialise output compare channel 5
}

void turnOffSiren() {
    // Clear OC5 output pin
    TCTL1 |= 0b00001000; // Set OM5 to 1
    TCTL1 &= 0b11111011; // Clear OL5 to 0 

    CFORC |= 0b00100000; // Force LOW to OC5

    TCTL1 &= 0b11110011; // Clear OM5 and OL5 to disconnect timer from OC5
}

void interrupt VectorNumber_Vtimch5 tc5_isr(void) {
    TC5 += SIREN_PULSE;  // Reset timer5 interrupt
}