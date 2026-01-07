.orig x3000
; programmed by Ryan Dotzlaw
; r0: holding input, and acting as a counter
; r1: for calculating -2X, then adding it to result
; r5: frame pointer
; r6: stack pointer
; r7: stores pc for subroutines

and r0, r0, x0
and r1, r1, x0
and r5, r5, x0
and r6, r6, x0
and r7, r7, x0

; run the function f(x) = f(x-1) - 2x,
;  where x is an integer >= 0 from memory
main
    ; load x into r0
    ld r0, X
    
    ; r5 : frame pointer
    ; r6 : stack pointer
    ld r5, stack
    ld r6, stack
    
    jsr recur
    
    halt

recur
    ; push r7 onto stack
    add r6, r6, #-1
    str r7, r6, #0
    
    ; clear r1
    and r1, r1, x0
    
    ; if r0 != 0
    ;  recurse more
    ; else if r0 == 0
    ;  return 0
    add r0, r0, #0
    brz base ; if r0 == 0, reached base case
    ; otherwise, r0 > 0, continue with recursion
    
    ; recurse on f(x-1), then -2*r0 from result
    add r0, r0, #-1
    jsr recur
    ; increase x to return to this level of recursion
    ; could probably use stack instead of increasing and decreasing x, but this way is simpler
    add r0, r0, #1
    add r1, r0, r0 ; place 2X into r0
    ; find 2's comp to get -2X
    not r1, r1
    add r1, r1, #1
    
    ; calc f(x-1) -2x
    ld r2, result
    add r2, r2, r1
    
    ; store in result
    st r2, result

base
    ; not popping r7 properly
    ldr r7, r6, #0
    add r6, r6, #1
    ret

result  .fill x0
stack   .fill x4000
X   .fill #10

.end