//
// Created by Furkan Cansever on 26.12.2020.
//

#include "unistd.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <csignal>
#include <map>
#include <string>
#include <fstream>



using namespace std;


struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; //0.3 sec
int processNum;
int fd;
string processOut,watchdogOut;
ofstream pOut,wOut;


void writeWatchDogProcessToPipe(int watchDogPID){
    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    char temp[30];
    fd = open(myfifo, O_WRONLY);
    string message = "P" + to_string(0) + " " + to_string(watchDogPID);
    strcpy(temp,message.c_str());
    write(fd,temp,strlen(temp) + 1);

}

int createManyChildProcess(int n, int *pidList, map<int,string> &pidListMap){

    string process;
    char temp[30];
    pid_t child;

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

void signalHandler( int signum ) {
    if(signum == 15) {
        wOut << "Watchdog is terminating gracefully\n";
        while (wait(NULL) > 0);
        wOut.close();
        close(fd);
        exit(signum);
    }
}


int main(int argc, char *argv[]){

    processNum = stoi(argv[1]);
    processOut = argv[2];
    watchdogOut = argv[3];

    wOut.open(watchdogOut,wOut.out | wOut.app);


    pid_t pidList[6];
    map<int,string>pidListMap;
    pidList[0] = getpid();
    pidListMap[pidList[0]] = "P0";
    pid_t child,wpid;
    // FIFO file path
    char * myfifo = (char*) "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);
    char temp[30];
    fd = open(myfifo, O_WRONLY);
    string process;

    signal(SIGTERM, signalHandler);

    writeWatchDogProcessToPipe(pidList[0]);

    child = createManyChildProcess(processNum, pidList,pidListMap);

    sleep(3);

    if (child > 0){
        while(1) {
            wpid = wait(NULL);
            printf("PID %d terminated\n", wpid);
            if ((pidListMap.find(wpid) != pidListMap.end()) ) {
                if (pidListMap[wpid] == "P1") {
                    wOut << "P1 is killed, all process must be killed\nRestarting all processes\n";
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
