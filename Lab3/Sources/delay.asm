;------------------------------------------------------
; Alarm System Assembler Program
; File: delay.asm
; Description: The Delay Module
; Author: Gilbert Arbez
; Date: Fall 2010
;------------------------------------------------------
TRUE          equ    1
FALSE         equ    0


;--- Define External Symbols
 XDEF delayms, setDelay, polldelay

; Some definitions
MSCOUNT equ 3000

.text: SECTION

;-------------------------------
; Subroutine delayms
; Parameters: num - number of milliseconds to delay - in D
; Returns: nothing
; Description: Delays for num ms. 
;--------------------------------
delayms: pshd  
dlms_while:           ; do
   jsr delay1ms       ; delay1ms()
   dbne d,dlms_while  ; num--; while(num !=0)
dlms_endwhile:
   puld            ; restores register
   rts

;------------------------------------------------------
; Subroutine setDelay
; Parameters: cnt - accumulator D
; Returns: nothing
; Global Variables: delayCount
; Description: Intialises the delayCount 
;              variable.
;------------------------------------------------------
setDelay: 
   std delayCount     ; delayCount = cnt;
   rts


;------------------------------------------------------
; Subroutine: polldelay
; Parameters:  none
; Returns: TRUE when delay counter reaches 0 - in accumulator A
; Local Variables
;   retval - acc A cntr - X register
; Global Variables:
;      delayCount
; Description: The subroutine delays for 1 ms, decrements delayCount.
;              If delayCount is zero, return TRUE; FALSE otherwise.
;------------------------------------------------------
; Stack Usage:
	OFFSET 0  ; to setup offset into stack
PDLY_VARSIZE:
PDLY_PR_X   DS.W 1 ; preserve X
PDLY_RA     DS.W 1 ; return address

polldelay:  pshx
   ldaa #FALSE	   ; byte retval=FALSE; // return value
   jsr delay1ms	   ; delay1ms();
   ldx delayCount    ; delayCount--;
   dex
   stx delayCount
pld_if:
   bne pld_endif    ; if(delayCount==0) 
   ldaa #TRUE        ; retval=TRUE;
pld_endif:
   ; restore registers and stack
   pulx
   rts

;------------------------
; Subroutine: delay1ms
; Parameters:  none
; Returns: nothing
; Description: The subroutine delays for 1 ms and returns.      
;   Core Clock is set to 24 MHz, so 1 cycle is 41 2/3 ns
;   NOP takes up 1 cycle, thus 41 2/3 ns
;   Need 24 cyles to create 1 microsecond delay
;   8 cycles creates a 333 1/3 nano delay
;	DEX - 1 cycle
;	BNE - 3 cyles - when branch is taken
;	Need 4 NOP
;   Run Loop 3000 times to create a 1 ms delay   
;---------------------------
; Stack Usage:
	OFFSET 0  ; to setup offset into stack
DLY1_VARSIZE:
DLY1_PR_X   DS.W 1 ; preserve X
DLY1_RA     DS.W 1 ; return address

delay1ms: pshx
   ldx #MSCOUNT	   ; byte cntr = MSCOUNT;
dly1_while:
   beq dly1_endwhile ; while(cntr != 0)
                     ; {
   nop               ;   asm { nop; nop; nop; nop; }
   nop
   nop
   nop
   dex               ;   cntr--;
   bra dly1_while    ; }
dly1_endwhile:
   pulx
   rts

;------------------------------------------------------
; Global variables
;------------------------------------------------------
.rodata SECTION

delayCount ds.w 1   ; 2 byte delay counter
