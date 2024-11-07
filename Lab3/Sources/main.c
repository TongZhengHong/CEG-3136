/*----------------------------------------------
File: Lab3Prog1.c
Description:  Simple program to test out moudules.
--------------------------------------------------*/
#include <mc9s12dg256.h>
/* Notes on mc9s12dg256.h:
   1) the type "byte" is defined as "unsigned char"
*/
#include <stdtypes.h>
#include <ctype.h>
#include <stdio.h>
#include "main_asm.h"
#include "delay_asm.h"
#include "keyPad_asm.h"
#include "SegDisp.h"
#include "lcdDisp.h"

/*----------------------------------------------------
Main program
 Local Variables: 
   ch character returned by readKeyPoll
   displayNum - the display number
Description:
  Program that reads a key presses from the keypad
  and displays the corresponding characters on the
  7 segment displays.
  Uses the Keypad Module and the Segment 
  Display Module
  (will also require the Delay Module);
  Displays on the LCD messages.
---------------------------------------------------*/
void main()
{
   byte displayNum = 0; // 7-segement number
   byte ch;
   char line2[16];  // for second line on LCD display
   
   PLL_init();        // set system clock frequency to 24 MHz
   initKeyPad();
   initDisp();
   initLCD();
   asm cli      // Allow interrupts for control during debugging
   
   printLCDStr("Testing Lab 3 Modules", 0);
   while (1) {
      segDisp(); // This call manages the 4 7-segment displays

      ch = pollReadKey();
      if(ch == NOKEY) continue;

      // On the LCD
      sprintf(line2, "Key: %c, Disp: %d", ch, displayNum);
      printLCDStr(line2, 1);
      // On the 7-segment display
      setCharDisplay(ch, displayNum);
      displayNum = ((byte) (displayNum+1)) % 4;
   }
}
