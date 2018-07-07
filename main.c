#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/wait.h>

int main() {

    FILE* out = fopen( "pipe.txt", "a");
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
            write(fd[1], "B", 2 );

            //DEBUG:

            perror("B branch has executed");

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

                    //DEBUG:

                    perror("C branch has executed");

                    //fprintf( out, "C");
                    break;

                default:
                    //code for parent
                    close( fd[1] );

                    //double wait to ensure both children terminate first?
                    wait(NULL);
                    wait(NULL);
                    read( fd[0], buffer, sizeof(buffer) );
                    fprintf( out, "A");

                    //DEBUG:

                    perror("A branch has executed");


                    fprintf( out, buffer);
                    break;
            }

            break;

    }

    return retValue;
}




