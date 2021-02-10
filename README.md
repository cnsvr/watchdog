# WatchDog

## Introduction 

In your professional lives as computer engineers,  you will often develop grand projects com-posed  of  several  processes  interacting  to  achieve  a  complex  task  in  a  collaborative  manner.However, some of these processes might fail in production (i.e.  when deployed in the real-lifeenvironment). Therefore,  such  grand  software  projects  typically  also  embody  an  additional watchdog process  that  keeps  track  of  the  ”well  being”  of  all  other  processes.
  
The  watchdog process detects the crashing of the project components, logs the error, and then restarts thatprocess (or all processes of the project, if necessary).  In this manner, it is possible to deploy animportant projectunattended, i.e.  without a human administrator keeping track of the properexecution of the software 7x24.  (Typically,  we also set up a connected warning system,  likeNagios, to inform the human administrator about the failure so that (s)he can analyze the realcause, but that is beyond the scope of this project.)

In this project, you are expected to create a simple program inC/C+ + that implementsa watchdog system.  The project consists of two parts; process definition and process control.The process definition part specifies the process-specific information such as process id, processname, and signals to be handled by each process.  The process control part creates processesand tries to keep these processes alive so that the system continues to run smoothly.

The  project  will  be  evaluated  automatically  in  the  Linux  environment  (Ubuntu  Version20.04.1) with a gcc/g++ compiler (Version 9.3.0).  Please follow all the requirements specifiedbelow.  Your submissions will be compiled and tested via automatic scripts.  Therefore,  it iscrucial that you follow the protocol (i.e.  the rules and the specifications) defined in the projectdocument.  Failure in the compilation and/or execution is your responsibility.  You should usethe file names, parameters, etc.  as mentioned in the project specifications.

## Project Description

The project is to be designed in a flexible manner.  That means,  you will haveNprocessesand a watchdog,  but thoseNprocesses will be created by running the same executable file,process.c/process.cpp.  During the evaluation, we may takeNto be 3 or maybe even 120.  So,do not develop your code depending on a specific value ofN.

You are expected to have three C/C++ programs;process,watchdog, andexecutorwith theproperties specified below.  Note that,executorprogram is provided to you, and you should notalter anything on the original program.

* ##  executor.cpp

  - File Properties:
    * The  name  of  the  file  isexecutor.cpp.   (This  file  is  already  provided  to  you.   Do  not modify.)
  - Operations:
    * Program reads instructions frominstructions.txt.
    * Creates a named pipe to be shared withwatchdogprogram.
    * Reads “#PID” tuples from the read end of the pipe (P1 3564, P2 3565 etc.).
    * Detects whether it is asignalorwaitcommand.
    * Sends the intended signal to the given process.
    * When  instructions  are  completed,  first  it  kills  the  watchdog  process,  then  all  of  theother running processes by sending the SIGTERM signal.
    
* ##  process.c/process.cpp
  
  - File Properties:
    * The name of the file must be process.c/process.cpp.
  - Process definition:
    * Assume you are asked to create a system with N processes. Name the processes as P1,P2,P3,P4, ... ,PN.
    * All processes should work concurrently.
    * Process P1 should be considered as the head process (not parent).
    * For the sake of the scenario, we assume the head is such a critical process that, if it fails, all other processes should be terminated, P 1 be re-created, and then all remaining processes P2,P3, ..., and PN should be created following P1.
  - Operations:
    * Each process is forked from the watchdog process (which is described in the following sections).
    * Each process should display a message (as explained below) indicating that it has started and then go to sleep until it receives a signal.
    * Each process must handle the signals given in Table 1.
    * Processes handle signals by only printing the value of the corresponding signal.
    * Specific to SIGTERM signal, the process should terminate after printing the required message.
    * The instructions file has the signals and wait commands listed line by line. A sample instruction file is shown in Table 2. For instance, if the instruction is ”SIGHUP P3”, P3 receives a SIGHUP signal. If the instruction is ”wait 1”, all the processes should sleep for 1 second.

