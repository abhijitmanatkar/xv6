# Enhancing Xv6 - OS Assignment 5

## How to run:

Install `qemu`. Then run:
```shell
$ make clean
$ make qemu-nox SCHEDULER=[OPTION] 
```
Where possible options for `SCHEDULER` are `RR`(Round robin, also the default), `FCFS`(First-come-first-serve), `PBS`(Priority based scheduling), `MLFQ`(Multilevel feedback queue scheduling)

### Implementation details :-

## System call - `waitx`
- The `waitx` system call is similar to the default `wait` system call, but it also returns the waiting time and running time of the child process. 
- To implemement this, the `proc` structure defined in `proc.h` was modified and the fields `ctime`, `etime` and `rtime` representing creation time, exit time and runtime were added.
- `ctime` is set to the current value of `ticks` when a process is allocated using `allocproc`. `etime` and `rtime` are set to 0.
- At every timer interrupt, `rtime` for all running processes is incremented by 1 (in the `trap` function in `trap.c`).
- Finally, when a process calls `exit`, the `etime` is set to the current value of ticks.
- `waitx`, just like `wait`, scans the process table for zombie processes, and on finding one, frees the resources taken up by the process. The process' `rtime` is returned as it is and `wtime` is calculated as `wtime` = `etime` - `ctime` - `rtime`.

## User command - `time`
- `time` (defined in `time.c`) is similar to the time command found in most shells.
- The process forks into a parent and a child. The child process runs the program specified as the argument to `time` while the parent process waits for the child process to terminate using `waitx`. 
- When the child process finishes, `waitx` returns in the parent process and copies `rtime` and `wtime` to the specified pointers. The parent process then prints this information to the console.

## User command - `ps` and corresponding system call
- The `ps` user command is implemented using the `psx` system call in `proc.c`.
- `curr_wtime`, `n_run`, `cur_q` and `q[5]` variables are added to the `proc` structure.
- `curr_wtime` is the time that the process has been waiting since the last time it ran. It is updated for every runnable process at every timer interrupt.
- `n_run` is the number of times the processs has been picked by a scheduler. It is updated when the `sceduler` switches to the process.
- `cur_q` is used in case of MLFQ scheduling. It keeps track of which queue the process is currently in. `q[i]` counts the number of ticks the process was runningin a particular queue. 
- The `ps` command calls the `psx` system call (which is very similar to `procdump`) and prints out the required information for all processes.

## FCFS Scheduling
- For FCFS scheduling, the scheduler selects a runnable process with the lowest `ctime`.
- Unlike Round Robin, scheduling need not happen at every tmer interrupt, so calling of `yield` at every timer interrupt is disabled for FCFS.
- Hence, every process runs till it voluntarily gives up the CPU (by sleeping) or exits.

## Priority Based Scheduling
- A `priority` variable is added to the `proc` structure. It ranges from 0 to 100. It is initialized to 60 when the process is allocated.
- Whenever the scheduler is called, it picks a runnable process with the highes priority (i.e. lowest priority value).
- The `set_priority` system call(defined in `proc.c`), accesssible via the `setPriority` user program (defined in `setPriority.c`), searches the process table for a process with the given pid. If found, it sets the `priority` of that process to the new priority. If the new priority is higher than the old priority, `yield` is called and the cpu enters the scheduler. 


## Multilevel Feedback Queue Scheduling

### Queue implementation
- A doubly linked list is used to implement the queues. `QUEUES[5]` is the array of pointers to the heads of each of the 5 queues.
- As memory cannot be dynamically allocated for each node at runtime, `NODES[128]` is an array of nodes which can be assigned and deassigned for a process as required. 
- Functions for pushing to a queue, popping from a queue, deleting a particular element from a queue, assigning and deassigning a queue node are defined in `queue.c`.

### Scheduling
- Whenever a process is initialized, or changes its state to `RUNNABLE`, it is pushed to the end of the highest priority queue.
- The 5 queues, in decreasing order of priority, have time slices of 1,2,4,8 and 16. 
- Each time the scheduler is called, 
- `curr_rtime` is added to the `proc` structure to keep track of how long a process has been running in a queue.
- Whenever the scheduler is called, it first ages the processes. After that it picks the first runnable process from the highest priority non-empty priority queue. `curr_rtime` is reset for the process. The process is popped from it's queue.
- At every timer interrupt, `curr_rtime` is incremented for the process.
- The `proc` staructure is also added with the variable `to_demote`, which is used to determine if the process needs to be demoted to a lower queue in case of using up its allowed time slic.
- After every timer interuupt, `curr_rtime` for the running process is compared with it's allowed time slice. If `curr_rtime` is greater, than `to_demote` is set to 1 and `yield` is called for rescheduling.
- Once back in the scheduler, if the process is still runnable, it is pushed to an appropriate queue. If `to_demote` is set to 1, then the process is pushed to a lower priority queue using `dem_proc`, otherwise it is pushed to its original queue.

### Aging
- To prevent starvation, aging is implemented. `curr_wtime` is used to determine whether a process should be aged or not.
- The process table is scanned for finding runnable processes that should be aged. The criteria is that the `curr_wtime` for the process must be greater than the aging limit(currently 10 ticks). If that is the case, then the process is moved to higher priority queue using `promote_proc`.

## Comparison of the scheduling policies

Running `benchmark.c` showed the following results for different schedulers:

    MLFQ:
        Running time - 2024 clock cycles
        Waiting time - 9 clock cycles.

    PBS (without different priorities):
        Running time - 2564 clock cycles
        Wating time - 7 clock cycles

    PBS (with different priorities):
        Running time - 2211 clock cycles
        Waiting time - 5 clock cycles

    FCFS:
        Running time - 2509 clock cycles
        Wating time - 7 clock cycles

    RR(Default):
        Running time - 2004 clock cycles
        Waiting time - 5 clock cycles

