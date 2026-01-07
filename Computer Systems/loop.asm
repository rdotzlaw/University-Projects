            .orig x3000
            ; clear registers
            ; probably an acessive use of registers lol
loop
            and r0, r0, x0
            and r2, r2, x0
            and r1, r1, x0
            and r3, r3, x0
            and r4, r4, x0
            
            ; print opening message
            lea r0, str1
            puts
            
            add r2, r2, #7 ; #7 = b0000 0111, basically, and with it to leave last 3 bits untouched
            add r3, r3, #-5 ; #-5 + b101 = 0
            
            getc ; char into r1 and r4
            add r1, r1, r0
            add r4, r4, r0
           
            ; now check if char is = x04, if so, goto endloop
            add r4, r4, x-04
            brz endloop
            
            ; reset r4
            and r4, r4, x0
            add r4, r4, r0
            
            out ; print out char
            
            lea r0, nl
            puts ; new line
            
            and r1, r1, r2 ; clear all but last 3 bits of r1
            
            add r1, r1, r3 ; r3 = -5, if r1 = #5, then branch to if1
            brz if1
            
            ; doesn't end with b101, so continue
            
            ; and r4 with b011 (#3) to clear out 3rd bit,, then check to see if r1 is = b01 (#1)
            and r4, r4, #3
            add r4, r4, #-1
            brz if2
            
            ; neither case, so just go to endif
            br loop
            
if1         ; if char ends with 101
            lea r0, str4
            puts
            br loop
            
if2         ; if char doesn't end in 101, but does end in 01
            lea r0, str5
            puts
            br loop
endloop            
            ; end message
            lea r0, nl
            puts
            lea r0, str2
            puts
            lea r0, str3
            puts
            
            halt
            
nl          .stringz "\n"
str1        .stringz "Enter any charater: "
str2        .stringz "Programmed by Ryan Dotzlaw.\n"
str3        .stringz "End of processing.\n"
str4        .stringz "The character ends with 101.\n" ; could probably condense str4 and str5 into one string, but too much effort
str5        .stringz "The character ends with 01.\n"
.end