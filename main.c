#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>

int main() {

    FILE* out = fopen( "pipe.txt", "w");
    char buffer[128];

    int fd[2];
    pipe( fd );

    //It might be easier to keep everything as a fd instead of outputting from the fd pipe into a file struct
    //or just do it for fun.
    //int fd = open( "pipe.txt", O_WRONLY | O_TRUNC| O_CREAT, S_IWGRP );

    pid_t pid_a, pid_b;
    int retValue = 0;

    pid_a = fork();

    switch (pid_a) {

        case -1:
            // This is a fail state
            perror( "Error forking child a: ");
            retValue = -1;
            break;

        case 0:
            //child process always returns 0 from a fork() call
            //child_a

            //child a should close input.
            close( fd[0] );
            write(fd[1], "C", 2 );
            //fprintf( out, "B" );
            break;

        default:
            //for the parent, which apparently has an arbitrary PID
            //printf("Parent PID is: %u\n", pid_a);

            //Now spawn a second child only in the parent code

            pid_b = fork();
            //Here's where it gets ugly.  I don't love the nestedness here
            switch (pid_b) {

                case -1:

                    perror( "Error on second fork: ");
                    retValue = -1;
                    break;

                case 0:
                    //code for child b
                    close( fd[0] );
                    write(fd[1], "C", 2 );
                    //fprintf( out, "C");
                    break;

                default:
                    //code for parent
                    close( fd[1] );

                    read( fd[0], buffer, sizeof(buffer) );
                    fprintf(out, buffer);
                    break;
            }

            //we shouldn't ever actually get here
            perror("Something has gone terribly wrong with your switches.  Unknown PID error: ");
            break;

    }

    return retValue;
}




