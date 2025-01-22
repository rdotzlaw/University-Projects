# Threads and Locking
#### Ryan Dotzlaw

## Compilation and Execution

Compile the binaries with:

        make

Remove the binaries with:
        
        make clean

All the programs can be run with the following commands:
        
        ./lock-maybe 
        ./spin-lock 
        ./atomic-lock 
        ./lock-really 
        ./mutex-lock 
        ./cond_var


### Atomic Variables Hypothesis
Do I expect the spin lock to work with an atomic lock variable?

Yes. The documentation provided on the atomic variables says that atomic data types are immune to data races,
which means that the locks should actually work properly, preventing multiple threads from trying to perform the '++' operation on the counter.
Since there isn't a case of multiple threads adding and setting the counter, the program, in theory should count correctly.

### System Calls
How do the system calls change between the three versions?

The system calls can be viewed in the following files:

        sc_maybe.txt for lock-maybe
        sc_atomic.txt for atomic-lock
        sc_really.txt for lock-really
        sc_mutex.txt for mutex-lock

The atomic-lock and lock-really both have an extra futex call near the end of execution. 
Also, lock-maybe's final write system call (writing to stdout) is different; since lock-maybe doesn't work, the output is wrong.


Does pthread_mutex_lock use any system calls?

It probably does (most likely futex), however, the system calls it does use are not any distinct ones that aren't used in the other three versions.

There's no overt "lock" or "pthread" system calls that would set the pthread_mutex_lock function apart from a self-made locking method.

### Performance
Compare the performance between the working spin lock and the one using pthread_mutex_lock

What do you predict?

I predict the mutex lock will be faster than the spin lock, since technically the spin lock isn't giving up it's time slice, just wasting it doing nothing.
This means that the mutex lock should waste less time just spinning, and actually get things done.

What are the results?

        > time ./lock-really
        Counter is 200000000
        4.176u 0.001s 0:05.09 81.9%     0+0k 0+0io 0pf+0w

        > time ./mutex-lock
        Counter is 200000000
        14.833u 8.455s 0:11.70 198.9%   0+0k 0+0io 0pf+0w

If I had to guess why the mutex-lock takes longer, I would say it's possible that the use of the pthread mutex lock leads to a lot more context switching between the threads.
Whereas, the spin-lock will just allow one thread to have longer continuous time on the CPU, leading to less time being used for context switching.


### Conditional Variables
In this design, we don’t use signals to notify that the data has been used, only that it is available. What may happen if lots of data is given quickly?

If data is given too quickly, before the next thread can use it, the data may be overwritten and lost
