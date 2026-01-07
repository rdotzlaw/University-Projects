; Keyboard interrupt test
; Each time a key is pressed, echo the character.
; Pretty standard code that waits for interrupts and then processes them.


.orig x3000

; this is the main line
Main
	LD R6,STACKBASE		; set up user stack

	LEA R0,KBHandler		
	LD R1,KBVEC
	STR R0,R1,#0		; set kb interrupt vector

	LD R0,KBEN		; enable keyboard interrupt
	STI R0,KBSR

Loop
	BR Loop			; loop forever

	HALT
; ---------------------------------------------------------------
; Keyboard interrupt handler (no need for Frame pointer)
KBHandler
	ADD R6,R6,#-1	; save R0
	STR R0,R6,#0
	ADD R6,R6,#-1	; save R1
	STR R1,R6,#0

	LDI R0,KBDR	; get key
	
	ld r2, ascii ; r2 is count, starting from #48 (ascii for 0)
	and r3, r3, x0
	add r3, r3, x1
	; keep in mind, since ascii chars only go up to #255, the max value for r2 is #8 + #48 = ascii 8
	; modify r0 into count of 1's in r0
cloop
    ; loop until r3 + pow = 0
    ; pow is b-1000 0000, meaning after added w/ r3 = 0, there are no more bits to check
    ld r4, pow
    add r4, r3, r4 ; if 0 break
    brz cdone ; else continue counting 1's
    ; each loop and r0, r3, if result isn't 0, add 1 to r2
    and r0, r3, r0 ; if not 0
    brp inc
postinc
    ; then ldi r0, kbdr, double r3, and repeat loop
    ldi r0, kbdr
    add r3, r3, r3 ; double 
    br cloop
    
inc ; increment r2, then go back
    add r2, r2, x1
    br postinc
cdone
    ; load r2 into r0
    and r0, r0, x0
    add r0, r2, x0

WaitToWrite
	LDI     R1,DSR	;get console output status
	BRzp    WaitToWrite

	STI R0,DDR	;write new character to console

	LDR R1,R6,#0	;restore the registers
	ADD R6,R6,#1

	LDR R0,R6,#0
	ADD R6,R6,#1

	RTI		;return from interrupt


STACKBASE   	.FILL   x4000	;stack base (can be changed)
KBSR 	    	.FILL	xFE00	;keyboard status register
KBDR    	.FILL	xFE02	;keyboard data register 
DSR	        .FILL	xFE04	;console status register
DDR     	.FILL	xFE06	;console data register 

ascii   .fill   #48
pow     .fill   #-128
KBEN	.FILL	x4000	;use to enable keyboard interrupt
KBVEC	.FILL	x0180	;keyboard vector number/location

	.END