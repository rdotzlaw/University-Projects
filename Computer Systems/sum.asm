    .orig x3000
    
        ; Clear registers
    and r3, r3, x0
    and r1, r1, x0
    
        ; Add x3 to r1, place result in r1
    add r1, r1, x3 
    and r2, r2, x0 ; clear r2
    add r2, r2, xA ; adds 10 (base 10) to r2
    add r3, r1, r2 ; adds r2 + r1 = 13 (xD) places in r3
    
    halt
    .end