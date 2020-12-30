#include <unistd.h>
#include <iostream>
#include <string>
#include <csignal>
#include <fstream>

using namespace std;

int pid;
string processOut;
ofstream pOut;

void signalHandler( int signum ) {
    if(signum == 15) {
        pOut << "P" + to_string(pid) + " received signal 15, terminating gracefully\n";
        pOut.close();
        exit(signum);
    }else {
        pOut << "P" + to_string(pid) + " received signal " + to_string(signum) + "\n";
    }
}

int main (int argc, char *argv[]) {

    pid = stoi(argv[1]);
    processOut = argv[2];

    pOut.open(processOut,pOut.out | pOut.app);

    signal(SIGHUP, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTRAP, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGXCPU, signalHandler);

    pOut << "P" + to_string(pid) + " is waiting for signal\n";

    while(1) {
        sleep(1);
    }

    return 0;
}