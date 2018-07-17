#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {

    const size_t WRITE_SIZE = 25;
    char buffer[WRITE_SIZE + 1];

    //fopen returns FILE*, open returns file descriptors.
    //int fd = open( "pipe.txt", O_WRONLY | O_TRUNC| O_CREAT, S_IWGRP );
    FILE* out = fopen( "pipe.txt", "w");
    int fd[2];
    pipe( fd );

    pid_t pid_a, pid_b;
    pid_a = fork();

    switch (pid_a) {

        case -1:

            // This is a fail state
            perror( "Error forking child a: ");
            exit( EXIT_FAILURE );

        case 0:

            //child process always returns 0 from a fork() call
            //child_a

            //child should close input.
            close( fd[0] );
            write( fd[1], "B is writing to the pipe\n", WRITE_SIZE );
            break;

        default:

            //Now spawn a second child only in the parent code
            pid_b = fork();

            switch (pid_b) {

                case -1:

                    perror( "Error on second fork: ");
                    exit( EXIT_FAILURE );

                case 0:
                    //code for child b
                    close( fd[0] );
                    write( fd[1], "C is writing to the pipe\n", WRITE_SIZE );
                    break;

                default:
                    //code for parent
                    wait(&pid_a);
                    wait(&pid_b);

                    //null terminate the buffer so that file functions behave
                    write( fd[1], "\0", 1 );
                    close( fd[1] );
                    read( fd[0], buffer, sizeof(buffer) );
                    fwrite ( "A is writing to the file\n", 1, 25, out );

                    //Reminder, "buffer" decays into a pointer to its first element.
                    // `buffer` is the same as referencing `&buffer[0]'
                    fwrite( buffer, 1, sizeof(buffer) - 1, out );
                    break;

            }

            break;

    }

    return 0;
}




