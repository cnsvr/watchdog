# WatchDog

## 1 Introduction 

In your professional lives as computer engineers,  you will often develop grand projects com-posed  of  several  processes  interacting  to  achieve  a  complex  task  in  a  collaborative  manner.However, some of these processes might fail in production (i.e.  when deployed in the real-lifeenvironment). Therefore,  such  grand  software  projects  typically  also  embody  an  additional watchdog process  that  keeps  track  of  the  ”well  being”  of  all  other  processes.
  
The  watchdog process detects the crashing of the project components, logs the error, and then restarts thatprocess (or all processes of the project, if necessary).  In this manner, it is possible to deploy animportant projectunattended, i.e.  without a human administrator keeping track of the properexecution of the software 7x24.  (Typically,  we also set up a connected warning system,  likeNagios, to inform the human administrator about the failure so that (s)he can analyze the realcause, but that is beyond the scope of this project.)

In this project, you are expected to create a simple program inC/C+ + that implementsa watchdog system.  The project consists of two parts; process definition and process control.The process definition part specifies the process-specific information such as process id, processname, and signals to be handled by each process.  The process control part creates processesand tries to keep these processes alive so that the system continues to run smoothly.

The  project  will  be  evaluated  automatically  in  the  Linux  environment  (Ubuntu  Version20.04.1) with a gcc/g++ compiler (Version 9.3.0).  Please follow all the requirements specifiedbelow.  Your submissions will be compiled and tested via automatic scripts.  Therefore,  it iscrucial that you follow the protocol (i.e.  the rules and the specifications) defined in the projectdocument.  Failure in the compilation and/or execution is your responsibility.  You should usethe file names, parameters, etc.  as mentioned in the project specifications.

## 2 Project Description

The project is to be designed in a flexible manner.  That means,  you will haveNprocessesand a watchdog,  but thoseNprocesses will be created by running the same executable file,process.c/process.cpp.  During the evaluation, we may takeNto be 3 or maybe even 120.  So,do not develop your code depending on a specific value ofN.

You are expected to have three C/C++ programs;process,watchdog, andexecutorwith theproperties specified below.  Note that,executorprogram is provided to you, and you should notalter anything on the original program.

* ## 2.1 executor.cpp

  - File Properties:
    * The  name  of  the  file  isexecutor.cpp.   (This  file  is  already  provided  to  you.   Do  not modify.)
  - Operations:
    * Program reads instructions frominstructions.txt.
    * Creates a named pipe to be shared withwatchdogprogram.
    * Reads “#PID” tuples from the read end of the pipe (P1 3564, P2 3565 etc.).
    * Detects whether it is asignalorwaitcommand.
    * Sends the intended signal to the given process.•When  instructions  are  completed,  first  it  kills  the  watchdog  process,  then  all  of  theother running processes by sending the SIGTERM signal.
