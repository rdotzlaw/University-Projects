## Makefile
#### Run *'make'* to compile all the binaries.

#### Run *'make clean'* to clean up all the compiled binaries.

## Thread and Process Management

After compiling, run with `./threads` or `./procs`

The program will read in from *'config.txt'* and use the value inside to determine how many worker processes are needed.

The handler ID will be printed out, so you can send signals to it through the command line.

Both programs create a handler, and several children processes/threads that will 'work' until they receive a signal to stop working

Sending the handler the `SIGINT` signal results in the handler telling all workers to clean up and exit and then the handler exiting itself

Sending the handler the `SIGHUP` signal results in the handler re-reading the config file and changing the number of running workers, either creating more, or removing some.