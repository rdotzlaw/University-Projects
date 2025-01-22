# CPU Task Management Simulation and Analysis

#### Ryan Dotzlaw


## Compilation and Execution

Compile the program with:

        make

Run the program with:

        ./main <# of CPUs> <length of S> <file to read from>

Run the program in debug mode (you get to see the size of the mlfq in real-time) with:

        ./main <# of CPUs> <length of S> <file to read from> <literally anything to make argc == 5>

## Locks

Each queue has a lock associated with it for use with the push/pop methods

This means that there are the three minimal locks:

* One for reader thread adding to ready queue
* One for CPU getting a task from the dispatcher
* One for CPU writing tasks to the "Done" area

## MLFQ Rules:

* RULE 1: Run the task with the highest priority first
* RULE 2: Run tasks with the same priority in a round-robin (pop off queue, then push onto back)
* RULE 3: New tasks enter the system with the highest priority
* RULE 4: Once a task runs for a full #TIME_ALLOT, lower it's priority by one
* RULE 5: After the time period: 'slice', raise all tasks to the highest priority

## Report and Analysis

I would like to note that I'm pretty sure I'm somehow converting the time units wrong somewhere, 
if the results in the assignment description are in the ballpark of what I should be getting.

But the fact that the conversion is consistent for all the results, means that I'm still able to draw conclusions using them.

Or my code is terribly inefficient, leading to *much* higher turnaround times.

All times are measured in usec (probably).

|                  | Type 0 Turnaround | Type 1 Turnaround | Type 2 Turnaround | Type 3 Turnaround | Type 0 Response | Type 1 Response | Type 2 Response | Type 3 Response |
|------------------|-------------------|-------------------|-------------------|-------------------|-----------------|-----------------|-----------------|-----------------|
| 1 CPU, S = 1000  | 364868            | 1051282           | 3726041           | 2951351           | 9933            | 9254            | 10486           | 9539            |
| 2 CPU, S = 1000  | 180181            | 514483            | 1870043           | 1506989           | 4236            | 3918            | 4351            | 3923            |
| 4 CPU, S = 1000  | 87828             | 254390            | 902012            | 758522            | 2020            | 1857            | 2035            | 1827            |
| 8 CPU, S = 1000  | 40381             | 119077            | 408602            | 383579            | 699             | 578             | 706             | 661             |
| 1 CPU, S = 2000  | 379228            | 1087589           | 3784067           | 3003683           | 9388            | 8919            | 10039           | 9099            |
| 2 CPU, S = 2000  | 177305            | 518746            | 1862046           | 1499762           | 3731            | 3506            | 3906            | 3509            |
| 4 CPU, S = 2000  | 85035             | 250702            | 884197            | 747258            | 1644            | 1447            | 1671            | 1503            |
| 8 CPU, S = 2000  | 41387             | 121682            | 421185            | 392349            | 912             | 747             | 891             | 906             |
| 1 CPU, S = 5000  | 368819            | 1066110           | 3728472           | 2957665           | 10062           | 9376            | 10930           | 9605            |
| 2 CPU, S = 5000  | 180673            | 528452            | 1846417           | 1503006           | 3340            | 3105            | 3546            | 3166            |
| 4 CPU, S = 5000  | 82417             | 246683            | 884841            | 751368            | 1467            | 1328            | 1530            | 1347            |
| 8 CPU, S = 5000  | 40421             | 119077            | 418325            | 391151            | 624             | 508             | 620             | 589             |
| 1 CPU, S = 10000 | 369093            | 1071013           | 3765970           | 2988050           | 10007           | 9307            | 10898           | 9610            |
| 2 CPU, S = 10000 | 173312            | 522201            | 1858445           | 1508788           | 4320            | 4070            | 4504            | 4015            |
| 4 CPU, S = 10000 | 86430             | 251573            | 888727            | 749033            | 1741            | 1548            | 1799            | 1582            |
| 8 CPU, S = 10000 | 41259             | 121413            | 422731            | 392454            | 613             | 509             | 606             | 584             |

### Is the difference in turnaround time and response time you expected to see as S and the number of CPUs change? Why or why not?

#### Turnaround Time
The difference for turnaround time was mostly as expected. 

As the number of CPUs doubled, the turnaround time was cut (roughly) in half for each type of task.
This makes sense since the double the CPU's means double the concurrent tasks, which in turn cuts the time a task is in the system in half.

As S increased, the turnaround time didn't change in ways that can't be explained by pointing to the doubling of the CPU count, which wasn't expected.
I was expecting that longer tasks would have a longer turnaround time as S got bigger, due to the longer tasks being starved of the CPU.
This was not the case, however, and it seems as though all the changes in turnaround time could be explained by the increase in CPU count.

#### Response Time
The difference for response time was as expected.

As the number of CPUs doubled, the response time was cut in half as well.
This makes sense because double the CPUs means there are twice as many CPUs that have a chance to be available to take a task.

As S increased, the response time mostly stayed the same up until a certain point.
When running the program with 8 CPUs and an S of 5000, the response time is cut down from the previous test by a factor of ~1/3 (going from ~>1500 to ~550).
This most likely happens because with a high enough S, and enough concurrent CPUs, when a task enters the system the highest priority queue will be mostly empty.
This is because S is high enough that a priority boost happens relatively infrequently, 
and there are enough CPUs running concurrently that the tasks in the system have had their priority reduced. These factors combined lead to the larger response time reduction.

### How does adjusting the S value in the system affect the turnaround time or response time for long-running tasks? Does it appear to be highly correlated?

I know that a lower S value helps prevent long-running tasks from starving, however my S values are all too small, or something, since there was little to no amounts of
correlation between the S value and the turnaround/response time for long-running tasks (Type 2 tasks).

The exception being the same as mentioned in the <b>Response Time</b> section, where 8 CPUs and an S value of 5000 showed a faster response time than the expected based on previous tests.

