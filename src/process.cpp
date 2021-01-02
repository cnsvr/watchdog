#include <unistd.h>
#include <iostream>
#include <string>
#include <csignal>
#include <fstream>

using namespace std;

int pid;               /// process id
string processOut;     /// path of process output file
ofstream pOut;         /// output stream for output file


/**
 * Kill process if SIGTERM signal comes, otherwise write operation to output file
 *
 * @param signum value of signal
 */

void signalHandler( int signum ) {
    pOut.open(processOut,pOut.out | pOut.app);
    if(signum == 15) {
        pOut << "P" + to_string(pid) + " received signal 15, terminating gracefully\n";
        pOut.close();
        exit(signum);
    }else {
        pOut << "P" + to_string(pid) + " received signal " + to_string(signum) + "\n";
        pOut.close();
    }
}

/**
 * Open output file and write operation to file.
 * Call signalHandler method if any signal comes, otherwise sleeps.
 *
 * @param argc number of arguments
 * @param argv array contains arguments
 * @return 0 when it is terminated.
 */

int main (int argc, char *argv[]) {

    pid = stoi(argv[1]);        /// id of process
    processOut = argv[2];           /// path of output file

    pOut.open(processOut,pOut.out | pOut.app);
    pOut << "P" + to_string(pid) + " is waiting for a signal\n";
    pOut.close();
    signal(SIGHUP, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTRAP, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGXCPU, signalHandler);


    while(1) {
        sleep(1);
    }

    return 0;
}