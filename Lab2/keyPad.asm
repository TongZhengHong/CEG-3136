;----------------------------------------------------------------------
; File: Keypad.asm
; Author:

; Description:
;  This contains the code for reading the
;  16-key keypad attached to Port A
;  See the schematic of the connection in the
;  design document.
;
;  The following subroutines are provided by the module
;
; char pollReadKey(): to poll keypad for a keypress
;                 Checks keypad for 2 ms for a keypress, and
;                 returns NOKEY if no keypress is found, otherwise
;                 the value returned will correspond to the
;                 ASCII code for the key, i.e. 0-9, *, # and A-D
; void initkey(): Initialises Port A for the keypad
;
; char readKey(): to read the key on the keypad
;                 The value returned will correspond to the
;                 ASCII code for the key, i.e. 0-9, *, # and A-D
;---------------------------------------------------------------------

; Include header files
 include "sections.inc"
 include "reg9s12.inc"  ; Defines EQU's for Peripheral Ports

**************EQUATES**********

;-----Conversion table
NUMKEYS		EQU	16		; Number of keys on the keypad
BADCODE 	EQU	$FF 	; returned of translation is unsuccessful
NOKEY		EQU $00 	; No key pressed during poll period
POLLCOUNT	EQU	1     	; Number of loops to create 1 ms poll time
PDELAY  	RMB 1		; Variable to achieve propagation delay

 SWITCH globalConst  	; Constant data

 SWITCH code_section  	; place in code section
;-----------------------------------------------------------	
; Subroutine: initKeyPad
;
; Description: 
; 	Initiliases PORT A by setting input and output pins 
; 	and enabling pull resistors
;-----------------------------------------------------------	
initKeyPad: psha

	ldaa #$F0   		; Set to PA4-7 as output and PA0-3 as input
  	staa DDRA

	ldaa #$01			; Enable pull-up resistors for Port A (bit 0 PUPAE)
	staa PUCR 

	pula
	rts

;-----------------------------------------------------------    
; Subroutine: ch <- pollReadKey
; Parameters: none
; Local variable:
; Returns:
;       ch: NOKEY when no key pressed,
;       otherwise, ASCII Code in accumulator B

; Description:
;  Loops for a period of 2ms, checking to see if
;  key is pressed. Calls readKey to read key if keypress 
;  detected (and debounced) on Port A and get ASCII code for
;  key pressed.
;-----------------------------------------------------------
; Stack Usage
	OFFSET 0  ; to setup offset into stack

readKey: psha

prk_initial_loop:
	ldaa #$00				; do {
	staa PORTA				; 	PORTA = 0b00000000 // Set all the output pins to LOW to detect key press
	ldaa PORTA						
	anda #%00001111				
	cmpa #%00001111				
	beq prk_initial_loop	; } while (PORTA & 0x0F == 0x0F);

	ldab PORTA        		; prev_PORTA = PORTA // Store in accumulator B

	pshd
	ldd #2					; delayms(2);
	jsr delayms
	puld

	ldaa PORTA				; if (PORTA != prev_PORTA)
	cba	
	bne prk_nokey			; 	return NOKEY;

	jsr pollReadKey			; code = readKey();
	cmpb #BADCODE			; if (code != BADCODE)
	bne prk_end				; 	return code;
							; else 
prk_nokey:
	ldab #NOKEY				; 	return NOKEY;

prk_end:
	pula
  	rts

;-----------------------------------------------------------	
; Subroutine: ch <- readKey
; Arguments: none
; Local variable: 
;	ch - ASCII Code in accumulator B

; Description:
;  Main subroutine that reads a code from the
;  keyboard using the subroutine readKeybrd.  The
;  code is then translated with the subroutine
;  translate to get the corresponding ASCII code.
;-----------------------------------------------------------	
; Stack Usage
	OFFSET 0  ; to setup offset into stack

