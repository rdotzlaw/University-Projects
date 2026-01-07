.orig x3000

; Register dictionary
; r0 for holding current index
; r1 for holding remaining array size
; r2 for holding value @r0, without modifying array
    and r0, r0, x0
    and r1, r1, x0
    and r2, r2, x0
    and r3, r3, x0
    lea r0, array
    ld r1, length
    
loop
    brz endloop
    
    ; check if odd
    ; only deal with last bit
    and r2, r2, x0
    ldr r2, r0, #0 ; put data @ r0 into r2
    and r2, r2, x1 ; remove all but LSB
    ; if equaled 1, then odd else even
    brz else ; if even
    br if ; if odd

if
    ; if odd
    and r3, r3, x0
    lea r2, sumOdd
    ld r3, sumOdd
    add r3, r3, #1
    str r3, r2, #0
    br loop2
else
    ; if even
    and r3, r3, x0
    lea r2, sumEven
    ld r3, sumEven
    add r3, r3, #1
    str r3, r2, #0
    br loop2
loop2  
    add r0, r0, #1 ; increment index
    add r1, r1, #-1 ; reduce remaining value count
    br loop
endloop
    lea r0, eopMsg ; display the eop msg
    puts
    halt

eopMsg  .stringz "\nProgrammed by Ryan Dotzlaw\nEnd of processing.\n"
sumOdd  .blkw #1 ; should be 6
sumEven .blkw #1 ; should be 4
length  .fill #10
array   .fill #12
        .fill #1
        .fill #-9
        .fill #-1
        .fill #2
        .fill #1
        .fill #2
        .fill #3
        .fill #-9
        .fill #-8
.end

