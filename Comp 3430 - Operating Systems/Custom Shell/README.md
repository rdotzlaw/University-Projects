
## Marginal At Best Shell
#### Ryan Dotzlaw

## Compilation
#### Run *'make'* to compile all the binaries.

#### Run *'make clean'* to clean up all the compiled binaries.

## Execution
To execute the program, run:
    
    ./mbsh [input file name]

Where [input file name] is the name of the file containing all the shell commands.



## Code Layout
### Core Functions
#### Main
The main function is just a loop, grabbing a single line from the input file, and passing it to the parse function.
After the line has been executed, this function will send kill signals to all created processes, because some seem to stick around, even after successful execution.

#### Parse
This function takes in a single line from the input file and deconstructs it into an array of strings

#### Create_cmd
This function will take the array of strings created by *parse* and convert them into command struct instances and place them in an array of commands.
These command structs will contain the arguments needed to run the program, along with any file descriptors needed for piping or redirects.
This function will also change the order of execution for process substitution, executing the process to substitute before the process that is receiving the output.

#### Exec_cmd
Just loops through the array of commands and executes them using a created child process. Before executing, it takes the fd's for both input and output set in the command struct
and replaces stdin and stdout with them (if the fd's were set). It finally does some cleanup, freeing memory for the next line.

## Issues, Concerns, Etc.
The Marginal at Best Shell can do basic redirects, piping, and process substitution. So far, I have found that multiple pipes work, on occasion, depending on the program.

For instance, the following commands:

        ln -sf /usr/share/dict/words
        sort -R < words | head -25 | grep e
Works consistently with no issue, sending output to stdout as expected.

Whereas the commands:

        ln -sf /usr/share/dict/words
        sort -R < words | head -25 | sort -d
Does not work at all, failing to send any output to stdout.

I'm not sure what could cause this, looking at my own program's output shows that the file descriptors for all the functions are set and not -1 (default), 
which means that the exec_cmd function will replace stdin and stdout with them.

However, redirects from a second pipe don't work at all, so:

        ln -sf /usr/share/dict/words
        sort -R < words | head -25 | grep e > only-e.txt
Won't work, however, like the issue with piping, the file descriptors are set up properly and an output file is created, but it remains empty.
It is likely that both of these issues are connected, but I'm not sure what I'm doing wrong.