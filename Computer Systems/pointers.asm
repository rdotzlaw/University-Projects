.orig x3000
; r0 is for holding input and output
; r2 is for holding 'y' or 'n's 2's complement
    lea r0, prompt
    puts
    ;  Get a character from the keyboard, if it 'Y' display '1',
    ;  if it is 'N' display '0', otherwise display '*'.
    ;  Do not display the quote characters
    getc ; puts char in r0
    and r2, r2, x0
    ld r2, Y
    add r2, r0, r2 ; if zero, then char is 'y'
    brz yes
    ld r2, N
    add r2, r0, r2 ; if zero, then char is 'n'
    brz no
    ; otherwise its neither
    br nei
    
    yes
        ldi r0, yesCharPtr
        out
        br done
    no
        ldi r0, noCharPtr
        out
        br done
    nei
        ldi r0, neitherPtr
        out
        br done
done
    lea r0, eopMsg
    puts
    halt
Y           .fill xFFA7 ; 2's comp of 'y'
N           .fill xFFB2 ; 2's comp of 'n'
yesCharPtr  .fill yesChar
noCharPtr   .fill noChar
neitherPtr  .fill neither
yesChar     .fill x0031
noChar      .fill x0030
neither     .fill x002A
prompt      .stringz "Enter a character.\n"
eopMsg      .stringz "\nProgrammed by Ryan Dotzlaw\nEnd of processing.\n"
.end