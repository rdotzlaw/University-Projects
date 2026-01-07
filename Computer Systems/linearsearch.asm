.orig x3000
; r5: frame pointer
; r6: stack pointer
; r7: stores pc for subroutines
; r0: i/o and general data movement
; r1: address of current target in targets array
; r2: count of remaining targets in targets array
; r3: used only in subroutine as the target value
; stack starts at x4000 and goes down in memory address
    
    ; loop through targets until all targets have been checked
    ;   each iteration in targets, loop through source and then print out valid msg
    ;       Not found if current target not found in source
    ;       Found at position: x if the target was found at pos x in source
    ; do this by creating a linear search subroutine that takes in values from the stack
    
    ; clear all used registers
    and r5, r5, x0
    and r6, r6, x0
    and r0, r0, x0
    and r1, r1, x0
    and r2, r2, x0
    and r3, r3, x0
    
   
    lea r1, TARGETS ; r1 is the address of the current target
    ld r2, NUMTARGETS ; r2 is the count of remaining targets
    
TLoop
    ; loop through targets and call the LSearch subroutine on each element
    ; push elements to be used by subroutine onto stack
    ;   Address if the array SOURCE
    ;   the value of N
    ;   the current target value
    ;   open spot reserved for return value
    
    
    ; reset stack each time the loop occurs
    ld r5, STACK ; frame ptr
    ld r6, STACK ; stack ptr
    
    ; push onto stack
    ; source address
    lea r0, SOURCE
    add r6, r6, #-1
    str r0, r6, #0
    ; value of N
    ld r0, N
    add r6, r6, #-1
    str r0, r6, #0
    ; target
    ldr r0, r1, #0 ; r1 is pointer to current target, r0 is now actual target
    add r6, r6, #-1
    str r0, r6, #0
    
    ; reserve spot for return
    add r6, r6, #-1
    
    ; set frame pointer to stack pointer here
    add r5, r5, #-4 ; 4 things put on stack (1 spot reserved), so just move by 4
    
    jsr LSearchStart
    
    ; take return value and decide what to print out
    
    ldr r0, r5, #0 ; r0 is now return value
    add r0, r0, #1 ; if -1
    brz NF
    br F
    
NF
    lea r0, NOTFOUNDMSG
    puts
    br TLoop2

F
    lea r0, FOUNDMSG
    puts
    ldr r0, r5, #0 ; r0 contains int value of return
    ; convert to ascii by adding ASCII value to r0
    ; r3 isn't important, so no need to put on stack during subroutine
    ld r3, ASCII
    add r0, r0, r3 ; convert int value into ascii for output
    out
    br TLoop2

TLoop2    
    
    ; operations in loop performed after LSearch subroutine
    ; increment Targets pointer
    ; decrement targets left
    add r1, r1, #1
    add r2, r2, #-1
    ; once remaining targets is 0, program is done
    brz Done
    br TLoop
    

Done
    lea r0, EOPMSG
    puts
    
    halt
    
; linear search subroutine
; will search through address popped off from the RTS pointing to array of length N (also on RTS)
; for an instance of target (also from RTS)
LSearchStart
    ; DO NOT MODIFY R7
    
    ; save register data
    ; r6 is stack pointer, dont touch
    ; r5 is frame pointer, dont touch
    
    add r6, r6, #-1
    str r1, r6, #0 ; r1 is pointer to targets array
    add r6, r6, #-1
    str r2, r6, #0 ; r2 is remaining values in targets array
    
    ; empty space reserved for return value on stack, clear it so its empty, then set to -1
    and r0, r0, #0
    add r0, r0, #-1
    str r0, r5, #0
    add r5, r5, #1
    ; now set r3 to pop (target)
    ldr r3, r5, #0
    add r5, r5, #1
    ; make r3 2's comp
    not r3, r3
    add r3, r3, #1
    
    ; now set r2 to pop (remaining vals in source)
    ldr r2, r5, #0
    ; store N into r4 for use with return
    ldr r4, r5, #0
    add r5, r5, #1
    ; now set r1 to pop (address of source)
    ldr r1, r5, #0
    add r5, r5, #1 ; r5 should now be equal to STACK (x4000)
    
    ; shift back to position for return value
    add r5, r5, #-4
    ; now just store return value into r5 when needed
    

    
    ; ** at this point r4 and r2 are equal
    ; r4 is N
    ; r3 is the target (2's comp)
    ; r2 is the remaining count
    ; r1 is the address of the current value
    
    
    br LSearchLoop
    
LSearchLoop
    ; go through every element in source to try and find an instance of the chosen target
    ; LOGIC PROBLEM HERE, NEVER FINDING ANY VALUES <-- this
    ; primary routine grabs -1 from stack, possibly this sets the return value in the wrong place
    ; or alternatively, the logic to jump to LSearchFound is flawed
    ldr r0, r1, #0 ; load current source value into r0
    add r0, r3, r0 ; add r0 and r3 (2's comp of target)
    brz LSearchFound
    
    add r1, r1, #1
    add r2, r2, #-1
    brz LSearchDone
    br LSearchLoop

LSearchFound

    ; return value is current index
    ; current index = r4 - r2 (n - vals left)
    not r2, r2
    add r2, r2, #1
    add r0, r4, r2
    
    ; store return into r5
    str r0, r5, #0

LSearchDone    

    
    ; reload register data in reverse order
    ldr r2, r6, #0
    add r6, r6, #1
    
    ldr r1, r6, #0
    add r6, r6, #1
    
    
    ret ; leave subroutine

    
STACK   .fill x4000 ; address of the start of the RTS
ASCII 	.fill 	x30
FOUNDMSG 	.stringz "\nFound at position: "
NOTFOUNDMSG 	.stringz "\nNot found."
EOPMSG 	.stringz "\nProgrammed by Ryan Dotzlaw\nEnd of Processing\n"
N 	.fill 	#10
SOURCE 	.fill 	#-33
	    .fill 	#99
        .fill 	#67
        .fill 	#0
        .fill 	#29
        .fill 	#-123
        .fill 	#17
        .fill 	#79
        .fill 	#-1
        .fill 	#22
NUMTARGETS 	.fill 	#4
TARGETS 	.fill 	#-33
            .fill 	#22
            .fill 	#29
            .fill 	#88

.end