pollReadKey:psha

	ldaa #%11100000
	staa PORTA					; PORTA = 11100000 		// Set row 0
	jsr propagation_delay		; propagation_delay(); 	// delay for stability
	ldab PORTA					; code = translate(PORTA);
	jsr translate
	cmpb #BADCODE				; if (code == BADCODE)
	bne rk_debounce				; 	rk_debounce();

	ldaa #%11010000
	staa PORTA					; PORTA = 11010000 		// Set row 1
	jsr propagation_delay		; propagation_delay(); 	// delay for stability
	ldab PORTA					; code = translate(PORTA);
	jsr translate
	cmpb #BADCODE				; if (code == BADCODE)
	bne rk_debounce				; 	rk_debounce();

	ldaa #%10110000
	staa PORTA					; PORTA = 10110000 		// Set row 2
	jsr propagation_delay		; propagation_delay(); 	// delay for stability
	ldab PORTA					; code = translate(PORTA);
	jsr translate
	cmpb #BADCODE				; if (code == BADCODE)
	bne rk_debounce				; 	rk_debounce();

	ldaa #%01110000
	staa PORTA					; PORTA = 01110000 		// Set row 3
	jsr propagation_delay		; propagation_delay(); 	// delay for stability
	ldab PORTA					; code = translate(PORTA);
	jsr translate
		
rk_debounce:
	ldaa PORTA					; while (PORTA & 0x0F < 0x0F);
	anda #$0F						 
	cmpa #$0F
	blt rk_debounce

	pshb
	ldd #10						; delayms(10);
	jsr delayms
	pulb

	ldaa PORTA					; if (PORTA & 0x0F == 0x0F) 
	anda #$0F					; 	// Confirm that all buttons are released (inputs all 1s)
	cmpa #$0F
	beq rk_end

rk_return_bad:
	ldab #BADCODE				; 	return(BADCODE);

rk_end:
	pula
	rts							; return(ch);

;-----------------------------------------------------------	
; Subroutine:  propagation_delay
; Arguments: none
; Returns: none
; Local Variables
;    	PDELAY - in Reserve Memory Bytes (RMB) - number of delay cycles
; Description:
;   Creates a short delay used in between setting outputs of 
; 	PORTA and reading inputs of PORTA to ensure stability
;-----------------------------------------------------------	
; Stack Usage:
   OFFSET 0
propagation_delay: movb  #$08, PDELAY 
pdelay_loop:                  		
		dec   PDELAY
		bne   pdelay_loop
		rts

; defnitions for structure cnvTbl_struct
 OFFSET 0
cnvTbl_code ds.b 1
cnvTbl_ascii  ds.b 1
cnvTbl_struct_len EQU *

; Conversion Table
cnvTbl  dc.b %11101110,'1'
	dc.b %11101101,'2'
	dc.b %11101011,'3'
	dc.b %11100111,'a'
	dc.b %11011110,'4'
	dc.b %11011101,'5'
	dc.b %11011011,'6'
	dc.b %11010111,'b'
	dc.b %10111110,'7'
	dc.b %10111101,'8'
	dc.b %10111011,'9'
	dc.b %10110111,'c'
	dc.b %01111110,'*'
	dc.b %01111101,'0'
	dc.b %01111011,'#'
	dc.b %01110111,'d'

;-----------------------------------------------------------	
; Subroutine:  ch <- translate(code)
; Arguments
;	code - in Acc B - code read from keypad port
; Returns
;	ch - saved on stack but returned in Acc B - ASCII code
; Local Variables
;    	ptr - in register X - pointer to the table
;	count - counter for loop in accumulator A
; Description:
;   Translates the code by using the conversion table
;   searching for the code.  If not found, then BADCODE
;   is returned.
;-----------------------------------------------------------	
; Stack Usage:
   OFFSET 0
TR_CH DS.B 1  ; for ch 
TR_PR_A DS.B 1 ; preserved regiters A
TR_PR_X DS.B 1 ; preserved regiters X
TR_RA DS.W 1 ; return address

translate: psha
	pshx								; preserve registers
	leas -1,SP 		    				; byte chascii;
	ldx #cnvTbl		    				; ptr = cnvTbl;
	clra			    				; ix = 0;
	movb #BADCODE,TR_CH,SP 				; ch = BADCODE;

TR_loop 			    				; do {
	cmpb cnvTbl_code,X  	    		;     if(code == ptr->code)
	bne TR_endif
				    					;     {
	movb cnvTbl_ascii,X,TR_CH,SP 		;        ch <- [ptr+1]
	bra TR_endwh  		    			;     	 break;

TR_endif  			    				;     } else {	
	leax cnvTbl_struct_len,X    		;           ptr++;
	inca 	; increment count      		;           ix++;
                                		;     }	
	cmpa #NUMKEYS               		; } WHILE (count < NUMKEYS)
	blo TR_LOOP	
tr_endwh 								; ENDWHILE

	pulb ; move ch to Acc B
	; restore registres
	pulx
	pula
	rts
