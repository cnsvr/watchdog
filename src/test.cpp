#include <stdio.h>
#include <unistd.h>

#include <cstdlib>

#include <fcntl.h>


int main(){
    int i;
    int n = 5;
    pid_t  pid;
    for (i = 0; i < n; ++i) {
        pid = fork();
        if (pid > 0) {   /* I am the parent, create more children */
            continue;
        } else if (pid == 0) { /* I am a child, get to work */
            printf("child num%d, parent num%d\n",getpid(),getppid());
            break;
        } else {
            printf("fork error\n");
            exit(1);
        }
    }
    return 0;
}