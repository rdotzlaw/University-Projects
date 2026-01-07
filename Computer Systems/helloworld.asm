    .orig x3000
    
    lea r0, mesg
    trap x22
    and r0, r0, x0
    halt
    
mesg    .stringz "Hello world.\n"
        .end