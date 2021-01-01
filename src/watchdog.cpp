/**
 * @mainpage
 * @author Furkan Cansever
 *
 *
 * \brief The main objective of this project is to
 *        implement a watchdog system that manages and controls
 *        processes in an efficient and convenience way.
 *
 * This programs follows the below steps.
 * It takes three arguments that are process number(N), path of procees and watchog output file.
 * Then, it create n child processes and
 * write its own process id and ids of child processes to
 * name pipe.At the same time, 'executor' executable file runs
 * at background and read all process id and
 * gives some instructions(sending signals to processes)
 * from 'instructions.txt' file. Then, according to this signal,
 * each process handle these signals in their own process and printing
 * some information about signal. At the same time, both Watchdog process and
 * its child processes write all operations to the output files.After instructions
 * finished, watchdog process waits until its child processes was killed. Then,
 * it kills itself with calling exit() function.
 *
 * In conclusion, this program manages its child processes in an efficient way,
 * and doesn't allow any child remain as zombie process.It also knows that
 * when head process is killed or terminated, kills all processes in a proper way,
 * and recreate all processes from scratch.With this management system,
 * all processes runs in a convenience way to handle their tasks.
 *
 */

#include "unistd.h"
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <csignal>
#include <map>
#include <string>
#include <fstream>



using namespace std;

struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; //0.3 sec
int processNum;         /// number of process
int fd;                 /// file descriptor for pipe communication
string processOut,watchdogOut;      /// path file for process and watchdog outputs
ofstream pOut,wOut;                 /// output stream for writing  to process and watchdog file


/**
 * Write WatchDog PID to Pipe.
 *
 * @param watchDogPID value for process id of WatchDog
 */
void writeWatchDogProcessToPipe(int watchDogPID){

    /**
     *  Open the named pipe(FIFO) under /tmp/myfifo directory.
     *  Write PID of its own process to the pipe.
     */

    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    char temp[30];
    fd = open(myfifo, O_WRONLY);
    string message = "P" + to_string(0) + " " + to_string(watchDogPID);
    strcpy(temp,message.c_str());
    write(fd,temp,strlen(temp) + 1);

}

/**
 *
 * Write PIDs of newly forked from Watchdog process to the pipe.
 * Execute 'process' executable file by giving process name and file path.
 * Write name and id of process to the watchdog file path.
 *
 * @param n number of process will be child process.
 * @param pidList array that keeps PIDs of processes.
 * @param pidListMap map that keeps name and PIDs of processes
 * @return id of newly created process and Watchdog process.
 */

int createManyChildProcess(int n, int *pidList, map<int,string> &pidListMap){

    string process;
    char temp[30];
    pid_t child;    /// id of child process

    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    fd = open(myfifo, O_WRONLY);


    for (int i = 1; i <= n; ++i) {
        child = fork();
        nanosleep(&delta,&delta);
        if (child == 0) {
            execlp("./process", "./process", to_string(i).c_str(), processOut.c_str(), NULL);
            break;
        } else if (child > 0) {   // Parent Process
            pidList[i] = child;
            process = "P" + to_string(i) + " " + to_string(child);
            strcpy(temp,process.c_str());
            write(fd, temp, strlen(temp) + 1);
            pidListMap[child] = "P" + to_string(i);
            wOut << "P" + to_string(i) + " is started and it has a pid of " + to_string(child) + "\n";
        }
    }

    close(fd);

    return child;
}

/**
 * Kill Watchdog process if SIGTERM signal comes to Watchdog process.
 * Wait until its all children killed
 *
 * @param signum value of signal
 */

void signalHandler( int signum ) {
    if(signum == 15) {
        wOut << "Watchdog is terminating gracefully\n";
        while (wait(NULL) > 0);
        wOut.close();
        close(fd);
        exit(signum);
    }
}

/**
 * Call writeWatchDogProcessToPipe and createManyChildProcess methods.
 * Wait until any child process is terminated.
 *
 * @param argc number of arguments
 * @param argv array contains arguments
 * @return 0 when it is terminated.
 */


int main(int argc, char *argv[]){

    processNum = stoi(argv[1]);                  /// number of process
    processOut = argv[2];                           /// path of process output file
    watchdogOut = argv[3];                         /// path of watchdog output file

    wOut.open(watchdogOut,wOut.out | wOut.app);


    pid_t pidList[processNum + 1];                 /// array keeps value of child processes
    map<int,string>pidListMap;                     /// map contains name and id of processes as key and value ( ex. pidListMap[23424] = "P1" )
    pidList[0] = getpid();
    pidListMap[pidList[0]] = "P0";
    pid_t child,wpid;                             /// id of processes


    char * myfifo = (char*) "/tmp/myfifo";      ///  FIFO file path

    mkfifo(myfifo, 0666);
    char temp[30];
    fd = open(myfifo, O_WRONLY);
    string process;

    signal(SIGTERM, signalHandler);

    writeWatchDogProcessToPipe(pidList[0]);

    child = createManyChildProcess(processNum, pidList,pidListMap);


    if (child > 0){
        while(1) {
            wpid = wait(NULL);
            if ((pidListMap.find(wpid) != pidListMap.end()) ) {
                if (pidListMap[wpid] == "P1") {
                    wOut << "P1 is killed, all processes must be killed\nRestarting all processes\n";
                    pidListMap.erase(pidList[1]);
                    for (int i = 2; i <= 5; ++i) {
                        kill(pidList[i], SIGTERM);
                        pidListMap.erase(pidList[i]);
                        sleep(1);
                    }
                    fd = open(myfifo, O_WRONLY);
                    child = createManyChildProcess(processNum, pidList, pidListMap);
                    if (child == 0) {
                        break;
                    }

                } else {
                    fd = open(myfifo, O_WRONLY);
                    pid_t newChild = fork();
                    string old_child = pidListMap[wpid].c_str();
                    wOut << old_child + " is killed\nRestarting " + old_child + "\n";
                    int p_index = stoi(old_child.substr(1));
                    pidListMap.erase(wpid);
                    if (newChild == 0) {
                        execlp("./process", "./process", to_string(p_index).c_str(),processOut.c_str(), NULL);
                        break;
                    } else if (newChild > 0) {   // Parent Process
                        pidList[p_index] = newChild;
                        process = old_child + " " + to_string(newChild);
                        strcpy(temp, process.c_str());
                        write(fd, temp, strlen(temp) + 1);
                        pidListMap[newChild] = old_child;
                        close(fd);
                        wOut << "P" + to_string(p_index) + " is started and it has a pid of " + to_string(newChild) + "\n";
                    }
                }

            }

        }
    }

    return 0;
}
