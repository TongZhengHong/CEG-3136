;-----Conversion table
NUMKEYS	EQU	16	; Number of keys on the keypad
BADCODE 	EQU	$FF 	; returned of translation is unsuccessful
NOKEY		EQU 	$00   ; No key pressed during poll period
POLLCOUNT	EQU	1     ; Number of loops to create 1 ms poll time

 SWITCH globalConst  ; Constant data

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

 SWITCH code_section  ; place in code section
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
	pshx	; preserve registers
	leas -1,SP 		    ; byte chascii;
	ldx #cnvTbl		    ; ptr = cnvTbl;
	clra			    ; ix = 0;
	movb #BADCODE,TR_CH,SP ; ch = BADCODE;

TR_loop 			    ; do {

	; IF code = [ptr]
	cmpb cnvTbl_code,X  	    ;     if(code == ptr->code)
	bne TR_endif
				    ;     {
	movb cnvTbl_ascii,X,TR_CH,SP ;        ch <- [ptr+1]
	bra TR_endwh  		    ;         break;
TR_endif  			    ;     }
				    ;     else {	
	leax cnvTbl_struct_len,X    ;           ptr++;
	inca ; increment count      ;           ix++;
                                    ;     }	
	cmpa #NUMKEYS               ;} WHILE count < NUMKEYS
	blo TR_LOOP	
tr_endwh ; ENDWHILE

	pulb ; move ch to Acc B
	; restore registres
	pulx
	pula
	rts


