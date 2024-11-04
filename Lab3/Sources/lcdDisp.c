/*-------------------------------------
File: lcdDisp.c  (LCD Diplay Module)

Description: C Module that provides
             display functions on the
             LCD. It makes use of the LCD ASM 
             Module developed in assembler.
-------------------------------------*/
#include "alarm.h"
#include "lcd_asm.h"

/*--------------------------
Function: initLCD
Parameters: None.
Returns: nothing
Description: Initialised the LCD hardware by
             calling the assembler subroutine.
             This function that simply calls the 
             lcd_init() function provided by the 
             LCD ASM Module.
---------------------------*/

void initLCD(void) {
  lcd_init();
}

/*--------------------------
Function: printStr

Parameters: str - pointer to string to be printed 
                  (only 16 chars are printed)
            lineno - 0 first line
                     1 second line
Returns: nothing

Description: Prints a string on the display on one of the
             two lines.  String is padded with spaces to
             erase any existing characters.
---------------------------*/
void printLCDStr(char *str, byte lineno) {
  const char* emptyLine = "                ";  // 16 spaces for whole line 

  char offset = lineno == 0 ? 0x00 : 0x40;
  set_lcd_addr(offset);
  type_lcd(emptyLine);    // Clear the line first 

  set_lcd_addr(offset);
  type_lcd(str);          // Write the content to the corresponding line
}
