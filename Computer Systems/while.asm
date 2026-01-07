.orig x3000

; Registers
;   r0: i/o
;   r1: long term i/o storage
;   r2: loop condition
;   r3,r4 : math


loop
    lea r0, start
    puts    ; output start message
    
    ; take user input
    getc
    and r1, r1, x0  ; clear r1
    add r1, r0, x0  ; load char into r1
    and r2, r2, x0  ; clear r2
    
    ; if char entered is != x04, continue loop
    add r2, r1, x-04 
    brz endloop
    ; otherwise, will continue into if
    
if
    ; need to see if char is in range of [A, Z]
    ; if so, shift ascii vals so char is the coresponding lowercase char
    
    ; char >= A and char <= Z
    ; so char >= #65 and char <= #90
    ; char - #65 < 0 -> endif
    ; char - #90 > 0 -> endif
    
    and r4, r4, x0
    and r3, r3, x0 ; clear r3
    ld r3, low
    add r4, r1, r3 ; r3: #-65, r1: char
    brn endif
    
    and r4, r4, x0
    and r3, r3, x0
    ld r3, up
    add r4, r1, r3
    brp endif
    
    ; if havent branched, shift char
    and r3, r3, x0
    ld r3, shift
    add r0, r0, r3
    ; will then continue into endif
    
endif
    ; print out whatever is in r0, then newline
    ; and continue the loop
    out         ; print char in r0
    lea r0, nl
    puts        ; new line
    br loop     ; continue loop
    
    
endloop
    lea r0, ending
    puts
    halt
    
shift   .fill #32 ; gap of 32 between A and a
up      .fill #-90
low     .fill #-65
start   .stringz "Enter a character: "
nl      .stringz "\n"
ending  .stringz "\nProgrammed by Ryan Dotzlaw\nEnd of processing."
.end