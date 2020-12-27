//
// Created by Furkan Cansever on 26.12.2020.
//

#include "unistd.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <map>



using namespace std;

struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; //0.3 sec


int main(int argc, char *argv[]){

    int fd;
    pid_t pidList[6];
    map<int,string>pidListMap;
    pidList[0] = getpid();
    pid_t child,wpid;
    // FIFO file path
    char * myfifo = (char*) "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);
    char temp[30];
    fd = open(myfifo, O_WRONLY);
    string process;

    for(int i=0;i<=5;i++)
    {

        child = fork();
        nanosleep(&delta,&delta);
        if (i == 0 && child > 0){
            process = "P" + to_string(i) + " " + to_string(getpid());
            strcpy(temp,process.c_str());
            write(fd, temp, strlen(temp) + 1);
        }
        else {
            if (child == 0) {
                printf("[son] pid %d from [parent] pid %d\n", getpid(), getppid());
                // string message = "P" + to_string(i) + " " + "is waiting for signal";
                // execl("/bin/echo","echo",message.c_str(),NULL);
                // execlp("pwd", "pwd", NULL);
                execlp("./process", "./process", to_string(i).c_str(), NULL);
                break;
                // exit(0);
            } else if (child > 0) {   // Parent Process
                // wpid = wait(NULL);
                // printf("%d\n",wpid);
                pidList[i] = child;
                process = "P" + to_string(i) + " " + to_string(child);
                strcpy(temp,process.c_str());
                write(fd, temp, strlen(temp) + 1);

                pidListMap[child] = "P" + to_string(i);

            }
        }
    }



    /*
    if(child > 0){
        while (1){
            wpid = wait(NULL);
            if (pidListMap.find(wpid) != pidListMap.end()){
                // found
                // if P1 killed, create all process

                if(pidListMap[wpid] != "P1") {

                }

                // else only create this process
            }else{
                // not found
            }
        }
    }
     */


    close(fd);

    return 0;
}