| Signal      | Value | Description |
| :-----------: | :-----------: | --------- |
| SIGHUP | 1 | Hangup (POSIX) <br/> Report that the user’s terminal is disconnected.|
| SIGINT | 2 | Interrupt (ANSI) <br/>  Program interrupt. (ctrl-c) |
| SIGILL | 4 | Illegal Instruction (ANSI) <br/>  Generally indicates that the executable file is corrupted. |
| SIGTRAP | 5 |  Trace trap (POSIX) |
|SIGFPE | 8 | Floating-Point arithmetic Exception (ANSI) <br/>  This includes division by zero and overflow. |
| SIGSEGV | 11 | Segmentation Violation (ANSI) <br/>  Indicates an invalid access to valid memory.|
| SIGTERM |  15 |   Termination (ANSI)|
| SIGXCPU | 24 |CPU limit exceeded.|


* ## watchdog.c/watchdog.cpp

  - File Properties:
    * The name of the file must be watchdog.c/watchdog.cpp.
    * Main method should be in the watchdog file.
  - Operations:
    * The watchdog program opens the named pipe.
    * The watchdog process writes its ID to the named pipe as P0 PID.
    * The watchdog should be the parent process, so each process should be created using fork() from the watchdog.
    *  The watchdog process writes the ID of each newly forked processes to the named pipe as P# PID.
    * The watchdog process should initiate each process from the process executable using exec().
    * If num of process = 10, your program should create 10 distinct processes (in addition to the watchdog)..
    * The number of processes in the program should be preserved, and be equal to the num of process all the time.
    * Watchdog sleeps until a process terminates.
    * If a process terminates, watchdog should detect and restart it. Do not forget to send P# PID tuple for the restarted process since the PID value has changed.
    * If the head process receives SIGTERM signal, all of the other processes should also be killed and restarted by the watchdog. The processes should be terminated in increasing order of the process numbers and this order should be visible in the output.
    * Watchdog kills processes via the SIGTERM signal.
    * The whole project should be terminated in a safe state. That is, executor kills all the child processes and the watchdog when it reaches the end of instructions.txt file.
    * Bonus Case: What happens if the watchdog dies? (You do not have to handle this case.)


## Input & Output

Your code must read the number of processes, the path of the input and output files from the command line. We will run your code as follows (Make sure that your final submission compiles and runs with these commands):

```bash
// Run the executor.cpp in background ./executor 5 instruction_path &
// For C++:
g++ process.cpp -std=c++14 -o process
g++ watchdog.cpp -std=c++14 -o watchdog ./watchdog 5 process_output watchdog_output
// For C:
gcc process.c -o process
gcc watchdog.c -o watchdog
./watchdog 10 process_output watchdog_output
```

* ## Input

  - Main method should be in the watchdog file and and should expect 3 command line argu- ments:
    * num of process: an integer value to specify the number of processes in the system 
    * process output path: absolute path of the process program’s output file
    * watchdog output path: absolute path of the watchdog program’s output file
  - The executor program takes 2 command line arguments:
    * num of process: an integer value to specify the number of processes in the system
    * instruction path: absolute path of the instructions file
    
 * ## Output
 
  - All operations performed by the process file should be logged to the file specified by the command line argument process output path.
    * When a process is ready to execute, it prints: PID is waiting for a signal
    * When a process receives a signal, it prints: P<ID> received signal <VALUE>
    * When a process receives SIGTERM signal, it prints: P<ID> is received signal 15, terminating gracefully
    
  - All operations performed by the watchdog should be logged to the file specified by the command line argument watchdog output path.
  * When watchdog creates a process, it prints: P<ID> is started and it has a pid of <PID VALUE>.
  * When the head process is killed, watchdog prints: P1 is killed, all processes must be killed.
  * When restarting all processes, watchdog prints: Restarting all processes.
  * When a process (except P1) is killed, watchdog prints: P<ID> is killed. And when a
process is being restarted/recreated, watchdog prints: Restarting P<ID>.
  * When the watchdog terminates normally (i.e., all the instructions are completed), it prints: Watchdog is terminating gracefully
 
