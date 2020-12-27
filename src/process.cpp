#include <unistd.h>
#include <iostream>
#include <string>
#include <csignal>

using namespace std;

static int pid = 0;

void signalHandler( int signum ) {
    if(signum == 15) {
        cout << "P"<< pid << " received signal 15, terminating gracefully" << endl;
        exit(signum);
    }else {
        cout << "P" << pid <<" received signal " << signum << endl;
    }
}

int main (int argc, char *argv[]) {

    pid = stoi(argv[1]);
    // string outputPath = argv[2];
    // It will take process output file and process number
    // register signal SIGINT and signal handler
    signal(SIGHUP, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTRAP, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGXCPU, signalHandler);

    cout << "P"<< pid << " is waiting for signal" << endl;

    while(1) {
        // cout << "Going to sleep...." << endl;
        sleep(1);
    }

    return 0;
}